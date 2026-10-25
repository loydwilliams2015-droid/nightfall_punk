#include "nf_retc_joint_provider.h"
#include <stdlib.h>
NfRetcFrame *nf_retc_joint_stage(const NfWorld *w,
 const Nf18a5Integrated *p,void *context){
 NfRetcJointContext *j=context;
 if(!j||!w||!p||!j->owner||!j->encounter||!j->push||
    !j->transaction_id||!w->tick_in_progress||
    w->tick_owner_context!=j->owner||
    j->encounter->world!=w||j->encounter->owner!=j->owner||
    j->push->runtime!=j->owner||
    j->owner->retc_publisher==NULL||
    !j->encounter->prepared||!j->push->armed||
    !j->push->staged_commit_valid||
    j->push->staged_commit.kind!=NF18B3_PUSH||
    j->push->staged_commit.phase!=NF18B3_APPLIED||
    !j->push->staged_commit.applied||
    j->push->staged_commit.tick!=p->world.tick||
    j->encounter->staged.contract_epoch!=j->push->contract_epoch||
    p->world.tick!=w->tick+1u||
    j->owner->physical.world.tick!=w->tick)
    return NULL;
 const Nf18b3Event *e=&j->push->staged_commit;
 NfRetcFrame *f=nf_retc_stage_source_push(p,e,j->transaction_id,
      (uint32_t)w->tick,&j->use,&j->decision,
      &j->actor,&j->object,&j->authority);
 if(!f)return NULL;
 if(!nf_retc_bind_encounter(f,j->encounter)){
    free(f);return NULL;
 }
 return f;
}
