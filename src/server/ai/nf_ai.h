#ifndef NF_AI_H
#define NF_AI_H

#include "nf_agent.h"
#include "nf_belief.h"
#include "nf_claim.h"
#include "nf_combat.h"
#include "nf_relations.h"
#include "nf_report.h"
#include "nf_semantics.h"
#include "nf_tactical.h"
#include "nf_world.h"

#include <stddef.h>
#include <stdint.h>

#define NF_AI_MAX_AGENTS 4u
#define NF_AI_MAX_AFFORDANCES 96u

typedef struct NfAiKnowledge {
    NfEntityId target;
    bool visible_now;
    NfVec3 last_seen_position;
    uint64_t last_seen_tick;
    NfVec3 last_heard_position;
    uint64_t last_heard_tick;
    float confidence;
    float precision_m;
    NfInformationChannel channel;
    uint32_t hypothesis_id;
} NfAiKnowledge;

typedef struct NfAiAffordance { uint32_t id; NfVec3 position; } NfAiAffordance;

typedef struct NfAiAgent {
    NfEntityId actor_id;
    NfAgentMode mode;
    NfSquadRole role;
    NfAiKnowledge knowledge;
    NfBeliefStore beliefs;
    NfTacticalSnapshot tactical_snapshot;
    NfTacticalCandidate tactical_candidates[NF_TACTICAL_CANDIDATE_CAP];
    size_t tactical_candidate_count;
    NfTacticalAction selected_action;
    float yaw;
    float pitch;
    float current_score;
    float mode_scores[NF_AGENT_MODE_COUNT];
    uint64_t next_perception_tick;
    uint64_t next_decision_tick;
    uint64_t reaction_ready_tick;
    uint64_t last_report_tick;
    uint32_t last_report_subject;
    uint32_t control_sequence;
    int selected_affordance;
    int strafe_sign;
    NfVec3 spawn;
    NfVec3 last_sample_position;
    uint32_t stuck_ticks;
    bool movement_requested;
} NfAiAgent;

typedef struct NfAiSystem {
    NfAiAgent agents[NF_AI_MAX_AGENTS];
    size_t count;
    NfAiAffordance affordances[NF_AI_MAX_AFFORDANCES];
    size_t affordance_count;
    NfReportBus reports;
    NfClaimTable *claims;
    NfRelationship rival_relationship;
    uint32_t seed;
} NfAiSystem;

void nf_ai_init(NfAiSystem *ai, NfWorld *world, size_t count, uint32_t seed);
void nf_ai_set_rival_relationship(NfAiSystem *ai, NfRelationship relationship);
void nf_ai_bind_claims(NfAiSystem *ai, NfClaimTable *claims);
size_t nf_ai_tick(
    NfAiSystem *ai, NfWorld *world, const NfSemanticBus *semantics,
    NfControlFrame *out, size_t cap);
const NfAiAgent *nf_ai_find_agent_const(const NfAiSystem *ai, NfEntityId actor_id);
const NfReportBus *nf_ai_report_bus_const(const NfAiSystem *ai);
bool nf_ai_spawn_for(const NfAiSystem *ai, NfEntityId actor_id, NfVec3 *out);
void nf_ai_on_respawn(NfAiSystem *ai, NfEntityId actor_id);

#endif
