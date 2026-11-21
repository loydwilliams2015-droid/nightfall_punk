/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef NF_RETC_H323_CHECKPOINT_H
#define NF_RETC_H323_CHECKPOINT_H
#include "nf_retc_h323_portable.h"
/* H323.C research-only: one immutable metadata generation and a separate,
 * durable manifest. Not a full-world A6 checkpoint or a multi-entry WAL.
 * A crash can leave an orphan snapshot, never a valid partial generation.
 * Caller must not infer authenticated B2/B3 effects from serialized IDs. */
int nf_h323_checkpoint_create(const char *base, uint32_t generation,
                             const NfH323Record *record);
int nf_h323_checkpoint_read(const char *base, uint32_t generation,
                           NfH323Record *out);
#endif
