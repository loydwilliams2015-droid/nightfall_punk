#include "nf_multi_ma03d.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#define CK(v) do{if(!(v)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#v);return 1;}checks++;}while(0)
static unsigned checks;
int main(void){
 for(unsigned f=0;f<5;f++)for(unsigned s=0;s<64;s++){
  double x[3]={0,1.2,2.4},v[3]={12+(s%3),0,0},m[3]={1+s%5,2+s%7,3+s%11};
  if(f==1)v[0]=0;
  if(f==2)x[1]=.1;
  if(f==3)v[0]=24;
  if(f==4)x[2]=5;
  NfMa03bChain oracle=nf_ma03b_oracle(x,v,m,.25,.6);
  Nf18a4Body b[3]={0};
  for(int i=0;i<3;i++){b[i].id=i+1;b[i].inverse_mass=(float)(1/m[i]);b[i].radius=.25f;
   b[i].height=1.8f;b[i].center=(NfVec3){(float)x[i],1,0};b[i].velocity.x=(float)v[i];}
  NfMa01c2Result got=nf_ma03d_three_body(b,.6f,1,1);
  if(f==2){CK(got.status==NF_NARROW_PENDING);continue;}
  CK(oracle.complete&&got.status==NF_NARROW_OK);
  CK(got.impacts==oracle.impacts);
  for(int i=0;i<3;i++){CK(fabs(got.bodies[i].center.x-oracle.x[i])<.02);
   CK(fabs(got.bodies[i].velocity.x-oracle.v[i])<.005);}
 }
 NfWorld w={0};Nf18a6Runtime runtime={0};NfMa024Receipt r={0};
 CK(!nf_ma025_prepublication(&w,&runtime,NULL,NULL,1,NF18B2_M4_MATERIAL_FIRST,&r));
 printf("MA03D_MA025 scenarios=320 assertions=%u PASS\n",checks);return 0;
}
