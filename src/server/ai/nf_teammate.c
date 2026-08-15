#include "nf_teammate.h"

#include "nf_contamination.h"
#include "nf_energy.h"
#include "nf_movement.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define NF_TEAMMATE_DECISION_INTERVAL 12u
#define NF_TEAMMATE_DIRECT_RANGE 30.0f
#define NF_TEAMMATE_ENCOUNTER_RANGE 4.5f
#define NF_TEAMMATE_WAYPOINT_RADIUS 1.55f
#define NF_TEAMMATE_HOTSPOT_NAV_MAX 0.28f
#define NF_TEAMMATE_REPORT_DELAY (NF_TICK_RATE / 5u)
#define NF_TEAMMATE_REPORT_LIFETIME (NF_TICK_RATE * 3u)

static float clamp01(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

static float distance_xz(NfVec3 a, NfVec3 b) {
    const float dx = a.x-b.x;
    const float dz = a.z-b.z;
    return sqrtf(dx*dx+dz*dz);
}

static float path_distance(NfVec3 start, const NfVec3 points[], size_t count) {
    float total = 0.0f;
    NfVec3 cursor = start;
    for (size_t i = 0u; i < count; ++i) {
        total += distance_xz(cursor,points[i]);
        cursor = points[i];
    }
    return total;
}

static bool ray_aabb(
    NfVec3 origin, NfVec3 direction, NfVec3 minimum, NfVec3 maximum,
    float *distance) {
    float tmin = 0.0f;
    float tmax = FLT_MAX;
    const float ov[3] = {origin.x,origin.y,origin.z};
    const float dv[3] = {direction.x,direction.y,direction.z};
    const float mn[3] = {minimum.x,minimum.y,minimum.z};
    const float mx[3] = {maximum.x,maximum.y,maximum.z};
    for (int i = 0; i < 3; ++i) {
        if (fabsf(dv[i]) < 1e-6f) {
            if (ov[i] < mn[i] || ov[i] > mx[i]) return false;
            continue;
        }
        float t1 = (mn[i]-ov[i])/dv[i];
        float t2 = (mx[i]-ov[i])/dv[i];
        if (t1 > t2) { const float temporary=t1; t1=t2; t2=temporary; }
        if (t1 > tmin) tmin=t1;
        if (t2 < tmax) tmax=t2;
        if (tmin > tmax) return false;
    }
    if (tmax < 0.0f) return false;
    if (distance != NULL) *distance = tmin >= 0.0f ? tmin : tmax;
    return true;
}

static bool line_blocked(const NfWorld *world, NfVec3 from, NfVec3 to) {
    const NfVec3 delta = {to.x-from.x,to.y-from.y,to.z-from.z};
    const float length = sqrtf(delta.x*delta.x+delta.y*delta.y+delta.z*delta.z);
    if (length < 0.001f) return false;
    const NfVec3 direction = {delta.x/length,delta.y/length,delta.z/length};
    for (size_t i = 0u; i < world->collider_count; ++i) {
        const NfCollider *collider = &world->colliders[i];
        if (collider->kind == NF_COLLIDER_LADDER) continue;
        float hit = 0.0f;
        if (ray_aabb(from,direction,collider->min,collider->max,&hit) &&
            hit > 0.05f && hit < length-0.20f) return true;
    }
    return false;
}

void nf_teammate_profile_from_hotspots(
    NfTeammateProfile *profile,
    const NfTeammateBiographyHotspot hotspots[NF_TEAMMATE_BIOGRAPHY_HOTSPOTS]) {
    if (profile == NULL || hotspots == NULL) return;
    *profile = (NfTeammateProfile){
        .competition_avoidance = 0.50f,
        .reciprocity = 0.50f,
        .independence = 0.50f,
        .evidence_discipline = 0.58f,
        .risk_sensitivity = 0.52f
    };
    for (size_t i = 0u; i < NF_TEAMMATE_BIOGRAPHY_HOTSPOTS; ++i) {
        const float c = clamp01(hotspots[i].consequence);
        switch (hotspots[i].kind) {
            case NF_TEAMMATE_BIO_FORMATIVE_COOPERATION:
                profile->reciprocity += 0.28f*c;
                profile->competition_avoidance += 0.18f*c;
                break;
            case NF_TEAMMATE_BIO_MENTORSHIP_INSTITUTION:
                profile->evidence_discipline += 0.30f*c;
                profile->risk_sensitivity += 0.10f*c;
                break;
            case NF_TEAMMATE_BIO_CREW_ENTRY:
                profile->reciprocity += 0.10f*c;
                profile->independence += 0.22f*c;
                profile->competition_avoidance += 0.10f*c;
                break;
            case NF_TEAMMATE_BIO_COMPETITIVE_DISRUPTION:
                profile->independence += 0.18f*c;
                profile->competition_avoidance -= 0.22f*c;
                break;
            default: break;
        }
    }
    profile->competition_avoidance=clamp01(profile->competition_avoidance);
    profile->reciprocity=clamp01(profile->reciprocity);
    profile->independence=clamp01(profile->independence);
    profile->evidence_discipline=clamp01(profile->evidence_discipline);
    profile->risk_sensitivity=clamp01(profile->risk_sensitivity);
}

void nf_teammate_init(
    NfTeammateSystem *teammate, NfWorld *world, bool enabled, uint32_t seed) {
    if (teammate == NULL || world == NULL) return;
    memset(teammate,0,sizeof(*teammate));
    nf_report_bus_init(&teammate->reports);
    teammate->enabled = enabled;
    teammate->spawn = (NfVec3){6.0f,0.05f,-16.0f};
    teammate->commitment = NF_TEAMMATE_COMMIT_ROUTE_A;
    teammate->biography[0]=(NfTeammateBiographyHotspot){NF_TEAMMATE_BIO_FORMATIVE_COOPERATION,0.90f};
    teammate->biography[1]=(NfTeammateBiographyHotspot){NF_TEAMMATE_BIO_MENTORSHIP_INSTITUTION,0.82f};
    teammate->biography[2]=(NfTeammateBiographyHotspot){NF_TEAMMATE_BIO_CREW_ENTRY,0.88f};
    nf_teammate_profile_from_hotspots(&teammate->profile,teammate->biography);
    nf_region_graph_init_spatial_lab(&teammate->graph);
    (void)seed;
    if (!enabled) return;
    teammate->actor_id = nf_world_spawn_actor(world,NF_FACTION_TEAMMATE,teammate->spawn);
}

bool nf_teammate_publish_route_report(
    NfTeammateSystem *teammate, const NfWorld *world,
    const NfRouteSystem *route, NfEntityId reporter) {
    if (teammate == NULL || world == NULL || route == NULL || !route->configured ||
        !route->open || reporter == 0u) return false;
    const NfActor *actor = nf_world_find_actor_const(world,reporter);
    if (actor == NULL || (actor->faction != NF_FACTION_PLAYER &&
        actor->faction != NF_FACTION_TEAMMATE)) return false;

    NfReport report = {
        .subject_key = route->gate_key,
        .kind = NF_REPORT_ROUTE_STATE,
        .scope = NF_REPORT_SCOPE_CREW,
        .origin = route->changed_by != 0u ? route->changed_by : reporter,
        .reporter = reporter,
        .coarse_position = route->gate_center,
        .value = 1.0f,
        .confidence = 0.82f,
        .origin_tick = route->changed_tick,
        .issued_tick = world->tick,
        .deliver_tick = world->tick + NF_TEAMMATE_REPORT_DELAY,
        .expiry_tick = world->tick + NF_TEAMMATE_REPORT_LIFETIME
    };
    uint32_t id = 0u;
    if (!nf_report_publish(&teammate->reports,report,&id)) return false;
    teammate->last_route_report_id = id;
    return true;
}

static NfTeammateEvidenceSource acquire_evidence(
    NfTeammateSystem *teammate, const NfWorld *world,
    const NfRouteSystem *route) {
    if (teammate == NULL || world == NULL || route == NULL || !route->open) {
        return NF_TEAMMATE_EVIDENCE_NONE;
    }
    const NfActor *self = nf_world_find_actor_const(world,teammate->actor_id);
    if (self == NULL) return NF_TEAMMATE_EVIDENCE_NONE;
    const NfVec3 eye = {
        self->transform.position.x,
        self->transform.position.y+self->movement.eye_height,
        self->transform.position.z
    };
    const float gate_distance = nf_route_distance_to_gate(route,self->transform.position);
    if (gate_distance <= NF_TEAMMATE_DIRECT_RANGE &&
        !line_blocked(world,eye,route->gate_center)) {
        teammate->last_report_confidence = 0.0f;
        return NF_TEAMMATE_EVIDENCE_DIRECT;
    }

    NfReport report = {0};
    if (nf_report_best(&teammate->reports,self->id,self->faction,world->tick,
            NF_REPORT_ROUTE_STATE,route->gate_key,&report) &&
        report.value > 0.5f && report.origin_tick >= route->changed_tick) {
        teammate->last_route_report_id = report.id;
        teammate->last_report_confidence = nf_report_weight(&report,world->tick);
        return NF_TEAMMATE_EVIDENCE_CREW_REPORT;
    }

    if (gate_distance <= NF_TEAMMATE_ENCOUNTER_RANGE) {
        teammate->last_report_confidence = 0.0f;
        return NF_TEAMMATE_EVIDENCE_ENCOUNTER;
    }
    teammate->last_report_confidence = 0.0f;
    return NF_TEAMMATE_EVIDENCE_NONE;
}

static float evidence_confidence(
    const NfTeammateSystem *teammate, NfTeammateEvidenceSource source) {
    if (source == NF_TEAMMATE_EVIDENCE_DIRECT) return 0.98f;
    if (source == NF_TEAMMATE_EVIDENCE_ENCOUNTER) return 0.94f;
    if (source == NF_TEAMMATE_EVIDENCE_CREW_REPORT) {
        return clamp01(teammate->last_report_confidence *
            (0.85f + 0.15f*teammate->profile.evidence_discipline));
    }
    return 0.0f;
}

static void update_memory_age(NfTeammateSystem *teammate, uint64_t now_tick) {
    NfTeammateRouteMemory *memory = &teammate->memory;
    if (!memory->known_open || memory->observed_tick == 0u || now_tick < memory->observed_tick) return;
    const uint64_t age = now_tick-memory->observed_tick;
    if (age <= NF_TEAMMATE_RECENT_MEMORY_TICKS) {
        memory->freshness = clamp01(1.0f-(float)age/(float)NF_TEAMMATE_RECENT_MEMORY_TICKS);
        if (memory->freshness < 0.35f) memory->freshness = 0.35f;
        return;
    }
    if (memory->consequence >= 0.65f || memory->repetitions >= 2u) {
        memory->consolidated = true;
        memory->freshness = 0.62f;
        if (memory->confidence < 0.58f) memory->confidence = 0.58f;
    } else {
        memory->freshness = clamp01(memory->freshness-0.04f);
        memory->confidence = clamp01(memory->confidence-0.035f);
        if (memory->confidence < 0.12f) memory->known_open = false;
    }
}

static bool observe_route(
    NfTeammateSystem *teammate, const NfWorld *world,
    NfTeammateEvidenceSource source) {
    if (source == NF_TEAMMATE_EVIDENCE_NONE) return false;
    NfTeammateRouteMemory *memory = &teammate->memory;
    if (!memory->known_open) memory->repetitions = 1u;
    else if (source != memory->source) ++memory->repetitions;
    memory->known_open = true;
    memory->source = source;
    memory->confidence = evidence_confidence(teammate,source);
    memory->consequence = 0.86f;
    memory->freshness = 1.0f;
    memory->observed_tick = world->tick;
    return true;
}

static float route_energy(
    const NfTeammateSystem *teammate, const NfWorld *world,
    const NfRouteSystem *route, bool route_b) {
    const NfActor *self = nf_world_find_actor_const(world,teammate->actor_id);
    if (self == NULL) return FLT_MAX;
    const NfVec3 points_a[3] = {route->route_a_entry,route->route_a_reconnect,route->goal};
    const NfVec3 points_b[3] = {route->route_b_entry,route->route_b_reconnect,route->goal};
    const float distance = path_distance(
        self->transform.position,route_b?points_b:points_a,3u);
    const float mobility = nf_contamination_move_scale(self);
    const uint8_t origin = nf_region_nearest(&teammate->graph,self->transform.position);
    const uint8_t destination = nf_region_nearest(&teammate->graph,route->goal);
    double topographic = 0.0;
    if (origin != NF_REGION_INVALID && destination != NF_REGION_INVALID) {
        topographic = nf_energy_topographic_cost(
            &world->energy,&teammate->graph,origin,destination);
        if (!isfinite(topographic) || topographic < 0.0) topographic = 0.0;
    }
    return distance/(mobility > 0.20f ? mobility : 0.20f) + (float)topographic*0.08f;
}

static bool projected_occupancy_conflict(
    const NfTeammateSystem *teammate, const NfWorld *world, NfVec3 goal) {
    const NfActor *self = nf_world_find_actor_const(world,teammate->actor_id);
    if (self == NULL) return false;
    for (size_t i = 0u; i < NF_MAX_ENTITIES; ++i) {
        const NfActor *other = &world->actors[i];
        if (!other->active || other->id == self->id || !other->combat.alive) continue;
        if (other->faction != NF_FACTION_PLAYER && other->faction != NF_FACTION_TEAMMATE) continue;
        if (distance_xz(other->transform.position,self->transform.position) < 1.05f) return true;
        NfVec3 projected = other->transform.position;
        projected.x += other->transform.velocity.x*0.45f;
        projected.z += other->transform.velocity.z*0.45f;
        if (distance_xz(projected,goal) < 1.20f &&
            distance_xz(self->transform.position,goal) < 4.5f) return true;
    }
    return false;
}

static NfVec3 route_waypoint(
    NfTeammateSystem *teammate, const NfRouteSystem *route,
    const NfActor *body) {
    const NfVec3 points_a[3] = {route->route_a_entry,route->route_a_reconnect,route->goal};
    const NfVec3 points_b[3] = {route->route_b_entry,route->route_b_reconnect,route->goal};
    const NfVec3 *points = teammate->commitment == NF_TEAMMATE_COMMIT_ROUTE_B
        ? points_b : points_a;
    while (teammate->waypoint_index < 2u &&
        distance_xz(body->transform.position,points[teammate->waypoint_index]) <= NF_TEAMMATE_WAYPOINT_RADIUS) {
        ++teammate->waypoint_index;
    }
    return points[teammate->waypoint_index > 2u ? 2u : teammate->waypoint_index];
}

static void evaluate_routes(
    NfTeammateSystem *teammate, const NfWorld *world,
    const NfRouteSystem *route) {
    const NfActor *body = nf_world_find_actor_const(world,teammate->actor_id);
    if (body == NULL) return;
    const float a_energy = route_energy(teammate,world,route,false);
    const float b_energy = route_energy(teammate,world,route,true);
    const float a_base = clamp01(1.0f-a_energy/92.0f);
    const float b_base = clamp01(1.0f-b_energy/92.0f);
    const float hotspot = teammate->memory.known_open
        ? clamp01(teammate->memory.confidence*teammate->memory.consequence*
            (0.55f+0.45f*teammate->memory.freshness))
        : 0.0f;
    const float bounded_nav = hotspot*NF_TEAMMATE_HOTSPOT_NAV_MAX;
    const float a_utility = clamp01(
        a_base + teammate->profile.risk_sensitivity*0.10f +
        teammate->profile.reciprocity*0.03f);
    float b_utility = -FLT_MAX;
    if (teammate->memory.known_open && route->open) {
        b_utility = clamp01(
            b_base + bounded_nav + teammate->profile.independence*0.06f);
    }

    teammate->trace.belief_open = teammate->memory.known_open;
    teammate->trace.route_a_energy = a_energy;
    teammate->trace.route_b_energy = b_energy;
    teammate->trace.route_a_utility = a_utility;
    teammate->trace.route_b_utility = b_utility;
    teammate->trace.hotspot_gradient = hotspot;

    NfTeammateCommitment next = teammate->commitment;
    if (teammate->commitment == NF_TEAMMATE_COMMIT_ROUTE_B && !route->open) {
        next = NF_TEAMMATE_COMMIT_ROUTE_A;
    } else if (b_utility > a_utility+0.04f) {
        next = NF_TEAMMATE_COMMIT_ROUTE_B;
    } else if (teammate->commitment != NF_TEAMMATE_COMMIT_ROUTE_B) {
        next = NF_TEAMMATE_COMMIT_ROUTE_A;
    }

    if (next != teammate->commitment) {
        teammate->commitment = next;
        teammate->waypoint_index = 0u;
        if (next == NF_TEAMMATE_COMMIT_ROUTE_B &&
            body->transform.position.z > route->route_b_entry.z+2.0f) {
            teammate->waypoint_index = 1u;
        }
        printf("[teammate-causal] EVIDENCE=%s BELIEF=%s GRADIENT=%.2f HOTSPOT=ROUTE_RECONNECT UTILITY[A=%.2f B=%.2f] COMMITMENT=%s ACTION=REROUTE\n",
            nf_teammate_evidence_name(teammate->memory.source),
            teammate->memory.known_open?"OPEN":"UNCHANGED",hotspot,
            a_utility,b_utility,nf_teammate_commitment_name(next));
    }
    teammate->trace.commitment = teammate->commitment;
}

size_t nf_teammate_tick(
    NfTeammateSystem *teammate, NfWorld *world, const NfRouteSystem *route,
    NfControlFrame *out, size_t cap) {
    if (teammate == NULL || world == NULL || route == NULL || out == NULL ||
        cap == 0u || !teammate->enabled || teammate->actor_id == 0u) return 0u;
    NfActor *body = nf_world_find_actor(world,teammate->actor_id);
    if (body == NULL) return 0u;

    update_memory_age(teammate,world->tick);
    const NfTeammateEvidenceSource evidence = acquire_evidence(teammate,world,route);
    teammate->trace.evidence = evidence;
    if (route->open && !teammate->memory.known_open && evidence == NF_TEAMMATE_EVIDENCE_NONE &&
        !teammate->negative_control_logged && world->tick >= route->changed_tick+20u) {
        printf("[teammate-causal] EVIDENCE=NONE BELIEF=UNCHANGED GRADIENT=0.00 HOTSPOT=NONE UTILITY=UNCHANGED COMMITMENT=%s ACTION=UNCHANGED\n",
            nf_teammate_commitment_name(teammate->commitment));
        teammate->negative_control_logged = true;
    }
    if (evidence != NF_TEAMMATE_EVIDENCE_NONE && !teammate->memory.known_open) {
        observe_route(teammate,world,evidence);
        teammate->next_decision_tick = world->tick;
        if (!teammate->evidence_logged) {
            printf("[teammate-evidence] source=%s confidence=%.2f tick=%llu route=OPEN report=%u\n",
                nf_teammate_evidence_name(evidence),teammate->memory.confidence,
                (unsigned long long)world->tick,teammate->last_route_report_id);
            teammate->evidence_logged = true;
        }
    }

    if (world->tick >= teammate->next_decision_tick) {
        evaluate_routes(teammate,world,route);
        teammate->next_decision_tick = world->tick+NF_TEAMMATE_DECISION_INTERVAL;
    }

    NfControlFrame frame = {
        .actor = teammate->actor_id,
        .source = NF_CONTROL_AI,
        .perceived_tick = world->tick,
        .sequence = ++teammate->control_sequence
    };
    if (!body->combat.alive) { out[0]=frame; return 1u; }

    const NfVec3 goal = route_waypoint(teammate,route,body);
    teammate->trace.yielded_for_occupancy = false;
    if (world->tick < teammate->yield_until_tick) {
        teammate->trace.yielded_for_occupancy = true;
    } else if (projected_occupancy_conflict(teammate,world,goal) &&
               teammate->profile.competition_avoidance >= 0.55f) {
        teammate->yield_until_tick = world->tick+NF_TICK_RATE/3u;
        teammate->trace.yielded_for_occupancy = true;
    }

    if (!teammate->trace.yielded_for_occupancy) {
        nf_spatial_apply_local_goal_control(
            world,teammate->actor_id,goal,NF_SPATIAL_TASK_ROAM,&frame);
        teammate->claim_until_tick = world->tick+NF_TICK_RATE/2u;
    }
    out[0]=frame;
    return 1u;
}

bool nf_teammate_spawn_for(
    const NfTeammateSystem *teammate, NfEntityId actor_id, NfVec3 *out) {
    if (teammate == NULL || out == NULL || !teammate->enabled ||
        teammate->actor_id != actor_id) return false;
    *out = teammate->spawn;
    return true;
}

void nf_teammate_on_respawn(
    NfTeammateSystem *teammate, NfEntityId actor_id, const NfWorld *world) {
    if (teammate == NULL || world == NULL || teammate->actor_id != actor_id) return;
    teammate->commitment = teammate->memory.known_open
        ? NF_TEAMMATE_COMMIT_ROUTE_B : NF_TEAMMATE_COMMIT_ROUTE_A;
    teammate->waypoint_index = 0u;
    teammate->yield_until_tick = 0u;
    teammate->claim_until_tick = 0u;
    teammate->next_decision_tick = world->tick;
}

void nf_teammate_force_memory_age(
    NfTeammateSystem *teammate, uint64_t now_tick) {
    if (teammate != NULL) update_memory_age(teammate,now_tick);
}

float nf_teammate_route_utility(
    const NfTeammateSystem *teammate, bool route_b) {
    if (teammate == NULL) return -FLT_MAX;
    return route_b ? teammate->trace.route_b_utility : teammate->trace.route_a_utility;
}

const char *nf_teammate_evidence_name(NfTeammateEvidenceSource source) {
    switch (source) {
        case NF_TEAMMATE_EVIDENCE_DIRECT: return "DIRECT";
        case NF_TEAMMATE_EVIDENCE_CREW_REPORT: return "CREW_REPORT";
        case NF_TEAMMATE_EVIDENCE_ENCOUNTER: return "ENCOUNTER";
        case NF_TEAMMATE_EVIDENCE_NONE:
        default: return "NONE";
    }
}

const char *nf_teammate_commitment_name(NfTeammateCommitment commitment) {
    switch (commitment) {
        case NF_TEAMMATE_COMMIT_ROUTE_A: return "ROUTE_A";
        case NF_TEAMMATE_COMMIT_ROUTE_B: return "ROUTE_B";
        case NF_TEAMMATE_COMMIT_YIELD: return "YIELD";
        case NF_TEAMMATE_COMMIT_HOLD: return "HOLD";
        default: return "UNKNOWN";
    }
}
