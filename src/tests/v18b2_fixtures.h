#ifndef NF18B2_FIXTURES_H
#define NF18B2_FIXTURES_H
#include "nf_eligibility18b2.h"
#include <string.h>
#include <stdlib.h>
enum { F_REACH=1,F_POSE=2,F_KEY=4,F_ITEM=8,F_PRE=16,F_MEMBER=32,
       F_FULL=64,F_WALL=128,F_UNKNOWN=256,F_STALE=512,F_BODY=1024,F_SUPPORT=2048 };
typedef struct B2Fixture {
    Nf18a5World material;
    Nf18b2Actor actors[16];
    Nf18b2Object objects[16];
    Nf18aCollider solids[16];
    Nf18b2Scene scene;
    Nf18b2Use use;
} B2Fixture;
static void fixture(B2Fixture *f,unsigned flags){
    memset(f,0,sizeof(*f));nf18a5_world_init(&f->material,2);
    uint8_t cells[NF18A5_CANON_VOXELS]={0};
    if(flags&F_WALL)cells[22]=NF18A5_SOLID;
    if(flags&F_BODY)cells[0]=NF18A5_SOLID;
    if(flags&F_UNKNOWN)cells[5]=NF18A5_MIXED;
    if(!nf18a5_load_canonical(&f->material.grid,0,0,0,cells,1))abort();
    f->actors[0]=(Nf18b2Actor){.id=7,.revision=1,.credentials=1,.inventory=2,
        .feet={1,0,1},.radius=.3f,.height=1.8f,.eye_height=1.5f,.active=1,.grounded=1};
    f->objects[0]=(Nf18b2Object){.id=12,.revision=1,.cell_id=1,.nexus_id=1,
        .slot_id=1,.target={3,1.5f,1},.max_reach=3,.required_credentials=1,
        .required_inventory=2,.capacity=1,.preconditions=1};
    if(flags&F_REACH)f->objects[0].max_reach=1.5f;
    if(flags&F_POSE)f->actors[0].active=0;
    if(flags&F_KEY)f->actors[0].credentials=0;
    if(flags&F_ITEM)f->actors[0].inventory=0;
    if(flags&F_PRE)f->objects[0].preconditions=0;
    if(flags&F_MEMBER){f->objects[0].members[0]=8;f->objects[0].member_count=1;}
    if(flags&F_FULL){f->objects[0].occupants[0]=8;f->objects[0].occupant_count=1;}
    f->scene=(Nf18b2Scene){.material=&f->material,
        .geometry={.world_version=f->material.revision,.tick=10},
        .actors=f->actors,.objects=f->objects,.actor_count=1,.object_count=1};
    if(flags&F_SUPPORT){
        f->solids[0]=(Nf18aCollider){.body_id=88,.min={0,-1,0},.max={4,0,4},.dynamic_body=1};
        f->scene.geometry.solids=f->solids;f->scene.geometry.solid_count=1;
        f->actors[0].support_id=88;f->objects[0].requires_support=1;
    }
    f->use=(Nf18b2Use){.actor_id=7,.object_id=12,.action_id=1,.tick=10,
        .world_revision=f->material.revision,.material_epoch=f->material.grid.global_revision,
        .actor_revision=1,.object_revision=1};
    if(flags&F_STALE)--f->use.material_epoch;
}
/* Authored-domain oracle: no calls to candidate code or geometry queries. */
static inline Nf18b2Reason expected_reason(unsigned f){
    if(f&F_STALE)return NF18B2_FRAME_CHANGED;
    if(f&F_POSE)return NF18B2_POSE;
    if(f&F_REACH)return NF18B2_REACH;
    if(f&F_SUPPORT)return NF18B2_SUPPORT;
    if(f&F_BODY)return NF18B2_BODY_BLOCKED;
    if(f&F_WALL)return NF18B2_OCCLUDED;
    if(f&F_KEY)return NF18B2_CREDENTIAL;
    if(f&F_ITEM)return NF18B2_INVENTORY;
    if(f&F_PRE)return NF18B2_PRECONDITION;
    if(f&F_MEMBER)return NF18B2_MEMBERSHIP;
    if(f&F_FULL)return NF18B2_CAPACITY;
    if(f&F_UNKNOWN)return NF18B2_MATERIAL_PENDING;
    return NF18B2_OK;
}
static inline Nf18b2Status expected_status(unsigned f){
    const Nf18b2Reason r=expected_reason(f);
    return r==NF18B2_OK?NF18B2_ELIGIBLE:r==NF18B2_FRAME_CHANGED?NF18B2_STALE:
        r==NF18B2_MATERIAL_PENDING?NF18B2_PENDING:NF18B2_BLOCKED;
}
#endif
