#ifndef NF_RETC_M2_EVENT_SCHEDULER_H
#define NF_RETC_M2_EVENT_SCHEDULER_H
/* H263-H267: bounded genuinely DELAYED second contact. The first impact
 * is real A4 capsule/box CCD. The second arrival is scheduled from the
 * post-impact crate speed and actual separated rival source position.
 * At that event, both A4 constraints converge in a coupled 1D solve,
 * with two authentic A4 receipts committed to one PRIVATE A5 revision.
 * No 3D generalization, B2/B3 grant, WAL or A6 publication. */
#include "nf_retc_m2_three_contact_tick.h"
/* H273C: source-derived 1D event clock. Input actor_t comes from real
 * A4 swept capsule-box TOI. Crate/rival time comes from independent
 * source velocities and source geometry; no staged impulse is applied.
 * 0=invalid; 1=actor-first; 2=rival-first/tie (unsupported);
 * 3=rival is not closing (actor-first if A4 TOI valid). */
typedef struct NfRetcM2ArrivalEvidence {
 float actor_time,rival_time,tie_margin;
 int order;
 bool rival_closing;
} NfRetcM2ArrivalEvidence;
static inline bool nf_retc_m2_arrival_evidence(
 float gap,float crate_v,float rival_v,float actor_crate_t,
 float dt,NfRetcM2ArrivalEvidence *out){
 if(!out)return false;
 *out=(NfRetcM2ArrivalEvidence){0};
 if(!isfinite(gap)||gap<=0.f||!isfinite(crate_v)||
    !isfinite(rival_v)||!isfinite(actor_crate_t)||
    !isfinite(dt)||actor_crate_t<=0.f||dt<=0.f||
    actor_crate_t>=dt)return false;
 out->actor_time=actor_crate_t;
 out->tie_margin=0.00001f;
 const float closing=crate_v-rival_v;
 if(!isfinite(closing))return false;
 if(closing<=0.f){
   out->rival_time=INFINITY;out->order=3;return true;
 }
 const float rival_time=gap/closing;
 if(!isfinite(rival_time)||rival_time<0.f)return false;
 out->rival_closing=true;
 out->rival_time=rival_time;
 out->order=(rival_time<=actor_crate_t+out->tie_margin)?2:1;
 return true;
}
static inline int nf_retc_m2_arrival_order(float gap,float crate_v,
 float rival_v,float actor_crate_t,float dt){
 NfRetcM2ArrivalEvidence clock={0};
 if(!nf_retc_m2_arrival_evidence(gap,crate_v,rival_v,actor_crate_t,
                                dt,&clock))return 0;
 return clock.order==3?1:clock.order;
}
typedef struct NfRetcM2EventChain {
 Nf18a5World material;
 Nf18a4Body bodies[3];
 Nf18a4Receipt contacts[2];
 Nf18a5Sample samples[2];
 float first_time,second_time,expected_velocity;
 unsigned passes;
 uint32_t reject_line;
 bool staged;
} NfRetcM2EventChain;
static inline bool nf_retc_m2_event_chain_in_tick(
 const NfWorld *w,const Nf18a6Runtime *owner,float dt,
 NfRetcM2EventChain *out){
 if(!out)return false;
 out->staged=false;out->reject_line=0u;
 if(!w||!owner||!w->tick_in_progress||w->tick_owner_context!=owner||
    !w->tick_owner||!owner->require_pair_research||
    !owner->research_pair_stage||owner->require_dual_contact_preflight||
    !isfinite(dt)||dt<=0.f||dt>.25f||w->tick>=UINT32_MAX||
    owner->last_commit_tick!=w->tick||
    owner->physical.world.tick!=w->tick||
    owner->physical.world.revision==UINT32_MAX||
    !owner->physical.world.grid.global_revision){ out->reject_line=(uint32_t)__LINE__; return false; }
 NfMa03Roster roster={0};
 if(!nf_ma03_roster_capture_in_tick(w,&roster)||
    roster.count!=2u||roster.source_tick!=w->tick){ out->reject_line=(uint32_t)__LINE__; return false; }
 const NfActor *actor=nf_world_find_actor_const(w,owner->actor_id);
 const NfMa03Participant *rival=NULL;
 for(size_t i=0u;i<2u;++i)
  if(roster.actors[i].id!=owner->actor_id)rival=&roster.actors[i];
 if(!actor||!rival){ out->reject_line=(uint32_t)__LINE__; return false; }
 const NfActor *other=nf_world_find_actor_const(w,rival->id);
 Nf18a4Body a=owner->physical.actor,b=owner->physical.object;
 if(!other||!actor->active||!actor->combat.alive||
    !other->active||!other->combat.alive||
    !actor->input.interact_held||actor->input.forward<=.1f||
    a.id!=owner->actor_id||a.id==b.id||b.id==rival->id||
    !nf18a4_body_valid(&a)||!nf18a4_body_valid(&b)||
    a.inverse_mass<=0.f||b.inverse_mass<=0.f||a.velocity.x<=.1f||
    fabsf(a.velocity.y)>.0001f||fabsf(a.velocity.z)>.0001f||
    !isfinite(b.velocity.x)||b.velocity.x<0.f||
    b.velocity.x>=a.velocity.x||fabsf(b.velocity.y)>.0001f||
    fabsf(b.velocity.z)>.0001f||
    !isfinite(rival->velocity.x)||rival->velocity.x<0.f||
    rival->velocity.x>=a.velocity.x||
    fabsf(rival->velocity.y)>.0001f||
    fabsf(rival->velocity.z)>.0001f||
    fabsf(rival->position.z-b.center.z)>.0001f||
    fabsf(a.center.z-b.center.z)>.0001f||
    fabsf(other->movement.body_height-a.height)>.0001f||
    fabsf(actor->transform.position.x-a.center.x)>.0001f||
    fabsf(actor->transform.velocity.x-a.velocity.x)>.0001f||
    fabsf(other->transform.position.x-rival->position.x)>.0001f||
    fabsf(other->transform.velocity.x-rival->velocity.x)>.0001f||
    b.box_half.x<=0.f||b.box_half.y<=0.f||b.box_half.z<=0.f||
    owner->object_collider_index>=w->collider_count){ out->reject_line=(uint32_t)__LINE__; return false; }
 const NfCollider *mirror=&w->colliders[owner->object_collider_index];
 if(mirror->kind!=NF_COLLIDER_SOLID||
    fabsf(mirror->min.x-(b.center.x-b.box_half.x))>.0001f||
    fabsf(mirror->min.y-(b.center.y-b.box_half.y))>.0001f||
    fabsf(mirror->min.z-(b.center.z-b.box_half.z))>.0001f||
    fabsf(mirror->max.x-(b.center.x+b.box_half.x))>.0001f||
    fabsf(mirror->max.y-(b.center.y+b.box_half.y))>.0001f||
    fabsf(mirror->max.z-(b.center.z+b.box_half.z))>.0001f){ out->reject_line=(uint32_t)__LINE__; return false; }
 const float gap=rival->position.x-
   (b.center.x+b.box_half.x+a.radius);
 if(!isfinite(gap)||gap<.01f||gap>.3f){ out->reject_line=(uint32_t)__LINE__; return false; }
 const uint32_t tick=(uint32_t)w->tick+1u;
 const uint32_t revision=owner->physical.world.revision;
 const Nf18a2ShapePolicy shape={a.radius,a.height,.05f,.4f,.65f};
 const Nf18a4Motor motor={a.velocity,0.f,0.f};
 const Nf18a4PairResult first=nf18a4_pair_step(
    a,b,shape,motor,NF18A4_RECIPROCAL_LINEAR,dt,tick,revision,0.f,0.f);
 if(first.status!=NF18A4_ACCEPTED||!first.swept_hit||
    first.receipt.status!=NF18A4_ACCEPTED||
    !first.receipt.impulses_applied||first.toi<=0.f||
    first.toi>=1.f){ out->reject_line=(uint32_t)__LINE__; return false; }
 const float t1=first.toi*dt;
 /* H273B: classify competing *source* contact arrivals before ANY
  * staged A4 impulse. Reject reversed or near-simultaneous schedules.
  * Full reversed-event solver remains a separate research challenger. */
 if(nf_retc_m2_arrival_order(gap,b.velocity.x,rival->velocity.x,
                            t1,dt)!=1){
    out->reject_line=(uint32_t)__LINE__;return false;
 }
 Nf18a4Body bodies[3]={a,b,{0}};
 bodies[0].center.x+=a.velocity.x*t1;
 bodies[1].center.x+=b.velocity.x*t1;
 bodies[2].id=rival->id;
 bodies[2].center=(NfVec3){rival->position.x,
     rival->position.y+.5f*other->movement.body_height,rival->position.z};
 bodies[2].velocity=rival->velocity;
 /* Advance the real rival to event one, even when still separated. */
 bodies[2].center.x+=rival->velocity.x*t1;
 bodies[2].inverse_mass=1.f/80.f;
 bodies[2].radius=a.radius;
 bodies[2].height=other->movement.body_height;
 if(!nf18a4_body_valid(&bodies[2])){ out->reject_line=(uint32_t)__LINE__; return false; }
 Nf18a4Constraint c[2]={{0},{0}};
 c[0].body_a=a.id;c[0].body_b=b.id;c[0].feature_id=b.id;
 c[0].normal_b_to_a=(NfVec3){-1.f,0.f,0.f};
 c[0].contact_point=(NfVec3){
     b.center.x-b.box_half.x,b.center.y,b.center.z};
 c[1].body_a=b.id;c[1].body_b=rival->id;
 c[1].feature_id=b.id^rival->id;
 c[1].normal_b_to_a=(NfVec3){-1.f,0.f,0.f};
 c[1].contact_point=(NfVec3){
     b.center.x+b.box_half.x,b.center.y,b.center.z};
 Nf18a4Receipt receipts[2]={{0},{0}};
 receipts[0]=nf18a4_apply_contact(&bodies[0],&bodies[1],
     c[0],NF18A4_RECIPROCAL_LINEAR,tick,revision);
 if(!receipts[0].impulses_applied||
    receipts[0].status!=NF18A4_ACCEPTED){ out->reject_line=(uint32_t)__LINE__; return false; }
 /* At first contact, the original crate-rival gap has evolved due to
    BOTH initial source velocities during the first event interval. */
 const float gap_t1=gap+(rival->velocity.x-b.velocity.x)*t1;
 if(!isfinite(gap_t1)||gap_t1<=.0001f){ out->reject_line=(uint32_t)__LINE__; return false; }
 const float relative=bodies[1].velocity.x-bodies[2].velocity.x;
 if(!isfinite(relative)||relative<=.0001f){ out->reject_line=(uint32_t)__LINE__; return false; }
 const float second_delay=gap_t1/relative;
 const float t2=t1+second_delay;
 if(!isfinite(t2)||t2>=dt||second_delay<=.0001f){ out->reject_line=(uint32_t)__LINE__; return false; }
 /* Between events: propagate all bodies by SOURCE-EVENT A4 velocities. */
 for(size_t i=0u;i<3u;++i)
   bodies[i].center.x+=bodies[i].velocity.x*second_delay;
 if(fabsf(bodies[2].center.x-
    (bodies[1].center.x+b.box_half.x+bodies[2].radius))>.004f||
    fabsf(bodies[0].center.x-
    (bodies[1].center.x-b.box_half.x-bodies[0].radius))>.006f)
   { out->reject_line=(uint32_t)__LINE__; return false; }
 const float ma=1.f/a.inverse_mass,mb=1.f/b.inverse_mass;
 const float mc=1.f/bodies[2].inverse_mass;
 const float oracle=(ma*a.velocity.x+mb*b.velocity.x+
                     mc*bodies[2].velocity.x)/(ma+mb+mc);
 if(!isfinite(oracle)){ out->reject_line=(uint32_t)__LINE__; return false; }
 unsigned passes=0u;
 for(unsigned pass=0u;pass<32u;++pass){
  for(size_t k=0u;k<2u;++k){
   const Nf18a4Receipt rr=nf18a4_apply_contact(
      &bodies[k],&bodies[k+1],c[k],NF18A4_RECIPROCAL_LINEAR,tick,revision);
   if(rr.status==NF18A4_INVALID){ out->reject_line=(uint32_t)__LINE__; return false; }
   if(rr.impulses_applied){
    if(!receipts[k].impulses_applied)receipts[k]=rr;
    else receipts[k].normal_impulse.x+=rr.normal_impulse.x;
   }
  }
  passes=pass+1u;
  if(fabsf(bodies[0].velocity.x-oracle)<.004f&&
     fabsf(bodies[1].velocity.x-oracle)<.004f&&
     fabsf(bodies[2].velocity.x-oracle)<.004f)break;
 }
 if(!receipts[0].impulses_applied||!receipts[1].impulses_applied||
    fabsf(bodies[0].velocity.x-oracle)>.004f||
    fabsf(bodies[1].velocity.x-oracle)>.004f||
    fabsf(bodies[2].velocity.x-oracle)>.004f){ out->reject_line=(uint32_t)__LINE__; return false; }
 /* Full source-to-final swept extent, not merely the final segment.
    These bounded tests support only monotone +X trajectories; any
    reversal is rejected before claiming static clearance. */
 Nf18a5Grid grid=owner->physical.world.grid;
 const NfVec3 start_pos[3]={
    a.center,b.center,
    {rival->position.x,
     rival->position.y+.5f*other->movement.body_height,rival->position.z}};
 for(size_t i=0u;i<3u;++i){
  const float radius=i==1u?fmaxf(b.box_half.x,b.box_half.z):a.radius;
  const float height=i==1u?fmaxf(2.f*b.box_half.y,2.f*radius):a.height;
  const Nf18a2ShapePolicy policy={radius,height,.05f,.4f,.65f};
  const NfVec3 feet={start_pos[i].x,
      start_pos[i].y-.5f*height,start_pos[i].z};
  const float end_x=bodies[i].center.x+
      bodies[i].velocity.x*(dt-t2);
  const NfVec3 delta={end_x-start_pos[i].x,0.f,0.f};
  uint32_t loads=0u;
  if(!isfinite(delta.x)||delta.x<-.0001f||
     nf18a5_sweep_query(&grid,feet,delta,policy,dt,
         tick,NULL,NULL,&loads)!=NF18A5_Q_FREE){ out->reject_line=(uint32_t)__LINE__; return false; }
  bodies[i].center.x=end_x;
 }
 NfRetcM2EventChain next={0};
 next.material=owner->physical.world;
 for(size_t k=0u;k<2u;++k){
  if(!nf18a5_receipt_sample(&receipts[k],bodies[k].id,bodies[k+1].id,
      0.f,next.material.grid.global_revision,&next.samples[k]))
    { out->reject_line=(uint32_t)__LINE__; return false; }
  next.contacts[k]=receipts[k];
 }
 const uint32_t ids[3]={a.id,b.id,rival->id};
 if(!nf18a5_world_commit_three(&next.material,revision,tick,
    a.id,ids,next.samples)||
    next.material.history.raw_samples!=
      owner->physical.world.history.raw_samples+2u){ out->reject_line=(uint32_t)__LINE__; return false; }
 for(size_t i=0u;i<3u;++i)next.bodies[i]=bodies[i];
 next.first_time=t1;next.second_time=t2;
 next.expected_velocity=oracle;next.passes=passes;
 next.staged=true;*out=next;
 return true;
}
#endif
