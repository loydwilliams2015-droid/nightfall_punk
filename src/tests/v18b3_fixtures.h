#ifndef NF18B3_FIXTURES_H
#define NF18B3_FIXTURES_H
/* Shared authored INPUT construction only. Expected outcomes are independent
   in test/sample drivers; no production evaluation is used as the oracle. */
#include "nf_interaction18b3.h"
#include <stdlib.h>
typedef struct B3Fixture {NfWorld *w;Nf18a6Runtime *p;Nf18b2EncounterRuntime *a;Nf18b3Runtime *r;uint32_t target;} B3Fixture;
static bool b3_allocate(B3Fixture *f){
    *f=(B3Fixture){malloc(sizeof(NfWorld)),malloc(sizeof(Nf18a6Runtime)),malloc(sizeof(Nf18b2EncounterRuntime)),malloc(sizeof(Nf18b3Runtime)),0};
    return f->w&&f->p&&f->a&&f->r;
}
static void b3_free(B3Fixture *f){free(f->r);free(f->a);free(f->p);free(f->w);}
static bool b3_fixture(B3Fixture *f,unsigned model,Nf18b3Kind kind,float x,float z,unsigned far){
    nf_world_init(f->w,42);nf_world_add_collider(f->w,NF_COLLIDER_SOLID,(NfVec3){0,-.5f,0},(NfVec3){8,0,4});
    if(!nf_world_spawn_actor_with_id(f->w,10,NF_FACTION_PLAYER,(NfVec3){x,0,z}))return false;
    NfActor *actor=nf_world_find_actor(f->w,10);actor->movement.grounded=true;actor->movement.mode=NF_MOVE_GROUND;
    const bool crate=kind==NF18B3_PUSH||kind==NF18B3_CARRY;
    Nf18a4Body body={.id=2010,.center=crate?(NfVec3){kind==NF18B3_PUSH?x+.56f:x+1.2f,.9f,z}:(NfVec3){3,.9f,3},
        .inverse_mass=1.f/24,.inverse_inertia={.3f,.3f,.3f},.box_half={.2f,.5f,.35f}};
    if(kind==NF18B3_CARRY)body.center.y=1.f; /* Safe conservative clearance envelope. */
    if(!nf18a6_init(f->p,f->w,10,body,3,NF18A6_WORLD_CAMERA))return false;
    uint8_t cells[NF18A5_CANON_VOXELS]={0};
    if(!nf18a5_load_canonical(&f->p->physical.world.grid,0,0,0,cells,1)||
       !nf18a5_load_canonical(&f->p->physical.world.grid,1,0,0,cells,1)||
       !nf18a6_bind_world_tick(f->w,f->p,NULL,NULL,NULL)||
       !nf18b2_encounter_init(f->a,f->w,f->p,(Nf18b2Model)model)||
       !nf18b2_register_actor(f->a,10,1,1,10)||!nf18b2_attach(f->a)||
       !nf18b3_init(f->r,f->w,f->p,f->a,10000,8))return false;
    f->target=crate?2010:2110;
    Nf18b3Object object={.id=f->target,.revision=1,.kind=kind==NF18B3_DOOR_CLOSE?NF18B3_DOOR_OPEN:kind,
        .travel=kind==NF18B3_REPAIR?.02f:kind==NF18B3_SWITCH?.04f:.3f,
        .speed=kind==NF18B3_REPAIR?.2f:kind==NF18B3_SWITCH?.3f:.6f,
        .max_force=400,.max_carry_mass=60,.units=1,.max_health=3};
    object.body=crate?body:(Nf18a4Body){.id=f->target,.center={x+1.2f,.9f,z},.inverse_mass=.05f,
        .inverse_inertia={.3f,.3f,.3f},.box_half={.08f,.4f,.08f}};
    if(kind==NF18B3_PICKUP){object.body.center=(NfVec3){x+.65f,1.1f,z};object.body.box_half=(NfVec3){.08f,.08f,.08f};}
    object.origin=object.body.center;
    if(kind==NF18B3_DOOR_CLOSE)object.body.center.x+=object.travel;
    Nf18b2Object descriptor={.id=f->target,.revision=1,.cell_id=1,.nexus_id=2,.slot_id=3,
        .target={object.body.center.x-object.body.box_half.x-.001f,1.62f,z},.max_reach=3,
        .required_credentials=1,.capacity=1,.preconditions=1,.requires_support=1};
    if(!nf18b3_register(f->r,object,descriptor)||!nf18b3_bind(f->r))return false;
    for(unsigned i=0;i<far;++i)if(nf_world_add_collider(f->w,NF_COLLIDER_SOLID,
        (NfVec3){20+(float)i,0,10},(NfVec3){20.5f+(float)i,2,10.5f})<0)return false;
    if(kind==NF18B3_PUSH)nf_world_set_input(f->w,10,(NfMoveInput){.strafe=1});
    return true;
}
static bool b3_step(B3Fixture *f){return nf_world_step_checked(f->w,1.f/60)==NF_WORLD_TICK_COMMITTED;}
static inline bool b3_run(B3Fixture *f,unsigned ticks){
    for(unsigned i=0;i<ticks;++i){if(!b3_step(f))return false;if(f->r->state.active==UINT32_MAX)break;}return true;
}
#endif
