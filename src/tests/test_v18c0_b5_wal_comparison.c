#define _POSIX_C_SOURCE 200809L
/* 1.8C.0 H0 incumbent/challenger: B5 mock callback versus actual POSIX
 * WAL, same restricted debit semantics. Independent integer oracle.
 * Neither implementation commits A4/A5 body state or B3 receipts.
 */
#include "nf_persist18b5.h"
#include "nf_wal18b7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
static unsigned long long assertions;
#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}assertions++;}while(0)
static uint32_t rnd(uint32_t *v){*v^=*v<<13;*v^=*v>>17;*v^=*v<<5;return *v;}
static bool accept_save(void *arg,const NfB5State *next){
 (void)arg;return next&&next->epoch>=1;
}
static bool reject_save(void *arg,const NfB5State *next){
 (void)arg;(void)next;return false;
}
typedef struct {uint32_t epoch,count;int64_t balance;uint32_t actor[64],action[64];int32_t debit[64];} Oracle;
static int trial(unsigned family,unsigned seed,unsigned *checks){
 char path[]="/tmp/nf18c0_XXXXXX";
 int fd=mkstemp(path);REQUIRE(fd>=0);close(fd);
 NfB5State *inc=calloc(1,sizeof *inc);
 Nf4Wal *wal=calloc(1,sizeof *wal);
 REQUIRE(inc&&wal);
 inc->epoch=1;inc->balance=100;
 Oracle oracle={.epoch=1,.balance=100};
 REQUIRE(nf4_open(wal,path,100)==NF4_APPLIED);
 uint32_t rng=0xC0180001u+(family+1u)*5003u+seed*101u;
 for(unsigned step=0;step<35;++step){
  uint32_t n=rnd(&rng);
  uint32_t actor=1+(n%7u),action=step+1;
  uint32_t epoch=oracle.epoch;
  int32_t debit=(int32_t)((n>>9)%17u);
  if(family==1&&step%9==0)epoch++;
  if(family==2&&step%5==0)debit=101;
  if(family==3&&step%7==0)actor=0;
  if(family==4&&step%11==0)epoch=0;
  NfB5Event e={actor,action,77,epoch,debit};
  Nf4Event w={actor,action,epoch,debit};
  bool valid=actor!=0&&epoch==oracle.epoch&&debit<=oracle.balance&&
    oracle.count<64;
  NfB5Result b=nf_b5_apply(inc,e,accept_save,NULL);
  Nf4Status f=nf4_apply(wal,w);
  if(valid){
   REQUIRE(b==NF_B5_APPLIED&&f==NF4_APPLIED);
   oracle.actor[oracle.count]=actor;oracle.action[oracle.count]=action;
   oracle.debit[oracle.count]=debit;oracle.count++;
   oracle.balance-=debit;oracle.epoch++;
   REQUIRE(nf_b5_apply(inc,e,accept_save,NULL)==NF_B5_DUPLICATE);
   REQUIRE(nf4_apply(wal,w)==NF4_DUPLICATE);
   if(step%5==0){
    REQUIRE(nf_b5_apply(inc,(NfB5Event){actor,action,77,epoch,debit+1},
      accept_save,NULL)==NF_B5_INVALID);
    REQUIRE(nf4_apply(wal,(Nf4Event){actor,action,epoch,debit+1})==NF4_INVALID);
   }
  }else{
   REQUIRE((b==NF_B5_INVALID||b==NF_B5_STALE||b==NF_B5_FULL)&&
           (f==NF4_INVALID||f==NF4_STALE||f==NF4_FULL));
  }
  REQUIRE(inc->balance==oracle.balance&&wal->ledger.balance==oracle.balance);
  REQUIRE(inc->epoch==oracle.epoch&&wal->ledger.epoch==oracle.epoch);
  REQUIRE(inc->count==oracle.count&&wal->ledger.count==oracle.count);
  (*checks)++;
 }
 /* A rejected B5 callback does not publish state.  The WAL's own
  * uncertainty semantics remain distinct; we do not equate them. */
 NfB5State snapshot=*inc;
 NfB5Event declined={9,9999,77,oracle.epoch,1};
 REQUIRE(nf_b5_apply(inc,declined,reject_save,NULL)==NF_B5_PERSIST_FAIL);
 REQUIRE(nf_b5_equal(inc,&snapshot));
 nf4_close(wal);
 REQUIRE(nf4_open(wal,path,100)==NF4_APPLIED);
 REQUIRE(wal->ledger.balance==oracle.balance&&
    wal->ledger.epoch==oracle.epoch&&wal->ledger.count==oracle.count);
 for(uint32_t i=0;i<oracle.count;++i)
  REQUIRE(wal->ledger.receipts[i].actor==oracle.actor[i]&&
    wal->ledger.receipts[i].action==oracle.action[i]&&
    wal->ledger.receipts[i].debit==oracle.debit[i]);
 nf4_close(wal);free(wal);free(inc);unlink(path);
 return 0;
}
int main(void){
 const char *names[]={"canonical","randomized","pathological","boundary","heldout"};
 clock_t begin=clock();
 unsigned checks=0;
 for(unsigned family=0;family<5;++family){
  for(unsigned seed=0;seed<64;++seed)if(trial(family,seed,&checks))return 1;
  printf("STRATUM %s cases=64 PASS\n",names[family]);
 }
 printf("C0_INCUMBENT_CHALLENGER scenarios=320 proposals=%u checks=%llu cpu_seconds=%.6f PASS\n",
    checks,assertions,(double)(clock()-begin)/CLOCKS_PER_SEC);
 return 0;
}
