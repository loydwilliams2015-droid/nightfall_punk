#ifndef NF_ENCOUNTER_H
#define NF_ENCOUNTER_H

#include "nf_ai.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_ENCOUNTER_MAX_PRESSURE_SLOTS 2u

typedef struct NfEncounterAgentState {
    NfEntityId actor_id;
    NfEntityId last_target;
    float tracked_yaw;
    float tracked_pitch;
    float aim_settle;
    float suppression;
    float last_health;
    float last_aim_error;
    bool pressure_authorized;
} NfEncounterAgentState;

typedef struct NfEncounterState {
    NfEncounterAgentState agents[NF_AI_MAX_AGENTS];
    size_t count;
    size_t pressure_slots;
    uint64_t next_role_tick;
    uint32_t seed;
} NfEncounterState;

void nf_encounter_init(
    NfEncounterState *encounter,
    const NfAiSystem *ai,
    const NfWorld *world,
    size_t pressure_slots,
    uint32_t seed);

void nf_encounter_filter_controls(
    NfEncounterState *encounter,
    NfAiSystem *ai,
    NfWorld *world,
    NfControlFrame *controls,
    size_t control_count);

size_t nf_encounter_pressure_count(const NfEncounterState *encounter);
const NfEncounterAgentState *nf_encounter_agent_state_const(
    const NfEncounterState *encounter,
    NfEntityId actor_id);

#endif
