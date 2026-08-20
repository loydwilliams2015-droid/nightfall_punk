#ifndef NF_WEAPON_GEOMETRY_H
#define NF_WEAPON_GEOMETRY_H

#include "nf_world.h"

#include <stdbool.h>

typedef struct NfWorldRayHit {
    bool hit;
    int collider_index;
    float distance;
    NfVec3 point;
} NfWorldRayHit;

/* Physical firearm origin: body-relative, never camera-origin authority. */
NfVec3 nf_weapon_muzzle_origin(const NfActor *actor, float yaw_radians);

/* Current authoritative world geometry. Ladders are non-blocking traversal affordances. */
bool nf_weapon_world_first_hit(
    const NfWorld *world,
    NfVec3 origin,
    NfVec3 direction,
    float range,
    NfWorldRayHit *out);

#endif
