#ifndef NF_CONTACT18A2_H
#define NF_CONTACT18A2_H

#include "nf_contact18a.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF18A2_MAX_CHUNKS 32u
#define NF18A2_CANONICAL_CHUNK_CELLS 16u
#define NF18A2_FINE_CHUNK_CELLS 256u
#define NF18A2_MAX_CONTACT_STREAMS 16u
#define NF18A2_SUMMARY_RING 64u
#define NF18A2_EVENT_RING 64u

/* The capsule is the canonical actor profile. Rounded-box behavior is an
 * explicit contact-policy feature, never a license to bypass geometry. */
typedef enum Nf18a2ContactProfile {
    NF18A2_PROFILE_CAPSULE = 0,
    NF18A2_PROFILE_ROUNDED_BOX_SUPPORT = 1,
    NF18A2_PROFILE_ROUNDED_BOX_LADDER = 2
} Nf18a2ContactProfile;

typedef struct Nf18a2ShapePolicy {
    float radius;
    float height;
    float foot_flat_radius;
    float step_height;
    float walkable_normal_y;
} Nf18a2ShapePolicy;

typedef struct Nf18a2ShapeRequest {
    bool stair_candidate;
    bool ladder_candidate;
    bool material_support;
    bool clearance_valid;
    bool affordance_authorized;
} Nf18a2ShapeRequest;

Nf18a2ContactProfile nf18a2_choose_profile(Nf18a2ShapePolicy shape,
                                            Nf18a2ShapeRequest request);
bool nf18a2_profile_valid(Nf18a2ShapePolicy shape);
/* Exact Euclidean distance from vertical capsule segment to AABB; negative
 * separation indicates overlap. Can be used by a future true capsule CCD. */
float nf18a2_capsule_box_separation(NfVec3 feet, Nf18a2ShapePolicy shape,
                                    const Nf18aCollider *box);

typedef struct Nf18a2Motor {
    float target_x;
    float target_z;
    float max_accel;
    float max_force;
    float gravity;
    float damping;
} Nf18a2Motor;

typedef struct Nf18a2RigidBody {
    uint32_t id;
    NfVec3 velocity;
    float mass;
} Nf18a2RigidBody;

bool nf18a2_motor_step(Nf18a2RigidBody *body, Nf18a2Motor motor,
                        float dt, bool grounded);
/* Solves 1D contact normal impulse with equal-and-opposite momentum exchange;
 * no rotation/friction or island integration is claimed. */
bool nf18a2_exchange_normal_impulse(Nf18a2RigidBody *a,
                                    Nf18a2RigidBody *b,
                                    NfVec3 normal_b_to_a,
                                    float restitution,
                                    float *impulse);

typedef struct Nf18a2AdaptiveConfig {
    uint8_t cheap_iterations;
    uint8_t reserve_iterations;
    uint8_t hard_max_iterations;
} Nf18a2AdaptiveConfig;

typedef struct Nf18a2AdaptiveResult {
    Nf18aSolveResult solve;
    uint8_t attempts;
    uint8_t final_budget;
    uint8_t reserve_used;
    uint8_t reserved;
} Nf18a2AdaptiveResult;
Nf18a2AdaptiveConfig nf18a2_default_adaptive(void);
Nf18a2AdaptiveResult nf18a2_solve_adaptive(
    uint32_t tick, uint32_t body_id, NfVec3 feet, NfVec3 velocity,
    Nf18aShape shape, float dt,
    const Nf18aCollider *colliders, size_t count,
    Nf18aConfig collision, Nf18a2AdaptiveConfig budget);

typedef enum Nf18a2ChunkResolution {
    NF18A2_CANONICAL_1M = 0,
    NF18A2_REFINED_0_5M = 1,
    NF18A2_REFINED_0_25M = 2
} Nf18a2ChunkResolution;

typedef struct Nf18a2Chunk {
    int32_t x;
    int32_t y;
    int32_t z;
    uint32_t version;
    uint32_t last_active_tick;
    uint8_t resolution;
    uint8_t pinned;
    uint8_t fine_resident;
    uint8_t canonical_resident;
    uint8_t canonical[NF18A2_CANONICAL_CHUNK_CELLS];
    uint8_t fine[NF18A2_FINE_CHUNK_CELLS];
} Nf18a2Chunk;

typedef struct Nf18a2ChunkCache {
    Nf18a2Chunk chunks[NF18A2_MAX_CHUNKS];
    uint8_t count;
    uint8_t capacity;
    uint16_t reserved;
    uint32_t global_epoch;
} Nf18a2ChunkCache;
void nf18a2_chunk_cache_init(Nf18a2ChunkCache *cache, uint8_t capacity);
/* Returns NULL explicitly on bounded capacity exhaustion. No silent eviction. */
Nf18a2Chunk *nf18a2_chunk_touch(Nf18a2ChunkCache *cache,
                                  int32_t x, int32_t y, int32_t z,
                                  Nf18a2ChunkResolution requested,
                                  uint32_t tick);
/* Optional explicit release following a deterministic idle-barrier. */
/* Supply authoritative occupancy/material bytes, not inferred upsampling.
 * Each 4m chunk has 4x4 canonical cells; fine 0.5m=8x8 or 0.25m=16x16. */
bool nf18a2_chunk_load_canonical(Nf18a2ChunkCache *cache,
                                   int32_t x, int32_t y, int32_t z,
                                   const uint8_t samples[NF18A2_CANONICAL_CHUNK_CELLS],
                                   uint32_t tick);
bool nf18a2_chunk_load_fine(Nf18a2ChunkCache *cache,
                              int32_t x, int32_t y, int32_t z,
                              Nf18a2ChunkResolution resolution,
                              const uint8_t *samples, size_t count, uint32_t tick);
bool nf18a2_chunk_evict_idle(Nf18a2ChunkCache *cache,
                               uint32_t tick, uint32_t min_idle_ticks);
int32_t nf18a2_chunk_coord(float world_position, float chunk_extent_m);

typedef struct Nf18a2ContactSummary {
    uint32_t contact_id;
    uint32_t first_tick;
    uint32_t last_tick;
    uint32_t samples;
    float impulse_sum;
    float impulse_peak;
    float approach_peak;
    uint32_t digest;
} Nf18a2ContactSummary;

typedef enum Nf18a2ThresholdKind {
    NF18A2_THRESHOLD_PEAK = 1u,
    NF18A2_THRESHOLD_ACCUMULATED = 2u,
    NF18A2_THRESHOLD_DURATION = 4u,
    NF18A2_THRESHOLD_EXTERNAL = 8u
} Nf18a2ThresholdKind;

typedef struct Nf18a2PersistentEvent {
    uint32_t contact_id;
    uint32_t first_tick;
    uint32_t last_tick;
    uint32_t contributing_samples;
    uint32_t digest;
    uint8_t reason_mask;
    uint8_t reserved[3];
    float impulse_sum;
    float impulse_peak;
} Nf18a2PersistentEvent;

typedef struct Nf18a2HistoryPolicy {
    uint32_t summary_ticks; /* default 15 ticks = 250 ms at 60 Hz */
    uint32_t duration_ticks;
    float peak_impulse;
    float accumulated_impulse;
} Nf18a2HistoryPolicy;

typedef struct Nf18a2HistoryStream {
    uint32_t contact_id;
    uint32_t window_start;
    uint32_t last_tick;
    uint32_t epoch_start;
    uint32_t epoch_samples;
    uint32_t window_samples;
    uint32_t digest;
    float window_sum;
    float window_peak;
    float window_speed_peak;
    float epoch_sum;
    float epoch_peak;
    uint8_t active;
    uint8_t events_promoted;
    uint16_t reserved;
} Nf18a2HistoryStream;

typedef struct Nf18a2CondensedHistory {
    Nf18a2HistoryPolicy policy;
    Nf18a2HistoryStream streams[NF18A2_MAX_CONTACT_STREAMS];
    Nf18a2ContactSummary summaries[NF18A2_SUMMARY_RING];
    Nf18a2PersistentEvent events[NF18A2_EVENT_RING];
    uint32_t summary_head;
    uint32_t summary_count;
    uint32_t event_head;
    uint32_t event_count;
    uint32_t evicted_summary_digest;
    uint32_t evicted_event_digest;
    uint32_t summary_evictions;
    uint32_t event_evictions;
    uint32_t last_tick;
    uint32_t last_version;
    uint32_t owner_id;
    uint8_t initialized;
    uint8_t reserved[3];
} Nf18a2CondensedHistory;

Nf18a2HistoryPolicy nf18a2_default_history_policy(void);
void nf18a2_condensed_history_init(Nf18a2CondensedHistory *history,
                                    Nf18a2HistoryPolicy policy);
/* Ingest only post-authoritative-commit contact evidence. No preview may
 * promote an event. Reject non-monotone state versions and overcapacity. */
bool nf18a2_history_commit_samples(Nf18a2CondensedHistory *history,
                                    uint32_t tick, uint32_t state_version,
                                    uint32_t owner_id,
                                    const Nf18aContact *contacts, size_t count);
/* Emits closed windows at/after tick. Event promotion is cumulative over
 * consecutive window summaries (never a raw-sample-to-event bypass). */
bool nf18a2_history_advance(Nf18a2CondensedHistory *history,
                             uint32_t tick);
/* Force-close an authoritative contact when its support/contact relation ends. */
bool nf18a2_history_end_contact(Nf18a2CondensedHistory *history,
                                 uint32_t contact_id, uint32_t tick,
                                 uint32_t state_version, uint32_t owner_id);

#endif
