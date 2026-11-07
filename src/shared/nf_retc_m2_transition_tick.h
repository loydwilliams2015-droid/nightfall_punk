#ifndef NF_RETC_M2_TRANSITION_TICK_H
#define NF_RETC_M2_TRANSITION_TICK_H
/* H218-H222: research-only PUSH -> CARRY and interruption/rearm.
 * A6 invokes this callback inside its sole tick-owner lease, but never
 * publishes its result. Staged consequences remain alternatives from the
 * SAME authoritative source; NO cross-tick physical state is inferred.
 * This does not grant B2 possession, B3 authority or any durable receipt. */
#include "nf_retc_m2_carry_tick.h"
typedef enum NfRetcM2TransitionMode {
 NF_RETC_M2_PUSH=1,NF_RETC_M2_CARRY=2,NF_RETC_M2_RELEASE=3
} NfRetcM2TransitionMode;
typedef struct NfRetcM2Transition {
 NfRetcM2PushEvidence push;
 NfRetcM2CarryEvidence carry;
 uint64_t source_tick;
 uint32_t source_revision,actor_id,object_id;
 NfRetcM2TransitionMode mode;
 bool armed,staged;
} NfRetcM2Transition;
static inline bool nf_retc_m2_transition_in_tick(
 const NfWorld *w,const Nf18a6Runtime *owner,float dt,
 NfRetcM2TransitionMode requested,NfRetcM2Transition *state){
 if(!state)return false;
 state->staged=false;
 if(!w||!owner||!w->tick_in_progress||
    w->tick_owner_context!=owner||!w->tick_owner||
    !owner->require_pair_research||!owner->research_pair_stage||
    owner->require_dual_contact_preflight)return false;
 const NfActor *actor=nf_world_find_actor_const(w,owner->actor_id);
 /* Release cannot retain a staged object claim. Invalid source also
    invalidates an existing research-only transition witness. */
 if(requested==NF_RETC_M2_RELEASE||!actor||!actor->active||
    !actor->combat.alive||!actor->input.interact_held){
  state->armed=false;
  state->mode=NF_RETC_M2_RELEASE;
  state->push.staged=false;
  state->carry.staged=false;
  return false;
 }
 if(requested==NF_RETC_M2_PUSH){
  state->armed=false;
  state->carry.staged=false;
  if(!nf_retc_m2_push_stage_in_tick(w,owner,dt,&state->push))return false;
  state->source_tick=w->tick;
  state->source_revision=owner->physical.world.revision;
  state->actor_id=owner->actor_id;
  state->object_id=owner->physical.object.id;
  state->mode=NF_RETC_M2_PUSH;
  state->armed=true;
  state->staged=true;
  return true;
 }
 if(requested!=NF_RETC_M2_CARRY||!state->armed||
    (state->mode!=NF_RETC_M2_PUSH&&state->mode!=NF_RETC_M2_CARRY)||
    state->source_tick!=w->tick||
    state->source_revision!=owner->physical.world.revision||
    state->actor_id!=owner->actor_id||
    state->object_id!=owner->physical.object.id){
  state->armed=false;
  return false;
 }
 /* Recheck live world mirror, actor and both A5 swept hulls on EVERY
    carry request; a past PUSH is not a permanent capability. */
 if(!nf_retc_m2_carry_stage_in_tick(w,owner,dt,&state->carry)){
  state->armed=false;
  return false;
 }
 state->mode=NF_RETC_M2_CARRY;
 state->staged=true;
 return true;
}
#endif
