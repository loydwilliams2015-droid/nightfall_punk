#include "nf_multi_ma03b.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
static unsigned checks;
#define CK(q) do{if(!(q)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#q);return 1;}checks++;}while(0)
static bool missing(void *p,const NfWorld *w,const Nf18a6Runtime *r,uint32_t action,Nf18b2Scene *scene,Nf18b2Use *use){
 (void)p;(void)w;(void)r;(void)action;(void)scene;(void)use;return false;
}
int main(void){
 for(unsigned family=0;family<5;family++)for(unsigned seed=0;seed<64;seed++){
  const double m[3]={1+(seed%5),2+(seed%7),3+(seed%11)};
  double x[3]={0,1.2,2.4},v[3]={12+(seed%3),0,0};
  if(family==1)v[0]=0;
  if(family==2)x[1]=.1;
  if(family==3)v[0]=24;
  if(family==4)x[2]=5;
  const double radius=.25,dt=.25;
  NfMa03bChain expected=nf_ma03b_oracle(x,v,m,radius,dt);
  if(family==2){CK(!expected.complete);continue;}
  CK(expected.complete);
  Nf18a4Body b[3]={0};
  for(int i=0;i<3;i++){b[i].id=i+1;b[i].inverse_mass=(float)(1/m[i]);
   b[i].radius=(float)radius;b[i].height=1.8f;b[i].center=(NfVec3){(float)x[i],1,0};
   b[i].velocity.x=(float)v[i];}
  double left=dt;unsigned hits=0;
  for(unsigned iteration=0;iteration<12;iteration++){
   double event=left;int pair=-1;
   for(int i=0;i<2;i++){
    double closing=(double)b[i].velocity.x-b[i+1].velocity.x;
    if(closing<=1.e-7)continue;
    double gap=(double)b[i+1].center.x-b[i].center.x-2*radius;
    double time=gap/closing;
    if(time>=-1.e-7&&time<event){event=fmax(0,time);pair=i;}
   }
   for(int i=0;i<3;i++)b[i].center.x+=(float)(b[i].velocity.x*event);
   left-=event;
   if(pair<0)break;
   Nf18a4Constraint c={0};c.body_a=b[pair].id;c.body_b=b[pair+1].id;
   c.feature_id=10+pair;c.normal_b_to_a=(NfVec3){-1,0,0};
   c.contact_point=(NfVec3){b[pair+1].center.x-(float)radius,1,0};
   c.restitution=1.f;
   Nf18a4Receipt r=nf18a4_apply_contact(&b[pair],&b[pair+1],c,NF18A4_RECIPROCAL_LINEAR,1,1);
   CK(r.status==NF18A4_ACCEPTED&&r.impulses_applied);hits++;
   if(left<=0)break;
  }
  CK(hits==expected.impacts);
  double p0=0,p1=0;
  for(int i=0;i<3;i++){p0+=m[i]*v[i];p1+=m[i]*b[i].velocity.x;
   CK(fabs(b[i].velocity.x-expected.v[i])<.004);
   CK(fabs(b[i].center.x-expected.x[i])<.015);}
  CK(fabs(p1-p0)<.005);
 }
 NfWorld *w=calloc(1,sizeof *w);Nf18a6Runtime *r=calloc(1,sizeof *r);
 CK(w&&r);
 CK(nf_ma023_recheck(w,r,missing,NULL,1,NF18B2_M4_MATERIAL_FIRST).status==NF18B2_INVALID);
 free(r);free(w);
 printf("MA03B_MA023 scenarios=320 checks=%u PASS\n",checks);
 return 0;
}
