#ifndef NF_RETC_MAP_SWEEP_H
#define NF_RETC_MAP_SWEEP_H
/* H299: actual immutable A6 A5 grid as input to a PRIVATE swept
 * query. The result is a bounded map witness, not movement admission
 * or a B2/B3 effect. Missing/stale terrain must never imply FREE. */
#include "nf_retc_map_material.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
typedef struct NfRetcSweptMapWitness {
 uint32_t tick,transaction_id,material_epoch;
 Nf18a5Query result;
} NfRetcSweptMapWitness;
static inline bool nf_retc_map_sweep_from_lease(
 const NfRetcReadLease *lease,const NfRetcReadView *view,
 uint32_t expected_tick,uint32_t expected_tx,
 uint32_t expected_owner,uint32_t expected_object,
 uint32_t expected_contract_epoch,NfVec3 feet,NfVec3 displacement,
 Nf18a2ShapePolicy shape,float dt,NfRetcSweptMapWitness *out)
{
 if(!out||!expected_tick||!expected_tx||!expected_owner||
    !expected_object||!expected_contract_epoch||
    !isfinite(dt)||dt<=0.f||
    !isfinite(feet.x)||!isfinite(feet.y)||!isfinite(feet.z)||
    !isfinite(displacement.x)||!isfinite(displacement.y)||
    !isfinite(displacement.z)||
    !isfinite(shape.radius)||shape.radius<=0.f||
    !isfinite(shape.height)||shape.height<=0.f||
    !isfinite(shape.foot_flat_radius)||!isfinite(shape.step_height)||
    !isfinite(shape.walkable_normal_y))return false;
 NfRetcSourceObservation src={0};
 if(!nf_retc_consumer_source_validate(lease,view,&src)||
    src.tick!=expected_tick||src.transaction_id!=expected_tx||
    src.owner_actor!=expected_owner||
    src.contract_epoch!=expected_contract_epoch||
    view->accepted->object_id!=expected_object)return false;
 const Nf18a5Grid *source=&view->accepted->physical.world.grid;
 NfRetcMaterialView material={0};
 if(!nf_retc_map_material_scan(source,expected_tick,&material)||
    source->policy==NF18A5_UNSAFE_FINE_CONTROL)return false;
 /* A5 sweep may write cache metadata: run only on a private grid. */
 Nf18a5Grid candidate=*source;
 uint32_t loads=0;
 Nf18a5Query q=nf18a5_sweep_query(&candidate,feet,displacement,
      shape,dt,expected_tick,NULL,NULL,&loads);
 if(q!=NF18A5_Q_FREE&&q!=NF18A5_Q_SOLID&&
    q!=NF18A5_Q_PENDING&&q!=NF18A5_Q_INVALID)return false;
 *out=(NfRetcSweptMapWitness){expected_tick,expected_tx,
                  material.material_epoch,q};
 return true;
}
#endif
