#ifndef NF_RETC_M2_REAL_CONTRACT_AUDIT_H
#define NF_RETC_M2_REAL_CONTRACT_AUDIT_H
/* H276B: after the REAL A6 joint provider has issued, realized and
 * published a typed B2/B3 PUSH, audit its source identities, physical
 * projection, and encounter ledger together. Pure read-only validator:
 * does not issue authority, publish a frame, or integrate an H275F
 * off-axis candidate into the production tick. */
#include "nf_retc_joint_provider.h"
#include <math.h>
#include <string.h>
static inline bool nf_retc_m2_real_contract_audit(
 const NfWorld *w,const Nf18a6Runtime *owner,
 const Nf18b2EncounterRuntime *enc,const Nf18b6Push *push,
 const NfRetcFrame *f){
 if(!w||!owner||!enc||!push||!f||enc->world!=w||
    enc->owner!=owner||push->runtime!=owner||
    !f->has_b2_contract||!f->has_encounter_registry||
    f->epoch==0u||f->epoch!=w->tick||
    f->epoch!=owner->last_commit_tick||
    f->epoch!=owner->joint_commit_epoch||
    f->physical.world.tick!=f->epoch||
    f->physical.world.tick!=owner->physical.world.tick||
    f->physical.world.revision!=owner->physical.world.revision||
    f->owner_actor!=owner->actor_id||
    f->object_id!=owner->physical.object.id||
    f->physical.actor.id!=owner->actor_id||
    f->physical.object.id!=owner->physical.object.id||
    f->event.kind!=NF18B3_PUSH||
    f->event.phase!=NF18B3_APPLIED||
    !f->event.authorized||!f->event.applied||
    f->event.actor!=owner->actor_id||
    f->event.object!=owner->physical.object.id||
    f->event.tick!=f->epoch||
    f->b2_use.actor_id!=f->event.actor||
    f->b2_use.object_id!=f->event.object||
    f->b2_use.action_id!=f->event.action||
    f->b2_use.tick!=f->epoch||
    f->b2_decision.status!=NF18B2_ELIGIBLE||
    f->b2_decision.physical!=NF18B2_ELIGIBLE||
    f->b2_decision.contract!=NF18B2_ELIGIBLE||
    f->event.contract_epoch!=f->encounter_registry.contract_epoch||
    memcmp(&enc->registry,&f->encounter_registry,
           sizeof f->encounter_registry)!=0||
    push->event.action!=f->event.action||
    push->event.tick!=f->epoch||
    push->published_epoch!=f->epoch||
    !push->receipt_count||
    push->receipts[push->receipt_count-1u].actor!=f->event.actor||
    push->receipts[push->receipt_count-1u].action!=f->event.action||
    push->receipts[push->receipt_count-1u].tick!=f->epoch||
    owner->retc_pending!=NULL)return false;
 const NfActor *actor=nf_world_find_actor_const(w,owner->actor_id);
 if(!actor||!actor->active||!actor->combat.alive||
    owner->object_collider_index>=w->collider_count)return false;
 const NfCollider *mirror=&w->colliders[owner->object_collider_index];
 if(mirror->kind!=NF_COLLIDER_SOLID||
    fabsf(actor->transform.position.x-f->physical.actor.center.x)>.0001f||
    fabsf(actor->transform.position.z-f->physical.actor.center.z)>.0001f||
    fabsf(mirror->min.x-(f->physical.object.center.x-
                          f->physical.object.box_half.x))>.0001f||
    fabsf(mirror->max.x-(f->physical.object.center.x+
                          f->physical.object.box_half.x))>.0001f||
    /* H282: the same immutable crate projection must account for Z
       as well as X. No B2/B3 receipt can validate a physically
       impossible off-axis accepted-frame collider. */
    fabsf(mirror->min.z-(f->physical.object.center.z-
                          f->physical.object.box_half.z))>.0001f||
    fabsf(mirror->max.z-(f->physical.object.center.z+
                          f->physical.object.box_half.z))>.0001f)return false;
 return true;
}
#endif
