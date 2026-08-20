#define _POSIX_C_SOURCE 200809L

#include "nf_net.h"
#include "nf_prediction.h"
#include "nf_relations.h"
#include "nf_security.h"
#include "nf_weapon_solution.h"
#include "nf_world.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void sleep_ms(unsigned ms){struct timespec ts={(time_t)(ms/1000u),(long)(ms%1000u)*1000000L};nanosleep(&ts,NULL);}
static void hello(NfNetHost *net,void *peer){NfHelloMessage h={0};nf_security_random(h.client_nonce,sizeof(h.client_nonce));uint8_t b[128];size_t n=nf_protocol_encode_hello(b,sizeof(b),&h);if(n)nf_net_send(net,peer,NF_NET_CHANNEL_RELIABLE,b,n,true);}

static void scenario_inputs(
    const char *scenario,uint32_t seq,bool have_target,const NfActor *local,NfVec3 target,
    NfMoveInput *move,NfCombatInput *combat){
    *move=(NfMoveInput){0};*combat=(NfCombatInput){0};
    const uint32_t phase=seq%240u;
    move->forward=phase<150u?0.72f:0.20f;
    move->strafe=phase<60u?0.0f:(phase<120u?0.72f:(phase<180u?-0.72f:0.0f));
    move->sprint_held=phase<72u;
    if((seq%180u)==20u)move->jump_pressed=true;

    if(have_target&&local!=NULL){
        const float dx=target.x-local->transform.position.x,dz=target.z-local->transform.position.z;
        const float horizontal=sqrtf(dx*dx+dz*dz);
        move->yaw_radians=atan2f(dx,dz);
        combat->aim_pitch_radians=atan2f((target.y+0.90f)-(local->transform.position.y+local->movement.eye_height),horizontal>0.01f?horizontal:0.01f);
    }

    if(strcmp(scenario,"baseline")==0){combat->fire_held=have_target;combat->fire_pressed=have_target&&(seq%18u)==0u;}
    else if(strcmp(scenario,"focus_hold")==0){combat->focus_held=phase>=45u&&phase<180u;combat->fire_held=have_target;combat->fire_pressed=have_target&&(seq%18u)==0u;}
    else if(strcmp(scenario,"reload_focus")==0){combat->focus_held=phase>=40u&&phase<190u;combat->reload_pressed=phase==55u;combat->fire_pressed=have_target&&(phase==35u||phase==125u||phase==190u);}
    else if(strcmp(scenario,"redirect_focus")==0){combat->focus_held=(phase>=52u&&phase<92u)||(phase>=112u&&phase<150u);combat->fire_pressed=have_target&&(phase==70u||phase==132u||phase==198u);}
    else if(strcmp(scenario,"sustained_hold")==0){combat->focus_held=phase>=20u&&phase<220u;combat->fire_pressed=have_target&&(seq%30u)==0u;}
    else {combat->focus_held=(phase%80u)<24u;combat->fire_pressed=have_target&&(seq%24u)==0u;}
}

int main(int argc,char **argv){
    const char *host="127.0.0.1",*scenario="baseline";uint16_t port=NF_NET_DEFAULT_PORT;double duration=6.0;
    for(int i=1;i<argc;++i){if(strcmp(argv[i],"--host")==0&&i+1<argc)host=argv[++i];else if(strcmp(argv[i],"--port")==0&&i+1<argc)port=(uint16_t)atoi(argv[++i]);else if(strcmp(argv[i],"--duration")==0&&i+1<argc)duration=atof(argv[++i]);else if(strcmp(argv[i],"--scenario")==0&&i+1<argc)scenario=argv[++i];}
    if(!nf_security_init()||!nf_net_global_init())return 2;NfNetHost net;if(!nf_net_client_open(&net)){nf_net_global_shutdown();return 2;}void *peer=nf_net_connect(&net,host,port);
    bool welcomed=false;uint8_t token[NF_NET_TOKEN_BYTES]={0};NfEntityId id=0;NfWorld world;nf_world_init(&world,20260807u);nf_world_build_movement_lab(&world);NfPrediction pred;nf_prediction_init(&pred);
    uint32_t start=nf_net_now_ms(),last_tick=start;unsigned snapshots=0,shot_events=0,focused_shots=0,blocked_shots=0;bool have_target=false;NfVec3 target={0};float max_focus_delta=0.0f;

    while((double)(nf_net_now_ms()-start)<duration*1000.0){
        NfNetEvent event;while(nf_net_service(&net,&event,0)>0){
            if(event.type==NF_NET_EVENT_CONNECT){peer=event.peer;hello(&net,peer);}else if(event.type==NF_NET_EVENT_RECEIVE){NfMessageType type=nf_protocol_peek_type(event.data,event.size);
                if(type==NF_MSG_WELCOME){NfWelcomeMessage w;if(nf_protocol_decode_welcome(event.data,event.size,&w)){id=w.entity_id;memcpy(token,w.session_token,NF_NET_TOKEN_BYTES);nf_world_init(&world,w.world_seed);nf_world_build_movement_lab(&world);nf_world_spawn_actor_with_id(&world,id,NF_FACTION_PLAYER,(NfVec3){0,0.05f,-18});world.tick=w.server_tick;nf_prediction_init(&pred);welcomed=true;}}
                else if(type==NF_MSG_SNAPSHOT&&welcomed){NfSnapshotMessage s;if(nf_protocol_decode_snapshot(event.data,event.size,&s)){++snapshots;NfFaction local_faction=NF_FACTION_NONE;const NfActorNetState *self=NULL;for(uint8_t i=0;i<s.actor_count;++i){if(s.actors[i].id==id){self=&s.actors[i];local_faction=s.actors[i].faction;break;}}if(self!=NULL){const NfActor *before=nf_world_find_actor_const(&world,id);const float predicted_focus=before?before->weapon_authority.focus_amount:0.0f;if(s.owner_authority_valid){const float d=fabsf(predicted_focus-s.owner_authority.focus_amount);if(d>max_focus_delta)max_focus_delta=d;}nf_prediction_reconcile_v16b(&pred,&world,id,self,s.owner_authority_valid?&s.owner_authority:NULL,s.acknowledged_input,s.server_tick,0.035f,0.08f);}have_target=false;for(uint8_t i=0;i<s.actor_count;++i){const NfActorNetState *a=&s.actors[i];NfRelationship r=nf_relation_between(local_faction,a->faction,NF_RELATION_HOSTILE);if(a->id!=id&&a->alive&&local_faction!=NF_FACTION_NONE&&nf_relation_can_damage(r,false)){target=a->position;have_target=true;break;}}}}
                else if(type==NF_MSG_COMBAT_EVENT){NfCombatEventMessage m;if(nf_protocol_decode_combat_event(event.data,event.size,&m)&&m.event.source==id&&m.event.type==NF_COMBAT_EVENT_GUNFIRE){++shot_events;if(m.event.shot_sequence>0u){const NfActor *local=nf_world_find_actor_const(&world,id);if(local&&local->weapon_authority.focus_amount>0.20f)++focused_shots;}if(m.event.shot_blocked)++blocked_shots;}}
            }
        }
        const uint32_t now=nf_net_now_ms();if(welcomed&&now-last_tick>=16u){last_tick=now;NfActor *local=nf_world_find_actor(&world,id);NfMoveInput move;NfCombatInput combat;scenario_inputs(scenario,pred.next_sequence,have_target,local,target,&move,&combat);NfInputCommand command=nf_prediction_make_command(&pred,world.tick,move,combat);nf_prediction_simulate_command(&world,id,&command);nf_prediction_record(&pred,&command,nf_world_find_actor(&world,id));NfInputBundle bundle={0};memcpy(bundle.session_token,token,NF_NET_TOKEN_BYTES);nf_prediction_build_redundant_bundle(&pred,command.sequence,&bundle);uint8_t packet[NF_NET_MAX_PACKET_BYTES];size_t n=nf_protocol_encode_input(packet,sizeof(packet),&bundle);if(n)nf_net_send(&net,peer,NF_NET_CHANNEL_STATE,packet,n,false);}
        sleep_ms(1);
    }
    if(peer!=NULL)nf_net_disconnect(peer,0);nf_net_flush(&net);nf_net_close(&net);nf_net_global_shutdown();
    printf("scenario,snapshots,shots,focused_shots,blocked_shots,corrections,resyncs,authority_corrections,max_pos_error_m,max_authority_error,max_focus_delta\n");
    printf("%s,%u,%u,%u,%u,%u,%u,%u,%.6f,%.6f,%.6f\n",scenario,snapshots,shot_events,focused_shots,blocked_shots,pred.corrections,pred.resyncs,pred.authority_corrections,pred.max_error,pred.max_authority_error,max_focus_delta);
    return welcomed&&snapshots>=3?0:1;
}
