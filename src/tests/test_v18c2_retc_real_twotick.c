#include "nf_retc_joint_provider.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"RET-C real dispatcher failure line %d: %s\\n",__LINE__,#x);return 1;}}while(0)
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
 const NfRetcFrame *f=nf_retc_read(publisher);
 const NfActor *a=nf_world_find_actor_const(w,10);
 if(!f||!a||f->epoch!=expected||w->tick!=expected||
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
 NfActor *actor=nf_world_find_actor(w,10);CHECK(actor);
 actor->movement.grounded=true;actor->movement.mode=NF_MOVE_GROUND;
 Nf18a4Body crate={0};crate.id=2010;crate.center=(NfVec3){1.55f,.9f,1};
 crate.inverse_mass=1.f/24.f;crate.box_half=(NfVec3){.2f,.5f,.35f};
 CHECK(nf18a6_init(r,w,10,crate,3,NF18A6_WORLD_BRIDGE));
 uint8_t empty[NF18A5_CANON_VOXELS]={0};
 CHECK(nf18a5_load_canonical(&r->physical.world.grid,0,0,0,empty,1));
 CHECK(nf18b2_encounter_init(enc,w,r,NF18B2_M4_MATERIAL_FIRST));
 CHECK(nf18b2_register_actor(enc,10,0,0,50));
 Nf18b2Object object={0};
 object.id=2010;object.revision=1;object.cell_id=1;
 object.nexus_id=1;object.slot_id=1;object.capacity=1;
 object.target=crate.center;object.max_reach=3.f;
 CHECK(nf18b2_register_object(enc,object,r->physical.world.grid.global_revision,0,0));
 const char *journal="/tmp/nightfall_retc_real_twotick.bin";
 remove(journal);
 CHECK(nf18a6_retc_bind(r,publisher,journal));
 CHECK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 push->runtime=r;push->contract_epoch=enc->registry.contract_epoch;
 CHECK(nf18b6_push_require_b2(push,b2_provider,enc,NF18B2_M4_MATERIAL_FIRST));
 NfRetcJointContext joint={0};joint.owner=r;joint.encounter=enc;
 joint.push=push;joint.transaction_id=1;
 CHECK(nf_retc_joint_attach(w,&joint));
 for(uint32_t tick=1;tick<=2;++tick){
   const uint32_t action=90u+tick;
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
       "RET-C B2 body probe: radius=%.3f height=%.3f geometry_clear=%d material_query=%d solids=%zu material_epoch=%u\\n",
       sample->radius,sample->height,(int)geometry_clear,
       (int)material_body,enc->frame.scene.geometry.solid_count,
       r->physical.world.grid.global_revision);
     fprintf(stderr,
       "RET-C B2 proposal tick=%u status=%d reason=%d physical=%d contract=%d queue=%zu traces=%zu actor=(%.3f,%.3f,%.3f) target=(%.3f,%.3f,%.3f)\\n",
       tick,(int)proposed,trace?(int)trace->authorization.reason:-1,
       trace?(int)trace->authorization.physical:-1,
       trace?(int)trace->authorization.contract:-1,
       enc->queue_count,enc->trace_count,
       actor->transform.position.x,actor->transform.position.y,
       actor->transform.position.z,object.target.x,object.target.y,object.target.z);
     return 1;
   }
   CHECK(nf18b6_push_arm_b2(push,w,action)==NF18B3_OK);
   NfMoveInput move={0};move.strafe=1;
   nf_world_set_input(w,10,move);
   const NfWorldTickStatus status=nf_world_step_checked(w,1.f/(float)NF_TICK_RATE);
   if(status!=NF_WORLD_TICK_COMMITTED){
      fprintf(stderr,"real dispatcher tick %u status=%d lastA6=%d B2=%d B3=%d\\n",
       tick,(int)status,(int)r->tick_trace.embodiment_status,
       (int)enc->traces[enc->trace_count-1].revalidation.status,(int)push->last_result);
      return 1;
   }
   CHECK(verify(w,r,enc,push,publisher,tick));
 }
 CHECK(nf_retc_joint_detach(w,&joint));
 CHECK(nf18a6_unbind_world_tick(w,r));
 nf_retc_dispose(publisher);
 CHECK(remove(journal)==0);
 free(publisher);free(push);free(enc);free(r);free(w);
 puts("RET-C real A6 dispatcher two consecutive committed joint ticks PASS");
 return 0;
}
