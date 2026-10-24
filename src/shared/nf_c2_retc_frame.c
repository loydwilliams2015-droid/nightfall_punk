#include "nf_c2_retc_frame.h"
#include <stdlib.h>
#include <string.h>
static uint32_t digest(uint32_t a,uint32_t b,uint32_t c){
 uint32_t v=2166136261u;
 v=(v^a)*16777619u;v=(v^b)*16777619u;return (v^c)*16777619u;
}
NfRetcFrame *nf_retc_stage(const Nf18a5Integrated *p,
 const Nf18b3Event *event,uint32_t tx,uint32_t prior_epoch,
 const NfRetcObligation *obs,uint32_t n){
 if(!p||!event||!tx||n>NF_RETC_OBLIGATIONS|| (n&&!obs)||
    p->world.tick!=prior_epoch+1u||event->tick!=p->world.tick||
    event->actor!=p->actor.id||event->object!=p->object.id||
    !event->authorized||!event->applied||event->phase!=NF18B3_APPLIED||
    !event->contract_epoch||!p->committed_receipts)return NULL;
 for(uint32_t i=0;i<n;++i)
    if(!obs[i].obligation_id||!obs[i].holder||!obs[i].beneficiary)return NULL;
 NfRetcFrame *f=calloc(1,sizeof *f);if(!f)return NULL;
 f->physical=*p;f->event=*event;f->epoch=p->world.tick;
 f->prior_epoch=prior_epoch;f->transaction_id=tx;
 f->owner_actor=p->actor.id;f->object_id=p->object.id;
 f->obligation_count=n;
 if(n)memcpy(f->obligations,obs,n*sizeof *obs);
 f->derived_actor_hash=digest(f->owner_actor,f->epoch,p->committed_receipts);
 f->derived_contract_hash=digest(event->action,event->contract_epoch,n);
 return f;
}
bool nf_retc_bind_b2(NfRetcFrame *f,
 const Nf18b2Use *u,const Nf18b2UseDecision *d,
 const Nf18b2Actor *a,const Nf18b2Object *o,
 const Nf18b2Authority *auth){
 if(!f||!u||!d||!a||!o||!auth||f->has_b2_contract||
    d->status!=NF18B2_ELIGIBLE||d->physical!=NF18B2_ELIGIBLE||
    d->contract!=NF18B2_ELIGIBLE||u->actor_id!=f->owner_actor||
    u->object_id!=f->object_id||u->action_id!=f->event.action||
    u->tick!=f->epoch||u->actor_revision!=a->revision||
    u->object_revision!=o->revision||a->id!=u->actor_id||
    o->id!=u->object_id||auth->publishing||
    auth->receipt_count>64u)return false;
 /* A B2 witness and bounded replay authority are a minimum: do not infer
    full world ownership or future contract consequences from eligibility. */
 f->b2_use=*u;f->b2_decision=*d;f->b2_actor=*a;
 f->b2_object=*o;f->b2_authority=*auth;f->has_b2_contract=true;
 f->derived_contract_hash=digest(f->event.action,f->event.contract_epoch,
     f->obligation_count^u->object_revision^a->credentials^a->inventory);
 return true;
}
bool nf_retc_publish(NfRetcPublisher *pub,NfRetcFrame *f){
 if(!pub||!f||pub->count>=NF_RETC_HISTORY)return false;
 const NfRetcFrame *prior=atomic_load_explicit(&pub->current,memory_order_acquire);
 if(f->prior_epoch!=(prior?prior->epoch:0)||f->epoch!=f->prior_epoch+1u)return false;
 if(prior&&f->transaction_id<=prior->transaction_id)return false;
 pub->owned[pub->count++]=f;
 atomic_store_explicit(&pub->current,f,memory_order_release);
 return true;
}
const NfRetcFrame *nf_retc_read(const NfRetcPublisher *pub){
 return pub?atomic_load_explicit(&pub->current,memory_order_acquire):NULL;
}
void nf_retc_dispose(NfRetcPublisher *pub){
 if(!pub)return;
 atomic_store_explicit(&pub->current,NULL,memory_order_release);
 for(uint32_t i=0;i<pub->count;++i)free(pub->owned[i]);
 pub->count=0;
}
