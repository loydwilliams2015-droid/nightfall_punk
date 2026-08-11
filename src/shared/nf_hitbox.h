#ifndef NF_HITBOX_H
#define NF_HITBOX_H

#include "nf_world.h"

#include <stdbool.h>

#define NF_CATTLER_BODY_HEIGHT 2.80f
#define NF_CATTLER_EYE_HEIGHT 2.35f
#define NF_CATTLER_BODY_WIDTH 0.84f
#define NF_CATTLER_WEAK_WIDTH 0.92f
#define NF_CATTLER_HEAD_CENTER_Y 2.56f
#define NF_CATTLER_HEAD_RADIUS 0.31f
#define NF_CATTLER_BODY_HALF_WIDTH 0.42f
#define NF_CATTLER_WEAK_HALF_WIDTH 0.46f
#define NF_CATTLER_FOOT_MIN_Y 0.06f
#define NF_CATTLER_FOOT_MAX_Y 0.34f
#define NF_CATTLER_KNEE_MIN_Y 0.34f
#define NF_CATTLER_KNEE_MAX_Y 0.90f
#define NF_CATTLER_BODY_MIN_Y 0.90f
#define NF_CATTLER_BODY_MAX_Y 2.48f
#define NF_CATTLER_FOOT_BAND_CENTER_Y 0.18f
#define NF_CATTLER_FOOT_BAND_HEIGHT 0.10f
#define NF_CATTLER_KNEE_BAND_CENTER_Y 0.60f
#define NF_CATTLER_KNEE_BAND_HEIGHT 0.10f

NfHitZone nf_cattler_hit_zone_for_ray(
    NfVec3 origin, NfVec3 direction, NfVec3 actor_base,
    float max_distance, float *distance_out);

#endif
