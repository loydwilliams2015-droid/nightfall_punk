#ifndef NF_RETC_MAP_PROJECTION_H
#define NF_RETC_MAP_PROJECTION_H
/* H297 bounded map READ MODEL from real A6 immutable publisher lease.
 * Projects accepted actor and dynamic object geometry only; NOT full
 * terrain/chunks, a map engine writer, physics solver or C3 recovery. */
#include "nf_retc_consumer_source.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
typedef struct NfRetcMapProjection {
 uint32_t tick,transaction_id,owner_actor,object_id,contract_epoch;
 NfVec3 actor_center,object_center,object_min,object_max;
} NfRetcMapProjection;
typedef struct NfRetcMapCursor {
 uint32_t tick,transaction_id;
} NfRetcMapCursor;
static inline bool nf_retc_map_project_lease(
 const NfRetcReadLease *lease,const NfRetcReadView *view,
 uint32_t expected_tick,uint32_t expected_transaction,
 uint32_t expected_owner,uint32_t expected_object,
 uint32_t expected_contract_epoch,NfRetcMapCursor *cursor,
 NfRetcMapProjection *out)
{
 if(!cursor||!out||!expected_tick||!expected_transaction||
    !expected_owner||!expected_object||!expected_contract_epoch)
  return false;
 NfRetcSourceObservation proof={0};
 if(!nf_retc_consumer_source_validate(lease,view,&proof))return false;
 const NfRetcFrame *f=view->accepted;
 const Nf18a4Body *actor=&f->physical.actor,*object=&f->physical.object;
 if(proof.tick!=expected_tick||
    proof.transaction_id!=expected_transaction||
    proof.owner_actor!=expected_owner||
    proof.contract_epoch!=expected_contract_epoch||
    f->object_id!=expected_object||
    actor->id!=expected_owner||object->id!=expected_object||
    expected_tick<=cursor->tick||
    expected_transaction<=cursor->transaction_id||
    !isfinite(actor->center.x)||!isfinite(actor->center.y)||!isfinite(actor->center.z)||
    !isfinite(object->center.x)||!isfinite(object->center.y)||!isfinite(object->center.z)||
    !isfinite(object->box_half.x)||!isfinite(object->box_half.y)||!isfinite(object->box_half.z)||
    object->box_half.x<=0.f||object->box_half.y<=0.f||object->box_half.z<=0.f)
  return false;
 NfVec3 min={object->center.x-object->box_half.x,
             object->center.y-object->box_half.y,
             object->center.z-object->box_half.z};
 NfVec3 max={object->center.x+object->box_half.x,
             object->center.y+object->box_half.y,
             object->center.z+object->box_half.z};
 if(!isfinite(min.x)||!isfinite(min.y)||!isfinite(min.z)||
    !isfinite(max.x)||!isfinite(max.y)||!isfinite(max.z))
  return false;
 *out=(NfRetcMapProjection){proof.tick,proof.transaction_id,expected_owner,
       expected_object,proof.contract_epoch,actor->center,object->center,min,max};
 *cursor=(NfRetcMapCursor){proof.tick,proof.transaction_id};
 return true;
}
#endif
