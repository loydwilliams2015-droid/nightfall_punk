/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef NF_RETC_H323_PORTABLE_H
#define NF_RETC_H323_PORTABLE_H
#include <stdint.h>
#include <stddef.h>
/* Research codec, not an A6 WAL/commit API. Explicit 24 little-endian u32 words,
 * including checksum. No native layouts, pointers, writes or issuer claims. */
#define NF_H323_BYTES 96u
#define NF_H323_MAGIC 0x33323348u /* H323 as LE bytes */
#define NF_H323_VERSION 1u
#define NF_H323_TYPE_COMMIT 2u
typedef struct NfH323Record {
 uint32_t type,flags,txn_low,txn_high,prior_tick,next_tick,owner_epoch;
 uint32_t actor[2],object,action[2],contact[2],material_epoch;
 uint32_t grant[2],receipt[2],prior_digest,new_digest;
} NfH323Record;
/* Accepted output is unchanged on any error. This codec is only an
 * integrity/identity envelope; no actual B2 grant or B3 effect is implied. */
int nf_h323_encode(const NfH323Record *r,uint8_t out[NF_H323_BYTES]);
int nf_h323_decode(const uint8_t *data,size_t n,NfH323Record *out);
#endif
