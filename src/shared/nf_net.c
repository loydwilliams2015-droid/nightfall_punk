#include "nf_net.h"
#include <enet/enet.h>
#include <string.h>

static uint32_t rng_next(NfNetHost *n){n->rng=n->rng*1664525u+1013904223u;return n->rng;}
static bool send_now(NfNetHost *net,void *peer,uint8_t channel,const uint8_t *data,size_t size,bool reliable){(void)net;if(peer==NULL||data==NULL||size==0||size>NF_NET_MAX_PACKET_BYTES)return false;ENetPacket *p=enet_packet_create(data,size,reliable?ENET_PACKET_FLAG_RELIABLE:0);if(p==NULL)return false;return enet_peer_send((ENetPeer*)peer,channel,p)==0;}
static void flush_due(NfNetHost *net){if(net==NULL||net->host==NULL)return;uint32_t now=nf_net_now_ms();for(size_t i=0;i<NF_NET_SIM_QUEUE;++i){NfQueuedPacket *q=&net->queue[i];if(q->used&&(int32_t)(now-q->due_ms)>=0){send_now(net,q->peer,q->channel,q->data,q->size,q->reliable);q->used=false;}}enet_host_flush((ENetHost*)net->host);}

bool nf_net_global_init(void){return enet_initialize()==0;}
void nf_net_global_shutdown(void){enet_deinitialize();}
bool nf_net_server_open(NfNetHost *net,uint16_t port,size_t max_peers){if(net==NULL)return false;memset(net,0,sizeof(*net));ENetAddress a={0};a.host=ENET_HOST_ANY;a.port=port;net->host=enet_host_create(&a,max_peers,NF_NET_CHANNEL_COUNT,0,0);net->server=true;net->rng=0x12345678u;return net->host!=NULL;}
bool nf_net_client_open(NfNetHost *net){if(net==NULL)return false;memset(net,0,sizeof(*net));net->host=enet_host_create(NULL,1,NF_NET_CHANNEL_COUNT,0,0);net->rng=0x87654321u;return net->host!=NULL;}
void nf_net_close(NfNetHost *net){if(net==NULL)return;if(net->host!=NULL)enet_host_destroy((ENetHost*)net->host);memset(net,0,sizeof(*net));}
void *nf_net_connect(NfNetHost *net,const char *host,uint16_t port){if(net==NULL||net->host==NULL||host==NULL)return NULL;ENetAddress a={0};if(enet_address_set_host(&a,host)!=0)return NULL;a.port=port;return enet_host_connect((ENetHost*)net->host,&a,NF_NET_CHANNEL_COUNT,0);}
void nf_net_disconnect(void *peer,uint32_t data){if(peer!=NULL)enet_peer_disconnect((ENetPeer*)peer,data);}
int nf_net_service(NfNetHost *net,NfNetEvent *out,uint32_t timeout_ms){if(net==NULL||net->host==NULL||out==NULL)return -1;flush_due(net);ENetEvent e;int rc=enet_host_service((ENetHost*)net->host,&e,timeout_ms);if(rc<=0){out->type=NF_NET_EVENT_NONE;return rc;}memset(out,0,sizeof(*out));out->peer=e.peer;switch(e.type){case ENET_EVENT_TYPE_CONNECT:out->type=NF_NET_EVENT_CONNECT;break;case ENET_EVENT_TYPE_DISCONNECT:out->type=NF_NET_EVENT_DISCONNECT;break;case ENET_EVENT_TYPE_RECEIVE:out->type=NF_NET_EVENT_RECEIVE;out->channel=e.channelID;out->size=e.packet->dataLength>NF_NET_MAX_PACKET_BYTES?NF_NET_MAX_PACKET_BYTES:e.packet->dataLength;memcpy(out->data,e.packet->data,out->size);enet_packet_destroy(e.packet);break;default:out->type=NF_NET_EVENT_NONE;break;}return 1;}
bool nf_net_send(NfNetHost *net,void *peer,uint8_t channel,const uint8_t *data,size_t size,bool reliable){if(net==NULL||net->host==NULL||peer==NULL||data==NULL||size==0||size>NF_NET_MAX_PACKET_BYTES)return false;if(!reliable&&net->sim_loss_percent>0.0f){float r=(float)(rng_next(net)%10000u)/100.0f;if(r<net->sim_loss_percent)return true;}uint32_t delay=net->sim_latency_ms;if(net->sim_jitter_ms>0){uint32_t span=net->sim_jitter_ms*2u+1u;int32_t j=(int32_t)(rng_next(net)%span)-(int32_t)net->sim_jitter_ms;int32_t d=(int32_t)delay+j;delay=d>0?(uint32_t)d:0u;}if(delay==0)return send_now(net,peer,channel,data,size,reliable);for(size_t i=0;i<NF_NET_SIM_QUEUE;++i){NfQueuedPacket *q=&net->queue[i];if(!q->used){q->used=true;q->peer=peer;q->due_ms=nf_net_now_ms()+delay;q->channel=channel;q->reliable=reliable;q->size=size;memcpy(q->data,data,size);return true;}}return false;}
void nf_net_flush(NfNetHost *net){flush_due(net);}
void nf_net_set_simulation(NfNetHost *net,uint32_t latency,uint32_t jitter,float loss){if(net==NULL)return;net->sim_latency_ms=latency;net->sim_jitter_ms=jitter;net->sim_loss_percent=loss<0?0:(loss>100?100:loss);}
uint32_t nf_net_now_ms(void){return enet_time_get();}
uint32_t nf_net_peer_rtt_ms(void *peer){return peer!=NULL?((ENetPeer*)peer)->roundTripTime:0u;}
