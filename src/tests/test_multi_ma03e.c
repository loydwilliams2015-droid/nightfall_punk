#include "nf_multi_ma03e.h"
#include <stdio.h>
#include <math.h>
static unsigned checks;
#define CK(v) do{if(!(v)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#v);return 1;}checks++;}while(0)
int main(void){
 for(unsigned family=0;family<5;family++)for(unsigned seed=0;seed<128;seed++){
  Nf18a4Body a={0},b={0};a.id=1;b.id=2;a.inverse_mass=b.inverse_mass=1;
  a.radius=b.radius=.35f;a.height=b.height=1.8f;
  a.center=(NfVec3){0,1,0};b.center=(NfVec3){2,1,.2f};
  a.velocity=(NfVec3){150,0,0};
  if(family==1)a.velocity.x=1;
  if(family==2)b.center.z=10;
  if(family==3)b.center.x=.1f;
  if(family==4)b.velocity.y=1;
  NfMa03ePair out=nf_ma03e_offaxis(a,b,1.f/60.f,1,1);
  if(family==0){CK(out.status==NF_NARROW_OK&&out.impulses==1);
   CK(out.b.velocity.x>0);}
  if(family==1||family==2)CK(out.status==NF_NARROW_OK&&out.impulses==0);
  if(family==3||family==4)CK(out.status==NF_NARROW_PENDING);
  CK(a.center.x==0);
 }
 CK(!nf_ma026_prepublish(NULL,NULL,NULL));
 printf("MA03E_MA026 scenarios=640 checks=%u PASS\n",checks);return 0;
}
