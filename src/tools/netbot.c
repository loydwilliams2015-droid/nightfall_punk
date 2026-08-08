#define _POSIX_C_SOURCE 200809L

#include "nf_net.h"
#include "nf_prediction.h"
#include "nf_security.h"
#include "nf_world.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void sleep_ms(unsigned ms) {
    struct timespec ts={(time_t)(ms/1000u),(long)(ms%1000u)*1000000L};
    nanosleep(&ts,NULL);
}
static void hello(NfNetHost *net,void *peer) {
    NfHelloMessage h={0}; nf_security_random(h.client_nonce,sizeof(h.client_nonce)); uint8_t b[128]; size_t n=nf_protocol_encode_hello(b,sizeof(b),&h); nf_net_send(net,peer,NF_NET_CHANNEL_RELIABLE,b,n,true);
}

int main(int argc,char **argv) {
    const char *host="127.0.0.1"; uint16_t port=NF_NET_DEFAULT_PORT; double duration=5.0; int pattern=0;
    for(int i=1;i<argc;++i) {
        if(strcmp(argv[i],"--host")==0&&i+1<argc)host=argv[++i];
        else if(strcmp(argv[i],"--port")==0&&i+1<argc)port=(uint16_t)atoi(argv[++i]);
        else if(strcmp(argv[i],"--duration")==0&&i+1<argc)duration=atof(argv[++i]);
        else if(strcmp(argv[i],"--pattern")==0&&i+1<argc)pattern=atoi(argv[++i]);
    }
    if(!nf_security_init()||!nf_net_global_init()) return 2;
    NfNetHost net;
    if(!nf_net_client_open(&net)) { nf_net_global_shutdown(); return 2; }
    void *peer=nf_net_connect(&net,host,port); bool connected=false,welcomed=false; uint8_t token[NF_NET_TOKEN_BYTES]={0}; NfEntityId id=0;
    NfWorld world; nf_world_init(&world,20260807u); nf_world_build_movement_lab(&world); NfPrediction pred; nf_prediction_init(&pred);
    uint32_t start=nf_net_now_ms(),last_tick=start; unsigned snapshots=0,combat_events=0,damage_events=0,death_events=0; bool have_target=false; NfVec3 target_position={0};

    while((double)(nf_net_now_ms()-start)<duration*1000.0) {
        NfNetEvent e;
        while(nf_net_service(&net,&e,0)>0) {
            if(e.type==NF_NET_EVENT_CONNECT) { connected=true; peer=e.peer; hello(&net,peer); }
            else if(e.type==NF_NET_EVENT_DISCONNECT) { connected=false; welcomed=false; }
            else if(e.type==NF_NET_EVENT_RECEIVE) {
                NfMessageType t=nf_protocol_peek_type(e.data,e.size);
                if(t==NF_MSG_WELCOME) {
                    NfWelcomeMessage w;
                    if(nf_protocol_decode_welcome(e.data,e.size,&w)) { id=w.entity_id; memcpy(token,w.session_token,NF_NET_TOKEN_BYTES); nf_world_init(&world,w.world_seed); nf_world_build_movement_lab(&world); nf_world_spawn_actor_with_id(&world,id,NF_FACTION_PLAYER,(NfVec3){0,0.05f,-18}); world.tick=w.server_tick; nf_prediction_init(&pred); welcomed=true; }
                } else if(t==NF_MSG_SNAPSHOT&&welcomed) {
                    NfSnapshotMessage s;
                    if(nf_protocol_decode_snapshot(e.data,e.size,&s)) {
                        ++snapshots; NfFaction local_faction=NF_FACTION_NONE; NfVec3 local_position={0};
                        for(uint8_t i=0;i<s.actor_count;++i) {
                            if(s.actors[i].id==id) { local_faction=s.actors[i].faction; local_position=s.actors[i].position; nf_prediction_reconcile(&pred,&world,id,&s.actors[i],s.acknowledged_input,s.server_tick,0.035f); break; }
                        }
                        have_target=false;
                        for(uint8_t i=0;i<s.actor_count;++i) {
                            const NfActorNetState *a=&s.actors[i];
                            if(a->id!=id&&a->alive&&local_faction!=NF_FACTION_NONE&&a->faction!=local_faction) { target_position=a->position; have_target=true; break; }
                        }
                        (void)local_position;
                    }
                } else if(t==NF_MSG_COMBAT_EVENT) {
                    NfCombatEventMessage m;
                    if(nf_protocol_decode_combat_event(e.data,e.size,&m)) { ++combat_events; if(m.event.type==NF_COMBAT_EVENT_DAMAGE||m.event.type==NF_COMBAT_EVENT_DEATH)++damage_events; if(m.event.type==NF_COMBAT_EVENT_DEATH)++death_events; }
                }
            }
        }
        uint32_t now=nf_net_now_ms();
        if(welcomed&&now-last_tick>=16u) {
            last_tick=now; NfActor *local=nf_world_find_actor(&world,id); NfMoveInput m={0}; NfCombatInput combat={0};
            m.forward=(pred.next_sequence%180u)<45u?0.20f:0.0f; m.strafe=(pattern&1)?-0.10f:0.10f; m.sprint_held=false;
            if(((pred.next_sequence+(uint32_t)pattern)%120u)==0u)m.jump_pressed=true;
            if(have_target&&local!=NULL) {
                float dx=target_position.x-local->transform.position.x, dz=target_position.z-local->transform.position.z; float horizontal=sqrtf(dx*dx+dz*dz);
                m.yaw_radians=atan2f(dx,dz); combat.aim_pitch_radians=atan2f((target_position.y+0.90f)-(local->transform.position.y+local->movement.eye_height),horizontal>0.01f?horizontal:0.01f);
            } else {
                m.yaw_radians=(float)pattern*1.5707963f; combat.aim_pitch_radians=0.0f;
            }
            combat.fire_held=have_target; combat.fire_pressed=have_target&&(pred.next_sequence%30u)==0u; combat.reload_pressed=(pred.next_sequence%210u)==0u; combat.weapon_slot=(pred.next_sequence%300u)==0u?(uint8_t)(((pred.next_sequence/300u)%2u)+1u):0u;
            NfInputCommand c=nf_prediction_make_command(&pred,world.tick,m,combat); nf_world_set_input(&world,id,c.move); nf_world_step(&world,1.0f/(float)NF_TICK_RATE); nf_prediction_record(&pred,&c,nf_world_find_actor(&world,id));
            NfInputBundle b={0}; memcpy(b.session_token,token,NF_NET_TOKEN_BYTES); nf_prediction_build_redundant_bundle(&pred,c.sequence,&b); uint8_t packet[NF_NET_MAX_PACKET_BYTES]; size_t n=nf_protocol_encode_input(packet,sizeof(packet),&b); if(n)nf_net_send(&net,peer,NF_NET_CHANNEL_STATE,packet,n,false);
        }
        sleep_ms(1);
    }
    if(peer!=NULL) nf_net_disconnect(peer,0);
    nf_net_flush(&net); nf_net_close(&net); nf_net_global_shutdown();
    printf("netbot pattern=%d connected=%s snapshots=%u combat_events=%u damage_events=%u death_events=%u corrections=%u max_error=%.3f\n",pattern,connected?"yes":"no",snapshots,combat_events,damage_events,death_events,pred.corrections,pred.max_error);
    return welcomed&&snapshots>=3&&combat_events>=2&&damage_events>=1?0:1;
}
