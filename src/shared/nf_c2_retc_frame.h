#ifndef NF_C2_RETC_FRAME_H
#define NF_C2_RETC_FRAME_H
/* Bounded Candidate A scientific prototype: immutable published frame.
 * No global world migration or durable journal guarantee yet. */
#include "nf_contact18a5_close.h"
#include "nf_interact18b3.h"
#include "nf_eligibility18b2.h"
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
 /* Typed B2 source-backed contractual witness; no invented obligation types. */
 Nf18b2Use b2_use;
 Nf18b2UseDecision b2_decision;
 Nf18b2Actor b2_actor;
 Nf18b2Object b2_object;
 Nf18b2Authority b2_authority;
 bool has_b2_contract;
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
/* Source-derived B2 evidence is copied into the PRIVATE frame.
 * This still does not realize/debit broader contract side effects. */
bool nf_retc_bind_b2(NfRetcFrame *candidate,
 const Nf18b2Use *use,const Nf18b2UseDecision *decision,
 const Nf18b2Actor *actor,const Nf18b2Object *object,
 const Nf18b2Authority *authority);
/* Source-derived transition seal, before publication: preserve pre-commit
 * authority, reject unsupported debit/transfer claims rather than infer them. */
bool nf_retc_seal_b2_b3(NfRetcFrame *candidate);
bool nf_retc_publish(NfRetcPublisher *pub,NfRetcFrame *candidate);
const NfRetcFrame *nf_retc_read(const NfRetcPublisher *pub);
void nf_retc_dispose(NfRetcPublisher *pub);
#endif
