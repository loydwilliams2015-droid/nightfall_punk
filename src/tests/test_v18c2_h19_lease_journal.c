#include "nf_c2_h19_lab.h"
#include <threads.h>
#include <stdio.h>
#include <string.h>
typedef struct {NfC2Lease *lease;atomic_uint *attempts;} Worker;
static int worker(void *arg){Worker *w=arg;for(int i=0;i<10000;i++){
 if(nf_c2_lease_acquire(w->lease)){atomic_fetch_add(w->attempts,1);nf_c2_lease_release(w->lease);}
}return 0;}
int main(void){
 NfC2Lease lease;nf_c2_lease_init(&lease);atomic_uint attempts=0;
 Worker w={&lease,&attempts};thrd_t threads[4];
 for(int i=0;i<4;i++)if(thrd_create(&threads[i],worker,&w)!=thrd_success)return 1;
 nf_c2_lease_close(&lease);
 for(int i=0;i<4;i++){int status=-1;if(thrd_join(threads[i],&status)!=thrd_success||status)return 2;}
 if(!nf_c2_lease_drained(&lease)||nf_c2_lease_acquire(&lease))return 3;
 for(uint32_t i=1;i<=2048;i++){
  NfC2Journal j={0};NfC2Frame source={.tick=i,.owner=10,.action=i,
    .a4_receipt=i,.a5_digest=i+100,.b3_receipt=i};
  NfC2Frame recovered={0};
  if(!nf_c2_propose(&j,source))return 4;
  if(nf_c2_recover(&j,&recovered))return 5; /* crash before commit */
  if(!nf_c2_commit(&j)||!nf_c2_recover(&j,&recovered))return 6;
  if(recovered.tick!=i||recovered.a4_receipt!=recovered.b3_receipt)return 7;
  j.durable.a5_digest^=1; /* torn/corrupted joint frame */
  if(nf_c2_recover(&j,&recovered))return 8;
 }
 puts("H19 lease close and 2048 logical joint-frame recovery scenarios PASS");
 return 0;
}
