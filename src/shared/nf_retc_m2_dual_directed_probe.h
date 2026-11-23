#ifndef NF_RETC_M2_DUAL_DIRECTED_PROBE_H
#define NF_RETC_M2_DUAL_DIRECTED_PROBE_H
/* H319O: PRIVATE and intentionally non-atomic feasibility probe for
 * two independently directed actor-to-SAME-object genuine A4 capsule/box
 * CCD contacts from one authoritative A6 owner in-tick roster.
 * Both A4 pair solvers use independent copies of the SAME object snapshot.
 * This is NOT a coupled three-body response, two A5 FREE sweeps, B2 grants,
 * B3 effects, durable transaction or an accepted A6 publication. */
#include "nf_retc_m2_three_contact_tick.h"
typedef struct NfRetcM2DualDirected {
 NfMa03Roster source;
 Nf18a4PairResult contact[2];
 uint32_t object_id,material_epoch,world_revision,tick;
 uint32_t actor_id[2];
 bool staged;
} NfRetcM2DualDirected;
static inline bool nf_retc_m2_dual_directed_probe_in_tick(
 const NfWorld *w,const Nf18a6Runtime *owner,float dt,
 NfRetcM2DualDirected *out){
 if(!out)return false;
 out->staged=false;
 if(!w||!owner||!w->tick_owner||!w->tick_in_progress||
    w->tick_owner_context!=owner||!owner->require_pair_research||
    owner->research_pair_stage==NULL||
    owner->require_dual_contact_preflight||
    !isfinite(dt)||dt<=0.f||dt>.25f||w->tick>=UINT32_MAX||
    owner->last_commit_tick!=w->tick||
    owner->physical.world.tick!=w->tick||
    !owner->physical.world.grid.global_revision||
    owner->object_collider_index>=w->collider_count)return false;
 NfMa03Roster roster={0};
 if(!nf_ma03_roster_capture_in_tick(w,&roster)||roster.count!=2u||
    roster.source_tick!=w->tick)return false;
 const Nf18a4Body crate=owner->physical.object;
 if(crate.id==0u||crate.inverse_mass<=0.f||crate.box_half.x<=0.f||
    crate.box_half.y<=0.f||crate.box_half.z<=0.f||
    fabsf(crate.velocity.x)>.00001f)return false;
 const NfCollider *mirror=&w->colliders[owner->object_collider_index];
 if(mirror->kind!=NF_COLLIDER_SOLID||
    fabsf(mirror->min.x-(crate.center.x-crate.box_half.x))>.0001f||
    fabsf(mirror->max.x-(crate.center.x+crate.box_half.x))>.0001f||
    fabsf(mirror->min.y-(crate.center.y-crate.box_half.y))>.0001f||
    fabsf(mirror->max.y-(crate.center.y+crate.box_half.y))>.0001f||
    fabsf(mirror->min.z-(crate.center.z-crate.box_half.z))>.0001f||
    fabsf(mirror->max.z-(crate.center.z+crate.box_half.z))>.0001f)
    return false;
 NfRetcM2DualDirected candidate={0};
 candidate.source=roster;
 candidate.tick=(uint32_t)w->tick+1u;
 candidate.world_revision=owner->physical.world.revision;
 candidate.material_epoch=owner->physical.world.grid.global_revision;
 candidate.object_id=crate.id;
 bool saw_owner=false,saw_peer=false;
 for(size_t i=0u;i<2u;++i){
  const NfMa03Participant *p=&roster.actors[i];
  const NfActor *actor=nf_world_find_actor_const(w,p->id);
  if(!actor||!actor->active||!actor->combat.alive||
     !p->input.interact_held||!actor->input.interact_held||
     fabsf(p->velocity.y)>.0001f||
     fabsf(p->velocity.z)>.0001f||
     fabsf(p->position.z-crate.center.z)>.0001f||
     fabsf(p->velocity.x)<.1f||
     p->id==crate.id)return false;
  const bool left=p->position.x<crate.center.x;
  if((left&&p->velocity.x<=.1f)||
     (!left&&p->velocity.x>=-.1f))return false;
  Nf18a4Body a={0};
  if(p->id==owner->actor_id){
   saw_owner=true;
   a=owner->physical.actor;
   if(a.id!=p->id||
      fabsf(a.center.x-p->position.x)>.0001f||
      fabsf(a.velocity.x-p->velocity.x)>.0001f)return false;
  }else{
   saw_peer=true;
   a.id=p->id;
   a.center=(NfVec3){p->position.x,
      p->position.y+actor->movement.body_height*.5f,
      p->position.z};
   a.velocity=p->velocity;
   a.inverse_mass=1.f/80.f;
   a.radius=owner->physical.actor.radius;
   a.height=actor->movement.body_height;
  }
  if(!nf18a4_body_valid(&a))return false;
  const Nf18a2ShapePolicy shape={
    a.radius,a.height,.05f,.4f,.65f};
  const Nf18a4Motor motor={a.velocity,0.f,0.f};
  candidate.contact[i]=nf18a4_pair_step(
    a,crate,shape,motor,NF18A4_RECIPROCAL_LINEAR,
    dt,candidate.tick,candidate.world_revision,0.f,0.f);
  const Nf18a4PairResult *r=&candidate.contact[i];
  if(r->status!=NF18A4_ACCEPTED||!r->swept_hit||
     r->receipt.status!=NF18A4_ACCEPTED||
     !r->receipt.impulses_applied||!r->receipt.contact_id||
     r->receipt.tick!=candidate.tick||
     r->receipt.world_version!=candidate.world_revision||
     r->actor.id!=a.id||r->object.id!=crate.id||
     r->toi<=0.f||r->toi>=1.f)return false;
  candidate.actor_id[i]=a.id;
 }
 if(!saw_owner||!saw_peer||
    candidate.actor_id[0]==candidate.actor_id[1])return false;
 candidate.staged=true;
 *out=candidate;
 return true;
}
#endif
