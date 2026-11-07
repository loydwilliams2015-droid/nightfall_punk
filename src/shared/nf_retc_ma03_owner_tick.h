#ifndef NF_RETC_MA03_OWNER_TICK_H
#define NF_RETC_MA03_OWNER_TICK_H
/* H178-H182: actual A6 H83 in-tick read-only owner research stage.
 * Executes two-body A4 collision, independently verifies its oracle,
 * records the real A4 contact receipt in a PRIVATE A5 world/history.
 * NEVER publishes A6 world, physical bodies, B3, WAL or readers.
 * Static A5 swept-volume clearance and two-event contractual
 * materialization remain open acceptance requirements. */
#include "nf_retc_ma03_a5_contact_stage.h"
#include <math.h>
typedef struct NfRetcMa03TickEvidence {
 Nf18a5World material_candidate;
 Nf18a4Body bodies[2];
 Nf18a5Sample sample;
 uint64_t source_tick;
 uint32_t owner_actor,impacts;
 /* M2 bounded per-actor consequences from source-executed A4 impulse. */
 NfVec3 original_velocity[2],material_impulse[2];
 Nf18a4Receipt applied_contact;
 bool staged;
} NfRetcMa03TickEvidence;
static inline bool nf_retc_ma03_a5_stage_in_owner_tick(
 const NfWorld *w,const Nf18a6Runtime *owner,float dt,
 NfRetcMa03TickEvidence *out){
 if(!out)return false;
 out->staged=false;
 if(!w||!owner||!w->tick_in_progress||
    w->tick_owner_context!=owner||!w->tick_owner||
    !owner->require_dual_contact_preflight||
    !isfinite(dt)||dt<=0.f||w->tick>=UINT32_MAX||
    owner->last_commit_tick!=w->tick||
    owner->physical.world.tick!=w->tick||
    owner->physical.world.revision==UINT32_MAX)return false;
 NfMa03Roster roster={0};
 if(!nf_ma03_roster_capture_in_tick(w,&roster)||
    roster.count!=2u||roster.source_tick!=w->tick)return false;
 Nf18a4Body bodies[2]={{0}};
 bool owner_seen=false;
 for(size_t i=0;i<2u;++i){
  const NfMa03Participant *p=&roster.actors[i];
  const NfActor *a=nf_world_find_actor_const(w,p->id);
  if(!a||!a->combat.alive||!a->active||
     fabsf(a->movement.body_height-owner->physical.actor.height)>.0001f)
    return false;
  if(p->id==owner->actor_id){
   owner_seen=true;
   const Nf18a4Body *bound=&owner->physical.actor;
   if(bound->id!=p->id||
      fabsf(bound->center.x-p->position.x)>.0001f||
      fabsf(bound->center.z-p->position.z)>.0001f||
      fabsf(bound->velocity.x-p->velocity.x)>.0001f||
      fabsf(bound->velocity.z-p->velocity.z)>.0001f)
      return false;
  }
  bodies[i].id=p->id;
  bodies[i].center=(NfVec3){p->position.x,
      p->position.y+a->movement.body_height*.5f,p->position.z};
  bodies[i].velocity=p->velocity;
  bodies[i].inverse_mass=1.f/80.f;
  bodies[i].radius=owner->physical.actor.radius;
  bodies[i].height=a->movement.body_height;
 }
 if(!owner_seen)return false;
 const uint32_t tick=(uint32_t)w->tick+1u;
 const uint32_t revision=owner->physical.world.revision;
 NfMa01c2Result solved=nf_ma01c2_integrate(bodies,2u,dt,tick,revision);
 NfMa03cEvidence oracle=nf_ma03c_compare(bodies,2u,dt,tick,revision);
 if(solved.status!=NF_NARROW_OK||solved.count!=2u||
    solved.impacts!=1u||oracle.matched!=1u||
    oracle.disagreed||oracle.unresolved)return false;
 /* H183-H187: both real actor swept capsules must clear STATIC A5
    canonical/fine geometry before either contact or history can stage.
    Queries use a PRIVATE grid because fine-cache fills are not authority.
    Missing, mixed without current fine data, or SOLID fails closed. */
 Nf18a5Grid clearance=owner->physical.world.grid;
 for(size_t i=0;i<2u;++i){
  const Nf18a4Body *start=&bodies[i],*end=&solved.bodies[i];
  const Nf18a2ShapePolicy shape={
   .radius=start->radius,.height=start->height,
   .foot_flat_radius=0.05f,.step_height=0.4f,
   .walkable_normal_y=0.65f
  };
  const NfVec3 feet={start->center.x,
       start->center.y-start->height*.5f,start->center.z};
  const NfVec3 path={end->center.x-start->center.x,
       end->center.y-start->center.y,end->center.z-start->center.z};
  uint32_t loads=0;
  if(nf18a5_sweep_query(&clearance,feet,path,shape,dt,tick,
          NULL,NULL,&loads)!=NF18A5_Q_FREE)
    return false;
 }
 const Nf18a4Receipt *receipt=&solved.receipts[0];
 if(receipt->status!=NF18A4_ACCEPTED||!receipt->impulses_applied||
    receipt->tick!=tick||receipt->world_version!=revision)return false;
 Nf18a5Sample sample={0};
 if(!nf18a5_receipt_sample(receipt,bodies[0].id,bodies[1].id,0.f,
                          owner->physical.world.grid.global_revision,
                          &sample))return false;
 Nf18a5World next=owner->physical.world;
 if(!nf18a5_world_commit(&next,revision,tick,owner->actor_id,&sample,1u)||
    next.revision!=revision+1u||next.tick!=tick||
    next.history.raw_samples!=owner->physical.world.history.raw_samples+1u)
    return false;
 /* H193: derive two actual reciprocal material consequences from the
    A4 solver; never synthesize actor-2 movement from a B3 proposal.
    The present solver is linear, equal-mass and collinear. */
 NfVec3 impulse[2]={{0.f,0.f,0.f},{0.f,0.f,0.f}};
 for(size_t i=0;i<2u;++i){
  const float im=bodies[i].inverse_mass;
  if(!(im>0.f)||!isfinite(im))return false;
  impulse[i]=(NfVec3){
    (solved.bodies[i].velocity.x-bodies[i].velocity.x)/im,
    (solved.bodies[i].velocity.y-bodies[i].velocity.y)/im,
    (solved.bodies[i].velocity.z-bodies[i].velocity.z)/im};
  if(!isfinite(impulse[i].x)||!isfinite(impulse[i].y)||
     !isfinite(impulse[i].z))return false;
 }
 if(fabsf(impulse[0].x+impulse[1].x)>.02f||
    fabsf(impulse[0].y+impulse[1].y)>.02f||
    fabsf(impulse[0].z+impulse[1].z)>.02f||
    fabsf(impulse[0].x) <= .001f||
    fabsf(fabsf(impulse[0].x)-sample.normal_impulse)>.02f)
   return false;
 out->material_candidate=next;
 out->bodies[0]=solved.bodies[0];
 out->bodies[1]=solved.bodies[1];
 out->sample=sample;
 out->source_tick=w->tick;
 out->owner_actor=owner->actor_id;
 out->impacts=solved.impacts;
 for(size_t i=0;i<2u;++i){
  out->original_velocity[i]=bodies[i].velocity;
  out->material_impulse[i]=impulse[i];
 }
 out->applied_contact=*receipt;
 out->staged=true;
 return true;
}
/* H194-H197: validate the actual contact and both material consequences
 * before any downstream authority considers this private staging evidence.
 * A positive result still grants NO ability to publish or journal. */
static inline bool nf_retc_ma03_reciprocal_effects_valid(
 const NfRetcMa03TickEvidence *e){
 if(!e||!e->staged||e->impacts!=1u||
    e->bodies[0].id==e->bodies[1].id||
    e->sample.body_a!=e->bodies[0].id||
    e->sample.body_b!=e->bodies[1].id||
    e->sample.contact_id!=e->applied_contact.contact_id||
    e->sample.tick!=e->applied_contact.tick||
    e->applied_contact.status!=NF18A4_ACCEPTED||
    !e->applied_contact.impulses_applied||
    e->material_candidate.tick!=e->sample.tick||
    e->sample.material_epoch!=e->material_candidate.grid.global_revision)
   return false;
 float delta[2][3];
 for(size_t i=0;i<2u;++i){
  if(!isfinite(e->bodies[i].inverse_mass)||
     e->bodies[i].inverse_mass<=0.f)return false;
  delta[i][0]=(e->bodies[i].velocity.x-e->original_velocity[i].x)/
                    e->bodies[i].inverse_mass;
  delta[i][1]=(e->bodies[i].velocity.y-e->original_velocity[i].y)/
                    e->bodies[i].inverse_mass;
  delta[i][2]=(e->bodies[i].velocity.z-e->original_velocity[i].z)/
                    e->bodies[i].inverse_mass;
  const float claimed[3]={e->material_impulse[i].x,
     e->material_impulse[i].y,e->material_impulse[i].z};
  for(size_t k=0;k<3u;++k)
   if(!isfinite(delta[i][k])||!isfinite(claimed[k])||
      fabsf(delta[i][k]-claimed[k])>.02f)return false;
 }
 for(size_t k=0;k<3u;++k)
  if(fabsf(delta[0][k]+delta[1][k])>.02f)return false;
 const float norm=sqrtf(delta[0][0]*delta[0][0]+
     delta[0][1]*delta[0][1]+delta[0][2]*delta[0][2]);
 const NfVec3 ri=e->applied_contact.normal_impulse;
 const float receipt_norm=sqrtf(ri.x*ri.x+ri.y*ri.y+ri.z*ri.z);
 if(!isfinite(norm)||!isfinite(receipt_norm)||
    norm<=.001f||fabsf(norm-receipt_norm)>.02f||
    fabsf(norm-e->sample.normal_impulse)>.02f)return false;
 return true;
}
#endif
