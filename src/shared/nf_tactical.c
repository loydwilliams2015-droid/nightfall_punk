#include "nf_tactical.h"

#include <float.h>
#include <math.h>

static float clamp01(float value) {
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}
static float clamp_signed(float value) {
    if (value < -1.0f) return -1.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static bool ray_aabb(
    NfVec3 origin, NfVec3 direction, NfVec3 minimum, NfVec3 maximum,
    float *distance) {
    float tmin = 0.0f;
    float tmax = FLT_MAX;
    const float ov[3] = {origin.x, origin.y, origin.z};
    const float dv[3] = {direction.x, direction.y, direction.z};
    const float lo[3] = {minimum.x, minimum.y, minimum.z};
    const float hi[3] = {maximum.x, maximum.y, maximum.z};
    for (int i = 0; i < 3; ++i) {
        if (fabsf(dv[i]) < 1e-6f) {
            if (ov[i] < lo[i] || ov[i] > hi[i]) return false;
            continue;
        }
        float t1 = (lo[i] - ov[i]) / dv[i];
        float t2 = (hi[i] - ov[i]) / dv[i];
        if (t1 > t2) { const float temporary = t1; t1 = t2; t2 = temporary; }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return false;
    }
    if (tmax < 0.0f) return false;
    if (distance != NULL) *distance = tmin >= 0.0f ? tmin : tmax;
    return true;
}

static bool blocked(const NfWorld *world, NfVec3 from, NfVec3 to) {
    const NfVec3 delta = {to.x - from.x, to.y - from.y, to.z - from.z};
    const float length = sqrtf(delta.x*delta.x + delta.y*delta.y + delta.z*delta.z);
    if (length < 0.001f) return false;
    const NfVec3 direction = {delta.x/length, delta.y/length, delta.z/length};
    for (size_t i = 0u; i < world->collider_count; ++i) {
        const NfCollider *collider = &world->colliders[i];
        if (collider->kind == NF_COLLIDER_LADDER) continue;
        float hit = 0.0f;
        if (ray_aabb(from, direction, collider->min, collider->max, &hit) &&
            hit > 0.05f && hit < length - 0.10f) return true;
    }
    return false;
}

float nf_tactical_exposure(
    const NfWorld *world, NfVec3 threat, NfVec3 candidate) {
    if (world == NULL) return 1.0f;
    const NfVec3 threat_eye = {threat.x, threat.y + 1.4f, threat.z};
    static const float heights[3] = {0.45f, 1.00f, 1.55f};
    unsigned exposed = 0u;
    for (size_t i = 0u; i < 3u; ++i) {
        const NfVec3 sample = {candidate.x, candidate.y + heights[i], candidate.z};
        if (!blocked(world, threat_eye, sample)) ++exposed;
    }
    return (float)exposed / 3.0f;
}

float nf_tactical_cover_utility(
    float exposure, float travel_m, float option_distance_m) {
    const float travel_fit = 1.0f - clamp01(travel_m / 18.0f);
    const float option_value = clamp01(option_distance_m / 20.0f);
    return clamp01(
        (1.0f - clamp01(exposure))*0.62f +
        travel_fit*0.28f + option_value*0.10f);
}

float nf_tactical_score_candidate(
    const NfTacticalSnapshot *snapshot, NfTacticalCandidate *candidate) {
    if (snapshot == NULL || candidate == NULL || !candidate->feasible) {
        if (candidate != NULL) candidate->score = -FLT_MAX;
        return -FLT_MAX;
    }
    const float viability = clamp01(snapshot->actor_viability);
    if (viability < 0.12f && candidate->actor_delta < -0.05f &&
        candidate->action != NF_TACTICAL_WITHDRAW &&
        candidate->action != NF_TACTICAL_ASSIST) {
        candidate->score = -FLT_MAX;
        candidate->reason_bits |= NF_TACTICAL_REASON_VIABILITY;
        return candidate->score;
    }
    const float uncertainty = clamp01(snapshot->uncertainty);
    const float urgency = clamp01(snapshot->objective_urgency);
    const float option_weight = 0.12f + uncertainty*0.18f + (1.0f-urgency)*0.10f;
    const float actor_weight = 0.28f + (1.0f-viability)*0.18f;
    const float crew_weight = 0.16f;
    const float relation_weight = 0.10f;
    const float locality_weight = 0.16f;
    candidate->actor_delta = clamp_signed(candidate->actor_delta);
    candidate->relation_delta = clamp_signed(candidate->relation_delta);
    candidate->crew_delta = clamp_signed(candidate->crew_delta);
    candidate->locality_delta = clamp_signed(candidate->locality_delta);
    candidate->options_delta = clamp_signed(candidate->options_delta);
    float score =
        candidate->actor_delta*actor_weight +
        candidate->relation_delta*relation_weight +
        candidate->crew_delta*crew_weight +
        candidate->locality_delta*locality_weight +
        candidate->options_delta*option_weight +
        clamp_signed(candidate->immediate_gain)*(0.18f + urgency*0.12f) +
        clamp_signed(candidate->role_bias)*0.08f -
        clamp01(candidate->action_cost)*0.24f -
        clamp01(candidate->uncertainty_cost)*(0.10f + uncertainty*0.20f) -
        clamp01(candidate->opportunity_cost)*0.18f;
    if (candidate->action == NF_TACTICAL_INVESTIGATE ||
        candidate->action == NF_TACTICAL_REPOSITION) {
        score += uncertainty * (1.0f-clamp01(snapshot->information_quality))*0.20f;
        candidate->reason_bits |= NF_TACTICAL_REASON_INFORMATION;
    }
    if (candidate->action == NF_TACTICAL_RESOURCE ||
        candidate->action == NF_TACTICAL_RESTORE) {
        score += (1.0f-clamp01(snapshot->metabolic_margin))*0.10f;
        candidate->reason_bits |= NF_TACTICAL_REASON_RESOURCE;
    }
    candidate->score = score;
    return score;
}

size_t nf_tactical_rank(
    const NfTacticalSnapshot *snapshot, NfTacticalCandidate *candidates,
    size_t count) {
    if (snapshot == NULL || candidates == NULL) return 0u;
    if (count > NF_TACTICAL_CANDIDATE_CAP) count = NF_TACTICAL_CANDIDATE_CAP;
    size_t feasible = 0u;
    for (size_t i = 0u; i < count; ++i) {
        (void)nf_tactical_score_candidate(snapshot, &candidates[i]);
        if (candidates[i].score > -FLT_MAX/2.0f) ++feasible;
    }
    for (size_t i = 1u; i < count; ++i) {
        NfTacticalCandidate key = candidates[i];
        size_t j = i;
        while (j > 0u && key.score > candidates[j-1u].score) {
            candidates[j] = candidates[j-1u];
            --j;
        }
        candidates[j] = key;
    }
    return feasible;
}

const char *nf_tactical_action_name(NfTacticalAction action) {
    switch (action) {
        case NF_TACTICAL_ATTACK: return "ATTACK";
        case NF_TACTICAL_REPOSITION: return "REPOSITION";
        case NF_TACTICAL_GUARD: return "GUARD";
        case NF_TACTICAL_WITHDRAW: return "WITHDRAW";
        case NF_TACTICAL_INVESTIGATE: return "INVESTIGATE";
        case NF_TACTICAL_ASSIST: return "ASSIST";
        case NF_TACTICAL_RESOURCE: return "RESOURCE";
        case NF_TACTICAL_RESTORE: return "RESTORE";
        default: return "NONE";
    }
}
