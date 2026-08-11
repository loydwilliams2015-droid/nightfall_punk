#ifndef NF_LIFEWORLD_H
#define NF_LIFEWORLD_H

#include "nf_energy.h"
#include "nf_region.h"
#include "nf_world.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_LIFEWORLD_MAX_HOTSPOTS 3u
#define NF_LIFEWORLD_MAX_ATTENTION 4u
#define NF_LIFEWORLD_PRIMARY_SCHEMA 1u

/* The first embodied proof window is approximately 40m x 20m = 800m^2.
   It is a graybox observation window, not the ontological definition of a
   landbase: interdependent flows and recurring use define membership. */
#define NF_LIFEWORLD_DEMO_CENTER_X (-46.0f)
#define NF_LIFEWORLD_DEMO_CENTER_Z (-10.0f)
#define NF_LIFEWORLD_DEMO_HALF_X 20.0f
#define NF_LIFEWORLD_DEMO_HALF_Z 10.0f

typedef enum NfLifeActivity {
    NF_LIFE_TRAVEL = 0,
    NF_LIFE_WORK,
    NF_LIFE_MAINTAIN,
    NF_LIFE_OCCUPY
} NfLifeActivity;

typedef enum NfProcessType {
    NF_PROCESS_GENERATOR = 1,
    NF_PROCESS_PUMP = 2
} NfProcessType;

typedef enum NfEpistemicHotspotType {
    NF_HOTSPOT_NONE = 0,
    NF_HOTSPOT_CONTESTED_OCCUPATION,
    NF_HOTSPOT_ACCESS_CHANGE,
    NF_HOTSPOT_RECOVERY_EDGE
} NfEpistemicHotspotType;

typedef struct NfLandbaseMemory {
    uint16_t infestation_q;
    uint16_t human_use_q;
    uint16_t maintenance_q;
    uint16_t disturbance_q;
    uint64_t last_cattler_tick;
    uint64_t last_human_tick;
} NfLandbaseMemory;

typedef struct NfProcessObject {
    uint32_t id;
    NfProcessType type;
    NfVec3 position;
    uint16_t condition_q;
    uint16_t throughput_q;
    uint16_t access_q;
    uint16_t flags;
} NfProcessObject;

typedef struct NfEpistemicHotspot {
    NfEpistemicHotspotType type;
    NfVec3 position;
    uint8_t region;
    uint16_t uncertainty_q;
    uint16_t consequence_q;
    uint16_t evidence_change_q;
    uint64_t changed_tick;
} NfEpistemicHotspot;

typedef struct NfAttentionChoice {
    NfEntityId actor_id;
    NfEpistemicHotspotType hotspot;
    uint8_t region;
    NfLifeActivity activity;
    bool exploration;
} NfAttentionChoice;

typedef struct NfLifeworldSurface {
    float darkness;
    float traffic;
    float infrastructure_activity;
    float disturbance;
    float recovery;
} NfLifeworldSurface;

typedef struct NfLifeworldSystem {
    NfVec3 center;
    float half_x;
    float half_z;
    uint8_t region;
    NfLandbaseMemory memory;
    NfProcessObject generator;
    NfProcessObject pump;
    NfEpistemicHotspot hotspots[NF_LIFEWORLD_MAX_HOTSPOTS];
    size_t hotspot_count;
    NfAttentionChoice attention[NF_LIFEWORLD_MAX_ATTENTION];
    size_t attention_count;
    uint16_t previous_pump_throughput_q;
    uint8_t cattlers_present;
    uint8_t humans_present;
    uint64_t next_update_tick;
    uint32_t seed;
} NfLifeworldSystem;

void nf_lifeworld_init(
    NfLifeworldSystem *system,
    const NfRegionGraph *graph,
    uint32_t seed);
void nf_lifeworld_tick(
    NfLifeworldSystem *system,
    const NfWorld *world,
    const NfRegionGraph *graph);

bool nf_lifeworld_contains(
    const NfLifeworldSystem *system,
    NfVec3 position);
NfLifeworldSurface nf_lifeworld_predict_surface(
    const NfLifeworldSystem *system);

/* Actor-facing situated valuation: inputs must already be belief-derived.
   The function may price embodiment/access/social or habitat fit, but it never
   reads current energy stocks from NfEnergySystem. */
double nf_lifeworld_situated_value(
    const NfActor *actor,
    const NfEnergyOpportunity *belief_opportunity,
    const NfRegionGraph *graph,
    bool cattler_form);

/* Primary JSON is intentionally tiny, exact and quantized. It contains causes
   required for continuity, never disposable predicted presentation state. */
size_t nf_lifeworld_write_primary_json(
    const NfLifeworldSystem *system,
    uint32_t world_seed,
    uint64_t tick,
    uint32_t round,
    char *out,
    size_t cap);

const char *nf_life_activity_name(NfLifeActivity activity);
const char *nf_hotspot_name(NfEpistemicHotspotType hotspot);

#endif
