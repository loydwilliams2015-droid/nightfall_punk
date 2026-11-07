#ifndef NF_RETC_M2_PAIR_TICK_H
#define NF_RETC_M2_PAIR_TICK_H
/* M2.2 H198-H202: one real actor and one A6-owned dynamic object,
 * staged with existing A4 CCD + private A5 closure in an actual owner tick.
 * This candidate is intentionally independent of the M2.1 two-actor
 * contact. It cannot publish A6, B3, WAL or network effects. */
#include "nf_retc_ma03_owner_tick.h"
#include <math.h>
typedef struct NfRetcM2PairEvidence {
 Nf18a5Integrated candidate;
 Nf18a5CloseResult result;
 uint64_t source_tick;
 uint32_t source_revision;
 bool staged;
} NfRetcM2PairEvidence;
static inline bool nf_retc_m2_pair_stage_in_tick(
 const NfWorld *world,const Nf18a6Runtime *owner,float dt,
 NfRetcM2PairEvidence *out){
 if(!out)return false;
 out->staged=false;
 if(!world||!owner||!world->tick_in_progress||
    world->tick_owner_context!=owner||!world->tick_owner||
    !owner->require_pair_research||owner->require_dual_contact_preflight||
    !isfinite(dt)||dt<=0.f||world->tick>=UINT32_MAX||
    owner->last_commit_tick!=world->tick||
    owner->physical.world.tick!=world->tick||
    owner->physical.world.revision==UINT32_MAX)return false;
 const NfActor *actor=nf_world_find_actor_const(world,owner->actor_id);
 if(!actor||!actor->active||!actor->combat.alive||
    actor->id!=owner->physical.actor.id||
    fabsf(actor->transform.position.x-owner->physical.actor.center.x)>.0001f||
    fabsf(actor->transform.position.z-owner->physical.actor.center.z)>.0001f||
    fabsf(actor->transform.velocity.x-owner->physical.actor.velocity.x)>.0001f||
    fabsf(actor->transform.velocity.z-owner->physical.actor.velocity.z)>.0001f||
    fabsf(actor->movement.body_height-owner->physical.actor.height)>.0001f)
   return false;
 NfRetcM2PairEvidence next={0};
 next.candidate=owner->physical;
 Nf18a4Body a=next.candidate.actor;
 Nf18a2ShapePolicy shape={a.radius,a.height,0.05f,0.4f,0.65f};
 Nf18a4Motor motor={a.velocity,0.f,0.f};
 NfVec3 probe={a.center.x,a.center.y-a.height*.5f,a.center.z};
 const uint32_t revision=owner->physical.world.revision;
 next.result=nf18a5_integrated_pair_stage_checked(
     &next.candidate,revision,(uint32_t)world->tick+1u,
     probe,NULL,NULL,shape,motor,dt,0.f,0.f,NULL,NULL);
 if(next.result.status!=NF18A5_CLOSE_COMMITTED||
    !next.result.dynamic_contact||
    next.result.applied.status!=NF18A4_ACCEPTED||
    !next.result.applied.impulses_applied||
    next.candidate.actor.id!=owner->actor_id||
    next.candidate.object.id!=owner->physical.object.id||
    next.candidate.committed_receipts!=owner->physical.committed_receipts+1u||
    next.candidate.world.history.raw_samples!=
       owner->physical.world.history.raw_samples+1u||
    next.candidate.world.revision!=revision+1u||
    next.candidate.world.tick!=world->tick+1u)return false;
 next.source_tick=world->tick;
 next.source_revision=revision;
 next.staged=true;
 *out=next;
 return true;
}
#endif
