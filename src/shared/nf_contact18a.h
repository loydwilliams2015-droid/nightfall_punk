#ifndef NF_CONTACT18A_H
#define NF_CONTACT18A_H

#include "nf_world.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF18A_MAX_CONTACTS 8u
#define NF18A_MAX_HISTORY 64u
#define NF18A_MAX_PLANES 4u

/* Contact normals point from the obstacle toward the moving body.
   All TOIs are fractions of the proposed dt, in [0,1]. */
typedef enum Nf18aContactKind {
    NF18A_CONTACT_NONE = 0,
    NF18A_CONTACT_TOUCH,
    NF18A_CONTACT_INITIAL_OVERLAP
} Nf18aContactKind;

typedef enum Nf18aSolveStatus {
    NF18A_SOLVED = 0,
    NF18A_BLOCKED,
    NF18A_PENDING_BUDGET,
    NF18A_INVALID_START,
    NF18A_INVALID_INPUT
} Nf18aSolveStatus;

typedef enum Nf18aHistoryKind {
    NF18A_EVENT_BEGIN = 0,
    NF18A_EVENT_PERSIST,
    NF18A_EVENT_END,
    NF18A_EVENT_HIT
} Nf18aHistoryKind;

typedef struct Nf18aShape {
    float radius;       /* AABB half extent on X and Z for this baseline */
    float height;       /* feet-to-head, always positive */
    float mass;         /* kg-equivalent for impulse estimate */
} Nf18aShape;

typedef struct Nf18aCollider {
    uint32_t body_id;
    NfVec3 min;
    NfVec3 max;
    NfVec3 velocity; /* constant over step; relative sweep only */
    uint8_t material_channel;
    uint8_t dynamic_body;
    uint16_t reserved;
} Nf18aCollider;

typedef struct Nf18aContact {
    uint32_t body_a;
    uint32_t body_b;
    uint32_t contact_id;
    uint32_t tick;
    NfVec3 point;
    NfVec3 normal;
    float toi;
    float separation;
    float approach_speed;
    float normal_impulse; /* estimate; no equal/opposite dynamic-body integration yet */
    uint8_t kind;
    uint8_t material_channel;
    uint8_t walkable_support;
    uint8_t consequential;
} Nf18aContact;

typedef struct Nf18aSolveResult {
    NfVec3 feet;
    NfVec3 velocity;
    Nf18aContact contacts[NF18A_MAX_CONTACTS];
    uint8_t contact_count;
    uint8_t iterations;
    uint8_t status;
    uint8_t support;
    uint16_t broadphase_candidates;
    uint16_t narrowphase_tests;
    float intended_distance;
    float realized_distance;
} Nf18aSolveResult;

typedef struct Nf18aHistoryEvent {
    uint32_t tick;
    uint32_t contact_id;
    uint8_t kind;
    uint8_t reserved[3];
    float normal_impulse;
} Nf18aHistoryEvent;

typedef struct Nf18aHistory {
    Nf18aHistoryEvent events[NF18A_MAX_HISTORY];
    uint32_t active[NF18A_MAX_CONTACTS];
    uint8_t active_count;
    uint8_t event_count;
    uint16_t head;
    uint32_t cold_hash;
    uint32_t cold_events;
    uint32_t owner_body_id;
    uint32_t last_commit_tick;
    uint32_t last_state_version;
    uint8_t has_committed;
    uint8_t reserved_commit[3];
} Nf18aHistory;

typedef struct Nf18aConfig {
    uint8_t iteration_budget;
    uint8_t max_contacts;
    uint16_t reserved;
    float skin;
    float simultaneous_toi_epsilon;
    float walkable_normal_y;
    float hit_impulse_threshold;
} Nf18aConfig;

Nf18aConfig nf18a_default_config(void);
bool nf18a_sweep_aabb(
    NfVec3 feet, Nf18aShape shape, NfVec3 displacement,
    Nf18aCollider collider, float dt,
    Nf18aContact *out);
Nf18aSolveResult nf18a_solve(
    uint32_t tick, uint32_t body_id, NfVec3 feet, NfVec3 velocity,
    Nf18aShape shape, float dt,
    const Nf18aCollider *colliders, size_t collider_count,
    Nf18aConfig config);
void nf18a_history_init(Nf18aHistory *history);
/* A preview trace is not an authoritative commit. Runtime must call
   nf18a_history_commit only after the world's atomic authoritative commit. */
void nf18a_history_record(Nf18aHistory *history, const Nf18aSolveResult *solve, uint32_t tick);
bool nf18a_history_commit(Nf18aHistory *history, const Nf18aSolveResult *solve,
                          uint32_t tick, uint32_t state_version, uint32_t owner_body_id);
uint32_t nf18a_result_hash(const Nf18aSolveResult *solve);
size_t nf18a_extract_world_colliders(
    const NfWorld *world, Nf18aCollider *out, size_t capacity);
const char *nf18a_status_name(Nf18aSolveStatus status);

#endif