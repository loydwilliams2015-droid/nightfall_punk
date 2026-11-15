#ifndef NF_RETC_H310_OWNER_JOINT_PROBE_H
#define NF_RETC_H310_OWNER_JOINT_PROBE_H
/* H310: same-world read-only integration probe. No committed capability.
 * A positive B2 result does NOT establish an active A4 coactor contact;
 * this function records exactly the first failed boundary without ever
 * advancing world ticks, authorizing B3, journaling or publishing A6.
 * The source object and actor state are not copied into an authority. */
#include "nf_retc_h307_cross_source_binding.h"
#include <stdlib.h>
typedef enum NfRetcH310ProbeStage {
 NF_RETC_H310_DENIED_B2=0,
 NF_RETC_H310_B2_SOURCE_VALID=1,
 NF_RETC_H310_A4_SOURCE_VALID=2,
 NF_RETC_H310_A5_SOURCE_VALID=3,
 NF_RETC_H310_STATIC_PAIR_FREE=4,
 NF_RETC_H310_STRUCTURE_VALID=5,
 NF_RETC_H310_PRIVATE_A5_CONTACT=6
} NfRetcH310ProbeStage;
typedef struct NfRetcH310ProbeResult {
 NfRetcH310ProbeStage stage;
 uint32_t actor_ids[2], source_tick, material_epoch;
 bool prepared;
} NfRetcH310ProbeResult;
static inline bool nf_retc_h310_owner_joint_probe(
 const NfWorld *world,const Nf18a6Runtime *owner,
 const Nf18b2EncounterRuntime *enc,const NfRetcPublisher *publisher,
 size_t first_index,size_t second_index,float dt,
 const NfRetcCanonicalBatch *batch,NfRetcH310ProbeResult *out)
{
 if(!out)return false;
 *out=(NfRetcH310ProbeResult){0};
 if(!nf_retc_multi_source_sealed_preflight(world,owner,enc,publisher,
           first_index,second_index,batch))return false;
 out->stage=NF_RETC_H310_B2_SOURCE_VALID;
 if(!isfinite(dt)||dt<=0.f)return false;
 NfRetcMa03PhysicalCandidate *p=calloc(1,sizeof *p);
 NfRetcMa03A5Envelope *e=calloc(1,sizeof *e);
 NfRetcMa03PairSweepWitness *s=calloc(1,sizeof *s);
 NfRetcMa03A5ContactStage *c=NULL;
 bool ok=false;
 if(!p||!e||!s)goto done;
 if(!nf_retc_ma03_stage_physics(world,owner,dt,p))goto done;
 out->stage=NF_RETC_H310_A4_SOURCE_VALID;
 if(!nf_retc_ma03_a5_prepare(world,owner,dt,p,e))goto done;
 out->stage=NF_RETC_H310_A5_SOURCE_VALID;
 if(!nf_retc_ma03_pair_swept_clearance(world,owner,dt,p,e,s))goto done;
 out->stage=NF_RETC_H310_STATIC_PAIR_FREE;
 if(!nf_retc_h307_structural_pair_match(batch,p,e,s))goto done;
 out->stage=NF_RETC_H310_STRUCTURE_VALID;
 c=calloc(1,sizeof *c);
 if(!c||!nf_retc_ma03_a5_stage_contact(world,owner,dt,p,e,c))goto done;
 out->stage=NF_RETC_H310_PRIVATE_A5_CONTACT;
 out->actor_ids[0]=p->source.actors[0].id;
 out->actor_ids[1]=p->source.actors[1].id;
 out->source_tick=(uint32_t)p->source_tick;
 out->material_epoch=e->material_epoch;
 out->prepared=true;
 ok=true;
done:
 free(c);free(s);free(e);free(p);
 return ok;
}
#endif
