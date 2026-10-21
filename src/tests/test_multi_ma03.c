#include "nf_multi_ma03.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
static unsigned long long checks;
#define CK(q) do{if(!(q)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#q);return 1;}checks++;}while(0)
static bool forged(void *ctx,const NfWorld *w,const Nf18a6Runtime *r,uint32_t action,Nf18b2Scene *s,Nf18b2Use *u){
 (void)ctx;(void)w;(void)r;(void)action;memset(s,0,sizeof *s);memset(u,0,sizeof *u);return true;
}
static int trial(unsigned family,unsigned seed){
 float x[3]={0,1.1f,2.2f},v[3]={12.f+(float)(seed%3),0,0};
 if(family==1)v[0]=0;
 if(family==2)x[1]=.1f;
 if(family==3)v[0]=30.f;
 if(family==4)x[2]=5.f;
 const float radius=.25f,dt=.25f;
 NfMa03Chain expected=nf_ma03_chain_oracle(x,v,radius,dt);
 if(family==2){CK(!expected.complete);return 0;}
 CK(expected.complete);
 Nf18a4Body bodies[3]={0};
 for(int i=0;i<3;i++){
  bodies[i].id=(uint32_t)(i+1);bodies[i].inverse_mass=1;
  bodies[i].radius=radius;bodies[i].height=1.8f;
  bodies[i].center=(NfVec3){x[i],1,0};bodies[i].velocity.x=v[i];
 }
 /* Independently calculate collision order from 1D closed-form world and
  * use actual A4 contact solver at each impact to compare impulse behavior.
  * This is intentionally NOT the MA01C2 production scheduler. */
 double now=0;unsigned impacts=0;
 for(unsigned iter=0;iter<8;iter++){
  double time=(double)dt-now;int pair=-1;
  for(int i=0;i<2;i++){
   double approaching=(double)bodies[i].velocity.x-bodies[i+1].velocity.x;
   if(approaching<=0)continue;
   double gap=(double)bodies[i+1].center.x-bodies[i].center.x-2*radius;
   double t=gap/approaching;
   if(t>=0&&t<time){time=t;pair=i;}
  }
  for(int i=0;i<3;i++)bodies[i].center.x+=(float)(time*bodies[i].velocity.x);
  now+=time;
  if(pair<0)break;
  Nf18a4Constraint c={0};c.body_a=bodies[pair].id;c.body_b=bodies[pair+1].id;
  c.feature_id=10u+(uint32_t)pair;c.normal_b_to_a=(NfVec3){-1,0,0};
  c.contact_point=(NfVec3){bodies[pair+1].center.x-radius,1,0};
  c.restitution=1.f;
  Nf18a4Receipt r=nf18a4_apply_contact(&bodies[pair],&bodies[pair+1],c,
                                     NF18A4_RECIPROCAL_LINEAR,1,1);
  CK(r.status==NF18A4_ACCEPTED&&r.impulses_applied);
  impacts++;
 }
 CK(impacts==expected.impacts);
 for(int i=0;i<3;i++){
  CK(fabsf(bodies[i].velocity.x-expected.v[i])<.003f);
  CK(fabsf(bodies[i].center.x-expected.x[i])<.004f);
 }
 NfWorld *world=calloc(1,sizeof *world);Nf18a6Runtime *runtime=calloc(1,sizeof *runtime);
 CK(world&&runtime);
 CK(nf_ma03_b2_owner_evaluate(world,runtime,forged,NULL,33,NF18B2_M4_MATERIAL_FIRST).status==NF18B2_INVALID);
 free(runtime);free(world);
 return 0;
}
int main(void){
 for(unsigned f=0;f<5;f++)for(unsigned s=0;s<64;s++)if(trial(f,s))return 1;
 printf("MA03_CHAIN_OWNER_TEST scenarios=320 checks=%llu PASS\n",checks);return 0;
}
