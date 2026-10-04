#ifndef NF_CONTACT18A3_H
#define NF_CONTACT18A3_H

#include "nf_contact18a2.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 1.8A.3 shape/traversal candidate.  Normal points obstacle -> actor.
 * Geometry and traversal are independently adjudicated on the SAME tick,
 * world version, actor and feature.  Only dual approval may commit. */
typedef enum Nf18a3Policy {
    NF18A3_BOX_BASELINE = 0,
    NF18A3_CAPSULE_STRICT = 1,
    NF18A3_HYBRID_DUAL = 2,
    NF18A3_CAPSULE_EASY_NEGATIVE = 3,
    NF18A3_HYBRID_AUTH_BYPASS_NEGATIVE = 4
} Nf18a3Policy;

typedef enum Nf18a3Transition {
    NF18A3_STEP = 1,
    NF18A3_LADDER_ATTACH = 2,
    NF18A3_LADDER_EXIT = 3
} Nf18a3Transition;

typedef enum Nf18a3Reason {
    NF18A3_OK = 0,
    NF18A3_INVALID = 1,
    NF18A3_STALE_WORLD = 2,
    NF18A3_WRONG_ACTOR = 3,
    NF18A3_WRONG_FEATURE = 4,
    NF18A3_NO_AUTHORITY = 5,
    NF18A3_UNSUPPORTED = 6,
    NF18A3_TOO_HIGH = 7,
    NF18A3_NO_CLEARANCE = 8,
    NF18A3_TOO_FAR = 9,
    NF18A3_BAD_ALIGNMENT = 10,
    NF18A3_MOVING_SUPPORT = 11,
    NF18A3_NO_LANDING = 12
} Nf18a3Reason;

typedef struct Nf18a3Geometry {
    const Nf18aCollider *solids;
    size_t solid_count;
    const Nf18aCollider *ladders; /* geometry triggers, not solids */
    size_t ladder_count;
    uint32_t world_version;
    uint32_t tick;
} Nf18a3Geometry;

typedef struct Nf18a3Contract {
    uint32_t actor_id;
    uint32_t feature_id;
    uint32_t world_version;
    uint32_t tick;
    uint8_t may_step;
    uint8_t may_attach_ladder;
    uint8_t may_exit_ladder;
    uint8_t material_support_valid;
    uint8_t support_stable;
    uint8_t allow_dynamic_support;
    uint8_t reserved[2];
    float authoritative_step_limit;
    float authoritative_reach_limit;
    float authoritative_min_facing_cosine;
    float authoritative_min_landing_width;
    float authoritative_body_radius;
    float authoritative_body_height;
    float authoritative_foot_flat_radius;
    NfVec3 authoritative_feet;
    uint32_t current_support_id;
    uint8_t actor_grounded;
    uint8_t actor_ladder_attached;
    uint8_t reserved_state[2];
} Nf18a3Contract;

typedef struct Nf18a3Request {
    Nf18a3Transition transition;
    Nf18a3Policy policy;
    uint32_t actor_id;
    uint32_t feature_id;
    NfVec3 feet;
    NfVec3 destination;
    NfVec3 facing;          /* horizontal, from actor toward feature */
    float body_radius;
    float body_height;
    float foot_flat_radius;
    float max_step;
    float max_ladder_reach;
    float min_facing_cosine;
    float min_landing_width;
    uint8_t grounded;
    uint8_t ladder_attached;
    uint8_t reserved[2];
} Nf18a3Request;

typedef struct Nf18a3Decision {
    Nf18a3Reason geometry_reason;
    Nf18a3Reason contract_reason;
    uint8_t geometry_approved;
    uint8_t contract_approved;
    uint8_t commit_eligible;
    uint8_t used_flat_support;
    float measured_step_height;
    float min_separation;
    uint16_t shape_queries;
    uint16_t swept_contacts;
    uint32_t feature_id;
    uint32_t actor_id;
    uint32_t world_version;
    uint32_t tick;
    uint32_t policy;
    uint32_t transition;
    NfVec3 original_feet;
    NfVec3 target_feet;
    uint32_t witness_hash;
} Nf18a3Decision;

/* Geometrically exact for a vertical line-segment capsule and translating
 * axis-aligned box under relative constant linear motion over dt. Does NOT
 * handle angular-motion CCD, arbitrary meshes or generalized convex hulls. */
bool nf18a3_capsule_sweep(NfVec3 feet, Nf18a2ShapePolicy shape,
                          NfVec3 displacement, Nf18aCollider collider,
                          float dt, Nf18aContact *out);

/* Resolves BOTH independent jurisdictions; negative-control policies
 * deliberately waive one protection and are never admissible in gameplay. */
Nf18a3Decision nf18a3_adjudicate(Nf18a3Geometry world,
                                 Nf18a3Contract contract,
                                 Nf18a3Request request);

bool nf18a3_capsule_clear(Nf18a3Geometry world, NfVec3 feet,
                          Nf18a2ShapePolicy shape);
const char *nf18a3_reason_name(Nf18a3Reason reason);
uint32_t nf18a3_decision_hash(const Nf18a3Decision *decision);

#endif
