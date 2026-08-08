#include "nf_world.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static void step_n(NfWorld *world,unsigned ticks){for(unsigned i=0;i<ticks;++i)nf_world_step(world,1.0f/(float)NF_TICK_RATE);}
static NfActor *spawn_test_actor(NfWorld *world,NfVec3 position){NfEntityId id=nf_world_spawn_actor(world,NF_FACTION_PLAYER,position);assert(id!=0);NfActor *a=nf_world_find_actor(world,id);assert(a!=NULL);return a;}
static void floor_world(NfWorld *w,uint32_t seed){nf_world_init(w,seed);nf_world_add_collider(w,NF_COLLIDER_SOLID,(NfVec3){-50,-0.5f,-50},(NfVec3){50,0,50});}

static void test_world_smoke(void){NfWorld w;nf_world_init(&w,42);assert(w.tick==0&&nf_world_active_actor_count(&w)==0);NfActor *a=spawn_test_actor(&w,(NfVec3){1,2,3});a->transform.velocity=(NfVec3){2,0,0};nf_world_step(&w,0.5f);assert(w.tick==1);}
static void test_spawn_with_network_id(void){NfWorld w;nf_world_init(&w,1);assert(nf_world_spawn_actor_with_id(&w,77,NF_FACTION_PLAYER,(NfVec3){0})==77);assert(nf_world_find_actor(&w,77)!=NULL);assert(w.next_entity_id==78);assert(nf_world_despawn_actor(&w,77));assert(nf_world_find_actor(&w,77)==NULL);}
static void test_gravity_and_ground(void){NfWorld w;floor_world(&w,2);NfActor *a=spawn_test_actor(&w,(NfVec3){0,3,0});step_n(&w,180);assert(a->movement.grounded);assert(fabsf(a->transform.position.y)<0.02f);}
static void test_walk_and_sprint(void){NfWorld a,b;floor_world(&a,3);floor_world(&b,4);NfActor *wa=spawn_test_actor(&a,(NfVec3){0});NfActor *sb=spawn_test_actor(&b,(NfVec3){0});wa->movement.grounded=true;sb->movement.grounded=true;wa->input.forward=1;sb->input.forward=1;sb->input.sprint_held=true;step_n(&a,60);step_n(&b,60);assert(sb->transform.position.z>wa->transform.position.z+1);}
static void test_right_handed_camera_basis(void){NfWorld w;floor_world(&w,5);NfActor *a=spawn_test_actor(&w,(NfVec3){0});a->movement.grounded=true;a->input.strafe=-1.0f;a->input.yaw_radians=0.0f;step_n(&w,20);assert(a->transform.position.x<0.0f);}
static void test_jump_and_crouch_second_jump(void){NfWorld w;floor_world(&w,6);NfActor *a=spawn_test_actor(&w,(NfVec3){0});a->movement.grounded=true;a->input.jump_pressed=true;nf_world_step(&w,1.0f/NF_TICK_RATE);assert(a->movement.jumps_used==1);a->input.jump_pressed=true;a->input.crouch_held=false;nf_world_step(&w,1.0f/NF_TICK_RATE);assert(a->movement.jumps_used==1);a->input.jump_pressed=true;a->input.crouch_held=true;nf_world_step(&w,1.0f/NF_TICK_RATE);assert(a->movement.jumps_used==2);}
static void test_fuzzy_rail_candidate(void){NfWorld w;floor_world(&w,7);nf_world_add_collider(&w,NF_COLLIDER_SOLID,(NfVec3){-1,0,1},(NfVec3){1,0.9f,1.5f});NfActor *a=spawn_test_actor(&w,(NfVec3){0});a->movement.grounded=true;a->input.forward=1;nf_world_step(&w,1.0f/NF_TICK_RATE);assert(a->movement.candidate.active&&a->movement.candidate.type==NF_TRAVERSAL_VAULT);}
static void test_dynamic_geometry_resync(void){NfWorld w;nf_world_init(&w,8);int idx=nf_world_add_moving_platform(&w,(NfVec3){0,1,0},(NfVec3){2,1.2f,2},(NfVec3){1,0,0},4,6);assert(idx>=0);w.tick=180;nf_world_sync_dynamic_geometry(&w);float x=w.colliders[idx].min.x;w.tick=181;nf_world_sync_dynamic_geometry(&w);assert(fabsf(w.colliders[idx].previous_min.x-x)<0.2f);}

int main(void){test_world_smoke();test_spawn_with_network_id();test_gravity_and_ground();test_walk_and_sprint();test_right_handed_camera_basis();test_jump_and_crouch_second_jump();test_fuzzy_rail_candidate();test_dynamic_geometry_resync();puts("nightfall v0.3 simulation tests: PASS");return 0;}
