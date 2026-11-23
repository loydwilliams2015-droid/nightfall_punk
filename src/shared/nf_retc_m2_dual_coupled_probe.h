#ifndef NF_RETC_M2_DUAL_COUPLED_PROBE_H
#define NF_RETC_M2_DUAL_COUPLED_PROBE_H
/* H319Q: bounded two-directed-contact coupling diagnostic.
 * Only source-bound pairwise A4 CCD arrivals that are contemporaneous
 * may enter an A4 three-body island; otherwise report a fail-closed
 * timing mismatch. Private copies only. No A5/B2/B3/A6 publication. */
#include "nf_retc_m2_dual_directed_probe.h"
typedef struct NfRetcM2DualCoupled {
 Nf18a4Body bodies[3];
 Nf18a4Receipt receipt[2];
 uint32_t actor_id[2],object_id;
 float toi[2],difference;
 bool contemporaneous,accepted,order_equivalent;
} NfRetcM2DualCoupled;
static inline bool nf_retc_m2_dual_coupled_probe(
 const NfRetcM2DualDirected *source,float dt,
 NfRetcM2DualCoupled *out){
 if(!out)return false;
 *out=(NfRetcM2DualCoupled){0};
 if(!source||!source->staged||!isfinite(dt)||dt<=0.f||dt>.25f||
    !source->tick||!source->world_revision||
    !source->actor_id[0]||!source->actor_id[1]||
    source->actor_id[0]==source->actor_id[1]||
    !source->object_id)return false;
 for(unsigned i=0u;i<2u;++i){
  const Nf18a4PairResult *r=&source->contact[i];
  if(r->status!=NF18A4_ACCEPTED||!r->swept_hit||
     !r->receipt.impulses_applied||
     r->receipt.status!=NF18A4_ACCEPTED||
     r->receipt.tick!=source->tick||
     r->receipt.world_version!=source->world_revision||
     r->actor.id!=source->actor_id[i]||
     r->object.id!=source->object_id||
     !isfinite(r->toi)||r->toi<=0.f||r->toi>=1.f)return false;
  out->actor_id[i]=source->actor_id[i];
  out->toi[i]=r->toi;
 }
 out->object_id=source->object_id;
 out->difference=fabsf(out->toi[0]-out->toi[1]);
 /* A mismatch cannot be silently flattened into a made-up manifold. */
 if(out->difference>.0001f)return true;
 out->contemporaneous=true;
 Nf18a4Body sourceBodies[3]={0};
 Nf18a4Constraint cs[2]={{0},{0}};
 const Nf18a4Body obj=source->contact[0].object;
 if(source->contact[1].object.id!=obj.id||
    !nf18a4_body_valid(&obj))return false;
 sourceBodies[1]=obj;
 /* Pair-step results are post-contact and cannot be reused as pre-contact
    bodies. Recover independent source pre-impulse velocities using the
    known two-body impulses, and reverse the independent pair travel.
    If source contact chronology cannot be reconstructed safely, refuse. */
 for(unsigned i=0u;i<2u;++i){
  const Nf18a4PairResult *r=&source->contact[i];
  Nf18a4Body a=r->actor;
  const float t=dt*r->toi;
  const float remaining=dt-t;
  const float sign=(a.center.x<obj.center.x)?-1.f:1.f;
  if(!isfinite(t)||!isfinite(remaining)||remaining<0.f||
     !isfinite(a.inverse_mass)||a.inverse_mass<=0.f||
     fabsf(r->receipt.normal_impulse.y)>.0001f||
     fabsf(r->receipt.normal_impulse.z)>.0001f)return false;
  /* Recover only a conservative one-dimensional pre-contact state.
     A4 impulses in pair_step are the actor-normal impulse. */
  a.velocity.x-=r->receipt.normal_impulse.x*a.inverse_mass;
  a.center.x-=r->actor.velocity.x*remaining;
  a.center.x-=a.velocity.x*t;
  sourceBodies[i==0u?0u:2u]=a;
  cs[i].body_a=a.id;
  cs[i].body_b=obj.id;
  cs[i].feature_id=obj.id^(i+1u);
  cs[i].normal_b_to_a=(NfVec3){sign,0.f,0.f};
  cs[i].contact_point=(NfVec3){
   obj.center.x+sign*obj.box_half.x,obj.center.y,obj.center.z};
 }
 /* This path is intentionally a feasibility discriminator. Never
    claim coupled PASS unless stable forward/reverse A4 solves agree. */
 const float instant=dt*out->toi[0];
 for(unsigned i=0u;i<3u;++i)
  sourceBodies[i].center.x+=sourceBodies[i].velocity.x*instant;
 Nf18a4Body forwards[3]={sourceBodies[0],sourceBodies[1],sourceBodies[2]};
 Nf18a4Body backwards[3]={sourceBodies[0],sourceBodies[1],sourceBodies[2]};
 const Nf18a4Constraint reversed[2]={cs[1],cs[0]};
 const Nf18a4IslandResult f=nf18a4_solve_island(
    forwards,3u,cs,2u,NF18A4_RECIPROCAL_LINEAR,
    source->tick,source->world_revision);
 const Nf18a4IslandResult b=nf18a4_solve_island(
    backwards,3u,reversed,2u,NF18A4_RECIPROCAL_LINEAR,
    source->tick,source->world_revision);
 if(f.status!=NF18A4_ACCEPTED||b.status!=NF18A4_ACCEPTED||
    f.contact_count!=2u||b.contact_count!=2u)return true;
 for(unsigned i=0u;i<3u;++i)
   if(fabsf(forwards[i].velocity.x-backwards[i].velocity.x)>.003f||
      fabsf(forwards[i].center.x-backwards[i].center.x)>.003f)
      return true;
 out->order_equivalent=true;
 out->accepted=true;
 for(unsigned i=0u;i<3u;++i)out->bodies[i]=forwards[i];
 for(unsigned i=0u;i<2u;++i)out->receipt[i]=f.receipts[i];
 return true;
}
#endif
