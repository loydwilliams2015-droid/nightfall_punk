#ifndef NF_CLAIM_H
#define NF_CLAIM_H

#include "nf_world.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_CLAIM_CAPACITY 64u

typedef enum NfClaimKind {
    NF_CLAIM_NONE = 0,
    NF_CLAIM_COVER,
    NF_CLAIM_TRAVERSAL,
    NF_CLAIM_INTERACTION,
    NF_CLAIM_REVIVE
} NfClaimKind;

typedef enum NfClaimStrength {
    NF_CLAIM_SOFT = 0,
    NF_CLAIM_HARD
} NfClaimStrength;

typedef enum NfClaimResult {
    NF_CLAIM_RESULT_INVALID = 0,
    NF_CLAIM_RESULT_GRANTED,
    NF_CLAIM_RESULT_SHARED,
    NF_CLAIM_RESULT_BLOCKED
} NfClaimResult;

typedef struct NfClaim {
    bool active;
    uint32_t id;
    NfClaimKind kind;
    NfClaimStrength strength;
    NfEntityId claimant;
    uint32_t subject_key;
    uint64_t acquired_tick;
    uint64_t expires_tick;
} NfClaim;

typedef struct NfClaimTable {
    NfClaim claims[NF_CLAIM_CAPACITY];
    uint32_t next_id;
} NfClaimTable;

void nf_claim_table_init(NfClaimTable *table);
void nf_claim_expire(NfClaimTable *table, uint64_t now_tick);
NfClaimResult nf_claim_try_acquire(
    NfClaimTable *table, NfClaimKind kind, NfClaimStrength strength,
    NfEntityId claimant, uint32_t subject_key, uint64_t now_tick,
    uint64_t duration_ticks, uint32_t *claim_id_out);
bool nf_claim_release(
    NfClaimTable *table, NfClaimKind kind, NfEntityId claimant,
    uint32_t subject_key);
bool nf_claim_is_blocked(
    const NfClaimTable *table, NfClaimKind kind, NfEntityId claimant,
    uint32_t subject_key, uint64_t now_tick);
NfEntityId nf_claim_hard_owner(
    const NfClaimTable *table, NfClaimKind kind, uint32_t subject_key,
    uint64_t now_tick);
size_t nf_claim_live_count(const NfClaimTable *table, uint64_t now_tick);
const char *nf_claim_kind_name(NfClaimKind kind);
const char *nf_claim_result_name(NfClaimResult result);

#endif
