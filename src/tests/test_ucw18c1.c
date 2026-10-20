#define _POSIX_C_SOURCE 200809L
#include "nf_ucw18c1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
static unsigned long long assertions=0;
#define T(x) do{if(!(x)){fprintf(stderr,"FAIL line %d %s\n",__LINE__,#x);return 1;}assertions++;}while(0)
static uint32_t random32(uint32_t *s){*s^=*s<<13;*s^=*s>>17;*s^=*s<<5;return *s;}
static int test(unsigned family,unsigned seed){
 UcwWorld initial;ucw_init(&initial,1000,1000);
 UcwCandidate a={0},b={0};
 uint32_t rand=0x18C10011u+family*1009u+seed*17u;
 UcwProposal p[4];
 for(unsigned i=0;i<4;++i){
  uint32_t x=random32(&rand);
  p[i]=(UcwProposal){.actor=i+1,.action=100+i,
     .object=100+i,.contract=200+i,.authority_epoch=1,
     .read_revision=1,.priority=x%13u,
     .material_delta=-(int32_t)(x%10u),
     .contract_delta=-(int32_t)((x>>5)%10u)};
 }
 if(family==1)p[1].object=p[0].object;
 if(family==2)p[2].contract=p[3].contract;
 if(family==3)p[1].authority_epoch=4;
 if(family==4)p[1].material_delta=-2000;
 UcwStatus expected=family==1||family==2?UCW_CONFLICT:
                     family==3?UCW_STALE:family==4?UCW_INVALID:UCW_OK;
 T(ucw_stage(&initial,p,4,&a)==expected);
 if(expected!=UCW_OK){T(!a.ready);return 0;}
 /* Same claims, different proposal arrival order: fixed canonical order. */
 UcwProposal backwards[4];for(unsigned i=0;i<4;++i)backwards[i]=p[3-i];
 T(ucw_stage(&initial,backwards,4,&b)==UCW_OK);
 T(a.ready&&b.ready);
 T(a.staged.history_hash==b.staged.history_hash);
 T(a.staged.material==b.staged.material&&a.staged.contracts==b.staged.contracts);
 T(a.staged.tick==1&&a.staged.revision==2);
 T(a.staged.material<=1000&&a.staged.contracts<=1000);
 T(ucw_stage(&a.staged,p,4,&b)==UCW_DUPLICATE||
   ucw_stage(&a.staged,p,4,&b)==UCW_STALE);
 UcwWorld suspended=a.staged;
 suspended.pending_frontier=5;suspended.resolution=UCW_PENDING;
 T(ucw_stage(&suspended,p,0,&b)==UCW_INVALID);
 T(ucw_frontier_step(&suspended,3)==UCW_UNCERTAIN);
 T(suspended.pending_frontier==2&&suspended.resolution==UCW_PENDING);
 T(ucw_frontier_step(&suspended,2)==UCW_OK);
 T(suspended.pending_frontier==0&&suspended.resolution==UCW_RESOLVED);
 return 0;
}
static int journal(void){
 char path[]="/tmp/ucw18c1_XXXXXX";
 int fd=mkstemp(path);T(fd>=0);close(fd);
 UcwJournal j;
 T(ucw_open(&j,path,100,50)==UCW_OK);
 UcwWorld w=j.state;UcwCandidate c={0};
 T(ucw_stage(&w,NULL,0,&c)==UCW_OK);
 T(ucw_commit(&j,&c)==UCW_OK);
 T(j.state.tick==1);
 ucw_close(&j);
 T(ucw_open(&j,path,100,50)==UCW_OK);
 T(j.state.tick==1&&j.state.material==100);
 UcwProposal p={.actor=1,.action=99,.object=7,.contract=9,
 .authority_epoch=2,.read_revision=2,.material_delta=-4,.contract_delta=-1};
 T(ucw_stage(&j.state,&p,1,&c)==UCW_OK);
 T(ucw_commit(&j,&c)==UCW_INVALID); /* no false durable B3 claim */
 T(j.state.tick==1);
 ucw_close(&j);
 fd=open(path,1|1024);T(fd>=0);T(write(fd,"bad",3)==3);close(fd);
 T(ucw_open(&j,path,100,50)==UCW_UNCERTAIN&&j.quarantine);
 ucw_close(&j);unlink(path);
 return 0;
}
int main(void){
 for(unsigned family=0;family<5;++family){
  for(unsigned seed=0;seed<256;++seed)if(test(family,seed))return 1;
  printf("stratum=%u scenarios=256 PASS\n",family);
 }
 if(journal())return 1;
 printf("UCW_C1_H0_PASS scenarios=1280 assertions=%llu\n",assertions);
 return 0;
}
