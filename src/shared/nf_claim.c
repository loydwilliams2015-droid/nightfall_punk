#include "nf_claim.h"

#include <string.h>

void nf_claim_table_init(NfClaimTable *table) {
    if (table == NULL) return;
    memset(table, 0, sizeof(*table));
    table->next_id = 1u;
}

void nf_claim_expire(NfClaimTable *table, uint64_t now_tick) {
    if (table == NULL) return;
    for (size_t i = 0u; i < NF_CLAIM_CAPACITY; ++i) {
        NfClaim *claim = &table->claims[i];
        if (claim->active && now_tick >= claim->expires_tick) memset(claim, 0, sizeof(*claim));
    }
}

static NfClaim *free_slot(NfClaimTable *table) {
    for (size_t i = 0u; i < NF_CLAIM_CAPACITY; ++i) if (!table->claims[i].active) return &table->claims[i];
    return NULL;
}

NfClaimResult nf_claim_try_acquire(
    NfClaimTable *table, NfClaimKind kind, NfClaimStrength strength,
    NfEntityId claimant, uint32_t subject_key, uint64_t now_tick,
    uint64_t duration_ticks, uint32_t *claim_id_out) {
    if (table == NULL || kind == NF_CLAIM_NONE || claimant == 0u || subject_key == 0u || duration_ticks == 0u)
        return NF_CLAIM_RESULT_INVALID;
    nf_claim_expire(table, now_tick);
    bool shared = false;
    for (size_t i = 0u; i < NF_CLAIM_CAPACITY; ++i) {
        NfClaim *claim = &table->claims[i];
        if (!claim->active || claim->kind != kind || claim->subject_key != subject_key) continue;
        if (claim->claimant == claimant) {
            claim->strength = strength;
            claim->expires_tick = now_tick + duration_ticks;
            if (claim_id_out != NULL) *claim_id_out = claim->id;
            return NF_CLAIM_RESULT_GRANTED;
        }
        if (claim->strength == NF_CLAIM_HARD || strength == NF_CLAIM_HARD) return NF_CLAIM_RESULT_BLOCKED;
        shared = true;
    }
    NfClaim *slot = free_slot(table);
    if (slot == NULL) return NF_CLAIM_RESULT_BLOCKED;
    if (table->next_id == 0u) table->next_id = 1u;
    *slot = (NfClaim){true,table->next_id++,kind,strength,claimant,subject_key,now_tick,now_tick+duration_ticks};
    if (claim_id_out != NULL) *claim_id_out = slot->id;
    return shared ? NF_CLAIM_RESULT_SHARED : NF_CLAIM_RESULT_GRANTED;
}

bool nf_claim_release(NfClaimTable *table, NfClaimKind kind, NfEntityId claimant, uint32_t subject_key) {
    if (table == NULL) return false;
    bool released = false;
    for (size_t i = 0u; i < NF_CLAIM_CAPACITY; ++i) {
        NfClaim *claim = &table->claims[i];
        if (claim->active && claim->kind == kind && claim->claimant == claimant && claim->subject_key == subject_key) {
            memset(claim, 0, sizeof(*claim)); released = true;
        }
    }
    return released;
}

bool nf_claim_is_blocked(const NfClaimTable *table, NfClaimKind kind, NfEntityId claimant, uint32_t subject_key, uint64_t now_tick) {
    if (table == NULL) return false;
    for (size_t i = 0u; i < NF_CLAIM_CAPACITY; ++i) {
        const NfClaim *claim = &table->claims[i];
        if (claim->active && claim->kind == kind && claim->subject_key == subject_key && now_tick < claim->expires_tick &&
            claim->claimant != claimant && claim->strength == NF_CLAIM_HARD) return true;
    }
    return false;
}

NfEntityId nf_claim_hard_owner(const NfClaimTable *table, NfClaimKind kind, uint32_t subject_key, uint64_t now_tick) {
    if (table == NULL) return 0u;
    for (size_t i = 0u; i < NF_CLAIM_CAPACITY; ++i) {
        const NfClaim *claim = &table->claims[i];
        if (claim->active && claim->kind == kind && claim->subject_key == subject_key && claim->strength == NF_CLAIM_HARD && now_tick < claim->expires_tick)
            return claim->claimant;
    }
    return 0u;
}

size_t nf_claim_live_count(const NfClaimTable *table, uint64_t now_tick) {
    if (table == NULL) return 0u;
    size_t count = 0u;
    for (size_t i = 0u; i < NF_CLAIM_CAPACITY; ++i) if (table->claims[i].active && now_tick < table->claims[i].expires_tick) ++count;
    return count;
}

const char *nf_claim_kind_name(NfClaimKind kind) {
    switch (kind) {
        case NF_CLAIM_COVER:return "COVER"; case NF_CLAIM_TRAVERSAL:return "TRAVERSAL";
        case NF_CLAIM_INTERACTION:return "INTERACTION"; case NF_CLAIM_REVIVE:return "REVIVE"; default:return "NONE";
    }
}
const char *nf_claim_result_name(NfClaimResult result) {
    switch (result) {
        case NF_CLAIM_RESULT_GRANTED:return "GRANTED"; case NF_CLAIM_RESULT_SHARED:return "SHARED";
        case NF_CLAIM_RESULT_BLOCKED:return "BLOCKED"; default:return "INVALID";
    }
}
