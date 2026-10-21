#include "nf_multi_ma03c.h"
#include <stdio.h>
#include <string.h>
static unsigned checks;
#define CK(x) do{if(!(x)){fprintf(stderr,"FAIL line %d %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static bool unavailable(void *ctx,const NfWorld *w,const Nf18a6Runtime *r,uint32_t action,Nf18b2Scene *s,Nf18b2Use *u){
 (void)ctx;(void)w;(void)r;(void)action;(void)s;(void)u;return false;
}
int main(void){
 for(unsigned family=0;family<5;family++)for(unsigned seed=0;seed<128;seed++){
  Nf18a4Body b[3]={0};
  for(unsigned i=0;i<3;i++){b[i].id=i+1;b[i].inverse_mass=1.f/(1.f+(float)(i+seed%3));
   b[i].radius=.3f;b[i].height=1.8f;b[i].center.y=1;}
  b[0].velocity.x=150;b[1].center.x=2;b[2].center.x=4;
  if(family==1)b[0].velocity.x=1;
  if(family==2)b[1].center.x=.1f;
  if(family==3)b[1].velocity.y=2;
  if(family==4)b[1].center.z=10;
  NfMa03cEvidence e=nf_ma03c_compare(b,family==4?3:2,1.f/60.f,1,1);
  if(family==0)CK(e.matched==1&&e.disagreed==0);
  else CK(e.unresolved==1&&e.disagreed==0);
 }
 NfWorld w={0};Nf18a6Runtime r={0};NfMa024Receipt receipt={0};
 CK(!nf_ma024_record(&w,&r,unavailable,NULL,7,NF18B2_M4_MATERIAL_FIRST,&receipt));
 CK(!nf_ma024_recheck(&w,&r,unavailable,NULL,7,NF18B2_M4_MATERIAL_FIRST,&receipt));
 printf("MA03C_MA024 scenarios=640 checks=%u PASS\n",checks);return 0;
}
