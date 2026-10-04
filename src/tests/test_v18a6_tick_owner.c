#define _POSIX_C_SOURCE 200809L
#include "nf_embody18a6.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned pass,fail;
static void gate(bool ok,const char *name){
    printf("%s,%s\n",name,ok?"PASS":"FAIL");
    if(ok)++pass;else ++fail;
}
static NfMoveInput rightward(void){NfMoveInput in={0};in.strafe=1.0f;return in;}
static Nf18a4Body crate(float x){
    Nf18a4Body b={0};b.id=2010u;b.center=(NfVec3){x,.9f,1.0f};
    b.inverse_mass=1.f/24.f;b.inverse_inertia=(NfVec3){.3f,.3f,.3f};
    b.box_half=(NfVec3){.2f,.5f,.35f};return b;
}
static bool setup(NfWorld *w,Nf18a6Runtime *r,bool loaded){
    nf_world_init(w,42u);
    nf_world_add_collider(w,NF_COLLIDER_SOLID,(NfVec3){0,-.5f,0},
                                              (NfVec3){4,0,4});
    const NfEntityId id=nf_world_spawn_actor_with_id(w,10u,NF_FACTION_PLAYER,
                                              (NfVec3){1,0,1});
    if(id!=10u)return false;
    NfActor *a=nf_world_find_actor(w,id);
    a->movement.grounded=true;a->movement.mode=NF_MOVE_GROUND;
    if(!nf18a6_init(r,w,id,crate(2.2f),3u,NF18A6_WORLD_CAMERA))return false;
    if(loaded){
        uint8_t voxels[NF18A5_CANON_VOXELS]={0};
        if(!nf18a5_load_canonical(&r->physical.world.grid,0,0,0,voxels,1u))return false;
    }
    nf_world_set_input(w,id,rightward());
    return true;
}
typedef struct RecursionProbe { unsigned calls;NfWorldTickStatus inner; } RecursionProbe;
static NfWorldTickStatus recurse(NfWorld *world,float dt,void *ctx){
    RecursionProbe *p=(RecursionProbe *)ctx;
    p->calls++;
    p->inner=nf_world_step_checked(world,dt);
    ++world->tick;
    return NF_WORLD_TICK_COMMITTED;
}
static NfWorldTickStatus defer(NfWorld *world,float dt,void *ctx){
    (void)world;(void)dt;
    ++*(unsigned *)ctx;
    return NF_WORLD_TICK_PENDING;
}
int main(void){
    NfWorld *w=malloc(sizeof(*w));
    Nf18a6Runtime *r=malloc(sizeof(*r));
    if(!w||!r)return 2;
    Nf18a6TickTrace t=nf18a6_classify_tick(1u,55u,55u,true,false,1u,1u);
    gate(!t.purple_present,"P01_one_way_decomposes");
    t=nf18a6_classify_tick(1u,55u,56u,true,true,1u,1u);
    gate(!t.purple_present,"P02_cross_target_not_purple");
    t=nf18a6_classify_tick(1u,55u,55u,true,true,1u,2u);
    gate(!t.purple_present,"P03_disjoint_material_channels_not_purple");
    t=nf18a6_classify_tick(1u,55u,55u,true,true,1u,1u);
    gate(t.purple_present&&t.purple_status==NF17C_PURPLE_ACTIVE && t.dependency_hash,
         "P04_true_target_local_SCC_detected");
    t=nf18a6_classify_tick(2u,55u,55u,true,true,1u,1u);
    gate(t.purple_present&&t.tick==2u,"P05_envelope_is_new_each_tick");

    nf_world_init(w,10u);
    RecursionProbe rp={0};
    gate(nf_world_bind_tick_owner(w,recurse,&rp),"S01_bind_one_owner");
    gate(!nf_world_bind_tick_owner(w,recurse,&rp),"S02_reject_second_owner");
    gate(nf_world_step_checked(w,1.f/60.f)==NF_WORLD_TICK_COMMITTED&&
         rp.inner==NF_WORLD_TICK_REENTRANT&&rp.calls==1u&&w->tick==1u,
         "S03_recursive_step_blocked");
    gate(nf_world_unbind_tick_owner(w,recurse,&rp),"S04_unbind_matching_owner");
    unsigned pending_calls=0u;
    gate(nf_world_bind_tick_owner(w,defer,&pending_calls),"S05_pending_owner_bind");
    nf_world_step(w,1.f/60.f);
    gate(w->tick==1u&&pending_calls==1u&&
         w->last_tick_status==NF_WORLD_TICK_PENDING,"S06_no_legacy_fallback_on_pending");
    gate(nf_world_unbind_tick_owner(w,defer,&pending_calls),"S07_unbind_pending_owner");

    gate(setup(w,r,false),"S08_real_world_pending_fixture");
    gate(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL),"S09_opt_in_bound");
    gate(nf18a6_step(r,w,rightward(),NULL,NULL,1.f/60.f,NULL).status==NF18A6_STALE,
         "S09b_direct_step_cannot_bypass_bound_owner");
    nf_world_step(w,1.f/60.f);
    gate(w->tick==0u&&r->last_commit_tick==0u&&
         w->last_tick_status==NF_WORLD_TICK_PENDING,
         "S10_missing_canonical_parent_cannot_fall_back");
    gate(r->tick_trace.red_proposed&&r->tick_trace.blue_material_checked&&
         !r->tick_trace.purple_present,"S11_unknown_material_not_falsely_purple");
    uint8_t voxels[NF18A5_CANON_VOXELS]={0};
    gate(nf18a5_load_canonical(&r->physical.world.grid,0,0,0,voxels,1u),
         "S12_authoritative_canonical_parent_loaded");
    nf_world_step(w,1.f/60.f);
    const NfActor *a=nf_world_find_actor_const(w,10u);
    gate(w->tick==1u&&a&&a->transform.position.x>1.f&&
         w->last_tick_status==NF_WORLD_TICK_COMMITTED,
         "S13_opt_in_world_tick_actual_actor");
    gate(r->camera.initialized&&r->tick_trace.world_status==NF_WORLD_TICK_COMMITTED &&
         !r->tick_trace.purple_present,"S14_one_way_success_decomposes");
    gate(!nf18a6_bind_world_tick(w,r,NULL,NULL,NULL),"S15_no_duplicate_bind");
    gate(nf18a6_unbind_world_tick(w,r),"S16_unbind_embodiment");
    gate(setup(w,r,true),"S17_dynamic_contact_fixture");
    gate(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL),"S18_dynamic_owner_bind");
    unsigned purple_commits=0,other_commits=0;
    for(unsigned i=0;i<50u;++i){
        nf_world_set_input(w,10u,rightward());
        nf_world_step(w,1.f/60.f);
        if(w->last_tick_status!=NF_WORLD_TICK_COMMITTED)break;
        if(r->tick_trace.purple_present){
            ++purple_commits;
            if(r->tick_trace.purple_status!=NF17C_PURPLE_COMMITTED)break;
        }else ++other_commits;
    }
    gate(purple_commits>0u && other_commits>0u,
         "S19_dynamic_receipt_promotes_real_purple_only");
    gate(r->physical.object.velocity.x>0.f &&
         r->physical.world.history.raw_samples>0,
         "S20_purple_contact_updates_bodies_and_history");
    gate(nf18a6_unbind_world_tick(w,r),"S21_dynamic_unbind");
    gate(setup(w,r,true),"S22_second_actor_guard_fixture");
    nf_world_spawn_actor_with_id(w,77u,NF_FACTION_RIVAL,(NfVec3){99,0,99});
    gate(!nf18a6_bind_world_tick(w,r,NULL,NULL,NULL),
         "S23_single_actor_owner_rejects_stranded_far_actor");
    printf("TOTAL,%u,%u\n",pass,fail);
    free(w);free(r);
    return fail?1:0;
}
