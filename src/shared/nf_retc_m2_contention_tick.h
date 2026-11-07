#ifndef NF_RETC_M2_CONTENTION_TICK_H
#define NF_RETC_M2_CONTENTION_TICK_H
/* H223-H227: source-roster arbitration of two competing PUSH intentions.
 * Deterministic actor-id tie-break is a BOUNDED diagnostic, not a
 * prolonged-contention fairness algorithm. Only an A6-owned winning
 * actor may stage the genuine private A4/A5 PUSH contact. Rival physical
 * bodies are NOT solved in this path; no B2/B3 authority or publication. */
#include "nf_retc_m2_transition_tick.h"
typedef struct NfRetcM2Contention {
 NfMa03Roster roster;
 NfRetcM2PushEvidence winner_effect;
 uint32_t winning_actor,claim_count,source_revision;
 uint64_t source_tick;
 bool staged;
} NfRetcM2Contention;
static inline uint32_t nf_retc_m2_select_claim(
 const NfMa03Roster *roster,NfVec3 object_center,
 uint32_t *claim_count){
 if(claim_count)*claim_count=0u;
 if(!roster||roster->count!=2u)return 0u;
 uint32_t winner=0u,count=0u;
 for(size_t i=0;i<2u;++i){
  const NfMa03Participant *p=&roster->actors[i];
  const float dx=object_center.x-p->position.x;
  if(!p->id||!p->input.interact_held||
     !isfinite(p->input.forward)||p->input.forward<=.1f||
     !isfinite(p->input.strafe)||fabsf(p->input.strafe)>.05f||
     !isfinite(p->input.yaw_radians)||fabsf(p->input.yaw_radians)>.05f||
     !isfinite(dx)||dx<.75f||dx>1.25f||
     !isfinite(p->position.z)||
     fabsf(p->position.z-object_center.z)>.2f)continue;
  ++count;
  if(!winner||p->id<winner)winner=p->id;
 }
 if(claim_count)*claim_count=count;
 return winner;
}
static inline bool nf_retc_m2_contend_in_tick(
 const NfWorld *w,const Nf18a6Runtime *owner,float dt,
 NfRetcM2Contention *out){
 if(!out)return false;
 out->staged=false;
 if(!w||!owner||!w->tick_in_progress||
    w->tick_owner_context!=owner||!w->tick_owner||
    !owner->require_pair_research||!owner->research_pair_stage||
    owner->require_dual_contact_preflight||
    owner->physical.world.tick!=w->tick||
    owner->last_commit_tick!=w->tick||
    owner->physical.world.revision==UINT32_MAX)return false;
 NfMa03Roster snapshot={0};
 if(!nf_ma03_roster_capture_in_tick(w,&snapshot)||
    snapshot.count!=2u||snapshot.source_tick!=w->tick)return false;
 /* Claim inputs MUST agree with live actors; no manufactured roster
    or dead claimant is silently ignored. */
 for(size_t i=0;i<2u;++i){
  const NfMa03Participant *p=&snapshot.actors[i];
  const NfActor *actor=nf_world_find_actor_const(w,p->id);
  if(!actor||!actor->active||!actor->combat.alive||
     actor->transform.position.x!=p->position.x||
     actor->transform.position.z!=p->position.z||
     actor->transform.velocity.x!=p->velocity.x||
     actor->input.interact_held!=p->input.interact_held||
     actor->input.forward!=p->input.forward)return false;
 }
 NfRetcM2Contention next={0};
 next.roster=snapshot;
 next.source_tick=w->tick;
 next.source_revision=owner->physical.world.revision;
 next.winning_actor=nf_retc_m2_select_claim(
    &snapshot,owner->physical.object.center,&next.claim_count);
 /* This physical owner is NOT permitted to stage a rival's PUSH.
    Winning a deterministic claim is NOT contractual authority. */
 if(next.winning_actor!=owner->actor_id||!next.claim_count){
  out->winning_actor=next.winning_actor;
  out->claim_count=next.claim_count;
  return false;
 }
 if(!nf_retc_m2_push_stage_in_tick(w,owner,dt,&next.winner_effect))
   return false;
 next.staged=true;
 *out=next;
 return true;
}
#endif
