#ifndef NF_RETC_MA03_A5_SWEEP_PAIR_H
#define NF_RETC_MA03_A5_SWEEP_PAIR_H
/* H305: two source-owned A4 trajectories must BOTH pass real A5 swept
 * canonical/fine geometry before promoting even a private contact plan.
 * No B2 grants, B3 realization, durable WAL, accepted A6 frame or tick. */
#include "nf_retc_ma03_a5_contact_stage.h"
typedef struct NfRetcMa03PairSweepWitness {
 uint32_t tick,material_epoch,actor_ids[2];
 Nf18a5Query query[2];
 bool prepared;
} NfRetcMa03PairSweepWitness;
static inline bool nf_retc_ma03_pair_swept_clearance(
 const NfWorld *world,const Nf18a6Runtime *owner,float dt,
 const NfRetcMa03PhysicalCandidate *physical,
 const NfRetcMa03A5Envelope *handoff,
 NfRetcMa03PairSweepWitness *out)
{
 if(!out||!world||!owner||!physical||!handoff||
    !nf_retc_ma03_a5_recheck(world,owner,dt,physical,handoff)||
    !handoff->material_epoch)return false;
 NfRetcMa03PairSweepWitness result={0};
 result.tick=handoff->source_tick+1u;
 result.material_epoch=handoff->material_epoch;
 for(size_t i=0;i<2u;++i){
  const Nf18a4Body *before=&handoff->bodies[i];
  const Nf18a4Body *source=&physical->solved.bodies[i];
  const NfMa03Participant *roster=&physical->source.actors[i];
  const NfVec3 feet={roster->position.x,roster->position.y,
                     roster->position.z};
  /* Each body's source center is constructed from its actual world
     roster; the solved center is an A4 result, never a caller path. */
  const NfVec3 displacement={
    before->center.x-roster->position.x,
    before->center.y-(roster->position.y+before->height*.5f),
    before->center.z-roster->position.z};
  const Nf18a2ShapePolicy shape={
    .radius=before->radius,.height=before->height,
    .foot_flat_radius=.05f,.step_height=.4f,
    .walkable_normal_y=.65f};
  if(before->id!=roster->id||before->id!=source->id||
     !isfinite(displacement.x)||!isfinite(displacement.y)||
     !isfinite(displacement.z))return false;
  Nf18a5Grid *private_grid=malloc(sizeof *private_grid);
  if(!private_grid)return false;
  *private_grid=owner->physical.world.grid;
  uint32_t loads=0;
  Nf18a5Query q=nf18a5_sweep_query(private_grid,feet,displacement,
       shape,dt,result.tick,NULL,NULL,&loads);
  free(private_grid);
  if(q!=NF18A5_Q_FREE)return false;
  result.query[i]=q;
  result.actor_ids[i]=before->id;
 }
 result.prepared=true;
 *out=result;
 return true;
}
#endif
