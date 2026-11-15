#ifndef NF_RETC_MAP_CLEARANCE_H
#define NF_RETC_MAP_CLEARANCE_H
/* H300: fail-closed terrain-read decision from a real A6 frame.
 * A diagnostic query returning true is NOT a FREE/authorized traversal.
 * This remains a reader oracle; A6's B2/A4/A5/B3 owner decides movement. */
#include "nf_retc_map_sweep.h"
static inline bool nf_retc_map_confirm_free(
 const NfRetcReadLease *lease,const NfRetcReadView *view,
 uint32_t tick,uint32_t transaction_id,uint32_t owner_id,
 uint32_t object_id,uint32_t contract_epoch,
 NfVec3 feet,NfVec3 displacement,Nf18a2ShapePolicy shape,float dt,
 NfRetcSweptMapWitness *out)
{
 if(!out)return false;
 NfRetcSweptMapWitness staged={0};
 if(!nf_retc_map_sweep_from_lease(lease,view,tick,transaction_id,
      owner_id,object_id,contract_epoch,feet,displacement,shape,dt,
      &staged)||staged.result!=NF18A5_Q_FREE)return false;
 *out=staged;
 return true;
}
#endif
