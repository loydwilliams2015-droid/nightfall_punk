#include "nf_c2_retc_frame.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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
bool nf_retc_seal_b2_b3(NfRetcFrame *f){
 if(!f||!f->has_b2_contract||f->event.phase!=NF18B3_APPLIED||
    !f->event.applied||!f->event.authorized||
    f->b2_use.tick!=f->epoch||
    f->b2_use.world_revision!=f->physical.world.revision||
    f->b2_use.material_epoch!=f->physical.world.grid.global_revision||
    f->b2_decision.status!=NF18B2_ELIGIBLE||
    f->b2_decision.physical!=NF18B2_ELIGIBLE||
    f->b2_decision.contract!=NF18B2_ELIGIBLE||
    f->b2_authority.publishing||
    f->b2_authority.receipt_count>=64u)return false;
 /* This experimental frame represents the admitted physical PUSH only.
    B3 pickup/carry/repair/interrupt do not yet have source-bound debit
    realizers; reject them instead of inventing contract side effects. */
 if(f->event.kind!=NF18B3_PUSH)return false;
 for(uint32_t i=0;i<f->b2_authority.receipt_count;++i)
    if(f->b2_authority.ticks[i]==f->epoch &&
       f->b2_authority.actors[i]==f->owner_actor &&
       f->b2_authority.features[i]==f->object_id)return false;
 return true;
}
bool nf_retc_bind_encounter(NfRetcFrame *f,
 const Nf18b2EncounterRuntime *encounter){
 if(!f||!encounter||f->has_encounter_registry||
    !encounter->prepared||encounter->owner==NULL||
    encounter->owner->actor_id!=f->owner_actor||
    encounter->owner->physical.world.tick!=f->prior_epoch||
    encounter->staged.account_count>NF18B2_MAX_REQUESTS||
    encounter->staged.object_count>NF18B2_MAX_REQUESTS||
    !encounter->staged.contract_epoch)return false;
 /* Exact prevalidated registry, not synthetic resource deltas. This must be
    called while the A6 owner holds its transaction lifetime boundary. */
 f->encounter_registry=encounter->staged;
 f->has_encounter_registry=true;
 f->derived_contract_hash=digest(f->derived_contract_hash,
     f->encounter_registry.revision,f->encounter_registry.contract_epoch);
 return true;
}
NfRetcFrame *nf_retc_stage_source_push(
 const Nf18a5Integrated *physical,const Nf18b3Event *event,
 uint32_t tx,uint32_t prior_epoch,const Nf18b2Use *use,
 const Nf18b2UseDecision *decision,const Nf18b2Actor *actor,
 const Nf18b2Object *object,const Nf18b2Authority *authority){
 if(!event||event->kind!=NF18B3_PUSH)return NULL;
 NfRetcFrame *f=nf_retc_stage(physical,event,tx,prior_epoch,NULL,0);
 if(!f)return NULL;
 if(!nf_retc_bind_b2(f,use,decision,actor,object,authority)||
    !nf_retc_seal_b2_b3(f)){free(f);return NULL;}
 return f;
}
bool nf_retc_publish(NfRetcPublisher *pub,NfRetcFrame *f){
 /* Source-backed candidate authority is mandatory: generic obligation tuples
    cannot stand in for typed B2 rights or material consequences. */
 if(!pub||!f||pub->count>=NF_RETC_HISTORY||
    !f->has_b2_contract||!nf_retc_seal_b2_b3(f))return false;
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

/* Bounded local research journal. No directory fsync / portable encoding:
   explicitly NOT the final production durability protocol. */
typedef struct {uint32_t magic,version,length,checksum;} RetcDiskHeader;
static uint32_t frame_checksum(const unsigned char *p,size_t n){
 uint32_t h=2166136261u;for(size_t i=0;i<n;++i)h=(h^p[i])*16777619u;return h;
}
bool nf_retc_restore_file(const char *path,NfRetcFrame *out){
 if(!path||!out)return false;
 FILE *fp=fopen(path,"rb");if(!fp)return false;
 RetcDiskHeader h={0};NfRetcFrame *temp=malloc(sizeof *temp);
 bool ok=temp&&fread(&h,sizeof h,1,fp)==1&&
  h.magic==0x52455443u&&h.version==1&&h.length==sizeof *temp&&
  fread(temp,sizeof *temp,1,fp)==1&&fgetc(fp)==EOF&&!ferror(fp);
 if(fclose(fp)!=0)ok=false;
 if(ok)ok=frame_checksum((const unsigned char *)temp,sizeof *temp)==h.checksum&&
  temp->has_b2_contract&&nf_retc_seal_b2_b3(temp)&&
  temp->owner_actor==temp->physical.actor.id&&
  temp->object_id==temp->physical.object.id;
 if(ok)*out=*temp;
 free(temp);return ok;
}
bool nf_retc_commit_file(NfRetcPublisher *pub,NfRetcFrame *f,
 const char *path){
 if(!pub||!f||!path||!f->has_b2_contract||
    !nf_retc_seal_b2_b3(f)||pub->count>=NF_RETC_HISTORY)return false;
 const NfRetcFrame *prior=nf_retc_read(pub);
 if(f->prior_epoch!=(prior?prior->epoch:0)||
    f->epoch!=f->prior_epoch+1u||
    (prior&&f->transaction_id<=prior->transaction_id))return false;
 /* Full frame includes source-bound B2, B3 and A5; write before pointer. */
 FILE *fp=fopen(path,"wb");if(!fp)return false;
 const RetcDiskHeader h={0x52455443u,1u,(uint32_t)sizeof *f,
   frame_checksum((const unsigned char *)f,sizeof *f)};
 bool ok=fwrite(&h,sizeof h,1,fp)==1&&fwrite(f,sizeof *f,1,fp)==1;
 if(ok)ok=fflush(fp)==0;
 if(fclose(fp)!=0)ok=false;
 if(!ok)return false;
 return nf_retc_publish(pub,f);
}
