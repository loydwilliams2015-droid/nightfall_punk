#ifndef NF_TEAMMATE_H
#define NF_TEAMMATE_H

#include "nf_agent.h"
#include "nf_region.h"
#include "nf_report.h"
#include "nf_route.h"
#include "nf_spatial.h"
#include "nf_world.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_TEAMMATE_BIOGRAPHY_HOTSPOTS 3u
#define NF_TEAMMATE_RECENT_MEMORY_TICKS (NF_TICK_RATE * 60u * 15u)

typedef enum NfTeammateEvidenceSource {
    NF_TEAMMATE_EVIDENCE_NONE = 0,
    NF_TEAMMATE_EVIDENCE_DIRECT,
    NF_TEAMMATE_EVIDENCE_CREW_REPORT,
    NF_TEAMMATE_EVIDENCE_ENCOUNTER
} NfTeammateEvidenceSource;

typedef enum NfTeammateBiographyKind {
    NF_TEAMMATE_BIO_FORMATIVE_COOPERATION = 0,
    NF_TEAMMATE_BIO_MENTORSHIP_INSTITUTION,
    NF_TEAMMATE_BIO_CREW_ENTRY,
    NF_TEAMMATE_BIO_COMPETITIVE_DISRUPTION
} NfTeammateBiographyKind;

typedef enum NfTeammateCommitment {
    NF_TEAMMATE_COMMIT_ROUTE_A = 0,
    NF_TEAMMATE_COMMIT_ROUTE_B,
    NF_TEAMMATE_COMMIT_YIELD,
    NF_TEAMMATE_COMMIT_HOLD
} NfTeammateCommitment;

typedef struct NfTeammateBiographyHotspot {
    NfTeammateBiographyKind kind;
    float consequence;
} NfTeammateBiographyHotspot;

typedef struct NfTeammateProfile {
    float competition_avoidance;
    float reciprocity;
    float independence;
    float evidence_discipline;
    float risk_sensitivity;
} NfTeammateProfile;

typedef struct NfTeammateRouteMemory {
    bool known_open;
    bool consolidated;
    NfTeammateEvidenceSource source;
    float confidence;
    float consequence;
    float freshness;
    uint32_t repetitions;
    uint64_t observed_tick;
} NfTeammateRouteMemory;

typedef struct NfTeammateDecisionTrace {
    NfTeammateEvidenceSource evidence;
    bool belief_open;
    float route_a_energy;
    float route_b_energy;
    float route_a_utility;
    float route_b_utility;
    float hotspot_gradient;
    NfTeammateCommitment commitment;
    bool yielded_for_occupancy;
} NfTeammateDecisionTrace;

typedef struct NfTeammateSystem {
    bool enabled;
    NfEntityId actor_id;
    NfVec3 spawn;
    NfRegionGraph graph;
    NfReportBus reports;
    uint32_t last_route_report_id;
    float last_report_confidence;
    NfTeammateBiographyHotspot biography[NF_TEAMMATE_BIOGRAPHY_HOTSPOTS];
    NfTeammateProfile profile;
    NfTeammateRouteMemory memory;
    NfTeammateDecisionTrace trace;
    NfTeammateCommitment commitment;
    uint8_t waypoint_index;
    uint32_t control_sequence;
    uint64_t next_decision_tick;
    uint64_t claim_until_tick;
    uint64_t yield_until_tick;
    bool negative_control_logged;
    bool evidence_logged;
} NfTeammateSystem;

void nf_teammate_profile_from_hotspots(
    NfTeammateProfile *profile,
    const NfTeammateBiographyHotspot hotspots[NF_TEAMMATE_BIOGRAPHY_HOTSPOTS]);
void nf_teammate_init(
    NfTeammateSystem *teammate, NfWorld *world, bool enabled, uint32_t seed);
bool nf_teammate_publish_route_report(
    NfTeammateSystem *teammate, const NfWorld *world,
    const NfRouteSystem *route, NfEntityId reporter);
size_t nf_teammate_tick(
    NfTeammateSystem *teammate, NfWorld *world, const NfRouteSystem *route,
    NfControlFrame *out, size_t cap);
bool nf_teammate_spawn_for(
    const NfTeammateSystem *teammate, NfEntityId actor_id, NfVec3 *out);
void nf_teammate_on_respawn(
    NfTeammateSystem *teammate, NfEntityId actor_id, const NfWorld *world);
void nf_teammate_force_memory_age(
    NfTeammateSystem *teammate, uint64_t now_tick);
float nf_teammate_route_utility(
    const NfTeammateSystem *teammate, bool route_b);
const char *nf_teammate_evidence_name(NfTeammateEvidenceSource source);
const char *nf_teammate_commitment_name(NfTeammateCommitment commitment);

#endif
