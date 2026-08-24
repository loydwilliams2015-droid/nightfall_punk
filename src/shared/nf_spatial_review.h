#ifndef NF_SPATIAL_REVIEW_H
#define NF_SPATIAL_REVIEW_H

#include "nf_spatial_logic.h"

#include <stdbool.h>
#include <stdint.h>

#define NF_SPATIAL_REVIEW_MAX_PATH (NF_SPATIAL_MAX_STEPS + 1)

typedef enum NfSpatialReviewMode {
    NF_SPATIAL_REVIEW_INSTRUMENTED = 0,
    NF_SPATIAL_REVIEW_PERCEPTUAL = 1,
    NF_SPATIAL_REVIEW_GREEDY = 2,
    NF_SPATIAL_REVIEW_RANDOM = 3,
    NF_SPATIAL_REVIEW_MODE_COUNT = 4
} NfSpatialReviewMode;

typedef struct NfSpatialReviewResult {
    uint32_t seed;
    uint8_t lab;
    uint8_t mode;
    uint8_t replicate;
    uint8_t success;
    uint16_t steps;
    uint16_t unique_cells;
    uint16_t revisits;
    uint16_t retreats;
    uint16_t support_transitions;
    uint16_t affordance_accepts;
    uint16_t topology_updates;
    uint16_t route_discovery_step;
    uint16_t cue_disagreement_steps;
    uint16_t path_invalid_steps;
    float health_remaining;
    float exposure_accum;
    float ecology_cost_accum;
    float resource_collected;
    float contract_context_seen;
    uint32_t deterministic_hash;
} NfSpatialReviewResult;

typedef struct NfSpatialReviewTrace {
    NfSpatialWorld world_initial;
    NfSpatialWorld world_final;
    uint16_t path_len;
    int path[NF_SPATIAL_REVIEW_MAX_PATH];
    uint8_t known_final[NF_SPATIAL_MAX_CELLS];
    uint8_t visible_final[NF_SPATIAL_MAX_CELLS];
    NfSpatialReviewResult result;
} NfSpatialReviewTrace;

NfSpatialReviewResult nf_spatial_review_run(
    NfSpatialLabKind lab,
    NfSpatialReviewMode mode,
    uint32_t seed,
    uint8_t replicate,
    NfSpatialReviewTrace *trace);

const char *nf_spatial_review_mode_name(NfSpatialReviewMode mode);

#endif
