#include "nf_retc_joint_provider.h"
#include <stdlib.h>
#include <stdio.h>
NfRetcFrame *nf_retc_joint_stage(const NfWorld *w,
 const Nf18a5Integrated *p,void *context){
 NfRetcJointContext *j=context;
 if(!j){fprintf(stderr,"RET-C joint_stage predicate 1: !j\n");return NULL;}
 if(!w){fprintf(stderr,"RET-C joint_stage predicate 2: !w\n");return NULL;}
 if(!p){fprintf(stderr,"RET-C joint_stage predicate 3: !p\n");return NULL;}
 if(!j->owner){fprintf(stderr,"RET-C joint_stage predicate 4: !j->owner\n");return NULL;}
 if(!j->encounter){fprintf(stderr,"RET-C joint_stage predicate 5: !j->encounter\n");return NULL;}
 if(!j->push){fprintf(stderr,"RET-C joint_stage predicate 6: !j->push\n");return NULL;}
 if(!j->transaction_id){fprintf(stderr,"RET-C joint_stage predicate 7: !j->transaction_id\n");return NULL;}
 if(!w->tick_in_progress){fprintf(stderr,"RET-C joint_stage predicate 8: !w->tick_in_progress\n");return NULL;}
 if(w->tick_owner_context!=j->owner){fprintf(stderr,"RET-C joint_stage predicate 9: w->tick_owner_context!=j->owner\n");return NULL;}
 if(j->encounter->world!=w){fprintf(stderr,"RET-C joint_stage predicate 10: j->encounter->world!=w\n");return NULL;}
 if(j->encounter->owner!=j->owner){fprintf(stderr,"RET-C joint_stage predicate 11: j->encounter->owner!=j->owner\n");return NULL;}
 if(j->push->runtime!=j->owner){fprintf(stderr,"RET-C joint_stage predicate 12: j->push->runtime!=j->owner\n");return NULL;}
 if(j->owner->retc_publisher==NULL){fprintf(stderr,"RET-C joint_stage predicate 13: j->owner->retc_publisher==NULL\n");return NULL;}
 if(!j->encounter->prepared){fprintf(stderr,"RET-C joint_stage predicate 14: !j->encounter->prepared\n");return NULL;}
 if(!j->push->armed){fprintf(stderr,"RET-C joint_stage predicate 15: !j->push->armed\n");return NULL;}
 if(!j->push->staged_commit_valid){fprintf(stderr,"RET-C joint_stage predicate 16: !j->push->staged_commit_valid\n");return NULL;}
 if(j->push->staged_commit.kind!=NF18B3_PUSH){fprintf(stderr,"RET-C joint_stage predicate 17: j->push->staged_commit.kind!=NF18B3_PUSH\n");return NULL;}
 if(j->push->staged_commit.phase!=NF18B3_APPLIED){fprintf(stderr,"RET-C joint_stage predicate 18: j->push->staged_commit.phase!=NF18B3_APPLIED\n");return NULL;}
 if(!j->push->staged_commit.applied){fprintf(stderr,"RET-C joint_stage predicate 19: !j->push->staged_commit.applied\n");return NULL;}
 if(j->push->staged_commit.tick!=p->world.tick){fprintf(stderr,"RET-C joint_stage predicate 20: j->push->staged_commit.tick!=p->world.tick\n");return NULL;}
 if(j->encounter->staged.contract_epoch!=j->push->contract_epoch){fprintf(stderr,"RET-C joint_stage predicate 21: j->encounter->staged.contract_epoch!=j->push->contract_epoch\n");return NULL;}
 if(p->world.tick!=w->tick+1u){fprintf(stderr,"RET-C joint_stage predicate 22: p->world.tick!=w->tick+1u\n");return NULL;}
 if(j->owner->physical.world.tick!=w->tick){fprintf(stderr,"RET-C joint_stage predicate 23: j->owner->physical.world.tick!=w->tick\n");return NULL;}
 const Nf18b3Event *e=&j->push->staged_commit;
 NfRetcFrame *f=nf_retc_stage_source_push(p,e,j->transaction_id,
      (uint32_t)w->tick,&j->use,&j->decision,
      &j->actor,&j->object,&j->authority);
 if(!f){fprintf(stderr,"RET-C joint_stage construction FAILED: nf_retc_stage_source_push\n");return NULL;}
 if(!nf_retc_bind_encounter(f,j->encounter)){
    fprintf(stderr,"RET-C joint_stage bind FAILED: nf_retc_bind_encounter\n");free(f);return NULL;
 }
 return f;
}

NfRetcFrame *nf_retc_joint_stage_owned(const NfWorld *w,
 const Nf18a5Integrated *p,void *context){
 NfRetcJointContext *j=context;
 if(!j){fprintf(stderr,"RET-C joint_stage_owned predicate 1: !j\n");return NULL;}
 if(!w){fprintf(stderr,"RET-C joint_stage_owned predicate 2: !w\n");return NULL;}
 if(!p){fprintf(stderr,"RET-C joint_stage_owned predicate 3: !p\n");return NULL;}
 if(!j->encounter){fprintf(stderr,"RET-C joint_stage_owned predicate 4: !j->encounter\n");return NULL;}
 if(!j->push){fprintf(stderr,"RET-C joint_stage_owned predicate 5: !j->push\n");return NULL;}
 if(!j->owner){fprintf(stderr,"RET-C joint_stage_owned predicate 6: !j->owner\n");return NULL;}
 if(!j->encounter->prepared){fprintf(stderr,"RET-C joint_stage_owned predicate 7: !j->encounter->prepared\n");return NULL;}
 if(!j->push->staged_commit_valid){fprintf(stderr,"RET-C joint_stage_owned predicate 8: !j->push->staged_commit_valid\n");return NULL;}
 if(j->encounter->queue_count!=1u){fprintf(stderr,"RET-C joint_stage_owned predicate 9: j->encounter->queue_count!=1u\n");return NULL;}
 if(!j->encounter->granted[0]){fprintf(stderr,"RET-C joint_stage_owned predicate 10: !j->encounter->granted[0]\n");return NULL;}
 if(j->encounter->frame.scene.actor_count!=1u){fprintf(stderr,"RET-C joint_stage_owned predicate 11: j->encounter->frame.scene.actor_count!=1u\n");return NULL;}
 if(j->encounter->frame.scene.object_count!=1u){fprintf(stderr,"RET-C joint_stage_owned predicate 12: j->encounter->frame.scene.object_count!=1u\n");return NULL;}
 if(!j->encounter->frame.scene.actors){fprintf(stderr,"RET-C joint_stage_owned predicate 13: !j->encounter->frame.scene.actors\n");return NULL;}
 if(!j->encounter->frame.scene.objects){fprintf(stderr,"RET-C joint_stage_owned predicate 14: !j->encounter->frame.scene.objects\n");return NULL;}
 const Nf18b2QueuedIntent *q=&j->encounter->queue[0];
 if(q->withdrawn){fprintf(stderr,"RET-C owned predicate 15: q->withdrawn\n");return NULL;}
 if(q->intent.kind!=NF18B2_PUSH){fprintf(stderr,"RET-C owned predicate 16: q->intent.kind!=NF18B2_PUSH\n");return NULL;}
 if(q->trace_index>=j->encounter->trace_count){fprintf(stderr,"RET-C owned predicate 17: q->trace_index>=j->encounter->trace_count\n");return NULL;}
 const Nf18b2EncounterTrace *t=&j->encounter->traces[q->trace_index];
 const Nf18b3Event *e=&j->push->staged_commit;
 if(t->revalidation.status!=NF18B2_ELIGIBLE){fprintf(stderr,"RET-C owned predicate 18: t->revalidation.status!=NF18B2_ELIGIBLE\n");return NULL;}
 if(t->revalidation.physical!=NF18B2_ELIGIBLE){fprintf(stderr,"RET-C owned predicate 19: t->revalidation.physical!=NF18B2_ELIGIBLE\n");return NULL;}
 if(t->revalidation.contract!=NF18B2_ELIGIBLE){fprintf(stderr,"RET-C owned predicate 20: t->revalidation.contract!=NF18B2_ELIGIBLE\n");return NULL;}
 if(q->intent.actor_id!=e->actor){fprintf(stderr,"RET-C owned predicate 21: q->intent.actor_id!=e->actor\n");return NULL;}
 if(q->intent.object_id!=e->object){fprintf(stderr,"RET-C owned predicate 22: q->intent.object_id!=e->object\n");return NULL;}
 if(q->intent.action_id!=e->action){fprintf(stderr,"RET-C owned predicate 23: q->intent.action_id!=e->action\n");return NULL;}
 if(q->witness.tick!=p->world.tick){fprintf(stderr,"RET-C owned predicate 24: q->witness.tick!=p->world.tick\n");return NULL;}
 if(q->witness.world_revision==UINT32_MAX){fprintf(stderr,"RET-C owned predicate 25: q->witness.world_revision==UINT32_MAX\n");return NULL;}
 if(q->witness.world_revision+1u!=p->world.revision){fprintf(stderr,"RET-C owned predicate 26: q->witness.world_revision+1u!=p->world.revision\n");return NULL;}
 if(q->witness.material_epoch!=p->world.grid.global_revision){fprintf(stderr,"RET-C owned predicate 27: q->witness.material_epoch!=p->world.grid.global_revision\n");return NULL;}
 if(j->encounter->frame.scene.actors[0].id!=e->actor){fprintf(stderr,"RET-C owned predicate 28: j->encounter->frame.scene.actors[0].id!=e->actor\n");return NULL;}
 if(j->encounter->frame.scene.objects[0].id!=e->object){fprintf(stderr,"RET-C owned predicate 29: j->encounter->frame.scene.objects[0].id!=e->object\n");return NULL;}
 /* Local B2 authority can only be empty: nonempty replay state must be
    provided by a genuine A6-owned persisted authority, not invented here. */
 Nf18b2Authority authority={0};
 NfRetcJointContext derived=*j;
 derived.use=q->witness;
 derived.decision=t->revalidation;
 derived.actor=j->encounter->frame.scene.actors[0];
 derived.object=j->encounter->frame.scene.objects[0];
 derived.authority=authority;
 return nf_retc_joint_stage(w,p,&derived);
}

static Nf18a6OwnerGate joint_begin(const NfWorld *w,uint32_t tick,void *arg){
 NfRetcJointContext *j=arg;
 if(!j||!j->encounter||!j->push)return NF18A6_GATE_INVALID;
 if(j->rollback_encounter)return NF18A6_GATE_INVALID;
 j->rollback_encounter=malloc(sizeof *j->rollback_encounter);
 if(!j->rollback_encounter)return NF18A6_GATE_INVALID;
 *j->rollback_encounter=*j->encounter;
 const Nf18a6OwnerGate b3=nf18b6_push_prepare_joint(j->push,w,tick);
 if(b3!=NF18A6_GATE_READY)return b3;
 return nf18b2_encounter_prepare_joint(j->encounter,w,tick);
}
static void joint_finish(const NfWorld *w,const Nf18a6Outcome *out,void *arg){
 NfRetcJointContext *j=arg;
 if(!j||!j->rollback_encounter)return;
 /* Roll back speculative preparation on rejection; committed state is
    derived exclusively from the pending durable A6 frame. */
 if(!out||out->status!=NF18A6_COMMITTED){
    *j->encounter=*j->rollback_encounter;
 }else{
    NfRetcFrame *frame=j->owner->retc_pending;
    if(frame&&j->owner->retc_publisher&&
       j->owner->retc_publisher->count<NF_RETC_HISTORY&&
       (!nf_retc_read(j->owner->retc_publisher)||
        nf_retc_read(j->owner->retc_publisher)->epoch+1u==frame->epoch)&&
       frame->epoch==out->tick&&frame->has_encounter_registry&&
       frame->event.kind==NF18B3_PUSH&&frame->event.phase==NF18B3_APPLIED){
       j->encounter->registry=frame->encounter_registry;
       j->push->event=frame->event;
       if(j->push->receipt_count<NF18B6_PUSH_RECEIPTS){
         j->push->receipts[j->push->receipt_count++]=(Nf18b6PushReceipt){
           frame->event.actor,frame->event.action,frame->event.tick};
       }
       j->push->published_epoch=frame->epoch;
       j->push->armed=false;
       /* The public pointer changes only after A6 physical projection AND
          B2/B3 projection, while the world tick owner is still active. */
       if(nf_retc_publish(j->owner->retc_publisher,frame)){
           j->owner->retc_pending=NULL;
           /* One accepted encounter is consumed; history/receipt authority
              persists, but stale pending intents may not block tick N+1. */
           j->encounter->queue_count=0;
           j->encounter->prepared=0;
           for(size_t k=0;k<NF18B2_MAX_REQUESTS;++k)
               j->encounter->granted[k]=0;
           j->push->staged_commit_valid=false;
           j->push->last_result=NF18B3_OK;
       }
    }
 }
 free(j->rollback_encounter);
 j->rollback_encounter=NULL;
 (void)w;
}
static NfRetcFrame *joint_stage_checked(const NfWorld *w,
 const Nf18a5Integrated *candidate,void *arg){
 NfRetcJointContext *j=arg;
 if(!j||!candidate||!j->push||!j->encounter||
    !nf18b2_encounter_validate_joint(j->encounter,w,candidate)||
    !j->push->armed||!j->push->event.authorized||
    j->push->event.phase!=NF18B3_WAITING||
    j->push->event.kind!=NF18B3_PUSH||
    j->push->event.tick!=candidate->world.tick||
    j->push->event.actor!=candidate->actor.id||
    j->push->event.object!=candidate->object.id||
    j->push->event.contract_epoch!=j->encounter->staged.contract_epoch||
    candidate->committed_receipts<=j->push->before_contacts||
    candidate->world.history.raw_samples<=j->push->before_history)
     return NULL;
 /* Prepare, never realize: no independent B3 finish or secondary WAL. */
 const Nf18b3Event prior=j->push->staged_commit;
 const bool prior_valid=j->push->staged_commit_valid;
 j->push->staged_commit=j->push->event;
 j->push->staged_commit.phase=NF18B3_APPLIED;
 j->push->staged_commit.applied=1;
 j->push->staged_commit_valid=true;
 NfRetcFrame *f=nf_retc_joint_stage_owned(w,candidate,j);
 j->push->staged_commit=prior;
 j->push->staged_commit_valid=prior_valid;
 return f;
}
/* Exclusive owner binding deliberately supplies no independent finish hook:
   only a future accepted-frame projector may update B2/B3 live state. */
bool nf_retc_joint_attach(NfWorld *w,NfRetcJointContext *j){
 if(!w||!j||!j->owner||!j->encounter||!j->push||
    !j->owner->retc_publisher||!j->owner->retc_journal||
    w->tick_in_progress||w->tick_owner_context!=j->owner||
    j->encounter->world!=w||j->encounter->owner!=j->owner||
    j->push->runtime!=j->owner||
    j->owner->owner_hooks.context||
    j->owner->owner_hooks.begin||j->owner->owner_hooks.prepublish||
    j->owner->owner_hooks.accept_candidate||
    j->owner->owner_hooks.stage_joint||
    j->owner->owner_hooks.finish||
    j->encounter->attached)return false;
 j->owner->owner_hooks.begin=joint_begin;
 j->owner->owner_hooks.stage_joint=joint_stage_checked;
 j->owner->owner_hooks.finish=joint_finish;
 j->owner->owner_hooks.context=j;
 return true;
}
bool nf_retc_joint_detach(NfWorld *w,NfRetcJointContext *j){
 if(!w||!j||!j->owner||w->tick_in_progress||
    w->tick_owner_context!=j->owner||
    j->owner->owner_hooks.context!=j||
    j->owner->owner_hooks.stage_joint!=joint_stage_checked||
    j->owner->owner_hooks.begin!=joint_begin||
    j->owner->owner_hooks.finish!=joint_finish||
    j->rollback_encounter)return false;
 j->owner->owner_hooks=(Nf18a6OwnerHooks){0};
 return true;
}
