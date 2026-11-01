#include "nf_c2_retc_frame.h"
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#define CK(x) do {if(!(x)){fprintf(stderr,"H57 lifetime failure %s:%d: %s\n",__FILE__,__LINE__,#x);return 1;}}while(0)
typedef struct {
 NfRetcPublisher *pub;
 atomic_bool acquired, release_reader;
 atomic_uint observed_epoch;
} Hold;
static void *reader(void *opaque){
 Hold *h=opaque;
 NfRetcReadLease lease={0};
 if(!nf_retc_read_lease_acquire(h->pub,&lease)){
   atomic_store(&h->observed_epoch,UINT32_MAX);
   atomic_store(&h->acquired,true);
   return NULL;
 }
 atomic_store(&h->acquired,true);
 while(!atomic_load(&h->release_reader))sched_yield();
 atomic_store(&h->observed_epoch,lease.accepted->epoch);
 nf_retc_read_lease_release(&lease);
 return NULL;
}
int main(void){
 for(uint32_t trial=1;trial<=32u;++trial){
   NfRetcPublisher *p=calloc(1,sizeof *p);
   NfRetcFrame *frame=calloc(1,sizeof *frame);
   CK(p&&frame);
   /* Source-independent deterministic tenure oracle, not forged game truth:
      tests ownership of allocated accepted-pointer memory only. */
   frame->epoch=trial;
   atomic_init(&p->current,frame);
   atomic_init(&p->active_readers,0u);
   atomic_init(&p->read_closing,false);
   p->owned[0]=frame;p->count=1u;
   Hold h={0};h.pub=p;
   atomic_init(&h.acquired,false);
   atomic_init(&h.release_reader,false);
   atomic_init(&h.observed_epoch,0u);
   pthread_t thread;
   CK(pthread_create(&thread,NULL,reader,&h)==0);
   while(!atomic_load(&h.acquired))sched_yield();
   CK(atomic_load(&h.observed_epoch)!=UINT32_MAX);
   CK(!nf_retc_try_dispose(p)); /* Deny reclamation while frame is borrowed. */
   NfRetcReadLease denied={0};
   CK(!nf_retc_read_lease_acquire(p,&denied)); /* No new reader after close. */
   CK(denied.accepted==NULL&&denied.publisher==NULL);
   CK(atomic_load(&p->current)==frame);
   atomic_store(&h.release_reader,true);
   CK(pthread_join(thread,NULL)==0);
   CK(atomic_load(&h.observed_epoch)==trial);
   CK(nf_retc_try_dispose(p)); /* All readers drained: frame may retire. */
   CK(atomic_load(&p->current)==NULL);
   CK(p->count==0u);
   CK(!nf_retc_read_lease_acquire(p,&denied));
   free(p);
 }
 puts("H57 scoped read lease / retire-after-drain / reject-after-close 32 trials PASS");
 return 0;
}
