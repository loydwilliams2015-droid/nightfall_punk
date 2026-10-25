#ifndef NF_RETC_JOINT_PROVIDER_H
#define NF_RETC_JOINT_PROVIDER_H
#include "nf_c2_retc_frame.h"
#include "nf_b6_push18b6.h"
/* Composite source-backed staging adapter. Caller must hold the A6 tick lease.
 * Neither B2 encounter nor B3 PUSH separately publishes through this API. */
typedef struct {
 Nf18a6Runtime *owner;
 Nf18b2EncounterRuntime *encounter;
 Nf18b6Push *push;
 Nf18b2Use use;
 Nf18b2UseDecision decision;
 Nf18b2Actor actor;
 Nf18b2Object object;
 Nf18b2Authority authority;
 uint32_t transaction_id;
} NfRetcJointContext;
NfRetcFrame *nf_retc_joint_stage(const NfWorld *world,
 const Nf18a5Integrated *candidate,void *context);
#endif
