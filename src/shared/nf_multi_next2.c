#include "nf_multi_next2.h"
#include <math.h>
#include <string.h>
#include <float.h>
static Nf18b2UseDecision reject(void){
 Nf18b2UseDecision d={0};d.status=NF18B2_INVALID;d.physical=NF18B2_INVALID;
 d.contract=NF18B2_INVALID;d.reason=NF18B2_BAD_INPUT;return d;
}
Nf18b2UseDecision nf_ma02_live_evaluate(NfMa02ServerSnapshot provider,void *ctx,
 uint32_t actor,uint32_t action,Nf18b2Model model){
 if(!provider||!actor||!action)return reject();
 Nf18b2Scene scene={0};Nf18b2Use use={0};
 if(!provider(ctx,&scene,&use)||use.actor_id!=actor||use.action_id!=action)
  return reject();
 return nf18b2_evaluate(&scene,use,model);
}
static void advance(Nf18a4Body *a,size_t n,float dt){
 for(size_t i=0;i<n;i++){
  a[i].center.x+=a[i].velocity.x*dt;
  a[i].center.y+=a[i].velocity.y*dt;
  a[i].center.z+=a[i].velocity.z*dt;
 }
}
NfMa01c2Result nf_ma01c2_integrate(const Nf18a4Body *b,size_t n,float dt,
 uint32_t tick,uint32_t revision){
 NfMa01c2Result r={0};r.status=NF_NARROW_INVALID;
 if(!b||n<2||n>NF18A4_MAX_BODIES||!tick||!revision||
    !isfinite(dt)||dt<=0)return r;
 r.count=n;memcpy(r.bodies,b,n*sizeof *b);
 for(size_t i=0;i<n;i++){
  if(!nf18a4_body_valid(&b[i])||b[i].radius<=0||
   b[i].height<2*b[i].radius){r.status=NF_NARROW_PENDING;return r;}
  for(size_t j=0;j<i;j++)if(b[i].id==b[j].id)return r;
 }
 float left=dt;r.status=NF_NARROW_PENDING;
 for(unsigned iteration=0;iteration<NF_MA01C2_MAX_EVENTS&&left>0;iteration++){
  size_t ia=n,ib=n;float best=FLT_MAX;
  for(size_t i=0;i<n;i++)for(size_t j=i+1;j<n;j++){
   NfMultiCcd hit=nf_multi_capsule_ccd18c1(&r.bodies[i],&r.bodies[j],left);
   if(hit.status!=NF_NARROW_OK){r.status=hit.status;return r;}
   if(hit.hit){
    float t=hit.toi*left;
    if(t<best || (t==best&&(ia==n||r.bodies[i].id<r.bodies[ia].id||
        (r.bodies[i].id==r.bodies[ia].id&&r.bodies[j].id<r.bodies[ib].id)))){
     best=t;ia=i;ib=j;
    }
   }
  }
  if(ia==n){
   advance(r.bodies,n,left);r.advanced_seconds+=left;
   r.status=NF_NARROW_OK;return r;
  }
  advance(r.bodies,n,best);r.advanced_seconds+=best;left-=best;
  /* Re-sample normal at impact. A zero-separation re-query reports
   * initial overlap, so use geometry from pre-advance CCD instead. */
  const Nf18a4Body *a=&r.bodies[ia],*o=&r.bodies[ib];
  const float dx=a->center.x-o->center.x,dz=a->center.z-o->center.z;
  const float len=sqrtf(dx*dx+dz*dz);
  if(!(len>1.e-7f)){r.status=NF_NARROW_PENDING;return r;}
  Nf18a4Constraint contact={0};
  contact.body_a=a->id;contact.body_b=o->id;contact.feature_id=a->id^o->id;
  contact.normal_b_to_a=(NfVec3){dx/len,0,dz/len};
  contact.contact_point=(NfVec3){o->center.x+o->radius*dx/len,o->center.y,
                                   o->center.z+o->radius*dz/len};
  Nf18a4Receipt receipt=nf18a4_apply_contact(&r.bodies[ia],&r.bodies[ib],
                             contact,NF18A4_RECIPROCAL_LINEAR,tick,revision);
  if(receipt.status!=NF18A4_ACCEPTED || !receipt.impulses_applied){
   r.status=NF_NARROW_PENDING;return r;
  }
  r.impacts++;
 }
 if(left<=0){r.status=NF_NARROW_OK;return r;}
 /* Bounded causal frontier not resolved: no caller should publish bodies. */
 return r;
}
