#ifndef NF_V18C2_WITNESS_H
#define NF_V18C2_WITNESS_H
#include "nf_multi_ma03.h"
typedef enum {NF_C2W_READY,NF_C2W_DENIED,NF_C2W_STALE,NF_C2W_PENDING} NfC2WitnessStatus;
/* Read-only research gate: inherited server snapshot provider must be
 * configured by the A6 owner, NOT accepted from client proposals.
 * No publication or cryptographic credential attestation. */
NfC2WitnessStatus nf_c2_witness(const NfWorld *world,
 const Nf18a6Runtime *runtime,NfMa03Provider provider,void *provider_ctx,
 uint32_t action,NfVec3 probe,uint32_t expected_material_epoch);
#endif
