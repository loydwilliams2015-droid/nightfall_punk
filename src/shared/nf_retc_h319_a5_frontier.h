#ifndef NF_RETC_H319_A5_FRONTIER_H
#define NF_RETC_H319_A5_FRONTIER_H
/* O1 finite-world scheduler + genuine A5 canonical/fine evidence.
 * Research-only read/preflight: neither A6 ownership nor physics authority.
 * A5 queries can update last_access_tick (cache residency bookkeeping),
 * never world materials, inventory, or a gameplay transaction.
 * Only DETAIL-selected regions are material-queried. A blocked/unresolved
 * selected region is downgraded to PENDING when consequential, otherwise
 * SUMMARY. Its reserved work budget is NOT reallocated this tick.
 */
#include "nf_retc_h319_simulation_frontier.h"
#include "nf_contact18a5.h"
#include <math.h>

typedef enum NfRetcH319A5Evidence {
    NF_RETC_H319_A5_NOT_QUERIED=0,
    NF_RETC_H319_A5_FREE=1,
    NF_RETC_H319_A5_BLOCKED=2,
    NF_RETC_H319_A5_UNRESOLVED=3
} NfRetcH319A5Evidence;

typedef struct NfRetcH319A5Frontier {
    NfRetcH319Frontier plan;
    NfRetcH319A5Evidence evidence[NF_RETC_H319_FRONTIER_MAX];
    uint32_t tick, material_epoch;
    uint8_t queried, blocked, unresolved;
} NfRetcH319A5Frontier;

static inline bool nf_retc_h319_a5_frontier(
     Nf18a5Grid *grid,uint32_t tick,uint32_t epoch,
     const NfRetcH319Region *regions,const NfVec3 *samples,
     size_t count,uint32_t budget,NfRetcH319A5Frontier *out) {
    if(!grid||!out||!regions||!samples||!tick||!epoch||
       grid->global_revision!=epoch||!count||
       count>NF_RETC_H319_FRONTIER_MAX)return false;
    /* Validate coordinates before the first A5 query: invalid input cannot
       partially change cache-query access bookkeeping. */
    for(size_t i=0;i<count;++i)
       if(!isfinite(samples[i].x)||!isfinite(samples[i].y)||
          !isfinite(samples[i].z))return false;
    NfRetcH319A5Frontier staged={0};
    if(!nf_retc_h319_frontier_select(regions,count,budget,&staged.plan))
        return false;
    staged.tick=tick;staged.material_epoch=epoch;
    for(size_t i=0;i<count;++i) {
        if(staged.plan.decisions[i]!=NF_RETC_H319_DETAIL)continue;
        ++staged.queried;
        const Nf18a5Query q=nf18a5_query(grid,samples[i].x,
            samples[i].y,samples[i].z,tick,true);
        if(q==NF18A5_Q_INVALID)return false;
        if(q==NF18A5_Q_FREE){
            staged.evidence[i]=NF_RETC_H319_A5_FREE;
            continue;
        }
        staged.evidence[i]=(q==NF18A5_Q_SOLID)?
            NF_RETC_H319_A5_BLOCKED:NF_RETC_H319_A5_UNRESOLVED;
        if(q==NF18A5_Q_SOLID)++staged.blocked;
        else ++staged.unresolved;
        --staged.plan.detailed;
        if(regions[i].consequential){
            staged.plan.decisions[i]=NF_RETC_H319_PENDING;
            ++staged.plan.pending;
        }else{
            staged.plan.decisions[i]=NF_RETC_H319_SUMMARY;
            ++staged.plan.summarized;
        }
    }
    if(grid->global_revision!=epoch)return false;
    *out=staged;
    return true;
}
#endif
