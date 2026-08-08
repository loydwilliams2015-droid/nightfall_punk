#ifndef NF_SPATIAL_H
#define NF_SPATIAL_H

#include "nf_region.h"
#include "nf_semantics.h"
#include "nf_world.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_SPATIAL_MAX_AGENTS 4u
#define NF_SPATIAL_LOCAL_SAMPLES 5u

typedef enum NfSpatialTask {
    NF_SPATIAL_TASK_ROAM = 0,
    NF_SPATIAL_TASK_WATCH,
    NF_SPATIAL_TASK_INVESTIGATE,
    NF_SPATIAL_TASK_OCCUPY,
    NF_SPATIAL_TASK_PRESSURE,
    NF_SPATIAL_TASK_FLANK,
    NF_SPATIAL_TASK_REGROUP,
    NF_SPATIAL_TASK_CONTEST
} NfSpatialTask;

typedef enum NfSpatialInterrupt {
    NF_SPATIAL_INTERRUPT_NONE = 0,
    NF_SPATIAL_INTERRUPT_REEVALUATE,
    NF_SPATIAL_INTERRUPT_TASK_BREAK,
    NF_SPATIAL_INTERRUPT_EMERGENCY,
    NF_SPATIAL_INTERRUPT_AUTHORITATIVE
} NfSpatialInterrupt;

typedef struct NfSpatialFields {
    float route_congestion;
    float ally_support;
    float enemy_threat;
    float exposure;
    float pressure;
    float objective_value;
    float preference_score;
} NfSpatialFields;

typedef struct NfSpatialAgentState {
    NfEntityId actor_id;
    uint8_t current_region;
    uint8_t target_region;
    uint8_t next_region;
    NfSpatialTask task;
    NfSpatialInterrupt interrupt;
    NfVec3 local_goal;
    NfSpatialFields fields;
    bool local_goal_valid;
    uint64_t commit_until_tick;
    uint64_t next_region_tick;
    uint64_t next_local_tick;
    uint64_t claim_until_tick;
    uint64_t region_last_visit[NF_REGION_MAX];
    float support_bias;
    float risk_bias;
    float residency_bias;
} NfSpatialAgentState;

typedef struct NfSpatialSystem {
    NfRegionGraph graph;
    NfSpatialAgentState agents[NF_SPATIAL_MAX_AGENTS];
    size_t count;
    float activity[NF_REGION_MAX];
    float incident[NF_REGION_MAX];
    float objective_dynamic[NF_REGION_MAX];
    uint64_t last_semantic_tick;
    uint64_t next_decay_tick;
    uint32_t seed;
} NfSpatialSystem;

struct NfAiSystem;

void nf_spatial_init(
    NfSpatialSystem *spatial,
    const struct NfAiSystem *ai,
    const NfWorld *world,
    uint32_t seed);
void nf_spatial_tick(
    NfSpatialSystem *spatial,
    const struct NfAiSystem *ai,
    const NfWorld *world,
    const NfSemanticBus *semantics);
void nf_spatial_on_respawn(
    NfSpatialSystem *spatial,
    NfEntityId actor_id,
    const NfWorld *world);
const NfSpatialAgentState *nf_spatial_agent_state_const(
    const NfSpatialSystem *spatial,
    NfEntityId actor_id);
float nf_spatial_field_preference(const NfSpatialFields *fields);
const char *nf_spatial_task_name(NfSpatialTask task);
const char *nf_spatial_interrupt_name(NfSpatialInterrupt interrupt);

#endif
