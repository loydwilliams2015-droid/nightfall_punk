#ifndef NF_CLEAVE17D_H
#define NF_CLEAVE17D_H

#include "nf_logic17c.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF17D_MAX_TRANSACTIONS 32u
#define NF17D_MAX_CONFLICT_SETS 16u
#define NF17D_MAX_CONFLICT_MEMBERS 16u
#define NF17D_MAX_CACHE_ENTRIES 32u

typedef enum Nf17dEvidenceLevel {
    NF17D_EVIDENCE_H0_CONCEPTUAL = 0,
    NF17D_EVIDENCE_H1_SYNTHETIC = 1,
    NF17D_EVIDENCE_H2_CONTROLLED = 2,
    NF17D_EVIDENCE_H3_RUNTIME = 3,
    NF17D_EVIDENCE_H4_PLAYER = 4
} Nf17dEvidenceLevel;

typedef enum Nf17dCleaveDisposition {
    NF17D_CLEAVE_PROMOTE = 0,
    NF17D_CLEAVE_RETAIN_EXPERIMENTAL = 1,
    NF17D_CLEAVE_DISABLE = 2,
    NF17D_CLEAVE_PRUNE = 3
} Nf17dCleaveDisposition;

typedef enum Nf17dSubsystem {
    NF17D_SUBSYS_SNAPSHOT_TRANSACTIONS = 0,
    NF17D_SUBSYS_TOPOLOGY_AUTHORITY = 1,
    NF17D_SUBSYS_CALIBRATION = 2,
    NF17D_SUBSYS_LEXICOGRAPHIC_UTILITY = 3,
    NF17D_SUBSYS_REASON_TRACE = 4,
    NF17D_SUBSYS_VERSIONED_CACHE = 5,
    NF17D_SUBSYS_PURPLE = 6,
    NF17D_SUBSYS_CASCADE = 7,
    NF17D_SUBSYS_REFINEMENT = 8,
    NF17D_SUBSYS_DISTRIBUTED_BELIEF_CHALLENGER = 9,
    NF17D_SUBSYS_GLOBAL_PRIORITY_ARBITRATION = 10,
    NF17D_SUBSYS_COUNT = 11
} Nf17dSubsystem;

typedef struct Nf17dSubsystemStatus {
    uint8_t evidence_level;
    uint8_t disposition;
    uint16_t reserved;
} Nf17dSubsystemStatus;

typedef struct Nf17dCleavePolicy {
    Nf17dSubsystemStatus subsystem[NF17D_SUBSYS_COUNT];
    uint8_t challenger_runtime_enabled;
    uint8_t observer_can_mutate_authority;
    uint8_t force_winner_on_unresolved;
    uint8_t semantic_only_physics;
} Nf17dCleavePolicy;

typedef struct Nf17dConflictSet {
    uint16_t count;
    uint16_t member[NF17D_MAX_CONFLICT_MEMBERS];
    Nf17bComponentMask combined_read_mask;
    Nf17bComponentMask combined_write_mask;
} Nf17dConflictSet;

typedef struct Nf17dCacheStamp {
    Nf17bComponentMask dependency_mask;
    uint32_t topology_version;
    uint32_t material_version;
    uint32_t contract_version;
    uint32_t occupancy_version;
    uint32_t resource_version;
    uint32_t support_version;
} Nf17dCacheStamp;

typedef struct Nf17dBudgetPool {
    uint32_t floor[NF17B_DOMAIN_COUNT];
    uint32_t remaining[NF17B_DOMAIN_COUNT];
    uint32_t shared_surplus;
    uint32_t emergency_reserve;
    uint32_t borrowed[NF17B_DOMAIN_COUNT];
} Nf17dBudgetPool;

typedef enum Nf17dTraceField {
    NF17D_TRACE_EVIDENCE = 1u << 0,
    NF17D_TRACE_BELIEF = 1u << 1,
    NF17D_TRACE_CONTRACT = 1u << 2,
    NF17D_TRACE_AFFORDANCE = 1u << 3,
    NF17D_TRACE_TRANSACTION = 1u << 4,
    NF17D_TRACE_CONFIDENCE = 1u << 5
} Nf17dTraceField;

typedef struct Nf17dReasonTraceRecord {
    Nf17cReasonTrace trace;
    uint32_t present_mask;
    uint8_t consequential;
    uint8_t reserved[3];
} Nf17dReasonTraceRecord;

typedef enum Nf17dViewPreset {
    NF17D_VIEW_PLAY = 0,
    NF17D_VIEW_WORLD = 1,
    NF17D_VIEW_ACTOR = 2,
    NF17D_VIEW_CAUSAL = 3,
    NF17D_VIEW_FULL = 4,
    NF17D_VIEW_COUNT = 5
} Nf17dViewPreset;

enum {
    NF17D_OVERLAY_CELLS = 1u << 0,
    NF17D_OVERLAY_NEXUSES = 1u << 1,
    NF17D_OVERLAY_BOUNDARIES = 1u << 2,
    NF17D_OVERLAY_MATERIAL = 1u << 3,
    NF17D_OVERLAY_EVIDENCE = 1u << 4,
    NF17D_OVERLAY_BELIEF = 1u << 5,
    NF17D_OVERLAY_CONFIDENCE = 1u << 6,
    NF17D_OVERLAY_CONTRACTS = 1u << 7,
    NF17D_OVERLAY_TRANSACTIONS = 1u << 8,
    NF17D_OVERLAY_CONFLICTS = 1u << 9,
    NF17D_OVERLAY_PURPLE = 1u << 10,
    NF17D_OVERLAY_FRONTIER = 1u << 11,
    NF17D_OVERLAY_REFINEMENT = 1u << 12,
    NF17D_OVERLAY_REASON_TRACE = 1u << 13
};

typedef struct Nf17dObservabilityFrame {
    uint32_t tick;
    uint32_t authoritative_hash;
    uint32_t overlay_mask;
    uint16_t transaction_count;
    uint16_t conflict_set_count;
    uint16_t purple_count;
    uint16_t pending_count;
    uint32_t reason_trace_hash;
} Nf17dObservabilityFrame;

void nf17d_policy_default(Nf17dCleavePolicy *policy);
bool nf17d_policy_is_safe(const Nf17dCleavePolicy *policy);

size_t nf17d_build_conflict_sets(
    const Nf17bTransaction *transactions,
    size_t transaction_count,
    Nf17dConflictSet *out,
    size_t out_capacity);

Nf17dCacheStamp nf17d_cache_stamp(
    const Nf17bAuthoritativeState *state,
    Nf17bComponentMask dependency_mask);
bool nf17d_cache_valid(
    const Nf17dCacheStamp *stamp,
    const Nf17bAuthoritativeState *state);

void nf17d_budget_init(
    Nf17dBudgetPool *pool,
    const uint32_t floor[NF17B_DOMAIN_COUNT],
    uint32_t shared_surplus,
    uint32_t emergency_reserve);
uint32_t nf17d_budget_request(
    Nf17dBudgetPool *pool,
    Nf17bDomain domain,
    uint32_t units,
    bool critical);
void nf17d_budget_release_domain(Nf17dBudgetPool *pool, Nf17bDomain domain);

bool nf17d_reason_trace_complete(const Nf17dReasonTraceRecord *record);

uint32_t nf17d_overlay_mask(Nf17dViewPreset preset);
Nf17dObservabilityFrame nf17d_observe(
    const Nf17bAuthoritativeState *state,
    Nf17dViewPreset preset,
    uint16_t transaction_count,
    uint16_t conflict_set_count,
    uint16_t purple_count,
    uint16_t pending_count,
    const Nf17dReasonTraceRecord *trace);

const char *nf17d_evidence_level_name(Nf17dEvidenceLevel level);
const char *nf17d_cleave_disposition_name(Nf17dCleaveDisposition disposition);
const char *nf17d_view_preset_name(Nf17dViewPreset preset);

#endif
