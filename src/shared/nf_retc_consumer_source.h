#ifndef NF_RETC_CONSUMER_SOURCE_H
#define NF_RETC_CONSUMER_SOURCE_H
/* H296: bind a read-only cross-consumer projection to the actual leased
 * immutable RET-C A6/B2/B3 frame, without inventing constitutional fields.
 * NOT a new publisher, B2 grant, real MAP/AUDIO/RENDER device, or C3 journal.
 * Failures preserve the caller's last accepted projection. */
#include "nf_c2_retc_frame.h"
#include <stdbool.h>
#include <stdint.h>
typedef struct NfRetcSourceObservation {
 uint32_t tick,transaction_id,owner_actor,contract_epoch;
} NfRetcSourceObservation;
static inline bool nf_retc_consumer_source_validate(
 const NfRetcReadLease *lease,const NfRetcReadView *view,
 NfRetcSourceObservation *out)
{
 if(!lease||!view||!out||!lease->accepted||!lease->publisher||
    view->accepted!=lease->accepted)return false;
 const NfRetcFrame *f=lease->accepted;
 /* Do not dereference view pointers until proving their identity as
    subobjects of the A6-owned accepted frame under the active lease. */
 if(view->physical!=&f->physical||view->event!=&f->event||
    view->contract!=&f->encounter_registry||
    view->eligibility!=&f->b2_use||
    !f->has_b2_contract||!f->has_encounter_registry||
    !f->epoch||!f->transaction_id||!f->owner_actor||
    view->epoch!=f->epoch||
    view->transaction_id!=f->transaction_id||
    f->event.tick!=f->epoch||
    f->physical.world.tick!=f->epoch||
    f->b2_use.tick!=f->epoch||
    f->encounter_registry.contract_epoch!=f->event.contract_epoch)
  return false;
 *out=(NfRetcSourceObservation){
  f->epoch,f->transaction_id,f->owner_actor,f->event.contract_epoch};
 return true;
}
#endif
