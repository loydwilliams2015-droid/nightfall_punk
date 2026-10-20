#include "nf_multi_narrow18c1.h"
#include <math.h>
#include <string.h>
static const Nf18a4Body *body(const NfMulti18c1World *w,uint32_t id){
 for(size_t i=0;i<w->count;i++)if(w->bodies[i].id==id)return &w->bodies[i];
 return NULL;
}
NfNarrowResult nf_multi_narrow18c1(const NfMulti18c1World *w,const NfMultiBroad *pairs,float dt){
 NfNarrowResult result={0};result.status=NF_NARROW_INVALID;
 if(!w||!pairs||!isfinite(dt)||dt<=0||w->count>NF18A4_MAX_BODIES||
  pairs->count>NF18A4_MAX_BODIES*(NF18A4_MAX_BODIES-1u)/2u)return result;
 result.status=NF_NARROW_OK;
 for(size_t i=0;i<pairs->count;i++){
  const Nf18a4Body *a=body(w,pairs->pairs[i].id_a),*b=body(w,pairs->pairs[i].id_b);
  if(!a||!b||a==b||!nf18a4_body_valid(a)||!nf18a4_body_valid(b)){
   result.status=NF_NARROW_INVALID;return result;
  }
  result.considered++;
  const bool a_cap=a->radius>0&&a->height>=2*a->radius;
  const bool b_cap=b->radius>0&&b->height>=2*b->radius;
  if(a_cap==b_cap){result.unsupported++;result.status=NF_NARROW_PENDING;continue;}
  const Nf18a4Body *actor=a_cap?a:b,*box=a_cap?b:a;
  if(box->box_half.x<=0||box->box_half.y<=0||box->box_half.z<=0){
   result.unsupported++;result.status=NF_NARROW_PENDING;continue;
  }
  Nf18a2ShapePolicy shape={0};
  shape.radius=actor->radius;shape.height=actor->height;
  shape.foot_flat_radius=0.1f;shape.step_height=0.35f;
  shape.min_support_normal_y=.7071067f;
  Nf18aCollider collider={0};
  collider.body_id=box->id;collider.min=(NfVec3){box->center.x-box->box_half.x,box->center.y-box->box_half.y,box->center.z-box->box_half.z};
  collider.max=(NfVec3){box->center.x+box->box_half.x,box->center.y+box->box_half.y,box->center.z+box->box_half.z};
  collider.velocity=box->velocity;collider.dynamic_body=box->inverse_mass>0;
  const NfVec3 feet={actor->center.x,actor->center.y-actor->height*.5f,actor->center.z};
  const NfVec3 motion={actor->velocity.x*dt,actor->velocity.y*dt,actor->velocity.z*dt};
  Nf18aContact hit={0};
  if(!nf18a3_capsule_sweep(feet,shape,motion,collider,dt,&hit))continue;
  if(hit.kind==NF18A_CONTACT_INITIAL_OVERLAP){
   result.status=NF_NARROW_PENDING;continue;
  }
  if(result.count==NF18A4_MAX_CONSTRAINTS){result.status=NF_NARROW_PENDING;continue;}
  Nf18a4Constraint *out=&result.constraints[result.count++];
  out->body_a=actor->id;out->body_b=box->id;out->feature_id=box->id;
  out->normal_b_to_a=hit.normal;out->contact_point=hit.point;out->restitution=0;out->friction=0;
 }
 /* No caller may treat a partial constraint set as a completed solve. */
 if(result.status!=NF_NARROW_OK)result.count=0;
 return result;
}
