#include "nf_retc_joint_provider.h"
#include "nf_retc_m2_real_contract_audit.h"
#include "nf_retc_multi_plan.h"
#include "nf_retc_multi_preflight.h"
#include "nf_retc_multi_physical.h"
#include "nf_retc_ma03_physics_stage.h"
#include "nf_retc_ma03_a5_envelope.h"
#include "nf_retc_ma03_a5_contact_stage.h"
#include "nf_retc_ma03_owner_tick.h"
#include "nf_retc_external.h"
#include "nf_retc_network.h"
#include "nf_multi_ma03.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"RET-C real dispatcher failure line %d: %s\
",__LINE__,#x);return 1;}}while(0)
static bool b2_provider(void *ctx,const NfWorld *w,const Nf18a6Runtime *owner,
 uint32_t action,Nf18b2Scene *scene,Nf18b2Use *use){
 Nf18b2EncounterRuntime *r=ctx;
 if(!r||r->world!=w||r->owner!=owner||r->queue_count!=1||
    r->queue[0].intent.action_id!=action||!nf18b2_live_frame(r,&r->frame))
    return false;
 *scene=r->frame.scene;*use=r->queue[0].witness;return true;
}
static bool verify(const NfWorld *w,const Nf18a6Runtime *r,
 const Nf18b2EncounterRuntime *enc,const Nf18b6Push *push,
 const NfRetcPublisher *publisher,uint32_t expected){
 NfRetcReadView view={0};
 if(!nf_retc_read_joint_view(publisher,&view))return false;
 const NfRetcFrame *f=view.accepted;
 if(view.epoch!=expected||view.event->tick!=expected||
    view.physical->world.tick!=expected||
    view.eligibility->tick!=expected||
    view.contract->contract_epoch!=view.event->contract_epoch)return false;
 const NfActor *a=nf_world_find_actor_const(w,10);
 const NfActor *other=nf_world_find_actor_const(w,20);
 if(!f||!a||!other||other->id!=20u||
    f->owner_actor==other->id||f->event.actor==other->id||
    f->b2_use.actor_id==other->id||
    f->epoch!=expected||w->tick!=expected||
    r->last_commit_tick!=expected||r->joint_commit_epoch!=expected||
    r->retc_pending||push->receipt_count!=expected||
    f->event.tick!=expected||push->event.tick!=expected||
    push->receipts[expected-1].tick!=expected||
    !f->has_encounter_registry||!f->has_b2_contract||
    memcmp(&enc->registry,&f->encounter_registry,sizeof enc->registry)!=0||
    r->physical.world.tick!=f->physical.world.tick)return false;
 const NfCollider *col=&w->colliders[r->object_collider_index];
 return fabsf(a->transform.position.x-f->physical.actor.center.x)<.0001f&&
    fabsf(a->transform.position.z-f->physical.actor.center.z)<.0001f&&
    fabsf(col->min.x-(f->physical.object.center.x-f->physical.object.box_half.x))<.0001f&&
    fabsf(col->max.x-(f->physical.object.center.x+f->physical.object.box_half.x))<.0001f;
}
typedef struct NfH178OwnerTickProbe {
 NfRetcMa03TickEvidence *evidence;
 unsigned calls,accepted;
} NfH178OwnerTickProbe;
static bool h178_owner_tick_probe(const NfWorld *w,
 const Nf18a6Runtime *owner,float dt,void *context){
 NfH178OwnerTickProbe *p=context;
 if(!p||!p->evidence)return false;
 ++p->calls;
 bool ok=nf_retc_ma03_a5_stage_in_owner_tick(w,owner,dt,p->evidence);
 if(ok)++p->accepted;
 return ok;
}
/* H276E: the tests run *inside* the genuine A6 owner tick after
 * real B2/B3 preparation, not in a synthetic grant fixture.
 * Private clones vary witnesses without changing source authorization. */
static bool (*h276e_original_input)(const NfWorld *,
 const Nf18a6Runtime *,const NfMoveInput *,void *)=NULL;
static unsigned h276e_positive=0u,h276e_denied=0u;
static bool h276e_input_probe(const NfWorld *w,const Nf18a6Runtime *r,
 const NfMoveInput *in,void *context){
 if(!h276e_original_input||
    !h276e_original_input(w,r,in,context))return false;
 NfRetcJointContext *live=(NfRetcJointContext *)context;
 NfRetcJointContext test=*live;
 Nf18b2EncounterRuntime *enc=malloc(sizeof *enc);
 Nf18b6Push *push=malloc(sizeof *push);
 if(!enc||!push){free(enc);free(push);return false;}
 *enc=*live->encounter;*push=*live->push;
 test.encounter=enc;test.push=push;
 /* 1 genuine positive; 2 withdrawn grant, 3 stale revision,
    4 mismatched B3, 5 revoked grant and invalid input all denied. */
 ++h276e_positive;
 enc->queue[0].withdrawn=1u;
 const bool withdrawn=!h276e_original_input(w,r,in,&test);
 enc->queue[0].withdrawn=0u;
 ++enc->queue[0].witness.world_revision;
 const bool stale=!h276e_original_input(w,r,in,&test);
 --enc->queue[0].witness.world_revision;
 ++push->event.action;
 const bool wrong_b3=!h276e_original_input(w,r,in,&test);
 --push->event.action;
 enc->granted[0]=0u;
 const bool revoked=!h276e_original_input(w,r,in,&test);
 enc->granted[0]=1u;
 NfMoveInput no_forward=*in;no_forward.forward=0.f;
 const bool invalid_input=!h276e_original_input(w,r,&no_forward,&test);
 /* H278: all five additional negative probes are inside the real
  * owner tick after an authentic B2 grant and staged B3 PUSH.
  * Restore every private mutation before accepting the live tick. */
 Nf18a6Runtime *mutable_owner=(Nf18a6Runtime *)r;
 const bool old_dual=mutable_owner->require_dual_contact_preflight;
 mutable_owner->require_dual_contact_preflight=true;
 const bool late_dual_flag=!h276e_original_input(w,r,in,&test);
 mutable_owner->require_dual_contact_preflight=old_dual;
 const bool old_callback_present=mutable_owner->research_dual_stage!=NULL;
 if(old_callback_present){free(enc);free(push);return false;}
 mutable_owner->research_dual_stage=h178_owner_tick_probe;
 const bool late_dual_callback=!h276e_original_input(w,r,in,&test);
 mutable_owner->research_dual_stage=NULL;
 test.owner=NULL;
 const bool detached_owner=!h276e_original_input(w,r,in,&test);
 test.owner=live->owner;
 enc->queue_count=2u;
 const bool multiple_requests=!h276e_original_input(w,r,in,&test);
 enc->queue_count=1u;
 ++push->event.object;
 const bool wrong_object=!h276e_original_input(w,r,in,&test);
 --push->event.object;
 /* H279: five real-owner-tick geometry mismatches on the existing
  * authentic B2/B3 grant. Always restore source before publication. */
 const uint32_t saved_actor_id=mutable_owner->physical.actor.id;
 mutable_owner->physical.actor.id=saved_actor_id+1u;
 const bool forged_actor=!h276e_original_input(w,r,in,&test);
 mutable_owner->physical.actor.id=saved_actor_id;
 const float saved_actor_x=mutable_owner->physical.actor.center.x;
 mutable_owner->physical.actor.center.x=saved_actor_x+0.01f;
 const bool detached_actor=!h276e_original_input(w,r,in,&test);
 mutable_owner->physical.actor.center.x=saved_actor_x;
 const float saved_object_x=mutable_owner->physical.object.center.x;
 mutable_owner->physical.object.center.x=saved_object_x+0.01f;
 const bool stale_crate=!h276e_original_input(w,r,in,&test);
 mutable_owner->physical.object.center.x=saved_object_x;
 NfWorld *mutable_world=(NfWorld *)w;
 NfCollider *object_col=&mutable_world->colliders[
   mutable_owner->object_collider_index];
 const float saved_min_x=object_col->min.x;
 object_col->min.x=saved_min_x+0.01f;
 const bool stale_min_x=!h276e_original_input(w,r,in,&test);
 object_col->min.x=saved_min_x;
 const float saved_max_z=object_col->max.z;
 object_col->max.z=saved_max_z+0.01f;
 const bool stale_max_z=!h276e_original_input(w,r,in,&test);
 object_col->max.z=saved_max_z;
 /* H280: a real B2 PUSH can be granted for an off-axis trajectory,
  * but a 180-degree yaw inversion must not authorize PUSH movement
  * away from the source-bound object even with an unchanged grant. */
 NfMoveInput away=*in;
 away.yaw_radians=3.14159265f;
 const bool reverse_approach=!h276e_original_input(w,r,&away,&test);
 free(enc);free(push);
 if(!(withdrawn&&stale&&wrong_b3&&revoked&&invalid_input&&
      late_dual_flag&&late_dual_callback&&detached_owner&&
      multiple_requests&&wrong_object&&forged_actor&&
      detached_actor&&stale_crate&&stale_min_x&&stale_max_z&&
      reverse_approach))return false;
 h276e_denied+=16u;
 return h276e_original_input(w,r,in,context);
}
int main(void){
 NfWorld *w=calloc(1,sizeof *w);
 Nf18a6Runtime *r=calloc(1,sizeof *r);
 Nf18b2EncounterRuntime *enc=calloc(1,sizeof *enc);
 Nf18b6Push *push=calloc(1,sizeof *push);
 NfRetcPublisher *publisher=calloc(1,sizeof *publisher);
 CHECK(w&&r&&enc&&push&&publisher);
 nf_world_init(w,42);
 nf_world_add_collider(w,NF_COLLIDER_SOLID,(NfVec3){0,-.5f,0},(NfVec3){4,0,4});
 CHECK(nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,(NfVec3){1,0,1})==10);
 /* H72: real A6 dispatcher runs in a world with a second actor present.
    The second actor is NOT yet part of a shared two-actor B2/B3 transaction.
    This is a coexistence / no-misattribution gate, not full MA03 acceptance. */
 CHECK(nf_world_spawn_actor_with_id(w,20,NF_FACTION_PLAYER,(NfVec3){12,0,12})==20);
 CHECK(nf_world_find_actor_const(w,20)!=NULL);
 NfActor *actor=nf_world_find_actor(w,10);CHECK(actor);
 actor->movement.grounded=true;actor->movement.mode=NF_MOVE_GROUND;
 Nf18a4Body crate={0};crate.id=2010;crate.center=(NfVec3){1.5601f,.9f,1};
 crate.inverse_mass=1.f/24.f;crate.box_half=(NfVec3){.2f,.5f,.35f};
 CHECK(nf18a6_init(r,w,10,crate,3,NF18A6_WORLD_BRIDGE));
 r->allow_passive_coactors=true; /* H73 explicit research opt-in */
 uint8_t empty[NF18A5_CANON_VOXELS]={0};
 CHECK(nf18a5_load_canonical(&r->physical.world.grid,0,0,0,empty,1));
 CHECK(nf18b2_encounter_init(enc,w,r,NF18B2_M4_MATERIAL_FIRST));
 CHECK(nf18b2_register_actor(enc,10,0,0,50));
 Nf18b2Object object={0};
 object.id=2010;object.revision=1;object.cell_id=1;
 object.nexus_id=1;object.slot_id=1;object.capacity=2; /* H98 authorized two-place contract */
 /* Interaction point sits on the actor-facing crate surface;
    the crate center is materially occluded and must not be used as a handle. */
 object.target=(NfVec3){crate.center.x-crate.box_half.x,crate.center.y,crate.center.z};
 object.max_reach=3.f;object.preconditions=1;
 CHECK(nf18b2_register_object(enc,object,r->physical.world.grid.global_revision,0,0));
 const char *journal="/tmp/nightfall_retc_real_twotick.bin";
 remove(journal);
 CHECK(nf18a6_retc_bind(r,publisher,journal));
 /* H77: the opt-in A6 bind must depend on the same real MA03
    source actor roster used to stage independent actor inputs. */
 NfMa03Roster roster={0};
 CHECK(nf_ma03_roster_capture(w,&roster));
 CHECK(roster.count==2&&roster.actors[0].id==10&&roster.actors[1].id==20);
 CHECK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 push->runtime=r;push->contract_epoch=enc->registry.contract_epoch;
 CHECK(nf18b6_push_require_b2(push,b2_provider,enc,NF18B2_M4_MATERIAL_FIRST));
 NfRetcJointContext joint={0};joint.owner=r;joint.encounter=enc;
 joint.push=push;joint.transaction_id=1;
 joint.require_contact_lineage=true; /* H276C real source gate */
 /* H276E probe installs only after the genuine joint provider attaches. */
 /* H276D: publication ownership cannot be borrowed by an H275F
    experimental candidate. Test real provider attachment, never a
    synthetic grant. Every rejected combination preserves zero frames. */
 const NfRetcFrame *h276d_prior=nf_retc_read(publisher);
 const uint64_t h276d_tick=w->tick;
 /* D1: isolated actor-object A4/A5 research mode refuses B2/B3 attach. */
 r->require_pair_research=true;
 CHECK(!nf_retc_joint_attach(w,&joint));
 CHECK(nf_retc_read(publisher)==h276d_prior&&w->tick==h276d_tick);
 r->require_pair_research=false;
 /* D2: multi-actor research preflight cannot become publication authority. */
 r->require_dual_contact_preflight=true;
 CHECK(!nf_retc_joint_attach(w,&joint));
 CHECK(nf_retc_read(publisher)==h276d_prior&&w->tick==h276d_tick);
 r->require_dual_contact_preflight=false;
 /* D3: registered research-only actor-object callback blocks attachment. */
 r->research_pair_stage=h178_owner_tick_probe;
 CHECK(!nf_retc_joint_attach(w,&joint));
 CHECK(nf_retc_read(publisher)==h276d_prior&&w->tick==h276d_tick);
 r->research_pair_stage=NULL;
 /* D4: registered dual-contact experiment blocks attachment. */
 r->research_dual_stage=h178_owner_tick_probe;
 CHECK(!nf_retc_joint_attach(w,&joint));
 CHECK(nf_retc_read(publisher)==h276d_prior&&w->tick==h276d_tick);
 r->research_dual_stage=NULL;
 /* D5: clean, authentic A6/B2/B3 owner can attach. Any subsequent
    forced research-mode toggle is rejected at the stage boundary. */
 CHECK(nf_retc_joint_attach(w,&joint));
 r->require_pair_research=true;
 CHECK(nf_retc_joint_stage_owned(w,&r->physical,&joint)==NULL);
 CHECK(nf_retc_read(publisher)==h276d_prior&&w->tick==h276d_tick);
 r->require_pair_research=false;
 /* H90 actual source-owned encounter queue fixture: force a second queued
    intent into the live B2 runtime, exercise the production joint staging
    boundary, and establish that it cannot produce an accepted frame. This
    is a structural queue test, not proof of two B2-eligible real actors. */
 {
  const size_t old_count=enc->queue_count;
  const bool old_prepared=enc->prepared;
  const bool old_staged=push->staged_commit_valid;
  const NfRetcFrame *before=nf_retc_read(publisher);
  const uint64_t tick_before=w->tick;
  enc->queue_count=2u;
  enc->prepared=true;
  push->staged_commit_valid=true;
  CHECK(nf_retc_joint_stage_owned(w,&r->physical,&joint)==NULL);
  CHECK(nf_retc_read(publisher)==before&&w->tick==tick_before);
  /* H111-H113: the source stage refuses zero, three and maximum queued
     intents, independently of the H90 two-intent refusal. */
  const size_t unsupported_counts[3]={0u,3u,NF18B2_MAX_REQUESTS};
  for(size_t k=0;k<3u;++k){
    enc->queue_count=unsupported_counts[k];
    CHECK(nf_retc_joint_stage_owned(w,&r->physical,&joint)==NULL);
    CHECK(nf_retc_read(publisher)==before&&w->tick==tick_before);
  }
  /* H114: even a single queued intent requires an owner-issued grant. */
  const uint8_t old_grant=enc->granted[0];
  enc->queue_count=1u;
  enc->granted[0]=0u;
  CHECK(nf_retc_joint_stage_owned(w,&r->physical,&joint)==NULL);
  CHECK(nf_retc_read(publisher)==before&&w->tick==tick_before);
  /* H115: current one-event RET-C staging must not silently truncate
     a genuine two-actor B2 scene, even with its single queue slot granted. */
  const size_t old_actor_count=enc->frame.scene.actor_count;
  enc->granted[0]=1u;
  enc->frame.scene.actor_count=2u;
  CHECK(nf_retc_joint_stage_owned(w,&r->physical,&joint)==NULL);
  CHECK(nf_retc_read(publisher)==before&&w->tick==tick_before);
  enc->frame.scene.actor_count=old_actor_count;
  enc->granted[0]=old_grant;
  /* H116: without prepared B2 state no joint stage is authorized. */
  enc->queue_count=1u;
  enc->granted[0]=1u;
  enc->prepared=false;
  CHECK(nf_retc_joint_stage_owned(w,&r->physical,&joint)==NULL);
  CHECK(nf_retc_read(publisher)==before&&w->tick==tick_before);
  enc->prepared=true;
  /* H117: B3 evidence must exist before any joint staging attempt. */
  push->staged_commit_valid=false;
  CHECK(nf_retc_joint_stage_owned(w,&r->physical,&joint)==NULL);
  CHECK(nf_retc_read(publisher)==before&&w->tick==tick_before);
  push->staged_commit_valid=true;
  /* H118: missing authoritative A6 owner is never permission. */
  Nf18a6Runtime *saved_owner=joint.owner;
  joint.owner=NULL;
  CHECK(nf_retc_joint_stage_owned(w,&r->physical,&joint)==NULL);
  CHECK(nf_retc_read(publisher)==before&&w->tick==tick_before);
  joint.owner=saved_owner;
  /* H119: missing B3 provider cannot expose physical-only success. */
  Nf18b6Push *saved_push=joint.push;
  joint.push=NULL;
  CHECK(nf_retc_joint_stage_owned(w,&r->physical,&joint)==NULL);
  CHECK(nf_retc_read(publisher)==before&&w->tick==tick_before);
  joint.push=saved_push;
  /* H120: source stage must refuse two registered object projections
     while its frame is limited to one B3 event and one source object. */
  const size_t saved_object_count=enc->frame.scene.object_count;
  enc->frame.scene.actor_count=1u;
  enc->frame.scene.object_count=2u;
  CHECK(nf_retc_joint_stage_owned(w,&r->physical,&joint)==NULL);
  CHECK(nf_retc_read(publisher)==before&&w->tick==tick_before);
  enc->frame.scene.object_count=saved_object_count;
  enc->frame.scene.actor_count=old_actor_count;
  enc->queue_count=old_count;
  enc->prepared=old_prepared;
  push->staged_commit_valid=old_staged;
 }
 NfRetcReadView first={0};
 CHECK(!nf_retc_read_joint_view(publisher,&first)); /* no accepted frame */
 NfRetcExternalCursor consumer={0},wire_consumer={0},network_consumer={0};
 uint8_t network_packet[NF_RETC_NETWORK_PACKET_BYTES]={0};
 CHECK(nf_retc_network_emit(publisher,network_packet,sizeof network_packet)==0);
 CHECK(NF_RETC_NETWORK_PACKET_BYTES<=NF_NET_MAX_PACKET_BYTES);
 h276e_original_input=r->owner_hooks.authorize_interaction_input;
 CHECK(h276e_original_input!=NULL);
 r->owner_hooks.authorize_interaction_input=h276e_input_probe;
 for(uint32_t tick=1;tick<=2;++tick){
   const uint32_t action=90u+tick;
   /* A6 publisher requires a strictly increasing transaction identity. */
   joint.transaction_id=tick;
   const Nf18b2Status proposed=nf18b2_propose(
     enc,(Nf18b2Intent){10,2010,action,NF18B2_PUSH});
   if(proposed!=NF18B2_ELIGIBLE){
     const Nf18b2EncounterTrace *trace=enc->trace_count?
       &enc->traces[enc->trace_count-1]:NULL;
     const Nf18b2Actor *sample=&enc->frame.actors[0];
     const Nf18a2ShapePolicy shape={sample->radius,sample->height,0,0,1};
     const bool geometry_clear=nf18a3_capsule_clear(enc->frame.scene.geometry,
         sample->feet,shape);
     const Nf18a5Query material_body=nf18a5_sweep_query(
         &r->physical.world.grid,sample->feet,(NfVec3){0,0,0},
         shape,1,(uint32_t)w->tick+1,NULL,NULL,NULL);
     fprintf(stderr,
       "RET-C B2 body probe: radius=%.3f height=%.3f geometry_clear=%d material_query=%d solids=%zu material_epoch=%u\
",
       sample->radius,sample->height,(int)geometry_clear,
       (int)material_body,enc->frame.scene.geometry.solid_count,
       r->physical.world.grid.global_revision);
     fprintf(stderr,
       "RET-C B2 proposal tick=%u status=%d reason=%d physical=%d contract=%d queue=%zu traces=%zu actor=(%.3f,%.3f,%.3f) target=(%.3f,%.3f,%.3f)\
",
       tick,(int)proposed,trace?(int)trace->authorization.reason:-1,
       trace?(int)trace->authorization.physical:-1,
       trace?(int)trace->authorization.contract:-1,
       enc->queue_count,enc->trace_count,
       actor->transform.position.x,actor->transform.position.y,
       actor->transform.position.z,object.target.x,object.target.y,object.target.z);
     return 1;
   }
   const Nf18b3Result armed=nf18b6_push_arm_b2(push,w,action);
   if(armed!=NF18B3_OK){
     const Nf18b2QueuedIntent *q=enc->queue_count?&enc->queue[0]:NULL;
     fprintf(stderr,
       "RET-C B3 arm tick=%u result=%d b2=%d event_phase=%d armed=%d contract_epoch=%u encounter_epoch=%u queue=%zu witness_revision=%u live_revision=%u target=(%.4f,%.4f,%.4f) crate=(%.4f,%.4f,%.4f)\\n",
       tick,(int)armed,(int)push->last_b2_status,
       (int)push->event.phase,(int)push->armed,
       push->contract_epoch,enc->registry.contract_epoch,enc->queue_count,
       q?q->witness.world_revision:0,r->physical.world.revision,
       enc->frame.objects[0].target.x,enc->frame.objects[0].target.y,
       enc->frame.objects[0].target.z,r->physical.object.center.x,
       r->physical.object.center.y,r->physical.object.center.z);
     return 1;
   }
   /* H276C: preserve the genuine pre-A6 source for independent
      physical/material lineage checks after the real joint publication. */
   Nf18a5Integrated *h276c_source=malloc(sizeof *h276c_source);
   CHECK(h276c_source);
   *h276c_source=r->physical;
   /* H276E: a real server-issued B2/B3 PUSH is the only acceptable
      source for A6 interact-held input in this production joint tick. */
   NfMoveInput move={0};move.strafe=1;move.forward=.2f;
   /* H280: tick 2 carries genuine yawed XZ input into the A6 owner. */
   move.yaw_radians=tick==1u?0.f:-.10f;
   move.interact_held=true;
   nf_world_set_input(w,10,move);
   const NfWorldTickStatus status=nf_world_step_checked(w,1.f/(float)NF_TICK_RATE);
   if(status!=NF_WORLD_TICK_COMMITTED){
      fprintf(stderr,"real dispatcher tick %u status=%d lastA6=%d B2=%d B3=%d\
",
       tick,(int)status,(int)r->tick_trace.embodiment_status,
       (int)enc->traces[enc->trace_count-1].revalidation.status,(int)push->last_result);
      return 1;
   }
   CHECK(h276e_positive==tick&&h276e_denied==16u*tick);
   CHECK(verify(w,r,enc,push,publisher,tick));
   const NfRetcFrame *h276c_accepted=nf_retc_read(publisher);
   CHECK(h276c_accepted&&
      nf_retc_joint_contact_lineage(
        h276c_source,&h276c_accepted->physical)); /* C1 actual B2/B3 pass */
   /* H280: record actual physically accepted XZ displacement and require
    * a genuinely off-axis actor result, positive crate PUSH displacement,
    * and source-identical XZ collider projection. Input alone is not proof
    * of contact or a valid accepted world consequence. */
   const Nf18a5Integrated *h280_next=&h276c_accepted->physical;
   const float h280_actor_dx=h280_next->actor.center.x-
                              h276c_source->actor.center.x;
   const float h280_actor_dz=h280_next->actor.center.z-
                              h276c_source->actor.center.z;
   const float h280_crate_dx=h280_next->object.center.x-
                              h276c_source->object.center.x;
   const float h280_crate_dz=h280_next->object.center.z-
                              h276c_source->object.center.z;
   fprintf(stderr,"H280 SOURCE_POSITIVE tick=%u yaw=%.5f actor_dx=%.8f actor_dz=%.8f crate_dx=%.8f crate_dz=%.8f receipts=%u\n",
       tick,move.yaw_radians,h280_actor_dx,h280_actor_dz,
       h280_crate_dx,h280_crate_dz,h280_next->committed_receipts);
   CHECK(isfinite(h280_actor_dx)&&isfinite(h280_actor_dz)&&
         isfinite(h280_crate_dx)&&isfinite(h280_crate_dz));
   CHECK(fabsf(h280_actor_dz)>0.000001f);
   CHECK(h280_crate_dx>0.000001f);
   const NfCollider *h280_col=&w->colliders[r->object_collider_index];
   CHECK(fabsf(h280_col->min.z-
      (h280_next->object.center.z-h280_next->object.box_half.z))<.0001f);
   CHECK(fabsf(h280_col->max.z-
      (h280_next->object.center.z+h280_next->object.box_half.z))<.0001f);
   Nf18a5Integrated *h276c_tamper=malloc(sizeof *h276c_tamper);
   CHECK(h276c_tamper);
   *h276c_tamper=h276c_accepted->physical;
   /* C2 stale source A5 revision denies prospective joint acceptance. */
   ++h276c_source->world.revision;
   CHECK(!nf_retc_joint_contact_lineage(h276c_source,h276c_tamper));
   --h276c_source->world.revision;
   /* C3 forged contact-stream body identity rejects a B3 action even
      when A5 cumulative policy deliberately retains no RAW samples. */
   unsigned h276c_stream=NF18A5_MAX_STREAMS;
   for(unsigned k=0u;k<NF18A5_MAX_STREAMS;++k){
      if(h276c_tamper->world.history.streams[k].active&&
         h276c_tamper->world.history.streams[k].last_tick==tick){
         h276c_stream=k;break;
      }
   }
   CHECK(h276c_stream<NF18A5_MAX_STREAMS);
   h276c_tamper->world.history.streams[h276c_stream].body_b+=1u;
   CHECK(!nf_retc_joint_contact_lineage(h276c_source,h276c_tamper));
   *h276c_tamper=h276c_accepted->physical;
   /* C4 stale contact stream tick cannot borrow real contractual receipt. */
   --h276c_tamper->world.history.streams[h276c_stream].last_tick;
   CHECK(!nf_retc_joint_contact_lineage(h276c_source,h276c_tamper));
   *h276c_tamper=h276c_accepted->physical;
   /* C5 forged source material epoch or receipt count fails closed. */
   ++h276c_tamper->world.history.streams[h276c_stream].last_material_epoch;
   CHECK(!nf_retc_joint_contact_lineage(h276c_source,h276c_tamper));
   *h276c_tamper=h276c_accepted->physical;
   ++h276c_tamper->committed_receipts;
   CHECK(!nf_retc_joint_contact_lineage(h276c_source,h276c_tamper));
   CHECK(nf_retc_read(publisher)==h276c_accepted&&
         nf_retc_joint_contact_lineage(
            h276c_source,&h276c_accepted->physical));
   free(h276c_tamper);
   free(h276c_source);
   /* H276B: all positives below originate in the REAL server-issued
      B2 proposal, real B3 arm, A6 joint owner tick and accepted frame.
      Private clone corruption never changes the authoritative pointer. */
   const NfRetcFrame *h276b_accepted=nf_retc_read(publisher);
   CHECK(h276b_accepted&&
         nf_retc_m2_real_contract_audit(
            w,r,enc,push,h276b_accepted)); /* B1: genuine joint result */
   NfRetcFrame *h276b_probe=malloc(sizeof *h276b_probe);
   CHECK(h276b_probe);
   *h276b_probe=*h276b_accepted;
   /* B2: stale source B2 grant tick cannot masquerade as accepted. */
   h276b_probe->b2_use.tick=tick+1u;
   CHECK(!nf_retc_m2_real_contract_audit(w,r,enc,push,h276b_probe));
   *h276b_probe=*h276b_accepted;
   /* B3: mismatched typed B3 action must be rejected. */
   h276b_probe->event.action+=1u;
   CHECK(!nf_retc_m2_real_contract_audit(w,r,enc,push,h276b_probe));
   *h276b_probe=*h276b_accepted;
   /* B4: invalid material/physical projection never borrows B2 grant. */
   h276b_probe->physical.object.center.x+=.1f;
   CHECK(!nf_retc_m2_real_contract_audit(w,r,enc,push,h276b_probe));
   *h276b_probe=*h276b_accepted;
   /* B5: unbound B2 decision or encounter ledger fails closed; full
      restoration recovers the genuine accepted source without mutation. */
   h276b_probe->b2_decision.contract=NF18B2_BLOCKED;
   CHECK(!nf_retc_m2_real_contract_audit(w,r,enc,push,h276b_probe));
   *h276b_probe=*h276b_accepted;
   h276b_probe->encounter_registry.contract_epoch+=1u;
   CHECK(!nf_retc_m2_real_contract_audit(w,r,enc,push,h276b_probe));
   CHECK(nf_retc_read(publisher)==h276b_accepted&&
         nf_retc_m2_real_contract_audit(w,r,enc,push,h276b_accepted));
   free(h276b_probe);
   /* H43: externally owned record from ONE accepted immutable frame. */
   NfRetcExternalRecord exported={0};
   CHECK(nf_retc_external_capture(publisher,&exported));
   CHECK(nf_retc_external_valid(&exported));
   /* H53: actual NF04 protocol framing of A6 accepted read-only observation. */
   CHECK(nf_retc_network_emit(publisher,network_packet,sizeof network_packet)==
         NF_RETC_NETWORK_PACKET_BYTES);
   CHECK(nf_protocol_peek_type(network_packet,sizeof network_packet)==
         NF_MSG_RETC_OBSERVATION);
   NfRetcExternalRecord network_observed={0};
   CHECK(nf_retc_network_consume(network_packet,sizeof network_packet,
                                 &network_consumer,&network_observed));
   CHECK(network_observed.epoch==tick&&network_observed.transaction_id==tick&&
         network_observed.actor_id==exported.actor_id&&
         network_observed.action_id==exported.action_id&&
         network_observed.contract_epoch==exported.contract_epoch);
   CHECK(!nf_retc_network_consume(network_packet,sizeof network_packet,
                                  &network_consumer,&network_observed)); /* replay */
   CHECK(network_observed.epoch==0&&network_consumer.epoch==tick);
   CHECK(!nf_retc_network_consume(network_packet,sizeof network_packet-1,
                                  &network_consumer,&network_observed)); /* torn */
   uint8_t fake_packet[NF_RETC_NETWORK_PACKET_BYTES]={0};
   memcpy(fake_packet,network_packet,sizeof fake_packet);
   fake_packet[6]=(uint8_t)NF_MSG_SNAPSHOT; /* cannot masquerade as gameplay */
   CHECK(!nf_retc_network_consume(fake_packet,sizeof fake_packet,
                                  &network_consumer,&network_observed));
   memcpy(fake_packet,network_packet,sizeof fake_packet);
   fake_packet[7]=1; /* reserved flag */
   CHECK(!nf_retc_network_consume(fake_packet,sizeof fake_packet,
                                  &network_consumer,&network_observed));
   memcpy(fake_packet,network_packet,sizeof fake_packet);
   fake_packet[40]^=1; /* checksum-protected payload corruption */
   CHECK(!nf_retc_network_consume(fake_packet,sizeof fake_packet,
                                  &network_consumer,&network_observed));
   CHECK(network_consumer.epoch==tick&&network_observed.epoch==0);
   CHECK(nf_retc_network_emit(publisher,network_packet,sizeof network_packet-1)==0);

   uint8_t encoded[NF_RETC_EXTERNAL_WIRE_BYTES]={0};
   CHECK(nf_retc_external_encode(encoded,sizeof encoded,&exported)==sizeof encoded);
   CHECK(encoded[0]==0x4e&&encoded[1]==0x46&&
         encoded[2]==0x45&&encoded[3]==0x58);
   CHECK(nf_retc_external_encode(encoded,sizeof encoded-1,&exported)==0);
   NfRetcExternalRecord decoded={0};
   CHECK(nf_retc_external_decode(encoded,sizeof encoded,&decoded));
   NfRetcExternalRecord consumed={0};
   CHECK(nf_retc_external_consume(&wire_consumer,encoded,sizeof encoded,&consumed));
   CHECK(consumed.epoch==tick);
   CHECK(!nf_retc_external_consume(&wire_consumer,encoded,sizeof encoded,&consumed));
   CHECK(consumed.epoch==0&&wire_consumer.epoch==tick);
   CHECK(decoded.epoch==exported.epoch&&decoded.transaction_id==exported.transaction_id&&
         decoded.actor_id==exported.actor_id&&decoded.object_id==exported.object_id&&
         decoded.action_id==exported.action_id&&
         decoded.first_account_balance==exported.first_account_balance&&
         decoded.actor_center.x==exported.actor_center.x&&
         decoded.object_center.z==exported.object_center.z);
   /* H48: reject corrupt, short, oversize and unsupported wire versions. */
   CHECK(!nf_retc_external_decode(encoded,sizeof encoded-1,&decoded));
   CHECK(decoded.epoch==0);
   CHECK(!nf_retc_external_decode(encoded,0,&decoded));
   CHECK(!nf_retc_external_decode(NULL,sizeof encoded,&decoded));
   CHECK(!nf_retc_external_decode(encoded,sizeof encoded,NULL));
   uint8_t tampered[NF_RETC_EXTERNAL_WIRE_BYTES];
   memcpy(tampered,encoded,sizeof tampered);tampered[2]^=1u;
   CHECK(!nf_retc_external_decode(tampered,sizeof tampered,&decoded));
   memcpy(tampered,encoded,sizeof tampered);tampered[4]^=1u;
   CHECK(!nf_retc_external_decode(tampered,sizeof tampered,&decoded));
   memcpy(tampered,encoded,sizeof tampered);tampered[32]^=1u;
   CHECK(!nf_retc_external_decode(tampered,sizeof tampered,&decoded));
   memcpy(tampered,encoded,sizeof tampered);tampered[91]^=1u;
   CHECK(!nf_retc_external_decode(tampered,sizeof tampered,&decoded));
   CHECK(decoded.epoch==0);
   /* H66: exhaustive ONE-bit tamper sweep across the entire 92-byte
      encoded frame, INCLUDING every checksum byte. An attacker can
      recompute this non-cryptographic checksum: authentication remains
      a separate full-network requirement. */
   for(size_t flip=0;flip<sizeof encoded*8u;++flip){
     memcpy(tampered,encoded,sizeof tampered);
     tampered[flip/8u]^=(uint8_t)(1u<<(flip%8u));
     CHECK(!nf_retc_external_decode(tampered,sizeof tampered,&decoded));
     CHECK(decoded.epoch==0);
   }


   CHECK(nf_retc_external_accept(&consumer,&exported));
   CHECK(!nf_retc_external_accept(&consumer,&exported)); /* replay */
   CHECK(consumer.epoch==tick&&consumer.transaction_id==tick);
   NfRetcExternalRecord malformed=exported;
   malformed.first_account_actor=999;
   CHECK(!nf_retc_external_valid(&malformed));
   malformed=exported;malformed.epoch=0;
   CHECK(!nf_retc_external_valid(&malformed));
   CHECK(!nf_retc_external_valid(NULL));
   /* H51: pathological / boundary source validation and no partial output. */
   malformed=exported;malformed.actor_center.x=NAN;
   CHECK(!nf_retc_external_valid(&malformed));
   CHECK(nf_retc_external_encode(encoded,sizeof encoded,&malformed)==0);
   malformed=exported;malformed.prior_epoch=UINT32_MAX;
   CHECK(!nf_retc_external_valid(&malformed));
   malformed=exported;malformed.transaction_id=0;
   CHECK(!nf_retc_external_valid(&malformed));
   malformed=exported;malformed.account_count=0;
   CHECK(!nf_retc_external_valid(&malformed));
   NfRetcExternalRecord no_record=exported;
   CHECK(!nf_retc_external_capture(publisher,NULL));
   CHECK(!nf_retc_external_consume(&wire_consumer,NULL,sizeof encoded,&no_record));
   CHECK(no_record.epoch==0&&wire_consumer.epoch==tick);

   CHECK(exported.epoch==tick&&exported.transaction_id==tick&&
         exported.actor_id==10&&exported.object_id==2010&&
         exported.action_id==action&&exported.contract_epoch>0&&
         exported.committed_receipts>0&&exported.account_count>0&&
         exported.first_account_actor==10);

   NfRetcReadView current={0};
   CHECK(nf_retc_read_joint_view(publisher,&current));
   CHECK(current.accepted&&current.epoch==tick&&
         current.transaction_id==tick&&current.event->tick==tick&&
         current.physical->world.tick==tick);
   if(tick==1u){
     first=current; /* immutable view remains usable until publisher disposal */
   }else{
     CHECK(first.accepted!=current.accepted);
     CHECK(first.epoch==1u&&first.transaction_id==1u&&
           first.event->tick==1u&&first.physical->world.tick==1u&&
           first.eligibility->tick==1u);
     CHECK(current.epoch==2u&&current.event->action==92u);
   }
 }
 puts("RET-C H42 one-load B2/B3/A5 reader retained both accepted epochs PASS");
 /* H79: at the ACTUAL A6 tick boundary an actor identity collision
    must refuse before publication, even if the bind-time roster passed. */
 {
   NfActor *coactor=nf_world_find_actor(w,20);CHECK(coactor);
   const NfRetcFrame *accepted_before=nf_retc_read(publisher);
   const uint64_t tick_before=w->tick;
   const uint32_t id_before=coactor->id;
   coactor->id=10u;
   CHECK(nf_world_step_checked(w,1.f/(float)NF_TICK_RATE)!=NF_WORLD_TICK_COMMITTED);
   CHECK(w->tick==tick_before&&nf_retc_read(publisher)==accepted_before);
   coactor->id=id_before;
 }

 /* H73 rejection: a co-present second actor cannot secretly request
    movement under the one-actor B2/B3 contract. The accepted frame stays
    byte-identical and the single source-world tick does not advance. */
 {
   NfActor *second=nf_world_find_actor(w,20);CHECK(second);
   const NfRetcFrame *before=nf_retc_read(publisher);
   const uint64_t before_tick=w->tick;
   const NfVec3 before_pos=second->transform.position;
   second->input.forward=1.f;
   CHECK(nf_world_step_checked(w,1.f/(float)NF_TICK_RATE)!=NF_WORLD_TICK_COMMITTED);
   CHECK(nf_retc_read(publisher)==before&&w->tick==before_tick);
   CHECK(memcmp(&second->transform.position,&before_pos,sizeof before_pos)==0);
   second->input.forward=0.f;
   /* H75: input-idle but physically moving co-actor is not passive. */
   second->transform.velocity.x=3.f;
   CHECK(nf_world_step_checked(w,1.f/(float)NF_TICK_RATE)!=NF_WORLD_TICK_COMMITTED);
   CHECK(w->tick==before_tick&&nf_retc_read(publisher)==before);
   CHECK(memcmp(&second->transform.position,&before_pos,sizeof before_pos)==0);
   second->transform.velocity.x=0.f;
   /* H74: distinct second-actor control channels remain unavailable.
      The original owner can request movement simultaneously, but a
      second independent input must refuse the entire authoritative tick. */
   NfMoveInput owner_move={0};owner_move.strafe=1.f;
   nf_world_set_input(w,10,owner_move);
   NfMoveInput coactor_move={0};coactor_move.strafe=-1.f;
   nf_world_set_input(w,20,coactor_move);
   CHECK(nf_world_step_checked(w,1.f/(float)NF_TICK_RATE)!=NF_WORLD_TICK_COMMITTED);
   CHECK(w->tick==before_tick&&nf_retc_read(publisher)==before);
   CHECK(memcmp(&second->transform.position,&before_pos,sizeof before_pos)==0);
   coactor_move=(NfMoveInput){0};
   nf_world_set_input(w,20,coactor_move);
   owner_move=(NfMoveInput){0};
   nf_world_set_input(w,10,owner_move);
 }
 /* Negative replay probe: retain the last committed transaction identity.
    A denial must not advance any physical, B2, B3 or published epoch. */
 {
   const NfRetcFrame *accepted=nf_retc_read(publisher);
   CHECK(accepted&&accepted->epoch==2&&accepted->transaction_id==2);
   const NfVec3 actor_before=nf_world_find_actor_const(w,10)->transform.position;
   const NfVec3 crate_before=r->physical.object.center;
   const uint32_t receipts_before=push->receipt_count;
   const uint32_t accepted_before=r->accepted;
   const uint64_t tick_before=w->tick;
   joint.transaction_id=2; /* deliberate replay */
   const uint32_t action=93u;
   CHECK(nf18b2_propose(enc,(Nf18b2Intent){10,2010,action,NF18B2_PUSH})==NF18B2_ELIGIBLE);
   CHECK(nf18b6_push_arm_b2(push,w,action)==NF18B3_OK);
   /* The rollback baseline is the authorized, staged pre-tick state. */
   const Nf18b2Registry registry_before=enc->registry;
   NfMoveInput move={0};move.strafe=1;
   nf_world_set_input(w,10,move);
   CHECK(nf_world_step_checked(w,1.f/(float)NF_TICK_RATE)!=NF_WORLD_TICK_COMMITTED);
   CHECK(w->tick==tick_before&&r->accepted==accepted_before);
   CHECK(nf_retc_read(publisher)==accepted&&push->receipt_count==receipts_before);
   NfRetcReadView replay_view={0};
   CHECK(nf_retc_read_joint_view(publisher,&replay_view));
   CHECK(replay_view.accepted==accepted&&replay_view.epoch==2u&&
         replay_view.event->tick==2u&&replay_view.physical->world.tick==2u);
   /* H53: rejected world transaction cannot produce a new NF04 observation.
      The last accepted wire packet stays byte-exact and replay-denied. */
   uint8_t after_rejection[NF_RETC_NETWORK_PACKET_BYTES]={0};
   CHECK(nf_retc_network_emit(publisher,after_rejection,sizeof after_rejection)==
         sizeof after_rejection);
   CHECK(memcmp(after_rejection,network_packet,sizeof after_rejection)==0);
   NfRetcExternalRecord denied_packet={0};
   CHECK(!nf_retc_network_consume(after_rejection,sizeof after_rejection,
                                  &network_consumer,&denied_packet));
   CHECK(denied_packet.epoch==0&&network_consumer.epoch==2u);
   CHECK(memcmp(&enc->registry,&registry_before,sizeof registry_before)==0);
   CHECK(memcmp(&r->physical.object.center,&crate_before,sizeof crate_before)==0);
   CHECK(memcmp(&nf_world_find_actor_const(w,10)->transform.position,&actor_before,sizeof actor_before)==0);
   puts("RET-C replay rejection: world, registry, publisher and receipts unchanged PASS");
 }
 /* H91 positive source-owned registration for the second REAL world actor.
    This is NOT dual eligibility or dual publication: existing A6/B3
    projection still supports only actor 10. */
 CHECK(nf18b2_register_actor(enc,20,0,0,50));
 CHECK(enc->registry.account_count==2u);
 CHECK(nf18b2_live_frame(enc,&enc->frame));
 CHECK(enc->frame.scene.actor_count==2u);
 CHECK(enc->frame.actors[0].id==10u||enc->frame.actors[1].id==10u);
 CHECK(enc->frame.actors[0].id==20u||enc->frame.actors[1].id==20u);
 CHECK(nf_retc_read(publisher)!=NULL&&w->tick==2u);
 /* H92: actual independent B2 propose calls from two registered world
    identities. Both submissions must pass the real evaluator to count as
    a positive two-intent milestone; no accepted dual-actor frame exists. */
 {
   const NfRetcFrame *accepted=nf_retc_read(publisher);
   const uint64_t previous_tick=w->tick;
   const size_t prior_count=enc->queue_count;
   const Nf18b2Status first=nf18b2_propose(
      enc,(Nf18b2Intent){10,2010,601,NF18B2_PUSH});
   const Nf18b2Status second=nf18b2_propose(
      enc,(Nf18b2Intent){20,2010,602,NF18B2_PUSH});
   CHECK(nf_retc_read(publisher)==accepted&&w->tick==previous_tick);
   /* Record exact B2 status, do not invent or promote an eligible grant. */
   fprintf(stderr,"H92 real B2 submissions: actor10=%d actor20=%d queue=%zu\\n",
        (int)first,(int)second,enc->queue_count);
   CHECK(first!=NF18B2_INVALID&&second!=NF18B2_INVALID);
   CHECK(enc->queue_count==prior_count+2u);
   /* H93: source-issued per-proposal decisions, not inferred eligibility. */
   const Nf18b2QueuedIntent *q0=&enc->queue[prior_count];
   const Nf18b2QueuedIntent *q1=&enc->queue[prior_count+1u];
   CHECK(q0->intent.actor_id==10u&&q1->intent.actor_id==20u);
   CHECK(q0->trace_index<enc->trace_count&&q1->trace_index<enc->trace_count);
   const Nf18b2EncounterTrace *t0=&enc->traces[q0->trace_index];
   const Nf18b2EncounterTrace *t1=&enc->traces[q1->trace_index];
   CHECK(t0->intent.actor_id==10u&&t1->intent.actor_id==20u);
   CHECK(first==t0->authorization.status&&second==t1->authorization.status);
   /* H94 classify the real second actor's source decision explicitly.
      Reject any unsupported claim that distant actor 20 is eligible. */
   CHECK(second!=NF18B2_ELIGIBLE);
   CHECK(t1->authorization.physical!=NF18B2_ELIGIBLE ||
         t1->authorization.contract!=NF18B2_ELIGIBLE);
   fprintf(stderr,"H93 source B2 authorization: actor10 status=%d reason=%d physical=%d contract=%d; actor20 status=%d reason=%d physical=%d contract=%d\\n",
    (int)t0->authorization.status,(int)t0->authorization.reason,
    (int)t0->authorization.physical,(int)t0->authorization.contract,
    (int)t1->authorization.status,(int)t1->authorization.reason,
    (int)t1->authorization.physical,(int)t1->authorization.contract);
 }
 /* H95 positive geometry probe uses real world actor 20 and genuine B2
    evaluation. Existing far-away H94 denial remains independently tested.
    No multi-actor frame is published. */
 {
  NfActor *second_actor=nf_world_find_actor(w,20u);
  CHECK(second_actor!=NULL);
  const NfVec3 old_pos=second_actor->transform.position;
  const size_t old_count=enc->queue_count;
  const NfRetcFrame *accepted=nf_retc_read(publisher);
  second_actor->transform.position=(NfVec3){1.0f,0.f,1.65f};
  const Nf18b2Status nearby=nf18b2_propose(
       enc,(Nf18b2Intent){20,2010,603,NF18B2_PUSH});
  fprintf(stderr,"H95 nearby source B2: status=%d count=%zu\\n",
      (int)nearby,enc->queue_count);
  CHECK(nearby!=NF18B2_INVALID);
  CHECK(enc->queue_count==old_count+1u);
  const Nf18b2QueuedIntent *q=&enc->queue[old_count];
  CHECK(q->intent.actor_id==20u&&q->trace_index<enc->trace_count);
  const Nf18b2EncounterTrace *t=&enc->traces[q->trace_index];
  fprintf(stderr,"H95 nearby reason=%d physical=%d contract=%d\\n",
      (int)t->authorization.reason,(int)t->authorization.physical,
      (int)t->authorization.contract);
  CHECK(nearby==t->authorization.status);
  /* H98: a legitimate capacity-two registered object admits actor 20
     after actor 10's established occupancy, without issuing fake grants. */
  CHECK(enc->frame.scene.object_count==1u);
  CHECK(enc->frame.objects[0].capacity==2u);
  CHECK(enc->frame.objects[0].occupant_count==1u);
  CHECK(enc->frame.objects[0].occupants[0]==10u);
  CHECK(t->authorization.physical==NF18B2_ELIGIBLE);
  CHECK(t->authorization.contract==NF18B2_ELIGIBLE);
  CHECK(t->authorization.reason==NF18B2_OK);
  CHECK(nearby==NF18B2_ELIGIBLE);
  /* H121: two authentic independently eligible source B2 proposals can
     form a bounded private plan, without inventing a B3 receipt. */
  const size_t first_index=old_count-2u;
  CHECK(enc->queue[first_index].intent.actor_id==10u);
  NfRetcMultiPlan multi={0};
  const Nf18b2QueuedIntent *h126_first=&enc->queue[first_index];
  const Nf18b2QueuedIntent *h126_second=&enc->queue[old_count];
  const Nf18b2UseDecision h126_first_eval=nf18b2_evaluate(
      &enc->frame.scene,h126_first->witness,enc->model);
  const Nf18b2UseDecision h126_second_eval=nf18b2_evaluate(
      &enc->frame.scene,h126_second->witness,enc->model);
  fprintf(stderr,"H126 plan diagnostics: first_status=%d first_reason=%d first_physical=%d first_contract=%d second_status=%d second_reason=%d first_trace=%d second_trace=%d first_registry=%u second_registry=%u live_registry=%u first_contract_epoch=%u second_contract_epoch=%u live_contract_epoch=%u\\n",
      (int)h126_first_eval.status,(int)h126_first_eval.reason,
      (int)h126_first_eval.physical,(int)h126_first_eval.contract,
      (int)h126_second_eval.status,(int)h126_second_eval.reason,
      (int)enc->traces[h126_first->trace_index].authorization.status,
      (int)enc->traces[h126_second->trace_index].authorization.status,
      h126_first->registry_revision,h126_second->registry_revision,
      enc->registry.revision,h126_first->contract_epoch,h126_second->contract_epoch,
      enc->registry.contract_epoch);
  CHECK(nf_retc_multi_plan_two(enc,first_index,old_count,&multi));
  CHECK(multi.prepared==1u&&multi.actor_ids[0]==10u&&
        multi.actor_ids[1]==20u&&multi.object_id==2010u);
  CHECK(multi.assessed_tick==enc->queue[first_index].witness.tick);
  /* H122: source-authorized plans remain stable under reversed input. */
  NfRetcMultiPlan reverse_multi={0};
  CHECK(nf_retc_multi_plan_two(enc,old_count,first_index,&reverse_multi));
  CHECK(reverse_multi.prepared==1u&&reverse_multi.actor_ids[0]==20u&&
        reverse_multi.actor_ids[1]==10u);
  CHECK(reverse_multi.world_revision==multi.world_revision&&
        reverse_multi.material_epoch==multi.material_epoch);
  /* H123: a stale second-actor revision makes preparation fail closed. */
  const uint32_t saved_revision=q->witness.world_revision;
  ((Nf18b2QueuedIntent *)q)->witness.world_revision++;
  NfRetcMultiPlan denied={0};
  CHECK(!nf_retc_multi_plan_two(enc,first_index,old_count,&denied));
  CHECK(!denied.prepared);
  ((Nf18b2QueuedIntent *)q)->witness.world_revision=saved_revision;
  /* H124: a blocked source trace cannot be laundered into a plan. */
  const Nf18b2Status saved_contract=t->authorization.status;
  ((Nf18b2EncounterTrace *)t)->authorization.status=NF18B2_BLOCKED;
  CHECK(!nf_retc_multi_plan_two(enc,first_index,old_count,&denied));
  CHECK(!denied.prepared);
  ((Nf18b2EncounterTrace *)t)->authorization.status=saved_contract;
  /* H125: duplicate source actor identities and the prepublication plan
     cannot mutate accepted A6 state or add any B3 receipts. */
  CHECK(!nf_retc_multi_plan_two(enc,old_count,old_count,&denied));
  CHECK(!denied.prepared);
  CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u);
  CHECK(push->receipt_count==2u);
  /* H127: acceptance explicitly permits pre-registration revision 15
     only because both current source evaluations and contract epoch pass. */
  CHECK(h126_first->registry_revision<enc->registry.revision);
  CHECK(h126_second->registry_revision==enc->registry.revision);
  CHECK(multi.prepared==1u);
  /* H128: unknown zero registry cannot authorize a stale proposal. */
  const uint32_t saved_registry=h126_first->registry_revision;
  ((Nf18b2QueuedIntent *)h126_first)->registry_revision=0u;
  denied=(NfRetcMultiPlan){0};
  CHECK(!nf_retc_multi_plan_two(enc,first_index,old_count,&denied));
  CHECK(!denied.prepared);
  ((Nf18b2QueuedIntent *)h126_first)->registry_revision=saved_registry;
  /* H129: future registry versions cannot be source-issued authority. */
  ((Nf18b2QueuedIntent *)h126_first)->registry_revision=enc->registry.revision+1u;
  denied=(NfRetcMultiPlan){0};
  CHECK(!nf_retc_multi_plan_two(enc,first_index,old_count,&denied));
  CHECK(!denied.prepared);
  ((Nf18b2QueuedIntent *)h126_first)->registry_revision=saved_registry;
  /* H130: any changed contractual epoch remains fail-closed even
     when each independently reconstructed physical witness is clear. */
  const uint32_t saved_epoch=h126_first->contract_epoch;
  ((Nf18b2QueuedIntent *)h126_first)->contract_epoch=saved_epoch+1u;
  denied=(NfRetcMultiPlan){0};
  CHECK(!nf_retc_multi_plan_two(enc,first_index,old_count,&denied));
  CHECK(!denied.prepared);
  ((Nf18b2QueuedIntent *)h126_first)->contract_epoch=saved_epoch;
  /* H131: stale B2 actor revision must be rejected by fresh evaluation.
     The accepted A6 pointer and B3 receipts remain unchanged. */
  const uint32_t saved_actor_revision=h126_first->witness.actor_revision;
  ((Nf18b2QueuedIntent *)h126_first)->witness.actor_revision++;
  denied=(NfRetcMultiPlan){0};
  CHECK(!nf_retc_multi_plan_two(enc,first_index,old_count,&denied));
  CHECK(!denied.prepared);
  ((Nf18b2QueuedIntent *)h126_first)->witness.actor_revision=saved_actor_revision;
  CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u);
  CHECK(push->receipt_count==2u);
  /* H132: derive two typed B3 proposals from actual eligible B2 sources. */
  NfRetcMultiEventProposal event_plan={0};
  CHECK(nf_retc_multi_propose_b3(enc,first_index,old_count,&event_plan));
  CHECK(event_plan.complete&&event_plan.plan.prepared);
  CHECK(event_plan.events[0].actor==10u&&event_plan.events[1].actor==20u);
  CHECK(event_plan.events[0].object==2010u&&event_plan.events[1].object==2010u);
  CHECK(event_plan.events[0].action==601u&&event_plan.events[1].action==603u);
  /* H133: order reversal does not change either source event identity. */
  NfRetcMultiEventProposal event_reverse={0};
  CHECK(nf_retc_multi_propose_b3(enc,old_count,first_index,&event_reverse));
  CHECK(event_reverse.complete);
  CHECK(event_reverse.events[0].actor==event_plan.events[1].actor);
  CHECK(event_reverse.events[1].actor==event_plan.events[0].actor);
  CHECK(event_reverse.events[0].action==event_plan.events[1].action);
  CHECK(event_reverse.events[1].action==event_plan.events[0].action);
  /* H134: a stale actor witness blocks B3 proposal construction. */
  ((Nf18b2QueuedIntent *)h126_first)->witness.actor_revision++;
  NfRetcMultiEventProposal reject_event={0};
  CHECK(!nf_retc_multi_propose_b3(enc,first_index,old_count,&reject_event));
  CHECK(!reject_event.complete);
  ((Nf18b2QueuedIntent *)h126_first)->witness.actor_revision=saved_actor_revision;
  /* H135: a mismatched contract epoch cannot enter B3 proposal. */
  ((Nf18b2QueuedIntent *)h126_first)->contract_epoch++;
  reject_event=(NfRetcMultiEventProposal){0};
  CHECK(!nf_retc_multi_propose_b3(enc,first_index,old_count,&reject_event));
  CHECK(!reject_event.complete);
  ((Nf18b2QueuedIntent *)h126_first)->contract_epoch=saved_epoch;
  /* H136: typed B3 proposals do not claim authorization/application or
     mutate the sole accepted A6 publication and receipt state. */
  for(size_t n=0;n<2u;++n){
    CHECK(event_plan.events[n].kind==NF18B3_PUSH);
    CHECK(event_plan.events[n].phase==NF18B3_PROPOSED);
    CHECK(event_plan.events[n].authorized==0u);
    CHECK(event_plan.events[n].applied==0u);
    CHECK(event_plan.events[n].tick==event_plan.plan.assessed_tick);
    CHECK(event_plan.events[n].contract_epoch==enc->registry.contract_epoch);
  }
  CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u);
  CHECK(push->receipt_count==2u);
  /* H137: the captured two-event B3 proposal must still match both
     current, independently source-evaluated B2 intents. */
  CHECK(nf_retc_multi_validate_b3(enc,first_index,old_count,&event_plan));
  /* H138: proposals from reversed input are independently verifiable. */
  CHECK(nf_retc_multi_validate_b3(enc,old_count,first_index,&event_reverse));
  /* H139: changing source actor revision invalidates the old proposal. */
  ((Nf18b2QueuedIntent *)h126_first)->witness.actor_revision++;
  CHECK(!nf_retc_multi_validate_b3(enc,first_index,old_count,&event_plan));
  ((Nf18b2QueuedIntent *)h126_first)->witness.actor_revision=saved_actor_revision;
  /* H140: a forged B3 event action or applied flag cannot be sealed. */
  NfRetcMultiEventProposal altered_event=event_plan;
  ++altered_event.events[1].action;
  CHECK(!nf_retc_multi_validate_b3(enc,first_index,old_count,&altered_event));
  altered_event=event_plan;
  altered_event.events[0].applied=1u;
  CHECK(!nf_retc_multi_validate_b3(enc,first_index,old_count,&altered_event));
  altered_event=event_plan;
  altered_event.events[1].authorized=1u;
  CHECK(!nf_retc_multi_validate_b3(enc,first_index,old_count,&altered_event));
  /* H141: revalidation is always prepublication and non-realizing. */
  CHECK(nf_retc_multi_validate_b3(enc,first_index,old_count,&event_plan));
  CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u);
  CHECK(push->receipt_count==2u);
  /* H142: source-exact B3 intentions form one deterministic private batch. */
  NfRetcCanonicalBatch batch={0};
  CHECK(nf_retc_multi_canonical_batch(enc,first_index,old_count,
                                      &event_plan,&batch));
  CHECK(batch.prepared&&batch.events[0].actor==10u&&
        batch.events[1].actor==20u);
  CHECK(batch.events[0].action==601u&&batch.events[1].action==603u);
  /* H143: reverse proposal order yields same canonical source identity. */
  NfRetcCanonicalBatch reversed_batch={0};
  CHECK(nf_retc_multi_canonical_batch(enc,old_count,first_index,
                                      &event_reverse,&reversed_batch));
  CHECK(reversed_batch.prepared&&reversed_batch.tick==batch.tick&&
        reversed_batch.contract_epoch==batch.contract_epoch);
  CHECK(reversed_batch.events[0].actor==batch.events[0].actor&&
        reversed_batch.events[1].actor==batch.events[1].actor&&
        reversed_batch.events[0].action==batch.events[0].action&&
        reversed_batch.events[1].action==batch.events[1].action);
  /* H144: forged B3 action must not enter an authoritative staging queue. */
  NfRetcMultiEventProposal forged_batch_source=event_plan;
  ++forged_batch_source.events[1].action;
  NfRetcCanonicalBatch denied_batch={0};
  CHECK(!nf_retc_multi_canonical_batch(enc,first_index,old_count,
                                       &forged_batch_source,&denied_batch));
  CHECK(!denied_batch.prepared);
  /* H145: changed B2 source actor revision refuses stale batch. */
  ((Nf18b2QueuedIntent *)h126_first)->witness.actor_revision++;
  denied_batch=(NfRetcCanonicalBatch){0};
  CHECK(!nf_retc_multi_canonical_batch(enc,first_index,old_count,
                                       &event_plan,&denied_batch));
  CHECK(!denied_batch.prepared);
  ((Nf18b2QueuedIntent *)h126_first)->witness.actor_revision=saved_actor_revision;
  /* H146: a canonicalized intention is neither granted nor applied;
     recomputation cannot publish A6 or issue real B3 receipts. */
  for(size_t i=0;i<2u;++i){
    CHECK(batch.events[i].kind==NF18B3_PUSH);
    CHECK(batch.events[i].phase==NF18B3_PROPOSED);
    CHECK(batch.events[i].authorized==0u&&!batch.events[i].applied);
  }
  CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u);
  CHECK(push->receipt_count==2u);
  /* H147: only the bound real A6 world owner and accepted source
     snapshot may admit a canonical two-intent batch for planning. */
  CHECK(nf_retc_multi_a6_preflight(w,r,enc,publisher,&batch));
  CHECK(nf_retc_multi_a6_preflight(w,r,enc,publisher,&reversed_batch));
  /* H148: future or replayed tick values cannot masquerade as next tick. */
  NfRetcCanonicalBatch changed_batch=batch;
  changed_batch.tick++;
  CHECK(!nf_retc_multi_a6_preflight(w,r,enc,publisher,&changed_batch));
  changed_batch=batch;
  changed_batch.tick=w->tick;
  CHECK(!nf_retc_multi_a6_preflight(w,r,enc,publisher,&changed_batch));
  /* H149: the real tick-owner binding is required, not a caller claim. */
  void *prior_tick_owner=w->tick_owner_context;
  w->tick_owner_context=NULL;
  CHECK(!nf_retc_multi_a6_preflight(w,r,enc,publisher,&batch));
  w->tick_owner_context=prior_tick_owner;
  /* H150: source actor identities and their ordering must match the
     actual actor roster and single active A6 owner. */
  changed_batch=batch;
  changed_batch.events[1].actor=999999u;
  CHECK(!nf_retc_multi_a6_preflight(w,r,enc,publisher,&changed_batch));
  changed_batch=batch;
  changed_batch.events[0].actor=20u;
  CHECK(!nf_retc_multi_a6_preflight(w,r,enc,publisher,&changed_batch));
  /* H151: absent accepted frame/publisher is never a commit claim;
     all preflights are read-only, retaining old B3 receipt count. */
  CHECK(!nf_retc_multi_a6_preflight(w,r,enc,NULL,&batch));
  CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u);
  CHECK(push->receipt_count==2u);
  /* H152: canonical B2/B3 intentions match real MA03 actors and A5
     source object, without adding movement authority to the second actor. */
  CHECK(nf_retc_multi_physical_preflight(w,r,enc,publisher,&batch));
  /* H153: desynchronizing the real actor20 pose from the B2 scene fails. */
  const NfVec3 before_second_pose=second_actor->transform.position;
  second_actor->transform.position.x+=0.25f;
  CHECK(!nf_retc_multi_physical_preflight(w,r,enc,publisher,&batch));
  second_actor->transform.position=before_second_pose;
  /* H154: an A5 source object identity mismatch fails closed. */
  const uint32_t before_object_id=r->physical.object.id;
  ++r->physical.object.id;
  CHECK(!nf_retc_multi_physical_preflight(w,r,enc,publisher,&batch));
  r->physical.object.id=before_object_id;
  /* H155: active coactor velocity has no source-owned A4/A5 multi-body
     realizer and must be refused, not implicitly treated as idle. */
  const NfVec3 before_second_velocity=second_actor->transform.velocity;
  second_actor->transform.velocity.x=1.f;
  CHECK(!nf_retc_multi_physical_preflight(w,r,enc,publisher,&batch));
  second_actor->transform.velocity=before_second_velocity;
  /* H156: owner-bound physical preflight is read-only and never publishes
     the canonical proposal, changes world tick or adds B3 receipts. */
  CHECK(nf_retc_multi_physical_preflight(w,r,enc,publisher,&batch));
  CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u);
  CHECK(push->receipt_count==2u);
  /* H157-H161: isolated real A6-bound two-actor physical candidate.
     Unlike B2 proposals, this actually executes the A4 two-body solver,
     checks independent analytical impulses, and returns two changed body
     trajectories. It has NO A5/B3 effect and publishes NOTHING. */
  {
   NfWorld *two_world=calloc(1,sizeof *two_world);
   Nf18a6Runtime *two_owner=calloc(1,sizeof *two_owner);
   CHECK(two_world&&two_owner);
   nf_world_init(two_world,77);
   CHECK(nf_world_spawn_actor_with_id(two_world,30,NF_FACTION_PLAYER,
           (NfVec3){0,0,0})==30);
   CHECK(nf_world_spawn_actor_with_id(two_world,40,NF_FACTION_PLAYER,
           (NfVec3){12.f,0,0})==40);
   NfActor *primary=nf_world_find_actor(two_world,30);
   NfActor *secondary=nf_world_find_actor(two_world,40);
   CHECK(primary&&secondary);
   primary->transform.velocity.x=4.f;
   Nf18a4Body inert_crate={0};
   inert_crate.id=3050;inert_crate.center=(NfVec3){4.f,.9f,0.f};
   inert_crate.inverse_mass=1.f/24.f;
   inert_crate.box_half=(NfVec3){.2f,.5f,.35f};
   CHECK(nf18a6_init(two_owner,two_world,30,inert_crate,8,
                      NF18A6_WORLD_BRIDGE));
   two_owner->allow_passive_coactors=true;
   CHECK(nf18a6_bind_world_tick(two_world,two_owner,NULL,NULL,NULL));
   /* Bind under the incumbent distant-passive safety envelope, then
      stage an isolated NEARBY collision candidate without attempting
      a production A6 tick (which must still reject this topology). */
   secondary->transform.position=(NfVec3){.9f,0.f,0.f};
   const NfVec3 first_start=primary->transform.position;
   const NfVec3 second_start=secondary->transform.position;
   const uint64_t source_tick=two_world->tick;
   /* H157 positive: independently source-owned A4 simulation for both. */
   NfRetcMa03PhysicalCandidate candidate={0};
   CHECK(nf_retc_ma03_stage_physics(two_world,two_owner,.125f,&candidate));
   CHECK(candidate.eligible&&candidate.solved.count==2u&&
         candidate.source.count==2u&&candidate.solved.impacts==1u);
   CHECK(candidate.solved.bodies[0].id==30u&&
         candidate.solved.bodies[1].id==40u);
   /* H158: an independent 1-D inelastic collision oracle agrees. */
   CHECK(candidate.reference.matched==1u&&
         candidate.reference.disagreed==0u&&
         candidate.reference.unresolved==0u);
   CHECK(candidate.solved.bodies[0].velocity.x<4.f&&
         candidate.solved.bodies[1].velocity.x>0.f);
   CHECK(candidate.solved.bodies[0].center.x>first_start.x&&
         candidate.solved.bodies[1].center.x>second_start.x);
   /* H159: an unchanged world rechecks the same deterministic solution. */
   CHECK(nf_retc_ma03_recheck_physics(two_world,two_owner,.125f,&candidate));
   /* H160: change the second real actor velocity and reject old result. */
   secondary->transform.velocity.x=1.f;
   CHECK(!nf_retc_ma03_recheck_physics(two_world,two_owner,.125f,&candidate));
   secondary->transform.velocity.x=0.f;
   CHECK(nf_retc_ma03_recheck_physics(two_world,two_owner,.125f,&candidate));
   /* H161: two real A4 candidate consequences are PRIVATE, not a tick. */
   CHECK(two_world->tick==source_tick&&two_owner->last_commit_tick==source_tick);
   CHECK(memcmp(&primary->transform.position,&first_start,sizeof first_start)==0);
   CHECK(memcmp(&secondary->transform.position,&second_start,sizeof second_start)==0);
   CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u);
   CHECK(push->receipt_count==2u);
   /* H168: connect TWO source-solved A4 bodies to one actual A5 source
      epoch without inventing a second A5 material history or B3 receipt. */
   const uint32_t a5_before_revision=two_owner->physical.world.revision;
   const uint32_t a5_before_epoch=two_owner->physical.world.grid.global_revision;
   const uint32_t a5_before_events=two_owner->physical.world.history.generated_events;
   const uint32_t a5_before_samples=two_owner->physical.world.history.raw_samples;
   NfRetcMa03A5Envelope a5_handoff={0};
   CHECK(nf_retc_ma03_a5_prepare(two_world,two_owner,.125f,
                                   &candidate,&a5_handoff));
   CHECK(a5_handoff.prepared&&a5_handoff.bodies[0].id==30u&&
         a5_handoff.bodies[1].id==40u);
   CHECK(nf_retc_ma03_a5_recheck(two_world,two_owner,.125f,
                                  &candidate,&a5_handoff));
   /* H169: material-world revision drift invalidates both A4 bodies'
      shared A5 handoff before any downstream realization. */
   ++two_owner->physical.world.revision;
   CHECK(!nf_retc_ma03_a5_recheck(two_world,two_owner,.125f,
                                   &candidate,&a5_handoff));
   two_owner->physical.world.revision=a5_before_revision;
   /* H170: no caller may substitute a forged second-actor consequence. */
   NfRetcMa03A5Envelope forged_handoff=a5_handoff;
   forged_handoff.bodies[1].velocity.x+=.5f;
   CHECK(!nf_retc_ma03_a5_recheck(two_world,two_owner,.125f,
                                   &candidate,&forged_handoff));
   /* H171: changed A5 material epoch or history provenance blocks replay. */
   ++two_owner->physical.world.grid.global_revision;
   CHECK(!nf_retc_ma03_a5_recheck(two_world,two_owner,.125f,
                                   &candidate,&a5_handoff));
   two_owner->physical.world.grid.global_revision=a5_before_epoch;
   ++two_owner->physical.world.history.generated_events;
   CHECK(!nf_retc_ma03_a5_recheck(two_world,two_owner,.125f,
                                   &candidate,&a5_handoff));
   two_owner->physical.world.history.generated_events=a5_before_events;
   /* H172: two-body/A5 handoff is private: no material history written,
      no A6 tick, actor projection, publisher or real B3 receipt changed. */
   CHECK(nf_retc_ma03_a5_recheck(two_world,two_owner,.125f,
                                  &candidate,&a5_handoff));
   CHECK(two_owner->physical.world.revision==a5_before_revision&&
         two_owner->physical.world.grid.global_revision==a5_before_epoch&&
         two_owner->physical.world.history.generated_events==a5_before_events&&
         two_owner->physical.world.history.raw_samples==a5_before_samples);
   CHECK(two_world->tick==source_tick);
   CHECK(memcmp(&primary->transform.position,&first_start,sizeof first_start)==0);
   CHECK(memcmp(&secondary->transform.position,&second_start,sizeof second_start)==0);
   CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u&&
         push->receipt_count==2u);
   /* H173: actual MA01 A4 collision receipt (not a fabricated event)
      drives a PRIVATE A5 history commit for both source actor IDs. */
   CHECK(candidate.solved.receipts[0].status==NF18A4_ACCEPTED);
   CHECK(candidate.solved.receipts[0].impulses_applied);
   NfRetcMa03A5ContactStage *a5_contact_stage=
       calloc(1,sizeof *a5_contact_stage);
   CHECK(a5_contact_stage);
   CHECK(nf_retc_ma03_a5_stage_contact(two_world,two_owner,.125f,
                     &candidate,&a5_handoff,a5_contact_stage));
   CHECK(a5_contact_stage->staged&&
         a5_contact_stage->contact.body_a==30u&&
         a5_contact_stage->contact.body_b==40u&&
         a5_contact_stage->contact.normal_impulse>0.f);
   CHECK(a5_contact_stage->candidate.history.raw_samples==a5_before_samples+1u);
   CHECK(a5_contact_stage->candidate.tick==two_world->tick+1u);
   /* H174: same source and receipt yields deterministic A5 history. */
   NfRetcMa03A5ContactStage *repeat_a5=calloc(1,sizeof *repeat_a5);
   CHECK(repeat_a5);
   CHECK(nf_retc_ma03_a5_stage_contact(two_world,two_owner,.125f,
                     &candidate,&a5_handoff,repeat_a5));
   CHECK(repeat_a5->staged&&
         repeat_a5->contact.contact_id==a5_contact_stage->contact.contact_id&&
         repeat_a5->contact.normal_impulse==a5_contact_stage->contact.normal_impulse&&
         nf18a5_history_hash(&repeat_a5->candidate.history)==
         nf18a5_history_hash(&a5_contact_stage->candidate.history));
   /* H175: changed source material revision cannot get a private commit. */
   ++two_owner->physical.world.revision;
   CHECK(!nf_retc_ma03_a5_stage_contact(two_world,two_owner,.125f,
                     &candidate,&a5_handoff,repeat_a5));
   two_owner->physical.world.revision=a5_before_revision;
   /* H176: a forged calculated second-body consequence is refused. */
   NfRetcMa03A5Envelope forged_material=a5_handoff;
   forged_material.bodies[1].velocity.x+=.25f;
   CHECK(!nf_retc_ma03_a5_stage_contact(two_world,two_owner,.125f,
                     &candidate,&forged_material,repeat_a5));
   /* H177: privately committed A5 history does not publish any live
      A5, A6, B2 or B3 effect; the single-A6 owner remains unchanged. */
   CHECK(two_owner->physical.world.revision==a5_before_revision&&
         two_owner->physical.world.history.raw_samples==a5_before_samples&&
         two_owner->physical.world.history.generated_events==a5_before_events);
   CHECK(two_world->tick==source_tick);
   CHECK(memcmp(&primary->transform.position,&first_start,sizeof first_start)==0);
   CHECK(memcmp(&secondary->transform.position,&second_start,sizeof second_start)==0);
   CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u&&
         push->receipt_count==2u);
   free(repeat_a5);
   free(a5_contact_stage);
   /* H162: moving source coactor produces a genuine two-body A4
      impulse, agrees with independent inelastic collision reference. */
   secondary->transform.velocity.x=1.f;
   NfRetcMa03PhysicalCandidate moving_target={0};
   CHECK(nf_retc_ma03_stage_physics(two_world,two_owner,.125f,&moving_target));
   CHECK(moving_target.eligible&&moving_target.reference.matched==1u&&
         moving_target.solved.impacts==1u);
   CHECK(fabsf(moving_target.solved.bodies[0].velocity.x-2.5f)<.003f);
   CHECK(fabsf(moving_target.solved.bodies[1].velocity.x-2.5f)<.003f);
   secondary->transform.velocity.x=0.f;
   /* H163: a distant source actor with no collision cannot fabricate
      an impulse receipt for the proposed pair. */
   secondary->transform.position.x=3.f;
   NfRetcMa03PhysicalCandidate no_contact={0};
   CHECK(!nf_retc_ma03_stage_physics(two_world,two_owner,.125f,&no_contact));
   CHECK(!no_contact.eligible);
   secondary->transform.position=second_start;
   /* H164: off-axis contact is outside the deliberately narrow
      independent 1-D reference domain and must remain unpromoted. */
   secondary->transform.position.z=.25f;
   NfRetcMa03PhysicalCandidate off_axis={0};
   CHECK(!nf_retc_ma03_stage_physics(two_world,two_owner,.125f,&off_axis));
   CHECK(!off_axis.eligible);
   secondary->transform.position=second_start;
   /* H165: a modified integration horizon cannot replay the old
      physical witness as though it were source-exact. */
   CHECK(!nf_retc_ma03_recheck_physics(two_world,two_owner,.09f,&candidate));
   CHECK(nf_retc_ma03_recheck_physics(two_world,two_owner,.125f,&candidate));
   /* H166: detach the actual owner binding in the staged view; no
      caller-supplied pointer is authority without the world boundary. */
   void *physics_tick_context=two_world->tick_owner_context;
   two_world->tick_owner_context=NULL;
   NfRetcMa03PhysicalCandidate detached={0};
   CHECK(!nf_retc_ma03_stage_physics(two_world,two_owner,.125f,&detached));
   CHECK(!detached.eligible);
   two_world->tick_owner_context=physics_tick_context;
   /* H167: the real A6 dispatcher still denies a nearby second actor,
      proving this research solver cannot bypass the production tick. */
   const uint64_t old_physical_tick=two_world->tick;
   CHECK(nf_world_step_checked(two_world,1.f/(float)NF_TICK_RATE)
         !=NF_WORLD_TICK_COMMITTED);
   CHECK(two_world->tick==old_physical_tick);
   CHECK(memcmp(&primary->transform.position,&first_start,
                sizeof first_start)==0);
   CHECK(memcmp(&secondary->transform.position,&second_start,
                sizeof second_start)==0);
   CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u&&
         push->receipt_count==2u);
   /* H183 positive static coverage: the entire pair of swept
      capsules spans four canonical chunks at y=0. Missing chunks are NOT
      implicitly free. Increase the isolated research grid capacity at
      initialization, then load the exact free source geometry. */
   const uint8_t free_cells[NF18A5_CANON_VOXELS]={0};
   for(int32_t cx=-1;cx<=0;++cx)
     for(int32_t cz=-1;cz<=0;++cz)
       CHECK(nf18a5_load_canonical(&two_owner->physical.world.grid,
                                    cx,0,cz,free_cells,1u));
   CHECK(two_owner->physical.world.grid.count==4u);
   CHECK(two_owner->physical.world.grid.global_revision>0u);
   /* H178: real A6 owner tick executes a two-body A4 contact and
      private A5 history via H83 diagnostic callback, then refuses commit. */
   NfRetcMa03TickEvidence *in_tick=calloc(1,sizeof *in_tick);
   CHECK(in_tick);
   NfH178OwnerTickProbe probe={.evidence=in_tick};
   two_owner->research_dual_stage=h178_owner_tick_probe;
   two_owner->research_dual_context=&probe;
   two_owner->require_dual_contact_preflight=true;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==1u&&probe.accepted==1u&&in_tick->staged);
   CHECK(in_tick->impacts==1u&&in_tick->sample.body_a==30u&&
         in_tick->sample.body_b==40u&&
         in_tick->material_candidate.history.raw_samples==a5_before_samples+1u);
   /* H179: repeat at the same accepted A6 source tick yields the same
      staged contact and history fingerprint, no published world tick. */
   const uint32_t in_tick_hash=nf18a5_history_hash(
       &in_tick->material_candidate.history);
   const float in_tick_impulse=in_tick->sample.normal_impulse;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==2u&&probe.accepted==2u&&in_tick->staged);
   CHECK(nf18a5_history_hash(&in_tick->material_candidate.history)==in_tick_hash);
   CHECK(in_tick->sample.normal_impulse==in_tick_impulse);
   /* H180: A6 A4 owner-source mismatch refuses private A5 staging. */
   const float old_bound_velocity=two_owner->physical.actor.velocity.x;
   two_owner->physical.actor.velocity.x+=.75f;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==3u&&probe.accepted==2u&&!in_tick->staged);
   two_owner->physical.actor.velocity.x=old_bound_velocity;
   /* H181: invalid A5 material epoch never authorizes a history sample. */
   two_owner->physical.world.grid.global_revision=0u;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==4u&&probe.accepted==2u&&!in_tick->staged);
   two_owner->physical.world.grid.global_revision=a5_before_epoch;
   /* H182: H83 cannot publish a partial two-actor transaction even with
      authentic A4 receipt and private A5 history successfully staged. */
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==5u&&probe.accepted==3u&&in_tick->staged);
   CHECK(two_world->tick==source_tick&&
         two_owner->physical.world.revision==a5_before_revision&&
         two_owner->physical.world.history.raw_samples==a5_before_samples);
   CHECK(memcmp(&primary->transform.position,&first_start,
                sizeof first_start)==0);
   CHECK(memcmp(&secondary->transform.position,&second_start,
                sizeof second_start)==0);
   CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u&&
         push->receipt_count==2u);
   /* H183: positive H178/H179/H182 owner-tick A4/A5 staging now
      requires BOTH swept actor routes to be canonically FREE. */
   CHECK(probe.accepted==3u&&in_tick->staged);
   /* H184: a missing authoritative chunk cannot be treated as FREE.
      The world source is deliberately modified then restored. */
   Nf18a5Chunk *center_chunk=NULL;
   for(size_t ci=0;ci<two_owner->physical.world.grid.count;++ci){
    Nf18a5Chunk *chunk=&two_owner->physical.world.grid.chunks[ci];
    if(chunk->x==0&&chunk->y==0&&chunk->z==0)
      center_chunk=chunk;
   }
   CHECK(center_chunk!=NULL);
   center_chunk->loaded=0u;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==6u&&probe.accepted==3u&&!in_tick->staged);
   center_chunk->loaded=1u;
   /* H185: a source SOLID voxel overlapping the swept capsule blocks. */
   const uint8_t previous_cell=center_chunk->canonical[0];
   center_chunk->canonical[0]=NF18A5_SOLID;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==7u&&probe.accepted==3u&&!in_tick->staged);
   /* H186: mixed canonical voxel without valid fine data is PENDING,
      never a positive static clearance claim. */
   center_chunk->canonical[0]=NF18A5_MIXED;
   center_chunk->fine_valid=0u;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==8u&&probe.accepted==3u&&!in_tick->staged);
   center_chunk->canonical[0]=previous_cell;
   /* H187: restored source clears and stages the SAME contact evidence;
      neither accepted world nor live A5/B3 may reflect any of the probes. */
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==9u&&probe.accepted==4u&&in_tick->staged);
   CHECK(in_tick->sample.body_a==30u&&in_tick->sample.body_b==40u);
   CHECK(in_tick->material_candidate.history.raw_samples==a5_before_samples+1u);
   CHECK(two_owner->physical.world.revision==a5_before_revision&&
         two_owner->physical.world.history.raw_samples==a5_before_samples&&
         two_world->tick==source_tick&&push->receipt_count==2u);
   CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u);
   /* H188: a genuinely loaded 0.5m fine grid can clear a MIXED
      canonical voxel when its single solid subvoxel lies beyond both
      swept capsule paths (z=0.5..1, both paths at z=0, radius 0.36). */
   const uint16_t fine_len=8u*8u*8u;
   uint8_t fine_clear[8u*8u*8u]={0};
   fine_clear[8u]=NF18A5_SOLID; /* y=0,z=1,x=0 */
   center_chunk->canonical[0]=NF18A5_MIXED;
   const uint32_t fine_parent=center_chunk->canonical_epoch;
   CHECK(nf18a5_load_fine(&two_owner->physical.world.grid,0,0,0,
                          2u,fine_clear,fine_len,fine_parent,1u));
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==10u&&probe.accepted==5u&&in_tick->staged);
   /* H189: stale fine-parent evidence must never grant static clearance. */
   center_chunk->fine_parent_epoch=fine_parent+1u;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==11u&&probe.accepted==5u&&!in_tick->staged);
   center_chunk->fine_parent_epoch=fine_parent;
   /* H190: fine-resolution SOLID directly in actor 30's swept hull is a
      real obstruction even though nearby exact subvoxels are FREE. */
   center_chunk->fine[0]=NF18A5_SOLID;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==12u&&probe.accepted==5u&&!in_tick->staged);
   center_chunk->fine[0]=NF18A5_FREE;
   /* H191: fine cache release cannot silently downgrade a MIXED cell
      to FREE under an unchanged canonical material claim. */
   CHECK(nf18a5_release_fine(&two_owner->physical.world.grid,0,0,0));
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==13u&&probe.accepted==5u&&!in_tick->staged);
   /* H192: reloading exact fine source restores staging with identical
      collision receipt; all live A6/A5/B3 effects remain unpublished. */
   CHECK(nf18a5_load_fine(&two_owner->physical.world.grid,0,0,0,
                          2u,fine_clear,fine_len,fine_parent,1u));
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==14u&&probe.accepted==6u&&in_tick->staged);
   CHECK(in_tick->sample.normal_impulse==in_tick_impulse&&
         nf18a5_history_hash(&in_tick->material_candidate.history)==in_tick_hash);
   CHECK(two_world->tick==source_tick&&
         two_owner->physical.world.revision==a5_before_revision&&
         two_owner->physical.world.history.raw_samples==a5_before_samples&&
         push->receipt_count==2u&&nf_retc_read(publisher)==accepted);
   center_chunk->canonical[0]=previous_cell;
   /* H193: the A6 in-tick owner supplies two REAL A4 material impulses
      that are equal/opposite and tied to the authentic A5 sample. */
   CHECK(in_tick->staged&&in_tick->impacts==1u);
   CHECK(in_tick->bodies[0].id==30u&&in_tick->bodies[1].id==40u);
   CHECK(in_tick->material_impulse[0].x < -1.f);
   CHECK(in_tick->material_impulse[1].x > 1.f);
   CHECK(fabsf(in_tick->material_impulse[0].x+
                in_tick->material_impulse[1].x)<.02f);
   /* H194: independently revalidate two actor velocity changes against
      the source-applied A4 contact receipt and private A5 history. */
   CHECK(nf_retc_ma03_reciprocal_effects_valid(in_tick));
   /* H195: a forged second actor impulse cannot cross M2's boundary. */
   in_tick->material_impulse[1].x+=1.f;
   CHECK(!nf_retc_ma03_reciprocal_effects_valid(in_tick));
   in_tick->material_impulse[1].x-=1.f;
   /* H196: a foreign A4 receipt identity is not an A5 material effect. */
   ++in_tick->applied_contact.contact_id;
   CHECK(!nf_retc_ma03_reciprocal_effects_valid(in_tick));
   --in_tick->applied_contact.contact_id;
   CHECK(nf_retc_ma03_reciprocal_effects_valid(in_tick));
   /* H197: repeat the true A6 owner tick with the same source and verify
      both reciprocal effects stay private, deterministic and unpublishable. */
   const float first_actor_impulse=in_tick->material_impulse[0].x;
   const float second_actor_impulse=in_tick->material_impulse[1].x;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==15u&&probe.accepted==7u&&in_tick->staged);
   CHECK(nf_retc_ma03_reciprocal_effects_valid(in_tick));
   CHECK(in_tick->material_impulse[0].x==first_actor_impulse&&
         in_tick->material_impulse[1].x==second_actor_impulse);
   CHECK(two_world->tick==source_tick&&
         two_owner->physical.world.revision==a5_before_revision&&
         two_owner->physical.world.history.raw_samples==a5_before_samples&&
         push->receipt_count==2u&&nf_retc_read(publisher)==accepted);
   two_owner->require_dual_contact_preflight=false;
   two_owner->research_dual_stage=NULL;
   two_owner->research_dual_context=NULL;
   free(in_tick);
   CHECK(nf18a6_unbind_world_tick(two_world,two_owner));
   free(two_owner);
   free(two_world);
  }
  /* H99: actual B2 source witnesses must resolve consistently in both
     input orders, without changing the accepted A6 frame or world tick. */
  const Nf18b2QueuedIntent *first_q=&enc->queue[old_count-2u];
  CHECK(first_q->intent.actor_id==10u);
  Nf18b2Use requests[2]={first_q->witness,q->witness};
  Nf18b2UseDecision forward[2]={{0}},reverse[2]={{0}};
  CHECK(nf18b2_resolve(&enc->frame.scene,requests,2u,
                       NF18B2_M4_MATERIAL_FIRST,forward));
  CHECK(forward[0].status==NF18B2_ELIGIBLE);
  CHECK(forward[1].status==NF18B2_ELIGIBLE);
  Nf18b2Use reversed[2]={requests[1],requests[0]};
  CHECK(nf18b2_resolve(&enc->frame.scene,reversed,2u,
                       NF18B2_M4_MATERIAL_FIRST,reverse));
  CHECK(reverse[0].status==forward[1].status);
  CHECK(reverse[1].status==forward[0].status);
  /* H100: competing actions from one actor may not consume two capacity
     slots; sorting by source actor/action makes reversed input equivalent. */
  Nf18b2Use second_action=q->witness;
  second_action.action_id=604u;
  Nf18b2Use duplicated[3]={requests[0],requests[1],second_action};
  Nf18b2UseDecision resolved[3]={{0}};
  CHECK(nf18b2_resolve(&enc->frame.scene,duplicated,3u,
                       NF18B2_M4_MATERIAL_FIRST,resolved));
  CHECK(resolved[0].status==NF18B2_ELIGIBLE);
  CHECK(resolved[1].status==NF18B2_ELIGIBLE);
  CHECK(resolved[2].status==NF18B2_DUPLICATE);
  CHECK(resolved[2].reason==NF18B2_REPLAY);
  Nf18b2Use duplicated_reverse[3]={second_action,requests[1],requests[0]};
  Nf18b2UseDecision reverse_resolved[3]={{0}};
  CHECK(nf18b2_resolve(&enc->frame.scene,duplicated_reverse,3u,
                       NF18B2_M4_MATERIAL_FIRST,reverse_resolved));
  CHECK(reverse_resolved[0].status==resolved[2].status);
  CHECK(reverse_resolved[1].status==resolved[1].status);
  CHECK(reverse_resolved[2].status==resolved[0].status);
  /* H101: capacity-one boundary on copied source snapshot; actor 10
     retains its existing slot, new actor 20 is refused in either order. */
  Nf18b2Object capacity_one=enc->frame.objects[0];
  capacity_one.capacity=1u;
  Nf18b2Scene limited_scene=enc->frame.scene;
  limited_scene.objects=&capacity_one;
  Nf18b2UseDecision limited[2]={{0}},limited_rev[2]={{0}};
  CHECK(nf18b2_resolve(&limited_scene,requests,2u,
                       NF18B2_M4_MATERIAL_FIRST,limited));
  CHECK(limited[0].status==NF18B2_ELIGIBLE);
  CHECK(limited[1].status==NF18B2_BLOCKED);
  CHECK(limited[1].reason==NF18B2_CAPACITY);
  CHECK(nf18b2_resolve(&limited_scene,reversed,2u,
                       NF18B2_M4_MATERIAL_FIRST,limited_rev));
  CHECK(limited_rev[0].status==limited[1].status);
  CHECK(limited_rev[0].reason==NF18B2_CAPACITY);
  CHECK(limited_rev[1].status==limited[0].status);
  /* H102: source actor revision mismatch must reject, never reissue
     eligibility from an old witness. */
  Nf18b2Use stale=q->witness;
  ++stale.actor_revision;
  const Nf18b2UseDecision stale_result=
       nf18b2_evaluate(&enc->frame.scene,stale,NF18B2_M4_MATERIAL_FIRST);
  CHECK(stale_result.status==NF18B2_STALE);
  CHECK(stale_result.reason==NF18B2_FRAME_CHANGED);
  /* H104: actual B2 witness with stale object revision cannot be
     promoted to eligibility after the object changes. */
  Nf18b2Use stale_object=q->witness;
  ++stale_object.object_revision;
  const Nf18b2UseDecision stale_object_result=
      nf18b2_evaluate(&enc->frame.scene,stale_object,NF18B2_M4_MATERIAL_FIRST);
  CHECK(stale_object_result.status==NF18B2_STALE);
  CHECK(stale_object_result.reason==NF18B2_FRAME_CHANGED);
  /* H105: material epoch drift invalidates a previously source-issued
     witness; source-owned eligibility is not replayable across revisions. */
  Nf18b2Use stale_material=q->witness;
  ++stale_material.material_epoch;
  const Nf18b2UseDecision stale_material_result=
      nf18b2_evaluate(&enc->frame.scene,stale_material,NF18B2_M4_MATERIAL_FIRST);
  CHECK(stale_material_result.status==NF18B2_STALE);
  CHECK(stale_material_result.reason==NF18B2_FRAME_CHANGED);
  /* H106: real B2 witness bound to an older world revision must refuse. */
  Nf18b2Use stale_world=q->witness;
  ++stale_world.world_revision;
  const Nf18b2UseDecision world_result=
      nf18b2_evaluate(&enc->frame.scene,stale_world,NF18B2_M4_MATERIAL_FIRST);
  CHECK(world_result.status==NF18B2_STALE);
  CHECK(world_result.reason==NF18B2_FRAME_CHANGED);
  /* H107: an altered tick cannot inherit a previous source-issued grant. */
  Nf18b2Use stale_tick=q->witness;
  ++stale_tick.tick;
  const Nf18b2UseDecision tick_result=
      nf18b2_evaluate(&enc->frame.scene,stale_tick,NF18B2_M4_MATERIAL_FIRST);
  CHECK(tick_result.status==NF18B2_STALE);
  CHECK(tick_result.reason==NF18B2_FRAME_CHANGED);
  /* H108: a byte-identical source witness submitted twice is not two
     independent authorizations, even when object capacity is two. */
  Nf18b2Use replay_uses[2]={q->witness,q->witness};
  Nf18b2UseDecision replay_decisions[2]={{0}};
  CHECK(nf18b2_resolve(&enc->frame.scene,replay_uses,2u,
                       NF18B2_M4_MATERIAL_FIRST,replay_decisions));
  CHECK(replay_decisions[0].status==NF18B2_ELIGIBLE);
  CHECK(replay_decisions[1].status==NF18B2_DUPLICATE);
  CHECK(replay_decisions[1].reason==NF18B2_REPLAY);
  /* H109: same actor/action routed to two different objects is invalid,
     never a claim for either resource. The second target is intentionally
     absent from this authoritative registry. */
  Nf18b2Use conflicting[2]={q->witness,q->witness};
  conflicting[1].object_id=2011u;
  Nf18b2UseDecision conflict_decisions[2]={{0}};
  CHECK(nf18b2_resolve(&enc->frame.scene,conflicting,2u,
                       NF18B2_M4_MATERIAL_FIRST,conflict_decisions));
  CHECK(conflict_decisions[0].status==NF18B2_INVALID);
  CHECK(conflict_decisions[1].status==NF18B2_INVALID);
  CHECK(conflict_decisions[0].reason==NF18B2_BAD_INPUT);
  CHECK(conflict_decisions[1].reason==NF18B2_BAD_INPUT);
  /* H110: unsupported adjudicator model fails closed and none of these
     research probes can advance the live A6 publication pointer or tick. */
  const Nf18b2UseDecision invalid_model=
      nf18b2_evaluate(&enc->frame.scene,q->witness,(Nf18b2Model)99);
  CHECK(invalid_model.status==NF18B2_INVALID);
  CHECK(invalid_model.reason==NF18B2_BAD_INPUT);
  CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u);
  /* H103: repeated arbitration is read-only: cannot alter registry,
     accepted pointer, tick, or occupancy. */
  const Nf18b2Registry registry_before=enc->registry;
  const Nf18b2Object object_before=enc->frame.objects[0];
  for(unsigned repeat=0;repeat<8u;++repeat){
    Nf18b2UseDecision repeat_result[3]={{0}};
    CHECK(nf18b2_resolve(&enc->frame.scene,duplicated,3u,
                         NF18B2_M4_MATERIAL_FIRST,repeat_result));
    CHECK(repeat_result[0].status==resolved[0].status);
    CHECK(repeat_result[1].status==resolved[1].status);
    CHECK(repeat_result[2].status==resolved[2].status);
  }
  CHECK(memcmp(&enc->registry,&registry_before,sizeof registry_before)==0);
  CHECK(memcmp(&enc->frame.objects[0],&object_before,sizeof object_before)==0);
  CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u);
  CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u);
  CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u);
  second_actor->transform.position=old_pos;
 }
 CHECK(nf_retc_joint_detach(w,&joint));
 CHECK(nf18a6_unbind_world_tick(w,r));
 nf_retc_dispose(publisher);
 CHECK(remove(journal)==0);
 free(publisher);free(push);free(enc);free(r);free(w);
 puts("RET-C real A6 dispatcher two consecutive committed joint ticks PASS");
 return 0;
}
