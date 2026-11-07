#ifndef NF_RETC_M2_THREE_ENTITY_TICK_H
#define NF_RETC_M2_THREE_ENTITY_TICK_H
/* H233-H237: three SOURCE entities in one private A6 attempt:
 * owner actor + dynamic crate have real A4 capsule-box CCD and A5 receipt;
 * rival actor has source-bound straight-line passive trajectory, validated
 * clear of both swept bodies. Any simultaneous three-body interaction
 * REFUSES rather than silently projecting uncomputed rival impulses.
 * No second physical owner, B2/B3 authority or A6 publication. */
#include "nf_retc_m2_fairness_tick.h"
typedef struct NfRetcM2ThreeEntity {
 NfRetcM2PushEvidence owner_push;
 NfMa03Roster roster;
 Nf18a4Body rival_before,rival_after;
 uint32_t rival_id,source_revision;
 uint64_t source_tick;
 bool staged;
} NfRetcM2ThreeEntity;
static inline bool nf_retc_m2_three_entity_in_tick(
 const NfWorld *w,const Nf18a6Runtime *owner,float dt,
 NfRetcM2ThreeEntity *out){
 if(!out)return false;
 out->staged=false;
 if(!w||!owner||!w->tick_in_progress||
    w->tick_owner_context!=owner||!w->tick_owner||
    !owner->require_pair_research||!owner->research_pair_stage||
    owner->require_dual_contact_preflight||
    !isfinite(dt)||dt<=0.f||dt>.25f||
    owner->last_commit_tick!=w->tick||
    owner->physical.world.tick!=w->tick)return false;
 NfMa03Roster roster={0};
 if(!nf_ma03_roster_capture_in_tick(w,&roster)||roster.count!=2u||
    roster.source_tick!=w->tick)return false;
 const NfMa03Participant *remote=NULL;
 bool owner_seen=false;
 for(size_t i=0;i<2u;++i){
  const NfMa03Participant *p=&roster.actors[i];
  const NfActor *actor=nf_world_find_actor_const(w,p->id);
  if(!actor||!actor->active||!actor->combat.alive||
     actor->transform.position.x!=p->position.x||
     actor->transform.position.y!=p->position.y||
     actor->transform.position.z!=p->position.z||
     actor->transform.velocity.x!=p->velocity.x||
     actor->transform.velocity.y!=p->velocity.y||
     actor->transform.velocity.z!=p->velocity.z||
     actor->input.interact_held!=p->input.interact_held||
     actor->input.forward!=p->input.forward)return false;
  if(p->id==owner->actor_id)owner_seen=true;
  else remote=p;
 }
 if(!owner_seen||!remote)return false;
 const NfActor *ra=nf_world_find_actor_const(w,remote->id);
 const Nf18a4Body *a=&owner->physical.actor;
 const Nf18a4Body *b=&owner->physical.object;
 if(!ra||ra->movement.body_height<=0.f||
    fabsf(ra->movement.body_height-a->height)>.0001f)return false;
 /* A rival in simultaneous collision reach makes the combined result
    unresolved: this slice must NOT pretend the rival is unaffected. */
 const float rival_end_x=remote->position.x+remote->velocity.x*dt;
 const float rival_end_z=remote->position.z+remote->velocity.z*dt;
 if(!isfinite(rival_end_x)||!isfinite(rival_end_z))return false;
 const float owner_end_x=a->center.x+a->velocity.x*dt;
 const float crate_end_x=b->center.x+b->velocity.x*dt;
 const float max_r=a->radius+fmaxf(b->box_half.x,b->box_half.z);
 const float rmin=fminf(remote->position.x,rival_end_x)-a->radius;
 const float rmax=fmaxf(remote->position.x,rival_end_x)+a->radius;
 const float amin=fminf(a->center.x,owner_end_x)-a->radius;
 const float amax=fmaxf(a->center.x,owner_end_x)+a->radius;
 const float bmin=fminf(b->center.x,crate_end_x)-max_r;
 const float bmax=fmaxf(b->center.x,crate_end_x)+max_r;
 const float rzmin=fminf(remote->position.z,rival_end_z);
 const float rzmax=fmaxf(remote->position.z,rival_end_z);
 const float zreach=2.f*a->radius+fmaxf(b->box_half.z,.01f);
 if(rzmin<=zreach&&rzmax>=-zreach&&
    ((rmax>=amin&&rmin<=amax)||(rmax>=bmin&&rmin<=bmax)))
   return false;
 NfRetcM2ThreeEntity next={0};
 if(!nf_retc_m2_push_stage_in_tick(w,owner,dt,&next.owner_push))
   return false;
 /* Second bound: compare against SOLVED actor+crate endpoints, not just
    source linear reach. Fail closed if the rival approaches either. */
 const Nf18a4Body *sa=&next.owner_push.pair.candidate.actor;
 const Nf18a4Body *sb=&next.owner_push.pair.candidate.object;
 const float solved_min=fminf(a->center.x,sa->center.x)-a->radius;
 const float solved_max=fmaxf(a->center.x,sa->center.x)+a->radius;
 const float solved_box_min=fminf(b->center.x,sb->center.x)-max_r;
 const float solved_box_max=fmaxf(b->center.x,sb->center.x)+max_r;
 if(rzmin<=zreach&&rzmax>=-zreach&&
    ((rmax>=solved_min&&rmin<=solved_max)||
     (rmax>=solved_box_min&&rmin<=solved_box_max)))return false;
 next.roster=roster;
 next.rival_before=(Nf18a4Body){0};
 next.rival_before.id=remote->id;
 next.rival_before.center=(NfVec3){remote->position.x,
      remote->position.y+ra->movement.body_height*.5f,remote->position.z};
 next.rival_before.velocity=remote->velocity;
 next.rival_before.radius=a->radius;
 next.rival_before.height=ra->movement.body_height;
 next.rival_before.inverse_mass=1.f/80.f;
 next.rival_after=next.rival_before;
 next.rival_after.center.x+=remote->velocity.x*dt;
 next.rival_after.center.y+=remote->velocity.y*dt;
 next.rival_after.center.z+=remote->velocity.z*dt;
 next.source_tick=w->tick;
 next.source_revision=owner->physical.world.revision;
 next.rival_id=remote->id;
 next.staged=true;
 *out=next;
 return true;
}
#endif
