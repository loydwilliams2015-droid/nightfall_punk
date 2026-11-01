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
 for(uint32_t trial=1;trial<=64u;++trial){
   NfRetcPublisher *p=calloc(1,sizeof *p);
   NfRetcFrame *frame=calloc(1,sizeof *frame);
   CK(p&&frame);
   frame->epoch=trial;
   atomic_init(&p->current,frame);
   atomic_init(&p->active_readers,0u);
   atomic_init(&p->read_closing,false);
   p->owned[0]=frame;p->count=1u;
   Hold h[4]={0};
   pthread_t t[4];
   for(size_t i=0;i<4u;++i){
     h[i].pub=p;
     atomic_init(&h[i].acquired,false);
     atomic_init(&h[i].release_reader,false);
     atomic_init(&h[i].observed_epoch,0u);
     CK(pthread_create(&t[i],NULL,reader,&h[i])==0);
   }
   for(size_t i=0;i<4u;++i){
     while(!atomic_load(&h[i].acquired))sched_yield();
     CK(atomic_load(&h[i].observed_epoch)!=UINT32_MAX);
   }
   CK(atomic_load(&p->active_readers)==4u);
   CK(!nf_retc_try_dispose(p)); /* Closing must reject destruction. */
   NfRetcReadLease denied={0};
   CK(!nf_retc_read_lease_acquire(p,&denied));
   CK(denied.accepted==NULL&&denied.publisher==NULL);
   for(size_t i=0;i<4u;++i){
     CK(atomic_load(&p->current)==frame);
     atomic_store(&h[i].release_reader,true);
     CK(pthread_join(t[i],NULL)==0);
     CK(atomic_load(&h[i].observed_epoch)==trial);
     if(i!=3u)CK(!nf_retc_try_dispose(p)); /* 3,2,1 live readers. */
   }
   CK(atomic_load(&p->active_readers)==0u);
   CK(nf_retc_try_dispose(p));
   CK(atomic_load(&p->current)==NULL&&p->count==0u);
   CK(!nf_retc_read_lease_acquire(p,&denied));
   free(p);
 }
 puts("H58 four-reader accepted-pointer tenure, 64 shutdown trials PASS");
 return 0;
}
