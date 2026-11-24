#ifndef NF_RETC_H319_MATERIAL_TRANSPORT_H
#define NF_RETC_H319_MATERIAL_TRANSPORT_H
/* O1: bounded simultaneous multi-cell conservation research.
 * Source snapshot + cumulative debit/credit before one in-memory publish.
 * Not physical fluid flow, heat conduction, A6 authority, or disk durability. */
#include "nf_retc_h319_material_ledger.h"
#include <stddef.h>
#include <string.h>
#define NF_RETC_H319_MAX_CELLS 8u
#define NF_RETC_H319_MAX_EDGES 16u

typedef struct NfRetcH319Edge {
    uint8_t from, to;
    int64_t mass_units, thermal_units;
} NfRetcH319Edge;

typedef struct NfRetcH319TransportReceipt {
    uint32_t tick, material_epoch;
    uint8_t cells, edges;
    int64_t total_mass, total_thermal;
} NfRetcH319TransportReceipt;

/* No grid edits; querying may change cache residency, but the canonical
 * material epoch must remain unchanged. Failure leaves ledger and receipt
 * byte-for-byte untouched. Existing A5 policy supplies FREE/PENDING/SOLID. */
static inline NfRetcH319Transfer nf_retc_h319_transport(
    Nf18a5Grid *grid, uint32_t tick, uint32_t expected_material_epoch,
    NfRetcH319Cell *cells, size_t n,
    const NfRetcH319Edge *edges, size_t ne,
    NfRetcH319TransportReceipt *receipt) {
    if(!grid||!cells||!edges||!receipt||!tick||
       !expected_material_epoch||grid->global_revision!=expected_material_epoch||
       !n||n>NF_RETC_H319_MAX_CELLS||!ne||ne>NF_RETC_H319_MAX_EDGES)
       return NF_RETC_H319_TRANSFER_INVALID;
    int64_t out_m[NF_RETC_H319_MAX_CELLS]={0};
    int64_t out_t[NF_RETC_H319_MAX_CELLS]={0};
    int64_t in_m[NF_RETC_H319_MAX_CELLS]={0};
    int64_t in_t[NF_RETC_H319_MAX_CELLS]={0};
    uint8_t used[NF_RETC_H319_MAX_CELLS]={0};
    for(size_t i=0;i<n;++i)
       if(cells[i].mass_units<0||cells[i].thermal_units<0)
          return NF_RETC_H319_TRANSFER_INVALID;
    for(size_t j=0;j<ne;++j) {
       const NfRetcH319Edge *e=&edges[j];
       if(e->from>=n||e->to>=n||e->from==e->to||
          e->mass_units<0||e->thermal_units<0||
          (!e->mass_units&&!e->thermal_units))
          return NF_RETC_H319_TRANSFER_INVALID;
       for(size_t k=0;k<j;++k)
          if(edges[k].from==e->from&&edges[k].to==e->to)
              return NF_RETC_H319_TRANSFER_INVALID; /* replay/ambiguity */
       if(e->mass_units>INT64_MAX-out_m[e->from]||
          e->thermal_units>INT64_MAX-out_t[e->from]||
          e->mass_units>INT64_MAX-in_m[e->to]||
          e->thermal_units>INT64_MAX-in_t[e->to])
          return NF_RETC_H319_TRANSFER_INVALID;
       out_m[e->from]+=e->mass_units;
       out_t[e->from]+=e->thermal_units;
       in_m[e->to]+=e->mass_units;
       in_t[e->to]+=e->thermal_units;
       used[e->from]=used[e->to]=1u;
    }
    /* No incoming resource can fund an outgoing debit in this frame. */
    int64_t total_m=0,total_t=0;
    for(size_t i=0;i<n;++i) {
        if(out_m[i]>cells[i].mass_units||
           out_t[i]>cells[i].thermal_units||
           in_m[i]>INT64_MAX-(cells[i].mass_units-out_m[i])||
           in_t[i]>INT64_MAX-(cells[i].thermal_units-out_t[i]))
           return NF_RETC_H319_TRANSFER_INVALID;
        if(cells[i].mass_units>INT64_MAX-total_m||
           cells[i].thermal_units>INT64_MAX-total_t)
           return NF_RETC_H319_TRANSFER_INVALID;
        total_m+=cells[i].mass_units;
        total_t+=cells[i].thermal_units;
    }
    bool pending=false, blocked=false;
    for(size_t i=0;i<n;++i)if(used[i]){
        const Nf18a5Query q=nf18a5_query(grid,cells[i].sample.x,
            cells[i].sample.y,cells[i].sample.z,tick,true);
        if(q==NF18A5_Q_INVALID)return NF_RETC_H319_TRANSFER_INVALID;
        if(q==NF18A5_Q_SOLID)blocked=true;
        if(q!=NF18A5_Q_FREE)pending=true;
    }
    if(grid->global_revision!=expected_material_epoch)
        return NF_RETC_H319_TRANSFER_INVALID;
    if(blocked)return NF_RETC_H319_TRANSFER_BLOCKED;
    if(pending)return NF_RETC_H319_TRANSFER_PENDING;
    NfRetcH319Cell staged[NF_RETC_H319_MAX_CELLS];
    memcpy(staged,cells,n*sizeof(*cells));
    for(size_t i=0;i<n;++i) {
        staged[i].mass_units=staged[i].mass_units-out_m[i]+in_m[i];
        staged[i].thermal_units=staged[i].thermal_units-out_t[i]+in_t[i];
    }
    NfRetcH319TransportReceipt result={
        tick,expected_material_epoch,(uint8_t)n,(uint8_t)ne,total_m,total_t};
    memcpy(cells,staged,n*sizeof(*cells));
    *receipt=result;
    return NF_RETC_H319_TRANSFER_APPLIED;
}
#endif
