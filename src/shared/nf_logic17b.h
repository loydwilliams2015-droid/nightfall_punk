#ifndef NF_LOGIC17B_H
#define NF_LOGIC17B_H

#include "nf_spatial_logic.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF17B_FIXED_SCALE 1000
#define NF17B_MAX_FRONTIER 64u
#define NF17B_MAX_TRAIL_CELLS NF_SPATIAL_MAX_CELLS

typedef int32_t Nf17bFixed;

typedef enum Nf17bDomain {
    NF17B_DOMAIN_MATERIAL = 0,
    NF17B_DOMAIN_STRUCTURE = 1,
    NF17B_DOMAIN_ACTOR = 2,
    NF17B_DOMAIN_CONTRACT = 3,
    NF17B_DOMAIN_SEMANTIC = 4,
    NF17B_DOMAIN_NAVIGATION = 5,
    NF17B_DOMAIN_ECOLOGY = 6,
    NF17B_DOMAIN_COUNT = 7
} Nf17bDomain;

typedef uint32_t Nf17bComponentMask;
enum {
    NF17B_COMPONENT_TOPOLOGY = 1u << 0,
    NF17B_COMPONENT_MATERIAL = 1u << 1,
    NF17B_COMPONENT_CONTRACT = 1u << 2,
    NF17B_COMPONENT_OCCUPANCY = 1u << 3,
    NF17B_COMPONENT_RESOURCE = 1u << 4,
    NF17B_COMPONENT_SUPPORT = 1u << 5,
    NF17B_COMPONENT_EVIDENCE = 1u << 6
};

typedef enum Nf17bTxnKind {
    NF17B_TXN_MOVE = 0,
    NF17B_TXN_FORCE = 1,
    NF17B_TXN_DAMAGE = 2,
    NF17B_TXN_DESTROY = 3,
    NF17B_TXN_OPEN = 4,
    NF17B_TXN_CLOSE = 5,
    NF17B_TXN_CLAIM = 6,
    NF17B_TXN_TRANSFER = 7,
    NF17B_TXN_SUPPORT_LOAD = 8,
    NF17B_TXN_RESOURCE_DELTA = 9,
    NF17B_TXN_REPORT = 10,
    NF17B_TXN_COUNT = 11
} Nf17bTxnKind;

typedef enum Nf17bConflictKind {
    NF17B_CONFLICT_COMPATIBLE = 0,
    NF17B_CONFLICT_MERGEABLE = 1,
    NF17B_CONFLICT_EXCLUSIVE = 2,
    NF17B_CONFLICT_TRANSFORMING = 3
} Nf17bConflictKind;

typedef struct Nf17bAuthoritativeState {
    uint32_t tick;
    uint32_t state_version;
    uint32_t topology_version;
    uint32_t material_version;
    uint32_t contract_version;
    uint32_t occupancy_version;
    uint32_t resource_version;
    uint32_t support_version;
} Nf17bAuthoritativeState;

typedef struct Nf17bSnapshot {
    Nf17bAuthoritativeState state;
    uint32_t deterministic_hash;
} Nf17bSnapshot;

typedef struct Nf17bTransaction {
    uint32_t tick;
    uint32_t ordinal;
    uint32_t source_id;
    uint32_t target_id;
    uint32_t scope_id;
    uint8_t domain;
    uint8_t kind;
    uint16_t contract_authority;
    Nf17bComponentMask precondition_mask;
    Nf17bComponentMask read_mask;
    Nf17bComponentMask write_mask;
    Nf17bComponentMask material_dependency_mask;
    Nf17bFixed payload;
    uint32_t provenance_hash;
} Nf17bTransaction;

typedef enum Nf17bAgreementStage {
    NF17B_AGREEMENT_PROPOSED = 0,
    NF17B_AGREEMENT_PROVISIONAL = 1,
    NF17B_AGREEMENT_RESERVED = 2,
    NF17B_AGREEMENT_COMMITTED = 3,
    NF17B_AGREEMENT_EXECUTING = 4,
    NF17B_AGREEMENT_COMPLETED = 5,
    NF17B_AGREEMENT_CANCELLED = 6,
    NF17B_AGREEMENT_FAILED = 7
} Nf17bAgreementStage;

typedef enum Nf17bCancelResult {
    NF17B_CANCEL_ROLLED_BACK = 0,
    NF17B_CANCEL_COMPENSATION_REQUIRED = 1
} Nf17bCancelResult;

typedef struct Nf17bAgreement {
    uint32_t agreement_id;
    uint8_t stage;
    uint8_t surfaced_consequence;
    uint16_t reserved;
} Nf17bAgreement;

typedef struct Nf17bCrossNexusTransfer {
    uint32_t source_nexus;
    uint32_t target_nexus;
    uint32_t material_channel_mask;
    Nf17bFixed magnitude;
    uint8_t target_state_changed;
    uint8_t threshold_crossed;
    uint16_t reserved;
} Nf17bCrossNexusTransfer;

typedef enum Nf17bFrontierPolicy {
    NF17B_FRONTIER_CAUSAL = 0,
    NF17B_FRONTIER_SEVERITY = 1,
    NF17B_FRONTIER_LOCALITY = 2,
    NF17B_FRONTIER_UISR_HIERARCHICAL = 3
} Nf17bFrontierPolicy;

typedef struct Nf17bFrontierItem {
    uint32_t id;
    uint32_t target_id;
    uint16_t causal_depth;
    uint16_t age;
    uint16_t severity;
    uint16_t locality;
    uint8_t domain;
    uint8_t consequential;
    uint16_t reserved;
} Nf17bFrontierItem;

typedef struct Nf17bFrontier {
    Nf17bFrontierItem items[NF17B_MAX_FRONTIER];
    size_t count;
} Nf17bFrontier;

typedef struct Nf17bRefinementMetrics {
    float causal_precision;
    float causal_recall;
    float fidelity_gain;
    float cost_efficiency;
    float horizon_gain;
} Nf17bRefinementMetrics;

typedef enum Nf17bRefinementDecision {
    NF17B_REFINEMENT_KEEP = 0,
    NF17B_REFINEMENT_REFINE = 1,
    NF17B_REFINEMENT_REAGGREGATE = 2
} Nf17bRefinementDecision;

typedef struct Nf17bTraversalProfile {
    uint32_t allowed_support_mask;
    uint8_t horizon;
    uint8_t observation_radius;
    uint16_t reserved;
    float exposure_weight;
    float ecology_weight;
    float resource_reward;
    float branch_reward;
    float affordance_reward;
    float contract_context_weight;
} Nf17bTraversalProfile;

typedef struct Nf17bRouteHypothesis {
    int next_cell;
    int frontier_cell;
    uint8_t reached_goal;
    uint8_t depth;
    uint16_t branches_preserved;
    float cost;
} Nf17bRouteHypothesis;

typedef struct Nf17bTrailField {
    uint16_t owner_actor;
    uint8_t shared;
    uint8_t reserved;
    float value[NF17B_MAX_TRAIL_CELLS];
} Nf17bTrailField;

Nf17bFixed nf17b_fixed_from_float(float value);
float nf17b_fixed_to_float(Nf17bFixed value);

void nf17b_state_init(Nf17bAuthoritativeState *state);
void nf17b_advance_tick(Nf17bAuthoritativeState *state);
void nf17b_commit_write_mask(Nf17bAuthoritativeState *state, Nf17bComponentMask write_mask);
uint32_t nf17b_state_hash(const Nf17bAuthoritativeState *state);
Nf17bSnapshot nf17b_snapshot(const Nf17bAuthoritativeState *state);

Nf17bConflictKind nf17b_classify_conflict(const Nf17bTransaction *a, const Nf17bTransaction *b);
bool nf17b_transaction_valid_for_snapshot(const Nf17bTransaction *txn, const Nf17bSnapshot *snapshot);

Nf17bCancelResult nf17b_cancel_agreement(Nf17bAgreement *agreement);
bool nf17b_cross_nexus_surfaces(const Nf17bCrossNexusTransfer *transfer);

void nf17b_frontier_init(Nf17bFrontier *frontier);
bool nf17b_frontier_push(Nf17bFrontier *frontier, Nf17bFrontierItem item);
bool nf17b_frontier_pop(Nf17bFrontier *frontier, Nf17bFrontierPolicy policy, Nf17bFrontierItem *out);
void nf17b_frontier_age(Nf17bFrontier *frontier);

Nf17bRefinementDecision nf17b_refinement_decide(
    Nf17bRefinementMetrics metrics,
    float refine_threshold,
    float reaggregate_threshold);

Nf17bRouteHypothesis nf17b_route_hypothesis(
    const NfSpatialWorld *world,
    int start,
    int goal,
    const Nf17bTraversalProfile *profile);

void nf17b_trail_init(Nf17bTrailField *field, uint16_t owner_actor, bool shared);
void nf17b_trail_deposit(Nf17bTrailField *field, int cell, float amount);
void nf17b_trail_decay(Nf17bTrailField *field, float retain);

const char *nf17b_conflict_name(Nf17bConflictKind conflict);
const char *nf17b_frontier_policy_name(Nf17bFrontierPolicy policy);

#endif
