#define _POSIX_C_SOURCE 200809L
/* Opt-in single-physical-actor server proof. No multiplayer/B5 recovery claim. */
#include "v18b2_encounter_server.h"
#include "nf_encounter18b2.h"
#include "nf_net.h"
#include "nf_security.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int nf18b2_encounter_server_main(int argc,char **argv){
    uint16_t port=27778;double duration=6;Nf18b2Model model=NF18B2_M4_MATERIAL_FIRST;
    for(int i=1;i<argc;++i){
        if(!strcmp(argv[i],"--port")&&i+1<argc)port=(uint16_t)atoi(argv[++i]);
        else if(!strcmp(argv[i],"--duration")&&i+1<argc)duration=atof(argv[++i]);
        else if(!strcmp(argv[i],"--b2-model")&&i+1<argc)model=(Nf18b2Model)atoi(argv[++i]);
    }
    if(!(duration>0)||!isfinite(duration)||(model!=NF18B2_M3_EXHAUSTIVE&&model!=NF18B2_M4_MATERIAL_FIRST))return 2;
    NfWorld *w=calloc(1,sizeof(*w));Nf18a6Runtime *owner=calloc(1,sizeof(*owner));
    Nf18b2EncounterRuntime *r=calloc(1,sizeof(*r));
    if(!w||!owner||!r){free(w);free(owner);free(r);return 2;}
    nf_world_init(w,42);nf_world_add_collider(w,NF_COLLIDER_SOLID,(NfVec3){0,-.5f,0},(NfVec3){4,0,4});
    nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,(NfVec3){1,0,1});
    NfActor *actor=nf_world_find_actor(w,10);actor->movement.grounded=true;actor->movement.mode=NF_MOVE_GROUND;
    Nf18a4Body crate={.id=2010,.center={2.2f,.9f,1},.inverse_mass=1.f/24,
        .inverse_inertia={.3f,.3f,.3f},.box_half={.2f,.5f,.35f}};
    bool ok=nf18a6_init(owner,w,10,crate,3,NF18A6_WORLD_CAMERA);
    uint8_t voxels[NF18A5_CANON_VOXELS]={0};
    ok=ok&&nf18a5_load_canonical(&owner->physical.world.grid,0,0,0,voxels,1)&&
        nf18a6_bind_world_tick(w,owner,NULL,NULL,NULL)&&nf18b2_encounter_init(r,w,owner,model)&&
        nf18b2_register_actor(r,10,1,1,10);
    const Nf18b2Object target={.id=2010,.revision=1,.cell_id=1,.nexus_id=2,.slot_id=3,
        .target={1.999f,1.62f,1},.max_reach=3,.required_credentials=1,.required_inventory=1,
        .capacity=1,.preconditions=1,.requires_support=1};
    ok=ok&&nf18b2_register_object(r,target,owner->physical.world.grid.global_revision,1,0)&&nf18b2_attach(r);
    NfNetHost net;
    if(!ok||!nf_security_init()||!nf_net_global_init()||!nf_net_server_open(&net,port,1)){
        free(r);free(owner);free(w);return 2;
    }
    void *peer=NULL;uint8_t token[NF_NET_TOKEN_BYTES]={0};bool welcomed=false,held=false;
    uint32_t last_sequence=0,last_input_ms=0,action=0;unsigned proposals=0,duplicates=0,frames=0;
    unsigned committed=0,pending=0,rejected=0;
    NfMoveInput movement={0};
    const uint32_t start=nf_net_now_ms();double next_tick=start;uint32_t next_snapshot=start;
    uint32_t arrivals[NF18B2_MAX_TRACES]={0};size_t reported=0;
    printf("[b2-server] opt-in A6 owner port=%u model=M%u crypto=%s scope=single-physical-actor\n",
        port,(unsigned)model,nf_security_is_strong()?"libsodium":"scaffold");fflush(stdout);
    while((double)(nf_net_now_ms()-start)<duration*1000){
        NfNetEvent e;
        while(nf_net_service(&net,&e,0)>0){
            if(e.type==NF_NET_EVENT_RECEIVE){
                const NfMessageType type=nf_protocol_peek_type(e.data,e.size);
                if(type==NF_MSG_HELLO){
                    NfHelloMessage h;if(!nf_protocol_decode_hello(e.data,e.size,&h))continue;
                    if(welcomed&&e.peer!=peer)continue;
                    peer=e.peer;
                    if(!welcomed){uint8_t nonce[16];nf_security_random(nonce,sizeof nonce);
                        nf_security_derive_token(h.client_nonce,nonce,token);}
                    welcomed=true;NfWelcomeMessage welcome={.entity_id=10,.server_tick=w->tick,.world_seed=w->seed,
                        .strong_crypto=nf_security_is_strong()};memcpy(welcome.session_token,token,sizeof token);
                    uint8_t data[128];size_t n=nf_protocol_encode_welcome(data,sizeof data,&welcome);
                    nf_net_send(&net,peer,NF_NET_CHANNEL_RELIABLE,data,n,true);
                }else if(type==NF_MSG_INPUT&&welcomed&&e.peer==peer){
                    NfInputBundle b;if(!nf_protocol_decode_input(e.data,e.size,&b)||!b.count||
                       !nf_security_token_equal(token,b.session_token))continue;
                    for(size_t i=0;i<b.count;++i){const NfInputCommand *c=&b.commands[i];
                        if((int32_t)(c->sequence-last_sequence)<=0){++duplicates;continue;}
                        if(!isfinite(c->move.forward)||!isfinite(c->move.strafe)||!isfinite(c->move.yaw_radians)||
                           fabsf(c->move.forward)>1||fabsf(c->move.strafe)>1||fabsf(c->move.yaw_radians)>100000)continue;
                        last_sequence=c->sequence;last_input_ms=nf_net_now_ms();
                        if(c->move.interact_held&&!held){action=c->sequence;
                            const size_t trace_before=r->trace_count;
                            Nf18b2Status status=nf18b2_propose(r,(Nf18b2Intent){10,2010,action,NF18B2_PUSH});
                            if(r->trace_count>trace_before)arrivals[trace_before]=last_input_ms;
                            ++proposals;printf("[b2-intent] action=%u assessment=%u tick=%llu\n",action,(unsigned)status,(unsigned long long)w->tick);
                        }
                        if(!c->move.interact_held&&held){
                            if(!nf18b2_withdraw(r,10,action))(void)nf18b2_interrupt(r,10,action);
                        }
                        held=c->move.interact_held;movement=c->move;movement.interact_held=false;
                    }
                }
            }else if(e.type==NF_NET_EVENT_DISCONNECT&&e.peer==peer){
                welcomed=false;held=false;movement=(NfMoveInput){0};
                if(!nf18b2_withdraw(r,10,action))(void)nf18b2_interrupt(r,10,action);
                /* No reconnect recovery/regrant protocol is silently supplied. */
                break;
            }
        }
        const uint32_t now=nf_net_now_ms();
        if((double)now>=next_tick){
            if(now-last_input_ms>NF_NET_STALE_HELD_INPUT_MS){movement=(NfMoveInput){0};
                if(held){if(!nf18b2_withdraw(r,10,action))(void)nf18b2_interrupt(r,10,action);held=false;}}
            nf_world_set_input(w,10,movement);
            const NfWorldTickStatus disposition=nf_world_step_checked(w,1.f/NF_TICK_RATE);++frames;
            committed+=disposition==NF_WORLD_TICK_COMMITTED;
            pending+=disposition==NF_WORLD_TICK_PENDING;
            rejected+=disposition==NF_WORLD_TICK_REJECTED;
            next_tick+=1000.0/NF_TICK_RATE;
            if((double)now-next_tick>250)next_tick=now;
            for(;reported<r->trace_count;++reported){const Nf18b2EncounterTrace *t=&r->traces[reported];
                printf("[b2-disposition] action=%u state=%s published=%u server_input_to_disposition_ms=%u\n",
                    t->intent.action_id,nf18b2_encounter_state_name(t->state),t->published,now-arrivals[reported]);}
        }
        if(welcomed&&(int32_t)(now-next_snapshot)>=0){
            NfSnapshotMessage s={.server_tick=w->tick,.acknowledged_input=last_sequence,.actor_count=1};
            nf_actor_to_net_state(actor,&s.actors[0]);uint8_t data[NF_NET_MAX_PACKET_BYTES];
            size_t n=nf_protocol_encode_snapshot(data,sizeof data,&s);nf_net_send(&net,peer,NF_NET_CHANNEL_STATE,data,n,false);
            next_snapshot=now+33;
        }
        nf_net_flush(&net);const struct timespec pause={0,1000000};nanosleep(&pause,NULL);
    }
    for(size_t i=0;i<r->trace_count;++i){const Nf18b2EncounterTrace *t=&r->traces[i];
        printf("[b2-encounter] action=%u state=%s published=%u applied_receipts=%u balance=%u->%u\n",
            t->intent.action_id,nf18b2_encounter_state_name(t->state),t->published,t->applied_receipts,t->balance_before,t->balance_after);
    }
    printf("[b2-summary] proposals=%u grants=%zu duplicate_packets=%u frames=%u tick=%llu actual_A4_receipts=%u history_samples=%u balance=%u committed_frames=%u pending_frames=%u rejected_frames=%u\n",
        proposals,r->grant_count,duplicates,frames,(unsigned long long)w->tick,owner->physical.committed_receipts,
        owner->physical.world.history.raw_samples,r->registry.accounts[0].balance,committed,pending,rejected);
    nf_net_close(&net);nf_net_global_shutdown();free(r);free(owner);free(w);return 0;
}
