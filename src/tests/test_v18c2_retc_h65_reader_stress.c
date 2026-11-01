/* SPDX-License-Identifier: GPL-3.0-or-later
 * H65: concurrent immutable joint-read snapshot coherence under a single
 * simulated publication-pointer handoff, NOT a real A6 writer.
 */
#include "nf_c2_retc_frame.h"
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ROUNDS 3000u
#define READERS 6u
#define REQUIRE(x) do {if(!(x)){fprintf(stderr,"H65 ERROR line %d: %s\n",__LINE__,#x);return 1;}}while(0)
typedef struct {
 NfRetcPublisher *pub;
 atomic_uint violations;
 atomic_uint reads;
 atomic_bool start;
} Stress;
static NfRetcFrame *make_frame(uint32_t epoch){
 NfRetcFrame *f=calloc(1,sizeof *f);
 if(!f)return NULL;
 f->epoch=epoch;f->prior_epoch=epoch-1u;
 f->transaction_id=epoch;f->owner_actor=10;f->object_id=2010;
 f->physical.world.tick=epoch;f->physical.actor.id=10;
 f->physical.object.id=2010;
 f->event.tick=epoch;f->event.actor=10;f->event.object=2010;
 f->event.action=91;f->event.kind=NF18B3_PUSH;
 f->event.phase=NF18B3_APPLIED;f->event.authorized=1;f->event.applied=1;
 f->event.contract_epoch=epoch+6u;
 f->b2_use.tick=epoch;f->b2_use.actor_id=10;
 f->b2_use.object_id=2010;f->b2_use.action_id=91;
 f->b2_actor.id=10;f->b2_object.id=2010;
 f->b2_decision.status=NF18B2_ELIGIBLE;
 f->b2_decision.physical=NF18B2_ELIGIBLE;
 f->b2_decision.contract=NF18B2_ELIGIBLE;
 f->has_b2_contract=true;f->has_encounter_registry=true;
 f->encounter_registry.contract_epoch=epoch+6u;
 return f;
}
static void *observe(void *arg){
 Stress *s=arg;
 while(!atomic_load(&s->start))sched_yield();
 for(unsigned i=0;i<ROUNDS;i++){
  NfRetcReadLease lease={0};
  if(!nf_retc_read_lease_acquire(s->pub,&lease)){
   atomic_fetch_add(&s->violations,1u);continue;
  }
  const NfRetcFrame *pinned=lease.accepted;
  NfRetcReadView view={0};
  if(!nf_retc_read_leased_view(&lease,&view)||
     !view.accepted||view.accepted!=pinned||
     !view.physical||!view.event||!view.contract||!view.eligibility||
     view.epoch!=pinned->epoch||view.transaction_id!=view.epoch||
     view.physical->world.tick!=view.epoch||
     view.event->tick!=view.epoch||
     view.eligibility->tick!=view.epoch||
     view.contract->contract_epoch!=view.epoch+6u){
    atomic_fetch_add(&s->violations,1u);
  }else atomic_fetch_add(&s->reads,1u);
  nf_retc_read_lease_release(&lease);
  if(lease.accepted||lease.publisher)
   atomic_fetch_add(&s->violations,1u);
  if(i%29u==0u)sched_yield();
 }
 return NULL;
}
int main(void){
 for(unsigned trial=0;trial<8u;trial++){
  NfRetcPublisher *p=calloc(1,sizeof *p);
  REQUIRE(p);
  NfRetcFrame *one=make_frame(1u),*two=make_frame(2u);
  REQUIRE(one&&two);
  p->owned[0]=one;p->owned[1]=two;p->count=2u;
  atomic_init(&p->current,one);atomic_init(&p->active_readers,0u);
  atomic_init(&p->read_closing,false);
  Stress s={.pub=p};
  atomic_init(&s.violations,0u);atomic_init(&s.reads,0u);
  atomic_init(&s.start,false);
  pthread_t t[READERS];
  for(unsigned i=0;i<READERS;i++)
   REQUIRE(pthread_create(&t[i],NULL,observe,&s)==0);
  atomic_store(&s.start,true);
  for(unsigned step=0;step<ROUNDS;step++){
   atomic_store_explicit(&p->current,(step%2u)?one:two,memory_order_release);
   if(step%11u==0u)sched_yield();
  }
  for(unsigned i=0;i<READERS;i++)REQUIRE(pthread_join(t[i],NULL)==0);
  REQUIRE(atomic_load(&s.violations)==0u);
  REQUIRE(atomic_load(&s.reads)==READERS*ROUNDS);
  REQUIRE(atomic_load(&p->active_readers)==0u);
  REQUIRE(nf_retc_try_dispose(p));
  REQUIRE(!nf_retc_read_lease_acquire(p,&(NfRetcReadLease){0}));
  free(p);
 }
 puts("H65 PASS: 8 trials x 6 readers x 3000 leased immutable joint views across simulated pointer swaps");
 return 0;
}
