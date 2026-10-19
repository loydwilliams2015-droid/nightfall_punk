#include "nf_persist18b5.h"
#include <string.h>
bool nf_b5_equal(const NfB5State *a,const NfB5State *b){
 return a&&b&&a->epoch==b->epoch&&a->count==b->count&&a->balance==b->balance&&memcmp(a->receipts,b->receipts,sizeof a->receipts)==0;
}
NfB5Result nf_b5_apply(NfB5State *s,NfB5Event e,NfB5Persist save,void *ctx){
 if(!s||!save||!e.actor||!e.action||!e.object||e.debit<0||s->count>NF_B5_RECEIPTS)return NF_B5_INVALID;
 for(uint32_t i=0;i<s->count;i++)if(s->receipts[i].actor==e.actor&&s->receipts[i].action==e.action){
  const NfB5Event *prior=&s->receipts[i];
  return prior->object==e.object&&prior->epoch==e.epoch&&prior->debit==e.debit
    ? NF_B5_DUPLICATE : NF_B5_INVALID;
 }
 if(e.epoch!=s->epoch)return NF_B5_STALE;
 if(s->count==NF_B5_RECEIPTS)return NF_B5_FULL;
 if(s->balance<(int64_t)e.debit||s->epoch==UINT32_MAX)return NF_B5_INVALID;
 NfB5State next=*s;next.balance-=e.debit;next.receipts[next.count++]=e;next.epoch++;
 if(!save(ctx,&next))return NF_B5_PERSIST_FAIL;
 *s=next;return NF_B5_APPLIED;
}
