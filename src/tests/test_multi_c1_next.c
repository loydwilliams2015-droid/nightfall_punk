#include "nf_multi_c1_next.h"
#include <stdio.h>
#include <math.h>
static unsigned checks;
#define CK(v) do{if(!(v)){fprintf(stderr,"FAIL %u\n",__LINE__);return 1;}checks++;}while(0)
int main(void){
 for(unsigned f=0;f<5;f++)for(unsigned s=0;s<128;s++){
  Nf18a4Body a={0},b={0};a.id=1;b.id=2;a.inverse_mass=b.inverse_mass=1;
  a.radius=b.radius=.35f;a.height=b.height=1.8f;
  a.center=(NfVec3){0,1,0};b.center=(NfVec3){2,1,0};
  a.velocity=(NfVec3){150,0,0};
  if(f==1)a.velocity.x=1;
  if(f==2)b.center.x=.1f;
  if(f==3)b.velocity.y=1;
  if(f==4)b.center.z=10;
  NfMultiCcd r=nf_multi_capsule_ccd18c1(&a,&b,1.f/60.f);
  if(f==0){CK(r.status==NF_NARROW_OK&&r.hit);CK(r.toi>=0&&r.toi<=1);
   Nf18a4Receipt impulse=nf18a4_apply_contact(&a,&b,r.contact,
    NF18A4_RECIPROCAL_LINEAR,1,1);
   CK(impulse.status==NF18A4_ACCEPTED&&impulse.impulses_applied);
   CK(b.velocity.x>0);
  }
  if(f==1||f==4)CK(r.status==NF_NARROW_OK&&!r.hit);
  if(f==2||f==3)CK(r.status==NF_NARROW_PENDING&&!r.hit);
  NfMa02Witness w={1,1,7,true,true,true,true};
  CK(nf_ma02_eligibility(1,1,&w)==NF_MA02_ALLOWED);
  w.material_known=false;CK(nf_ma02_eligibility(1,1,&w)==NF_MA02_PENDING);
  w.material_known=true;w.contract_grants=false;
  CK(nf_ma02_eligibility(1,1,&w)==NF_MA02_DENIED);
  w.contract_grants=true;w.authenticated=false;
  CK(nf_ma02_eligibility(1,1,&w)==NF_MA02_DENIED);
 }
 printf("MA01C_MA02_H1 scenarios=640 checks=%u PASS\n",checks);return 0;
}
