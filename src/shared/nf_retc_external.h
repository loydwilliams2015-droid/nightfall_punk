#ifndef NF_RETC_EXTERNAL_H
#define NF_RETC_EXTERNAL_H
/* Bounded external observation of ONE accepted A6/B2/B3 source frame.
 * Does not serialize a full NfWorld or authorize network gameplay. */
#include "nf_c2_retc_frame.h"
#include <stddef.h>
#include <stdint.h>
typedef struct NfRetcExternalRecord {
 uint32_t epoch,prior_epoch,transaction_id,actor_id,object_id,action_id;
 uint32_t contract_epoch,world_revision,material_epoch,committed_receipts;
 uint32_t contract_revision,account_count,first_account_actor,first_account_balance;
 NfVec3 actor_center,object_center;
} NfRetcExternalRecord;
/* A failed capture clears out so stale records cannot be re-used. */
/* H44: side-effect-free structural validation; no authenticity guarantee. */
bool nf_retc_external_valid(const NfRetcExternalRecord *record);
/* H45: non-authoritative consumer cursor, never grants permissions. */
typedef struct NfRetcExternalCursor { uint32_t epoch,transaction_id; } NfRetcExternalCursor;
bool nf_retc_external_accept(NfRetcExternalCursor *cursor,const NfRetcExternalRecord *record);
#define NF_RETC_EXTERNAL_WIRE_BYTES 92u
/* Versioned LE diagnostic envelope with checksum, NOT cryptographic auth. */
bool nf_retc_external_decode(const uint8_t *bytes,size_t size,NfRetcExternalRecord *out);
size_t nf_retc_external_encode(uint8_t *out,size_t cap,const NfRetcExternalRecord *record);
bool nf_retc_external_capture(const NfRetcPublisher *publisher,NfRetcExternalRecord *out);
#endif
