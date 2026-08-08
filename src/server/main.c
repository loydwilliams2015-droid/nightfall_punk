#include "nf_net.h"
#include "nf_prediction.h"
#include "nf_security.h"
#include "nf_world.h"

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NF_SERVER_CLIENTS NF_NET_MAX_PLAYERS

typedef struct NfServerClient {
    bool occupied;
    bool connected;
    void *peer;
    NfEntityId entity_id;
    uint8_t session_token[NF_NET_TOKEN_BYTES];
    uint32_t last_input_sequence;
    NfMoveInput current_input;
    uint32_t reserved_until_ms;
} NfServerClient;

static volatile sig_atomic_t g_running = 1;
static void on_signal(int sig){(void)sig;g_running=0;}
static void sleep_ms(unsigned ms){struct timespec ts={(time_t)(ms/1000u),(long)(ms%1000u)*1000000L};nanosleep(&ts,NULL);}
static bool seq_newer(uint32_t a,uint32_t b){return (int32_t)(a-b)>0;}
static bool input_valid(const NfMoveInput *m){return m!=NULL&&isfinite(m->forward)&&isfinite(m->strafe)&&isfinite(m->yaw_radians)&&fabsf(m->forward)<=1.01f&&fabsf(m->strafe)<=1.01f&&fabsf(m->yaw_radians)<100000.0f;}

static NfServerClient *by_peer(NfServerClient clients[],void *peer){for(size_t i=0;i<NF_SERVER_CLIENTS;++i)if(clients[i].occupied&&clients[i].connected&&clients[i].peer==peer)return &clients[i];return NULL;}
static NfServerClient *by_token(NfServerClient clients[],const uint8_t token[NF_NET_TOKEN_BYTES],uint32_t now){for(size_t i=0;i<NF_SERVER_CLIENTS;++i)if(clients[i].occupied&&!clients[i].connected&&(int32_t)(clients[i].reserved_until_ms-now)>0&&nf_security_token_equal(clients[i].session_token,token))return &clients[i];return NULL;}
static NfServerClient *free_slot(NfServerClient clients[]){for(size_t i=0;i<NF_SERVER_CLIENTS;++i)if(!clients[i].occupied)return &clients[i];return NULL;}
static NfVec3 spawn_for_slot(size_t slot){static const NfVec3 p[NF_SERVER_CLIENTS]={{0,0.05f,-18},{-2,0.05f,-18},{2,0.05f,-18},{4,0.05f,-18}};return p[slot%NF_SERVER_CLIENTS];}
static size_t slot_index(NfServerClient clients[],NfServerClient *c){return (size_t)(c-clients);}

static void expire_reservations(NfWorld *world,NfServerClient clients[],uint32_t now){for(size_t i=0;i<NF_SERVER_CLIENTS;++i){NfServerClient *c=&clients[i];if(c->occupied&&!c->connected&&(int32_t)(now-c->reserved_until_ms)>=0){nf_world_despawn_actor(world,c->entity_id);memset(c,0,sizeof(*c));}}}

static void send_reject(NfNetHost *net,void *peer,uint8_t reason){uint8_t buf[32];size_t n=nf_protocol_encode_reject(buf,sizeof(buf),reason);if(n)nf_net_send(net,peer,NF_NET_CHANNEL_RELIABLE,buf,n,true);}

static void handle_hello(NfNetHost *net,NfWorld *world,NfServerClient clients[],void *peer,const uint8_t *data,size_t size){NfHelloMessage hello;if(!nf_protocol_decode_hello(data,size,&hello)){send_reject(net,peer,1);return;}uint32_t now=nf_net_now_ms();NfServerClient *c=NULL;bool resumed=false;if(!nf_security_token_is_zero(hello.resume_token)){c=by_token(clients,hello.resume_token,now);resumed=c!=NULL;}if(c==NULL)c=free_slot(clients);if(c==NULL){send_reject(net,peer,2);return;}if(!resumed){size_t idx=slot_index(clients,c);memset(c,0,sizeof(*c));c->occupied=true;c->entity_id=nf_world_spawn_actor(world,NF_FACTION_PLAYER,spawn_for_slot(idx));uint8_t server_nonce[NF_NET_NONCE_BYTES];nf_security_random(server_nonce,sizeof(server_nonce));nf_security_derive_token(hello.client_nonce,server_nonce,c->session_token);}c->connected=true;c->peer=peer;c->reserved_until_ms=0;NfWelcomeMessage welcome={.entity_id=c->entity_id,.server_tick=world->tick,.world_seed=world->seed,.strong_crypto=nf_security_is_strong()};memcpy(welcome.session_token,c->session_token,NF_NET_TOKEN_BYTES);uint8_t buf[128];size_t n=nf_protocol_encode_welcome(buf,sizeof(buf),&welcome);nf_net_send(net,peer,NF_NET_CHANNEL_RELIABLE,buf,n,true);printf("[server] %s entity=%u clients=%zu\n",resumed?"resumed":"welcome",c->entity_id,nf_world_active_actor_count(world));}

static void handle_input(NfServerClient *c,const uint8_t *data,size_t size){NfInputBundle b;if(c==NULL||!nf_protocol_decode_input(data,size,&b)||!nf_security_token_equal(c->session_token,b.session_token))return;bool jump=false;for(uint8_t i=0;i<b.count;++i){NfInputCommand *cmd=&b.commands[i];if(!input_valid(&cmd->move)||!seq_newer(cmd->sequence,c->last_input_sequence))continue;jump=jump||cmd->move.jump_pressed;c->current_input=cmd->move;c->last_input_sequence=cmd->sequence;}if(jump)c->current_input.jump_pressed=true;}

static void send_snapshot(NfNetHost *net,const NfWorld *world,NfServerClient clients[],NfServerClient *recipient){NfSnapshotMessage s={.server_tick=world->tick,.acknowledged_input=recipient->last_input_sequence};for(size_t i=0;i<NF_SERVER_CLIENTS&&s.actor_count<NF_NET_MAX_SNAPSHOT_ACTORS;++i){if(!clients[i].occupied)continue;const NfActor *a=nf_world_find_actor_const(world,clients[i].entity_id);if(a!=NULL)nf_actor_to_net_state(a,&s.actors[s.actor_count++]);}uint8_t buf[NF_NET_MAX_PACKET_BYTES];size_t n=nf_protocol_encode_snapshot(buf,sizeof(buf),&s);if(n)nf_net_send(net,recipient->peer,NF_NET_CHANNEL_STATE,buf,n,false);}

int main(int argc,char **argv){
    uint16_t port=NF_NET_DEFAULT_PORT;uint32_t sim_latency=0,sim_jitter=0;float sim_loss=0.0f;double duration=0.0;
    for(int i=1;i<argc;++i){if(strcmp(argv[i],"--port")==0&&i+1<argc)port=(uint16_t)atoi(argv[++i]);else if(strcmp(argv[i],"--sim-latency")==0&&i+1<argc)sim_latency=(uint32_t)atoi(argv[++i]);else if(strcmp(argv[i],"--sim-jitter")==0&&i+1<argc)sim_jitter=(uint32_t)atoi(argv[++i]);else if(strcmp(argv[i],"--sim-loss")==0&&i+1<argc)sim_loss=(float)atof(argv[++i]);else if(strcmp(argv[i],"--duration")==0&&i+1<argc)duration=atof(argv[++i]);}
    signal(SIGINT,on_signal);signal(SIGTERM,on_signal);if(!nf_security_init()||!nf_net_global_init()){fprintf(stderr,"nightfall: network/security init failed\n");return 1;}NfNetHost net;if(!nf_net_server_open(&net,port,NF_SERVER_CLIENTS)){fprintf(stderr,"nightfall: could not open UDP port %u\n",port);nf_net_global_shutdown();return 1;}nf_net_set_simulation(&net,sim_latency,sim_jitter,sim_loss);
    NfWorld world;nf_world_init(&world,20260807u);nf_world_build_movement_lab(&world);NfServerClient clients[NF_SERVER_CLIENTS]={0};
    printf("nightfall!punk dedicated server v0.3\n");printf("port=%u tick=%u snapshot=%u max_players=%u crypto=%s sim=%ums +/- %ums %.1f%% loss\n",port,NF_TICK_RATE,NF_NET_SNAPSHOT_HZ,NF_NET_MAX_PLAYERS,nf_security_is_strong()?"libsodium":"scaffold",sim_latency,sim_jitter,sim_loss);
    const double fixed_ms=1000.0/(double)NF_TICK_RATE;uint32_t last=nf_net_now_ms(),start=last;double acc=0.0;
    while(g_running){NfNetEvent ev;while(nf_net_service(&net,&ev,0)>0){if(ev.type==NF_NET_EVENT_RECEIVE){NfMessageType t=nf_protocol_peek_type(ev.data,ev.size);NfServerClient *c=by_peer(clients,ev.peer);if(t==NF_MSG_HELLO)handle_hello(&net,&world,clients,ev.peer,ev.data,ev.size);else if(t==NF_MSG_INPUT)handle_input(c,ev.data,ev.size);else if(t==NF_MSG_PING&&c!=NULL){NfPingMessage p;if(nf_protocol_decode_ping(ev.data,ev.size,NF_MSG_PING,&p)){uint8_t b[32];size_t n=nf_protocol_encode_ping(b,sizeof(b),NF_MSG_PONG,&p);nf_net_send(&net,c->peer,NF_NET_CHANNEL_RELIABLE,b,n,true);}}}else if(ev.type==NF_NET_EVENT_DISCONNECT){NfServerClient *c=by_peer(clients,ev.peer);if(c!=NULL){c->connected=false;c->peer=NULL;c->current_input=(NfMoveInput){0};c->reserved_until_ms=nf_net_now_ms()+NF_NET_RECONNECT_WINDOW_MS;printf("[server] reserved entity=%u for reconnect\n",c->entity_id);}}}
        uint32_t now=nf_net_now_ms();uint32_t elapsed=now-last;last=now;if(elapsed>250u)elapsed=250u;acc+=(double)elapsed;expire_reservations(&world,clients,now);
        while(acc>=fixed_ms){for(size_t i=0;i<NF_SERVER_CLIENTS;++i){NfServerClient *c=&clients[i];if(c->occupied){nf_world_set_input(&world,c->entity_id,c->connected?c->current_input:(NfMoveInput){0});c->current_input.jump_pressed=false;}}nf_world_step(&world,1.0f/(float)NF_TICK_RATE);if(world.tick%(NF_TICK_RATE/NF_NET_SNAPSHOT_HZ)==0u)for(size_t i=0;i<NF_SERVER_CLIENTS;++i)if(clients[i].occupied&&clients[i].connected)send_snapshot(&net,&world,clients,&clients[i]);acc-=fixed_ms;}
        nf_net_flush(&net);if(duration>0.0&&(double)(now-start)>=duration*1000.0)break;sleep_ms(1);
    }
    for(size_t i=0;i<NF_SERVER_CLIENTS;++i){ if(clients[i].connected) nf_net_disconnect(clients[i].peer,0); }
    nf_net_flush(&net); nf_net_close(&net); nf_net_global_shutdown();
    printf("[server] shutdown tick=%llu\n",(unsigned long long)world.tick);
    return 0;
}
