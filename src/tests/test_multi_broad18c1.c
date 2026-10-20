#include "nf_multi_broad18c1.h"
#include <stdio.h>
#include <string.h>
static unsigned long long checks;
#define CK(x) do{if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);return 1;}++checks;}while(0)
static uint32_t rng(uint32_t *s){*s^=*s<<13;*s^=*s>>17;*s^=*s<<5;return *s;}
static int test(unsigned family,unsigned seed){
 NfMulti18c1World w={0};w.revision=1;w.count=8;
 uint32_t s=seed*1103u+family*167u+1u;
 for(unsigned i=0;i<8;i++){
  Nf18a4Body *b=&w.bodies[i];
  b->id=10+i;b->inverse_mass=1;
  b->box_half=(NfVec3){.45f,.45f,.45f};
  uint32_t r=rng(&s);
  b->center=(NfVec3){(float)(i*5u)+(float)(r%3u),0,0};
  b->velocity=(NfVec3){(float)((int)(r%7u)-3),0,0};
 }
 if(family==1){w.bodies[0].center.x=0;w.bodies[1].center.x=10;
   w.bodies[0].velocity.x=600;w.bodies[1].velocity.x=-600;}
 if(family==2){w.bodies[0].center.x=0;w.bodies[1].center.x=.5f;}
 if(family==3)w.bodies[2].velocity.x=9000.f;
 if(family==4){w.bodies[0].radius=.4f;w.bodies[0].height=1.8f;
   w.bodies[0].box_half=(NfVec3){0,0,0};}
 NfMultiBroad a={0},b={0};
 CK(nf_multi_broad18c1(&w,1.f/60.f,&a));
 NfMulti18c1World reversed=w;
 for(unsigned i=0;i<4;i++){
  Nf18a4Body tmp=reversed.bodies[i];reversed.bodies[i]=reversed.bodies[7-i];
  reversed.bodies[7-i]=tmp;
 }
 CK(nf_multi_broad18c1(&reversed,1.f/60.f,&b));
 CK(a.count==b.count&&a.tested_pairs==28&&b.tested_pairs==28);
 for(size_t i=0;i<a.count;i++){
  CK(a.pairs[i].id_a==b.pairs[i].id_a&&a.pairs[i].id_b==b.pairs[i].id_b);
  CK(a.pairs[i].id_a<a.pairs[i].id_b);
 }
 if(family==1){
  bool found=false;
  for(size_t i=0;i<a.count;++i)if(a.pairs[i].id_a==10&&a.pairs[i].id_b==11)found=true;
  CK(found);
 }
 CK(w.revision==1&&w.count==8);
 return 0;
}
int main(void){
 for(unsigned family=0;family<5;++family){
  for(unsigned seed=0;seed<256;++seed)if(test(family,seed))return 1;
  printf("MA01 family=%u scenarios=256 PASS\n",family);
 }
 printf("MA01_BROADPHASE_PASS scenarios=1280 checks=%llu\n",checks);
 return 0;
}
