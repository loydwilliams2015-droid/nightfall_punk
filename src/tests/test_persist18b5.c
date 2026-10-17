#include "nf_persist18b5.h"
#include <assert.h>
#include <stdio.h>
static bool yes(void *p,const NfB5State *s){*(NfB5State *)p=*s;return true;}
static bool no(void *p,const NfB5State *s){(void)p;(void)s;return false;}
int main(void){unsigned checks=0;NfB5State s={.epoch=1,.balance=100},disk={0},before;
 NfB5Event e={.actor=1,.action=1,.object=2,.epoch=1,.debit=10};
#define C(x) do{assert(x);checks++;}while(0)
 C(nf_b5_apply(&s,e,yes,&disk)==NF_B5_APPLIED);
 C(nf_b5_equal(&s,&disk));C(s.balance==90);
 C(nf_b5_apply(&s,e,yes,&disk)==NF_B5_DUPLICATE);
 before=s;e.action=2;e.epoch=1;
 C(nf_b5_apply(&s,e,yes,&disk)==NF_B5_STALE);C(nf_b5_equal(&s,&before));
 e.epoch=s.epoch;
 C(nf_b5_apply(&s,e,no,&disk)==NF_B5_PERSIST_FAIL);
 C(nf_b5_equal(&s,&before));C(nf_b5_equal(&disk,&before));
 C(nf_b5_apply(&s,e,yes,&disk)==NF_B5_APPLIED);C(s.balance==80);
 for(unsigned i=3;i<=64;i++){e.debit=0;e.action=i;e.epoch=s.epoch;C(nf_b5_apply(&s,e,yes,&disk)==NF_B5_APPLIED);}
 C(s.count==64);e.action=65;e.epoch=s.epoch;before=s;
 C(nf_b5_apply(&s,e,yes,&disk)==NF_B5_FULL);C(nf_b5_equal(&s,&before));
 printf("B5 snapshot-first unit: %u checks PASS\n",checks);return 0;
}
