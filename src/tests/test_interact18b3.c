#include "nf_interact18b3.h"
#include <assert.h>
#include <stdio.h>
static bool yes(void *ctx,const Nf18b3Event *e){(void)e;unsigned *n=ctx;++*n;return true;}
static bool no(void *ctx,const Nf18b3Event *e){(void)ctx;(void)e;return false;}
int main(void){unsigned n=0,checks=0;
#define C(x) do{assert(x);++checks;}while(0)
 for(unsigned k=0;k<NF18B3_KIND_COUNT;++k){
  Nf18b3Event e={1,2,3,4,5,6,7,(Nf18b3Kind)k,NF18B3_PROPOSED,0,0};
  C(nf18b3_authorize(&e,true,true,false)==NF18B3_OK);
  C(nf18b3_realize(&e,9,6,7,yes,&n)==NF18B3_DENIED);
  C(n==k);
  e.phase=NF18B3_PROPOSED;e.authorized=0;
  C(nf18b3_authorize(&e,true,true,false)==NF18B3_OK);
  C(nf18b3_realize(&e,5,6,7,no,&n)==NF18B3_PENDING);
  C(!e.applied);
  C(nf18b3_realize(&e,5,6,7,yes,&n)==NF18B3_OK);
  C(nf18b3_realize(&e,5,6,7,yes,&n)==NF18B3_DUPLICATE);
  C(nf18b3_interrupt(&e)==NF18B3_OK);
  C(e.applied);
 }
 C(n==NF18B3_KIND_COUNT);
 printf("B3 typed H0 unit assertions=%u PASS (not integrated physics)\n",checks);
 return 0;
}
