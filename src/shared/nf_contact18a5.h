#ifndef NF_CONTACT18A5_H
#define NF_CONTACT18A5_H
#include "nf_contact18a4.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF18A5_CHUNK_EDGE_M 4
#define NF18A5_CANON_VOXELS 64u
#define NF18A5_FINE_MAX_VOXELS 4096u
#define NF18A5_MAX_CHUNKS 12u
#define NF18A5_MAX_STREAMS 16u
#define NF18A5_SUMMARIES 64u
#define NF18A5_OUTBOX 32u
#define NF18A5_RAW_RETAIN 256u

/* Canonical 1 m voxels contain 0=FREE, 1=SOLID, 2=MIXED.
   MIXED requires actually resident, epoch-matching fine data to claim FREE. */
typedef enum Nf18a5Voxel { NF18A5_FREE=0, NF18A5_SOLID=1, NF18A5_MIXED=2 } Nf18a5Voxel;
typedef enum Nf18a5GridPolicy {
    NF18A5_COARSE_ONLY=0,
    NF18A5_EAGER_FINE=1,
    NF18A5_SELECTIVE_FINE=2,
    NF18A5_HYSTERETIC_FINE=3,
    NF18A5_UNSAFE_FINE_CONTROL=4
} Nf18a5GridPolicy;
typedef enum Nf18a5Query { NF18A5_Q_FREE=0, NF18A5_Q_SOLID, NF18A5_Q_PENDING, NF18A5_Q_INVALID } Nf18a5Query;
typedef struct Nf18a5Chunk {
    int32_t x,y,z;
    uint32_t canonical_epoch, fine_parent_epoch, last_access_tick;
    uint16_t fine_count;
    uint8_t loaded, fine_resolution, fine_valid, pinned;
    uint8_t canonical[NF18A5_CANON_VOXELS];
    uint8_t fine[NF18A5_FINE_MAX_VOXELS];
} Nf18a5Chunk;
typedef struct Nf18a5Grid {
    Nf18a5Chunk chunks[NF18A5_MAX_CHUNKS];
    uint32_t global_revision; /* Material epoch: NOT affected by cache residency. */
    uint32_t cache_revision;
    uint8_t count, capacity, policy, reserved;
} Nf18a5Grid;
void nf18a5_grid_init(Nf18a5Grid *g, uint8_t capacity, Nf18a5GridPolicy policy);
bool nf18a5_load_canonical(Nf18a5Grid *g,int32_t x,int32_t y,int32_t z,
                            const uint8_t cells[NF18A5_CANON_VOXELS],uint32_t tick);
bool nf18a5_load_fine(Nf18a5Grid *g,int32_t x,int32_t y,int32_t z,
                       uint8_t scale,const uint8_t *voxels,size_t len,uint32_t parent_epoch,uint32_t tick);
Nf18a5Query nf18a5_query(Nf18a5Grid *g,float x,float y,float z,uint32_t tick,bool exact);
bool nf18a5_pin(Nf18a5Grid *g,int32_t x,int32_t y,int32_t z,bool pinned);
bool nf18a5_release_fine(Nf18a5Grid *g,int32_t x,int32_t y,int32_t z);
bool nf18a5_evict(Nf18a5Grid *g,uint32_t tick,uint32_t idle_ticks);
/* Policy decides whether the caller should request fine authoritative data. */
bool nf18a5_should_refine(Nf18a5Grid *g,int32_t x,int32_t y,int32_t z,
                          bool consequential,uint32_t tick,uint32_t retain_ticks);
size_t nf18a5_resident_bytes(const Nf18a5Grid *g);

typedef enum Nf18a5HistoryPolicy {
    NF18A5_RAW_ALL=0,
    NF18A5_PEAK_ONLY=1,
    NF18A5_WINDOWS_ONLY=2,
    NF18A5_CUMULATIVE_DURABLE=3,
    NF18A5_OVERWRITE_CONTROL=4
} Nf18a5HistoryPolicy;
enum { NF18A5_REASON_PEAK=1, NF18A5_REASON_ACCUM=2,
       NF18A5_REASON_DURATION=4, NF18A5_REASON_LOAD=8 };
typedef struct Nf18a5Sample {
    uint32_t tick, contact_id, body_a, body_b;
    float normal_impulse, resting_force, approach_speed;
    uint32_t material_epoch;
} Nf18a5Sample;
typedef struct Nf18a5Summary {
    uint32_t contact_id, first_tick, last_tick, count, material_epoch, digest;
    double impulse_integral, force_time;
    float peak_impulse, peak_force;
} Nf18a5Summary;
typedef struct Nf18a5Event {
    uint64_t sequence;
    uint32_t contact_id, first_tick, last_tick, samples, digest, material_epoch;
    uint8_t reason_mask;
    uint8_t reserved[3];
    double impulse_sum, force_time;
    float impulse_peak, force_peak;
} Nf18a5Event;
typedef struct Nf18a5Stream {
    uint32_t contact_id, body_a, body_b, last_tick, epoch_first, epoch_count;
    uint32_t epoch_digest, last_material_epoch;
    uint8_t active;
    Nf18a5Summary window;
    double epoch_impulse, epoch_load;
    float epoch_peak, epoch_force_peak;
} Nf18a5Stream;
typedef struct Nf18a5History {
    Nf18a5Stream streams[NF18A5_MAX_STREAMS];
    Nf18a5Summary summaries[NF18A5_SUMMARIES];
    Nf18a5Event outbox[NF18A5_OUTBOX];
    Nf18a5Sample raw_records[NF18A5_RAW_RETAIN];
    uint32_t raw_retained;
    uint64_t next_sequence, ack_sequence;
    uint32_t last_tick, last_world_version, owner, summary_count, summary_digest;
    uint32_t outbox_count, generated_events, summary_evictions, raw_samples, lost_records;
    uint16_t window_ticks, duration_ticks;
    float peak_threshold, cumulative_threshold, load_threshold;
    uint8_t initialized, policy;
} Nf18a5History;
void nf18a5_history_init(Nf18a5History *h,Nf18a5HistoryPolicy policy);
/* Atomic within one history object. Called after physical world transaction commit;
   the caller must not claim durable persistence until outbox ACK. */
bool nf18a5_history_commit(Nf18a5History *h,uint32_t tick,uint32_t version,
                           uint32_t owner,const Nf18a5Sample *samples,size_t count);
bool nf18a5_history_close(Nf18a5History *h,uint32_t tick,uint32_t version,
                          uint32_t owner,uint32_t contact_id);
/* The outbox is append-only until an external durable sink confirms a sequence.
   ACK must be for a contiguous prefix. */
bool nf18a5_ack(Nf18a5History *h,uint64_t through_sequence);
/* POSIX lab sink: validate existing append log, append missing events,
   fsync, then ACK. A separate authoritative world WAL is still future work. */
bool nf18a5_persist_outbox(Nf18a5History *h,const char *path);
bool nf18a5_receipt_sample(const Nf18a4Receipt *receipt,uint32_t body_a,
                            uint32_t body_b,float resting_force,uint32_t material_epoch,
                            Nf18a5Sample *out);
/* A world revision and its journal commit as one local copy-on-write operation;
   this is NOT proof of disk durability nor an installed network transaction. */
typedef struct Nf18a5World {
    Nf18a5Grid grid;
    Nf18a5History history;
    uint32_t revision, tick;
} Nf18a5World;
void nf18a5_world_init(Nf18a5World *w,uint8_t capacity);
bool nf18a5_world_commit(Nf18a5World *w,uint32_t expected_version,uint32_t tick,
                         uint32_t owner,const Nf18a5Sample *samples,size_t count);
/* Atomically stage one canonical material edit with its committed contacts. */
bool nf18a5_world_material_commit(Nf18a5World *w,uint32_t expected_version,
                                  uint32_t tick,uint32_t owner,
                                  int32_t cx,int32_t cy,int32_t cz,
                                  const uint8_t canonical[NF18A5_CANON_VOXELS],
                                  const Nf18a5Sample *samples,size_t count);
uint32_t nf18a5_history_hash(const Nf18a5History *h);
#endif
