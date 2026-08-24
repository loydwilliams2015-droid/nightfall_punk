#ifndef NF_SPATIAL_LOGIC_H
#define NF_SPATIAL_LOGIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_SPATIAL_MAX_W 16
#define NF_SPATIAL_MAX_H 16
#define NF_SPATIAL_MAX_CELLS (NF_SPATIAL_MAX_W * NF_SPATIAL_MAX_H)
#define NF_SPATIAL_MAX_OBJECTS 8
#define NF_SPATIAL_MAX_STEPS 128

#define NF_SPATIAL_UNASSIGNED 0u
#define NF_SPATIAL_ACTOR_PLAYER 1u
#define NF_SPATIAL_ACTOR_RIVAL 2u
#define NF_SPATIAL_ACTOR_COMMONS 3u

typedef enum NfSpatialLabKind {
    NF_SPATIAL_LAB_CORRIDOR = 0,
    NF_SPATIAL_LAB_LOOP = 1,
    NF_SPATIAL_LAB_VERTICAL = 2,
    NF_SPATIAL_LAB_COVER = 3,
    NF_SPATIAL_LAB_SNAP = 4,
    NF_SPATIAL_LAB_DYNAMIC = 5,
    NF_SPATIAL_LAB_ECOLOGY = 6,
    NF_SPATIAL_LAB_CONTRACT = 7,
    NF_SPATIAL_LAB_COUNT = 8
} NfSpatialLabKind;

typedef enum NfSpatialPolicyKind {
    NF_SPATIAL_POLICY_INFORMED = 0,
    NF_SPATIAL_POLICY_GREEDY = 1,
    NF_SPATIAL_POLICY_RANDOM = 2,
    NF_SPATIAL_POLICY_COUNT = 3
} NfSpatialPolicyKind;

typedef enum NfSupportKind {
    NF_SUPPORT_GROUND = 0,
    NF_SUPPORT_PLATFORM = 1,
    NF_SUPPORT_LADDER = 2,
    NF_SUPPORT_LEDGE = 3,
    NF_SUPPORT_AIR = 4
} NfSupportKind;

typedef struct NfSpatialCell {
    uint8_t solid;
    uint8_t authored_forbid_affordance;
    uint8_t support_kind;
    uint8_t dirty;
    float cover;
    float exposure;
    float resource;
    float ecological_risk;
    float affordance;
    float contract_memory;
} NfSpatialCell;

typedef struct NfSpatialWorld {
    int width;
    int height;
    uint32_t seed;
    uint32_t topology_revision;
    NfSpatialLabKind lab;
    NfSpatialCell cells[NF_SPATIAL_MAX_CELLS];
    uint16_t object_authority[NF_SPATIAL_MAX_OBJECTS];
    int object_cell[NF_SPATIAL_MAX_OBJECTS];
    int start_cell;
    int goal_cell;
} NfSpatialWorld;

typedef struct NfContractExchangeEvent {
    uint8_t object_id;
    uint16_t from_actor;
    uint16_t to_actor;
    float assigned_weight;
    float memory_decay;
    int cell;
    int radius;
    uint32_t tick;
} NfContractExchangeEvent;

typedef struct NfSpatialRoundResult {
    uint32_t seed;
    uint8_t lab;
    uint8_t policy;
    uint8_t replicate;
    uint8_t success;
    uint16_t steps;
    uint16_t unique_cells;
    uint16_t revisits;
    uint16_t retreats;
    uint16_t support_transitions;
    uint16_t affordance_accepts;
    uint16_t topology_updates;
    float health_remaining;
    float exposure_accum;
    float ecology_cost_accum;
    float resource_collected;
    float contract_context_seen;
    uint32_t deterministic_hash;
} NfSpatialRoundResult;

void nf_spatial_world_init(NfSpatialWorld *world, NfSpatialLabKind lab, uint32_t seed);
bool nf_spatial_is_valid_cell(const NfSpatialWorld *world, int cell);
int nf_spatial_cell_xy(const NfSpatialWorld *world, int x, int y);
int nf_spatial_neighbor_count(const NfSpatialWorld *world, int cell);
int nf_spatial_shortest_path(const NfSpatialWorld *world, int start, int goal);
bool nf_spatial_line_of_sight(const NfSpatialWorld *world, int from_cell, int to_cell);
size_t nf_spatial_observe_local(const NfSpatialWorld *world, int origin_cell, int radius, uint8_t *known, size_t known_count);
float nf_spatial_affordance_candidate(const NfSpatialWorld *world, int from_cell, int to_cell);
void nf_spatial_mark_dirty(NfSpatialWorld *world, int center_cell, int radius);
uint16_t nf_spatial_recompute_dirty(NfSpatialWorld *world, uint16_t budget);
bool nf_spatial_apply_contract_event(NfSpatialWorld *world, const NfContractExchangeEvent *event);
NfSpatialRoundResult nf_spatial_run_round(NfSpatialLabKind lab, NfSpatialPolicyKind policy, uint32_t seed, uint8_t replicate);
const char *nf_spatial_lab_name(NfSpatialLabKind lab);
const char *nf_spatial_policy_name(NfSpatialPolicyKind policy);

#endif
