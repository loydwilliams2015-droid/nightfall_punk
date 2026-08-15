#include "nf_tactical.h"

#include <float.h>
#include <math.h>

static float clamp01(float value) {
    if (value < 0.0f) return 0.0f;
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
        if (t1 > t2) {
            const float temporary = t1;
            t1 = t2;
            t2 = temporary;
        }
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
            hit > 0.05f && hit < length - 0.10f) {
            return true;
        }
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
        travel_fit*0.28f +
        option_value*0.10f);
}
