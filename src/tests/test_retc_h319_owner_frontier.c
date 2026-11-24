#include "nf_retc_h319_owner_frontier.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"H319 OWNER_FRONTIER FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static NfWorldTickStatus owner_stub(NfWorld *w,float dt,void *ctx) {
    (void)w;(void)dt;(void)ctx;return NF_WORLD_TICK_REJECTED;
}
int main(void) {
    NfWorld *world=calloc(1,sizeof *world);
    Nf18a6Runtime *owner=calloc(1,sizeof *owner);
    CK(world&&owner);
    world->tick=17u;world->tick_owner=owner_stub;
    world->tick_owner_context=owner;
    world->actors[0].id=10u;world->actors[0].active=true;
    world->actors[0].combat.alive=true;
    owner->actor_id=10u;owner->last_commit_tick=17u;
    owner->physical.world.tick=17u;
    owner->physical.world.revision=4u;
    Nf18a5Grid *grid=&owner->physical.world.grid;
    nf18a5_grid_init(grid,3u,NF18A5_SELECTIVE_FINE);
    uint8_t vox[NF18A5_CANON_VOXELS]={0};
    CK(nf18a5_load_canonical(grid,0,0,0,vox,17u));
    memset(vox,NF18A5_MIXED,sizeof vox);
    CK(nf18a5_load_canonical(grid,1,0,0,vox,17u));
    memset(vox,NF18A5_SOLID,sizeof vox);
    CK(nf18a5_load_canonical(grid,2,0,0,vox,17u));
    const NfRetcH319Region regions[4]={
       {.id=10u,.work_cost=2u,.consequential=1u,.observed=1u,.resident=1u},
       {.id=20u,.work_cost=2u,.consequential=1u,.resident=1u},
       {.id=30u,.work_cost=2u,.observed=1u,.resident=1u},
       {.id=40u,.work_cost=2u,.resident=1u}};
    const NfVec3 samples[4]={
       {.5f,.5f,.5f},{4.5f,.5f,.5f},{8.5f,.5f,.5f},
       {12.5f,.5f,.5f}};
    NfRetcH319OwnerFrontier out={0};
    unsigned sweeps=0u;
    for(uint32_t budget=0;budget<32u;++budget){
        CK(nf_retc_h319_owner_frontier_stage(world,owner,regions,samples,
             4u,budget,&out));
        CK(out.source_tick==17u&&out.owner_actor==10u);
        CK(out.world_revision==4u&&out.material_epoch==grid->global_revision);
        CK(out.material.plan.spent<=budget);
        CK(out.material.plan.detailed+out.material.plan.pending+
           out.material.plan.summarized+out.material.plan.dormant==4u);
        ++sweeps;
    }
    CK(nf_retc_h319_owner_frontier_stage(world,owner,regions,samples,4u,6u,&out));
    CK(out.material.plan.detailed==1u);
    CK(out.material.plan.pending==1u);
    CK(out.material.plan.summarized==1u);
    CK(out.material.plan.dormant==1u);
    const NfRetcH319OwnerFrontier snapshot=out;
    #define REJECT(cmd) do {cmd;CK(!nf_retc_h319_owner_frontier_stage(world,owner,regions,samples,4u,6u,&out));CK(memcmp(&out,&snapshot,sizeof out)==0);}while(0)
    REJECT(world->tick_owner_context=NULL);
    world->tick_owner_context=owner;
    REJECT(world->tick_in_progress=true);
    world->tick_in_progress=false;
    REJECT(owner->last_commit_tick=16u);
    owner->last_commit_tick=17u;
    REJECT(owner->physical.world.tick=16u);
    owner->physical.world.tick=17u;
    REJECT(owner->actor_id=99u);
    owner->actor_id=10u;
    REJECT(world->actors[0].combat.alive=false);
    world->actors[0].combat.alive=true;
    REJECT(world->tick=18u);
    world->tick=17u;
    REJECT(world->tick_owner=NULL);
    world->tick_owner=owner_stub;
    REJECT(owner->physical.world.revision=0u);
    owner->physical.world.revision=4u;
    CK(nf_retc_h319_owner_frontier_stage(world,owner,regions,samples,4u,6u,&out));
    CK(out.source_tick==17u&&out.material_epoch==snapshot.material_epoch);
    printf("H319 OWNER_FRONTIER_COMPLETE budget_sweeps=%u invalid_owner_controls=9 source_A6_snapshot=1 source_A5=1 actor_verified=1 publication=0\n",sweeps);
    free(owner);free(world);return 0;
}
