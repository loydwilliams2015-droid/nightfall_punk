#ifndef NF_RETC_H319_MATERIAL_LEDGER_H
#define NF_RETC_H319_MATERIAL_LEDGER_H
/* ONI-inspired bounded material accounting research, not fluid simulation.
 * Both destinations must be canonical A5 FREE. Input validation and staging
 * complete before any ledger mutation. No world commit or A6 authority. */
#include "nf_contact18a5.h"
#include <limits.h>
#include <stdint.h>
typedef struct NfRetcH319Cell {
    int64_t mass_units;
    int64_t thermal_units; /* abstract conserved inventory, NOT temperature */
    NfVec3 sample;
} NfRetcH319Cell;
typedef enum NfRetcH319Transfer {
    NF_RETC_H319_TRANSFER_APPLIED=0,
    NF_RETC_H319_TRANSFER_PENDING=1,
    NF_RETC_H319_TRANSFER_BLOCKED=2,
    NF_RETC_H319_TRANSFER_INVALID=3
} NfRetcH319Transfer;
static inline NfRetcH319Transfer nf_retc_h319_transfer(
    Nf18a5Grid *grid,uint32_t tick,
    NfRetcH319Cell *from,NfRetcH319Cell *to,
    int64_t mass,int64_t thermal){
    if(!grid||!from||!to||from==to||!tick||mass<0||thermal<0||
       (!mass&&!thermal)||from->mass_units<0||to->mass_units<0||
       from->thermal_units<0||to->thermal_units<0||
       mass>from->mass_units||thermal>from->thermal_units||
       mass>INT64_MAX-to->mass_units||
       thermal>INT64_MAX-to->thermal_units)return NF_RETC_H319_TRANSFER_INVALID;
    const Nf18a5Query a=nf18a5_query(grid,
       from->sample.x,from->sample.y,from->sample.z,tick,true);
    const Nf18a5Query b=nf18a5_query(grid,
       to->sample.x,to->sample.y,to->sample.z,tick,true);
    if(a==NF18A5_Q_INVALID||b==NF18A5_Q_INVALID)
        return NF_RETC_H319_TRANSFER_INVALID;
    if(a==NF18A5_Q_SOLID||b==NF18A5_Q_SOLID)
        return NF_RETC_H319_TRANSFER_BLOCKED;
    if(a!=NF18A5_Q_FREE||b!=NF18A5_Q_FREE)
        return NF_RETC_H319_TRANSFER_PENDING;
    from->mass_units-=mass;
    to->mass_units+=mass;
    from->thermal_units-=thermal;
    to->thermal_units+=thermal;
    return NF_RETC_H319_TRANSFER_APPLIED;
}
#endif
