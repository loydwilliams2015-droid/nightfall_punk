#ifndef NF_RETC_H307_CROSS_SOURCE_BINDING_H
#define NF_RETC_H307_CROSS_SOURCE_BINDING_H
/* H307: STRICTLY STRUCTURAL two-event A4/A5/B3 proposal lineage check.
 * A match is necessary but NOT SUFFICIENT for real B2 authorization, live
 * A6 staging, two typed B3 effects, WAL or publication. Inputs can be
 * caller-supplied; only an independent owner source seal can grant trust.
 * Never use this function as an authorization or commit predicate alone. */
#include "nf_retc_multi_source_seal.h"
#include "nf_retc_ma03_a5_sweep_pair.h"
static inline bool nf_retc_h307_structural_pair_match(
 const NfRetcCanonicalBatch *batch,
 const NfRetcMa03PhysicalCandidate *physical,
 const NfRetcMa03A5Envelope *handoff,
 const NfRetcMa03PairSweepWitness *sweep)
{
 if(!batch||!physical||!handoff||!sweep||
    !batch->prepared||!physical->eligible||!handoff->prepared||
    !sweep->prepared||!batch->tick||!batch->contract_epoch||
    !handoff->material_epoch||handoff->source_tick==UINT32_MAX||
    physical->source_tick!=handoff->source_tick||
    physical->source.source_tick!=physical->source_tick||
    physical->source.count!=2u||physical->solved.count!=2u||
    physical->solved.impacts!=1u||
    physical->owner_actor!=handoff->owner_actor||
    sweep->tick!=batch->tick||
    sweep->tick!=handoff->source_tick+1u||
    sweep->material_epoch!=handoff->material_epoch)
   return false;
 /* H308: require an actual accepted two-body A4 receipt with the
    SAME tick/revision as the A5 material handoff. The existing
    structural pair alone does not prove physical contact ancestry.
    These checks are necessary evidence, never B2 authorization. */
 const Nf18a4Receipt *contact=&physical->solved.receipts[0];
 if(contact->tick!=batch->tick||
    contact->world_version!=handoff->source_revision||
    contact->status!=NF18A4_ACCEPTED||
    !contact->impulses_applied||!contact->contact_id||
    !isfinite(contact->normal_impulse.x)||
    !isfinite(contact->normal_impulse.y)||
    !isfinite(contact->normal_impulse.z)||
    !isfinite(contact->tangent_impulse.x)||
    !isfinite(contact->tangent_impulse.y)||
    !isfinite(contact->tangent_impulse.z)||
    !isfinite(contact->signed_energy_delta))return false;
 if(batch->events[0].actor==batch->events[1].actor||
    batch->events[0].object!=batch->events[1].object||
    !batch->events[0].object||!batch->events[0].action||
    !batch->events[1].action||
    batch->events[0].actor!=physical->owner_actor)
   return false;
 for(size_t i=0;i<2u;++i){
  const Nf18b3Event *e=&batch->events[i];
  const uint32_t actor=physical->source.actors[i].id;
  if(!actor||e->actor!=actor||sweep->actor_ids[i]!=actor||
     physical->solved.bodies[i].id!=actor||
     handoff->bodies[i].id!=actor||
     e->tick!=batch->tick||
     e->world_epoch!=handoff->source_revision||
     e->material_epoch!=handoff->material_epoch||
     e->contract_epoch!=batch->contract_epoch||
     e->kind!=NF18B3_PUSH||
     e->phase!=NF18B3_PROPOSED||
     e->authorized||e->applied||
     sweep->query[i]!=NF18A5_Q_FREE)return false;
 }
 return true;
}
#endif
