#ifndef NF_CATTLER_H
#define NF_CATTLER_H

#include "nf_agent.h"
#include "nf_region.h"
#include "nf_relations.h"
#include "nf_semantics.h"
#include "nf_world.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_CATTLER_MAX_AGENTS 5u
#define NF_CATTLER_DEFAULT_AGENTS 3u
#define NF_CATTLER_BLACKBOARD_REPORTS 8u

typedef enum NfCattlerMode {
    NF_CATTLER_LURK = 0,
    NF_CATTLER_STALK,
    NF_CATTLER_SNIPE,
    NF_CATTLER_SWOOP,
    NF_CATTLER_INFEST,
    NF_CATTLER_EXPAND,
    NF_CATTLER_RECOVER,
    NF_CATTLER_WITHDRAW,
    NF_CATTLER_AVOID,
    NF_CATTLER_MODE_COUNT
} NfCattlerMode;

typedef enum NfCattlerSocialMode {
    NF_CATTLER_PACK = 0,
    NF_CATTLER_LONER
} NfCattlerSocialMode;

typedef enum NfCattlerEventMode {
    NF_CATTLER_EVENT_NORMAL = 0,
    NF_CATTLER_EVENT_HUNT_SURGE
} NfCattlerEventMode;

typedef enum NfCattlerDebugProfile {
    NF_CATTLER_PROFILE_NORMAL = 0,
    NF_CATTLER_PROFILE_PACK,
    NF_CATTLER_PROFILE_LONER,
    NF_CATTLER_PROFILE_HORDE
} NfCattlerDebugProfile;

typedef enum NfCattlerEvidenceType {
    NF_CATTLER_EVIDENCE_NONE = 0,
    NF_CATTLER_EVIDENCE_VISUAL,
    NF_CATTLER_EVIDENCE_GUNFIRE,
    NF_CATTLER_EVIDENCE_DAMAGE,
    NF_CATTLER_EVIDENCE_PACK_REPORT
} NfCattlerEvidenceType;

typedef struct NfCattlerKnowledge {
    NfEntityId prey;
    NfVec3 last_position;
    uint8_t region;
    NfCattlerEvidenceType evidence;
    float confidence;
    uint64_t evidence_tick;
    bool visible_now;
} NfCattlerKnowledge;

typedef struct NfCattlerReport {
    bool active;
    NfEntityId reporter;
    NfEntityId prey;
    NfVec3 position_hint;
    uint8_t region;
    NfCattlerEvidenceType evidence;
    float confidence;
    uint64_t tick;
} NfCattlerReport;

typedef struct NfCattlerRegionState {
    float infestation;
    float resource_value;
    float prey_activity;
    float disturbance;
    float territorial_pressure;
    uint64_t last_occupied_tick;
    uint64_t last_prey_tick;
    uint32_t expansion_count;
} NfCattlerRegionState;

typedef struct NfCattlerScore {
    float infestation;
    float prey;
    float displacement;
    float expansion;
    float survival;
} NfCattlerScore;

typedef struct NfCattlerAgent {
    NfEntityId actor_id;
    NfCattlerMode mode;
    NfCattlerSocialMode social;
    NfRancherProfile profile;
    NfCattlerKnowledge knowledge;
    NfCattlerScore score;
    NfVec3 spawn;
    float yaw;
    float pitch;
    float locomotor_integrity;
    float commitment;
    uint8_t current_region;
    uint8_t target_region;
    uint8_t next_region;
    uint8_t home_region;
    uint8_t claimed_watch_region;
    uint64_t next_perception_tick;
    uint64_t next_decision_tick;
    uint64_t swoop_ready_tick;
    uint64_t recover_until_tick;
    uint64_t last_region_change_tick;
    uint32_t control_sequence;
} NfCattlerAgent;

typedef struct NfCattlerSystem {
    NfRegionGraph graph;
    NfCattlerAgent agents[NF_CATTLER_MAX_AGENTS];
    size_t count;
    NfCattlerReport reports[NF_CATTLER_BLACKBOARD_REPORTS];
    size_t next_report;
    NfCattlerRegionState regions[NF_REGION_MAX];
    NfCattlerScore ecology_score;
    NfCattlerEventMode event_mode;
    NfCattlerDebugProfile debug_profile;
    uint64_t event_until_tick;
    uint64_t event_cooldown_until_tick;
    uint64_t next_event_check_tick;
    uint32_t seed;
} NfCattlerSystem;

void nf_cattler_init(
    NfCattlerSystem *system, NfWorld *world, size_t count,
    NfCattlerDebugProfile profile, uint32_t seed);
size_t nf_cattler_tick(
    NfCattlerSystem *system, NfWorld *world, const NfSemanticBus *semantics,
    NfControlFrame *out, size_t cap);
void nf_cattler_on_combat_event(
    NfCattlerSystem *system, const NfWorld *world, const NfCombatEvent *event);
void nf_cattler_on_respawn(NfCattlerSystem *system, NfEntityId actor_id, const NfWorld *world);
bool nf_cattler_spawn_for(const NfCattlerSystem *system, NfEntityId actor_id, NfVec3 *out);
const NfCattlerAgent *nf_cattler_find_const(const NfCattlerSystem *system, NfEntityId actor_id);
const NfCattlerRegionState *nf_cattler_region_state_const(const NfCattlerSystem *system, uint8_t region);
float nf_cattler_habitat_score(
    const NfCattlerSystem *system, const NfCattlerAgent *agent,
    const NfWorld *world, uint8_t region);
const char *nf_cattler_mode_name(NfCattlerMode mode);
const char *nf_cattler_social_name(NfCattlerSocialMode mode);
const char *nf_cattler_event_name(NfCattlerEventMode mode);

#endif
