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
/* H273D: independent bounded 1D perfectly-inelastic rival-first
 * reference oracle. NO A4 receipt, A5 mutation, B2/B3 grant or tick commit.
 * Used only when the exact source-derived event clock classified rival-first;
 * ties are explicitly excluded by its positive separation requirement. */
typedef struct NfRetcM2ReverseOracle {
 float rival_time,actor_time,crate_after_rival,final_common_speed;
 bool predicted;
} NfRetcM2ReverseOracle;
static inline bool nf_retc_m2_reverse_oracle(
 float gap,float actor_v,float crate_v,float rival_v,
 float actor_mass,float crate_mass,float rival_mass,
 float actor_crate_t,float dt,NfRetcM2ReverseOracle *out){
 if(!out)return false;
 *out=(NfRetcM2ReverseOracle){0};
 NfRetcM2ArrivalEvidence clock={0};
 if(!nf_retc_m2_arrival_evidence(gap,crate_v,rival_v,
                                actor_crate_t,dt,&clock)||
    clock.order!=2||!clock.rival_closing||
    clock.actor_time-clock.rival_time<=clock.tie_margin||
    !isfinite(actor_v)||actor_v<=crate_v||
    !isfinite(actor_mass)||!isfinite(crate_mass)||
    !isfinite(rival_mass)||actor_mass<=0.f||
    crate_mass<=0.f||rival_mass<=0.f)return false;
 const float total=crate_mass+rival_mass;
 const float post=(crate_mass*crate_v+rival_mass*rival_v)/total;
 /* The pre-impact actor/crate source gap is determined from
    real A4 source TOI and pre-contact relative speed. */
 const float remaining_gap=(actor_v-crate_v)*
     (actor_crate_t-clock.rival_time);
 const float closing=actor_v-post;
 if(!isfinite(total)||!isfinite(post)||!isfinite(remaining_gap)||
    remaining_gap<=0.f||!isfinite(closing)||closing<=0.f)return false;
 const float t2=clock.rival_time+remaining_gap/closing;
 const float total_mass=actor_mass+total;
 const float vfinal=(actor_mass*actor_v+
                     crate_mass*crate_v+rival_mass*rival_v)/total_mass;
 if(!isfinite(t2)||t2<=clock.rival_time+clock.tie_margin||
    t2>=dt||!isfinite(total_mass)||!isfinite(vfinal))return false;
 out->rival_time=clock.rival_time;
 out->actor_time=t2;
 out->crate_after_rival=post;
 out->final_common_speed=vfinal;
 out->predicted=true;
 return true;
}
/* H273E: PRIVATE first genuine A4 rival-first contact from the actual
 * A6 owner and roster. This is NOT the complete two-impact solver and
 * provides NO A5 revision, B2/B3 authorization or accepted world tick. */
typedef struct NfRetcM2ReverseFirst {
 Nf18a4Body crate_after,rival_after;
 Nf18a4Receipt receipt;
 NfRetcM2ReverseOracle oracle;
 uint64_t source_tick;
 uint32_t source_revision;
 bool staged;
} NfRetcM2ReverseFirst;
static inline bool nf_retc_m2_reverse_first_in_tick(
 const NfWorld *w,const Nf18a6Runtime *owner,float dt,
 NfRetcM2ReverseFirst *out){
 if(!out)return false;
 *out=(NfRetcM2ReverseFirst){0};
 if(!w||!owner||!w->tick_in_progress||!w->tick_owner||
    w->tick_owner_context!=owner||!owner->require_pair_research||
    !owner->research_pair_stage||!isfinite(dt)||dt<=0.f||
    dt>.25f||w->tick>=UINT32_MAX||
    owner->last_commit_tick!=w->tick||
    owner->physical.world.tick!=w->tick||
    owner->physical.world.revision==UINT32_MAX)return false;
 NfMa03Roster roster={0};
 if(!nf_ma03_roster_capture_in_tick(w,&roster)||
    roster.count!=2u||roster.source_tick!=w->tick)return false;
 const NfActor *actor=nf_world_find_actor_const(w,owner->actor_id);
 const NfMa03Participant *rival=NULL;
 for(size_t i=0;i<2u;++i)
   if(roster.actors[i].id!=owner->actor_id)rival=&roster.actors[i];
 if(!actor||!rival)return false;
 const NfActor *other=nf_world_find_actor_const(w,rival->id);
 Nf18a4Body a=owner->physical.actor,b=owner->physical.object;
 if(!other||!actor->active||!actor->combat.alive||
    !other->active||!other->combat.alive||
    !actor->input.interact_held||actor->input.forward<=.1f||
    a.id!=owner->actor_id||a.id==b.id||b.id==rival->id||
    !nf18a4_body_valid(&a)||!nf18a4_body_valid(&b)||
    a.inverse_mass<=0.f||b.inverse_mass<=0.f||
    a.velocity.x<=b.velocity.x||b.velocity.x<=rival->velocity.x||
    b.velocity.x<0.f||rival->velocity.x<0.f||
    fabsf(a.velocity.y)>.0001f||fabsf(a.velocity.z)>.0001f||
    fabsf(b.velocity.y)>.0001f||fabsf(b.velocity.z)>.0001f||
    fabsf(rival->velocity.y)>.0001f||fabsf(rival->velocity.z)>.0001f||
    fabsf(rival->position.z-b.center.z)>.0001f||
    fabsf(a.center.z-b.center.z)>.0001f||
    fabsf(actor->transform.position.x-a.center.x)>.0001f||
    fabsf(actor->transform.velocity.x-a.velocity.x)>.0001f||
    fabsf(other->transform.position.x-rival->position.x)>.0001f||
    fabsf(other->transform.velocity.x-rival->velocity.x)>.0001f||
    owner->object_collider_index>=w->collider_count)return false;
 const NfCollider *mirror=&w->colliders[owner->object_collider_index];
 if(mirror->kind!=NF_COLLIDER_SOLID||
    fabsf(mirror->min.x-(b.center.x-b.box_half.x))>.0001f||
    fabsf(mirror->max.x-(b.center.x+b.box_half.x))>.0001f||
    fabsf(mirror->min.y-(b.center.y-b.box_half.y))>.0001f||
    fabsf(mirror->max.y-(b.center.y+b.box_half.y))>.0001f||
    fabsf(mirror->min.z-(b.center.z-b.box_half.z))>.0001f||
    fabsf(mirror->max.z-(b.center.z+b.box_half.z))>.0001f)return false;
 const float gap=rival->position.x-(b.center.x+b.box_half.x+a.radius);
 if(!isfinite(gap)||gap<.01f||gap>.3f)return false;
 const uint32_t tick=(uint32_t)w->tick+1u;
 const uint32_t revision=owner->physical.world.revision;
 const Nf18a2ShapePolicy shape={a.radius,a.height,.05f,.4f,.65f};
 const Nf18a4Motor motor={a.velocity,0.f,0.f};
 const Nf18a4PairResult predicted=nf18a4_pair_step(
  a,b,shape,motor,NF18A4_RECIPROCAL_LINEAR,dt,tick,revision,0.f,0.f);
 if(predicted.status!=NF18A4_ACCEPTED||!predicted.swept_hit||
    !isfinite(predicted.toi)||predicted.toi<=0.f||
    predicted.toi>=1.f)return false;
 NfRetcM2ReverseOracle reference={0};
 if(!nf_retc_m2_reverse_oracle(gap,a.velocity.x,b.velocity.x,
   rival->velocity.x,1.f/a.inverse_mass,1.f/b.inverse_mass,
   80.f,predicted.toi*dt,dt,&reference))return false;
 Nf18a4Body target={0};
 target.id=rival->id;
 target.center=(NfVec3){rival->position.x,
   rival->position.y+.5f*other->movement.body_height,rival->position.z};
 target.velocity=rival->velocity;
 target.inverse_mass=1.f/80.f;
 target.radius=a.radius;
 target.height=other->movement.body_height;
 if(!nf18a4_body_valid(&target))return false;
 b.center.x+=b.velocity.x*reference.rival_time;
 target.center.x+=target.velocity.x*reference.rival_time;
 if(fabsf(target.center.x-(b.center.x+b.box_half.x+target.radius))>.004f)
   return false;
 Nf18a4Constraint c={0};
 c.body_a=b.id;c.body_b=target.id;c.feature_id=b.id^target.id;
 c.normal_b_to_a=(NfVec3){-1.f,0.f,0.f};
 c.contact_point=(NfVec3){b.center.x+b.box_half.x,b.center.y,b.center.z};
 const Nf18a4Receipt receipt=nf18a4_apply_contact(&b,&target,c,
   NF18A4_RECIPROCAL_LINEAR,tick,revision);
 if(receipt.status!=NF18A4_ACCEPTED||!receipt.impulses_applied||
    fabsf(b.velocity.x-reference.crate_after_rival)>.004f||
    fabsf(target.velocity.x-reference.crate_after_rival)>.004f)return false;
 out->crate_after=b;out->rival_after=target;out->receipt=receipt;
 out->oracle=reference;out->source_tick=w->tick;
 out->source_revision=revision;out->staged=true;
 return true;
}
/* H273F: second TRUE A4 event, after the source-backed rival-first
 * A4 impulse. Still PRIVATE: the two event receipts are not an accepted
 * A5 material history or an A6/B2/B3 publication.
 * The second collision solves actor+crate; the rival can separate, so a
 * global all-three common-speed oracle is NOT assumed here. */
typedef struct NfRetcM2ReverseSecond {
 NfRetcM2ReverseFirst first;
 Nf18a4Body actor_after,crate_after,rival_after;
 Nf18a4Receipt second_receipt;
 float expected_pair_velocity;
 bool staged;
} NfRetcM2ReverseSecond;
static inline bool nf_retc_m2_reverse_second_in_tick(
 const NfWorld *w,const Nf18a6Runtime *owner,float dt,
 NfRetcM2ReverseSecond *out){
 if(!out)return false;
 *out=(NfRetcM2ReverseSecond){0};
 NfRetcM2ReverseFirst first={0};
 if(!nf_retc_m2_reverse_first_in_tick(w,owner,dt,&first)||
    !first.staged)return false;
 Nf18a4Body a=owner->physical.actor;
 Nf18a4Body b=first.crate_after;
 Nf18a4Body rival=first.rival_after;
 const float t1=first.oracle.rival_time,t2=first.oracle.actor_time;
 if(!nf18a4_body_valid(&a)||!nf18a4_body_valid(&b)||
    !nf18a4_body_valid(&rival)||!isfinite(t1)||!isfinite(t2)||
    !(t1>0.f&&t2>t1&&t2<dt))return false;
 const float delay=t2-t1;
 /* Actor moves at source speed up to event two. Crate and rival move
    at the genuine first A4 velocities, not the diagnostic oracle. */
 a.center.x+=a.velocity.x*t2;
 b.center.x+=b.velocity.x*delay;
 rival.center.x+=rival.velocity.x*delay;
 if(fabsf(a.center.x+a.radius-(b.center.x-b.box_half.x))>.004f||
    fabsf(rival.center.x-(b.center.x+b.box_half.x+rival.radius))>.004f)
    return false;
 const float ma=1.f/a.inverse_mass,mb=1.f/b.inverse_mass;
 const float reference=(ma*a.velocity.x+mb*b.velocity.x)/(ma+mb);
 if(!isfinite(reference))return false;
 Nf18a4Constraint c={0};
 c.body_a=a.id;c.body_b=b.id;c.feature_id=b.id;
 c.normal_b_to_a=(NfVec3){-1.f,0.f,0.f};
 c.contact_point=(NfVec3){b.center.x-b.box_half.x,b.center.y,b.center.z};
 const Nf18a4Receipt rr=nf18a4_apply_contact(
     &a,&b,c,NF18A4_RECIPROCAL_LINEAR,
     (uint32_t)w->tick+1u,first.source_revision);
 if(rr.status!=NF18A4_ACCEPTED||!rr.impulses_applied||
    fabsf(a.velocity.x-reference)>.004f||
    fabsf(b.velocity.x-reference)>.004f||
    fabsf(rival.velocity.x-first.rival_after.velocity.x)>.00001f)
    return false;
 out->first=first;out->actor_after=a;out->crate_after=b;
 out->rival_after=rival;out->second_receipt=rr;
 out->expected_pair_velocity=reference;out->staged=true;
 return true;
}
/* H273G: two authentic reversed-arrival A4 contacts become one PRIVATE
 * A5 three-participant history revision. A5's graph contract indexes
 * samples actor->crate, crate->rival, while the physical temporal order
 * is crate->rival THEN actor->crate. No live A6/B2/B3 publication.
 * Caller must not misread these graph-indexed arrays as event order. */
typedef struct NfRetcM2ReverseMaterial {
 NfRetcM2ReverseSecond physical;
 Nf18a5World material;
 Nf18a5Sample samples[2];
 uint32_t first_contact_id,second_contact_id;
 bool staged;
} NfRetcM2ReverseMaterial;
static inline bool nf_retc_m2_reverse_material_in_tick(
 const NfWorld *w,const Nf18a6Runtime *owner,float dt,
 NfRetcM2ReverseMaterial *out){
 if(!out)return false;
 out->staged=false;
 if(!w||!owner||owner->physical.world.revision==UINT32_MAX||
    !owner->physical.world.grid.global_revision)return false;
 NfRetcM2ReverseSecond physical={0};
 if(!nf_retc_m2_reverse_second_in_tick(w,owner,dt,&physical)||
    !physical.staged)return false;
 const uint32_t revision=owner->physical.world.revision;
 const uint32_t tick=(uint32_t)w->tick+1u;
 if(physical.first.source_tick!=w->tick||
    physical.first.source_revision!=revision||
    physical.first.receipt.status!=NF18A4_ACCEPTED||
    physical.second_receipt.status!=NF18A4_ACCEPTED||
    !physical.first.receipt.impulses_applied||
    !physical.second_receipt.impulses_applied||
    physical.first.oracle.rival_time>=physical.first.oracle.actor_time)return false;
 Nf18a5World material=owner->physical.world;
 Nf18a5Sample samples[2]={{0},{0}};
 const uint32_t actor=owner->actor_id,crate=owner->physical.object.id;
 const uint32_t rival=physical.first.rival_after.id;
 if(!actor||!crate||!rival||actor==crate||actor==rival||
    crate==rival||physical.actor_after.id!=actor||
    physical.crate_after.id!=crate||
    physical.first.crate_after.id!=crate||
    physical.first.rival_after.id!=rival||
    physical.rival_after.id!=rival)return false;
 /* Commit order follows canonical graph topology, NOT event chronology. */
 if(!nf18a5_receipt_sample(&physical.second_receipt,actor,crate,
     0.f,material.grid.global_revision,&samples[0])||
    !nf18a5_receipt_sample(&physical.first.receipt,crate,rival,
     0.f,material.grid.global_revision,&samples[1]))return false;
 if(samples[0].contact_id==samples[1].contact_id)return false;
 const uint32_t ids[3]={actor,crate,rival};
 if(!nf18a5_world_commit_three(&material,revision,tick,actor,
      ids,samples)||material.revision!=revision+1u||
    material.tick!=tick||
    material.history.raw_samples!=
      owner->physical.world.history.raw_samples+2u)return false;
 /* Publish only the privately staged candidate after both edges succeed. */
 out->physical=physical;out->material=material;
 out->samples[0]=samples[0];out->samples[1]=samples[1];
 out->first_contact_id=samples[1].contact_id;
 out->second_contact_id=samples[0].contact_id;
 out->staged=true;
 return true;
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
