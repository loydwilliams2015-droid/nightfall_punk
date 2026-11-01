#ifndef NF_C2_RETC_FRAME_H
#define NF_C2_RETC_FRAME_H
/* Bounded Candidate A scientific prototype: immutable published frame.
 * No global world migration or durable journal guarantee yet. */
#include "nf_contact18a5_close.h"
#include "nf_interact18b3.h"
#include "nf_eligibility18b2.h"
#include "nf_encounter18b2.h"
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
 /* Exact source-backed encounter ledger: staged ownership, balances and object state. */
 Nf18b2Registry encounter_registry;
 bool has_encounter_registry;
 uint32_t obligation_count;
 NfRetcObligation obligations[NF_RETC_OBLIGATIONS];
 uint32_t derived_actor_hash,derived_contract_hash;
} NfRetcFrame;
typedef struct NfRetcPublisher {
 _Atomic(const NfRetcFrame *) current;
 NfRetcFrame *owned[NF_RETC_HISTORY];
 uint32_t count;
 /* H57: protects published frame lifetime for explicitly leased readers.
    Owner/writer serialization and publisher-object lifetime are separate. */
 atomic_uint active_readers;
 atomic_bool read_closing;
} NfRetcPublisher;
typedef struct NfRetcReadLease {
 const NfRetcPublisher *publisher;
 const NfRetcFrame *accepted;
} NfRetcReadLease;
/* A successful lease pins an accepted immutable frame until release.
   Acquisition fails once shutdown begins. Publishers themselves must remain
   allocated until all acquisition attempts have returned. */
bool nf_retc_read_lease_acquire(const NfRetcPublisher *pub,
                               NfRetcReadLease *out);
void nf_retc_read_lease_release(NfRetcReadLease *lease);
/* Nonblocking shutdown: marks closing, rejects new leases and refuses
   destruction while an existing lease remains. Call again after drain.
   Single authoritative writer must already be quiesced by A6. */
bool nf_retc_try_dispose(NfRetcPublisher *pub);
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
bool nf_retc_bind_encounter(NfRetcFrame *candidate,
 const Nf18b2EncounterRuntime *encounter);
/* Bounded source-backed PUSH realizer: construct the accepted typed B2/B3
 * candidate without producing independent physical/contractual side effects. */
NfRetcFrame *nf_retc_stage_source_push(
 const Nf18a5Integrated *physical,const Nf18b3Event *event,
 uint32_t tx,uint32_t prior_epoch,const Nf18b2Use *use,
 const Nf18b2UseDecision *decision,const Nf18b2Actor *actor,
 const Nf18b2Object *object,const Nf18b2Authority *authority);
/* Fail closed: a frame is not durably accepted until this full-frame journal
 * has been written successfully. Bounded research format, same ABI only. */
bool nf_retc_persist_file(NfRetcPublisher *pub,NfRetcFrame *candidate,
 const char *path);
bool nf_retc_commit_file(NfRetcPublisher *pub,NfRetcFrame *candidate,
 const char *path);
bool nf_retc_restore_file(const char *path,NfRetcFrame *out);
bool nf_retc_publish(NfRetcPublisher *pub,NfRetcFrame *candidate);
const NfRetcFrame *nf_retc_read(const NfRetcPublisher *pub);
/* H42 bounded read-only projection: all these domains come from exactly one
 * accepted pointer load. Reject incomplete B2/B3/encounter frames rather
 * than mixing with live mutable state. View storage belongs to pub and is
 * valid only until publisher disposal; callers must coordinate its lifetime.
 * This is NOT a global reader migration or a concurrent shutdown guarantee. */
typedef struct NfRetcReadView {
 const NfRetcFrame *accepted;
 const Nf18a5Integrated *physical;
 const Nf18b3Event *event;
 const Nf18b2Registry *contract;
 const Nf18b2Use *eligibility;
 uint32_t epoch,transaction_id;
} NfRetcReadView;
bool nf_retc_read_joint_view(const NfRetcPublisher *pub,NfRetcReadView *out);
/* H57: validated joint view under active lease; invalidate before release. */
bool nf_retc_read_leased_view(const NfRetcReadLease *lease,NfRetcReadView *out);
void nf_retc_dispose(NfRetcPublisher *pub);
#endif
