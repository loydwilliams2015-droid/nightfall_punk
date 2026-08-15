#ifndef NF_TACTICAL_H
#define NF_TACTICAL_H

#include "nf_world.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_TACTICAL_CANDIDATE_CAP 12u

typedef enum NfTacticalAction {
    NF_TACTICAL_NONE = 0,
    NF_TACTICAL_ATTACK,
    NF_TACTICAL_REPOSITION,
    NF_TACTICAL_GUARD,
    NF_TACTICAL_WITHDRAW,
    NF_TACTICAL_INVESTIGATE,
    NF_TACTICAL_ASSIST,
    NF_TACTICAL_RESOURCE,
    NF_TACTICAL_RESTORE
} NfTacticalAction;

enum {
    NF_TACTICAL_REASON_VIABILITY = 1u << 0,
    NF_TACTICAL_REASON_RELATION = 1u << 1,
    NF_TACTICAL_REASON_CREW = 1u << 2,
    NF_TACTICAL_REASON_LOCALITY = 1u << 3,
    NF_TACTICAL_REASON_OPTIONS = 1u << 4,
    NF_TACTICAL_REASON_INFORMATION = 1u << 5,
    NF_TACTICAL_REASON_RESOURCE = 1u << 6
};

typedef struct NfTacticalSnapshot {
    float actor_viability;
    float relation_continuity;
    float crew_continuity;
    float locality_value;
    float future_options;
    float information_quality;
    float uncertainty;
    float objective_urgency;
    float ammo_fraction;
    float metabolic_margin;
} NfTacticalSnapshot;

typedef struct NfTacticalCandidate {
    bool feasible;
    NfTacticalAction action;
    uint32_t subject_key;
    NfVec3 position;
    float actor_delta;
    float relation_delta;
    float crew_delta;
    float locality_delta;
    float options_delta;
    float immediate_gain;
    float action_cost;
    float uncertainty_cost;
    float opportunity_cost;
    float role_bias;
    float score;
    uint32_t reason_bits;
} NfTacticalCandidate;

float nf_tactical_exposure(const NfWorld *world, NfVec3 threat, NfVec3 candidate);
float nf_tactical_cover_utility(float exposure, float travel_m, float option_distance_m);
float nf_tactical_score_candidate(
    const NfTacticalSnapshot *snapshot, NfTacticalCandidate *candidate);
size_t nf_tactical_rank(
    const NfTacticalSnapshot *snapshot, NfTacticalCandidate *candidates,
    size_t count);
const char *nf_tactical_action_name(NfTacticalAction action);

#endif
