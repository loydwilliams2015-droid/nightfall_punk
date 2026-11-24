#ifndef NF_RETC_H319_OWNER_FRONTIER_H
#define NF_RETC_H319_OWNER_FRONTIER_H
/* H319/O1 research: A6-owned source snapshot is a prerequisite to spatial
 * planning. This is read/stage only, NEVER a second tick or authorization.
 * The caller MUST NOT treat these results as physical contact witnesses. */
#include "nf_retc_h319_a5_frontier.h"
#include "nf_embody18a6.h"
#include <limits.h>

typedef struct NfRetcH319OwnerFrontier {
    uint64_t source_tick;
    uint32_t world_revision, material_epoch, owner_actor;
    NfRetcH319A5Frontier material;
} NfRetcH319OwnerFrontier;

static inline bool nf_retc_h319_owner_frontier_stage(
     const NfWorld *world,Nf18a6Runtime *owner,
     const NfRetcH319Region *regions,const NfVec3 *samples,
     size_t count,uint32_t budget,NfRetcH319OwnerFrontier *out){
    if(!world||!owner||!out||!world->tick||
       world->tick>UINT32_MAX||world->tick_in_progress||
       !world->tick_owner||world->tick_owner_context!=owner||
       !owner->actor_id||owner->last_commit_tick!=world->tick||
       owner->physical.world.tick!=world->tick||
       !owner->physical.world.revision)return false;
    /* Owner must correspond to an actual live actor in this world snapshot. */
    bool source_actor=false;
    for(size_t i=0;i<NF_MAX_ENTITIES;++i)
       if(world->actors[i].id==owner->actor_id &&
          world->actors[i].active && world->actors[i].combat.alive){
          source_actor=true;break;
       }
    if(!source_actor)return false;
    Nf18a5Grid *grid=&owner->physical.world.grid;
    const uint32_t epoch=grid->global_revision;
    if(!epoch)return false;
    NfRetcH319OwnerFrontier staged={0};
    if(!nf_retc_h319_a5_frontier(grid,(uint32_t)world->tick,epoch,
        regions,samples,count,budget,&staged.material))return false;
    if(world->tick!=staged.material.tick ||
       world->tick_owner_context!=owner||world->tick_in_progress||
       owner->last_commit_tick!=world->tick||
       owner->physical.world.tick!=world->tick||
       owner->physical.world.grid.global_revision!=epoch)return false;
    staged.source_tick=world->tick;
    staged.world_revision=owner->physical.world.revision;
    staged.material_epoch=epoch;
    staged.owner_actor=owner->actor_id;
    *out=staged;
    return true;
}
#endif
