#ifndef NF_RETC_MA03_PHYSICS_STAGE_H
#define NF_RETC_MA03_PHYSICS_STAGE_H
/* H157-H161: bounded REAL world-roster, A6-owner-bound two-actor A4 solver.
 * This is a PRIVATE physical candidate, not A5 integrated histories,
 * B2/B3 realization, published NfRetcFrame, or durable transaction.
 * Equal-mass upright capsules, horizontal collinear reference only. */
#include "nf_multi_ma03c.h"
#include <math.h>
#include <string.h>
typedef struct NfRetcMa03PhysicalCandidate {
 NfMa03Roster source;
 NfMa01c2Result solved;
 NfMa03cEvidence reference;
 uint64_t source_tick;
 uint32_t owner_actor;
 bool eligible;
} NfRetcMa03PhysicalCandidate;
static inline bool nf_retc_ma03_stage_physics(
 const NfWorld *world,const Nf18a6Runtime *owner,float dt,
 NfRetcMa03PhysicalCandidate *out){
 if(!out)return false;
 *out=(NfRetcMa03PhysicalCandidate){0};
 if(!world||!owner||world->tick_in_progress||
    world->tick_owner_context!=owner||world->tick_owner==NULL||
    world->tick>=UINT32_MAX||!isfinite(dt)||dt<=0.f||
    owner->last_commit_tick!=world->tick||
    owner->physical.world.tick!=world->tick)return false;
 NfRetcMa03PhysicalCandidate c={0};
 if(!nf_ma03_roster_capture(world,&c.source)||
    c.source.count!=2u||c.source.source_tick!=world->tick)return false;
 Nf18a4Body bodies[2]={{0},{0}};
 bool found_owner=false;
 for(size_t i=0;i<2u;++i){
  const NfMa03Participant *p=&c.source.actors[i];
  const NfActor *a=nf_world_find_actor_const(world,p->id);
  /* H304: refuse nonfinite actor/intent sources BEFORE the A4 solver.
   * NaN values can otherwise bypass later equality/tolerance comparisons. */
  if(!a||!a->active||!a->combat.alive||
     !isfinite(p->position.x)||!isfinite(p->position.y)||
     !isfinite(p->position.z)||
     !isfinite(p->velocity.x)||!isfinite(p->velocity.y)||
     !isfinite(p->velocity.z)||
     !isfinite(p->input.forward)||!isfinite(p->input.strafe)||
     !isfinite(p->input.yaw_radians)||
     !isfinite(a->movement.body_height)||a->movement.body_height<=0.f)
   return false;
  if(p->id==owner->actor_id){
   found_owner=true;
   /* A6's own A4 physical actor must match its authoritative world actor. */
   const Nf18a4Body *b=&owner->physical.actor;
   /* H306: reject nonfinite or detached A6 A4-owner fields BEFORE
      fabsf tolerances; include the previously unverified Y velocity.
      The owner must provide the same exact capsule dimensions as the
      actual authoritative actor, not a caller-selected substitute. */
   if(b->id!=p->id||
      !isfinite(b->center.x)||!isfinite(b->center.y)||
      !isfinite(b->center.z)||
      !isfinite(b->velocity.x)||!isfinite(b->velocity.y)||
      !isfinite(b->velocity.z)||
      !isfinite(b->omega.x)||!isfinite(b->omega.y)||
      !isfinite(b->omega.z)||
      !isfinite(b->radius)||b->radius<=0.f||
      !isfinite(b->height)||b->height<=0.f||
      !isfinite(b->inverse_mass)||b->inverse_mass<0.f||
      !isfinite(b->inverse_inertia.x)||
      !isfinite(b->inverse_inertia.y)||
      !isfinite(b->inverse_inertia.z)||
      fabsf(b->height-a->movement.body_height)>.0001f||
      fabsf(b->center.x-p->position.x)>.0001f||
      fabsf(b->center.z-p->position.z)>.0001f||
      fabsf((b->center.y-b->height*.5f)-p->position.y)>.0001f||
      fabsf(b->velocity.x-p->velocity.x)>.0001f||
      fabsf(b->velocity.y-p->velocity.y)>.0001f||
      fabsf(b->velocity.z-p->velocity.z)>.0001f)return false;
  }
  bodies[i].id=p->id;
  bodies[i].center=(NfVec3){p->position.x,
     p->position.y+a->movement.body_height*.5f,p->position.z};
  bodies[i].velocity=p->velocity;
  bodies[i].inverse_mass=1.f/80.f; /* bounded equal-mass research model */
  bodies[i].radius=owner->physical.actor.radius;
  bodies[i].height=a->movement.body_height;
 }
 if(!found_owner||bodies[0].radius<=0.f||
    fabsf(bodies[0].height-bodies[1].height)>.0001f)return false;
 c.solved=nf_ma01c2_integrate(bodies,2u,dt,(uint32_t)world->tick+1u,1u);
 c.reference=nf_ma03c_compare(bodies,2u,dt,(uint32_t)world->tick+1u,1u);
 if(c.solved.status!=NF_NARROW_OK||
    c.solved.count!=2u||c.solved.impacts!=1u||
    c.reference.matched!=1u||c.reference.disagreed||
    c.reference.unresolved)return false;
 c.source_tick=world->tick;
 c.owner_actor=owner->actor_id;
 c.eligible=true;
 *out=c;
 return true;
}
/* H289: the private two-actor contact candidate must be EXACTLY
 * reproducible from the real immutable A6-owned roster and A4 solver.
 * Equality is per-field rather than memcmp over padding bytes.
 * Research-only: recheck does NOT commit, authorize, or publish. */
static inline bool nf_retc_ma03_recheck_physics(
 const NfWorld *world,const Nf18a6Runtime *owner,float dt,
 const NfRetcMa03PhysicalCandidate *expected){
 if(!expected||!expected->eligible)return false;
 NfRetcMa03PhysicalCandidate fresh={0};
 if(!nf_retc_ma03_stage_physics(world,owner,dt,&fresh))return false;
 if(fresh.source_tick!=expected->source_tick||
    fresh.owner_actor!=expected->owner_actor||
    fresh.eligible!=expected->eligible||
    fresh.source.source_tick!=expected->source.source_tick||
    fresh.source.count!=expected->source.count||
    fresh.solved.status!=expected->solved.status||
    fresh.solved.count!=expected->solved.count||
    fresh.solved.impacts!=expected->solved.impacts||
    fresh.reference.matched!=expected->reference.matched||
    fresh.reference.disagreed!=expected->reference.disagreed||
    fresh.reference.unresolved!=expected->reference.unresolved)
    return false;
 for(size_t i=0;i<2u;++i){
  const NfMa03Participant *a=&fresh.source.actors[i];
  const NfMa03Participant *b=&expected->source.actors[i];
  const Nf18a4Body *sa=&fresh.solved.bodies[i];
  const Nf18a4Body *sb=&expected->solved.bodies[i];
  if(a->id!=b->id||
     a->position.x!=b->position.x||a->position.y!=b->position.y||
     a->position.z!=b->position.z||
     a->velocity.x!=b->velocity.x||a->velocity.y!=b->velocity.y||
     a->velocity.z!=b->velocity.z||
     a->input.forward!=b->input.forward||
     a->input.strafe!=b->input.strafe||
     a->input.yaw_radians!=b->input.yaw_radians||
     a->input.jump_pressed!=b->input.jump_pressed||
     a->input.crouch_held!=b->input.crouch_held||
     a->input.sprint_held!=b->input.sprint_held||
     a->input.interact_held!=b->input.interact_held||
     sa->id!=sb->id||
     sa->center.x!=sb->center.x||
     sa->center.y!=sb->center.y||
     sa->center.z!=sb->center.z||
     sa->velocity.x!=sb->velocity.x||
     sa->velocity.y!=sb->velocity.y||
     sa->velocity.z!=sb->velocity.z||
     sa->inverse_mass!=sb->inverse_mass||
     sa->radius!=sb->radius||
     sa->height!=sb->height)
     return false;
 }
 return true;
}
#endif
