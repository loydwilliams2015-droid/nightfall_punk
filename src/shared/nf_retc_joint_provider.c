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

NfRetcFrame *nf_retc_joint_stage_owned(const NfWorld *w,
 const Nf18a5Integrated *p,void *context){
 NfRetcJointContext *j=context;
 if(!j||!w||!p||!j->encounter||!j->push||!j->owner||
    !j->encounter->prepared||!j->push->staged_commit_valid||
    j->encounter->queue_count!=1u||!j->encounter->granted[0]||
    j->encounter->frame.scene.actor_count!=1u||
    j->encounter->frame.scene.object_count!=1u||
    !j->encounter->frame.scene.actors||
    !j->encounter->frame.scene.objects)return NULL;
 const Nf18b2QueuedIntent *q=&j->encounter->queue[0];
 if(q->withdrawn||q->intent.kind!=NF18B2_PUSH||
    q->trace_index>=j->encounter->trace_count)return NULL;
 const Nf18b2EncounterTrace *t=&j->encounter->traces[q->trace_index];
 const Nf18b3Event *e=&j->push->staged_commit;
 if(t->revalidation.status!=NF18B2_ELIGIBLE||
    t->revalidation.physical!=NF18B2_ELIGIBLE||
    t->revalidation.contract!=NF18B2_ELIGIBLE||
    q->intent.actor_id!=e->actor||q->intent.object_id!=e->object||
    q->intent.action_id!=e->action||q->witness.tick!=p->world.tick||
    q->witness.world_revision!=p->world.revision||
    q->witness.material_epoch!=p->world.grid.global_revision||
    j->encounter->frame.scene.actors[0].id!=e->actor||
    j->encounter->frame.scene.objects[0].id!=e->object)return NULL;
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
