#include "nf_vpc01_two_actor.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
static unsigned checks=0;
#define CK(v) do{if(!(v)){fprintf(stderr,"VPC-01 H60 failure line %d: %s\n",__LINE__,#v);return 1;}++checks;}while(0)
static int two_actor(uint32_t seed){
 NfVpc01World a={0},b={0};nf_vpc01_init(&a);nf_vpc01_init(&b);
 const float offset=(float)(seed%17u)*.0125f;
 a.body[1].center.z=offset;b.body[1].center.z=offset;
 bool contact=false;
 for(unsigned tick=0;tick<110u;++tick){
  NfVpc01Input in={0};
  if(tick<75u){
    in.requested[0].x=1.0f;
    in.requested[1].x=-1.0f;
  }
  const NfVpc01World before=a;
  NfVpc01Candidate ca={0},cb={0};
  CK(nf_vpc01_stage(&a,&in,1.f/60.f,&ca)==NF_VPC01_COMMITTED);
  CK(nf_vpc01_stage(&b,&in,1.f/60.f,&cb)==NF_VPC01_COMMITTED);
  CK(!memcmp(&ca,&cb,sizeof ca)); /* source-exact deterministic pair */
  CK(!memcmp(&a,&before,sizeof a));
  if(ca.contact_applied){
    CK(ca.contact.status==NF18A4_ACCEPTED);
    CK(ca.contact.impulses_applied==1);
    CK(ca.contact.tick==a.tick+1u);
    contact=true;
  }
  CK(nf_vpc01_publish(&a,&ca)==NF_VPC01_COMMITTED);
  CK(nf_vpc01_publish(&b,&cb)==NF_VPC01_COMMITTED);
  CK(!memcmp(&a,&b,sizeof a));
  const float dx=a.body[0].center.x-a.body[1].center.x;
  const float dz=a.body[0].center.z-a.body[1].center.z;
  const float dist=sqrtf(dx*dx+dz*dz);
  if(dist+0.002f<a.body[0].radius+a.body[1].radius){
   fprintf(stderr,"VPC-01 penetration seed=%u tick=%u a=(%.6f,%.6f) b=(%.6f,%.6f) dist=%.6f contacts=%u\\n",
      seed,a.tick,a.body[0].center.x,a.body[0].center.z,
      a.body[1].center.x,a.body[1].center.z,dist,a.contact_events);
   return 1;
  }
 }
 CK(contact&&a.contact_events>=1u);
 if(seed==0u){
  /* Exact head-on collision is collinear: crossing would be a real defect. */
  if(!(a.body[0].center.x<a.body[1].center.x)){
   fprintf(stderr,"VPC-01 head-on crossing: a=%.6f b=%.6f contacts=%u\\n",
     a.body[0].center.x,a.body[1].center.x,a.contact_events);
   return 1;
  }
 }
 /* Lateral deflection can legitimately exchange actors' x-order. Never
    mistake x-order for nonpenetration in a two-dimensional contact model. */
 return 0;
}
static int pod_push(void){
 NfVpc01World w={0};nf_vpc01_init(&w);
 w.body[0].center=(NfVec3){-2.5f,.95f,3.f};
 w.body[1].center=(NfVec3){5.f,.95f,5.f};
 NfVpc01Input in={0};in.requested[0].x=1.f;
 const float before=w.body[2].center.x;
 unsigned contacts=0;
 for(unsigned tick=0;tick<105u;++tick){
  NfVpc01Candidate c={0};
  CK(nf_vpc01_stage(&w,&in,1.f/60.f,&c)==NF_VPC01_COMMITTED);
  if(c.contact_applied){CK(c.contact.impulses_applied==1);contacts++;}
  CK(nf_vpc01_publish(&w,&c)==NF_VPC01_COMMITTED);
 }
 CK(contacts>=1u);
 CK(w.body[2].center.x>before+.05f);
 CK(w.body[0].center.x<w.body[2].center.x);
 return 0;
}
static int negative_controls(void){
 NfVpc01World w={0};nf_vpc01_init(&w);
 const NfVpc01World frozen=w;
 NfVpc01Input in={0};NfVpc01Candidate c={0};
 in.requested[0].x=NAN;
 CK(nf_vpc01_stage(&w,&in,1.f/60.f,&c)==NF_VPC01_INVALID);
 CK(!memcmp(&w,&frozen,sizeof w));
 in=(NfVpc01Input){0};in.requested[0].y=1.f;
 CK(nf_vpc01_stage(&w,&in,1.f/60.f,&c)==NF_VPC01_INVALID);
 in=(NfVpc01Input){0};
 CK(nf_vpc01_stage(&w,&in,0.f,&c)==NF_VPC01_INVALID);
 CK(nf_vpc01_stage(&w,&in,.2f,&c)==NF_VPC01_INVALID);
 w.body[1].center=w.body[0].center;
 CK(nf_vpc01_stage(&w,&in,1.f/60.f,&c)==NF_VPC01_PENDING);
 CK(c.next.tick==0u);
 w=frozen;
 CK(nf_vpc01_stage(&w,&in,1.f/60.f,&c)==NF_VPC01_COMMITTED);
 CK(nf_vpc01_publish(&w,&c)==NF_VPC01_COMMITTED);
 CK(nf_vpc01_publish(&w,&c)==NF_VPC01_STALE);
 CK(w.tick==1u&&w.revision==2u);
 CK(nf_vpc01_publish(NULL,&c)==NF_VPC01_STALE);
 return 0;
}
int main(void){
 for(uint32_t seed=0u;seed<32u;++seed)if(two_actor(seed))return 1;
 if(pod_push()||negative_controls())return 1;
 printf("VPC01_H60 primary intentional physics: 2 actors, 1 pod; 32 replay seeds; A4 reciprocal contact, push, invalid and stale cases; checks=%u PASS\n",checks);
 return 0;
}
