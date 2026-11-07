#ifndef NF_RETC_MA03_A5_CONTACT_STAGE_H
#define NF_RETC_MA03_A5_CONTACT_STAGE_H
/* H173-H177: two REAL A4 bodies and their ACTUAL reciprocal collision
 * receipt feed a PRIVATE copy of the owner A5 world/history.
 * This is a single CONTACT shared by two actors, not two independent
 * contractual B3 realizations. Nothing here journals or publishes A6.
 * A5 static swept-volume clearance, joint B2/B3, WAL still remain open. */
#include "nf_retc_ma03_a5_envelope.h"
#include "nf_contact18a5_close.h"
typedef struct NfRetcMa03A5ContactStage {
 Nf18a5World candidate;
 Nf18a5Sample contact;
 Nf18a4Body bodies[2];
 uint32_t source_tick,source_revision,material_epoch;
 bool staged;
} NfRetcMa03A5ContactStage;
static inline bool nf_retc_ma03_a5_stage_contact(
 const NfWorld *world,const Nf18a6Runtime *owner,float dt,
 const NfRetcMa03PhysicalCandidate *physical,
 const NfRetcMa03A5Envelope *handoff,
 NfRetcMa03A5ContactStage *out){
 if(!out)return false;
 out->staged=false;
 if(!world||!owner||!physical||!handoff||
    !nf_retc_ma03_a5_recheck(world,owner,dt,physical,handoff)||
    physical->solved.impacts!=1u||
    handoff->source_tick==UINT32_MAX||
    !handoff->material_epoch)return false;
 const Nf18a4Receipt *receipt=&physical->solved.receipts[0];
 if(receipt->tick!=handoff->source_tick+1u||
    receipt->world_version!=handoff->source_revision||
    receipt->status!=NF18A4_ACCEPTED||
    !receipt->impulses_applied||
    !receipt->contact_id)return false;
 NfRetcMa03A5ContactStage next={0};
 if(!nf18a5_receipt_sample(receipt,handoff->bodies[0].id,
                              handoff->bodies[1].id,0.f,
                              handoff->material_epoch,&next.contact))
   return false;
 next.candidate=owner->physical.world;
 if(!nf18a5_world_commit(&next.candidate,handoff->source_revision,
                         receipt->tick,handoff->owner_actor,
                         &next.contact,1u))
   return false;
 if(next.candidate.tick!=receipt->tick||
    next.candidate.revision!=handoff->source_revision+1u||
    next.candidate.history.raw_samples!=handoff->raw_samples+1u||
    next.candidate.grid.global_revision!=handoff->material_epoch)return false;
 next.bodies[0]=handoff->bodies[0];
 next.bodies[1]=handoff->bodies[1];
 next.source_tick=handoff->source_tick;
 next.source_revision=handoff->source_revision;
 next.material_epoch=handoff->material_epoch;
 next.staged=true;
 *out=next;
 return true;
}
#endif
