#ifndef NF_RETC_M2_THREE_CONTACT_TICK_H
#define NF_RETC_M2_THREE_CONTACT_TICK_H
/* H238-H242: bounded source-exact, straight collinear, three-body
 * A4 actual-contact CHALLENGER using genuine capsule/box CCD for the
 * first arrival and a simultaneously touching rival on the box's far
 * face. Independent perfectly inelastic 1D momentum oracle verifies
 * the converged A4 contact impulses. Both actual A4 receipts are folded
 * into ONE private A5 history revision. NO public owner tick, B2/B3,
 * durability, non-collinear manifold, or general world support. */
#include "nf_retc_m2_three_entity_tick.h"
typedef struct NfRetcM2ThreeContact {
 Nf18a5World material;
 Nf18a4Body bodies[3];
 Nf18a4Receipt contacts[2];
 Nf18a5Sample samples[2];
 uint64_t source_tick;
 uint32_t source_revision;
 float expected_velocity;
 unsigned passes;
 bool staged;
} NfRetcM2ThreeContact;
static inline bool nf_retc_m2_three_contact_in_tick(
 const NfWorld *w,const Nf18a6Runtime *owner,float dt,
 NfRetcM2ThreeContact *out){
 if(!out)return false;
 out->staged=false;
 if(!w||!owner||!w->tick_in_progress||
    w->tick_owner_context!=owner||!w->tick_owner||
    !owner->require_pair_research||!owner->research_pair_stage||
    owner->require_dual_contact_preflight||
    !isfinite(dt)||dt<=0.f||dt>.25f||
    owner->last_commit_tick!=w->tick||
    owner->physical.world.tick!=w->tick||
    owner->physical.world.revision==UINT32_MAX||
    !owner->physical.world.grid.global_revision)return false;
 NfMa03Roster roster={0};
 if(!nf_ma03_roster_capture_in_tick(w,&roster)||roster.count!=2u||
    roster.source_tick!=w->tick)return false;
 const NfActor *primary=nf_world_find_actor_const(w,owner->actor_id);
 const NfMa03Participant *rival=NULL;
 for(size_t i=0;i<2u;++i)if(roster.actors[i].id!=owner->actor_id)
    rival=&roster.actors[i];
 if(!primary||!rival)return false;
 const NfActor *other=nf_world_find_actor_const(w,rival->id);
 Nf18a4Body a=owner->physical.actor;
 Nf18a4Body b=owner->physical.object;
 if(!other||!primary->active||!primary->combat.alive||
    !other->active||!other->combat.alive||
    a.id!=owner->actor_id||a.id==b.id||b.id==rival->id||
    !primary->input.interact_held||
    primary->input.forward<=.1f||
    a.inverse_mass<=0.f||b.inverse_mass<=0.f||
    a.velocity.x<=.1f||fabsf(a.velocity.y)>.0001f||
    fabsf(a.velocity.z)>.0001f||
    fabsf(b.velocity.x)>.0001f||fabsf(b.velocity.y)>.0001f||
    fabsf(b.velocity.z)>.0001f||
    fabsf(rival->velocity.x)>.0001f||
    fabsf(rival->velocity.y)>.0001f||
    fabsf(rival->velocity.z)>.0001f||
    fabsf(rival->position.z-b.center.z)>.0001f||
    fabsf(a.center.z-b.center.z)>.0001f||
    fabsf(other->movement.body_height-a.height)>.0001f||
    fabsf(primary->transform.position.x-a.center.x)>.0001f||
    fabsf(primary->transform.velocity.x-a.velocity.x)>.0001f||
    fabsf(other->transform.position.x-rival->position.x)>.0001f||
    fabsf(other->transform.velocity.x-rival->velocity.x)>.0001f||
    b.box_half.x<=0.f||b.box_half.y<=0.f||
    b.box_half.z<=0.f||
    fabsf(rival->position.x-(b.center.x+b.box_half.x+a.radius))>.001f)
    return false;
 const uint32_t tick=(uint32_t)w->tick+1u;
 const uint32_t revision=owner->physical.world.revision;
 const Nf18a2ShapePolicy shape={a.radius,a.height,.05f,.4f,.65f};
 const Nf18a4Motor motor={a.velocity,0.f,0.f};
 const Nf18a4PairResult first=nf18a4_pair_step(
   a,b,shape,motor,NF18A4_RECIPROCAL_LINEAR,dt,tick,revision,0.f,0.f);
 if(first.status!=NF18A4_ACCEPTED||!first.swept_hit||
    first.receipt.status!=NF18A4_ACCEPTED||
    !first.receipt.impulses_applied||first.toi<=0.f||
    first.toi>=1.f)return false;
 const float contact_time=dt*first.toi;
 Nf18a4Body bodies[3]={a,b,{0}};
 bodies[0].center.x+=a.velocity.x*contact_time;
 bodies[1].center.x+=b.velocity.x*contact_time;
 bodies[2].id=rival->id;
 bodies[2].center=(NfVec3){rival->position.x,
    rival->position.y+other->movement.body_height*.5f,rival->position.z};
 bodies[2].velocity=rival->velocity;
 bodies[2].inverse_mass=1.f/80.f;
 bodies[2].radius=a.radius;
 bodies[2].height=other->movement.body_height;
 if(!nf18a4_body_valid(&bodies[2])||
    fabsf((bodies[1].center.x+b.box_half.x+bodies[2].radius)-
           bodies[2].center.x)>.001f||
    fabsf((bodies[1].center.x-b.box_half.x-bodies[0].radius)-
           bodies[0].center.x)>.006f)return false;
 /* 1D independent oracle: perfectly inelastic triple contact yields
    common velocity by conservation of momentum, with no external drive. */
 const float ma=1.f/a.inverse_mass,mb=1.f/b.inverse_mass;
 const float mc=1.f/bodies[2].inverse_mass;
 const float oracle=(ma*a.velocity.x+mb*b.velocity.x+
                     mc*bodies[2].velocity.x)/(ma+mb+mc);
 if(!isfinite(oracle))return false;
 Nf18a4Constraint constraints[2]={{0},{0}};
 constraints[0].body_a=a.id;constraints[0].body_b=b.id;
 constraints[0].feature_id=b.id;
 constraints[0].normal_b_to_a=(NfVec3){-1.f,0.f,0.f};
 constraints[0].contact_point=(NfVec3){
    b.center.x-b.box_half.x,b.center.y,b.center.z};
 constraints[1].body_a=b.id;constraints[1].body_b=bodies[2].id;
 constraints[1].feature_id=b.id^bodies[2].id;
 constraints[1].normal_b_to_a=(NfVec3){-1.f,0.f,0.f};
 constraints[1].contact_point=(NfVec3){
    b.center.x+b.box_half.x,b.center.y,b.center.z};
 Nf18a4Receipt receipts[2]={{0},{0}};
 unsigned passes=0u;
 for(unsigned pass=0u;pass<32u;++pass){
  for(size_t k=0;k<2u;++k){
   Nf18a4Receipt r=nf18a4_apply_contact(&bodies[k],&bodies[k+1],
       constraints[k],NF18A4_RECIPROCAL_LINEAR,tick,revision);
   if(r.status==NF18A4_INVALID)return false;
   if(r.impulses_applied){
    if(!receipts[k].impulses_applied)receipts[k]=r;
    else receipts[k].normal_impulse.x+=r.normal_impulse.x;
   }
  }
  passes=pass+1u;
  if(fabsf(bodies[0].velocity.x-oracle)<.004f&&
     fabsf(bodies[1].velocity.x-oracle)<.004f&&
     fabsf(bodies[2].velocity.x-oracle)<.004f)break;
 }
 if(!receipts[0].impulses_applied||!receipts[1].impulses_applied||
    passes>32u||
    fabsf(bodies[0].velocity.x-oracle)>.004f||
    fabsf(bodies[1].velocity.x-oracle)>.004f||
    fabsf(bodies[2].velocity.x-oracle)>.004f)return false;
 /* This stage conservatively restricts all trajectories to exact static
    material clearance. The dynamic crate is handled by A4 contacts. */
 Nf18a5Grid grid=owner->physical.world.grid;
 const float remaining=dt-contact_time;
 for(size_t k=0;k<3u;++k){
  const float radius=k==1u?fmaxf(b.box_half.x,b.box_half.z):a.radius;
  const float height=k==1u?fmaxf(2.f*b.box_half.y,2.f*radius):a.height;
  const Nf18a2ShapePolicy policy={radius,height,.05f,.4f,.65f};
  NfVec3 feet={bodies[k].center.x,
      bodies[k].center.y-.5f*height,bodies[k].center.z};
  NfVec3 delta={bodies[k].velocity.x*remaining,0.f,0.f};
  uint32_t loads=0u;
  if(nf18a5_sweep_query(&grid,feet,delta,policy,remaining,tick,
      NULL,NULL,&loads)!=NF18A5_Q_FREE)return false;
  bodies[k].center.x+=delta.x;
 }
 NfRetcM2ThreeContact next={0};
 next.material=owner->physical.world;
 for(size_t k=0;k<2u;++k){
  if(!nf18a5_receipt_sample(&receipts[k],bodies[k].id,bodies[k+1].id,
       0.f,next.material.grid.global_revision,&next.samples[k]))return false;
  next.contacts[k]=receipts[k];
 }
 if(!nf18a5_world_commit(&next.material,revision,tick,owner->actor_id,
     next.samples,2u)||next.material.history.raw_samples!=
         owner->physical.world.history.raw_samples+2u)return false;
 for(size_t i=0;i<3u;++i)next.bodies[i]=bodies[i];
 next.source_tick=w->tick;
 next.source_revision=revision;
 next.expected_velocity=oracle;
 next.passes=passes;
 next.staged=true;
 *out=next;
 return true;
}
#endif
