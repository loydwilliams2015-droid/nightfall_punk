#include "nf_prediction.h"
#include "nf_protocol.h"
#include "nf_security.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static void protocol_roundtrip(void){
    uint8_t buf[NF_NET_MAX_PACKET_BYTES];NfHelloMessage h={0},h2={0};for(size_t i=0;i<NF_NET_NONCE_BYTES;++i)h.client_nonce[i]=(uint8_t)(i+1);assert(nf_protocol_decode_hello(buf,nf_protocol_encode_hello(buf,sizeof(buf),&h),&h2));assert(memcmp(h.client_nonce,h2.client_nonce,NF_NET_NONCE_BYTES)==0);
    NfInputBundle in={.count=2};memset(in.session_token,0x5a,sizeof(in.session_token));in.commands[0]=(NfInputCommand){.sequence=10,.client_tick=100,.move={.forward=1,.strafe=-0.5f,.yaw_radians=0.25f,.jump_pressed=true},.combat={.fire_held=true,.fire_pressed=true,.aim_pitch_radians=0.12f}};in.commands[1]=(NfInputCommand){.sequence=11,.client_tick=101,.move={.forward=1,.sprint_held=true},.combat={.reload_pressed=true,.weapon_slot=2}};NfInputBundle out={0};size_t n=nf_protocol_encode_input(buf,sizeof(buf),&in);assert(n>0&&nf_protocol_decode_input(buf,n,&out));assert(out.count==2&&out.commands[0].sequence==10&&out.commands[0].move.jump_pressed&&out.commands[0].combat.fire_pressed&&fabsf(out.commands[0].combat.aim_pitch_radians-0.12f)<0.001f&&out.commands[1].combat.reload_pressed&&out.commands[1].combat.weapon_slot==2);
    NfSnapshotMessage s={.server_tick=900,.acknowledged_input=11,.actor_count=1};s.actors[0]=(NfActorNetState){.id=7,.faction=NF_FACTION_RIVAL,.position={1,2,3},.velocity={4,5,6},.mode=NF_MOVE_MANTLE,.candidate_type=NF_TRAVERSAL_MANTLE,.candidate_feature=13,.health=64,.alive=true,.weapon=NF_WEAPON_PISTOL,.weapon_state=NF_WEAPON_READY,.ammo_mag=7,.reserve_ammo=31};NfSnapshotMessage s2={0};n=nf_protocol_encode_snapshot(buf,sizeof(buf),&s);assert(n>0&&nf_protocol_decode_snapshot(buf,n,&s2));assert(s2.server_tick==900&&s2.actors[0].id==7&&s2.actors[0].candidate_feature==13&&s2.actors[0].faction==NF_FACTION_RIVAL&&s2.actors[0].health==64.0f&&s2.actors[0].weapon==NF_WEAPON_PISTOL&&s2.actors[0].ammo_mag==7);
    NfCombatEventMessage ce={.event={.sequence=33,.server_tick=901,.type=NF_COMBAT_EVENT_DAMAGE,.source=4,.target=7,.weapon=NF_WEAPON_CARBINE,.hit_zone=NF_HIT_HEAD,.amount=27.0f,.position={1,2,3},.rewind_ms=83}},ce2={0};n=nf_protocol_encode_combat_event(buf,sizeof(buf),&ce);assert(n>0&&nf_protocol_decode_combat_event(buf,n,&ce2));assert(ce2.event.sequence==33&&ce2.event.target==7&&ce2.event.hit_zone==NF_HIT_HEAD&&ce2.event.rewind_ms==83);
}

static void prediction_reconcile(void){NfWorld w;nf_world_init(&w,9);nf_world_add_collider(&w,NF_COLLIDER_SOLID,(NfVec3){-50,-0.5f,-50},(NfVec3){50,0,50});NfEntityId id=nf_world_spawn_actor_with_id(&w,42,NF_FACTION_PLAYER,(NfVec3){0,0,0});NfActor *a=nf_world_find_actor(&w,id);a->movement.grounded=true;NfPrediction p;nf_prediction_init(&p);NfActorNetState auth={0};uint32_t ack=0;uint64_t tick=0;for(int i=0;i<6;++i){NfMoveInput m={.forward=1};NfInputCommand c=nf_prediction_make_command(&p,w.tick,m,(NfCombatInput){0});nf_world_set_input(&w,id,m);nf_world_step(&w,1.0f/(float)NF_TICK_RATE);nf_prediction_record(&p,&c,a);if(i==2){nf_actor_to_net_state(a,&auth);auth.position.z-=0.20f;auth.health=73.0f;ack=c.sequence;tick=w.tick;}}assert(nf_prediction_reconcile(&p,&w,id,&auth,ack,tick,0.035f));assert(p.corrections==1&&p.last_error>0.15f&&fabsf(a->health-73.0f)<0.01f);}

static void security_scaffold(void){assert(nf_security_init());uint8_t a[NF_NET_NONCE_BYTES],b[NF_NET_NONCE_BYTES],t[NF_NET_TOKEN_BYTES],t2[NF_NET_TOKEN_BYTES];nf_security_random(a,sizeof(a));nf_security_random(b,sizeof(b));nf_security_derive_token(a,b,t);nf_security_derive_token(a,b,t2);assert(!nf_security_token_is_zero(t));assert(nf_security_token_equal(t,t2));}

int main(void){protocol_roundtrip();prediction_reconcile();security_scaffold();puts("nightfall v0.4 network + combat protocol tests: PASS");return 0;}
