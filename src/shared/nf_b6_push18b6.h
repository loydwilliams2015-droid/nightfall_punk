#ifndef NF_B6_PUSH18B6_H
#define NF_B6_PUSH18B6_H
/* 1.8B.6 H1: B3 PUSH semantic receipt, bound to the EXISTING A6 owner.
 * It never drives physics or journals a separate authoritative world.
 * Only one actor/crate is supported. No network-authentication claim.
 */
#include "nf_embody18a6.h"
#include "nf_interact18b3.h"
#include <stdbool.h>
#include <stdint.h>
#define NF18B6_PUSH_RECEIPTS 64u
typedef struct Nf18b6PushReceipt {
    uint32_t actor, action, tick;
} Nf18b6PushReceipt;
typedef struct Nf18b6Push {
    Nf18a6Runtime *runtime; /* borrowed, bound to world by existing A6 APIs */
    Nf18b3Event event;
    Nf18b3Result last_result;
    uint32_t contract_epoch;
    uint32_t before_contacts, before_history;
    Nf18b6PushReceipt receipts[NF18B6_PUSH_RECEIPTS];
    uint32_t receipt_count;
    bool armed;
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
bool nf18b6_push_detach(Nf18b6Push *ctx,Nf18a6Runtime *runtime,
                         const NfWorld *world);
#endif
