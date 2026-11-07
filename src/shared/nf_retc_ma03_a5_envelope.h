#ifndef NF_RETC_MA03_A5_ENVELOPE_H
#define NF_RETC_MA03_A5_ENVELOPE_H
/* H168-H172: source-exact A5 handoff witness for two already SOLVED A4 bodies.
 * This is NOT a second Nf18a5Integrated actor, a material-history effect,
 * a B3 receipt, or an A6 accepted frame. A real two-actor A5 transaction
 * and single durable A6 commit remain independently required. */
#include "nf_retc_ma03_physics_stage.h"
typedef struct NfRetcMa03A5Envelope {
 Nf18a4Body bodies[2];
 uint32_t source_tick,source_revision,material_epoch;
 uint32_t raw_samples,generated_events;
 uint32_t owner_actor;
 bool prepared;
} NfRetcMa03A5Envelope;
static inline bool nf_retc_ma03_a5_prepare(
 const NfWorld *w,const Nf18a6Runtime *owner,
 float dt,const NfRetcMa03PhysicalCandidate *physical,
 NfRetcMa03A5Envelope *out){
 if(!out)return false;
 *out=(NfRetcMa03A5Envelope){0};
 if(!w||!owner||!physical||!physical->eligible||
    w->tick>=UINT32_MAX||
    physical->source_tick!=w->tick||
    physical->owner_actor!=owner->actor_id||
    owner->physical.world.tick!=w->tick||
    !owner->physical.world.revision||
    !nf_retc_ma03_recheck_physics(w,owner,dt,physical))
   return false;
 NfRetcMa03A5Envelope e={0};
 for(size_t i=0;i<2u;++i){
  if(physical->solved.bodies[i].id!=physical->source.actors[i].id)
   return false;
  e.bodies[i]=physical->solved.bodies[i];
 }
 e.source_tick=(uint32_t)w->tick;
 e.source_revision=owner->physical.world.revision;
 e.material_epoch=owner->physical.world.grid.global_revision;
 e.raw_samples=owner->physical.world.history.raw_samples;
 e.generated_events=owner->physical.world.history.generated_events;
 e.owner_actor=owner->actor_id;
 e.prepared=true;
 *out=e;
 return true;
}
static inline bool nf_retc_ma03_a5_recheck(
 const NfWorld *w,const Nf18a6Runtime *owner,float dt,
 const NfRetcMa03PhysicalCandidate *physical,
 const NfRetcMa03A5Envelope *expected){
 if(!expected||!expected->prepared)return false;
 NfRetcMa03A5Envelope current={0};
 if(!nf_retc_ma03_a5_prepare(w,owner,dt,physical,&current)||
    expected->source_tick!=current.source_tick||
    expected->source_revision!=current.source_revision||
    expected->material_epoch!=current.material_epoch||
    expected->raw_samples!=current.raw_samples||
    expected->generated_events!=current.generated_events||
    expected->owner_actor!=current.owner_actor)return false;
 for(size_t i=0;i<2u;++i){
  const Nf18a4Body *a=&expected->bodies[i],*b=&current.bodies[i];
  if(a->id!=b->id||
     a->center.x!=b->center.x||
     a->center.y!=b->center.y||
     a->center.z!=b->center.z||
     a->velocity.x!=b->velocity.x||
     a->velocity.y!=b->velocity.y||
     a->velocity.z!=b->velocity.z)return false;
 }
 return true;
}
#endif
