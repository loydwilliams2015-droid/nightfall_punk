#define _POSIX_C_SOURCE 200809L
#include "nf_c2_retc_frame.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sched.h>
static uint32_t digest(uint32_t a,uint32_t b,uint32_t c){
 uint32_t v=2166136261u;
 v=(v^a)*16777619u;v=(v^b)*16777619u;return (v^c)*16777619u;
}
NfRetcFrame *nf_retc_stage(const Nf18a5Integrated *p,
 const Nf18b3Event *event,uint32_t tx,uint32_t prior_epoch,
 const NfRetcObligation *obs,uint32_t n){
 if(!p||!event||!tx||prior_epoch==UINT32_MAX||n>NF_RETC_OBLIGATIONS|| (n&&!obs)||
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
    f->b2_use.world_revision==UINT32_MAX||
    f->b2_use.world_revision+1u!=f->physical.world.revision||
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
/* H64: a B2-only or materially/contractually split frame is never an
 * externally authoritative frame, even if its individual PUSH seal passes.
 * This bounded check is not final whole-world transaction authority. */
static bool retc_joint_complete(const NfRetcFrame *f){
 return f&&f->epoch&&f->prior_epoch!=UINT32_MAX&&
   f->epoch==f->prior_epoch+1u&&f->transaction_id&&
   f->has_b2_contract&&f->has_encounter_registry&&
   f->encounter_registry.contract_epoch!=0u&&
   f->encounter_registry.contract_epoch==f->event.contract_epoch&&
   f->physical.world.tick==f->epoch&&
   f->event.tick==f->epoch&&
   f->owner_actor==f->physical.actor.id&&
   f->object_id==f->physical.object.id&&
   f->event.actor==f->owner_actor&&f->event.object==f->object_id&&
   f->b2_use.tick==f->epoch&&
   f->b2_use.actor_id==f->owner_actor&&
   f->b2_use.object_id==f->object_id&&
   f->b2_use.action_id==f->event.action&&
   f->b2_actor.id==f->owner_actor&&
   f->b2_object.id==f->object_id&&
   nf_retc_seal_b2_b3((NfRetcFrame *)f);
}
/* H57: explicit read tenure. Closing rejects entrants, drains already
 * acquired readers before reclaim. Caller must own the publisher-object
 * lifetime itself and quiesce the A6 writer before reclaiming. */
bool nf_retc_read_lease_acquire(const NfRetcPublisher *pub,
                               NfRetcReadLease *out){
 if(!out)return false;
 *out=(NfRetcReadLease){0};
 if(!pub||atomic_load_explicit(&pub->read_closing,memory_order_seq_cst))return false;
 atomic_uint *readers=(atomic_uint *)&pub->active_readers;
 atomic_fetch_add_explicit(readers,1u,memory_order_seq_cst);
 if(atomic_load_explicit(&pub->read_closing,memory_order_seq_cst)){
    atomic_fetch_sub_explicit(readers,1u,memory_order_seq_cst);return false;
 }
 const NfRetcFrame *f=atomic_load_explicit(&pub->current,memory_order_seq_cst);
 if(!f){atomic_fetch_sub_explicit(readers,1u,memory_order_seq_cst);return false;}
 out->publisher=pub;out->accepted=f;
 return true;
}
void nf_retc_read_lease_release(NfRetcReadLease *lease){
 if(!lease||!lease->publisher)return;
 atomic_uint *readers=(atomic_uint *)&lease->publisher->active_readers;
 lease->accepted=NULL;lease->publisher=NULL;
 atomic_fetch_sub_explicit(readers,1u,memory_order_seq_cst);
}
bool nf_retc_try_dispose(NfRetcPublisher *pub){
 if(!pub)return false;
 atomic_store_explicit(&pub->read_closing,true,memory_order_seq_cst);
 if(atomic_load_explicit(&pub->active_readers,memory_order_seq_cst)!=0u)return false;
 atomic_store_explicit(&pub->current,NULL,memory_order_seq_cst);
 for(uint32_t i=0;i<pub->count;++i)free(pub->owned[i]);
 for(uint32_t i=0;i<pub->count;++i)pub->owned[i]=NULL;
 pub->count=0;
 return true;
}
bool nf_retc_publish(NfRetcPublisher *pub,NfRetcFrame *f){
 /* Source-backed candidate authority is mandatory: generic obligation tuples
    cannot stand in for typed B2 rights or material consequences. */
 if(!pub||!f||(pub&&atomic_load_explicit(&pub->read_closing,memory_order_acquire))||
    pub->count>=NF_RETC_HISTORY||
    !retc_joint_complete(f))return false;
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
/* H42: one acquire-load pins a bounded, immutable joint epoch.  The live
   world/encounter are deliberately not read here; their separate projections
   cannot be made atomic by repeatedly sampling public world fields. */
static bool nf_retc_validated_view(const NfRetcFrame *f,NfRetcReadView *out){
 if(!out)return false;
 *out=(NfRetcReadView){0};
 if(!f||!f->epoch||!f->transaction_id||
    !f->has_b2_contract||!f->has_encounter_registry||
    f->physical.world.tick!=f->epoch||f->event.tick!=f->epoch||
    f->b2_use.tick!=f->epoch||
    f->event.actor!=f->owner_actor||f->event.object!=f->object_id||
    f->physical.actor.id!=f->owner_actor||
    f->physical.object.id!=f->object_id||
    f->b2_use.actor_id!=f->owner_actor||
    f->b2_use.object_id!=f->object_id||
    f->b2_use.action_id!=f->event.action||
    f->b2_actor.id!=f->owner_actor||f->b2_object.id!=f->object_id||
    !f->event.authorized||!f->event.applied||
    f->event.phase!=NF18B3_APPLIED||f->event.kind!=NF18B3_PUSH||
    f->b2_decision.status!=NF18B2_ELIGIBLE||
    f->b2_decision.physical!=NF18B2_ELIGIBLE||
    f->b2_decision.contract!=NF18B2_ELIGIBLE||
    !f->event.contract_epoch||
    f->encounter_registry.contract_epoch!=f->event.contract_epoch||
    f->encounter_registry.account_count>NF18B2_MAX_REQUESTS||
    f->encounter_registry.object_count>NF18B2_MAX_REQUESTS||
    f->obligation_count>NF_RETC_OBLIGATIONS)return false;
 out->accepted=f;
 out->physical=&f->physical;
 out->event=&f->event;
 out->contract=&f->encounter_registry;
 out->eligibility=&f->b2_use;
 out->epoch=f->epoch;
 out->transaction_id=f->transaction_id;
 return true;
}
bool nf_retc_read_joint_view(const NfRetcPublisher *pub,NfRetcReadView *out){
 /* Backwards-compatible owner-only access; no external lifetime guarantee. */
 return nf_retc_validated_view(nf_retc_read(pub),out);
}
bool nf_retc_read_leased_view(const NfRetcReadLease *lease,NfRetcReadView *out){
 return nf_retc_validated_view(lease?lease->accepted:NULL,out);
}
void nf_retc_dispose(NfRetcPublisher *pub){
 if(!pub)return;
 /* Caller must already have quiesced the A6 owner/writer. */
 while(!nf_retc_try_dispose(pub))sched_yield();
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
  retc_joint_complete(temp)&&
  temp->owner_actor==temp->physical.actor.id&&
  temp->object_id==temp->physical.object.id;
 if(ok)*out=*temp;
 free(temp);return ok;
}
/* H286: no A6 tick, no publisher mutation, no filesystem write.
 * The caller supplies a separately trusted checkpoint. These simple
 * identities are a bounded scientific prerequisite, not cryptographic
 * provenance or a production C3 replay authority. */
NfRetcRecoveryStatus nf_retc_recovery_inspect_file(
 const char *path,uint32_t trusted_owner,uint32_t trusted_object,
 uint32_t trusted_epoch,uint32_t trusted_transaction_id,
 NfRetcFrame *out){
 if(!path||!out||!trusted_owner||!trusted_object)return NF_RETC_RECOVERY_REJECT;
 NfRetcFrame *candidate=malloc(sizeof *candidate);
 if(!candidate)return NF_RETC_RECOVERY_REJECT;
 NfRetcRecoveryStatus status=NF_RETC_RECOVERY_REJECT;
 if(nf_retc_restore_file(path,candidate)&&
    candidate->owner_actor==trusted_owner&&
    candidate->object_id==trusted_object&&
    candidate->event.kind==NF18B3_PUSH&&
    candidate->event.phase==NF18B3_APPLIED&&
    candidate->event.authorized&&candidate->event.applied&&
    candidate->b2_decision.status==NF18B2_ELIGIBLE&&
    candidate->b2_decision.physical==NF18B2_ELIGIBLE&&
    candidate->b2_decision.contract==NF18B2_ELIGIBLE&&
    candidate->event.contract_epoch==candidate->encounter_registry.contract_epoch){
   if(candidate->epoch==trusted_epoch&&
      candidate->transaction_id==trusted_transaction_id)
      status=NF_RETC_RECOVERY_PRIOR;
   else if(trusted_epoch<UINT32_MAX&&
           candidate->epoch==trusted_epoch+1u&&
           candidate->prior_epoch==trusted_epoch&&
           candidate->transaction_id>trusted_transaction_id)
      status=NF_RETC_RECOVERY_NEXT;
 }
 if(status!=NF_RETC_RECOVERY_REJECT)*out=*candidate;
 free(candidate);
 return status;
}
/* H284 lab-only deterministic kill boundaries. Production builds do not
 * compile this hook. These exits model process termination, not power loss,
 * and do not imply directory durability or whole-owner restart recovery. */
#ifdef NF_CONTACT_LAB_ONLY
static void h284_kill_boundary(unsigned point){
 const char *flag=getenv("NF_RETC_H284_KILL_POINT");
 if(flag&&flag[0]==(char)('0'+point)&&flag[1]=='\0')_exit(110+(int)point);
}
#else
static void h284_kill_boundary(unsigned point){(void)point;}
#endif
/* H287: POSIX/Linux directory-entry durability. An fsync of the staged
 * regular file does not make the rename's parent directory durable. After
 * rename, fsync the parent directory BEFORE returning persistence success.
 * If dir sync fails, the new journal may already exist: report failure;
 * do not pretend rollback or delete an already visible transaction.
 * Path is bounded by the persistence entrypoint and parent is opened only.
 * This is not yet a portable versioned WAL or total power-loss proof. */
static bool retc_sync_parent_directory(const char *path){
 if(!path||!*path)return false;
#ifdef NF_CONTACT_LAB_ONLY
 /* H288: emulate a directory-fsync error only in the isolated C research
  * lab, AFTER rename. Production builds do not read this environment knob.
  * The renamed file may remain visible: never advertise commit success. */
 const char *fail=getenv("NF_RETC_H288_FAIL_DIRSYNC");
 if(fail&&strcmp(fail,"1")==0){errno=EIO;return false;}
#endif
 const char *slash=strrchr(path,'/');
 char directory[1024];
 if(!slash)strcpy(directory,".");
 else if(slash==path)strcpy(directory,"/");
 else {
   size_t len=(size_t)(slash-path);
   if(len>=sizeof directory)return false;
   memcpy(directory,path,len);
   directory[len]='\0';
 }
 int fd=open(directory,O_RDONLY
#ifdef O_DIRECTORY
    |O_DIRECTORY
#endif
 );
 if(fd<0)return false;
 bool ok=fsync(fd)==0;
 if(close(fd)!=0)ok=false;
 return ok;
}
bool nf_retc_persist_file(NfRetcPublisher *pub,NfRetcFrame *f,
 const char *path){
 if(!pub||!f||!path||atomic_load_explicit(&pub->read_closing,memory_order_acquire)||
    !retc_joint_complete(f)||pub->count>=NF_RETC_HISTORY)return false;
 const NfRetcFrame *prior=nf_retc_read(pub);
 if(f->prior_epoch!=(prior?prior->epoch:0)||
    f->epoch!=f->prior_epoch+1u||
    (prior&&f->transaction_id<=prior->transaction_id))return false;
 /* Full frame includes source-bound B2, B3 and A5; write before pointer. */
 char stage[1024];if(strlen(path)>990u||snprintf(stage,sizeof stage,"%s.staging",path)>=(int)sizeof stage)return false;
 FILE *fp=fopen(stage,"wb");if(!fp)return false;
 h284_kill_boundary(1u);
 const RetcDiskHeader h={0x52455443u,1u,(uint32_t)sizeof *f,
   frame_checksum((const unsigned char *)f,sizeof *f)};
 bool ok=fwrite(&h,sizeof h,1,fp)==1&&fwrite(f,sizeof *f,1,fp)==1;
 if(ok)h284_kill_boundary(2u);
 if(ok)ok=fflush(fp)==0;
 if(ok)h284_kill_boundary(3u);
 if(ok)ok=fsync(fileno(fp))==0;
 if(ok)h284_kill_boundary(4u);
 if(fclose(fp)!=0)ok=false;
 if(ok)ok=rename(stage,path)==0;
 if(ok)h284_kill_boundary(5u); /* after rename; directory not yet synced */
 if(ok)ok=retc_sync_parent_directory(path);
 if(ok)h284_kill_boundary(6u); /* directory metadata now synced */
 if(!ok){remove(stage);return false;}
 /* POSIX local-filesystem directory sync is now part of the success gate.
    Whole-owner restart, portable serialization, power-failure semantics and
    atomic B2/B3 world rehydration remain outstanding. */
 return true;
}
bool nf_retc_commit_file(NfRetcPublisher *pub,NfRetcFrame *f,
 const char *path){
 return nf_retc_persist_file(pub,f,path)&&nf_retc_publish(pub,f);
}
