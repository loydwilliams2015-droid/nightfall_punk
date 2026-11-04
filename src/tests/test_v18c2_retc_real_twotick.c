#include "nf_retc_joint_provider.h"
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
 CHECK(nf_retc_joint_attach(w,&joint));
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
   NfMoveInput move={0};move.strafe=1;
   nf_world_set_input(w,10,move);
   const NfWorldTickStatus status=nf_world_step_checked(w,1.f/(float)NF_TICK_RATE);
   if(status!=NF_WORLD_TICK_COMMITTED){
      fprintf(stderr,"real dispatcher tick %u status=%d lastA6=%d B2=%d B3=%d\
",
       tick,(int)status,(int)r->tick_trace.embodiment_status,
       (int)enc->traces[enc->trace_count-1].revalidation.status,(int)push->last_result);
      return 1;
   }
   CHECK(verify(w,r,enc,push,publisher,tick));
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
