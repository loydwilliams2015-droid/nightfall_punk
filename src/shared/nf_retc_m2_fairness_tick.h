#ifndef NF_RETC_M2_FAIRNESS_TICK_H
#define NF_RETC_M2_FAIRNESS_TICK_H
/* H228-H232: A6-invoked PRIVATE two-claimant round-robin diagnostic.
 * 'round' counts research arbitration attempts on a REJECTED world tick;
 * it is NOT an accepted simulation tick, contract grant, or B3 receipt.
 * An alternate selected rival cannot borrow the current A6 actor's body.
 * State belongs to the caller's in-memory test probe, not global scope. */
#include "nf_retc_m2_contention_tick.h"
typedef struct NfRetcM2Fairness {
 NfRetcM2PushEvidence owner_effect;
 uint32_t last_winner, winner, claim_count, owner_actor, object_id;
 uint32_t source_revision, rounds, owner_stages, rival_decisions;
 uint64_t source_tick;
 bool bound, staged;
} NfRetcM2Fairness;
static inline uint32_t nf_retc_m2_fair_select(
 const NfMa03Roster *roster,NfVec3 center,uint32_t previous,
 uint32_t *count){
 uint32_t eligible[2]={0u,0u};
 uint32_t total=0u;
 if(count)*count=0u;
 if(!roster||roster->count!=2u)return 0u;
 /* Use the exact same eligibility surface as the incumbent selector. */
 for(size_t i=0;i<2u;++i){
  NfMa03Roster single=*roster;
  single.actors[1].input.interact_held=false;
  single.actors[0]=roster->actors[i];
  uint32_t c=0u;
  const uint32_t selected=nf_retc_m2_select_claim(&single,center,&c);
  if(c==1u&&selected==roster->actors[i].id){
   eligible[total++]=selected;
  }
 }
 if(count)*count=total;
 if(total==0u)return 0u;
 if(total==1u)return eligible[0];
 if(eligible[0]==eligible[1])return 0u;
 const uint32_t lo=eligible[0]<eligible[1]?eligible[0]:eligible[1];
 const uint32_t hi=eligible[0]>eligible[1]?eligible[0]:eligible[1];
 return previous==lo?hi:lo;
}
static inline bool nf_retc_m2_fair_in_tick(
 const NfWorld *w,const Nf18a6Runtime *owner,float dt,
 NfRetcM2Fairness *state){
 if(!state)return false;
 state->staged=false;
 if(!w||!owner||!w->tick_in_progress||
    w->tick_owner_context!=owner||!w->tick_owner||
    !owner->require_pair_research||!owner->research_pair_stage||
    owner->require_dual_contact_preflight||
    owner->physical.world.tick!=w->tick||
    owner->last_commit_tick!=w->tick||
    owner->physical.world.revision==UINT32_MAX||
    owner->object_collider_index>=w->collider_count)return false;
 const Nf18a4Body *object=&owner->physical.object;
 const NfCollider *mirror=&w->colliders[owner->object_collider_index];
 if(mirror->kind!=NF_COLLIDER_SOLID||
    fabsf(mirror->min.x-(object->center.x-object->box_half.x))>.0001f||
    fabsf(mirror->min.y-(object->center.y-object->box_half.y))>.0001f||
    fabsf(mirror->min.z-(object->center.z-object->box_half.z))>.0001f||
    fabsf(mirror->max.x-(object->center.x+object->box_half.x))>.0001f||
    fabsf(mirror->max.y-(object->center.y+object->box_half.y))>.0001f||
    fabsf(mirror->max.z-(object->center.z+object->box_half.z))>.0001f)
    return false;
 NfMa03Roster roster={0};
 if(!nf_ma03_roster_capture_in_tick(w,&roster)||roster.count!=2u||
    roster.source_tick!=w->tick)return false;
 for(size_t i=0;i<2u;++i){
  const NfMa03Participant *p=&roster.actors[i];
  const NfActor *a=nf_world_find_actor_const(w,p->id);
  if(!a||!a->active||!a->combat.alive||
     a->transform.position.x!=p->position.x||
     a->transform.position.z!=p->position.z||
     a->transform.velocity.x!=p->velocity.x||
     a->input.interact_held!=p->input.interact_held||
     a->input.forward!=p->input.forward)return false;
 }
 if(state->bound&&(state->source_tick!=w->tick||
    state->source_revision!=owner->physical.world.revision||
    state->owner_actor!=owner->actor_id||
    state->object_id!=object->id)){
   state->bound=false;
   state->last_winner=0u;
   return false; /* no silent rebound after source invalidation */
 }
 uint32_t count=0u;
 const uint32_t winner=nf_retc_m2_fair_select(
     &roster,object->center,state->bound?state->last_winner:0u,&count);
 if(!winner||!count)return false;
 if(winner==owner->actor_id &&
    !nf_retc_m2_push_stage_in_tick(w,owner,dt,&state->owner_effect))
    return false;
 state->source_tick=w->tick;
 state->source_revision=owner->physical.world.revision;
 state->owner_actor=owner->actor_id;
 state->object_id=object->id;
 state->winner=winner;
 state->claim_count=count;
 state->last_winner=winner;
 state->bound=true;
 ++state->rounds;
 if(winner==owner->actor_id){
  ++state->owner_stages;
  state->staged=true;
 }else{
  ++state->rival_decisions;
  state->owner_effect.staged=false;
 }
 return state->staged;
}
#endif
