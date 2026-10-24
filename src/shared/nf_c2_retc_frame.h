#ifndef NF_C2_RETC_FRAME_H
#define NF_C2_RETC_FRAME_H
/* Bounded Candidate A scientific prototype: immutable published frame.
 * No global world migration or durable journal guarantee yet. */
#include "nf_contact18a5_close.h"
#include "nf_interact18b3.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#define NF_RETC_OBLIGATIONS 8u
#define NF_RETC_HISTORY 8u
typedef struct {
 uint32_t obligation_id,holder,beneficiary,kind,quantity,status;
} NfRetcObligation;
typedef struct NfRetcFrame {
 Nf18a5Integrated physical;
 Nf18b3Event event;
 uint32_t epoch,prior_epoch,transaction_id;
 uint32_t owner_actor,object_id;
 uint32_t obligation_count;
 NfRetcObligation obligations[NF_RETC_OBLIGATIONS];
 uint32_t derived_actor_hash,derived_contract_hash;
} NfRetcFrame;
typedef struct {
 _Atomic(const NfRetcFrame *) current;
 NfRetcFrame *owned[NF_RETC_HISTORY];
 uint32_t count;
} NfRetcPublisher;
/* Stage makes a private deep copy. Caller retains the candidate until publish. */
NfRetcFrame *nf_retc_stage(const Nf18a5Integrated *physical,
 const Nf18b3Event *event,uint32_t tx,uint32_t prior_epoch,
 const NfRetcObligation *obligations,uint32_t obligation_count);
bool nf_retc_publish(NfRetcPublisher *pub,NfRetcFrame *candidate);
const NfRetcFrame *nf_retc_read(const NfRetcPublisher *pub);
void nf_retc_dispose(NfRetcPublisher *pub);
#endif
