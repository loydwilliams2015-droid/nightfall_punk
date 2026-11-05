#ifndef NF_RETC_MULTI_PREFLIGHT_H
#define NF_RETC_MULTI_PREFLIGHT_H
/* H147-H151: read-only admission check against the real A6 owner and
 * last accepted immutable frame. Not a tick lease, transaction or commit.
 * It cannot stage A4/A5 effects, B3 receipts or any journal publication. */
#include "nf_retc_multi_plan.h"
#include "nf_c2_retc_frame.h"
static inline bool nf_retc_multi_a6_preflight(
 const NfWorld *world,const Nf18a6Runtime *owner,
 const Nf18b2EncounterRuntime *enc,const NfRetcPublisher *publisher,
 const NfRetcCanonicalBatch *batch){
 if(!world||!owner||!enc||!publisher||!batch||!batch->prepared||
    world->tick_in_progress||world->tick_owner_context!=owner||
    enc->world!=world||enc->owner!=owner||
    owner->retc_publisher!=publisher||owner->retc_pending||
    !batch->tick||world->tick>=UINT32_MAX||
    batch->tick!=world->tick+1u||
    owner->physical.world.tick!=world->tick||
    !batch->contract_epoch||
    batch->contract_epoch!=enc->registry.contract_epoch)return false;
 const NfRetcFrame *accepted=nf_retc_read(publisher);
 if(!accepted||accepted->epoch!=world->tick||
    accepted->physical.world.tick!=world->tick||
    !accepted->has_b2_contract||!accepted->has_encounter_registry)return false;
 for(size_t i=0;i<2u;++i){
  const Nf18b3Event *e=&batch->events[i];
  if(!nf_world_find_actor_const(world,e->actor)||
     !e->actor||!e->object||!e->action||
     e->tick!=batch->tick||
     e->contract_epoch!=batch->contract_epoch||
     e->kind!=NF18B3_PUSH||e->phase!=NF18B3_PROPOSED||
     e->authorized||e->applied)return false;
 }
 if(batch->events[0].actor==batch->events[1].actor||
    batch->events[0].object!=batch->events[1].object||
    batch->events[0].world_epoch!=batch->events[1].world_epoch||
    batch->events[0].material_epoch!=batch->events[1].material_epoch||
    batch->events[0].actor>batch->events[1].actor||
    batch->events[0].actor!=owner->actor_id)return false;
 return true;
}
#endif
