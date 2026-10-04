#ifndef NF_EMBODY18A6_H
#define NF_EMBODY18A6_H
/* Opt-in, single-actor 1.8A.6 embodiment authority bridge.
   Not a replacement for the multiplayer/AI world scheduler. */
#include "nf_contact18a5_close.h"
#include "nf_camera.h"
#include "nf_logic17c.h"

typedef enum Nf18a6Status {
    NF18A6_COMMITTED = 0, NF18A6_BLOCKED, NF18A6_PENDING,
    NF18A6_STALE, NF18A6_UNSUPPORTED, NF18A6_INVALID, NF18A6_DISK_FAILED
} Nf18a6Status;
typedef enum Nf18a6Model {
    NF18A6_LEGACY_REFERENCE=0,  /* legacy NfWorld movement only: test control */
    NF18A6_LAB_ONLY=1,          /* physical A5 body, no NfActor: test control */
    NF18A6_WORLD_BRIDGE=2,      /* selected authority integration */
    NF18A6_WORLD_CAMERA=3       /* selected bridge + read-only presentation */
} Nf18a6Model;
/* Red = actor's proposed physical action; blue = material obstruction or
   response. These are diagnostic names for scoped projections, NOT replacements
   for the seven typed 1.7B domains. Purple only for a proven same-target,
   same-tick bidirectional material dependency. */
typedef struct Nf18a6TickTrace {
    uint32_t tick;
    uint32_t target_id;
    uint32_t dependency_hash;
    uint8_t red_proposed;
    uint8_t blue_material_checked;
    uint8_t purple_present;
    uint8_t purple_status; /* Nf17cPurpleStatus if present */
    NfWorldTickStatus world_status;
    Nf18a6Status embodiment_status;
} Nf18a6TickTrace;
typedef struct Nf18a6Runtime {
    Nf18a5Integrated physical;
    NfCameraState camera;
    uint32_t last_commit_tick;
    uint32_t actor_id;
    uint32_t object_collider_index; /* NfWorld material/weapon geometry mirror */
    uint32_t last_hash;
    uint32_t model;
    uint64_t accepted, pending, blocked;
    /* Borrowed provider/journal lifetime must outlive the world-tick binding. */
    Nf18a5FineProvider tick_provider;
    void *tick_provider_context;
    const char *tick_journal;
    Nf18a6TickTrace tick_trace;
} Nf18a6Runtime;
typedef struct Nf18a6Outcome {
    Nf18a6Status status;
    uint32_t tick, world_revision, material_epoch;
    uint32_t raw_samples, event_count;
    uint32_t authoritative_hash;
    uint8_t actual_impulse, cache_loads, camera_updated, reserved;
    NfVec3 realized_feet;
} Nf18a6Outcome;
typedef struct Nf18a6Snapshot {
    uint32_t tick, revision, material_epoch, contact_digest;
    uint32_t actor_id, state_hash;
    NfVec3 feet, velocity;
} Nf18a6Snapshot;

/* Initializes from an existing real NfActor. Caller provides world/chunk
   witnesses and crate state; no authority is inferred from the visual scene. */
bool nf18a6_init(Nf18a6Runtime *r,NfWorld *world,
                NfEntityId actor_id,Nf18a4Body object,uint8_t chunk_capacity,
                Nf18a6Model model);
/* The bridge owns one actor tick. Do not call nf_world_step on this actor in
   that tick: doing so would create a second movement authority. No mutations
   on non-COMMITTED status. Physics WAL is local/same-ABI, not a full game WAL.
   Camera and movement are projected AFTER the A5 physical transaction commits. */
Nf18a6Outcome nf18a6_step(Nf18a6Runtime *r,NfWorld *world,NfMoveInput input,
                      Nf18a5FineProvider provider,void *provider_ctx,
                      float dt,const char *journal);
/* Reconstruct NfWorld authoritative body and mirrored crate from the A5
   local journal after crash. Caller must rebuild the same canonical NfWorld
   context first. No network reconstitution or multi-actor WAL promise. */
/* Safe opt-in scheduler integration, single active actor only. nf_world_step
   dispatches here instead of calling historical locomotion on this tick. */
bool nf18a6_bind_world_tick(NfWorld *world,Nf18a6Runtime *runtime,
                            Nf18a5FineProvider provider,void *provider_context,
                            const char *journal);
bool nf18a6_unbind_world_tick(NfWorld *world,Nf18a6Runtime *runtime);
/* Pure diagnostic scoped SCC test using existing 1.7C Purple builder. */
Nf18a6TickTrace nf18a6_classify_tick(uint32_t tick,uint32_t red_target,
             uint32_t blue_target,bool red_to_blue,bool blue_to_red,
             uint32_t red_material_mask,uint32_t blue_material_mask);
bool nf18a6_recover(Nf18a6Runtime *r,NfWorld *world,const char *journal);
Nf18a6Snapshot nf18a6_snapshot(const Nf18a6Runtime *r,const NfWorld *world);
/* Read-only diagnostic agreement: no client state can become world authority. */
bool nf18a6_snapshot_matches(const Nf18a6Snapshot *a,const Nf18a6Snapshot *b);
const char *nf18a6_status_name(Nf18a6Status s);
#endif
