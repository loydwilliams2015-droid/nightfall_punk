#ifndef NF_RETC_MAP_MATERIAL_H
#define NF_RETC_MAP_MATERIAL_H
/* H298 read-only validation of resident A5 canonical/fine data in an
 * actual accepted RET-C A6 frame. A per-resident summary is NOT complete
 * world clearance: unrepresented cells remain unknown. No writes to A5. */
#include "nf_retc_consumer_source.h"
#include <stdbool.h>
#include <stdint.h>
typedef struct NfRetcMaterialView {
 uint32_t tick,material_epoch,cache_revision;
 uint32_t resident_chunks,free_cells,solid_cells,mixed_cells,pending_cells;
 bool full_world_clearance; /* Always false for this bounded projection. */
} NfRetcMaterialView;
static inline bool nf_retc_map_material_scan(
 const Nf18a5Grid *g,uint32_t tick,NfRetcMaterialView *out)
{
 if(!g||!out||!tick||!g->global_revision||
    !g->capacity||g->capacity>NF18A5_MAX_CHUNKS||
    g->count>g->capacity||g->policy>NF18A5_UNSAFE_FINE_CONTROL)
  return false;
 NfRetcMaterialView local={0};
 local.tick=tick;
 local.material_epoch=g->global_revision;
 local.cache_revision=g->cache_revision;
 local.resident_chunks=g->count;
 for(uint32_t i=0;i<g->count;++i){
  const Nf18a5Chunk *c=&g->chunks[i];
  if(!c->loaded||!c->canonical_epoch||
     c->canonical_epoch>g->global_revision||
     c->fine_count>NF18A5_FINE_MAX_VOXELS||
     (c->fine_valid&&(!c->fine_resolution||
       c->fine_parent_epoch!=c->canonical_epoch||
       !c->fine_count)))
    return false;
  for(uint32_t j=0;j<NF18A5_CANON_VOXELS;++j){
   const uint8_t v=c->canonical[j];
   if(v==NF18A5_FREE)++local.free_cells;
   else if(v==NF18A5_SOLID)++local.solid_cells;
   else if(v==NF18A5_MIXED){
    ++local.mixed_cells;
    if(!c->fine_valid)++local.pending_cells;
    else{
     bool solid=false;
     for(uint32_t k=0;k<c->fine_count;++k){
      if(c->fine[k]>NF18A5_MIXED)return false;
      if(c->fine[k]!=NF18A5_FREE)solid=true;
     }
     if(solid)++local.pending_cells; /* Not proof of swept clearance. */
    }
   }else return false;
  }
 }
 /* Never convert partial resident knowledge into world-wide FREE. */
 local.full_world_clearance=false;
 *out=local;
 return true;
}
static inline bool nf_retc_map_material_from_lease(
 const NfRetcReadLease *lease,const NfRetcReadView *view,
 uint32_t expected_tick,NfRetcMaterialView *out)
{
 if(!out||!expected_tick)return false;
 NfRetcSourceObservation proof={0};
 if(!nf_retc_consumer_source_validate(lease,view,&proof)||
    proof.tick!=expected_tick)return false;
 return nf_retc_map_material_scan(
     &view->accepted->physical.world.grid,proof.tick,out);
}
#endif
