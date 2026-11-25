#ifndef NF_RETC_H319_SIMULATION_FRONTIER_H
#define NF_RETC_H319_SIMULATION_FRONTIER_H
/* H319/O1 finite-world scheduling research: bounded attention, NOT world physics.
 * Pure integer policy. It NEVER edits source regions or manufactures material.
 * Consequential unresolved work is PENDING, never silently simulated at low fidelity.
 * Deterministic ranking breaks all ties by stable region ID. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define NF_RETC_H319_FRONTIER_MAX 16u

typedef enum NfRetcH319Detail {
    NF_RETC_H319_DORMANT=0,
    NF_RETC_H319_SUMMARY=1,
    NF_RETC_H319_DETAIL=2,
    NF_RETC_H319_PENDING=3
} NfRetcH319Detail;

typedef struct NfRetcH319Region {
    uint32_t id;
    uint32_t work_cost; /* fixed simulation budget units, never estimated at runtime */
    uint8_t consequential, observed, active, resident;
    uint8_t distance_bucket, idle_ticks;
    uint8_t reserved[2];
} NfRetcH319Region;

typedef struct NfRetcH319Frontier {
    uint32_t budget, spent;
    uint8_t count, detailed, summarized, dormant, pending;
    NfRetcH319Detail decisions[NF_RETC_H319_FRONTIER_MAX];
} NfRetcH319Frontier;

static inline uint32_t nf_retc_h319_frontier_priority(NfRetcH319Region r) {
    const uint32_t distance=r.distance_bucket>15u?15u:r.distance_bucket;
    const uint32_t aging=r.idle_ticks>15u?15u:r.idle_ticks;
    return (r.consequential?128u:0u)+(r.observed?64u:0u)+
           (r.active?32u:0u)+(15u-distance)+aging;
}

/* The decision is keyed by input index; consumers MUST re-bind by stable ID.
 * Requires nonzero unique IDs, explicit cost and canonical boolean flags.
 * A nonresident relevant region is PENDING if consequential, otherwise SUMMARY;
 * a budget-short consequential region is likewise PENDING, not "approximated".
 * This grants no authority to advance time or commit material effects. */
static inline bool nf_retc_h319_frontier_select(
     const NfRetcH319Region *regions,size_t count,uint32_t budget,
     NfRetcH319Frontier *out) {
    if(!regions||!out||!count||count>NF_RETC_H319_FRONTIER_MAX)
        return false;
    for(size_t i=0;i<count;++i){
        const NfRetcH319Region *r=&regions[i];
        if(!r->id||!r->work_cost||r->consequential>1u||
           r->observed>1u||r->active>1u||r->resident>1u)
            return false;
        for(size_t j=0;j<i;++j)if(regions[j].id==r->id)return false;
    }
    size_t ranked[NF_RETC_H319_FRONTIER_MAX];
    for(size_t i=0;i<count;++i)ranked[i]=i;
    for(size_t i=1;i<count;++i) {
        const size_t next=ranked[i];
        const uint32_t p=nf_retc_h319_frontier_priority(regions[next]);
        size_t j=i;
        while(j>0){
            const size_t old=ranked[j-1u];
            const uint32_t q=nf_retc_h319_frontier_priority(regions[old]);
            if(q>p||(q==p&&regions[old].id<regions[next].id))break;
            ranked[j]=ranked[j-1u];--j;
        }
        ranked[j]=next;
    }
    NfRetcH319Frontier result={0};
    result.budget=budget;result.count=(uint8_t)count;
    for(size_t i=0;i<count;++i){
        const size_t k=ranked[i];
        const NfRetcH319Region *r=&regions[k];
        const bool relevant=r->consequential||r->observed||r->active;
        NfRetcH319Detail mode=NF_RETC_H319_DORMANT;
        if(relevant&&r->resident&&r->work_cost<=budget-result.spent){
            result.spent+=r->work_cost;mode=NF_RETC_H319_DETAIL;
            ++result.detailed;
        }else if(r->consequential){
            mode=NF_RETC_H319_PENDING;++result.pending;
        }else if(relevant){
            mode=NF_RETC_H319_SUMMARY;++result.summarized;
        }else ++result.dormant;
        result.decisions[k]=mode;
    }
    *out=result;
    return true;
}
#endif
