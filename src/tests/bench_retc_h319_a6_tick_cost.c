/* Headless real A6 dispatch timing; independent fresh single-actor scenes.
 * No renderer, network, contention, or full-world performance claim. */
#define _POSIX_C_SOURCE 200809L
#include "nf_retc_h319_owner_frontier.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#define CK(x) do {if(!(x)){fprintf(stderr,"H319 TICK COST FAIL %d: %s\n",__LINE__,#x);return 1;}}while(0)
enum { SAMPLES=128 };
static uint64_t ns(struct timespec t){
    return (uint64_t)t.tv_sec*UINT64_C(1000000000)+(uint64_t)t.tv_nsec;
}
static int compare(const void *a,const void *b){
    const uint64_t x=*(const uint64_t *)a,y=*(const uint64_t *)b;
    return (x>y)-(x<y);
}
static uint64_t percentile(const uint64_t *sorted,unsigned p){
    return sorted[(SAMPLES*p+99u)/100u-1u];
}
int main(void){
    NfWorld *w=malloc(sizeof *w);
    Nf18a6Runtime *r=malloc(sizeof *r);
    CK(w&&r);
    uint64_t tick_ns[SAMPLES],stage_ns[SAMPLES];
    size_t resident_max=0u;
    unsigned pending_max=0u;
    for(unsigned i=0;i<SAMPLES;++i){
        nf_world_init(w,42u);
        nf_world_add_collider(w,NF_COLLIDER_SOLID,(NfVec3){0,-.5f,0},(NfVec3){4,0,4});
        CK(nf_world_spawn_actor_with_id(w,10u,NF_FACTION_PLAYER,(NfVec3){1,0,1})==10u);
        NfActor *a=nf_world_find_actor(w,10u);CK(a);
        a->movement.grounded=true;a->movement.mode=NF_MOVE_GROUND;
        Nf18a4Body crate={0};
        crate.id=2010u;crate.center=(NfVec3){2.2f,.9f,1.f};
        crate.inverse_mass=1.f/24.f;crate.inverse_inertia=(NfVec3){.3f,.3f,.3f};
        crate.box_half=(NfVec3){.2f,.5f,.35f};
        CK(nf18a6_init(r,w,10u,crate,3u,NF18A6_WORLD_CAMERA));
        Nf18a5Grid *g=&r->physical.world.grid;
        uint8_t vox[NF18A5_CANON_VOXELS]={0};
        CK(nf18a5_load_canonical(g,0,0,0,vox,1u));
        CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
        NfMoveInput in={0};in.strafe=1.f;
        nf_world_set_input(w,10u,in);
        struct timespec t0,t1,t2;
        CK(clock_gettime(CLOCK_MONOTONIC,&t0)==0);
        const NfWorldTickStatus status=nf_world_step_checked(w,1.f/60.f);
        CK(clock_gettime(CLOCK_MONOTONIC,&t1)==0);
        CK(status==NF_WORLD_TICK_COMMITTED&&w->tick==1u);
        const NfRetcH319Region regions[2]={
            {.id=10u,.work_cost=1u,.consequential=1u,.resident=1u},
            {.id=20u,.work_cost=1u,.consequential=1u,.resident=0u}
        };
        const NfVec3 points[2]={{1.5f,.5f,1.5f},{4.5f,.5f,1.5f}};
        NfRetcH319OwnerFrontier plan={0};
        CK(nf_retc_h319_owner_frontier_stage(w,r,w->tick,r->physical.world.revision,
            g->global_revision,regions,points,2u,1u,&plan));
        CK(clock_gettime(CLOCK_MONOTONIC,&t2)==0);
        CK(ns(t1)>=ns(t0)&&ns(t2)>=ns(t1));
        tick_ns[i]=ns(t1)-ns(t0);stage_ns[i]=ns(t2)-ns(t1);
        CK(plan.material.plan.detailed==1u&&plan.material.plan.pending==1u);
        CK(w->tick==1u&&plan.source_tick==1u);
        const size_t bytes=nf18a5_resident_bytes(g);
        if(bytes>resident_max)resident_max=bytes;
        if(plan.material.plan.pending>pending_max)pending_max=plan.material.plan.pending;
        CK(nf18a6_unbind_world_tick(w,r));
    }
    qsort(tick_ns,SAMPLES,sizeof tick_ns[0],compare);
    qsort(stage_ns,SAMPLES,sizeof stage_ns[0],compare);
    printf("H319 A6_COST samples=%u actors=1 committed=%u tick_p50_ns=%" PRIu64
           " tick_p95_ns=%" PRIu64 " tick_p99_ns=%" PRIu64 " tick_max_ns=%" PRIu64
           " stage_p50_ns=%" PRIu64 " stage_p95_ns=%" PRIu64 " stage_p99_ns=%" PRIu64
           " stage_max_ns=%" PRIu64 " max_resident_payload_bytes=%zu max_pending=%u full_game_certified=0\n",
           SAMPLES,SAMPLES,percentile(tick_ns,50),percentile(tick_ns,95),
           percentile(tick_ns,99),tick_ns[SAMPLES-1],percentile(stage_ns,50),
           percentile(stage_ns,95),percentile(stage_ns,99),stage_ns[SAMPLES-1],
           resident_max,pending_max);
    free(r);free(w);return 0;
}
