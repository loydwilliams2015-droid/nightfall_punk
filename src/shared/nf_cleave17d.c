#include "nf_cleave17d.h"

#include <string.h>

static bool component_version_matches(
    Nf17bComponentMask bit,
    Nf17bComponentMask dependency_mask,
    uint32_t stamped,
    uint32_t current) {
    return (dependency_mask & bit) == 0u || stamped == current;
}

void nf17d_policy_default(Nf17dCleavePolicy *policy) {
    if (policy == NULL) return;
    memset(policy, 0, sizeof(*policy));

    for (size_t i = 0u; i < NF17D_SUBSYS_COUNT; ++i) {
        policy->subsystem[i].evidence_level = (uint8_t)NF17D_EVIDENCE_H0_CONCEPTUAL;
        policy->subsystem[i].disposition = (uint8_t)NF17D_CLEAVE_RETAIN_EXPERIMENTAL;
    }

    policy->subsystem[NF17D_SUBSYS_SNAPSHOT_TRANSACTIONS] =
        (Nf17dSubsystemStatus){NF17D_EVIDENCE_H1_SYNTHETIC, NF17D_CLEAVE_PROMOTE, 0u};
    policy->subsystem[NF17D_SUBSYS_TOPOLOGY_AUTHORITY] =
        (Nf17dSubsystemStatus){NF17D_EVIDENCE_H2_CONTROLLED, NF17D_CLEAVE_PROMOTE, 0u};
    policy->subsystem[NF17D_SUBSYS_CALIBRATION] =
        (Nf17dSubsystemStatus){NF17D_EVIDENCE_H1_SYNTHETIC, NF17D_CLEAVE_PROMOTE, 0u};
    policy->subsystem[NF17D_SUBSYS_LEXICOGRAPHIC_UTILITY] =
        (Nf17dSubsystemStatus){NF17D_EVIDENCE_H1_SYNTHETIC, NF17D_CLEAVE_PROMOTE, 0u};
    policy->subsystem[NF17D_SUBSYS_REASON_TRACE] =
        (Nf17dSubsystemStatus){NF17D_EVIDENCE_H1_SYNTHETIC, NF17D_CLEAVE_PROMOTE, 0u};
    policy->subsystem[NF17D_SUBSYS_VERSIONED_CACHE] =
        (Nf17dSubsystemStatus){NF17D_EVIDENCE_H1_SYNTHETIC, NF17D_CLEAVE_PROMOTE, 0u};

    policy->subsystem[NF17D_SUBSYS_PURPLE] =
        (Nf17dSubsystemStatus){NF17D_EVIDENCE_H0_CONCEPTUAL, NF17D_CLEAVE_RETAIN_EXPERIMENTAL, 0u};
    policy->subsystem[NF17D_SUBSYS_CASCADE] =
        (Nf17dSubsystemStatus){NF17D_EVIDENCE_H0_CONCEPTUAL, NF17D_CLEAVE_RETAIN_EXPERIMENTAL, 0u};
    policy->subsystem[NF17D_SUBSYS_REFINEMENT] =
        (Nf17dSubsystemStatus){NF17D_EVIDENCE_H0_CONCEPTUAL, NF17D_CLEAVE_RETAIN_EXPERIMENTAL, 0u};

    policy->subsystem[NF17D_SUBSYS_DISTRIBUTED_BELIEF_CHALLENGER] =
        (Nf17dSubsystemStatus){NF17D_EVIDENCE_H1_SYNTHETIC, NF17D_CLEAVE_DISABLE, 0u};
    policy->subsystem[NF17D_SUBSYS_GLOBAL_PRIORITY_ARBITRATION] =
        (Nf17dSubsystemStatus){NF17D_EVIDENCE_H0_CONCEPTUAL, NF17D_CLEAVE_PRUNE, 0u};

    policy->challenger_runtime_enabled = 0u;
    policy->observer_can_mutate_authority = 0u;
    policy->force_winner_on_unresolved = 0u;
    policy->semantic_only_physics = 0u;
}

bool nf17d_policy_is_safe(const Nf17dCleavePolicy *policy) {
    if (policy == NULL) return false;
    if (policy->observer_can_mutate_authority != 0u) return false;
    if (policy->force_winner_on_unresolved != 0u) return false;
    if (policy->semantic_only_physics != 0u) return false;
    if (policy->subsystem[NF17D_SUBSYS_SNAPSHOT_TRANSACTIONS].disposition !=
        (uint8_t)NF17D_CLEAVE_PROMOTE) return false;
    if (policy->subsystem[NF17D_SUBSYS_TOPOLOGY_AUTHORITY].disposition !=
        (uint8_t)NF17D_CLEAVE_PROMOTE) return false;
    return true;
}

static bool transactions_interact(const Nf17bTransaction *a, const Nf17bTransaction *b) {
    if (a == NULL || b == NULL || a->tick != b->tick) return false;

    if (a->target_id == b->target_id &&
        nf17b_classify_conflict(a, b) != NF17B_CONFLICT_COMPATIBLE) {
        return true;
    }

    const bool same_scope =
        a->scope_id != 0u &&
        a->scope_id == b->scope_id;

    const Nf17bComponentMask hazard =
        (a->write_mask & (b->read_mask | b->write_mask)) |
        (b->write_mask & (a->read_mask | a->write_mask));

    return same_scope && hazard != 0u;
}

size_t nf17d_build_conflict_sets(
    const Nf17bTransaction *transactions,
    size_t transaction_count,
    Nf17dConflictSet *out,
    size_t out_capacity) {

    if (transactions == NULL || transaction_count == 0u) return 0u;
    if (transaction_count > NF17D_MAX_TRANSACTIONS) transaction_count = NF17D_MAX_TRANSACTIONS;

    uint16_t parent[NF17D_MAX_TRANSACTIONS];
    bool interacting[NF17D_MAX_TRANSACTIONS];
    for (size_t i = 0u; i < transaction_count; ++i) {
        parent[i] = (uint16_t)i;
        interacting[i] = false;
    }

    for (size_t i = 0u; i < transaction_count; ++i) {
        for (size_t j = i + 1u; j < transaction_count; ++j) {
            if (!transactions_interact(&transactions[i], &transactions[j])) continue;

            interacting[i] = true;
            interacting[j] = true;

            uint16_t ri = (uint16_t)i;
            while (parent[ri] != ri) ri = parent[ri];
            uint16_t rj = (uint16_t)j;
            while (parent[rj] != rj) rj = parent[rj];
            if (ri != rj) parent[rj] = ri;
        }
    }

    for (size_t i = 0u; i < transaction_count; ++i) {
        uint16_t root = (uint16_t)i;
        while (parent[root] != root) root = parent[root];
        uint16_t node = (uint16_t)i;
        while (parent[node] != node) {
            const uint16_t next = parent[node];
            parent[node] = root;
            node = next;
        }
    }

    uint16_t roots[NF17D_MAX_CONFLICT_SETS];
    size_t produced = 0u;
    for (size_t i = 0u; i < transaction_count; ++i) {
        if (!interacting[i]) continue;
        const uint16_t root = parent[i];

        size_t set_index = produced;
        for (size_t s = 0u; s < produced; ++s) {
            if (roots[s] == root) {
                set_index = s;
                break;
            }
        }

        if (set_index == produced) {
            if (produced >= NF17D_MAX_CONFLICT_SETS) break;
            roots[produced] = root;
            if (out != NULL && produced < out_capacity) memset(&out[produced], 0, sizeof(out[produced]));
            ++produced;
        }

        if (out != NULL && set_index < out_capacity) {
            Nf17dConflictSet *set = &out[set_index];
            if (set->count < NF17D_MAX_CONFLICT_MEMBERS) {
                set->member[set->count++] = (uint16_t)i;
            }
            set->combined_read_mask |= transactions[i].read_mask;
            set->combined_write_mask |= transactions[i].write_mask;
        }
    }

    return produced;
}

size_t nf17d_build_scoped_purple_envelopes(
    const Nf17cDomainDependency *deps,
    size_t dep_count,
    uint32_t tick,
    Nf17dScopedPurpleEnvelope *out,
    size_t out_capacity) {

    if (deps == NULL || dep_count == 0u) return 0u;
    if (dep_count > NF17D_MAX_DEPENDENCIES) dep_count = NF17D_MAX_DEPENDENCIES;

    uint32_t seen_targets[NF17D_MAX_DEPENDENCIES];
    size_t seen_count = 0u;
    size_t produced = 0u;

    for (size_t i = 0u; i < dep_count; ++i) {
        const uint32_t target = deps[i].target_id;
        bool seen = false;
        for (size_t t = 0u; t < seen_count; ++t) {
            if (seen_targets[t] == target) {
                seen = true;
                break;
            }
        }
        if (seen) continue;
        seen_targets[seen_count++] = target;

        Nf17cDomainDependency local[NF17D_MAX_DEPENDENCIES];
        size_t local_count = 0u;
        for (size_t j = 0u; j < dep_count; ++j) {
            if (deps[j].target_id == target && local_count < NF17D_MAX_DEPENDENCIES) {
                local[local_count++] = deps[j];
            }
        }

        Nf17cPurpleEnvelope temporary[NF17C_MAX_PURPLE_ENVELOPES];
        const size_t found = nf17c_build_purple_envelopes(
            local, local_count, tick, temporary, NF17C_MAX_PURPLE_ENVELOPES);

        for (size_t p = 0u; p < found; ++p) {
            if (out != NULL && produced < out_capacity && p < NF17C_MAX_PURPLE_ENVELOPES) {
                out[produced].envelope = temporary[p];
                out[produced].target_id = target;
            }
            ++produced;
        }
    }

    return produced;
}

Nf17dCacheStamp nf17d_cache_stamp(
    const Nf17bAuthoritativeState *state,
    Nf17bComponentMask dependency_mask) {

    Nf17dCacheStamp stamp;
    memset(&stamp, 0, sizeof(stamp));
    stamp.dependency_mask = dependency_mask;
    if (state == NULL) return stamp;

    stamp.topology_version = state->topology_version;
    stamp.material_version = state->material_version;
    stamp.contract_version = state->contract_version;
    stamp.occupancy_version = state->occupancy_version;
    stamp.resource_version = state->resource_version;
    stamp.support_version = state->support_version;
    return stamp;
}

bool nf17d_cache_valid(
    const Nf17dCacheStamp *stamp,
    const Nf17bAuthoritativeState *state) {

    if (stamp == NULL || state == NULL) return false;

    return
        component_version_matches(NF17B_COMPONENT_TOPOLOGY, stamp->dependency_mask,
                                  stamp->topology_version, state->topology_version) &&
        component_version_matches(NF17B_COMPONENT_MATERIAL, stamp->dependency_mask,
                                  stamp->material_version, state->material_version) &&
        component_version_matches(NF17B_COMPONENT_CONTRACT, stamp->dependency_mask,
                                  stamp->contract_version, state->contract_version) &&
        component_version_matches(NF17B_COMPONENT_OCCUPANCY, stamp->dependency_mask,
                                  stamp->occupancy_version, state->occupancy_version) &&
        component_version_matches(NF17B_COMPONENT_RESOURCE, stamp->dependency_mask,
                                  stamp->resource_version, state->resource_version) &&
        component_version_matches(NF17B_COMPONENT_SUPPORT, stamp->dependency_mask,
                                  stamp->support_version, state->support_version);
}

void nf17d_budget_init(
    Nf17dBudgetPool *pool,
    const uint32_t floor[NF17B_DOMAIN_COUNT],
    uint32_t shared_surplus,
    uint32_t emergency_reserve) {

    if (pool == NULL) return;
    memset(pool, 0, sizeof(*pool));
    pool->shared_surplus = shared_surplus;
    pool->emergency_reserve = emergency_reserve;

    for (size_t i = 0u; i < NF17B_DOMAIN_COUNT; ++i) {
        const uint32_t value = floor == NULL ? 0u : floor[i];
        pool->floor[i] = value;
        pool->remaining[i] = value;
    }
}

uint32_t nf17d_budget_request(
    Nf17dBudgetPool *pool,
    Nf17bDomain domain,
    uint32_t units,
    bool critical) {

    if (pool == NULL || domain < 0 || domain >= NF17B_DOMAIN_COUNT || units == 0u) return 0u;

    uint32_t granted = 0u;
    const size_t index = (size_t)domain;

    const uint32_t own = pool->remaining[index] < units ? pool->remaining[index] : units;
    pool->remaining[index] -= own;
    units -= own;
    granted += own;

    if (units > 0u) {
        const uint32_t shared = pool->shared_surplus < units ? pool->shared_surplus : units;
        pool->shared_surplus -= shared;
        pool->borrowed[index] += shared;
        units -= shared;
        granted += shared;
    }

    if (critical && units > 0u) {
        const uint32_t emergency = pool->emergency_reserve < units ? pool->emergency_reserve : units;
        pool->emergency_reserve -= emergency;
        pool->borrowed[index] += emergency;
        granted += emergency;
    }

    return granted;
}

void nf17d_budget_release_domain(Nf17dBudgetPool *pool, Nf17bDomain domain) {
    if (pool == NULL || domain < 0 || domain >= NF17B_DOMAIN_COUNT) return;
    const size_t index = (size_t)domain;
    pool->shared_surplus += pool->remaining[index];
    pool->remaining[index] = 0u;
}

bool nf17d_reason_trace_complete(const Nf17dReasonTraceRecord *record) {
    if (record == NULL) return false;
    if (record->consequential == 0u) return true;

    const uint32_t required =
        NF17D_TRACE_EVIDENCE |
        NF17D_TRACE_BELIEF |
        NF17D_TRACE_CONTRACT |
        NF17D_TRACE_AFFORDANCE |
        NF17D_TRACE_TRANSACTION |
        NF17D_TRACE_CONFIDENCE;

    if ((record->present_mask & required) != required) return false;
    if (record->trace.calibrated_confidence < 0.0f ||
        record->trace.calibrated_confidence > 1.0f) return false;
    return true;
}

uint32_t nf17d_overlay_mask(Nf17dViewPreset preset) {
    const uint32_t world =
        NF17D_OVERLAY_CELLS |
        NF17D_OVERLAY_NEXUSES |
        NF17D_OVERLAY_BOUNDARIES |
        NF17D_OVERLAY_MATERIAL;

    const uint32_t actor =
        NF17D_OVERLAY_EVIDENCE |
        NF17D_OVERLAY_BELIEF |
        NF17D_OVERLAY_CONFIDENCE |
        NF17D_OVERLAY_CONTRACTS;

    const uint32_t causal =
        NF17D_OVERLAY_TRANSACTIONS |
        NF17D_OVERLAY_CONFLICTS |
        NF17D_OVERLAY_PURPLE |
        NF17D_OVERLAY_FRONTIER |
        NF17D_OVERLAY_REFINEMENT |
        NF17D_OVERLAY_REASON_TRACE;

    switch (preset) {
        case NF17D_VIEW_PLAY: return 0u;
        case NF17D_VIEW_WORLD: return world;
        case NF17D_VIEW_ACTOR: return actor;
        case NF17D_VIEW_CAUSAL: return causal;
        case NF17D_VIEW_FULL: return world | actor | causal;
        default: return 0u;
    }
}

Nf17dObservabilityFrame nf17d_observe(
    const Nf17bAuthoritativeState *state,
    Nf17dViewPreset preset,
    uint16_t transaction_count,
    uint16_t conflict_set_count,
    uint16_t purple_count,
    uint16_t pending_count,
    const Nf17dReasonTraceRecord *trace) {

    Nf17dObservabilityFrame frame;
    memset(&frame, 0, sizeof(frame));
    if (state == NULL) return frame;

    frame.tick = state->tick;
    frame.authoritative_hash = nf17b_state_hash(state);
    frame.overlay_mask = nf17d_overlay_mask(preset);
    frame.transaction_count = transaction_count;
    frame.conflict_set_count = conflict_set_count;
    frame.purple_count = purple_count;
    frame.pending_count = pending_count;
    if (trace != NULL) frame.reason_trace_hash = nf17c_reason_trace_hash(&trace->trace);
    return frame;
}

const char *nf17d_evidence_level_name(Nf17dEvidenceLevel level) {
    static const char *names[] = {"H0-conceptual", "H1-synthetic", "H2-controlled", "H3-runtime", "H4-player"};
    return level >= NF17D_EVIDENCE_H0_CONCEPTUAL && level <= NF17D_EVIDENCE_H4_PLAYER
        ? names[level] : "unknown";
}

const char *nf17d_cleave_disposition_name(Nf17dCleaveDisposition disposition) {
    static const char *names[] = {"promote", "retain-experimental", "disable", "prune"};
    return disposition >= NF17D_CLEAVE_PROMOTE && disposition <= NF17D_CLEAVE_PRUNE
        ? names[disposition] : "unknown";
}

const char *nf17d_view_preset_name(Nf17dViewPreset preset) {
    static const char *names[] = {"play", "world", "actor", "causal", "full"};
    return preset >= NF17D_VIEW_PLAY && preset < NF17D_VIEW_COUNT
        ? names[preset] : "unknown";
}