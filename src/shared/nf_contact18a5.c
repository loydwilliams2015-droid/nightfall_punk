#include "nf_contact18a5.h"
#include <limits.h>
#include <math.h>
#include <string.h>

static uint32_t m32(uint32_t v){v^=v>>16;v*=0x7feb352du;v^=v>>15;v*=0x846ca68bu;v^=v>>16;return v;}
static uint32_t fd(float x){if(!isfinite(x))return UINT32_MAX;return m32((uint32_t)(int32_t)llround((double)x*1000.0));}
static bool acceptable(const uint8_t *v,size_t n){if(!v)return false;for(size_t i=0;i<n;++i)if(v[i]>2u)return false;return true;}
static Nf18a5Chunk *find_chunk(Nf18a5Grid *g,int32_t x,int32_t y,int32_t z){
    if(!g)return NULL;
    for(unsigned i=0;i<g->count;++i){Nf18a5Chunk *c=&g->chunks[i];if(c->loaded && c->x==x && c->y==y && c->z==z)return c;}
    return NULL;
}
void nf18a5_grid_init(Nf18a5Grid *g,uint8_t capacity,Nf18a5GridPolicy policy){
    if(!g)return;
    memset(g,0,sizeof(*g));g->capacity=capacity<=NF18A5_MAX_CHUNKS?capacity:NF18A5_MAX_CHUNKS;
#ifndef NF18A5_TEST_CONTROLS
    if(policy==NF18A5_UNSAFE_FINE_CONTROL)policy=NF18A5_HYSTERETIC_FINE;
#endif
    g->policy=(uint8_t)policy;g->global_revision=1u;g->cache_revision=1u;
}
bool nf18a5_load_canonical(Nf18a5Grid *g,int32_t x,int32_t y,int32_t z,
                            const uint8_t cells[NF18A5_CANON_VOXELS],uint32_t tick){
    if(!g || !acceptable(cells,NF18A5_CANON_VOXELS) || !g->capacity || !tick ||
       g->global_revision==UINT32_MAX)return false;
    Nf18a5Chunk *c=find_chunk(g,x,y,z);
    if(c && tick<c->last_access_tick)return false;
    if(!c){if(g->count>=g->capacity)return false;c=&g->chunks[g->count++];memset(c,0,sizeof(*c));c->x=x;c->y=y;c->z=z;c->loaded=1u;}
    if(c->canonical_epoch==UINT32_MAX)return false;
    memcpy(c->canonical,cells,NF18A5_CANON_VOXELS);++c->canonical_epoch;
    c->fine_valid=0u;c->fine_resolution=0u;c->fine_count=0u;c->fine_parent_epoch=0u;
    c->last_access_tick=tick;++g->global_revision;++g->cache_revision;return true;
}
bool nf18a5_load_fine(Nf18a5Grid *g,int32_t x,int32_t y,int32_t z,
                       uint8_t scale,const uint8_t *data,size_t len,uint32_t parent,uint32_t tick){
    if(!g || (scale!=2u && scale!=4u) || !data || len!=(size_t)(4u*scale)*(4u*scale)*(4u*scale) ||
       !acceptable(data,len) || g->global_revision==UINT32_MAX)return false;
    Nf18a5Chunk *c=find_chunk(g,x,y,z);
    if(!c || tick<c->last_access_tick || parent!=c->canonical_epoch)return false;
    /* A fine voxel may only refine MIXED. FREE/SOLID canonical facts cannot
       be contradicted by an allegedly fine representation. */
    for(unsigned cy=0;cy<4u;++cy)for(unsigned cz=0;cz<4u;++cz)for(unsigned cx=0;cx<4u;++cx){
        const uint8_t coarse=c->canonical[(cy*4u+cz)*4u+cx];
        bool any0=false,any1=false;
        for(unsigned dy=0;dy<scale;++dy)for(unsigned dz=0;dz<scale;++dz)for(unsigned dx=0;dx<scale;++dx){
            unsigned fy=cy*scale+dy,fz=cz*scale+dz,fx=cx*scale+dx;
            uint8_t v=data[(fy*(4u*scale)+fz)*(4u*scale)+fx];
            if(v==0u)any0=true;else if(v==1u)any1=true;else return false;
            if((coarse==NF18A5_FREE && v!=NF18A5_FREE) ||
               (coarse==NF18A5_SOLID && v!=NF18A5_SOLID))return false;
        }
        if(coarse==NF18A5_MIXED && (!any0 || !any1))return false;
    }
    memcpy(c->fine,data,len);c->fine_count=(uint16_t)len;c->fine_resolution=scale;
    c->fine_valid=1u;c->fine_parent_epoch=parent;c->last_access_tick=tick;
    ++g->cache_revision;return true;
}
static bool cell_coord(float x,int32_t *chunk,unsigned *local){
    if(!isfinite(x))return false;
    double floorval=floor((double)x);
    double chunkval=floor(floorval/4.0);
    if(chunkval<(double)INT32_MIN || chunkval>(double)INT32_MAX)return false;
    *chunk=(int32_t)chunkval;*local=(unsigned)((int64_t)floorval-(int64_t)*chunk*4LL);
    return *local<4u;
}
Nf18a5Query nf18a5_query(Nf18a5Grid *g,float x,float y,float z,uint32_t tick,bool exact){
    if(!g || !tick)return NF18A5_Q_INVALID;
    int32_t cx,cy,cz;unsigned ix,iy,iz;
    if(!cell_coord(x,&cx,&ix)||!cell_coord(y,&cy,&iy)||!cell_coord(z,&cz,&iz))return NF18A5_Q_INVALID;
    Nf18a5Chunk *c=find_chunk(g,cx,cy,cz);
    if(!c || tick<c->last_access_tick)return NF18A5_Q_PENDING;
    c->last_access_tick=tick;
    uint8_t coarse=c->canonical[(iy*4u+iz)*4u+ix];
    if(coarse==NF18A5_SOLID)return NF18A5_Q_SOLID;
    if(coarse==NF18A5_FREE)return NF18A5_Q_FREE;
    if(g->policy==NF18A5_COARSE_ONLY || !exact)return NF18A5_Q_SOLID;
    if(!c->fine_valid || c->fine_parent_epoch!=c->canonical_epoch){
        /* Intentional bad control is never an authoritative mode. */
#ifdef NF18A5_TEST_CONTROLS
        if(g->policy==NF18A5_UNSAFE_FINE_CONTROL)return NF18A5_Q_FREE;
#endif
        return NF18A5_Q_PENDING;
    }
    unsigned s=c->fine_resolution,n=4u*s;
    unsigned fx=(unsigned)floor(((double)x-4.0*(double)cx)*(double)s);
    unsigned fy=(unsigned)floor(((double)y-4.0*(double)cy)*(double)s);
    unsigned fz=(unsigned)floor(((double)z-4.0*(double)cz)*(double)s);
    if(fx>=n || fy>=n || fz>=n)return NF18A5_Q_INVALID;
    unsigned i=(fy*n+fz)*n+fx;
    if(i>=c->fine_count)return NF18A5_Q_INVALID;
    return c->fine[i]==NF18A5_FREE?NF18A5_Q_FREE:NF18A5_Q_SOLID;
}
bool nf18a5_release_fine(Nf18a5Grid *g,int32_t x,int32_t y,int32_t z){
    Nf18a5Chunk *c=find_chunk(g,x,y,z);
    if(!c || c->pinned || !c->fine_valid)return false;
    c->fine_valid=0u;c->fine_count=0u;c->fine_resolution=0u;
    c->fine_parent_epoch=0u;++g->cache_revision;return true;
}
bool nf18a5_pin(Nf18a5Grid *g,int32_t x,int32_t y,int32_t z,bool pinned){
    Nf18a5Chunk *c=find_chunk(g,x,y,z);if(!c)return false;c->pinned=pinned?1u:0u;return true;
}
bool nf18a5_evict(Nf18a5Grid *g,uint32_t tick,uint32_t idle){
    if(!g)return false;
    size_t best=g->count;
    for(size_t i=0;i<g->count;++i){const Nf18a5Chunk *c=&g->chunks[i];
        if(c->pinned||tick<c->last_access_tick||tick-c->last_access_tick<idle)continue;
        if(best==g->count || c->last_access_tick<g->chunks[best].last_access_tick ||
           (c->last_access_tick==g->chunks[best].last_access_tick &&
            (c->x<g->chunks[best].x || (c->x==g->chunks[best].x && (c->y<g->chunks[best].y ||
             (c->y==g->chunks[best].y && c->z<g->chunks[best].z))))))best=i;
    }
    if(best==g->count)return false;
    for(size_t i=best+1u;i<g->count;++i)g->chunks[i-1u]=g->chunks[i];
    --g->count;++g->cache_revision;return true;
}
bool nf18a5_should_refine(Nf18a5Grid *g,int32_t x,int32_t y,int32_t z,
                          bool consequential,uint32_t tick,uint32_t retain_ticks){
    if(!g)return false;
    Nf18a5Chunk *c=find_chunk(g,x,y,z);
    if(!c || tick<c->last_access_tick)return false;
    if(g->policy==NF18A5_COARSE_ONLY)return false;
    if(g->policy==NF18A5_EAGER_FINE)return true;
    if(g->policy==NF18A5_SELECTIVE_FINE)return consequential;
    if(g->policy==NF18A5_HYSTERETIC_FINE)
        return consequential || (c->fine_valid && tick-c->last_access_tick<=retain_ticks);
    return consequential;
}
size_t nf18a5_resident_bytes(const Nf18a5Grid *g){
    if(!g)return 0;
    size_t n=0;
    for(size_t i=0;i<g->count;++i)n+=NF18A5_CANON_VOXELS+g->chunks[i].fine_count;
    return n;
}

void nf18a5_history_init(Nf18a5History *h,Nf18a5HistoryPolicy p){
    if(!h)return;
    memset(h,0,sizeof(*h));h->policy=(uint8_t)p;h->window_ticks=15u;h->duration_ticks=60u;
    h->peak_threshold=20.0f;h->cumulative_threshold=80.0f;h->load_threshold=60.0f;h->next_sequence=1u;
}
static bool valid_history(const Nf18a5History *h){
    return h && h->policy<=NF18A5_OVERWRITE_CONTROL && h->window_ticks>0u &&
      h->duration_ticks>=h->window_ticks && h->next_sequence>0u &&
      h->peak_threshold>0 && isfinite(h->peak_threshold) &&
      h->cumulative_threshold>0 && isfinite(h->cumulative_threshold) &&
      h->load_threshold>0 && isfinite(h->load_threshold);
}
static Nf18a5Stream *find_stream(Nf18a5History *h,uint32_t contact){
    for(unsigned i=0;i<NF18A5_MAX_STREAMS;++i)if(h->streams[i].active && h->streams[i].contact_id==contact)return &h->streams[i];
    return NULL;
}
static Nf18a5Stream *alloc_stream(Nf18a5History *h,uint32_t contact,uint32_t a,uint32_t b,uint32_t tick){
    for(unsigned i=0;i<NF18A5_MAX_STREAMS;++i)if(!h->streams[i].active){
        Nf18a5Stream *s=&h->streams[i];memset(s,0,sizeof(*s));
        s->active=1u;s->contact_id=contact;s->body_a=a;s->body_b=b;s->epoch_first=tick;return s;
    }
    return NULL;
}
static bool queue_event(Nf18a5History *h,const Nf18a5Event *e){
    if(h->outbox_count==NF18A5_OUTBOX){
        if(h->policy!=NF18A5_OVERWRITE_CONTROL)return false;
        memmove(h->outbox,h->outbox+1,(NF18A5_OUTBOX-1u)*sizeof(h->outbox[0]));
        --h->outbox_count;++h->lost_records;
    }
    h->outbox[h->outbox_count++]=*e;return true;
}
static bool flush(Nf18a5History *h,Nf18a5Stream *s){
    if(!s->window.count)return true;
    if(h->summary_count==NF18A5_SUMMARIES){
        h->summary_digest=m32(h->summary_digest^h->summaries[0].digest);
        memmove(h->summaries,h->summaries+1,(NF18A5_SUMMARIES-1u)*sizeof(h->summaries[0]));
        --h->summary_count;++h->summary_evictions;
    }
    h->summaries[h->summary_count++]=s->window;
    uint8_t flags=0u;
    if(s->epoch_peak>=h->peak_threshold)flags|=NF18A5_REASON_PEAK;
    if(s->epoch_impulse>=h->cumulative_threshold)flags|=NF18A5_REASON_ACCUM;
    if(s->epoch_count>=h->duration_ticks)flags|=NF18A5_REASON_DURATION;
    if(s->epoch_load>=h->load_threshold)flags|=NF18A5_REASON_LOAD;
    if(h->policy==NF18A5_PEAK_ONLY) flags&=NF18A5_REASON_PEAK;
    if(h->policy==NF18A5_WINDOWS_ONLY) {
        flags=0u;
        if(s->window.peak_impulse>=h->peak_threshold)flags|=NF18A5_REASON_PEAK;
        if(s->window.impulse_integral>=h->cumulative_threshold)flags|=NF18A5_REASON_ACCUM;
    }
    if(flags){
        if(h->next_sequence==UINT64_MAX)return false;
        Nf18a5Event ev={0};
        ev.sequence=h->next_sequence++;ev.contact_id=s->contact_id;ev.first_tick=s->epoch_first;
        ev.last_tick=s->window.last_tick;ev.samples=s->epoch_count;
        ev.digest=s->epoch_digest;ev.material_epoch=s->last_material_epoch;ev.reason_mask=flags;
        ev.impulse_sum=s->epoch_impulse;ev.force_time=s->epoch_load;
        ev.impulse_peak=s->epoch_peak;ev.force_peak=s->epoch_force_peak;
        if(!queue_event(h,&ev))return false;
        ++h->generated_events;
        s->epoch_count=0u;s->epoch_impulse=0.0;s->epoch_load=0.0;
        s->epoch_peak=0.0f;s->epoch_force_peak=0.0f;s->epoch_digest=0u;
        s->epoch_first=s->window.last_tick+1u;
    }
    memset(&s->window,0,sizeof(s->window));return true;
}
static bool ingest(Nf18a5History *h,const Nf18a5Sample *s){
    Nf18a5Stream *st=find_stream(h,s->contact_id);
    if(!st)st=alloc_stream(h,s->contact_id,s->body_a,s->body_b,s->tick);
    if(!st || st->body_a!=s->body_a || st->body_b!=s->body_b)return false;
    if(st->last_tick && s->tick<=st->last_tick)return false;
    if(st->last_tick && (s->tick-st->last_tick>1u || s->material_epoch!=st->last_material_epoch)){
        if(!flush(h,st))return false;
        st->epoch_count=0u;st->epoch_impulse=0.0;st->epoch_load=0.0;
        st->epoch_peak=0.0f;st->epoch_force_peak=0.0f;
        st->epoch_digest=0u;st->epoch_first=s->tick;
    }
    if(st->window.count && s->tick-st->window.first_tick>=h->window_ticks){
        if(!flush(h,st))return false;
    }
    if(st->window.count==0u){st->window.contact_id=s->contact_id;st->window.first_tick=s->tick;}
    st->last_tick=s->tick;st->last_material_epoch=s->material_epoch;
    Nf18a5Summary *w=&st->window;w->last_tick=s->tick;w->material_epoch=s->material_epoch;
    ++w->count;w->impulse_integral+=s->normal_impulse;w->force_time+=s->resting_force/60.0;
    if(s->normal_impulse>w->peak_impulse)w->peak_impulse=s->normal_impulse;
    if(s->resting_force>w->peak_force)w->peak_force=s->resting_force;
    uint32_t digest=m32(s->tick^s->contact_id^fd(s->normal_impulse)^fd(s->resting_force)^s->material_epoch);
    w->digest=m32(w->digest^digest);
    ++st->epoch_count;st->epoch_impulse+=s->normal_impulse;st->epoch_load+=s->resting_force/60.0;
    if(s->normal_impulse>st->epoch_peak)st->epoch_peak=s->normal_impulse;
    if(s->resting_force>st->epoch_force_peak)st->epoch_force_peak=s->resting_force;
    st->epoch_digest=m32(st->epoch_digest^digest);
    if(h->policy==NF18A5_RAW_ALL){
        if(h->raw_retained>=NF18A5_RAW_RETAIN)return false;
        h->raw_records[h->raw_retained++]=*s;
    }
    ++h->raw_samples;return true;
}
bool nf18a5_history_commit(Nf18a5History *h,uint32_t tick,uint32_t version,
                            uint32_t owner,const Nf18a5Sample *samples,size_t count){
    if(!valid_history(h)||!owner||!tick||!version||count>NF18A5_MAX_STREAMS||
       (count && !samples) ||(h->initialized && (owner!=h->owner || tick<=h->last_tick || version<=h->last_world_version)))return false;
    for(size_t i=0;i<count;++i){const Nf18a5Sample *s=&samples[i];
        if(s->tick!=tick || s->body_a!=owner || !s->body_b || !s->contact_id || !s->material_epoch ||
           !isfinite(s->normal_impulse)||s->normal_impulse<0.0f ||
           !isfinite(s->resting_force)||s->resting_force<0.0f ||
           !isfinite(s->approach_speed)||s->approach_speed<0.0f)return false;
        for(size_t j=0;j<i;++j)if(samples[j].contact_id==s->contact_id)return false;
    }
    Nf18a5History next=*h;
    Nf18a5Sample ordered[NF18A5_MAX_STREAMS];
    for(size_t i=0;i<count;++i)ordered[i]=samples[i];
    for(size_t i=1;i<count;++i){
        Nf18a5Sample item=ordered[i];size_t j=i;
        while(j>0u && ordered[j-1u].contact_id>item.contact_id){
            ordered[j]=ordered[j-1u];--j;
        }
        ordered[j]=item;
    }
    for(size_t i=0;i<count;++i)if(!ingest(&next,&ordered[i]))return false;
    /* Each committed tick provides the complete per-owner contact set.
       A missing contact is an end, not evidence of continuous resting support. */
    for(size_t i=0;i<NF18A5_MAX_STREAMS;++i){
        Nf18a5Stream *st=&next.streams[i];
        if(st->active && st->last_tick!=tick){
            if(!flush(&next,st))return false;
            memset(st,0,sizeof(*st));
        }
    }
    next.last_tick=tick;next.last_world_version=version;next.owner=owner;next.initialized=1u;
    *h=next;return true;
}
bool nf18a5_history_close(Nf18a5History *h,uint32_t tick,uint32_t version,
                           uint32_t owner,uint32_t contact){
    if(!valid_history(h)||!contact||!h->initialized||owner!=h->owner ||
       tick<=h->last_tick||version<=h->last_world_version)return false;
    Nf18a5History next=*h;Nf18a5Stream *s=find_stream(&next,contact);
    if(!s || !flush(&next,s))return false;
    memset(s,0,sizeof(*s));next.last_tick=tick;next.last_world_version=version;
    *h=next;return true;
}
bool nf18a5_ack(Nf18a5History *h,uint64_t through){
    if(!h || !through || !h->outbox_count || through<=h->ack_sequence)return false;
    unsigned removed=0u;
    for(unsigned i=0;i<h->outbox_count;++i){
        if(h->outbox[i].sequence==through){removed=i+1u;break;}
        if(h->outbox[i].sequence>through)break;
    }
    if(!removed || h->outbox[0].sequence!=h->ack_sequence+1u)return false;
    memmove(h->outbox,h->outbox+removed,(h->outbox_count-removed)*sizeof(h->outbox[0]));
    h->outbox_count-=removed;h->ack_sequence=through;return true;
}
bool nf18a5_receipt_sample(const Nf18a4Receipt *r,uint32_t a,uint32_t b,
                            float resting_force,uint32_t material_epoch,Nf18a5Sample *out){
    if(!r||!out||!a||!b||!r->tick||!r->world_version||!r->contact_id||
       r->status!=NF18A4_ACCEPTED||!r->impulses_applied||
       !isfinite(resting_force)||resting_force<0.0f||!material_epoch)return false;
    NfVec3 n=r->normal_impulse;
    const float impulse=sqrtf(n.x*n.x+n.y*n.y+n.z*n.z);
    if(!isfinite(impulse))return false;
    *out=(Nf18a5Sample){r->tick,r->contact_id,a,b,impulse,resting_force,0.0f,material_epoch};return true;
}
void nf18a5_world_init(Nf18a5World *w,uint8_t capacity){
    if(!w)return;
    memset(w,0,sizeof(*w));nf18a5_grid_init(&w->grid,capacity,NF18A5_HYSTERETIC_FINE);
    nf18a5_history_init(&w->history,NF18A5_CUMULATIVE_DURABLE);w->revision=1u;
}
bool nf18a5_world_commit(Nf18a5World *w,uint32_t expected,uint32_t tick,
                          uint32_t owner,const Nf18a5Sample *samples,size_t count){
    if(!w||expected!=w->revision||expected==UINT32_MAX||tick<=w->tick)return false;
    for(size_t i=0;i<count;++i)if(samples && samples[i].material_epoch!=w->grid.global_revision)return false;
    Nf18a5History temp=w->history;
    if(!nf18a5_history_commit(&temp,tick,expected+1u,owner,samples,count))return false;
    w->history=temp;w->tick=tick;w->revision=expected+1u;return true;
}
bool nf18a5_world_material_commit(Nf18a5World *w,uint32_t expected,uint32_t tick,
                                  uint32_t owner,int32_t cx,int32_t cy,int32_t cz,
                                  const uint8_t canonical[NF18A5_CANON_VOXELS],
                                  const Nf18a5Sample *samples,size_t count){
    if(!w || !canonical || expected!=w->revision || expected==UINT32_MAX ||
       tick<=w->tick || (count>0u && !samples))return false;
    /* Stage new material revision and history in one copy. An event outbox
       failure rejects the proposed material update as well. */
    Nf18a5World next=*w;
    if(!nf18a5_load_canonical(&next.grid,cx,cy,cz,canonical,tick))return false;
    for(size_t i=0;i<count;++i)
        if(samples[i].material_epoch!=next.grid.global_revision)return false;
    if(!nf18a5_history_commit(&next.history,tick,expected+1u,owner,samples,count))return false;
    next.tick=tick;next.revision=expected+1u;
    *w=next;return true;
}
uint32_t nf18a5_history_hash(const Nf18a5History *h){
    if(!h)return 0u;
    uint32_t x=m32(h->last_tick^h->last_world_version^h->raw_samples^h->outbox_count);
    for(unsigned i=0;i<h->summary_count;++i)x=m32(x^h->summaries[i].digest^h->summaries[i].count);
    for(unsigned i=0;i<h->outbox_count;++i){
        const Nf18a5Event *e=&h->outbox[i];x=m32(x^e->contact_id^e->digest^(uint32_t)e->sequence^e->reason_mask);
    }
    return x;
}
