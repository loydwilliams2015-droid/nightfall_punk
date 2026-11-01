#include "nf_vpc01_two_actor.h"
#include <float.h>
#include <math.h>
#include <string.h>
#include <limits.h>
/* H60 canonical VPC-01: two upright agents and a moveable pod. Horizontal
 * swept-disc envelopes approximate actor capsules and the pod's footprint.
 * A4 supplies actual reciprocal impulses; gravity, A5 material, B2/B3,
 * simultaneous manifolds and the network remain outside this bounded model. */
static float dot2(NfVec3 a,NfVec3 b){return a.x*b.x+a.z*b.z;}
static NfVec3 add_scaled(NfVec3 p,NfVec3 v,float t){
 return (NfVec3){p.x+v.x*t,p.y,p.z+v.z*t};
}
static void advance(NfVpc01World *w,float dt){
 for(size_t i=0;i<NF_VPC01_BODY_COUNT;++i)
  w->body[i].center=add_scaled(w->body[i].center,w->body[i].velocity,dt);
}
void nf_vpc01_init(NfVpc01World *w){
 if(!w)return;
 *w=(NfVpc01World){0};w->revision=1u;
 w->body[0]=(Nf18a4Body){.id=101u,.center={-2.0f,.95f,0},.radius=.38f,
   .height=1.9f,.inverse_mass=1.f/80.f};
 w->body[1]=(Nf18a4Body){.id=102u,.center={ 2.0f,.95f,0},.radius=.38f,
   .height=1.9f,.inverse_mass=1.f/80.f};
 w->body[2]=(Nf18a4Body){.id=201u,.center={0,.55f,3.0f},.radius=.56f,
   .box_half={.56f,.55f,.56f},.inverse_mass=1.f/30.f};
}
typedef struct {size_t a,b;float toi;bool found;} Hit;
static NfVpc01Status swept_hit(const NfVpc01World *w,float dt,Hit *out){
 *out=(Hit){0};
 for(size_t a=0;a<NF_VPC01_BODY_COUNT;++a){
  for(size_t b=a+1;b<NF_VPC01_BODY_COUNT;++b){
   const Nf18a4Body *x=&w->body[a],*y=&w->body[b];
   const NfVec3 p={x->center.x-y->center.x,0,x->center.z-y->center.z};
   const NfVec3 v={x->velocity.x-y->velocity.x,0,x->velocity.z-y->velocity.z};
   const float rr=x->radius+y->radius;
   const float c=dot2(p,p)-rr*rr;
   if(c<-.002f)return NF_VPC01_PENDING; /* preexisting deep penetration */
   const float aa=dot2(v,v),bv=dot2(p,v);
   if(aa<=1.e-10f||bv>=0)continue; /* static or separating */
   const float delta=bv*bv-aa*c;
   if(delta<0)continue;
   float t=(-bv-sqrtf(delta))/aa;
   if(t<0){if(c<.002f)t=0;else continue;}
   if(t>dt+1.e-6f)continue;
   if(!out->found||t<out->toi-1.e-5f){
    *out=(Hit){a,b,t,true};
   }else if(fabsf(t-out->toi)<=1.e-5f){
    return NF_VPC01_PENDING; /* unresolved simultaneous contacts */
   }
  }
 }
 return NF_VPC01_COMMITTED;
}
NfVpc01Status nf_vpc01_stage(const NfVpc01World *w,
 const NfVpc01Input *in,float dt,NfVpc01Candidate *out){
 if(!out)return NF_VPC01_INVALID;
 *out=(NfVpc01Candidate){0};
 if(!w||!in||!isfinite(dt)||dt<=0.f||dt>1.f/20.f||
    w->tick==UINT32_MAX||w->revision==UINT32_MAX)return NF_VPC01_INVALID;
 for(size_t i=0;i<NF_VPC01_BODY_COUNT;++i){
  if(!nf18a4_body_valid(&w->body[i])||w->body[i].radius<=0.f||
     (i&&w->body[i].id<=w->body[i-1].id))return NF_VPC01_INVALID;
 }
 NfVpc01World next=*w;
 for(size_t i=0;i<NF_VPC01_ACTOR_COUNT;++i){
  NfVec3 m=in->requested[i];
  if(!isfinite(m.x)||!isfinite(m.y)||!isfinite(m.z)||fabsf(m.y)>1.e-6f||
     fabsf(m.x)>1.f||fabsf(m.z)>1.f)return NF_VPC01_INVALID;
  float len=sqrtf(dot2(m,m));
  if(len>1.f){m.x/=len;m.z/=len;}
  Nf18a4MotorReceipt motor={0};
  Nf18a4Motor drive={{m.x*3.f,0,m.z*3.f},1400.f,17.5f};
  if(!nf18a4_motor_drive(&next.body[i],drive,dt,&motor))
   return NF_VPC01_INVALID;
 }
 /* Laboratory pod has friction-like damping, not a full ground contact. */
 next.body[2].velocity.x*=.993f;
 next.body[2].velocity.z*=.993f;
 Hit h={0};
 if(swept_hit(&next,dt,&h)!=NF_VPC01_COMMITTED)return NF_VPC01_PENDING;
 if(h.found){
  advance(&next,h.toi);
  Nf18a4Body *a=&next.body[h.a],*b=&next.body[h.b];
  NfVec3 n={a->center.x-b->center.x,0,a->center.z-b->center.z};
  const float distance=sqrtf(dot2(n,n));
  if(distance<1.e-6f)return NF_VPC01_PENDING;
  n.x/=distance;n.z/=distance;
  Nf18a4Constraint contact={0};
  contact.body_a=a->id;contact.body_b=b->id;
  contact.feature_id=1u;contact.normal_b_to_a=n;
  contact.contact_point=(NfVec3){a->center.x-n.x*a->radius,
    (a->center.y+b->center.y)*.5f,a->center.z-n.z*a->radius};
  contact.restitution=.08f;contact.friction=0.f;
  Nf18a4Receipt receipt=nf18a4_apply_contact(
   a,b,contact,NF18A4_RECIPROCAL_LINEAR,w->tick+1u,w->revision);
  if(receipt.status!=NF18A4_ACCEPTED||!receipt.impulses_applied)
   return NF_VPC01_PENDING;
  /* A second collision in the remaining interval needs a real manifold.
     Deliberately refuse rather than pretend sequential ordering is accepted. */
  Hit second={0};
  if(swept_hit(&next,dt-h.toi,&second)!=NF_VPC01_COMMITTED||
     second.found)return NF_VPC01_PENDING;
  advance(&next,dt-h.toi);
  next.contact_events++;
  out->contact=receipt;out->contact_applied=true;
 }else advance(&next,dt);
 for(size_t i=0;i<NF_VPC01_BODY_COUNT;++i)
  if(!nf18a4_body_valid(&next.body[i]))return NF_VPC01_INVALID;
 next.tick++;next.revision++;
 out->from_tick=w->tick;out->from_revision=w->revision;
 out->next=next;
 return NF_VPC01_COMMITTED;
}
NfVpc01Status nf_vpc01_publish(NfVpc01World *w,const NfVpc01Candidate *c){
 if(!w||!c||w->tick!=c->from_tick||w->revision!=c->from_revision||
    c->next.tick!=w->tick+1u||c->next.revision!=w->revision+1u||
    c->next.contact_events<w->contact_events||
    c->next.body[0].id!=101u||c->next.body[1].id!=102u||
    c->next.body[2].id!=201u)return NF_VPC01_STALE;
 *w=c->next;
 return NF_VPC01_COMMITTED;
}
