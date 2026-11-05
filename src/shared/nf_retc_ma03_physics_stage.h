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
  if(!a||!a->active||!a->combat.alive||
     !isfinite(a->movement.body_height)||a->movement.body_height<=0.f)
   return false;
  if(p->id==owner->actor_id){
   found_owner=true;
   /* A6's own A4 physical actor must match its authoritative world actor. */
   const Nf18a4Body *b=&owner->physical.actor;
   if(b->id!=p->id||
      fabsf(b->center.x-p->position.x)>.0001f||
      fabsf(b->center.z-p->position.z)>.0001f||
      fabsf((b->center.y-b->height*.5f)-p->position.y)>.0001f||
      fabsf(b->velocity.x-p->velocity.x)>.0001f||
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
static inline bool nf_retc_ma03_recheck_physics(
 const NfWorld *world,const Nf18a6Runtime *owner,float dt,
 const NfRetcMa03PhysicalCandidate *expected){
 if(!expected||!expected->eligible)return false;
 NfRetcMa03PhysicalCandidate fresh={0};
 if(!nf_retc_ma03_stage_physics(world,owner,dt,&fresh))return false;
 if(fresh.source_tick!=expected->source_tick||
    fresh.owner_actor!=expected->owner_actor||
    fresh.source.count!=expected->source.count||
    fresh.solved.count!=expected->solved.count||
    fresh.solved.impacts!=expected->solved.impacts)return false;
 for(size_t i=0;i<2u;++i){
  const NfMa03Participant *a=&fresh.source.actors[i];
  const NfMa03Participant *b=&expected->source.actors[i];
  if(a->id!=b->id||
     a->position.x!=b->position.x||a->position.y!=b->position.y||
     a->position.z!=b->position.z||
     a->velocity.x!=b->velocity.x||a->velocity.y!=b->velocity.y||
     a->velocity.z!=b->velocity.z||
     a->input.forward!=b->input.forward||
     a->input.strafe!=b->input.strafe||
     fresh.solved.bodies[i].velocity.x!=expected->solved.bodies[i].velocity.x||
     fresh.solved.bodies[i].center.x!=expected->solved.bodies[i].center.x)
     return false;
 }
 return true;
}
#endif
