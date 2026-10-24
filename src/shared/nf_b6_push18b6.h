#ifndef NF_B6_PUSH18B6_H
#define NF_B6_PUSH18B6_H
/* 1.8B.6 H1: B3 PUSH semantic receipt, bound to the EXISTING A6 owner.
 * It never drives physics or journals a separate authoritative world.
 * Only one actor/crate is supported. No network-authentication claim.
 */
#include "nf_embody18a6.h"
#include "nf_interact18b3.h"
#include "nf_eligibility18b2.h"
#include <stdbool.h>
#include <stdint.h>
#define NF18B6_PUSH_RECEIPTS 64u
typedef struct Nf18b6PushReceipt {
    uint32_t actor, action, tick;
} Nf18b6PushReceipt;
/* Provider MUST be owned by the authoritative server. The adapter verifies
 * that supplied B2 scene geometry and actor pose match the existing A6 world.
 * Credential/inventory/ownership values remain a server-provider obligation.
 * Snapshots may be transient but must be stable for each synchronous call. */
typedef bool (*Nf18b6B2Snapshot)(void *context,const NfWorld *world,
    const Nf18a6Runtime *runtime,uint32_t action,
    Nf18b2Scene *scene,Nf18b2Use *use);
typedef struct Nf18b6Push {
    Nf18a6Runtime *runtime; /* borrowed, bound to world by existing A6 APIs */
    Nf18b3Event event;
    Nf18b3Result last_result;
    uint32_t contract_epoch;
    uint32_t before_contacts, before_history;
    Nf18b6PushReceipt receipts[NF18B6_PUSH_RECEIPTS];
    uint32_t receipt_count;
    bool armed;
    const char *h24_envelope_path; /* opt-in prepublication checkpoint validation */
    const char *h20_receipt_journal; /* borrowed, optional append-only physical receipt file */
    bool require_h9; /* opt-in single-use owner-held authorization */
    Nf18b6B2Snapshot b2_snapshot;
    void *b2_context;
    Nf18b2Model b2_model;
    Nf18b2Status last_b2_status;
} Nf18b6Push;
/* Attach only to a previously initialized A6 runtime with unused hooks
 * and an unbound world tick. Bind A6 via nf18a6_bind_world_tick afterward.
 */
bool nf18b6_push_attach(Nf18b6Push *ctx,Nf18a6Runtime *runtime,
                        const NfWorld *world,uint32_t contract_epoch);
/* The last two eligibility booleans MUST derive from trusted A3/A5 and
 * server-owned contractual information; client bits are not evidence.
 * An armed action only records a receipt if real A4 impulse and A5 history
 * were committed. Idle movement remains possible without an action.
 */
Nf18b3Result nf18b6_push_arm(Nf18b6Push *ctx,const NfWorld *world,
                             uint32_t action,bool physical,bool contract,
                             bool uncertain);
/* Opt into the mandatory B2 gate. Once enabled, old Boolean arming is
 * disabled to prevent accidentally bypassing the trusted B2 evaluation.
 * The provider is called at proposal and again at A6 begin/prepublication.
 */
bool nf18b6_push_require_b2(Nf18b6Push *ctx,Nf18b6B2Snapshot provider,
    void *context,Nf18b2Model model);
Nf18b3Result nf18b6_push_arm_b2(Nf18b6Push *ctx,const NfWorld *world,
    uint32_t action);
/* Once enabled, requires a source-bound H9 authorization inside A6 prepublish. */
/* Source-bound postcommit receipt journal (not a joint atomic WAL). */
/* H26: restore unified persisted physical and typed B3 consequences together. */
bool nf18b6_push_h26_recover(NfWorld *world,Nf18a6Runtime *runtime,
 Nf18b6Push *push,const char *envelope);
bool nf18b6_push_h24_envelope(Nf18b6Push *ctx,const char *path);
bool nf18b6_push_h20_journal(Nf18b6Push *ctx,const char *path);
bool nf18b6_push_require_h9(Nf18b6Push *ctx);
bool nf18b6_push_detach(Nf18b6Push *ctx,Nf18a6Runtime *runtime,
                         const NfWorld *world);
#endif
