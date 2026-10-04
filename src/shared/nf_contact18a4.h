#ifndef NF_CONTACT18A4_H
#define NF_CONTACT18A4_H
#include "nf_contact18a3.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF18A4_MAX_BODIES 8u
#define NF18A4_MAX_CONSTRAINTS 16u

/* B->A unit normal, point in world coordinates. Nonzero inverse inertias
   are principal-axis values in the present world frame (no orientation solve). */
typedef struct Nf18a4Body {
    uint32_t id;
    NfVec3 center, velocity, omega;
    NfVec3 inverse_inertia;
    float inverse_mass;
    float radius, height; /* Actor capsule if nonzero; ignored for a crate */
    NfVec3 box_half;      /* Dynamic crate if not an actor */
    uint32_t last_traversal_stage_tick;
    uint32_t last_traversal_stage_version;
} Nf18a4Body;

typedef struct Nf18a4Motor {
    NfVec3 target_velocity;
    float max_force;
    float max_accel;
} Nf18a4Motor;

typedef struct Nf18a4MotorReceipt {
    NfVec3 impulse;
    float work_joules;   /* signed change in translational kinetic energy */
    float applied_force;
} Nf18a4MotorReceipt;

typedef enum Nf18a4Policy {
    NF18A4_ESTIMATE_ONLY_CONTROL = 0,
    NF18A4_RECIPROCAL_LINEAR = 1,
    NF18A4_ANGULAR_NORMAL = 2,
    NF18A4_FRICTION_FIXED6 = 3,
    NF18A4_FRICTION_ADAPTIVE = 4
} Nf18a4Policy;

typedef enum Nf18a4Status {
    NF18A4_ACCEPTED = 0,
    NF18A4_SEPARATING = 1,
    NF18A4_PENDING_CONTACT = 2,
    NF18A4_INVALID = 3,
    NF18A4_GATED = 4
} Nf18a4Status;

typedef struct Nf18a4Constraint {
    uint32_t body_a, body_b, feature_id;
    NfVec3 normal_b_to_a, contact_point;
    float restitution, friction;
} Nf18a4Constraint;

typedef struct Nf18a4Receipt {
    uint32_t tick, contact_id, world_version;
    NfVec3 normal_impulse, tangent_impulse;
    float signed_energy_delta;
    uint8_t status, impulses_applied, iterations, reserve_used;
    uint8_t policy, reserved[3];
} Nf18a4Receipt;

typedef struct Nf18a4IslandResult {
    uint8_t status, iterations, reserve_used, contact_count;
    uint16_t constraint_evaluations;
    Nf18a4Receipt receipts[NF18A4_MAX_CONSTRAINTS];
} Nf18a4IslandResult;

typedef struct Nf18a4PairResult {
    Nf18a4Body actor, object;
    Nf18a4Receipt receipt;
    Nf18a4MotorReceipt motor_receipt;
    Nf18aContact geometric_contact;
    uint8_t swept_hit, status;
    float toi;
} Nf18a4PairResult;

bool nf18a4_body_valid(const Nf18a4Body *body);
bool nf18a4_motor_drive(Nf18a4Body *actor, Nf18a4Motor motor,
                        float dt, Nf18a4MotorReceipt *receipt);
/* Applies real velocity and angular-velocity changes to both bodies when
   dynamic. ESTIMATE_ONLY is a scientific negative control, never production. */
Nf18a4Receipt nf18a4_apply_contact(Nf18a4Body *a, Nf18a4Body *b,
                                   Nf18a4Constraint contact,
                                   Nf18a4Policy policy, uint32_t tick,
                                   uint32_t world_version);
/* Deterministic stable ordering, bounded 2/4/6 budget; no mutation on
   INVALID input or insufficient capacity. Receipts are not world commits. */
Nf18a4IslandResult nf18a4_solve_island(Nf18a4Body *bodies, size_t body_count,
                                     const Nf18a4Constraint *contacts,
                                     size_t contact_count, Nf18a4Policy policy,
                                     uint32_t tick, uint32_t world_version);
/* True translating capsule/AABB CCD (1.8A.3), then real pairwise response;
   no angular CCD, rotating crate collision geometry, or general manifold. */
Nf18a4PairResult nf18a4_pair_step(Nf18a4Body actor, Nf18a4Body crate,
                                 Nf18a2ShapePolicy shape, Nf18a4Motor motor,
                                 Nf18a4Policy policy, float dt,
                                 uint32_t tick, uint32_t world_version,
                                 float restitution, float friction);
/* This is a TRUSTED adjudication bridge, not a commit or teleport.
   It checks frame binding and produces bounded motor impulse only. */
Nf18a4Status nf18a4_stage_traversal(Nf18a4Body *actor,
                                  Nf18a3Geometry world,
                                  Nf18a3Contract contract,
                                  Nf18a3Request request, Nf18a4Motor motor, float dt,
                                  uint32_t tick, uint32_t world_version,
                                  Nf18a4MotorReceipt *receipt);
/* Adapts an APPLIED contact receipt into 1.8A.2 summary input.
   The caller still owns authoritative world commit and durability. */
bool nf18a4_pair_to_sample(const Nf18a4PairResult *pair,
                           Nf18aContact *sample);
bool nf18a4_receipt_to_sample(const Nf18a4Receipt *receipt,
                             uint32_t body_a, uint32_t body_b,
                             Nf18aContact *sample);
const char *nf18a4_policy_name(Nf18a4Policy policy);
uint32_t nf18a4_pair_hash(const Nf18a4PairResult *result);
#endif
