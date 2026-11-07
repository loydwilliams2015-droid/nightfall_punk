#ifndef NF_RETC_M2_PUSH_TICK_H
#define NF_RETC_M2_PUSH_TICK_H
/* H208-H212: research-only A6 actor-intended PUSH gating around actual
 * A4 capsule->dynamic-box response and private A5 contact history.
 * The NfActor input is source-owned and may be interrupted by release.
 * This is NOT a B2 permission or B3 receipt, and never publishes. */
#include "nf_retc_m2_support_tick.h"
typedef struct NfRetcM2PushEvidence {
 NfRetcM2PairEvidence pair;
 uint32_t source_object;
 uint32_t source_collider_index;
 bool staged;
} NfRetcM2PushEvidence;
static inline bool nf_retc_m2_push_stage_in_tick(
 const NfWorld *w,const Nf18a6Runtime *owner,float dt,
 NfRetcM2PushEvidence *out){
 if(!out)return false;
 out->staged=false;
 if(!w||!owner||!w->tick_in_progress||
    w->tick_owner_context!=owner||!owner->require_pair_research||
    !owner->research_pair_stage||
    owner->object_collider_index>=w->collider_count)return false;
 const NfActor *actor=nf_world_find_actor_const(w,owner->actor_id);
 if(!actor||!actor->active||!actor->combat.alive||
    !actor->input.interact_held||
    !isfinite(actor->input.forward)||actor->input.forward<=0.1f||
    !isfinite(actor->input.strafe)||fabsf(actor->input.strafe)>.05f||
    !isfinite(actor->input.yaw_radians)||fabsf(actor->input.yaw_radians)>.05f||
    owner->physical.object.inverse_mass<=0.f||
    owner->physical.actor.velocity.x<=.1f||
    fabsf(owner->physical.actor.velocity.z)>.01f)return false;
 /* Live collider mirror is required as source witness; callers cannot
    secretly move the crate A4 body independently from world geometry. */
 const NfCollider *mirror=&w->colliders[owner->object_collider_index];
 const Nf18a4Body *obj=&owner->physical.object;
 if(mirror->kind!=NF_COLLIDER_SOLID||
    fabsf(mirror->min.x-(obj->center.x-obj->box_half.x))>.0001f||
    fabsf(mirror->min.y-(obj->center.y-obj->box_half.y))>.0001f||
    fabsf(mirror->min.z-(obj->center.z-obj->box_half.z))>.0001f||
    fabsf(mirror->max.x-(obj->center.x+obj->box_half.x))>.0001f||
    fabsf(mirror->max.y-(obj->center.y+obj->box_half.y))>.0001f||
    fabsf(mirror->max.z-(obj->center.z+obj->box_half.z))>.0001f)
    return false;
 NfRetcM2PushEvidence next={0};
 if(!nf_retc_m2_pair_stage_in_tick(w,owner,dt,&next.pair)||
    next.pair.result.applied.normal_impulse.x>=0.f||
    next.pair.candidate.object.velocity.x<=
       owner->physical.object.velocity.x||
    next.pair.candidate.actor.velocity.x>=
       owner->physical.actor.velocity.x)return false;
 next.source_object=obj->id;
 next.source_collider_index=owner->object_collider_index;
 next.staged=true;
 *out=next;
 return true;
}
#endif
