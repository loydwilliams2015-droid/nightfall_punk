/* Spiral 09: whole-batch PRESENTATION read-admission laboratory. Not A6 authority. */
#ifndef NF_SPIRAL09_BUNDLE_H
#define NF_SPIRAL09_BUNDLE_H
#include "../spiral08/spiral08_consumer.h"
typedef struct {
    const NfS08Frame *actors;
    size_t actor_count;
    const NfS08Frame *previous_poses; /* optional: if non-NULL, actor_count entries */
    float max_pose_step;
    uint32_t max_pose_ticks;
    const uint64_t *event_ids;
    size_t event_count;
    uint64_t last_event_id;
    const size_t *tile_bytes;
    size_t tile_count, max_tiles, max_tile_bytes;
    NfS08Cursor proposed_cursor;
    uint64_t expected_source_digest;
} NfS09Batch;
typedef struct {
    NfS08Cursor cursor;
    uint64_t last_event_id;
    size_t actors, events, tiles, resident_bytes;
} NfS09Plan;
/* No externally visible output is touched until ALL checks pass. */
static inline bool nf_s09_plan(const NfS09Batch *b, const NfS08Cursor *prior,
                              NfS09Plan *output) {
    if (!b || !output || !nf_s08_render_batch(b->actors,b->actor_count) ||
        !nf_s08_consume(prior,&b->proposed_cursor,b->expected_source_digest) ||
        b->actors[0].generation!=b->proposed_cursor.generation ||
        b->actors[0].tick!=b->proposed_cursor.tick ||
        b->actors[0].revision!=b->proposed_cursor.revision) return false;
    if (b->previous_poses) {
        for(size_t i=0;i<b->actor_count;i++) {
            if (b->previous_poses[i].actor!=b->actors[i].actor ||
                !nf_s08_animation_pair(b->previous_poses[i],b->actors[i],
                                      b->max_pose_step,b->max_pose_ticks))return false;
        }
    }
    if (b->event_count && !nf_s08_audio_events(b->event_ids,b->event_count,b->last_event_id))return false;
    if (b->tile_count && !nf_s08_tiles(b->tile_bytes,b->tile_count,b->max_tiles,b->max_tile_bytes))return false;
    size_t used=0;
    for(size_t i=0;i<b->tile_count;i++)used+=b->tile_bytes[i]; /* bounded above by nf_s08_tiles */
    const NfS09Plan ready = {
        b->proposed_cursor,
        b->event_count ? b->event_ids[b->event_count-1] : b->last_event_id,
        b->actor_count,b->event_count,b->tile_count,used
    };
    *output=ready;
    return true;
}
#endif
