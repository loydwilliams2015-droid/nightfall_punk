#include "nf_v18c2_witness.h"
#include <stdlib.h>
NfC2WitnessStatus nf_c2_witness(const NfWorld *w,
 const Nf18a6Runtime *r,NfMa03Provider provider,void *context,
 uint32_t action,NfVec3 probe,uint32_t epoch){
 if(!w||!r||!provider||!action||w->tick_in_progress||
    w->tick_owner_context!=r||!w->tick_owner||
    w->tick!=r->last_commit_tick||r->physical.world.tick!=w->tick||
    epoch!=r->physical.world.grid.global_revision)return NF_C2W_STALE;
 /* Copy the actual A5 grid: query may change residency bookkeeping.
  * The authoritative material state remains untouched by this probe. */
 Nf18a5Grid *snapshot=malloc(sizeof *snapshot);
 if(!snapshot)return NF_C2W_PENDING;
 *snapshot=r->physical.world.grid;
 Nf18a5Query material=nf18a5_query(snapshot,probe.x,probe.y,probe.z,
                                    (uint32_t)w->tick+1u,true);
 free(snapshot);
 if(material==NF18A5_Q_PENDING)return NF_C2W_PENDING;
 if(material!=NF18A5_Q_FREE)return NF_C2W_DENIED;
 Nf18b2UseDecision decision=nf_ma03_b2_owner_evaluate(w,r,provider,
                                 context,action,NF18B2_M4_MATERIAL_FIRST);
 if(decision.status==NF18B2_ELIGIBLE)return NF_C2W_READY;
 if(decision.status==NF18B2_PENDING)return NF_C2W_PENDING;
 if(decision.status==NF18B2_STALE)return NF_C2W_STALE;
 return NF_C2W_DENIED;
}
