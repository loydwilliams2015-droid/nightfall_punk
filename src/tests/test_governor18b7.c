#include "nf_governor18b7.h"
#include <stdio.h>
#include <string.h>
static unsigned long long checks;
#define CK(x) do{if(!(x)){fprintf(stderr,"FAIL:%d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static uint32_t rnd(uint32_t *x){*x^=*x<<13;*x^=*x>>17;*x^=*x<<5;return *x;}
int main(void){
 Nf4gState s;nf4g_init(&s);
 Nf4gMetrics healthy={.budget_us=16667,.used_us=6000,
  .telemetry_valid=true,.journal_verified=true,.optional_backlog=10};
 CK(s.mode==NF4G_READ_ONLY);
 for(unsigned i=0;i<3;++i){CK(nf4g_step(&s,healthy)==NF4G_READ_ONLY);
  CK(s.granted_optional_us==0);}
 CK(nf4g_step(&s,healthy)==NF4G_NORMAL);
 CK(s.granted_optional_us==250);
 Nf4gMetrics m=healthy;m.critical_backlog=1;
 CK(nf4g_step(&s,m)==NF4G_THROTTLED);CK(!s.granted_optional_us);
 m=healthy;m.uncertain_disk=true;
 CK(nf4g_step(&s,m)==NF4G_READ_ONLY);CK(!s.granted_optional_us);
 m=healthy;m.journal_verified=false;
 CK(nf4g_step(&s,m)==NF4G_READ_ONLY);
 m=healthy;m.used_us=16000;
 CK(nf4g_step(&s,m)==NF4G_THROTTLED);
 m=healthy;m.used_us=20000;
 CK(nf4g_step(&s,m)==NF4G_READ_ONLY);
 m=healthy;m.telemetry_valid=false;
 CK(nf4g_step(&s,m)==NF4G_THROTTLED);
 for(unsigned i=0;i<4;++i)nf4g_step(&s,healthy);
 CK(s.mode==NF4G_NORMAL);
 m=healthy;m.optional_backlog=0;
 CK(nf4g_step(&s,m)==NF4G_NORMAL&&s.granted_optional_us==0);
 uint32_t seed=0x40BAD55u;
 for(unsigned i=0;i<10000;++i){
  uint32_t x=rnd(&seed);
  Nf4gMetrics input={.budget_us=1u+(x%40000u),
   .used_us=(x>>7)%50000u,.critical_backlog=(x>>11)%4u,
   .optional_backlog=(x>>15)%40u,.telemetry_valid=(bool)((x>>19)&1u),
   .journal_verified=(bool)((x>>20)&1u),.uncertain_disk=(bool)((x>>21)&1u)};
  Nf4gState a=s,b=s;
  Nf4gMode ka=nf4g_step(&a,input),kb=nf4g_step(&b,input);
  CK(ka==kb&&memcmp(&a,&b,sizeof a)==0);
  CK(a.granted_optional_us<=250u);
  CK(!(a.mode!=NF4G_NORMAL&&a.granted_optional_us));
  if(!input.journal_verified||input.uncertain_disk||
     input.used_us>input.budget_us)CK(a.mode==NF4G_READ_ONLY);
  s=a;
 }
 printf("governor_inputs=10000 checks=%llu PASS\n",checks);
 return 0;
}
