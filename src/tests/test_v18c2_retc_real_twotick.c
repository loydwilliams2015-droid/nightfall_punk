#define _POSIX_C_SOURCE 200809L
#include "nf_retc_joint_provider.h"
#include "nf_retc_m2_real_contract_audit.h"
#include "nf_retc_multi_plan.h"
#include "nf_retc_multi_preflight.h"
#include "nf_retc_multi_physical.h"
#include "nf_retc_multi_source_seal.h"
#include "nf_retc_ma03_physics_stage.h"
#include "nf_retc_ma03_a5_envelope.h"
#include "nf_retc_ma03_a5_contact_stage.h"
#include "nf_retc_ma03_a5_sweep_pair.h"
#include "nf_retc_h307_cross_source_binding.h"
#include "nf_retc_h310_owner_joint_probe.h"
#include "nf_retc_ma03_owner_tick.h"
#include "nf_retc_external.h"
#include "nf_retc_consumer_source.h"
#include "nf_retc_map_projection.h"
#include "nf_retc_map_material.h"
#include "nf_retc_map_sweep.h"
#include "nf_retc_map_clearance.h"
#include "nf_retc_network.h"
#include "nf_multi_ma03.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
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
/* H296: verify real A6 leased frame provenance for three potential
 * consumers, not merely synthetic witness validation. The same accepted
 * A6/B2/B3 pointer must survive 50 forged read-view projections. */
static bool h296_leased_source_check(const NfRetcPublisher *pub,
                                    uint32_t expected){
 NfRetcReadLease lease={0};
 if(!nf_retc_read_lease_acquire(pub,&lease))return false;
 bool ok=false;
 do{
  NfRetcReadView v={0};
  NfRetcSourceObservation observed={0};
  if(!nf_retc_read_leased_view(&lease,&v)||
     !nf_retc_consumer_source_validate(&lease,&v,&observed)||
     observed.tick!=expected||
     observed.transaction_id!=v.transaction_id||
     observed.owner_actor!=v.accepted->owner_actor||
     observed.contract_epoch!=v.event->contract_epoch)
    break;
  bool cases_ok=true;
  for(unsigned h296_case=0;h296_case<50u;++h296_case){
   NfRetcReadView changed=v;
   NfRetcSourceObservation sentinel={
    0xaaaaaaaaU,0xbbbbbbbbU,0xccccccccU,0xddddddddU};
   const unsigned magnitude=h296_case%5u+1u;
   switch(h296_case/5u){
    case 0:changed.accepted=NULL;break;
    case 1:changed.physical=NULL;break;
    case 2:changed.event=NULL;break;
    case 3:changed.contract=NULL;break;
    case 4:changed.eligibility=NULL;break;
    case 5:changed.epoch+=magnitude;break;
    case 6:changed.transaction_id+=magnitude;break;
    case 7:changed.physical=(const Nf18a5Integrated *)(const void *)&v.accepted->b2_use;break;
    case 8:changed.event=(const Nf18b3Event *)(const void *)&v.accepted->b2_use;break;
    case 9:changed.contract=(const Nf18b2Registry *)(const void *)&v.accepted->event;break;
   }
   if(nf_retc_consumer_source_validate(&lease,&changed,&sentinel)||
      sentinel.tick!=0xaaaaaaaaU||
      sentinel.transaction_id!=0xbbbbbbbbU||
      sentinel.owner_actor!=0xccccccccU||
      sentinel.contract_epoch!=0xddddddddU){
     cases_ok=false;break;
   }
   fprintf(stderr,"H296 LEASED_SOURCE_REJECT case=%u real_epoch=%u unchanged=1\n",
           h296_case,expected);
  }
  if(!cases_ok)break;
  if(!nf_retc_consumer_source_validate(&lease,&v,&observed))break;
  fprintf(stderr,"H296 REAL_A6_LEASED_SOURCE PASS 50 denials and positive recheck\n");
  ok=true;
 }while(0);
 nf_retc_read_lease_release(&lease);
 return ok;
}
/* H297: accepted A6/B2/B3 source drives a real read-only actor/crate
 * map projection. Each denied source mutation must leave both the
 * consumer projection and the consumer cursor bit-for-bit unchanged. */
static bool h297_real_map_check(const NfRetcPublisher *pub,uint32_t expected){
 NfRetcReadLease lease={0};
 if(!nf_retc_read_lease_acquire(pub,&lease))return false;
 bool ok=false;
 do{
  NfRetcReadView v={0};
  if(!nf_retc_read_leased_view(&lease,&v))break;
  const NfRetcFrame *f=v.accepted;
  if(!f||f->epoch!=expected)break;
  NfRetcMapCursor cursor={0};
  NfRetcMapProjection accepted={0};
  if(!nf_retc_map_project_lease(&lease,&v,f->epoch,f->transaction_id,
       f->owner_actor,f->object_id,f->event.contract_epoch,
       &cursor,&accepted))break;
  const Nf18a4Body *a=&f->physical.actor,*b=&f->physical.object;
  if(accepted.actor_center.x!=a->center.x||
     accepted.actor_center.y!=a->center.y||
     accepted.actor_center.z!=a->center.z||
     accepted.object_min.x!=b->center.x-b->box_half.x||
     accepted.object_max.z!=b->center.z+b->box_half.z||
     accepted.tick!=expected)break;
  bool valid=true;
  for(unsigned h297_case=0;h297_case<50u;++h297_case){
   const unsigned kind=h297_case/5u;
   const uint32_t n=h297_case%5u+1u;
   NfRetcReadView forged=v;
   uint32_t tick=f->epoch,tx=f->transaction_id,
     owner=f->owner_actor,obj=f->object_id,
     contract=f->event.contract_epoch;
   NfRetcMapCursor denied_cursor={0};
   NfRetcMapProjection denied_map=accepted;
   switch(kind){
    case 0:tick+=n;break;
    case 1:tx+=n;break;
    case 2:owner+=n;break;
    case 3:obj+=n;break;
    case 4:contract+=n;break;
    case 5:denied_cursor.tick=f->epoch+n;break;
    case 6:denied_cursor.transaction_id=f->transaction_id+n;break;
    case 7:forged.accepted=NULL;break;
    case 8:forged.physical=NULL;break;
    case 9:forged.event=NULL;break;
   }
   const NfRetcMapCursor old_cursor=denied_cursor;
   if(nf_retc_map_project_lease(&lease,&forged,tick,tx,owner,obj,contract,
        &denied_cursor,&denied_map)||
      memcmp(&old_cursor,&denied_cursor,sizeof old_cursor)!=0||
      memcmp(&accepted,&denied_map,sizeof accepted)!=0){
     valid=false;break;
   }
   fprintf(stderr,"H297 REAL_A6_MAP_DENIAL case=%u epoch=%u unchanged=1\n",
           h297_case,expected);
  }
  if(!valid)break;
  NfRetcMapProjection duplicate=accepted;
  if(nf_retc_map_project_lease(&lease,&v,f->epoch,f->transaction_id,
       f->owner_actor,f->object_id,f->event.contract_epoch,
       &cursor,&duplicate)||
     memcmp(&accepted,&duplicate,sizeof accepted)!=0)break;
  fprintf(stderr,"H297 REAL_A6_MAP PASS 50 denials, original bounds and replay denial\n");
  ok=true;
 }while(0);
 nf_retc_read_lease_release(&lease);
 return ok;
}
/* H298: scan the material grid attached to the actual accepted A6
 * lease; mutation tests use private copies, NEVER change accepted source. */
static bool h298_real_material_check(const NfRetcPublisher *pub,uint32_t expected){
 NfRetcReadLease lease={0};
 if(!nf_retc_read_lease_acquire(pub,&lease))return false;
 bool ok=false;
 do{
  NfRetcReadView view={0};
  NfRetcMaterialView actual={0};
  if(!nf_retc_read_leased_view(&lease,&view)||
     !nf_retc_map_material_from_lease(&lease,&view,expected,&actual)||
     actual.tick!=expected||!actual.material_epoch||
     actual.full_world_clearance)break;
  const Nf18a5Grid *source=&view.accepted->physical.world.grid;
  if(!source->count)break;
  Nf18a5Grid *private_copy=malloc(sizeof *private_copy);
  if(!private_copy)break;
  bool passed=true;
  for(unsigned h298_case=0;h298_case<50u;++h298_case){
   memcpy(private_copy,source,sizeof *private_copy);
   Nf18a5Chunk *chunk=&private_copy->chunks[0];
   const unsigned n=h298_case%5u+1u;
   switch(h298_case/5u){
    case 0:private_copy->global_revision=0u;break;
    case 1:private_copy->capacity=0u;break;
    case 2:private_copy->count=(uint8_t)(NF18A5_MAX_CHUNKS+n);break;
    case 3:private_copy->policy=255u;break;
    case 4:chunk->loaded=0u;break;
    case 5:chunk->canonical_epoch=0u;break;
    case 6:chunk->canonical[0]=(uint8_t)(NF18A5_MIXED+n);break;
    case 7:chunk->fine_valid=1u;chunk->fine_parent_epoch=0u;break;
    case 8:chunk->fine_valid=1u;chunk->fine_resolution=0u;
           chunk->fine_parent_epoch=chunk->canonical_epoch;break;
    case 9:chunk->fine_count=(uint16_t)(NF18A5_FINE_MAX_VOXELS+n);break;
   }
   NfRetcMaterialView result=actual;
   if(nf_retc_map_material_scan(private_copy,expected,&result)||
      memcmp(&actual,&result,sizeof actual)!=0){passed=false;break;}
   fprintf(stderr,"H298 A5_RESIDENT_DENIAL case=%u epoch=%u preserved=1\n",
           h298_case,expected);
  }
  if(passed){
   memcpy(private_copy,source,sizeof *private_copy);
   Nf18a5Chunk *chunk=&private_copy->chunks[0];
   chunk->canonical[0]=NF18A5_MIXED;
   chunk->fine_valid=0u;
   NfRetcMaterialView pending={0};
   if(!nf_retc_map_material_scan(private_copy,expected,&pending)||
      pending.pending_cells==0u||pending.full_world_clearance)
     passed=false;
  }
  free(private_copy);
  if(!passed)break;
  fprintf(stderr,"H298 REAL_A6_MATERIAL PASS 50 denials and MIXED pending\n");
  ok=true;
 }while(0);
 nf_retc_read_lease_release(&lease);
 return ok;
}
/* H299: actual accepted A6 material grid -> private A5 swept query.
 * 50 hostile source/geometry denial controls preserve the last witness. */
static bool h299_real_swept_map_check(const NfRetcPublisher *pub,
                                     uint32_t expected){
 NfRetcReadLease lease={0};
 if(!nf_retc_read_lease_acquire(pub,&lease))return false;
 bool ok=false;
 do{
  NfRetcReadView v={0};
  if(!nf_retc_read_leased_view(&lease,&v)||!v.accepted)break;
  const NfRetcFrame *f=v.accepted;
  NfVec3 feet={f->physical.actor.center.x,
               f->physical.actor.center.y-f->physical.actor.height*.5f,
               f->physical.actor.center.z};
  NfVec3 zero={0};
  Nf18a2ShapePolicy shape={.radius=f->physical.actor.radius,
     .height=f->physical.actor.height,.foot_flat_radius=.05f,
     .step_height=.4f,.walkable_normal_y=.65f};
  NfRetcSweptMapWitness baseline={0};
  if(!nf_retc_map_sweep_from_lease(&lease,&v,f->epoch,f->transaction_id,
       f->owner_actor,f->object_id,f->event.contract_epoch,feet,zero,
       shape,.016f,&baseline)||
     baseline.tick!=expected||
     baseline.material_epoch!=f->physical.world.grid.global_revision)
    break;
  bool passed=true;
  for(unsigned case_id=0;case_id<50u;++case_id){
   const uint32_t n=case_id%5u+1u;
   uint32_t tick=f->epoch,tx=f->transaction_id,
     owner=f->owner_actor,obj=f->object_id,
     contract=f->event.contract_epoch;
   NfVec3 p=feet,d=zero;
   Nf18a2ShapePolicy sh=shape;
   float dt=.016f;
   switch(case_id/5u){
    case 0:tick+=n;break;
    case 1:tx+=n;break;
    case 2:owner+=n;break;
    case 3:obj+=n;break;
    case 4:contract+=n;break;
    case 5:dt=-(float)n;break;
    case 6:sh.radius=-(float)n;break;
    case 7:sh.height=-(float)n;break;
    case 8:p.x=NAN;break;
    case 9:d.z=NAN;break;
   }
   NfRetcSweptMapWitness untouched=baseline;
   if(nf_retc_map_sweep_from_lease(&lease,&v,tick,tx,owner,obj,
        contract,p,d,sh,dt,&untouched)||
      memcmp(&untouched,&baseline,sizeof baseline)!=0){
     passed=false;break;
   }
   fprintf(stderr,"H299 REAL_A6_SWEEP_DENIAL case=%u epoch=%u unchanged=1\n",
           case_id,expected);
  }
  if(!passed)break;
  fprintf(stderr,"H299 REAL_A6_SWEEP PASS 50 denial cases actual A5 result=%u\n",
          (unsigned)baseline.result);
  ok=true;
 }while(0);
 nf_retc_read_lease_release(&lease);
 return ok;
}
/* H300: real A6 accepted A5 terrain must certify FREE, not merely
 * return a well-formed diagnostic. Fifty edge cases outside resident
 * A5 chunks must not turn PENDING into an authorized positive result;
 * 50 bounded positive controls remain within known canonical FREE. */
static bool h300_real_clearance_check(const NfRetcPublisher *pub,
                                      uint32_t expected){
 NfRetcReadLease lease={0};
 if(!nf_retc_read_lease_acquire(pub,&lease))return false;
 bool ok=false;
 do{
  NfRetcReadView view={0};
  if(!nf_retc_read_leased_view(&lease,&view)||!view.accepted)break;
  const NfRetcFrame *f=view.accepted;
  const Nf18a2ShapePolicy shape={.radius=.1f,.height=.5f,
     .foot_flat_radius=.05f,.step_height=.15f,.walkable_normal_y=.65f};
  const NfVec3 known_free={1.5f,.5f,1.5f};
  bool pass=true;
  for(unsigned case_id=0;case_id<50u;++case_id){
   const uint32_t n=case_id%10u+1u;
   NfVec3 p=known_free,d={0};
   switch(case_id/10u){
    case 0:p.x=100.f+(float)n;break;
    case 1:p.y=100.f+(float)n;break;
    case 2:p.z=100.f+(float)n;break;
    case 3:p.x=-100.f-(float)n;break;
    case 4:p.z=-100.f-(float)n;break;
   }
   NfRetcSweptMapWitness diagnostic={0};
   NfRetcSweptMapWitness unchanged={
        0xaaaaU,0xbbbbU,0xccccU,NF18A5_Q_SOLID};
   const NfRetcSweptMapWitness sentinel=unchanged;
   if(!nf_retc_map_sweep_from_lease(&lease,&view,
          f->epoch,f->transaction_id,f->owner_actor,f->object_id,
          f->event.contract_epoch,p,d,shape,.016f,&diagnostic)||
      diagnostic.result!=NF18A5_Q_PENDING||
      nf_retc_map_confirm_free(&lease,&view,
          f->epoch,f->transaction_id,f->owner_actor,f->object_id,
          f->event.contract_epoch,p,d,shape,.016f,&unchanged)||
      memcmp(&unchanged,&sentinel,sizeof sentinel)!=0){
     pass=false;break;
   }
   fprintf(stderr,"H300 REAL_A6_PENDING_DENIED case=%u epoch=%u preserved=1\n",
           case_id,expected);
  }
  if(!pass)break;
  for(unsigned case_id=0;case_id<50u;++case_id){
   NfVec3 d={.005f*(float)(case_id%10u),0,0};
   NfRetcSweptMapWitness accepted={0};
   if(!nf_retc_map_confirm_free(&lease,&view,
       f->epoch,f->transaction_id,f->owner_actor,f->object_id,
       f->event.contract_epoch,known_free,d,shape,.016f,&accepted)||
       accepted.result!=NF18A5_Q_FREE||
       accepted.tick!=expected){
      pass=false;break;
   }
   fprintf(stderr,"H300 REAL_A6_FREE_CONTROL case=%u epoch=%u accepted=1\n",
           case_id,expected);
  }
  if(!pass)break;
  fprintf(stderr,"H300 REAL_A6_CLEARANCE PASS 50 pending denials + 50 free controls\n");
  ok=true;
 }while(0);
 nf_retc_read_lease_release(&lease);
 return ok;
}
/* H301 differential source-oracle sweep: 40 spatial trajectories on the
 * accepted A6 material frame, compared against A5 directly on a private
 * grid. Neither the trusted publication nor its cache is modified.
 * This establishes semantic parity for a bounded resident/non-resident
 * scene, not full world topology or production B2 actor arbitration. */
static bool h301_real_sweep_differential(const NfRetcPublisher *pub,
                                         uint32_t expected){
 NfRetcReadLease lease={0};
 if(!nf_retc_read_lease_acquire(pub,&lease))return false;
 bool ok=false;
 do{
  NfRetcReadView v={0};
  if(!nf_retc_read_leased_view(&lease,&v)||!v.accepted)break;
  const NfRetcFrame *f=v.accepted;
  const Nf18a5Grid *source=&f->physical.world.grid;
  Nf18a5Grid *oracle_grid=malloc(sizeof *oracle_grid);
  Nf18a5Grid *source_snapshot=malloc(sizeof *source_snapshot);
  if(!oracle_grid||!source_snapshot){
   free(oracle_grid);free(source_snapshot);break;
  }
  memcpy(source_snapshot,source,sizeof *source_snapshot);
  bool passed=true;
  const Nf18a2ShapePolicy shape={
   .radius=.1f,.height=.5f,.foot_flat_radius=.05f,
   .step_height=.15f,.walkable_normal_y=.65f};
  for(unsigned case_id=0;case_id<40u;++case_id){
   const unsigned j=case_id%10u;
   NfVec3 p={1.5f,.5f,1.5f};
   NfVec3 d={0};
   if(case_id<10u)d.x=.005f*(float)j;
   else if(case_id<20u)d.z=.005f*(float)j;
   else if(case_id<30u)p.x=100.f+(float)j;
   else p.z=-100.f-(float)j;
   memcpy(oracle_grid,source,sizeof *oracle_grid);
   uint32_t loads=0;
   const Nf18a5Query oracle=nf18a5_sweep_query(
        oracle_grid,p,d,shape,.016f,f->epoch,NULL,NULL,&loads);
   NfRetcSweptMapWitness observed={0};
   if(!nf_retc_map_sweep_from_lease(&lease,&v,
        f->epoch,f->transaction_id,f->owner_actor,f->object_id,
        f->event.contract_epoch,p,d,shape,.016f,&observed)||
      oracle!=observed.result||
      (case_id<20u&&oracle!=NF18A5_Q_FREE)||
      (case_id>=20u&&oracle!=NF18A5_Q_PENDING)){
    passed=false;break;
   }
   NfRetcSweptMapWitness clearance={
       0xaaaaU,0xbbbbU,0xccccU,NF18A5_Q_INVALID};
   const NfRetcSweptMapWitness original=clearance;
   const bool admitted=nf_retc_map_confirm_free(&lease,&v,
        f->epoch,f->transaction_id,f->owner_actor,f->object_id,
        f->event.contract_epoch,p,d,shape,.016f,&clearance);
   if(admitted!=(oracle==NF18A5_Q_FREE)||
      (admitted&&clearance.result!=NF18A5_Q_FREE)||
      (!admitted&&memcmp(&clearance,&original,sizeof original)!=0)||
      memcmp(source,source_snapshot,sizeof *source_snapshot)!=0){
    passed=false;break;
   }
   fprintf(stderr,
      "H301 REAL_A6_DIFFERENTIAL case=%u epoch=%u oracle=%u admitted=%u source_unchanged=1\n",
      case_id,expected,(unsigned)oracle,(unsigned)admitted);
  }
  free(oracle_grid);free(source_snapshot);
  if(!passed)break;
  fprintf(stderr,"H301 REAL_A6_DIFFERENTIAL PASS 40 paths, exact A5 parity\n");
  ok=true;
 }while(0);
 nf_retc_read_lease_release(&lease);
 return ok;
}
static bool verify(const NfWorld *w,const Nf18a6Runtime *r,
 const Nf18b2EncounterRuntime *enc,const Nf18b6Push *push,
 const NfRetcPublisher *publisher,uint32_t expected){
 NfRetcReadView view={0};
 if(!h296_leased_source_check(publisher,expected)||
    !h297_real_map_check(publisher,expected)||
    !h298_real_material_check(publisher,expected)||
    !h299_real_swept_map_check(publisher,expected)||
    !h300_real_clearance_check(publisher,expected)||
    !h301_real_sweep_differential(publisher,expected)||
    !nf_retc_read_joint_view(publisher,&view))return false;
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
/* H283: validate the real A6 accepted joint on-disk journal as a complete
 * B2/B3/A4/A5 frame; corrupt independent PRIVATE COPIES, never the live
 * journal or the published pointer. This is integrity preflight, not crash
 * recovery or a durable directory-sync guarantee. */
static int h283_journal_readback_probe(
    const char *journal,const NfRetcPublisher *publisher,
    const NfRetcFrame *accepted,uint32_t tick){
    if(!journal||!publisher||!accepted||accepted->epoch!=tick)return 1;
    NfRetcFrame *restored=malloc(sizeof *restored);
    NfRetcFrame *unchanged=malloc(sizeof *unchanged);
    FILE *fp=NULL;
    unsigned char *bytes=NULL;
    int rc=1;
    char scratch[1024]={0};
    if(!restored||!unchanged||
       snprintf(scratch,sizeof scratch,"%s.h283-probe",journal)>=(int)sizeof scratch)
        goto finish;
    if(!nf_retc_restore_file(journal,restored)||
       memcmp(restored,accepted,sizeof *restored)!=0||
       nf_retc_read(publisher)!=accepted)goto finish;
    fp=fopen(journal,"rb");
    if(!fp||fseek(fp,0,SEEK_END)!=0)goto finish;
    const long file_len=ftell(fp);
    if(file_len<16L+30L||file_len>(long)sizeof *restored+64L||
       fseek(fp,0,SEEK_SET)!=0)goto finish;
    const size_t n=(size_t)file_len;
    bytes=malloc(n);
    if(!bytes||fread(bytes,1,n,fp)!=n)goto finish;
    if(fclose(fp)!=0){fp=NULL;goto finish;} fp=NULL;
    *unchanged=*accepted;
    for(unsigned c=0;c<30u;++c){
        const size_t offset=c<16u?(size_t)c:
          16u+(size_t)(c-16u)*(n-17u)/13u;
        const unsigned char original=bytes[offset];
        bytes[offset]^=0x5Au;
        fp=fopen(scratch,"wb");
        if(!fp)goto finish;
        if(fwrite(bytes,1,n,fp)!=n||fclose(fp)!=0){
          fp=NULL;goto finish;
        }fp=NULL;
        *restored=*unchanged;
        const bool corrupt_recovered=nf_retc_restore_file(scratch,restored);
        bytes[offset]=original;
        if(corrupt_recovered||
           memcmp(restored,unchanged,sizeof *restored)!=0||
           nf_retc_read(publisher)!=accepted)goto finish;
        fprintf(stderr,"H283 JOURNAL_REJECT tick=%u group=%u case=%u offset=%zu\n",
                tick,c/5u,c,offset);
    }
    if(!nf_retc_restore_file(journal,restored)||
       memcmp(restored,accepted,sizeof *restored)!=0||
       nf_retc_read(publisher)!=accepted)goto finish;
    fprintf(stderr,"H283 JOURNAL_PASS tick=%u restored=1 corrupt_rejected=30 source_owned=1\n",
            tick);
    rc=0;
finish:
    if(fp)fclose(fp);
    if(scratch[0])remove(scratch);
    free(bytes);free(restored);free(unchanged);
    return rc;
}
/* H284: fork 30 real A6/B2/B3 second-tick owner attempts (5 distinct
 * persistence kill points, 6 independent process executions each).
 * The parent retains the previous real accepted immutable source frame.
 * After-rename crashes expose a complete FUTURE disk epoch despite the
 * parent having no newly published pointer; explicitly record this as an
 * unresolved C3 recovery boundary rather than falsely declaring atomicity.
 * Test-only journal restoration does not represent production recovery. */
static int h284_real_crash_matrix(const char *journal,
 NfWorld *w,Nf18a6Runtime *r,NfRetcPublisher *publisher){
 if(!journal||!w||!r||!publisher||w->tick!=1u)return 1;
 const NfRetcFrame *prior=nf_retc_read(publisher);
 if(!prior||prior->epoch!=1u)return 1;
 FILE *fp=fopen(journal,"rb");
 if(!fp)return 1;
 int rc=1;
 unsigned char *snapshot=NULL;
 NfRetcFrame *restored=NULL;
 char stage[1024]={0};
 if(fseek(fp,0,SEEK_END)!=0)goto finish;
 const long n_long=ftell(fp);
 if(n_long<=0L||n_long>(long)sizeof(NfRetcFrame)+64L||
    fseek(fp,0,SEEK_SET)!=0)goto finish;
 const size_t n=(size_t)n_long;
 snapshot=malloc(n);
 restored=malloc(sizeof *restored);
 if(!snapshot||!restored||fread(snapshot,1,n,fp)!=n)goto finish;
 if(fclose(fp)!=0){fp=NULL;goto finish;}fp=NULL;
 if(snprintf(stage,sizeof stage,"%s.staging",journal)>=(int)sizeof stage)
    goto finish;
 for(unsigned case_id=0;case_id<30u;++case_id){
    const unsigned point=case_id%5u+1u;
    char setting[3]={0};
    setting[0]=(char)('0'+point);
    const pid_t pid=fork();
    if(pid<0)goto finish;
    if(pid==0){
       if(setenv("NF_RETC_H284_KILL_POINT",setting,1)!=0)_exit(90);
       const NfWorldTickStatus status=
          nf_world_step_checked(w,1.f/(float)NF_TICK_RATE);
       (void)status;
       _exit(91); /* must instead terminate from real journal hook */
    }
    int status=0;
    if(waitpid(pid,&status,0)!=pid||!WIFEXITED(status)||
       WEXITSTATUS(status)!=(int)(110u+point))goto finish;
    if(!nf_retc_restore_file(journal,restored)||
       restored->epoch!=(point==5u?2u:1u)||
       nf_retc_read(publisher)!=prior||w->tick!=1u)goto finish;
    /* Journal can be overwritten with a complete new epoch before
     * publication. This is NOT yet a C3 restart acceptance proof. */
    if(point==5u)
      fprintf(stderr,"H284 PREPUBLICATION_GAP case=%u disk_epoch=2 live_epoch=1\n",
              case_id);
    fprintf(stderr,"H284 PROCESS_KILL case=%u group=%u point=%u recovered_epoch=%u\n",
            case_id,case_id/5u,point,restored->epoch);
    fp=fopen(journal,"wb");
    if(!fp)goto finish;
    if(fwrite(snapshot,1,n,fp)!=n||fflush(fp)!=0||
       fsync(fileno(fp))!=0||fclose(fp)!=0){
       fp=NULL;goto finish;
    }fp=NULL;
    if(stage[0])remove(stage);
    if(!nf_retc_restore_file(journal,restored)||
       restored->epoch!=1u||nf_retc_read(publisher)!=prior)
      goto finish;
 }
 fprintf(stderr,
   "H284 PROCESS_MATRIX PASS killed=30 before_rename=24 after_rename=6 disk_live_gap=6\n");
 rc=0;
finish:
 if(fp)fclose(fp);
 if(stage[0])remove(stage);
 free(snapshot);free(restored);
 return rc;
}
static int h285_real_cold_restart_matrix(const char *journal,
 NfWorld *w,Nf18a6Runtime *r,NfRetcPublisher *publisher,
 bool h286_admission){
 if(!journal||!w||!r||!publisher||w->tick!=1u)return 1;
 const NfRetcFrame *prior=nf_retc_read(publisher);
 if(!prior||prior->epoch!=1u)return 1;
 FILE *fp=fopen(journal,"rb");
 if(!fp)return 1;
 int rc=1;
 unsigned char *snapshot=NULL;
 NfRetcFrame *restored=NULL;
 char stage[1024]={0};
 if(fseek(fp,0,SEEK_END)!=0)goto finish;
 const long n_long=ftell(fp);
 if(n_long<=0L||n_long>(long)sizeof(NfRetcFrame)+64L||
    fseek(fp,0,SEEK_SET)!=0)goto finish;
 const size_t n=(size_t)n_long;
 snapshot=malloc(n);
 restored=malloc(sizeof *restored);
 if(!snapshot||!restored||fread(snapshot,1,n,fp)!=n)goto finish;
 if(fclose(fp)!=0){fp=NULL;goto finish;}fp=NULL;
 if(snprintf(stage,sizeof stage,"%s.staging",journal)>=(int)sizeof stage)
    goto finish;
 for(unsigned case_id=0;case_id<40u;++case_id){
    const unsigned point=case_id%5u+1u;
    char setting[3]={0};
    setting[0]=(char)('0'+point);
    const pid_t pid=fork();
    if(pid<0)goto finish;
    if(pid==0){
       if(setenv("NF_RETC_H284_KILL_POINT",setting,1)!=0)_exit(90);
       const NfWorldTickStatus status=
          nf_world_step_checked(w,1.f/(float)NF_TICK_RATE);
       (void)status;
       _exit(91); /* must instead terminate from real journal hook */
    }
    int status=0;
    if(waitpid(pid,&status,0)!=pid||!WIFEXITED(status)||
       WEXITSTATUS(status)!=(int)(110u+point))goto finish;
    if(!nf_retc_restore_file(journal,restored)||
       restored->epoch!=(point==5u?2u:1u)||
       nf_retc_read(publisher)!=prior||w->tick!=1u)goto finish;
    /* Independently exec a cold reader in a FRESH process after every
     * genuine A6 child dies. The new process has no inherited owner,
     * publisher, encounter registry, or in-memory authoritative frame.
     * It can validate a journal snapshot but cannot choose C3 promotion. */
    const pid_t reader=fork();
    if(reader<0)goto finish;
    if(reader==0){
       char epoch_arg[4]={0};
       epoch_arg[0]=(char)('0'+(point==5u?2u:1u));
       execl("/proc/self/exe","nightfall-retc-h285-cold",
             "--cold-audit",journal,epoch_arg,(char *)NULL);
       _exit(93);
    }
    int reader_status=0;
    if(waitpid(reader,&reader_status,0)!=reader||
       !WIFEXITED(reader_status)||WEXITSTATUS(reader_status)!=0)
      goto finish;
    /* A separate clean process must NOT accept a journal under the
     * wrong expected epoch. This catches stale/future replay. */
    const pid_t denied_reader=fork();
    if(denied_reader<0)goto finish;
    if(denied_reader==0){
       const char *wrong_epoch=point==5u?"1":"2";
       execl("/proc/self/exe","nightfall-retc-h285-cold",
             "--cold-audit",journal,wrong_epoch,(char *)NULL);
       _exit(94);
    }
    int denied_status=0;
    if(waitpid(denied_reader,&denied_status,0)!=denied_reader||
       !WIFEXITED(denied_status)||WEXITSTATUS(denied_status)!=4)
      goto finish;
    fprintf(stderr,"H285 COLD_DENIAL case=%u wrong_epoch_rejected=1\n",case_id);
    if(h286_admission){
      /* One fresh executable per classification. The independent
       * checkpoint tuple is deliberately not read from the candidate. */
      static const char *const denied[]={
        "wrong-owner","wrong-object","wrong-epoch","wrong-transaction"};
      for(unsigned audit=0;audit<5u;++audit){
        const char *mode=audit==0u?
             (point==5u?"next":"prior"):denied[audit-1u];
        const pid_t child=fork();
        if(child<0)goto finish;
        if(child==0){
          execl("/proc/self/exe","nightfall-retc-h286",
                "--admission-audit",journal,mode,(char*)NULL);
          _exit(95);
        }
        int child_status=0;
        if(waitpid(child,&child_status,0)!=child||
           !WIFEXITED(child_status)||WEXITSTATUS(child_status)!=0)
          goto finish;
      }
      fprintf(stderr,
       "H286 ADMISSION_CASE case=%u group=%u point=%u status=%s denials=4 checkpoint=1\n",
       case_id,case_id/5u,point,point==5u?"NEXT":"PRIOR");
    }
    if((access(stage,F_OK)==0)!=(point!=5u))goto finish;
    fprintf(stderr,
      "H285 COLD_RESTART case=%u group=%u point=%u expected_epoch=%u fresh_process=1 stage_present=%u\n",
      case_id,case_id/5u,point,restored->epoch,point!=5u);
    /* Journal can be overwritten with a complete new epoch before
     * publication. This is NOT yet a C3 restart acceptance proof. */
    if(point==5u)
      fprintf(stderr,"H285 PREPUBLICATION_GAP case=%u disk_epoch=2 live_epoch=1\n",
              case_id);
    fprintf(stderr,"H285 PROCESS_KILL case=%u group=%u point=%u recovered_epoch=%u\n",
            case_id,case_id/5u,point,restored->epoch);
    fp=fopen(journal,"wb");
    if(!fp)goto finish;
    if(fwrite(snapshot,1,n,fp)!=n||fflush(fp)!=0||
       fsync(fileno(fp))!=0||fclose(fp)!=0){
       fp=NULL;goto finish;
    }fp=NULL;
    if(stage[0])remove(stage);
    if(!nf_retc_restore_file(journal,restored)||
       restored->epoch!=1u||nf_retc_read(publisher)!=prior)
      goto finish;
 }
 fprintf(stderr,
   "H285 COLD_MATRIX PASS killed=40 cold_restarts=40 prior_epoch=32 new_epoch=8 disk_live_gap=8\n");
 if(h286_admission)
   fprintf(stderr,
     "H286 ADMISSION_MATRIX PASS kills=40 independent_admissions=40 independent_denials=160 prior=32 next=8\n");
 rc=0;
finish:
 if(fp)fclose(fp);
 if(stage[0])remove(stage);
 free(snapshot);free(restored);
 return rc;
}
/* H287: directory durability experiment: 40 source-authorized child
 * process interruptions, including before vs AFTER parent-directory fsync.
 * Source journal is reset only by the independent research parent, never by
 * the production persistence implementation. */
static int h287_real_directory_matrix(const char *journal,
 NfWorld *w,Nf18a6Runtime *r,NfRetcPublisher *publisher){
 if(!journal||!w||!r||!publisher||w->tick!=1u)return 1;
 const NfRetcFrame *prior=nf_retc_read(publisher);
 if(!prior||prior->epoch!=1u)return 1;
 FILE *fp=fopen(journal,"rb");
 if(!fp)return 1;
 int rc=1;
 unsigned char *snapshot=NULL;
 NfRetcFrame *restored=NULL;
 char stage[1024]={0};
 if(fseek(fp,0,SEEK_END)!=0)goto finish;
 const long n_long=ftell(fp);
 if(n_long<=0L||n_long>(long)sizeof(NfRetcFrame)+64L||
    fseek(fp,0,SEEK_SET)!=0)goto finish;
 const size_t n=(size_t)n_long;
 snapshot=malloc(n);
 restored=malloc(sizeof *restored);
 if(!snapshot||!restored||fread(snapshot,1,n,fp)!=n)goto finish;
 if(fclose(fp)!=0){fp=NULL;goto finish;}fp=NULL;
 if(snprintf(stage,sizeof stage,"%s.staging",journal)>=(int)sizeof stage)
    goto finish;
 for(unsigned case_id=0;case_id<40u;++case_id){
    /* Five different persistence boundaries across eight source trials. */
    static const unsigned points[5]={1u,2u,4u,5u,6u};
    const unsigned point=points[case_id%5u];
    char setting[3]={0};
    setting[0]=(char)('0'+point);
    const pid_t pid=fork();
    if(pid<0)goto finish;
    if(pid==0){
       if(setenv("NF_RETC_H284_KILL_POINT",setting,1)!=0)_exit(90);
       const NfWorldTickStatus status=
          nf_world_step_checked(w,1.f/(float)NF_TICK_RATE);
       (void)status;
       _exit(91); /* must instead terminate from real journal hook */
    }
    int status=0;
    if(waitpid(pid,&status,0)!=pid||!WIFEXITED(status)||
       WEXITSTATUS(status)!=(int)(110u+point))goto finish;
    if(!nf_retc_restore_file(journal,restored)||
       restored->epoch!=(point>=5u?2u:1u)||
       nf_retc_read(publisher)!=prior||w->tick!=1u)goto finish;
    /* Independently exec a cold reader in a FRESH process after every
     * genuine A6 child dies. The new process has no inherited owner,
     * publisher, encounter registry, or in-memory authoritative frame.
     * It can validate a journal snapshot but cannot choose C3 promotion. */
    const pid_t reader=fork();
    if(reader<0)goto finish;
    if(reader==0){
       char epoch_arg[4]={0};
       epoch_arg[0]=(char)('0'+(point>=5u?2u:1u));
       execl("/proc/self/exe","nightfall-retc-h285-cold",
             "--cold-audit",journal,epoch_arg,(char *)NULL);
       _exit(93);
    }
    int reader_status=0;
    if(waitpid(reader,&reader_status,0)!=reader||
       !WIFEXITED(reader_status)||WEXITSTATUS(reader_status)!=0)
      goto finish;
    /* A separate clean process must NOT accept a journal under the
     * wrong expected epoch. This catches stale/future replay. */
    const pid_t denied_reader=fork();
    if(denied_reader<0)goto finish;
    if(denied_reader==0){
       const char *wrong_epoch=point>=5u?"1":"2";
       execl("/proc/self/exe","nightfall-retc-h285-cold",
             "--cold-audit",journal,wrong_epoch,(char *)NULL);
       _exit(94);
    }
    int denied_status=0;
    if(waitpid(denied_reader,&denied_status,0)!=denied_reader||
       !WIFEXITED(denied_status)||WEXITSTATUS(denied_status)!=4)
      goto finish;
    fprintf(stderr,"H287 COLD_DENIAL case=%u wrong_epoch_rejected=1\n",case_id);
    if((access(stage,F_OK)==0)!=(point<5u))goto finish;
    fprintf(stderr,
      "H287 DIRSYNC_RESTART case=%u group=%u point=%u expected_epoch=%u fresh_process=1 stage_present=%u\n",
      case_id,case_id/5u,point,restored->epoch,point<5u);
    /* Journal can be overwritten with a complete new epoch before
     * publication. This is NOT yet a C3 restart acceptance proof. */
    if(point>=5u)
      fprintf(stderr,"H287 AFTER_RENAME case=%u disk_epoch=2 live_epoch=1\n",
              case_id);
    fprintf(stderr,"H287 PROCESS_KILL case=%u group=%u point=%u recovered_epoch=%u\n",
            case_id,case_id/5u,point,restored->epoch);
    fp=fopen(journal,"wb");
    if(!fp)goto finish;
    if(fwrite(snapshot,1,n,fp)!=n||fflush(fp)!=0||
       fsync(fileno(fp))!=0||fclose(fp)!=0){
       fp=NULL;goto finish;
    }fp=NULL;
    if(stage[0])remove(stage);
    if(!nf_retc_restore_file(journal,restored)||
       restored->epoch!=1u||nf_retc_read(publisher)!=prior)
      goto finish;
 }
 fprintf(stderr,
   "H287 DIRSYNC_MATRIX PASS killed=40 clean_cold=40 rejected_cold=40 before_rename=24 after_rename=16 post_dirsync=8\n");
 rc=0;
finish:
 if(fp)fclose(fp);
 if(stage[0])remove(stage);
 free(snapshot);free(restored);
 return rc;
}
/* H288: one substantive failure mode repeated forty times in eight
 * 5-run groups. The *real* A6/B2/B3 tick reaches a successful rename,
 * then the lab-only parent-directory fsync returns EIO. Production should
 * refuse publication, while cold disk inspection observes a NEXT candidate.
 * Parent restores the pre-test journal only to preserve positive controls.
 * This is not full owner restart, power-loss, or a production race test. */
static int h288_directory_sync_failure_matrix(
 const char *journal,NfWorld *w,NfRetcPublisher *publisher){
 if(!journal||!w||!publisher||w->tick!=1u)return 1;
 const NfRetcFrame *prior=nf_retc_read(publisher);
 if(!prior||prior->epoch!=1u)return 1;
 char stage[1024]={0};
 if(snprintf(stage,sizeof stage,"%s.staging",journal)>=(int)sizeof stage)
   return 1;
 FILE *fp=fopen(journal,"rb");
 if(!fp)return 1;
 unsigned char *snapshot=NULL;
 NfRetcFrame *candidate=malloc(sizeof *candidate);
 int rc=1;
 if(fseek(fp,0,SEEK_END)!=0)goto finish;
 long length=ftell(fp);
 if(length<=0||length>(long)sizeof(NfRetcFrame)+64L||
    fseek(fp,0,SEEK_SET)!=0)goto finish;
 size_t n=(size_t)length;
 snapshot=malloc(n);
 if(!snapshot||!candidate||fread(snapshot,1,n,fp)!=n)goto finish;
 if(fclose(fp)!=0){fp=NULL;goto finish;}fp=NULL;
 for(unsigned i=0u;i<40u;++i){
    pid_t pid=fork();
    if(pid<0)goto finish;
    if(pid==0){
      if(setenv("NF_RETC_H288_FAIL_DIRSYNC","1",1)!=0)_exit(92);
      const NfWorldTickStatus st=
          nf_world_step_checked(w,1.f/(float)NF_TICK_RATE);
      _exit(st!=NF_WORLD_TICK_COMMITTED&&nf_retc_read(publisher)==prior?0:93);
    }
    int result=0;
    if(waitpid(pid,&result,0)!=pid||!WIFEXITED(result)||
       WEXITSTATUS(result)!=0)goto finish;
    if(nf_retc_read(publisher)!=prior||w->tick!=1u||
       !nf_retc_restore_file(journal,candidate)||
       candidate->epoch!=2u||candidate->prior_epoch!=1u)
      goto finish;
    if(nf_retc_recovery_inspect_file(journal,prior->owner_actor,
       prior->object_id,prior->epoch,prior->transaction_id,candidate)
       !=NF_RETC_RECOVERY_NEXT)goto finish;
    if(nf_retc_read(publisher)!=prior)goto finish;
    fprintf(stderr,
      "H288 FAIL_CLOSED case=%u group=%u disk_epoch=2 live_epoch=1 status=NEXT\n",
      i,i/5u);
    fp=fopen(journal,"wb");
    if(!fp)goto finish;
    const bool written=fwrite(snapshot,1,n,fp)==n&&
       fflush(fp)==0&&fsync(fileno(fp))==0;
    if(fclose(fp)!=0||!written){fp=NULL;goto finish;}fp=NULL;
    remove(stage);
    if(!nf_retc_restore_file(journal,candidate)||
       candidate->epoch!=1u||nf_retc_read(publisher)!=prior)goto finish;
 }
 fprintf(stderr,
   "H288 FAIL_SYNC_MATRIX PASS injected=40 rejected_publications=40 recovered_next_candidates=40 publisher_unchanged=40\n");
 rc=0;
finish:
 if(fp)fclose(fp);
 if(stage[0])remove(stage);
 free(snapshot);free(candidate);
 return rc;
}
/* H285 cold-process integrity audit. Do not construct a simulated world:
 * only read the actual real issuer's journal from a fresh process image.
 * A successful read is not a legitimate owner restart or C3 promotion. */
static int h285_cold_audit(const char *path,const char *expected){
 if(!path||!expected||expected[1]!='\0'||
    (expected[0]!='1'&&expected[0]!='2'))return 2;
 const uint32_t epoch=(uint32_t)(expected[0]-'0');
 NfRetcFrame *f=malloc(sizeof *f);
 if(!f)return 3;
 const bool good=nf_retc_restore_file(path,f)&&
   f->epoch==epoch&&f->prior_epoch==epoch-1u&&
   f->physical.world.tick==epoch&&
   f->owner_actor==10u&&f->object_id==2010u&&
   f->physical.actor.id==10u&&f->physical.object.id==2010u&&
   f->event.kind==NF18B3_PUSH&&f->event.phase==NF18B3_APPLIED&&
   f->event.authorized&&f->event.applied&&
   f->event.tick==epoch&&f->event.actor==10u&&
   f->event.object==2010u&&
   f->b2_use.tick==epoch&&f->b2_use.actor_id==10u&&
   f->b2_use.object_id==2010u&&
   f->b2_use.action_id==f->event.action&&
   f->b2_decision.status==NF18B2_ELIGIBLE&&
   f->b2_decision.physical==NF18B2_ELIGIBLE&&
   f->b2_decision.contract==NF18B2_ELIGIBLE&&
   f->event.contract_epoch==f->encounter_registry.contract_epoch&&
   f->has_b2_contract&&f->has_encounter_registry;
 if(good)fprintf(stderr,"H285 COLD_SOURCE_OK epoch=%u B2_B3_A4_A5=1\n",epoch);
 free(f);
 return good?0:4;
}
/* H286 strict read-only admission inspection in a COLD process.
 * The trusted tuple is supplied independently of the candidate journal.
 * Wrong identities, epochs and transaction bounds must fail closed without
 * touching the provided output storage. This is not a new live A6 owner. */
static int h286_cold_admission(const char *journal,const char *mode){
 if(!journal||!mode)return 2;
 NfRetcFrame *out=malloc(sizeof *out);
 if(!out)return 3;
 memset(out,0xA5,sizeof *out);
 uint32_t owner=10u,object=2010u,epoch=1u,tx=1u;
 NfRetcRecoveryStatus want=NF_RETC_RECOVERY_REJECT;
 if(strcmp(mode,"prior")==0)want=NF_RETC_RECOVERY_PRIOR;
 else if(strcmp(mode,"next")==0)want=NF_RETC_RECOVERY_NEXT;
 else if(strcmp(mode,"wrong-owner")==0)owner=11u;
 else if(strcmp(mode,"wrong-object")==0)object=2011u;
 else if(strcmp(mode,"wrong-epoch")==0)epoch=7u;
 else if(strcmp(mode,"wrong-transaction")==0)tx=999u;
 else {free(out);return 2;}
 const NfRetcRecoveryStatus got=nf_retc_recovery_inspect_file(
    journal,owner,object,epoch,tx,out);
 bool good=(got==want);
 if(want==NF_RETC_RECOVERY_REJECT){
   const unsigned char *bytes=(const unsigned char *)out;
   for(size_t i=0;i<sizeof *out;++i)
      if(bytes[i]!=0xA5){good=false;break;}
 }else{
   good=good&&out->owner_actor==10u&&out->object_id==2010u&&
        out->epoch==(want==NF_RETC_RECOVERY_NEXT?2u:1u)&&
        out->event.kind==NF18B3_PUSH&&out->event.applied&&
        out->has_b2_contract&&out->has_encounter_registry;
 }
 if(good)fprintf(stderr,"H286 COLD_ADMISSION mode=%s status=%u unchanged_on_reject=1\n",mode,(unsigned)got);
 free(out);return good?0:4;
}
int main(int argc,char **argv){
 if(argc==4&&strcmp(argv[1],"--admission-audit")==0)
    return h286_cold_admission(argv[2],argv[3]);
 if(argc==4&&strcmp(argv[1],"--cold-audit")==0)
    return h285_cold_audit(argv[2],argv[3]);
 /* H281: 40 independent source-owned cases. No altered production
  * authorization, synthetic issuer, or research-mode bypass. The default
  * case remains bit-for-bit the H280 positive physical control. */
 unsigned h281_case=0u;
 const bool h283_journal_probes=argc==2&&strcmp(argv[1],"--journal-probes")==0;
 const bool h284_crash_matrix=argc==2&&strcmp(argv[1],"--crash-matrix")==0;
 const bool h285_cold_matrix=argc==2&&strcmp(argv[1],"--cold-matrix")==0;
 const bool h286_admission_matrix=argc==2&&strcmp(argv[1],"--admission-matrix")==0;
 const bool h287_directory_matrix=argc==2&&strcmp(argv[1],"--directory-matrix")==0;
 const bool h288_sync_failure=argc==2&&strcmp(argv[1],"--sync-failure-matrix")==0;
 if(argc==3&&strcmp(argv[1],"--sweep-case")==0){
   char *end=NULL;
   const unsigned long parsed=strtoul(argv[2],&end,10);
   if(!argv[2][0]||!end||*end||parsed>=40ul){
      fprintf(stderr,"H281 invalid sweep case; expected 0..39\n");return 2;
   }
   h281_case=(unsigned)parsed;
 }else if(argc!=1&&!h283_journal_probes&&!h284_crash_matrix&&!h285_cold_matrix&&!h286_admission_matrix&&!h287_directory_matrix&&!h288_sync_failure){
   fprintf(stderr,"H281/H284 usage: [--sweep-case 0..39] [--journal-probes] [--crash-matrix]\n");return 2;
 }
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
       "RET-C B3 arm tick=%u result=%d b2=%d event_phase=%d armed=%d contract_epoch=%u encounter_epoch=%u queue=%zu witness_revision=%u live_revision=%u target=(%.4f,%.4f,%.4f) crate=(%.4f,%.4f,%.4f)\n",
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
   NfMoveInput move={0};move.strafe=1;
   /* H281: eight yaw families by five forward-input variations.
    * All 40 cases retain two real B2/B3 authorized owner ticks.
    * First tick is the exact H280 unchanged anchor; tick 2 varies.
    * No geometry or authority structures are fabricated for cases. */
   move.forward=tick==1u?.2f:.2f+.002f*(float)(h281_case%5u);
   move.yaw_radians=tick==1u?0.f:
      -.10f-.005f*(float)(h281_case/5u);
   move.interact_held=true;
   nf_world_set_input(w,10,move);
   if(h284_crash_matrix&&tick==2u)
      CHECK(h284_real_crash_matrix(journal,w,r,publisher)==0);
   if((h285_cold_matrix||h286_admission_matrix)&&tick==2u)
      CHECK(h285_real_cold_restart_matrix(journal,w,r,publisher,
             h286_admission_matrix)==0);
   if(h287_directory_matrix&&tick==2u)
      CHECK(h287_real_directory_matrix(journal,w,r,publisher)==0);
   if(h288_sync_failure&&tick==2u)
      CHECK(h288_directory_sync_failure_matrix(journal,w,publisher)==0);
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
   if(h283_journal_probes)
      CHECK(h283_journal_readback_probe(
          journal,publisher,h276c_accepted,tick)==0);
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
   fprintf(stderr,
       "H281 SOURCE_MATRIX case=%u tick=%u forward=%.5f yaw=%.5f actor_dx=%.8f actor_dz=%.8f crate_dx=%.8f crate_dz=%.8f receipts=%u\n",
       h281_case,tick,move.forward,move.yaw_radians,
       h280_actor_dx,h280_actor_dz,h280_crate_dx,h280_crate_dz,
       h280_next->committed_receipts);
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
   /* H282 C2->C3 litmus: thirty independent rejected corruptions of
      a REAL owner-accepted B2/B3 frame, organized into six groups of
      five (time / owner IDs / B3 / B2 / contract / physical geometry).
      The original accepted frame and live publisher remain untouched.
      This is a fail-closed audit test, NOT a crash-recovery simulation. */
   NfRetcFrame *h282_probe=malloc(sizeof *h282_probe);
   CHECK(h282_probe);
   for(unsigned h282_case=0u;h282_case<30u;++h282_case){
      *h282_probe=*h276b_accepted;
      NfRetcFrame *probe=h282_probe;
      switch(h282_case){
      case 0u: probe->epoch+=1u; break; /* epoch */
      case 1u: probe->physical.world.tick+=1u; break; /* physical_world_tick */
      case 2u: probe->physical.world.revision+=1u; break; /* physical_revision */
      case 3u: probe->b2_use.tick+=1u; break; /* b2_tick */
      case 4u: probe->event.tick+=1u; break; /* b3_tick */
      case 5u: probe->owner_actor+=1u; break; /* owner_actor */
      case 6u: probe->object_id+=1u; break; /* object_id */
      case 7u: probe->physical.actor.id+=1u; break; /* physical_actor_id */
      case 8u: probe->physical.object.id+=1u; break; /* physical_object_id */
      case 9u: probe->event.actor+=1u; break; /* event_actor */
      case 10u: probe->event.object+=1u; break; /* event_object */
      case 11u: probe->event.action+=1u; break; /* event_action */
      case 12u: probe->event.kind=0; break; /* event_kind */
      case 13u: probe->event.phase=0; break; /* event_phase */
      case 14u: probe->event.authorized=false; break; /* event_authorized */
      case 15u: probe->b2_use.actor_id+=1u; break; /* b2_actor */
      case 16u: probe->b2_use.object_id+=1u; break; /* b2_object */
      case 17u: probe->b2_use.action_id+=1u; break; /* b2_action */
      case 18u: probe->b2_decision.status=NF18B2_BLOCKED; break; /* b2_eligible_status */
      case 19u: probe->b2_decision.physical=NF18B2_BLOCKED; break; /* b2_physical_status */
      case 20u: probe->b2_decision.contract=NF18B2_BLOCKED; break; /* b2_contract_status */
      case 21u: probe->encounter_registry.contract_epoch+=1u; break; /* ledger_contract_epoch */
      case 22u: probe->has_b2_contract=false; break; /* b2_presence */
      case 23u: probe->has_encounter_registry=false; break; /* ledger_presence */
      case 24u: probe->event.contract_epoch+=1u; break; /* event_contract_epoch */
      case 25u: probe->physical.actor.center.x+=.01f; break; /* actor_center_x */
      case 26u: probe->physical.actor.center.z+=.01f; break; /* actor_center_z */
      case 27u: probe->physical.object.center.x+=.01f; break; /* crate_center_x */
      case 28u: probe->physical.object.box_half.x+=.01f; break; /* crate_half_x */
      case 29u: probe->physical.object.center.z+=.01f; break; /* crate_center_z */
        default: CHECK(false);
      }
      CHECK(!nf_retc_m2_real_contract_audit(
          w,r,enc,push,h282_probe));
      CHECK(nf_retc_read(publisher)==h276b_accepted&&
            nf_retc_m2_real_contract_audit(
               w,r,enc,push,h276b_accepted));
      fprintf(stderr,"H282 BOUNDARY_REJECT tick=%u group=%u case=%u\n",
        tick,h282_case/5u,h282_case);
   }
   free(h282_probe);
   fprintf(stderr,"H282 BOUNDARY_PASS tick=%u rejected=30 source_owned=1\n",tick);
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
   fprintf(stderr,"H92 real B2 submissions: actor10=%d actor20=%d queue=%zu\n",
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
   fprintf(stderr,"H93 source B2 authorization: actor10 status=%d reason=%d physical=%d contract=%d; actor20 status=%d reason=%d physical=%d contract=%d\n",
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
  fprintf(stderr,"H95 nearby source B2: status=%d count=%zu\n",
      (int)nearby,enc->queue_count);
  CHECK(nearby!=NF18B2_INVALID);
  CHECK(enc->queue_count==old_count+1u);
  const Nf18b2QueuedIntent *q=&enc->queue[old_count];
  CHECK(q->intent.actor_id==20u&&q->trace_index<enc->trace_count);
  const Nf18b2EncounterTrace *t=&enc->traces[q->trace_index];
  fprintf(stderr,"H95 nearby reason=%d physical=%d contract=%d\n",
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
  fprintf(stderr,"H126 plan diagnostics: first_status=%d first_reason=%d first_physical=%d first_contract=%d second_status=%d second_reason=%d first_trace=%d second_trace=%d first_registry=%u second_registry=%u live_registry=%u first_contract_epoch=%u second_contract_epoch=%u live_contract_epoch=%u\n",
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
  /* H302: 40 real-A6-source coactor refusal cases, 8 motion/input
   * dimensions x 5 invalid/unauthorized values. Every iteration restores
   * the live actor and proves no publication or B3 receipt occurred.
   * This closes an admission vulnerability, NOT active multi-actor B3. */
  for(unsigned h302_case=0u;h302_case<40u;++h302_case){
   const NfTransform before_pose=second_actor->transform;
   const NfMoveInput before_input=second_actor->input;
   const unsigned dim=h302_case/5u;
   const unsigned variant=h302_case%5u;
   const float bad=variant==0u?NAN:
     variant==1u?INFINITY:variant==2u?-INFINITY:
     variant==3u?1.f:-1.f;
   /* Finite pose changes must differ from this fixture's true position;
    * absolute 1.0 can coincide with the legitimate coactor source. */
   switch(dim){
    case 0:second_actor->transform.position.x=
       variant>=3u?before_pose.position.x+bad*.25f:bad;break;
    case 1:second_actor->transform.position.y=
       variant>=3u?before_pose.position.y+bad*.25f:bad;break;
    case 2:second_actor->transform.position.z=
       variant>=3u?before_pose.position.z+bad*.25f:bad;break;
    case 3:second_actor->transform.velocity.x=bad;break;
    case 4:second_actor->transform.velocity.y=bad;break;
    case 5:second_actor->transform.velocity.z=bad;break;
    case 6:second_actor->input.forward=bad;break;
    case 7:second_actor->input.strafe=bad;break;
   }
   const bool rejected=!nf_retc_multi_physical_preflight(
       w,r,enc,publisher,&batch);
   second_actor->transform=before_pose;
   second_actor->input=before_input;
   CHECK(rejected&&nf_retc_read(publisher)==accepted&&
         w->tick==2u&&push->receipt_count==2u);
   CHECK(nf_retc_multi_physical_preflight(w,r,enc,publisher,&batch));
   fprintf(stderr,
      "H302 REAL_B2_COACTOR_DENIAL case=%u dim=%u variant=%u accepted_unchanged=1\n",
      h302_case,dim,variant);
  }
  fprintf(stderr,"H302 REAL_B2_COACTOR PASS 40 denials and positive rechecks\n");
  /* H156: owner-bound physical preflight is read-only and never publishes
     the canonical proposal, changes world tick or adds B3 receipts. */
  CHECK(nf_retc_multi_physical_preflight(w,r,enc,publisher,&batch));
  CHECK(nf_retc_read(publisher)==accepted&&w->tick==2u);
  CHECK(push->receipt_count==2u);
  /* H303: a self-consistent canonical B3 batch is still not proof
   * of TWO genuine current B2 source grants. Seal against the real
   * encounter queue and its live authorization traces before admitting
   * even a read-only plan. No A6 or B3 writes occur in this laboratory. */
  CHECK(nf_retc_multi_source_sealed_preflight(
       w,r,enc,publisher,first_index,old_count,&batch));
  for(unsigned h303_case=0u;h303_case<40u;++h303_case){
   NfRetcCanonicalBatch tampered=batch;
   const uint32_t n=(uint32_t)(h303_case%4u+1u);
   const unsigned group=h303_case/4u;
   switch(group){
    case 0:tampered.events[0].action+=n;break;
    case 1:tampered.events[1].action+=n;break;
    case 2:tampered.events[0].world_epoch+=n;break;
    case 3:tampered.events[1].world_epoch+=n;break;
    case 4:tampered.events[0].material_epoch+=n;break;
    case 5:tampered.events[1].material_epoch+=n;break;
    case 6:tampered.events[0].contract_epoch+=n;break;
    case 7:tampered.events[1].contract_epoch+=n;break;
    case 8:tampered.events[0].tick+=n;break;
    case 9:tampered.events[1].tick+=n;break;
   }
   CHECK(!nf_retc_multi_source_sealed_preflight(
       w,r,enc,publisher,first_index,old_count,&tampered));
   CHECK(nf_retc_read(publisher)==accepted&&
         w->tick==2u&&push->receipt_count==2u);
   CHECK(nf_retc_multi_source_sealed_preflight(
       w,r,enc,publisher,first_index,old_count,&batch));
   fprintf(stderr,"H303 TWO_B2_SOURCE_SEAL_DENIAL case=%u group=%u preserved=1\n",
           h303_case,group);
  }
  fprintf(stderr,"H303 TWO_B2_SOURCE_SEAL PASS 40 source-bound denials\n");
  /* H310 diagnostic integration: B2 source-valid actor10/actor20 shares
     the authoritative A6 encounter, but no positive *active* A4 contact
     exists in this fixture. Never turn two B2 proposals into a commit. */
  NfRetcH310ProbeResult h310_base={0};
  CHECK(!nf_retc_h310_owner_joint_probe(w,r,enc,publisher,
                 first_index,old_count,.125f,&batch,&h310_base));
  CHECK(h310_base.stage==NF_RETC_H310_B2_SOURCE_VALID&&
        !h310_base.prepared);
  CHECK(nf_retc_read(publisher)==accepted&&
        w->tick==2u&&push->receipt_count==2u);
  for(unsigned h310_case=0u;h310_case<20u;++h310_case){
    NfRetcCanonicalBatch edited=batch;
    const unsigned group=h310_case/4u;
    const uint32_t delta=h310_case%4u+1u;
    switch(group){
      case 0:edited.events[0].actor+=delta;break;
      case 1:edited.events[1].actor+=delta;break;
      case 2:edited.events[0].action+=delta;break;
      case 3:edited.events[1].contract_epoch+=delta;break;
      case 4:edited.events[1].tick+=delta;break;
    }
    NfRetcH310ProbeResult denied={0};
    CHECK(!nf_retc_h310_owner_joint_probe(w,r,enc,publisher,
              first_index,old_count,.125f,&edited,&denied));
    CHECK(denied.stage==NF_RETC_H310_DENIED_B2&&!denied.prepared);
    CHECK(!nf_retc_h310_owner_joint_probe(w,r,enc,publisher,
              first_index,old_count,.125f,&batch,&denied));
    CHECK(denied.stage==NF_RETC_H310_B2_SOURCE_VALID&&!denied.prepared);
    CHECK(nf_retc_read(publisher)==accepted&&
          w->tick==2u&&push->receipt_count==2u);
    fprintf(stderr,
       "H310 JOINT_SOURCE_DENIAL case=%u group=%u delta=%u preserved=1\n",
       h310_case,group,delta);
  }
  /* Ten malformed time horizons cannot be used to substitute an A4
     candidate. They are negative controls, not production time steps. */
  const float h310_invalid_dt[10]={NAN,-NAN,INFINITY,-INFINITY,
      0.f,-0.f,-1.f,-.01f,-100.f,-10000.f};
  for(unsigned h310_time=0;h310_time<10u;++h310_time){
    NfRetcH310ProbeResult denied={0};
    CHECK(!nf_retc_h310_owner_joint_probe(w,r,enc,publisher,
      first_index,old_count,h310_invalid_dt[h310_time],&batch,&denied));
    CHECK(denied.stage==NF_RETC_H310_B2_SOURCE_VALID&&!denied.prepared);
    CHECK(nf_retc_read(publisher)==accepted&&
          w->tick==2u&&push->receipt_count==2u);
    fprintf(stderr,
       "H310 JOINT_TIME_DENIAL case=%u preserved=1\n",h310_time);
  }
  fprintf(stderr,
     "H310 OWNER_JOINT_PROBE PASS 20 grant-source refusals + 10 time refusals; authentic B2 stops before active A4\n");
  /* H311: POSITIVE A4 contact feasibility using the exact real B2
     identities 10 and 20 in their SAME A6-owner world (not 30/40
     from the separate laboratory fixture). The B2 proposal is NOT
     reissued after temporary pose changes: it MUST lose source
     freshness. Never commit A6/B3 from this physical-only result. */
  {
    NfActor *h311_owner_actor=nf_world_find_actor(w,10u);
    NfActor *h311_coactor=nf_world_find_actor(w,20u);
    CHECK(h311_owner_actor&&h311_coactor);
    const NfTransform h311_original_primary=h311_owner_actor->transform;
    const NfTransform h311_original_second=h311_coactor->transform;
    const Nf18a4Body h311_original_cached=r->physical.actor;
    const uint64_t h311_tick_before=w->tick;
    const NfRetcFrame *h311_frame_before=nf_retc_read(publisher);
    const uint32_t h311_receipts_before=push->receipt_count;
    CHECK(nf_retc_multi_source_sealed_preflight(
        w,r,enc,publisher,first_index,old_count,&batch));
    for(unsigned h311_case=0u;h311_case<10u;++h311_case){
      const float speed=4.0f+.1f*(float)h311_case;
      const float gap=.80f+.01f*(float)h311_case;
      h311_owner_actor->transform.velocity=(NfVec3){speed,0.f,0.f};
      h311_coactor->transform.position=(NfVec3){
          h311_owner_actor->transform.position.x+gap,
          h311_owner_actor->transform.position.y,
          h311_owner_actor->transform.position.z};
      h311_coactor->transform.velocity=(NfVec3){0.f,0.f,0.f};
      r->physical.actor.center=(NfVec3){
          h311_owner_actor->transform.position.x,
          h311_owner_actor->transform.position.y+
             h311_owner_actor->movement.body_height*.5f,
          h311_owner_actor->transform.position.z};
      r->physical.actor.velocity=h311_owner_actor->transform.velocity;
      r->physical.actor.height=h311_owner_actor->movement.body_height;
      NfRetcMa03PhysicalCandidate same_source={0};
      CHECK(nf_retc_ma03_stage_physics(w,r,.125f,&same_source));
      CHECK(same_source.eligible&&same_source.solved.count==2u&&
            same_source.solved.impacts==1u&&
            same_source.source.actors[0].id==10u&&
            same_source.source.actors[1].id==20u&&
            same_source.reference.matched==1u&&
            same_source.reference.disagreed==0u&&
            nf_retc_ma03_recheck_physics(w,r,.125f,&same_source));
      /* H312: issue FRESH source-derived B2 decisions at the actual
         collision pose, in an isolated unbound encounter. This DOES
         NOT turn a research proposal into owner-issued consumption or
         production authority. A blocked result is an observed boundary. */
      Nf18b2EncounterRuntime *h312_current=calloc(1,sizeof *h312_current);
      CHECK(h312_current);
      /* H312-C1: a SECOND encounter_init on an already attached A6
         owner is correctly refused. For a read-only research probe,
         clone the previously source-registered encounter instead of
         violating one-owner initialization. Rebind all internal scene
         pointers to the cloned storage, clear proposals and never attach. */
      *h312_current=*enc;
      h312_current->queue_count=0u;
      h312_current->trace_count=0u;
      h312_current->grant_count=0u;
      h312_current->prepared=0u;
      h312_current->attached=0u;
      h312_current->frame.scene.actors=h312_current->frame.actors;
      h312_current->frame.scene.objects=h312_current->frame.objects;
      CHECK(nf18b2_live_frame(h312_current,&h312_current->frame));
      const Nf18b2Status h312_first=nf18b2_propose(h312_current,
             (Nf18b2Intent){10u,2010u,700u+h311_case,NF18B2_PUSH});
      const Nf18b2Status h312_second=nf18b2_propose(h312_current,
             (Nf18b2Intent){20u,2010u,800u+h311_case,NF18B2_PUSH});
      /* H313: actual source B2 trace attribution, never a guessed
         geometry or contract cause. Both statuses MUST be explained
         by their own decision from the same current encounter scene. */
      CHECK(h312_current->trace_count>=2u);
      const Nf18b2EncounterTrace *h313_first=
         &h312_current->traces[h312_current->trace_count-2u];
      const Nf18b2EncounterTrace *h313_second=
         &h312_current->traces[h312_current->trace_count-1u];
      CHECK(h313_first->intent.actor_id==10u&&
            h313_second->intent.actor_id==20u&&
            h313_first->authorization.status==h312_first&&
            h313_second->authorization.status==h312_second);
      CHECK(h313_first->authorization.reason<=NF18B2_REPLAY&&
            h313_second->authorization.reason<=NF18B2_REPLAY);
      fprintf(stderr,
        "H313 B2_REASON case=%u actor10_status=%d actor10_reason=%d actor10_physical=%d actor10_contract=%d actor20_status=%d actor20_reason=%d actor20_physical=%d actor20_contract=%d preserved=1\n",
        h311_case,(int)h312_first,
        (int)h313_first->authorization.reason,
        (int)h313_first->authorization.physical,
        (int)h313_first->authorization.contract,
        (int)h312_second,(int)h313_second->authorization.reason,
        (int)h313_second->authorization.physical,
        (int)h313_second->authorization.contract);
      /* H314: SOURCE-EXACT BODY_BLOCKED jurisdiction split.
         The B2 evaluator defines BODY_BLOCKED as either A3 capsule
         interference or A5 solid occupancy. Recompute BOTH for each
         actual actor at each H311 positive A4 contact pose. No rule
         relaxation, fabricated B2 grant, B3 effect or A6 publication. */
      for(unsigned h314_actor=0u;h314_actor<2u;++h314_actor){
        const Nf18b2Scene *scene=&h312_current->frame.scene;
        const uint32_t actor_id=h314_actor?20u:10u;
        const Nf18b2Actor *source_actor=NULL;
        for(size_t h314_j=0u;h314_j<scene->actor_count;++h314_j)
          if(scene->actors[h314_j].id==actor_id)
            source_actor=&scene->actors[h314_j];
        CHECK(source_actor&&scene->material);
        const Nf18a2ShapePolicy h314_shape={
          source_actor->radius,source_actor->height,0.f,0.f,1.f};
        const bool h314_a3_clear=nf18a3_capsule_clear(
          scene->geometry,source_actor->feet,h314_shape);
        const Nf18a5Query h314_a5=nf18a5_sweep_query(
          &scene->material->grid,source_actor->feet,
          (NfVec3){0.f,0.f,0.f},h314_shape,1,
          scene->geometry.tick,NULL,NULL,NULL);
        const bool h314_body_blocked=
          !h314_a3_clear||h314_a5==NF18A5_Q_SOLID;
        CHECK((h314_actor==0u&&!h314_body_blocked)||
              (h314_actor==1u&&h314_body_blocked));
        CHECK(w->tick==h311_tick_before&&
              nf_retc_read(publisher)==h311_frame_before&&
              push->receipt_count==h311_receipts_before);
        fprintf(stderr,
          "H314 BODY_COMPONENT case=%u actor=%u a3_clear=%u a5_query=%u blocked=%u preserved=1\n",
          2u*h311_case+h314_actor,actor_id,
          (unsigned)h314_a3_clear,(unsigned)h314_a5,
          (unsigned)h314_body_blocked);
      }
      const unsigned h312_both_eligible=
             h312_first==NF18B2_ELIGIBLE&&
             h312_second==NF18B2_ELIGIBLE;
      NfRetcMultiPlan h312_plan={0};
      if(h312_both_eligible){
        CHECK(h312_current->queue_count>=2u);
        CHECK(nf_retc_multi_plan_two(h312_current,
              h312_current->queue_count-2u,
              h312_current->queue_count-1u,&h312_plan));
        CHECK(h312_plan.prepared&&h312_plan.actor_ids[0]==10u&&
              h312_plan.actor_ids[1]==20u&&
              h312_plan.object_id==2010u);
      }else{
        CHECK(!h312_plan.prepared);
      }
      CHECK(nf_retc_ma03_recheck_physics(w,r,.125f,&same_source));
      CHECK(w->tick==h311_tick_before&&
            nf_retc_read(publisher)==h311_frame_before&&
            push->receipt_count==h311_receipts_before);
      fprintf(stderr,
        "H312 CURRENT_COLLISION_B2 case=%u first=%d second=%d both=%u source=1 preserved=1\n",
        h311_case,(int)h312_first,(int)h312_second,h312_both_eligible);
      free(h312_current);
      CHECK(!nf_retc_multi_source_sealed_preflight(
         w,r,enc,publisher,first_index,old_count,&batch));
      CHECK(w->tick==h311_tick_before&&
            nf_retc_read(publisher)==h311_frame_before&&
            push->receipt_count==h311_receipts_before);
      fprintf(stderr,
        "H311 SAME_WORLD_A4_CONTACT case=%u actors=10,20 source=1 stale_b2_denied=1\n",
        h311_case);
      h311_owner_actor->transform=h311_original_primary;
      h311_coactor->transform=h311_original_second;
      r->physical.actor=h311_original_cached;
      CHECK(nf_retc_multi_source_sealed_preflight(
          w,r,enc,publisher,first_index,old_count,&batch));
      CHECK(w->tick==h311_tick_before&&
            nf_retc_read(publisher)==h311_frame_before&&
            push->receipt_count==h311_receipts_before);
      fprintf(stderr,"H311 SOURCE_RESTORE case=%u b2_valid=1 preserved=1\n",
                    h311_case);
    }
  }
  fprintf(stderr,
     "H311 SAME_WORLD_PHYSICAL PASS 10 positive actor10/20 contacts + 10 stale-B2 denials + restores\n");
  /* H315: search ten genuine same-A6 owner collision configurations
     LATERALLY AWAY from the A3 crate obstruction, without modifying
     the A3, A5, B2 rules or authorizing/publishing a transaction.
     Record actual stage decisions even if the positive intersection is
     empty; each independent observation is falsifiable and source-derived. */
  {
    NfActor *p315=nf_world_find_actor(w,10u);
    NfActor *q315=nf_world_find_actor(w,20u);
    CHECK(p315&&q315);
    const NfTransform original_p315=p315->transform;
    const NfTransform original_q315=q315->transform;
    const Nf18a4Body original_a4_315=r->physical.actor;
    const uint64_t tick315=w->tick;
    const NfRetcFrame *frame315=nf_retc_read(publisher);
    const uint32_t receipts315=push->receipt_count;
    unsigned intersection315=0u;
    for(unsigned case315=0u;case315<10u;++case315){
      const float lateral=1.1f+.15f*(float)case315;
      p315->transform.position.z=original_p315.position.z+lateral;
      q315->transform.position=(NfVec3){
        p315->transform.position.x+.80f,
        p315->transform.position.y,
        p315->transform.position.z};
      p315->transform.velocity=(NfVec3){4.5f,0.f,0.f};
      q315->transform.velocity=(NfVec3){0.f,0.f,0.f};
      r->physical.actor.center=(NfVec3){
        p315->transform.position.x,
        p315->transform.position.y+p315->movement.body_height*.5f,
        p315->transform.position.z};
      r->physical.actor.velocity=p315->transform.velocity;
      r->physical.actor.height=p315->movement.body_height;
      NfRetcMa03PhysicalCandidate physical315={0};
      const bool a4_315=nf_retc_ma03_stage_physics(w,r,.125f,&physical315);
      if(a4_315)CHECK(physical315.eligible&&
                        physical315.reference.matched==1u&&
                        physical315.reference.disagreed==0u&&
                        nf_retc_ma03_recheck_physics(w,r,.125f,&physical315));
      Nf18b2EncounterRuntime *scratch315=calloc(1,sizeof *scratch315);
      CHECK(scratch315);
      *scratch315=*enc;
      scratch315->queue_count=0u;scratch315->trace_count=0u;
      scratch315->grant_count=0u;scratch315->prepared=0u;
      scratch315->attached=0u;
      scratch315->frame.scene.actors=scratch315->frame.actors;
      scratch315->frame.scene.objects=scratch315->frame.objects;
      CHECK(nf18b2_live_frame(scratch315,&scratch315->frame));
      const Nf18b2Status b10=nf18b2_propose(scratch315,
          (Nf18b2Intent){10u,2010u,1100u+case315,NF18B2_PUSH});
      const Nf18b2Status b20=nf18b2_propose(scratch315,
          (Nf18b2Intent){20u,2010u,1200u+case315,NF18B2_PUSH});
      CHECK(scratch315->trace_count>=2u);
      const Nf18b2Reason reason10=scratch315->traces[
          scratch315->trace_count-2u].authorization.reason;
      const Nf18b2Reason reason20=scratch315->traces[
          scratch315->trace_count-1u].authorization.reason;
      /* H316: independent ray/AABB oracle reproduces the full A3
         visibility jurisdiction; A5 line clearance queried separately.
         Exact B2 source actors and unchanged object target are used.
         This is not a new permission or authorized B3 transaction. */
      const Nf18b2Scene *scene316=&scratch315->frame.scene;
      const Nf18b2Object *obj316=NULL;
      for(size_t oi316=0;oi316<scene316->object_count;++oi316)
        if(scene316->objects[oi316].id==2010u)
          obj316=&scene316->objects[oi316];
      CHECK(obj316&&scene316->material);
      for(unsigned a316=0u;a316<2u;++a316){
        const uint32_t id316=a316?20u:10u;
        const Nf18b2Actor *actor316=NULL;
        for(size_t ai316=0;ai316<scene316->actor_count;++ai316)
          if(scene316->actors[ai316].id==id316)
            actor316=&scene316->actors[ai316];
        CHECK(actor316);
        const NfVec3 eye316={actor316->feet.x,
           actor316->feet.y+actor316->eye_height,actor316->feet.z};
        const NfVec3 end316=obj316->target;
        const NfVec3 delta316={end316.x-eye316.x,
            end316.y-eye316.y,end316.z-eye316.z};
        unsigned a3_hits316=0u;
        uint32_t first_hit316=0u;
        for(size_t coll316=0;coll316<scene316->geometry.solid_count;++coll316){
          const Nf18aCollider *c316=&scene316->geometry.solids[coll316];
          double near316=0.,far316=1.;
          const double start316[3]={eye316.x,eye316.y,eye316.z};
          const double endaxis316[3]={end316.x,end316.y,end316.z};
          const double lo316[3]={c316->min.x,c316->min.y,c316->min.z};
          const double hi316[3]={c316->max.x,c316->max.y,c316->max.z};
          bool hit316=true;
          for(unsigned axis316=0u;axis316<3u;++axis316){
            const double move316=endaxis316[axis316]-start316[axis316];
            if(fabs(move316)<1e-12){
              if(start316[axis316]<lo316[axis316]||
                 start316[axis316]>hi316[axis316])hit316=false;
            }else{
              double enter316=(lo316[axis316]-start316[axis316])/move316;
              double leave316=(hi316[axis316]-start316[axis316])/move316;
              if(enter316>leave316){
                const double swap316=enter316;enter316=leave316;
                leave316=swap316;
              }
              if(enter316>near316)near316=enter316;
              if(leave316<far316)far316=leave316;
              if(near316>far316)hit316=false;
            }
            if(!hit316)break;
          }
          if(hit316&&near316<1.-1e-6&&far316>=0.){
            if(!a3_hits316)first_hit316=c316->body_id;
            ++a3_hits316;
          }
        }
        const Nf18a2ShapePolicy ray316={.0001f,.0002f,0.f,0.f,1.f};
        const NfVec3 rayfeet316={eye316.x,eye316.y-.0001f,eye316.z};
        const Nf18a5Query a5ray316=nf18a5_sweep_query(
           &scene316->material->grid,rayfeet316,delta316,
           ray316,1,scene316->geometry.tick,NULL,NULL,NULL);
        const Nf18b2Reason reason316=a316?reason20:reason10;
        const unsigned occluded316=(unsigned)(
           a3_hits316>0u||a5ray316==NF18A5_Q_SOLID);
        CHECK((reason316==NF18B2_OCCLUDED)==(occluded316!=0u));
        CHECK(w->tick==tick315&&nf_retc_read(publisher)==frame315&&
              push->receipt_count==receipts315);
        fprintf(stderr,
           "H316 VISIBILITY case=%u actor=%u a3_hits=%u first_hit=%u a5_ray=%u occluded=%u source_reason=%u preserved=1\n",
           2u*case315+a316,id316,a3_hits316,first_hit316,
           (unsigned)a5ray316,occluded316,(unsigned)reason316);
      }
      unsigned a3clear[2]={0u,0u};
      unsigned a5free[2]={0u,0u};
      for(size_t t315=0;t315<2u;++t315){
        const Nf18b2Scene *scene=&scratch315->frame.scene;
        const uint32_t aid=t315?20u:10u;
        const Nf18b2Actor *src=NULL;
        for(size_t j315=0;j315<scene->actor_count;++j315)
          if(scene->actors[j315].id==aid)src=&scene->actors[j315];
        CHECK(src&&scene->material);
        const Nf18a2ShapePolicy shape315={
          src->radius,src->height,0.f,0.f,1.f};
        a3clear[t315]=(unsigned)nf18a3_capsule_clear(
          scene->geometry,src->feet,shape315);
        a5free[t315]=(unsigned)(nf18a5_sweep_query(
          &scene->material->grid,src->feet,
          (NfVec3){0.f,0.f,0.f},shape315,1,
          scene->geometry.tick,NULL,NULL,NULL)==NF18A5_Q_FREE);
      }
      const unsigned joint315=(unsigned)(a4_315&&
          b10==NF18B2_ELIGIBLE&&b20==NF18B2_ELIGIBLE&&
          a3clear[0]&&a3clear[1]&&a5free[0]&&a5free[1]);
      intersection315+=joint315;
      CHECK(w->tick==tick315&&
            nf_retc_read(publisher)==frame315&&
            push->receipt_count==receipts315);
      fprintf(stderr,
        "H315 LATERAL_SEARCH case=%u z_offset_mm=%u a4=%u b10=%d b20=%d reason10=%d reason20=%d a3_10=%u a3_20=%u a5_10=%u a5_20=%u joint=%u preserved=1\n",
        case315,(unsigned)(lateral*1000.f+.5f),(unsigned)a4_315,
        (int)b10,(int)b20,(int)reason10,(int)reason20,
        a3clear[0],a3clear[1],a5free[0],a5free[1],joint315);
      free(scratch315);
      p315->transform=original_p315;
      q315->transform=original_q315;
      r->physical.actor=original_a4_315;
      CHECK(nf_retc_multi_source_sealed_preflight(
         w,r,enc,publisher,first_index,old_count,&batch));
    }
    CHECK(w->tick==tick315&&nf_retc_read(publisher)==frame315&&
          push->receipt_count==receipts315);
    fprintf(stderr,
       "H315 LATERAL_SEARCH_COMPLETE cases=10 joint_positive=%u source_restored=1\n",
       intersection315);
  }
  /* H317: opposite-X real-world approach, ten positive-or-negative
     configurations. Retain the same object target and A3 collider.
     Never synthesize B2 grants or commit an A6 transaction. */
  {
    NfActor *p317=nf_world_find_actor(w,10u);
    NfActor *q317=nf_world_find_actor(w,20u);
    CHECK(p317&&q317);
    const NfTransform p0_317=p317->transform,q0_317=q317->transform;
    const Nf18a4Body cached317=r->physical.actor;
    const uint64_t tick317=w->tick;
    const NfRetcFrame *accepted317=nf_retc_read(publisher);
    const uint32_t receipts317=push->receipt_count;
    unsigned joint317=0u;
    for(unsigned k317=0u;k317<10u;++k317){
      const float z317=1.10f+.15f*(float)k317;
      p317->transform.position.z=p0_317.position.z+z317;
      q317->transform.position=(NfVec3){
        p317->transform.position.x-.80f,
        p317->transform.position.y,p317->transform.position.z};
      p317->transform.velocity=(NfVec3){-4.5f,0.f,0.f};
      q317->transform.velocity=(NfVec3){0.f,0.f,0.f};
      r->physical.actor.center=(NfVec3){
        p317->transform.position.x,
        p317->transform.position.y+p317->movement.body_height*.5f,
        p317->transform.position.z};
      r->physical.actor.velocity=p317->transform.velocity;
      r->physical.actor.height=p317->movement.body_height;
      NfRetcMa03PhysicalCandidate a4_317={0};
      const bool contact317=nf_retc_ma03_stage_physics(w,r,.125f,&a4_317);
      if(contact317)CHECK(a4_317.eligible&&
        a4_317.reference.matched==1u&&
        a4_317.reference.disagreed==0u&&
        nf_retc_ma03_recheck_physics(w,r,.125f,&a4_317));
      Nf18b2EncounterRuntime *b317=calloc(1,sizeof *b317);
      CHECK(b317);
      *b317=*enc;
      b317->queue_count=0u;b317->trace_count=0u;
      b317->grant_count=0u;b317->prepared=0u;b317->attached=0u;
      b317->frame.scene.actors=b317->frame.actors;
      b317->frame.scene.objects=b317->frame.objects;
      CHECK(nf18b2_live_frame(b317,&b317->frame));
      const Nf18b2Status first317=nf18b2_propose(b317,
        (Nf18b2Intent){10u,2010u,1310u+k317,NF18B2_PUSH});
      const Nf18b2Status second317=nf18b2_propose(b317,
        (Nf18b2Intent){20u,2010u,1320u+k317,NF18B2_PUSH});
      CHECK(b317->trace_count>=2u);
      const Nf18b2Reason reasonfirst317=b317->traces[
         b317->trace_count-2u].authorization.reason;
      const Nf18b2Reason reasonsecond317=b317->traces[
         b317->trace_count-1u].authorization.reason;
      /* H318: source-exact reverse-side stage attribution.
         The previously observed PENDING decision can arise from the
         A5 body capsule or the finite swept visibility ray. Record
         both using the unmodified B2 scene, and record actual A4
         relative pose/velocity rather than inferring contact. */
      const Nf18b2Scene *scene318=&b317->frame.scene;
      const Nf18b2Actor *actor318=NULL;
      const Nf18b2Object *target318=NULL;
      for(size_t ai318=0;ai318<scene318->actor_count;++ai318)
        if(scene318->actors[ai318].id==20u)
          actor318=&scene318->actors[ai318];
      for(size_t oi318=0;oi318<scene318->object_count;++oi318)
        if(scene318->objects[oi318].id==2010u)
          target318=&scene318->objects[oi318];
      CHECK(actor318&&target318&&scene318->material);
      const Nf18a2ShapePolicy bodyshape318={
        actor318->radius,actor318->height,0.f,0.f,1.f};
      const Nf18a5Query body318=nf18a5_sweep_query(
        &scene318->material->grid,actor318->feet,
        (NfVec3){0.f,0.f,0.f},bodyshape318,1,
        scene318->geometry.tick,NULL,NULL,NULL);
      const NfVec3 eye318={actor318->feet.x,
         actor318->feet.y+actor318->eye_height,actor318->feet.z};
      const NfVec3 ray318={target318->target.x-eye318.x,
         target318->target.y-eye318.y,
         target318->target.z-eye318.z};
      const Nf18a2ShapePolicy rayshape318={.0001f,.0002f,0.f,0.f,1.f};
      const NfVec3 rayfeet318={eye318.x,eye318.y-.0001f,eye318.z};
      const Nf18a5Query visibility318=nf18a5_sweep_query(
        &scene318->material->grid,rayfeet318,ray318,
        rayshape318,1,scene318->geometry.tick,NULL,NULL,NULL);
      const float dx318=q317->transform.position.x-
                           p317->transform.position.x;
      const float dv318=p317->transform.velocity.x-
                           q317->transform.velocity.x;
      const unsigned projected318=(unsigned)(dx318*dv318>0.f);
      CHECK(reasonsecond317!=NF18B2_MATERIAL_PENDING||
           body318==NF18A5_Q_PENDING||
           visibility318==NF18A5_Q_PENDING);
      CHECK(w->tick==tick317&&
            nf_retc_read(publisher)==accepted317&&
            push->receipt_count==receipts317);
      fprintf(stderr,
        "H318 REVERSE_SOURCE case=%u a4=%u dx_mm=%d dv_cms=%d projected_closing=%u body_a5=%u ray_a5=%u b2_reason=%u preserved=1\n",
        k317,(unsigned)contact317,
        (int)lroundf(dx318*1000.f),
        (int)lroundf(dv318*100.f),projected318,
        (unsigned)body318,(unsigned)visibility318,
        (unsigned)reasonsecond317);
      const unsigned jointcase317=(unsigned)(contact317&&
         first317==NF18B2_ELIGIBLE&&second317==NF18B2_ELIGIBLE);
      joint317+=jointcase317;
      CHECK(w->tick==tick317&&nf_retc_read(publisher)==accepted317&&
            push->receipt_count==receipts317);
      fprintf(stderr,
        "H317 OPPOSITE_APPROACH case=%u z_offset_mm=%u a4=%u b10=%d b20=%d reason10=%d reason20=%d joint=%u preserved=1\n",
        k317,(unsigned)(z317*1000.f+.5f),(unsigned)contact317,
        (int)first317,(int)second317,(int)reasonfirst317,
        (int)reasonsecond317,jointcase317);
      free(b317);
      p317->transform=p0_317;q317->transform=q0_317;
      r->physical.actor=cached317;
      CHECK(nf_retc_multi_source_sealed_preflight(
         w,r,enc,publisher,first_index,old_count,&batch));
    }
    CHECK(w->tick==tick317&&nf_retc_read(publisher)==accepted317&&
          push->receipt_count==receipts317);
    fprintf(stderr,
      "H317 OPPOSITE_APPROACH_COMPLETE cases=10 joint_positive=%u restored=1\n",
      joint317);
  }
  /* H319: no-policy-change, real-source two-actor feasibility grid.
     This does NOT prove two actor->object A4 contacts, issue owner-owned B2
     credentials, apply typed B3 or commit A6. A candidate is positive ONLY
     if A4's genuine same-world actor-pair solver and BOTH independent B2
     source requests accept the same unchanged source scene. */
  {
    NfActor *owner319=nf_world_find_actor(w,10u);
    NfActor *peer319=nf_world_find_actor(w,20u);
    CHECK(owner319&&peer319);
    const NfTransform save_owner319=owner319->transform;
    const NfTransform save_peer319=peer319->transform;
    const Nf18a4Body save_cached319=r->physical.actor;
    const uint64_t tick319=w->tick;
    const NfRetcFrame *frame319=nf_retc_read(publisher);
    const uint32_t b3count319=push->receipt_count;
    unsigned contact319=0u,grantpair319=0u,joint319=0u;
    unsigned reason_pending319=0u,reason_occluded319=0u;
    unsigned order_differences319=0u,reverse_dual319=0u;
    for(unsigned side319=0u;side319<2u;++side319){
      for(unsigned xi319=0u;xi319<4u;++xi319){
        for(unsigned zi319=0u;zi319<8u;++zi319){
          const unsigned case319=side319*32u+xi319*8u+zi319;
          const float side=(side319==0u)?1.f:-1.f;
          const float dx=(.65f+.10f*(float)xi319)*side;
          const float dz=.75f+.25f*(float)zi319;
          owner319->transform.position=save_owner319.position;
          owner319->transform.position.z+=dz;
          peer319->transform.position=(NfVec3){
            owner319->transform.position.x+dx,
            owner319->transform.position.y,
            owner319->transform.position.z};
          owner319->transform.velocity=(NfVec3){side*4.5f,0.f,0.f};
          peer319->transform.velocity=(NfVec3){0.f,0.f,0.f};
          r->physical.actor.center=(NfVec3){
            owner319->transform.position.x,
            owner319->transform.position.y+
                 owner319->movement.body_height*.5f,
            owner319->transform.position.z};
          r->physical.actor.velocity=owner319->transform.velocity;
          r->physical.actor.height=owner319->movement.body_height;
          NfRetcMa03PhysicalCandidate actual319={0};
          const bool a4=nf_retc_ma03_stage_physics(w,r,.125f,&actual319);
          if(a4){
            CHECK(actual319.eligible&&actual319.reference.matched==1u&&
                  actual319.reference.disagreed==0u&&
                  nf_retc_ma03_recheck_physics(w,r,.125f,&actual319));
            ++contact319;
          }
          Nf18b2EncounterRuntime *private319=calloc(1,sizeof *private319);
          CHECK(private319);
          *private319=*enc;
          private319->queue_count=0u;private319->trace_count=0u;
          private319->grant_count=0u;private319->prepared=0u;
          private319->attached=0u;
          private319->frame.scene.actors=private319->frame.actors;
          private319->frame.scene.objects=private319->frame.objects;
          CHECK(nf18b2_live_frame(private319,&private319->frame));
          const Nf18b2Status b10=nf18b2_propose(private319,
            (Nf18b2Intent){10u,2010u,3000u+case319,NF18B2_PUSH});
          const Nf18b2Status b20=nf18b2_propose(private319,
            (Nf18b2Intent){20u,2010u,3100u+case319,NF18B2_PUSH});
          CHECK(private319->trace_count>=2u);
          const Nf18b2Reason second_reason=private319->traces[
              private319->trace_count-1u].authorization.reason;
          /* Reverse proposal order in a distinct B2-private clone of the
             same real owner-world pose. Evaluate source decisions only.
             Never synthesize issuer grants or mutate the accepted frame. */
          Nf18b2EncounterRuntime *reverse319=calloc(1,sizeof *reverse319);
          CHECK(reverse319);
          *reverse319=*enc;
          reverse319->queue_count=0u;reverse319->trace_count=0u;
          reverse319->grant_count=0u;reverse319->prepared=0u;
          reverse319->attached=0u;
          reverse319->frame.scene.actors=reverse319->frame.actors;
          reverse319->frame.scene.objects=reverse319->frame.objects;
          CHECK(nf18b2_live_frame(reverse319,&reverse319->frame));
          const Nf18b2Status reverse20=nf18b2_propose(reverse319,
            (Nf18b2Intent){20u,2010u,3100u+case319,NF18B2_PUSH});
          const Nf18b2Status reverse10=nf18b2_propose(reverse319,
            (Nf18b2Intent){10u,2010u,3000u+case319,NF18B2_PUSH});
          CHECK(reverse319->trace_count>=2u);
          const Nf18b2Reason reason20reverse=reverse319->traces[
              reverse319->trace_count-2u].authorization.reason;
          const bool same319=b10==reverse10&&b20==reverse20&&
                            second_reason==reason20reverse;
          order_differences319+=(unsigned)!same319;
          reverse_dual319+=(unsigned)(reverse10==NF18B2_ELIGIBLE&&
                                     reverse20==NF18B2_ELIGIBLE);
          CHECK(w->tick==tick319&&
                nf_retc_read(publisher)==frame319&&
                push->receipt_count==b3count319);
          fprintf(stderr,
            "H319 REVERSE_ORDER case=%u forward10=%d reverse10=%d forward20=%d reverse20=%d reason20=%d reverse_reason20=%d same=%u preserved=1\n",
            case319,(int)b10,(int)reverse10,(int)b20,(int)reverse20,
            (int)second_reason,(int)reason20reverse,(unsigned)same319);
          free(reverse319);
          if(second_reason==NF18B2_OCCLUDED)++reason_occluded319;
          if(second_reason==NF18B2_MATERIAL_PENDING)++reason_pending319;
          const bool both=b10==NF18B2_ELIGIBLE&&b20==NF18B2_ELIGIBLE;
          grantpair319+=(unsigned)both;
          const bool intersection=a4&&both;
          joint319+=(unsigned)intersection;
          CHECK(w->tick==tick319&&
                nf_retc_read(publisher)==frame319&&
                push->receipt_count==b3count319);
          fprintf(stderr,
            "H319 SOURCE_GRID case=%u side=%u dx_mm=%d z_mm=%d a4=%u b10=%d b20=%d reason20=%d joint=%u preserved=1\n",
            case319,side319,(int)lroundf(dx*1000.f),
            (int)lroundf(dz*1000.f),(unsigned)a4,
            (int)b10,(int)b20,(int)second_reason,
            (unsigned)intersection);
          free(private319);
          owner319->transform=save_owner319;
          peer319->transform=save_peer319;
          r->physical.actor=save_cached319;
          CHECK(nf_retc_multi_source_sealed_preflight(
            w,r,enc,publisher,first_index,old_count,&batch));
        }
      }
    }
    CHECK(w->tick==tick319&&nf_retc_read(publisher)==frame319&&
          push->receipt_count==b3count319);
    fprintf(stderr,
      "H319 SOURCE_GRID_COMPLETE cases=64 a4=%u dual_b2=%u intersection=%u pending20=%u occluded20=%u order_differences=%u reverse_dual=%u restored=1\n",
      contact319,grantpair319,joint319,reason_pending319,
      reason_occluded319,order_differences319,reverse_dual319);
  }
  /* H319B: the 64-pose H319 grid observed B2 dual-eligible cases only
     when actor20 was at negative X and A4 was false. Test a physically
     distinct approach: actor20 advances toward stationary actor10. Do
     not change A3/A5 materials, B2 policy, source tick, or A6 publication.
     This remains actor-pair physical feasibility, NOT actor->object PUSH. */
  {
    NfActor *ownerB=nf_world_find_actor(w,10u);
    NfActor *peerB=nf_world_find_actor(w,20u);
    CHECK(ownerB&&peerB);
    const NfTransform oldOwnerB=ownerB->transform;
    const NfTransform oldPeerB=peerB->transform;
    const Nf18a4Body oldCachedB=r->physical.actor;
    const uint64_t tickB=w->tick;
    const NfRetcFrame *frameB=nf_retc_read(publisher);
    const uint32_t receiptsB=push->receipt_count;
    unsigned contactsB=0u,dualB=0u,jointB=0u;
    for(unsigned gapB=0u;gapB<4u;++gapB){
      for(unsigned lateralB=0u;lateralB<8u;++lateralB){
        const unsigned caseB=gapB*8u+lateralB;
        const float dxB=-.65f-.10f*(float)gapB;
        const float dzB=.75f+.25f*(float)lateralB;
        ownerB->transform.position=oldOwnerB.position;
        ownerB->transform.position.z+=dzB;
        peerB->transform.position=(NfVec3){
          ownerB->transform.position.x+dxB,
          ownerB->transform.position.y,
          ownerB->transform.position.z};
        ownerB->transform.velocity=(NfVec3){0.f,0.f,0.f};
        peerB->transform.velocity=(NfVec3){4.5f,0.f,0.f};
        r->physical.actor.center=(NfVec3){
          ownerB->transform.position.x,
          ownerB->transform.position.y+
             ownerB->movement.body_height*.5f,
          ownerB->transform.position.z};
        r->physical.actor.velocity=ownerB->transform.velocity;
        r->physical.actor.height=ownerB->movement.body_height;
        NfRetcMa03PhysicalCandidate physicalB={0};
        const bool contactB=nf_retc_ma03_stage_physics(w,r,.125f,&physicalB);
        if(contactB){
          CHECK(physicalB.eligible&&physicalB.reference.matched==1u&&
                physicalB.reference.disagreed==0u&&
                nf_retc_ma03_recheck_physics(w,r,.125f,&physicalB));
          ++contactsB;
        }
        Nf18b2EncounterRuntime *scratchB=calloc(1,sizeof *scratchB);
        CHECK(scratchB);
        *scratchB=*enc;
        scratchB->queue_count=0u;scratchB->trace_count=0u;
        scratchB->grant_count=0u;scratchB->prepared=0u;
        scratchB->attached=0u;
        scratchB->frame.scene.actors=scratchB->frame.actors;
        scratchB->frame.scene.objects=scratchB->frame.objects;
        CHECK(nf18b2_live_frame(scratchB,&scratchB->frame));
        const Nf18b2Status b10B=nf18b2_propose(scratchB,
          (Nf18b2Intent){10u,2010u,4100u+caseB,NF18B2_PUSH});
        const Nf18b2Status b20B=nf18b2_propose(scratchB,
          (Nf18b2Intent){20u,2010u,4200u+caseB,NF18B2_PUSH});
        CHECK(scratchB->trace_count>=2u);
        const Nf18b2Reason reasonB=scratchB->traces[
          scratchB->trace_count-1u].authorization.reason;
        const bool bothB=b10B==NF18B2_ELIGIBLE&&b20B==NF18B2_ELIGIBLE;
        dualB+=(unsigned)bothB;
        const bool positiveB=contactB&&bothB;
        jointB+=(unsigned)positiveB;
        CHECK(w->tick==tickB&&nf_retc_read(publisher)==frameB&&
              push->receipt_count==receiptsB);
        fprintf(stderr,
          "H319B PEER_APPROACH case=%u dx_mm=%d z_mm=%d a4=%u b10=%d b20=%d reason20=%d joint=%u preserved=1\n",
          caseB,(int)lroundf(dxB*1000.f),
          (int)lroundf(dzB*1000.f),(unsigned)contactB,
          (int)b10B,(int)b20B,(int)reasonB,(unsigned)positiveB);
        free(scratchB);
        ownerB->transform=oldOwnerB;
        peerB->transform=oldPeerB;
        r->physical.actor=oldCachedB;
        CHECK(nf_retc_multi_source_sealed_preflight(
            w,r,enc,publisher,first_index,old_count,&batch));
      }
    }
    CHECK(w->tick==tickB&&nf_retc_read(publisher)==frameB&&
          push->receipt_count==receiptsB);
    fprintf(stderr,
      "H319B PEER_APPROACH_COMPLETE cases=32 a4=%u dual_b2=%u intersection=%u restored=1\n",
      contactsB,dualB,jointB);
  }
  /* H319C: balanced bilateral closing from the previously B2-eligible
     negative-X side. A3/A5/B2 remain untouched. In contrast with H319B
     (peer-only moving), both source actors now close at 2.25 m/s each.
     No owner grant issuance, B3 realization, A6 tick or publication. */
  {
    NfActor *ownerC=nf_world_find_actor(w,10u);
    NfActor *peerC=nf_world_find_actor(w,20u);
    CHECK(ownerC&&peerC);
    const NfTransform oldOwnerC=ownerC->transform;
    const NfTransform oldPeerC=peerC->transform;
    const Nf18a4Body oldCachedC=r->physical.actor;
    const uint64_t tickC=w->tick;
    const NfRetcFrame *frameC=nf_retc_read(publisher);
    const uint32_t receiptsC=push->receipt_count;
    unsigned contactsC=0u,dualC=0u,jointC=0u;
    unsigned rawImpactsC=0u,matchedC=0u,rawDisagreeC=0u;
    unsigned reversedMatchedE=0u,reversedUnresolvedE=0u;
    unsigned reversedImpactsE=0u,reorderedParityE=0u;
    for(unsigned gapC=0u;gapC<4u;++gapC){
      for(unsigned lateralC=0u;lateralC<8u;++lateralC){
        const unsigned caseC=gapC*8u+lateralC;
        const float dxC=-.65f-.10f*(float)gapC;
        const float dzC=.75f+.25f*(float)lateralC;
        ownerC->transform.position=oldOwnerC.position;
        ownerC->transform.position.z+=dzC;
        peerC->transform.position=(NfVec3){
          ownerC->transform.position.x+dxC,
          ownerC->transform.position.y,
          ownerC->transform.position.z};
        ownerC->transform.velocity=(NfVec3){-2.25f,0.f,0.f};
        peerC->transform.velocity=(NfVec3){2.25f,0.f,0.f};
        r->physical.actor.center=(NfVec3){
          ownerC->transform.position.x,
          ownerC->transform.position.y+
              ownerC->movement.body_height*.5f,
          ownerC->transform.position.z};
        r->physical.actor.velocity=ownerC->transform.velocity;
        r->physical.actor.height=ownerC->movement.body_height;
        NfRetcMa03PhysicalCandidate physicalC={0};
        /* H319D root-cause instrumentation: capture a REAL world roster,
           reconstruct the same pair using source actor dimensions and A6
           cached physical owner, then execute the ORIGINAL raw A4 solver
           plus independent MA03c oracle. This is not fabricated contact. */
        NfMa03Roster sourceD={0};
        CHECK(nf_ma03_roster_capture(w,&sourceD)&&
              sourceD.count==2u&&sourceD.source_tick==w->tick);
        Nf18a4Body rawBodiesD[2]={{0},{0}};
        for(size_t bodyD=0u;bodyD<2u;++bodyD){
          const NfMa03Participant *participantD=&sourceD.actors[bodyD];
          const NfActor *actorD=nf_world_find_actor_const(
              w,participantD->id);
          CHECK(actorD&&actorD->active&&actorD->combat.alive);
          rawBodiesD[bodyD].id=participantD->id;
          rawBodiesD[bodyD].center=(NfVec3){
             participantD->position.x,
             participantD->position.y+actorD->movement.body_height*.5f,
             participantD->position.z};
          rawBodiesD[bodyD].velocity=participantD->velocity;
          rawBodiesD[bodyD].inverse_mass=1.f/80.f;
          rawBodiesD[bodyD].radius=r->physical.actor.radius;
          rawBodiesD[bodyD].height=actorD->movement.body_height;
        }
        const NfMa01c2Result rawD=nf_ma01c2_integrate(
            rawBodiesD,2u,.125f,(uint32_t)w->tick+1u,1u);
        const NfMa03cEvidence oracleD=nf_ma03c_compare(
            rawBodiesD,2u,.125f,(uint32_t)w->tick+1u,1u);
        rawImpactsC+=(unsigned)(rawD.impacts==1u);
        matchedC+=(unsigned)(oracleD.matched==1u);
        rawDisagreeC+=(unsigned)(oracleD.disagreed!=0u||
                                   oracleD.unresolved!=0u);
        /* H319E source exact mirror trial: the independent 1D MA03c
           oracle only supports a body0-left / body1-right approach.
           Swap the two original, A6-sourced bodies in a PRIVATE copy,
           without mutating live actor identity, collision policy, B2,
           the production stage, or applying any effects. Verify whether
           that ordering restores a reference match. */
        Nf18a4Body reversedBodiesE[2]={rawBodiesD[1],rawBodiesD[0]};
        const NfMa01c2Result reversedRawE=nf_ma01c2_integrate(
            reversedBodiesE,2u,.125f,(uint32_t)w->tick+1u,1u);
        const NfMa03cEvidence reversedOracleE=nf_ma03c_compare(
            reversedBodiesE,2u,.125f,(uint32_t)w->tick+1u,1u);
        reversedMatchedE+=(unsigned)(reversedOracleE.matched==1u);
        reversedUnresolvedE+=(unsigned)(reversedOracleE.unresolved!=0u);
        reversedImpactsE+=(unsigned)(reversedRawE.impacts==1u);
        const bool impactParityE=rawD.impacts==reversedRawE.impacts;
        reorderedParityE+=(unsigned)impactParityE;
        CHECK(reversedBodiesE[0].id==rawBodiesD[1].id&&
              reversedBodiesE[1].id==rawBodiesD[0].id);
        fprintf(stderr,
          "H319E REORDERED case=%u forward_impacts=%u swapped_impacts=%u matched=%u disagreed=%u unresolved=%u impact_parity=%u preserved=1\n",
          caseC,(unsigned)rawD.impacts,(unsigned)reversedRawE.impacts,
          reversedOracleE.matched,reversedOracleE.disagreed,
          reversedOracleE.unresolved,(unsigned)impactParityE);
        /* H319F metamorphic guard: spatial-order normalization in the
           reference cannot alter actor identity or the actual raw solver.
           Compare private reverse/forward reference results and require
           owner stage to accept precisely fully validated raw contacts. */
        CHECK(oracleD.matched==reversedOracleE.matched&&
              oracleD.disagreed==reversedOracleE.disagreed&&
              oracleD.unresolved==reversedOracleE.unresolved);
        const bool contactC=nf_retc_ma03_stage_physics(w,r,.125f,&physicalC);
        CHECK(contactC==(rawD.status==NF_NARROW_OK&&
              rawD.count==2u&&rawD.impacts==1u&&
              oracleD.matched==1u&&oracleD.disagreed==0u&&
              oracleD.unresolved==0u));
        fprintf(stderr,
          "H319F REFERENCE_ORDER case=%u forward_match=%u reverse_match=%u stage=%u raw_impact=%u equivalent=1 preserved=1\n",
          caseC,oracleD.matched,reversedOracleE.matched,
          (unsigned)contactC,(unsigned)rawD.impacts);
        if(contactC){
          CHECK(physicalC.eligible&&physicalC.reference.matched==1u&&
                physicalC.reference.disagreed==0u&&
                nf_retc_ma03_recheck_physics(w,r,.125f,&physicalC));
          ++contactsC;
        }
        Nf18b2EncounterRuntime *scratchC=calloc(1,sizeof *scratchC);
        CHECK(scratchC);
        *scratchC=*enc;
        scratchC->queue_count=0u;scratchC->trace_count=0u;
        scratchC->grant_count=0u;scratchC->prepared=0u;
        scratchC->attached=0u;
        scratchC->frame.scene.actors=scratchC->frame.actors;
        scratchC->frame.scene.objects=scratchC->frame.objects;
        CHECK(nf18b2_live_frame(scratchC,&scratchC->frame));
        const Nf18b2Status b10C=nf18b2_propose(scratchC,
          (Nf18b2Intent){10u,2010u,5100u+caseC,NF18B2_PUSH});
        const Nf18b2Status b20C=nf18b2_propose(scratchC,
          (Nf18b2Intent){20u,2010u,5200u+caseC,NF18B2_PUSH});
        CHECK(scratchC->trace_count>=2u);
        const Nf18b2Reason reasonC=scratchC->traces[
          scratchC->trace_count-1u].authorization.reason;
        CHECK(!contactC||(
           rawD.status==NF_NARROW_OK&&rawD.count==2u&&
           rawD.impacts==1u&&oracleD.matched==1u&&
           oracleD.disagreed==0u&&oracleD.unresolved==0u));
        fprintf(stderr,
          "H319D RAW_PAIR case=%u status=%d count=%u impacts=%u matched=%u disagreed=%u unresolved=%u stage=%u preserved=1\n",
          caseC,(int)rawD.status,(unsigned)rawD.count,
          (unsigned)rawD.impacts,oracleD.matched,oracleD.disagreed,
          oracleD.unresolved,(unsigned)contactC);
        const bool bothC=b10C==NF18B2_ELIGIBLE&&
                         b20C==NF18B2_ELIGIBLE;
        dualC+=(unsigned)bothC;
        const bool positiveC=contactC&&bothC;
        jointC+=(unsigned)positiveC;
        CHECK(w->tick==tickC&&nf_retc_read(publisher)==frameC&&
              push->receipt_count==receiptsC);
        fprintf(stderr,
          "H319C BALANCED_CLOSE case=%u dx_mm=%d z_mm=%d a4=%u b10=%d b20=%d reason20=%d joint=%u preserved=1\n",
          caseC,(int)lroundf(dxC*1000.f),
          (int)lroundf(dzC*1000.f),(unsigned)contactC,
          (int)b10C,(int)b20C,(int)reasonC,(unsigned)positiveC);
        free(scratchC);
        ownerC->transform=oldOwnerC;
        peerC->transform=oldPeerC;
        r->physical.actor=oldCachedC;
        CHECK(nf_retc_multi_source_sealed_preflight(
            w,r,enc,publisher,first_index,old_count,&batch));
      }
    }
    CHECK(w->tick==tickC&&nf_retc_read(publisher)==frameC&&
          push->receipt_count==receiptsC);
    fprintf(stderr,
      "H319C BALANCED_CLOSE_COMPLETE cases=32 a4=%u dual_b2=%u intersection=%u restored=1\n",
      contactsC,dualC,jointC);
    fprintf(stderr,
      "H319D RAW_PAIR_COMPLETE cases=32 raw_impacts=%u matched=%u disagree_or_unresolved=%u stage_a4=%u restored=1\n",
      rawImpactsC,matchedC,rawDisagreeC,contactsC);
    fprintf(stderr,
      "H319E REORDERED_COMPLETE cases=32 swapped_impacts=%u matched=%u unresolved=%u impact_parity=%u restored=1\n",
      reversedImpactsE,reversedMatchedE,reversedUnresolvedE,
      reorderedParityE);
    fprintf(stderr,
      "H319F REFERENCE_ORDER_COMPLETE cases=32 stage_a4=%u matched=%u equivalent=32 restored=1\n",
      contactsC,matchedC);
  }
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
   /* H306: A6-owned A4 capsule is a separate authoritative source
    * from the roster. A nonfinite owner center, velocity, capsule,
    * inverse mass, or angular speed must not escape fabsf comparisons.
    * 10 actual A6 owner fields x 4 invalid floating-point forms.
    * Restore the original bound source and recheck the real A4 result. */
   for(unsigned h306_case=0u;h306_case<40u;++h306_case){
     const unsigned dim=h306_case/4u;
     const unsigned variant=h306_case%4u;
     Nf18a4Body *bound=&two_owner->physical.actor;
     float *target=NULL;
     switch(dim){
       case 0:target=&bound->center.x;break;
       case 1:target=&bound->center.y;break;
       case 2:target=&bound->center.z;break;
       case 3:target=&bound->velocity.x;break;
       case 4:target=&bound->velocity.y;break;
       case 5:target=&bound->velocity.z;break;
       case 6:target=&bound->radius;break;
       case 7:target=&bound->height;break;
       case 8:target=&bound->inverse_mass;break;
       case 9:target=&bound->omega.x;break;
     }
     CHECK(target);
     const float original=*target;
     *target=variant==0u?NAN:
        variant==1u?-NAN:variant==2u?INFINITY:-INFINITY;
     NfRetcMa03PhysicalCandidate refused={0};
     const bool denied=!nf_retc_ma03_stage_physics(
                          two_world,two_owner,.125f,&refused);
     *target=original;
     CHECK(denied&&!refused.eligible&&
           nf_retc_ma03_recheck_physics(two_world,two_owner,.125f,&candidate)&&
           two_world->tick==source_tick&&
           two_owner->last_commit_tick==source_tick&&
           memcmp(&primary->transform.position,&first_start,
                  sizeof first_start)==0&&
           memcmp(&secondary->transform.position,&second_start,
                  sizeof second_start)==0&&
           nf_retc_read(publisher)==accepted&&push->receipt_count==2u);
     fprintf(stderr,
       "H306 A6_OWNER_A4_SOURCE_DENIAL case=%u field=%u variant=%u preserved=1\n",
       h306_case,dim,variant);
   }
   fprintf(stderr,
      "H306 A6_OWNER_A4_SOURCE PASS 40 owner-field refusals and 40 rechecks\n");
   /* H304 real A6-bound private 2-body source safety matrix:
    * 10 dimensions x 4 nonfinite forms; rejection must preserve
    * the prior valid candidate, A6 world tick and actor projections.
    * This is NOT a production coactor commit or B3 realization. */
   for(unsigned h304_case=0u;h304_case<40u;++h304_case){
     const unsigned dimension=h304_case/4u;
     const unsigned variant=h304_case%4u;
     const NfTransform prior=secondary->transform;
     const NfMoveInput prior_input=secondary->input;
     const float prior_height=secondary->movement.body_height;
     const float invalid=variant==0u?NAN:
       variant==1u?-NAN:variant==2u?INFINITY:-INFINITY;
     switch(dimension){
       case 0:secondary->transform.position.x=invalid;break;
       case 1:secondary->transform.position.y=invalid;break;
       case 2:secondary->transform.position.z=invalid;break;
       case 3:secondary->transform.velocity.x=invalid;break;
       case 4:secondary->transform.velocity.y=invalid;break;
       case 5:secondary->transform.velocity.z=invalid;break;
       case 6:secondary->input.forward=invalid;break;
       case 7:secondary->input.strafe=invalid;break;
       case 8:secondary->input.yaw_radians=invalid;break;
       case 9:secondary->movement.body_height=invalid;break;
     }
     NfRetcMa03PhysicalCandidate denied={0};
     const bool rejected=!nf_retc_ma03_stage_physics(
                                 two_world,two_owner,.125f,&denied);
     secondary->transform=prior;
     secondary->input=prior_input;
     secondary->movement.body_height=prior_height;
     CHECK(rejected&&!denied.eligible&&
           nf_retc_ma03_recheck_physics(two_world,two_owner,.125f,&candidate)&&
           two_world->tick==source_tick&&
           two_owner->last_commit_tick==source_tick&&
           memcmp(&secondary->transform.position,&second_start,
                  sizeof second_start)==0);
     fprintf(stderr,
       "H304 TWO_BODY_SOURCE_DENIAL case=%u dim=%u variant=%u original_pass=1\n",
       h304_case,dimension,variant);
   }
   fprintf(stderr,"H304 TWO_BODY_STAGE PASS 40 nonfinite source denials\n");
   /* H289: 40 independent signed perturbations across 20 actual
    * A6-roster / solved-A4 dimensions. Each private candidate mutation
    * must fail recheck, while the original stays source-valid.
    * This is NOT a second actor authoritative commit or gameplay tick. */
   for(unsigned h289_case=0u;h289_case<40u;++h289_case){
     NfRetcMa03PhysicalCandidate tampered=candidate;
     const unsigned field=h289_case/2u;
     float *target=NULL;
     NfMa03Participant *source=&tampered.source.actors[field<9u?0u:1u];
     switch(field%9u){
       case 0u: target=&source->position.x;break;
       case 1u: target=&source->position.y;break;
       case 2u: target=&source->position.z;break;
       case 3u: target=&source->velocity.x;break;
       case 4u: target=&source->velocity.y;break;
       case 5u: target=&source->velocity.z;break;
       case 6u: target=&source->input.forward;break;
       case 7u: target=&source->input.strafe;break;
       case 8u: target=&source->input.yaw_radians;break;
     }
     if(field==18u)target=&tampered.solved.bodies[0].center.z;
     if(field==19u)target=&tampered.solved.bodies[1].velocity.z;
     CHECK(target);
     *target+=(h289_case%2u==0u?0.125f:-0.125f);
     CHECK(!nf_retc_ma03_recheck_physics(two_world,two_owner,.125f,
                                         &tampered));
     CHECK(nf_retc_ma03_recheck_physics(two_world,two_owner,.125f,
                                        &candidate));
     CHECK(two_world->tick==source_tick&&
           two_owner->last_commit_tick==source_tick);
     fprintf(stderr,"H289 SOURCE_RECHECK_REJECT case=%u field=%u sign=%u original_pass=1\n",
             h289_case,field,h289_case%2u);
   }
   fprintf(stderr,"H289 TWO_ACTOR_MATRIX PASS private_mutations=40 denied=40 original_replays=40\n");
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
   /* H305: previously the two actual A4 actor trajectories and the A5
      static swept-material gate lived in separate research acceptance paths.
      Bind both to the SAME A6 owner / roster and current A5 material epoch
      before any PRIVATE contact plan is admitted. No A6 tick is published. */
   NfRetcMa03A5Envelope h305_handoff={0};
   CHECK(nf_retc_ma03_a5_prepare(two_world,two_owner,.125f,
                                 &candidate,&h305_handoff));
   NfRetcMa03PairSweepWitness h305_original={0};
   CHECK(nf_retc_ma03_pair_swept_clearance(two_world,two_owner,.125f,
                              &candidate,&h305_handoff,&h305_original));
   CHECK(h305_original.prepared&&h305_original.actor_ids[0]==30u&&
         h305_original.actor_ids[1]==40u&&
         h305_original.query[0]==NF18A5_Q_FREE&&
         h305_original.query[1]==NF18A5_Q_FREE);
   /* H307 cross-branch source binding: these are independently real
    * but DIFFERENT B2 and physical research scenes (10/20 vs 30/40).
    * Fail closed rather than laundering their evidence together. */
   CHECK(!nf_retc_h307_structural_pair_match(
      &batch,&candidate,&h305_handoff,&h305_original));
   /* A synthetic B3 proposal is used ONLY to prove the necessary
    * structural predicate has a positive and independently falsifiable
    * control. It is NOT an authentic B2 grant or an authorized event. */
   NfRetcCanonicalBatch h307_structural_control=batch;
   h307_structural_control.tick=h305_original.tick;
   for(unsigned h307_actor=0;h307_actor<2u;++h307_actor){
      Nf18b3Event *e=&h307_structural_control.events[h307_actor];
      e->actor=candidate.source.actors[h307_actor].id;
      e->tick=h305_original.tick;
      e->world_epoch=h305_handoff.source_revision;
      e->material_epoch=h305_handoff.material_epoch;
      e->contract_epoch=h307_structural_control.contract_epoch;
   }
   CHECK(nf_retc_h307_structural_pair_match(
       &h307_structural_control,&candidate,&h305_handoff,&h305_original));
   /* H308: 12 actual reciprocal A4 receipt fields x 2 bad variants.
    * A caller-supplied proposal matched to actors/epochs is insufficient
    * if the accepted A4 contact receipt was altered. Structurally valid
    * synthetic B3 control remains UNAUTHORIZED; no A6 tick can publish. */
   for(unsigned h308_case=0;h308_case<24u;++h308_case){
     NfRetcMa03PhysicalCandidate altered=candidate;
     Nf18a4Receipt *receipt=&altered.solved.receipts[0];
     const unsigned field=h308_case/2u,variant=h308_case%2u;
     switch(field){
       case 0:receipt->tick+=variant+1u;break;
       case 1:receipt->world_version+=variant+1u;break;
       case 2:receipt->status=variant?NF18A4_INVALID:NF18A4_SEPARATING;break;
       case 3:receipt->impulses_applied=0u;break;
       case 4:receipt->contact_id=0u;break;
       case 5:receipt->normal_impulse.x=variant?INFINITY:NAN;break;
       case 6:receipt->normal_impulse.y=variant?INFINITY:NAN;break;
       case 7:receipt->normal_impulse.z=variant?INFINITY:NAN;break;
       case 8:receipt->tangent_impulse.x=variant?INFINITY:NAN;break;
       case 9:receipt->tangent_impulse.y=variant?INFINITY:NAN;break;
       case 10:receipt->tangent_impulse.z=variant?INFINITY:NAN;break;
       case 11:receipt->signed_energy_delta=variant?INFINITY:NAN;break;
     }
     CHECK(!nf_retc_h307_structural_pair_match(
        &h307_structural_control,&altered,&h305_handoff,&h305_original));
     CHECK(nf_retc_h307_structural_pair_match(
        &h307_structural_control,&candidate,&h305_handoff,&h305_original));
     CHECK(!nf_retc_h307_structural_pair_match(
        &batch,&candidate,&h305_handoff,&h305_original));
     CHECK(two_world->tick==source_tick&&nf_retc_read(publisher)==accepted&&
           push->receipt_count==2u);
     fprintf(stderr,
       "H308 A4_RECEIPT_DENIAL case=%u field=%u variant=%u preserved=1\n",
       h308_case,field,variant);
   }
   fprintf(stderr,
      "H308 A4_RECEIPT_LINEAGE PASS 24 refusals + restored structural controls\n");
   for(unsigned h307_case=0;h307_case<40u;++h307_case){
      NfRetcCanonicalBatch forged=h307_structural_control;
      NfRetcMa03PairSweepWitness forged_sweep=h305_original;
      const unsigned field=h307_case/4u;
      const uint32_t delta=h307_case%4u+1u;
      switch(field){
       case 0:forged.events[0].actor+=delta;break;
       case 1:forged.events[1].actor+=delta;break;
       case 2:forged.events[0].world_epoch+=delta;break;
       case 3:forged.events[1].world_epoch+=delta;break;
       case 4:forged.events[0].material_epoch+=delta;break;
       case 5:forged.events[1].material_epoch+=delta;break;
       case 6:forged.events[0].tick+=delta;break;
       case 7:forged.events[1].tick+=delta;break;
       case 8:forged_sweep.actor_ids[0]+=delta;break;
       case 9:forged_sweep.material_epoch+=delta;break;
      }
      CHECK(!nf_retc_h307_structural_pair_match(
          &forged,&candidate,&h305_handoff,&forged_sweep));
      CHECK(nf_retc_h307_structural_pair_match(
          &h307_structural_control,&candidate,&h305_handoff,&h305_original));
      CHECK(!nf_retc_h307_structural_pair_match(
          &batch,&candidate,&h305_handoff,&h305_original));
      CHECK(two_world->tick==source_tick&&
            nf_retc_read(publisher)==accepted&&
            push->receipt_count==2u);
      fprintf(stderr,
          "H307 CROSS_SOURCE_REJECT case=%u field=%u delta=%u preserved=1\n",
          h307_case,field,delta);
   }
   fprintf(stderr,"H307 SOURCE_BINDING PASS 40 denials with structural positive and real-source mismatch\n");
   const uint32_t h305_original_cache=two_owner->physical.world.grid.cache_revision;
   for(unsigned h305_case=0;h305_case<40u;++h305_case){
     const uint32_t old_epoch=two_owner->physical.world.grid.global_revision;
     const uint32_t old_raw=two_owner->physical.world.history.raw_samples;
     const uint32_t old_b3=push->receipt_count;
     NfRetcMa03PairSweepWitness witnessed=h305_original;
     bool admitted=false;
     if(h305_case<20u){
       /* Twenty separate cache-only generations: cache bookkeeping
          cannot silently change material authority or swept clearance. */
       two_owner->physical.world.grid.cache_revision=
                      h305_original_cache+h305_case+1u;
       admitted=nf_retc_ma03_pair_swept_clearance(
          two_world,two_owner,.125f,&candidate,&h305_handoff,&witnessed);
       CHECK(admitted&&witnessed.prepared&&
             witnessed.query[0]==NF18A5_Q_FREE&&
             witnessed.query[1]==NF18A5_Q_FREE);
       two_owner->physical.world.grid.cache_revision=h305_original_cache;
     }else{
       /* Twenty denied alterations: each of four actually resident
          canonical chunks is made SOLID or unresolved MIXED. */
       const unsigned variant=h305_case-20u;
       const unsigned chunk_id=variant%4u;
       Nf18a5Chunk *chunk=&two_owner->physical.world.grid.chunks[chunk_id];
       const Nf18a5Chunk old_chunk=*chunk;
       memset(chunk->canonical,variant<10u?NF18A5_SOLID:NF18A5_MIXED,
              sizeof chunk->canonical);
       chunk->fine_valid=0u;
       admitted=nf_retc_ma03_pair_swept_clearance(
          two_world,two_owner,.125f,&candidate,&h305_handoff,&witnessed);
       *chunk=old_chunk;
       CHECK(!admitted&&
             memcmp(&witnessed,&h305_original,sizeof witnessed)==0);
     }
     CHECK(nf_retc_ma03_pair_swept_clearance(
         two_world,two_owner,.125f,&candidate,&h305_handoff,&witnessed));
     CHECK(two_owner->physical.world.grid.global_revision==old_epoch&&
           two_owner->physical.world.history.raw_samples==old_raw&&
           push->receipt_count==old_b3&&two_world->tick==source_tick&&
           nf_retc_read(publisher)==accepted);
     fprintf(stderr,
       "H305 A4_A5_DUAL_SWEEP case=%u kind=%s admitted=%u source_unchanged=1\n",
       h305_case,h305_case<20u?"cache-free":"blocked",(unsigned)admitted);
   }
   fprintf(stderr,
       "H305 A4_A5_PAIR PASS 20 cache-free + 20 blocked private sweeps\n");
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
   /* H291: real A6 owner callback must reject 40 independently altered
      source Y-axis capsule claims (20 center heights, 20 Y velocities).
      No A6 tick, private A5 sample, B3 receipt or reader may publish. */
   const float h291_source_y=two_owner->physical.actor.center.y;
   const float h291_velocity_y=two_owner->physical.actor.velocity.y;
   for(unsigned h291_case=0;h291_case<40u;++h291_case){
    const float offset=(float)(h291_case/2u+1u)*.01f;
    if((h291_case%2u)==0u)
      two_owner->physical.actor.center.y=h291_source_y+offset;
    else
      two_owner->physical.actor.velocity.y=h291_velocity_y-offset;
    CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
    CHECK(probe.calls==4u+h291_case&&probe.accepted==2u&&!in_tick->staged);
    CHECK(two_world->tick==source_tick&&
          two_owner->physical.world.revision==a5_before_revision&&
          push->receipt_count==2u&&nf_retc_read(publisher)==accepted);
    fprintf(stderr,"H291 A6_Y_SOURCE_DENIAL case=%u axis=%s preserved=1\n",
            h291_case,(h291_case%2u)==0u?"center_y":"velocity_y");
    two_owner->physical.actor.center.y=h291_source_y;
    two_owner->physical.actor.velocity.y=h291_velocity_y;
   }
   fprintf(stderr,"H291 40 real owner Y-source denials PASS\n");
   /* H292: 48 genuine owner-source exception/extreme-float denials:
      six 3D state dimensions x eight adversarial inputs. */
   const float h292_bad[8]={NAN,INFINITY,-INFINITY,1e30f,
                            -1e30f,1000.f,-1000.f,3.402823e38f};
   float *h292_fields[6]={&two_owner->physical.actor.center.x,
                          &two_owner->physical.actor.center.y,
                          &two_owner->physical.actor.center.z,
                          &two_owner->physical.actor.velocity.x,
                          &two_owner->physical.actor.velocity.y,
                          &two_owner->physical.actor.velocity.z};
   for(unsigned h292_case=0;h292_case<48u;++h292_case){
     const unsigned field=h292_case/8u,pattern=h292_case%8u;
     const float original=*h292_fields[field];
     *h292_fields[field]=h292_bad[pattern];
     CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
     CHECK(probe.calls==44u+h292_case&&probe.accepted==2u&&!in_tick->staged);
     CHECK(two_world->tick==source_tick&&
           two_owner->physical.world.revision==a5_before_revision&&
           push->receipt_count==2u&&nf_retc_read(publisher)==accepted);
     fprintf(stderr,"H292 FINITE_OWNER_REJECT case=%u field=%u pattern=%u preserved=1\n",
             h292_case,field,pattern);
     *h292_fields[field]=original;
   }
   fprintf(stderr,"H292 48 owner-source exceptional/extreme float denials PASS\n");
   /* H293: malformed 3D owner capsule dimensions cannot authorize a
      contact or mutate the accepted A6/A5/B3/reader state. */
   const float h293_radius=two_owner->physical.actor.radius;
   const float h293_height=two_owner->physical.actor.height;
   for(unsigned h293_case=0;h293_case<40u;++h293_case){
     const float invalid=-(float)(h293_case/2u)*.05f;
     if((h293_case%2u)==0u)
       two_owner->physical.actor.radius=invalid;
     else
       two_owner->physical.actor.height=invalid;
     CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
     CHECK(probe.calls==92u+h293_case&&probe.accepted==2u&&!in_tick->staged);
     CHECK(two_world->tick==source_tick&&
           two_owner->physical.world.revision==a5_before_revision&&
           push->receipt_count==2u&&nf_retc_read(publisher)==accepted);
     fprintf(stderr,"H293 CAPSULE_REJECT case=%u shape=%s preserved=1\n",
             h293_case,(h293_case%2u)==0u?"radius":"height");
     two_owner->physical.actor.radius=h293_radius;
     two_owner->physical.actor.height=h293_height;
   }
   fprintf(stderr,"H293 40 invalid capsule geometry denial cases PASS\n");
   /* H181: invalid A5 material epoch never authorizes a history sample. */
   two_owner->physical.world.grid.global_revision=0u;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==132u&&probe.accepted==2u&&!in_tick->staged);
   two_owner->physical.world.grid.global_revision=a5_before_epoch;
   /* H182: H83 cannot publish a partial two-actor transaction even with
      authentic A4 receipt and private A5 history successfully staged. */
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==133u&&probe.accepted==3u&&in_tick->staged);
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
   CHECK(probe.calls==134u&&probe.accepted==3u&&!in_tick->staged);
   center_chunk->loaded=1u;
   /* H185: a source SOLID voxel overlapping the swept capsule blocks. */
   const uint8_t previous_cell=center_chunk->canonical[0];
   center_chunk->canonical[0]=NF18A5_SOLID;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==135u&&probe.accepted==3u&&!in_tick->staged);
   /* H186: mixed canonical voxel without valid fine data is PENDING,
      never a positive static clearance claim. */
   center_chunk->canonical[0]=NF18A5_MIXED;
   center_chunk->fine_valid=0u;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==136u&&probe.accepted==3u&&!in_tick->staged);
   center_chunk->canonical[0]=previous_cell;
   /* H187: restored source clears and stages the SAME contact evidence;
      neither accepted world nor live A5/B3 may reflect any of the probes. */
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==137u&&probe.accepted==4u&&in_tick->staged);
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
   CHECK(probe.calls==138u&&probe.accepted==5u&&in_tick->staged);
   /* H189: stale fine-parent evidence must never grant static clearance. */
   center_chunk->fine_parent_epoch=fine_parent+1u;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==139u&&probe.accepted==5u&&!in_tick->staged);
   center_chunk->fine_parent_epoch=fine_parent;
   /* H190: fine-resolution SOLID directly in actor 30's swept hull is a
      real obstruction even though nearby exact subvoxels are FREE. */
   center_chunk->fine[0]=NF18A5_SOLID;
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==140u&&probe.accepted==5u&&!in_tick->staged);
   center_chunk->fine[0]=NF18A5_FREE;
   /* H191: fine cache release cannot silently downgrade a MIXED cell
      to FREE under an unchanged canonical material claim. */
   CHECK(nf18a5_release_fine(&two_owner->physical.world.grid,0,0,0));
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==141u&&probe.accepted==5u&&!in_tick->staged);
   /* H192: reloading exact fine source restores staging with identical
      collision receipt; all live A6/A5/B3 effects remain unpublished. */
   CHECK(nf18a5_load_fine(&two_owner->physical.world.grid,0,0,0,
                          2u,fine_clear,fine_len,fine_parent,1u));
   CHECK(nf_world_step_checked(two_world,.125f)==NF_WORLD_TICK_REJECTED);
   CHECK(probe.calls==142u&&probe.accepted==6u&&in_tick->staged);
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
   /* H290: forty real A6-tick PRIVATE material/receipt mutations. Each
    * changed causal predecessor or A4/A5 reciprocal effect MUST reject;
    * true accepted source evidence must still pass. This creates no new
    * publisher, world tick, B2 use, or B3 receipt. */
   NfRetcMa03TickEvidence *h290_changed=malloc(sizeof *h290_changed);
   CHECK(h290_changed);
   for(unsigned h290_case=0;h290_case<40u;++h290_case){
     *h290_changed=*in_tick;
     const unsigned field=h290_case/2u;
     const bool positive=(h290_case%2u)==0u;
     uint32_t *u=NULL;
     float *v=NULL;
     switch(field){
       case 0: h290_changed->source_tick+=positive?1u:(uint64_t)-1;break;
       case 1: u=&h290_changed->source_revision;break;
       case 2: u=&h290_changed->source_material_epoch;break;
       case 3: u=&h290_changed->source_raw_samples;break;
       case 4: h290_changed->material_candidate.tick+=positive?1u:(uint32_t)-1;break;
       case 5: u=&h290_changed->material_candidate.revision;break;
       case 6: u=&h290_changed->material_candidate.history.raw_samples;break;
       case 7: u=&h290_changed->material_candidate.grid.global_revision;break;
       case 8: u=&h290_changed->applied_contact.tick;break;
       case 9: u=&h290_changed->applied_contact.world_version;break;
       case 10: u=&h290_changed->sample.tick;break;
       case 11: u=&h290_changed->sample.material_epoch;break;
       case 12: u=&h290_changed->sample.body_a;break;
       case 13: u=&h290_changed->sample.body_b;break;
       case 14: u=&h290_changed->sample.contact_id;break;
       case 15: u=&h290_changed->applied_contact.contact_id;break;
       case 16: u=&h290_changed->bodies[0].id;break;
       case 17: u=&h290_changed->bodies[1].id;break;
       case 18: v=&h290_changed->material_impulse[0].x;break;
       case 19: v=&h290_changed->material_impulse[1].x;break;
     }
     if(u)*u+=positive?1u:(uint32_t)-1;
     if(v)*v+=positive?1.f:-1.f;
     CHECK(!nf_retc_ma03_reciprocal_effects_valid(h290_changed));
     CHECK(nf_retc_ma03_reciprocal_effects_valid(in_tick));
     CHECK(two_world->tick==source_tick&&
           two_owner->physical.world.revision==a5_before_revision&&
           push->receipt_count==2u&&nf_retc_read(publisher)==accepted);
     fprintf(stderr,"H290 A5_LINEAGE_REJECT case=%u field=%u sign=%u original_pass=1\n",
             h290_case,field,h290_case%2u);
   }
   free(h290_changed);
   fprintf(stderr,"H290 REAL_A5_MATRIX PASS 40 negative and 40 positive source rechecks\n");
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
   CHECK(probe.calls==143u&&probe.accepted==7u&&in_tick->staged);
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
