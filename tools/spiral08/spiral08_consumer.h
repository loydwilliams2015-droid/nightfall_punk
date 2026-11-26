/* NIGHTFALL!PUNK Spiral 08 — offline read-only consumer gates.
 * Independent laboratory header. Not A6 admission or production integration.
 * C11, no heap allocation or external dependencies. */
#ifndef NF_SPIRAL08_CONSUMERS_H
#define NF_SPIRAL08_CONSUMERS_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
typedef struct { uint32_t generation, tick, revision, actor; float x,y,z; } NfS08Frame;
typedef struct { uint64_t source_digest; uint32_t generation,tick,revision; } NfS08Cursor;
static inline bool nf_s08_finite(const NfS08Frame *f) {
    return f && f->actor && f->generation && isfinite(f->x) && isfinite(f->y) && isfinite(f->z);
}
/* RENDER: no duplicate actors or mixed accepted-frame cursors. */
static inline bool nf_s08_render_batch(const NfS08Frame *v, size_t n) {
    if (!v || !n || n>32u) return false;
    for(size_t i=0;i<n;i++) {
        if(!nf_s08_finite(&v[i]))return false;
        if(i && (v[i].generation!=v[0].generation || v[i].tick!=v[0].tick || v[i].revision!=v[0].revision))return false;
        for(size_t j=0;j<i;j++)if(v[i].actor==v[j].actor)return false;
    }
    return true;
}
/* ANIMATION: same actor and generation, monotonic bounded motion. */
static inline bool nf_s08_animation_pair(NfS08Frame a,NfS08Frame b, float max_distance, uint32_t max_ticks) {
    if(!nf_s08_finite(&a)||!nf_s08_finite(&b)||a.actor!=b.actor||a.generation!=b.generation ||
       b.tick<=a.tick || b.tick-a.tick>max_ticks || !max_ticks ||
       !(isfinite(max_distance)&&max_distance>0.0f)) return false;
    const double dx=(double)b.x-a.x,dy=(double)b.y-a.y,dz=(double)b.z-a.z;
    return dx*dx+dy*dy+dz*dz <= (double)max_distance*max_distance;
}
/* AUDIO: batch validation only; never opens an output device. */
static inline bool nf_s08_audio_events(const uint64_t *ids,size_t n,uint64_t last_id) {
    if(!ids||!n||n>32u)return false;
    for(size_t i=0;i<n;i++)if(ids[i]<=last_id||(i && ids[i]<=ids[i-1]))return false;
    return true;
}
/* MAP: candidate residency cannot exceed byte or tile budgets. */
static inline bool nf_s08_tiles(const size_t *bytes,size_t n,size_t tile_cap,size_t byte_cap) {
    if(!bytes||!n||!tile_cap||!byte_cap||n>tile_cap||n>64u)return false;
    size_t sum=0;
    for(size_t i=0;i<n;i++) {
        if(!bytes[i]||bytes[i]>byte_cap-sum)return false;
        sum+=bytes[i];
    }
    return true;
}
/* AUTHORITY: read admission only. No minting of leases or A6 publication. */
static inline bool nf_s08_consume(const NfS08Cursor *prior,const NfS08Cursor *next,
                                 uint64_t expected_digest) {
    if(!next || !expected_digest || next->source_digest!=expected_digest || !next->generation)return false;
    if(!prior)return true;
    if(prior->source_digest!=expected_digest || prior->generation!=next->generation)return false;
    return (next->tick>prior->tick)||(next->tick==prior->tick && next->revision>prior->revision);
}
#endif
