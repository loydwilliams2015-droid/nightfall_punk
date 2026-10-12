#define _POSIX_C_SOURCE 200809L
#include "nf_net.h"
#include "nf_security.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int main(int argc,char **argv){
    uint16_t port=27778;double duration=4;
    int b3_kind=-1;unsigned hold_ms=450;
    for(int i=1;i<argc;++i){if(!strcmp(argv[i],"--port")&&i+1<argc)port=(uint16_t)atoi(argv[++i]);
        else if(!strcmp(argv[i],"--duration")&&i+1<argc)duration=atof(argv[++i]);
        else if(!strcmp(argv[i],"--b3-kind")&&i+1<argc){b3_kind=atoi(argv[++i]);hold_ms=1200;}}
    if(!nf_security_init()||!nf_net_global_init())return 2;
    NfNetHost net;if(!nf_net_client_open(&net))return 2;
    /* Loss/jitter here exercise existing transport, not a B5 recovery claim. */
    nf_net_set_simulation(&net,20,10,2);
    void *peer=nf_net_connect(&net,"127.0.0.1",port);uint8_t token[16]={0};
    unsigned snapshots=0,packets=0;uint32_t sequence=0;uint64_t tick=0;
    bool welcomed=false;NfVec3 initial={0},latest={0};uint32_t welcome_ms=0;
    const uint32_t start=nf_net_now_ms();uint32_t last_send=start;
    while((double)(nf_net_now_ms()-start)<duration*1000){
        NfNetEvent e;while(nf_net_service(&net,&e,0)>0){
            if(e.type==NF_NET_EVENT_CONNECT){NfHelloMessage h={0};nf_security_random(h.client_nonce,16);
                uint8_t data[128];size_t n=nf_protocol_encode_hello(data,sizeof data,&h);
                nf_net_send(&net,peer,NF_NET_CHANNEL_RELIABLE,data,n,true);
            }else if(e.type==NF_NET_EVENT_RECEIVE){const NfMessageType type=nf_protocol_peek_type(e.data,e.size);
                if(type==NF_MSG_WELCOME){NfWelcomeMessage w;if(nf_protocol_decode_welcome(e.data,e.size,&w)){
                    memcpy(token,w.session_token,16);tick=w.server_tick;welcomed=true;welcome_ms=nf_net_now_ms();}}
                else if(type==NF_MSG_SNAPSHOT){NfSnapshotMessage s;if(nf_protocol_decode_snapshot(e.data,e.size,&s)&&s.actor_count){
                    if(!snapshots)initial=s.actors[0].position;
                    latest=s.actors[0].position;tick=s.server_tick;++snapshots;}}
            }
        }
        const uint32_t now=nf_net_now_ms();
        if(welcomed&&now-last_send>=16){last_send=now;NfInputBundle b={.count=1};memcpy(b.session_token,token,16);
            NfInputCommand *c=&b.commands[0];c->sequence=++sequence;c->client_tick=tick;
            /* One push attempt, followed by withdrawal. Repeated wire packets
               carry the same action ID and must not cause a second debit. */
            const uint32_t age=now-welcome_ms;
            c->move.strafe=(b3_kind<0?age<hold_ms:b3_kind==3&&age<350)?1:0;c->move.interact_held=age<hold_ms;
            uint8_t data[NF_NET_MAX_PACKET_BYTES];size_t n=nf_protocol_encode_input(data,sizeof data,&b);
            nf_net_send(&net,peer,NF_NET_CHANNEL_STATE,data,n,false);
            if(sequence%3==0)nf_net_send(&net,peer,NF_NET_CHANNEL_STATE,data,n,false);
            ++packets;
        }
        nf_net_flush(&net);const struct timespec pause={0,1000000};nanosleep(&pause,NULL);
    }
    printf("b2-netbot welcomed=%s snapshots=%u packets=%u tick=%llu initial_x=%.4f final_x=%.4f\n",
        welcomed?"yes":"no",snapshots,packets,(unsigned long long)tick,initial.x,latest.x);
    if(peer)nf_net_disconnect(peer,0);
    nf_net_flush(&net);nf_net_close(&net);nf_net_global_shutdown();
    return welcomed&&snapshots>3&&(b3_kind>=0&&b3_kind!=3?true:latest.x>initial.x)?0:1;
}
