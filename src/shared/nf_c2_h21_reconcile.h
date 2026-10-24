#ifndef NF_C2_H21_RECONCILE_H
#define NF_C2_H21_RECONCILE_H
#include <stdbool.h>
#include <stdint.h>
/* Receipt evidence does not prove full physical restoration. Never report
 * COMPLETE unless actual source-bound A5 state and all B3 records agree. */
typedef enum {NF_C2_H21_REJECT=0,NF_C2_H21_UNRESOLVED=1,NF_C2_H21_MATCHED=2} NfC2H21Status;
NfC2H21Status nf_c2_h21_reconcile(const char *receipt_file,
 uint32_t tick,uint32_t actor,uint32_t action,
 uint32_t a4_receipts,uint32_t a5_samples,uint32_t b3_receipts);
#endif
