#ifndef NF_RETC_MULTI_PHYSICAL_H
#define NF_RETC_MULTI_PHYSICAL_H
/* H152-H156: read-only MA03 roster and A5 owner physical witness.
 * This refuses active coactor motion: it is not a dual-actor solver,
 * A6 tick lease, B3 realization, journal, or commit permission. */
#include "nf_retc_multi_preflight.h"
#include "nf_multi_ma03.h"
#include <math.h>
static inline bool nf_retc_multi_physical_preflight(
 const NfWorld *world,const Nf18a6Runtime *owner,
 const Nf18b2EncounterRuntime *enc,const NfRetcPublisher *publisher,
 const NfRetcCanonicalBatch *batch){
 if(!nf_retc_multi_a6_preflight(world,owner,enc,publisher,batch))
  return false;
 if(enc->frame.scene.actor_count!=2u||
    enc->frame.scene.object_count!=1u||
    !enc->frame.scene.actors||!enc->frame.scene.objects||
    owner->physical.object.id!=batch->events[0].object||
    enc->frame.scene.objects[0].id!=owner->physical.object.id||
    enc->frame.scene.material!=&owner->physical.world)
  return false;
 NfMa03Roster roster={0};
 if(!nf_ma03_roster_capture(world,&roster)||roster.count!=2u||
    roster.source_tick!=world->tick)return false;
 for(size_t i=0;i<2u;++i){
  const uint32_t id=batch->events[i].actor;
  if(roster.actors[i].id!=id)return false;
  const Nf18b2Actor *b2=NULL;
  for(size_t j=0;j<enc->frame.scene.actor_count;++j)
   if(enc->frame.scene.actors[j].id==id)
    b2=&enc->frame.scene.actors[j];
  if(!b2||!isfinite(b2->feet.x)||!isfinite(b2->feet.y)||
     !isfinite(b2->feet.z)||
     fabsf(roster.actors[i].position.x-b2->feet.x)>0.0001f||
     fabsf(roster.actors[i].position.y-b2->feet.y)>0.0001f||
     fabsf(roster.actors[i].position.z-b2->feet.z)>0.0001f)
   return false;
  /* Only the sole A6 owner has physical tick authority today. Coactors
     must be passive until real A4/A5 multi-body staging exists. */
  if(id!=owner->actor_id&&
     (fabsf(roster.actors[i].velocity.x)>0.0001f||
      fabsf(roster.actors[i].velocity.y)>0.0001f||
      fabsf(roster.actors[i].velocity.z)>0.0001f||
      fabsf(roster.actors[i].input.forward)>0.0001f||
      fabsf(roster.actors[i].input.strafe)>0.0001f))
   return false;
 }
 return true;
}
#endif
