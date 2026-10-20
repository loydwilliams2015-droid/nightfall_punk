#include "nf_multi_narrow18c1.h"
#include <stdio.h>
#include <string.h>
static unsigned checks;
#define CK(x) do{if(!(x)){fprintf(stderr,"FAIL line %d\n",__LINE__);return 1;}checks++;}while(0)
int main(void){
 for(unsigned family=0;family<5;family++)for(unsigned seed=0;seed<64;seed++){
  NfMulti18c1World w={0};w.count=2;w.revision=1;
  Nf18a4Body *a=&w.bodies[0],*b=&w.bodies[1];
  a->id=1;a->inverse_mass=1;a->radius=.3f;a->height=1.8f;
  a->center=(NfVec3){0,.9f,0};a->velocity=(NfVec3){4.f,0,0};
  b->id=2;b->inverse_mass=1;b->box_half=(NfVec3){.5f,.5f,.5f};
  b->center=(NfVec3){1.0f+(float)(seed%3u)*.2f,.5f,0};
  if(family==1)b->center.x=50;
  if(family==2)b->center.x=2.0f,a->velocity.x=120;
  if(family==3)b->radius=.3f,b->height=1.8f;
  if(family==4)b->center.x=.1f;
  NfMultiBroad broad={0};
  CK(nf_multi_broad18c1(&w,1.f/60.f,&broad));
  const NfMulti18c1World original=w;
  NfNarrowResult n=nf_multi_narrow18c1(&w,&broad,1.f/60.f);
  CK(!memcmp(&original,&w,sizeof w));
  if(family==1)CK(n.status==NF_NARROW_OK&&n.count==0);
  if(family==3&&broad.count)CK(n.status==NF_NARROW_PENDING&&n.count==0);
  if(family==4&&broad.count)CK(n.status==NF_NARROW_PENDING&&n.count==0);
  CK(n.count<=NF18A4_MAX_CONSTRAINTS);
 }
 printf("MA01B_CCD_H1 scenarios=320 checks=%u PASS\n",checks);return 0;
}
