#include "nf_multi18c1.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
static unsigned long long checks=0;
#define CK(x) do{if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static NfMulti18c1World make_world(unsigned seed){
 NfMulti18c1World w={0};w.revision=1;w.count=3;
 for(unsigned i=0;i<3;++i){
  Nf18a4Body *b=&w.bodies[i];b->id=i+1;
  b->inverse_mass=1.f;
  b->center=(NfVec3){(float)i,0,0};
 }
 w.bodies[0].velocity=(NfVec3){-1.f-(float)(seed%4u)*.1f,0,0};
 w.bodies[1].velocity=(NfVec3){0,0,0};
 w.bodies[2].velocity=(NfVec3){0,0,0};
 return w;
}
static int scenario(unsigned group,unsigned seed){
 NfMulti18c1World w=make_world(seed),snapshot=w;
 NfMulti18c1Proposal p[2]={0};
 p[0].actor=1;p[0].action=1;p[0].revision=1;
 p[0].motor=(Nf18a4Motor){{-1.f,0,0},2.f,2.f};
 p[1].actor=3;p[1].action=2;p[1].revision=1;
 p[1].motor=(Nf18a4Motor){{0,0,0},2.f,2.f};
 Nf18a4Constraint c={
  .body_a=1,.body_b=2,.feature_id=17,
  .normal_b_to_a={1,0,0},.contact_point={.5f,0,0},
  .restitution=0,.friction=0
 };
 if(group==1)p[1].actor=1;
 if(group==2)p[1].revision=2;
 if(group==3)c.normal_b_to_a=(NfVec3){0,0,0};
 if(group==4)p[1].motor.max_force=-1;
 NfMulti18c1Candidate a,b;const NfMulti18c1Status s=nf_multi18c1_stage(
   &w,p,2,&c,1,1.f/60.f,&a);
 if(group){CK(s!=NF_MULTI_COMMITTED);CK(!memcmp(&w,&snapshot,sizeof w));return 0;}
 CK(s==NF_MULTI_COMMITTED);
 NfMulti18c1Proposal reversed[2]={p[1],p[0]};
 CK(nf_multi18c1_stage(&w,reversed,2,&c,1,1.f/60.f,&b)==NF_MULTI_COMMITTED);
 CK(!memcmp(&a.successor,&b.successor,sizeof a.successor));
 CK(a.contact.status==NF18A4_ACCEPTED);
 CK(a.contact.receipts[0].impulses_applied==1);
 /* Input mom before/after excluding external motor impulses: set target to
    original velocity in the reference no-motor scenario. */
 CK(a.successor.bodies[0].velocity.x>w.bodies[0].velocity.x);
 CK(a.successor.bodies[1].velocity.x<0);
 CK(!memcmp(&w,&snapshot,sizeof w));
 CK(nf_multi18c1_publish(&w,&a)==NF_MULTI_COMMITTED);
 CK(w.tick==1&&w.revision==2);
 CK(nf_multi18c1_publish(&w,&a)==NF_MULTI_STALE);
 return 0;
}
int main(void){
 for(unsigned g=0;g<5;++g){
  for(unsigned seed=0;seed<256;++seed)if(scenario(g,seed))return 1;
  printf("MULTI_A4 group=%u cases=256 PASS\n",g);
 }
 printf("MULTI_A4_H1 scenarios=1280 checks=%llu PASS\n",checks);
 return 0;
}
