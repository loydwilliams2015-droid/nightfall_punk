#ifndef NF_RETC_MULTI_SOURCE_SEAL_H
#define NF_RETC_MULTI_SOURCE_SEAL_H
/* H303 M2: source-authenticated, READ-ONLY TWO-B2 proposal admission.
 * The existing A6 preflight checks internal consistency, but is not a
 * substitute for recomputing both live B2 grants and the canonical batch.
 * Never publishes A6, realizes B3, or grants active coactor movement. */
#include "nf_retc_multi_physical.h"
static inline bool nf_retc_multi_source_sealed_preflight(
 const NfWorld *world,const Nf18a6Runtime *owner,
 const Nf18b2EncounterRuntime *enc,const NfRetcPublisher *publisher,
 size_t first_index,size_t second_index,const NfRetcCanonicalBatch *batch)
{
 if(!batch||!nf_retc_multi_physical_preflight(world,owner,enc,
                                               publisher,batch))
  return false;
 NfRetcMultiEventProposal fresh_proposal={0};
 NfRetcCanonicalBatch canonical={0};
 if(!nf_retc_multi_propose_b3(enc,first_index,second_index,&fresh_proposal)||
    !nf_retc_multi_canonical_batch(enc,first_index,second_index,
                                   &fresh_proposal,&canonical)||
    !canonical.prepared||!batch->prepared||
    canonical.tick!=batch->tick||
    canonical.contract_epoch!=batch->contract_epoch)
  return false;
 for(size_t i=0;i<2u;++i){
  const Nf18b3Event *a=&batch->events[i],*b=&canonical.events[i];
  if(a->actor!=b->actor||a->object!=b->object||
     a->action!=b->action||a->tick!=b->tick||
     a->world_epoch!=b->world_epoch||
     a->material_epoch!=b->material_epoch||
     a->contract_epoch!=b->contract_epoch||
     a->kind!=b->kind||a->phase!=b->phase||
     a->authorized!=b->authorized||a->applied!=b->applied)
    return false;
 }
 return true;
}
#endif
