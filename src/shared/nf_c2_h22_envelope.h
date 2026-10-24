#ifndef NF_C2_H22_ENVELOPE_H
#define NF_C2_H22_ENVELOPE_H
#include "nf_contact18a5_close.h"
#include "nf_interact18b3.h"
#include <stdint.h>
/* One persisted envelope: A5 checkpoint bytes + B3 source receipt metadata.
 * This is a checkpoint/recovery challenger, NOT yet the live A6 publisher. */
typedef struct {uint32_t tick,actor,action,a4_receipts,a5_samples,b3_receipts;
 Nf18b3Event b3_event; /* H23 optional complete typed contractual event */
} NfC2H22Receipt;
bool nf_c2_h22_save(const char *path,const Nf18a5Integrated *physical,NfC2H22Receipt receipt);
/* H23 strict recovery refuses partial or unbound B3 event metadata. */
bool nf_c2_h23_restore_full(const char *path,Nf18a5Integrated *physical,Nf18b3Event *event);
bool nf_c2_h22_restore(const char *path,Nf18a5Integrated *physical,NfC2H22Receipt *receipt);
#endif
