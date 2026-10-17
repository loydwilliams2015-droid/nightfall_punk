#include "nf_persist18b5.h"
#include <stdio.h>
#include <stdint.h>
static uint32_t rng=0xB518CAFEu;
static uint32_t next(void){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
static bool yes(void *p,const NfB5State *s){*(NfB5State *)p=*s;return true;}
static bool no(void *p,const NfB5State *s){(void)p;(void)s;return false;}
int main(void){const char *names[]={"canonical","randomized","pathological","boundary","heldout"};
 const unsigned counts[]={128,1024,256,256,1024};unsigned tested=0,bad=0;
 for(unsigned stratum=0;stratum<5;stratum++){
  if(stratum==4)rng=0xB518A11Du;
  unsigned failures=0;
  for(unsigned i=0;i<counts[stratum];i++){
   NfB5State s={.epoch=1,.balance=100},disk=s,before=s;
   uint32_t x=next();
   NfB5Event e={.actor=1+(x&3u),.action=1+(i&63u),
    .object=1+((x>>5)&7u),.epoch=1,.debit=(int32_t)((x>>9)%120u)};
   bool persist=(x&8u)!=0;
   if(stratum==0){e.debit=10;persist=true;}
   if(stratum==2){e.epoch=(x&16u)?2u:1u;persist=false;}
   if(stratum==3){e.debit=(i&1u)?100:101;persist=true;}
   NfB5Result got=nf_b5_apply(&s,e,persist?yes:no,&disk);
   NfB5Result want=e.epoch!=1?NF_B5_STALE:
     e.debit>100?NF_B5_INVALID:(!persist?NF_B5_PERSIST_FAIL:NF_B5_APPLIED);
   if(got!=want||((got==NF_B5_APPLIED)!=(!nf_b5_equal(&s,&before)))||
      (got!=NF_B5_APPLIED&&!nf_b5_equal(&disk,&before))||
      (got==NF_B5_APPLIED&&!nf_b5_equal(&s,&disk)))failures++;
   tested++;
  }
  bad+=failures;printf("%s cases=%u errors=%u\n",names[stratum],counts[stratum],failures);
 }
 printf("total=%u mismatch=%u\n",tested,bad);return bad?1:0;
}
