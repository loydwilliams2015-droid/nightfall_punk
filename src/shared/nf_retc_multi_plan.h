#ifndef NF_RETC_MULTI_PLAN_H
#define NF_RETC_MULTI_PLAN_H
/* H121-H125: bounded, read-only research plan for two genuine B2 proposals.
 * This is NOT an A6 commit, B3 receipt, or an accepted NfRetcFrame.
 * No caller may publish a plan or treat it as contractual realization. */
#include "nf_encounter18b2.h"
#include <string.h>
typedef struct NfRetcMultiPlan {
 Nf18b2Use uses[2];
 Nf18b2UseDecision decisions[2];
 uint32_t actor_ids[2];
 uint32_t object_id,assessed_tick,world_revision,material_epoch;
 uint8_t prepared;
} NfRetcMultiPlan;
static inline bool nf_retc_multi_plan_two(const Nf18b2EncounterRuntime *enc,
 size_t first_index,size_t second_index,NfRetcMultiPlan *out){
 if(!out)return false;
 *out=(NfRetcMultiPlan){0};
 if(!enc||first_index==second_index||first_index>=enc->queue_count||
    second_index>=enc->queue_count||!enc->frame.scene.actors||
    !enc->frame.scene.objects)return false;
 const Nf18b2QueuedIntent *a=&enc->queue[first_index];
 const Nf18b2QueuedIntent *b=&enc->queue[second_index];
 if(a->withdrawn||b->withdrawn||a->trace_index>=enc->trace_count||
    b->trace_index>=enc->trace_count||
    !a->intent.actor_id||a->intent.actor_id==b->intent.actor_id||
    !a->intent.object_id||a->intent.object_id!=b->intent.object_id||
    a->intent.kind!=NF18B2_PUSH||b->intent.kind!=NF18B2_PUSH||
    a->witness.actor_id!=a->intent.actor_id||
    b->witness.actor_id!=b->intent.actor_id||
    a->witness.object_id!=a->intent.object_id||
    b->witness.object_id!=b->intent.object_id||
    a->witness.action_id!=a->intent.action_id||
    b->witness.action_id!=b->intent.action_id||
    a->witness.tick!=b->witness.tick||
    a->witness.world_revision!=b->witness.world_revision||
    a->witness.material_epoch!=b->witness.material_epoch||
    /* H127: registration can advance the registry between independently
       proposed intents. A prior registry number is NOT permission: retain
       strict contract epoch and freshly re-evaluate each B2 source witness.
       Zero and future registry versions are impossible authorization bases. */
    !a->registry_revision||!b->registry_revision||
    a->registry_revision>enc->registry.revision||
    b->registry_revision>enc->registry.revision||
    a->contract_epoch!=enc->registry.contract_epoch||
    b->contract_epoch!=enc->registry.contract_epoch)return false;
 const Nf18b2EncounterTrace *ta=&enc->traces[a->trace_index];
 const Nf18b2EncounterTrace *tb=&enc->traces[b->trace_index];
 if(ta->intent.actor_id!=a->intent.actor_id||
    tb->intent.actor_id!=b->intent.actor_id||
    ta->authorization.status!=NF18B2_ELIGIBLE||
    tb->authorization.status!=NF18B2_ELIGIBLE)return false;
 const Nf18b2UseDecision da=nf18b2_evaluate(
     &enc->frame.scene,a->witness,enc->model);
 const Nf18b2UseDecision db=nf18b2_evaluate(
     &enc->frame.scene,b->witness,enc->model);
 if(da.status!=NF18B2_ELIGIBLE||db.status!=NF18B2_ELIGIBLE||
    da.physical!=NF18B2_ELIGIBLE||db.physical!=NF18B2_ELIGIBLE||
    da.contract!=NF18B2_ELIGIBLE||db.contract!=NF18B2_ELIGIBLE)return false;
 Nf18b2Use uses[2]={a->witness,b->witness};
 Nf18b2UseDecision resolved[2]={{0}};
 if(!nf18b2_resolve(&enc->frame.scene,uses,2u,enc->model,resolved)||
    resolved[0].status!=NF18B2_ELIGIBLE||
    resolved[1].status!=NF18B2_ELIGIBLE)return false;
 out->uses[0]=uses[0];out->uses[1]=uses[1];
 out->decisions[0]=resolved[0];out->decisions[1]=resolved[1];
 out->actor_ids[0]=uses[0].actor_id;out->actor_ids[1]=uses[1].actor_id;
 out->object_id=uses[0].object_id;
 out->assessed_tick=uses[0].tick;
 out->world_revision=uses[0].world_revision;
 out->material_epoch=uses[0].material_epoch;
 out->prepared=1u;
 return true;
}
/* H132-H136: a two-event PROPOSAL, not a B3 realization. The event structs
 * carry typed source identities only. Authorized/applied are always zero.
 * The A6 single-event frame and durable journal MUST NOT consume this. */
#include "nf_interact18b3.h"
typedef struct NfRetcMultiEventProposal {
 NfRetcMultiPlan plan;
 Nf18b3Event events[2];
 uint8_t complete;
} NfRetcMultiEventProposal;
static inline bool nf_retc_multi_propose_b3(const Nf18b2EncounterRuntime *enc,
 size_t first_index,size_t second_index,NfRetcMultiEventProposal *out){
 if(!out)return false;
 *out=(NfRetcMultiEventProposal){0};
 NfRetcMultiPlan p={0};
 if(!nf_retc_multi_plan_two(enc,first_index,second_index,&p)||
    !p.prepared||!enc->registry.contract_epoch)return false;
 for(size_t i=0;i<2u;++i){
  const Nf18b2Use *u=&p.uses[i];
  if(!u->actor_id||!u->object_id||!u->action_id||!u->tick)return false;
  out->events[i]=(Nf18b3Event){
    .actor=u->actor_id,.object=u->object_id,.action=u->action_id,
    .tick=u->tick,.world_epoch=u->world_revision,
    .material_epoch=u->material_epoch,
    .contract_epoch=enc->registry.contract_epoch,
    .kind=NF18B3_PUSH,.phase=NF18B3_PROPOSED,
    .authorized=0u,.applied=0u
  };
 }
 out->plan=p;
 out->complete=1u;
 return true;
}

/* H137-H141: read-only freshness seal for a research proposal. This
 * never grants, stages, realizes, persists or publishes B3 consequences.
 * The current registry/scene is authoritative; captured bytes cannot
 * override a changed source witness or authorization trace. */
static inline bool nf_retc_multi_validate_b3(
 const Nf18b2EncounterRuntime *enc,size_t first_index,
 size_t second_index,const NfRetcMultiEventProposal *proposal){
 if(!enc||!proposal||!proposal->complete||!proposal->plan.prepared)
   return false;
 NfRetcMultiEventProposal fresh={0};
 if(!nf_retc_multi_propose_b3(enc,first_index,second_index,&fresh))
   return false;
 if(proposal->plan.object_id!=fresh.plan.object_id||
    proposal->plan.assessed_tick!=fresh.plan.assessed_tick||
    proposal->plan.world_revision!=fresh.plan.world_revision||
    proposal->plan.material_epoch!=fresh.plan.material_epoch)
   return false;
 for(size_t i=0;i<2u;++i){
  const Nf18b3Event *a=&proposal->events[i],*b=&fresh.events[i];
  if(proposal->plan.actor_ids[i]!=fresh.plan.actor_ids[i]||
     memcmp(&proposal->plan.uses[i],&fresh.plan.uses[i],
            sizeof proposal->plan.uses[i])!=0||
     memcmp(&proposal->plan.decisions[i],&fresh.plan.decisions[i],
            sizeof proposal->plan.decisions[i])!=0||
     a->actor!=b->actor||a->object!=b->object||a->action!=b->action||
     a->tick!=b->tick||a->world_epoch!=b->world_epoch||
     a->material_epoch!=b->material_epoch||
     a->contract_epoch!=b->contract_epoch||
     a->kind!=NF18B3_PUSH||a->phase!=NF18B3_PROPOSED||
     a->authorized||a->applied)return false;
 }
 return true;
}

#endif
