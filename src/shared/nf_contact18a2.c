#include "nf_contact18a2.h"

#include <math.h>
#include <string.h>
#include <limits.h>

static float clampf(float x, float a, float b) { return x < a ? a : (x > b ? b : x); }
static NfVec3 sub3(NfVec3 a, NfVec3 b) { return (NfVec3){a.x-b.x,a.y-b.y,a.z-b.z}; }
static float dot3(NfVec3 a, NfVec3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
static uint32_t mix32(uint32_t x) {
    x ^= x>>16; x *= 0x7feb352du; x ^= x>>15; x *= 0x846ca68bu; x ^= x>>16; return x;
}
static uint32_t quantf(float v) {
    if (!isfinite(v)) return UINT32_MAX;
    double q = round((double)v*1000.0);
    if (q > (double)INT32_MAX) return INT32_MAX;
    if (q < (double)INT32_MIN) return (uint32_t)INT32_MIN;
    return (uint32_t)(int32_t)q;
}

bool nf18a2_profile_valid(Nf18a2ShapePolicy shape) {
    return isfinite(shape.radius) && shape.radius > 0.0f &&
        isfinite(shape.height) && shape.height >= shape.radius*2.0f &&
        isfinite(shape.foot_flat_radius) && shape.foot_flat_radius >= 0.0f &&
        shape.foot_flat_radius <= shape.radius &&
        isfinite(shape.step_height) && shape.step_height >= 0.0f &&
        isfinite(shape.walkable_normal_y) && shape.walkable_normal_y >= 0.0f &&
        shape.walkable_normal_y <= 1.0f;
}
Nf18a2ContactProfile nf18a2_choose_profile(Nf18a2ShapePolicy shape,
                                            Nf18a2ShapeRequest request) {
    if (!nf18a2_profile_valid(shape)) return NF18A2_PROFILE_CAPSULE;
    if (!request.material_support || !request.clearance_valid ||
        !request.affordance_authorized) return NF18A2_PROFILE_CAPSULE;
    if (request.ladder_candidate) return NF18A2_PROFILE_ROUNDED_BOX_LADDER;
    if (request.stair_candidate) return NF18A2_PROFILE_ROUNDED_BOX_SUPPORT;
    return NF18A2_PROFILE_CAPSULE;
}
float nf18a2_capsule_box_separation(NfVec3 feet, Nf18a2ShapePolicy shape,
                                    const Nf18aCollider *box) {
    if (!box || !nf18a2_profile_valid(shape) ||
        !isfinite(feet.x) || !isfinite(feet.y) || !isfinite(feet.z)) return NAN;
    /* Vertical centerline segment [feet.y+r, feet.y+h-r]. Its closest distance
       to an AABB is the hypotenuse of XY horizontal offset and vertical gap. */
    const float dx=feet.x-clampf(feet.x,box->min.x,box->max.x);
    const float dz=feet.z-clampf(feet.z,box->min.z,box->max.z);
    const float bottom=feet.y+shape.radius;
    const float top=feet.y+shape.height-shape.radius;
    float dy=0.0f;
    if (top < box->min.y) dy=box->min.y-top;
    else if (bottom > box->max.y) dy=bottom-box->max.y;
    return sqrtf(dx*dx+dy*dy+dz*dz)-shape.radius;
}

bool nf18a2_motor_step(Nf18a2RigidBody *body, Nf18a2Motor motor,
                        float dt, bool grounded) {
    if (!body || body->id==0u || !(isfinite(body->mass)&&body->mass>0.0f) ||
        !(isfinite(dt)&&dt>0.0f) ||
        !isfinite(body->velocity.x)||!isfinite(body->velocity.y)||!isfinite(body->velocity.z)||
        !isfinite(motor.target_x)||!isfinite(motor.target_z)||
        !(isfinite(motor.max_accel)&&motor.max_accel>=0.0f) ||
        !(isfinite(motor.max_force)&&motor.max_force>=0.0f) ||
        !(isfinite(motor.gravity)&&motor.gravity>=0.0f) ||
        !(isfinite(motor.damping)&&motor.damping>=0.0f)) return false;
    const float dx=motor.target_x-body->velocity.x;
    const float dz=motor.target_z-body->velocity.z;
    const float dist=sqrtf(dx*dx+dz*dz);
    const float accel_cap=fminf(motor.max_accel,motor.max_force/body->mass);
    const float dv_cap=accel_cap*dt;
    const float fraction=(dist>dv_cap && dist>0.0f) ? dv_cap/dist : 1.0f;
    body->velocity.x+=dx*fraction;
    body->velocity.z+=dz*fraction;
    if (!grounded) body->velocity.y-=motor.gravity*dt;
    /* Optional environmental drag. Input motor remains a declared energy source. */
    const float drag=1.0f/(1.0f+motor.damping*dt);
    body->velocity.x*=drag;
    body->velocity.z*=drag;
    return isfinite(body->velocity.x)&&isfinite(body->velocity.y)&&isfinite(body->velocity.z);
}
bool nf18a2_exchange_normal_impulse(Nf18a2RigidBody *a,
                                    Nf18a2RigidBody *b,
                                    NfVec3 n, float restitution, float *impulse) {
    if (!a||!b||!impulse||a->id==b->id||a->id==0u||b->id==0u||
        !(isfinite(a->mass)&&a->mass>0.0f)||!(isfinite(b->mass)&&b->mass>0.0f)||
        !(isfinite(restitution)&&restitution>=0.0f&&restitution<=1.0f)||
        !isfinite(n.x)||!isfinite(n.y)||!isfinite(n.z)||
        !isfinite(a->velocity.x)||!isfinite(a->velocity.y)||!isfinite(a->velocity.z)||
        !isfinite(b->velocity.x)||!isfinite(b->velocity.y)||!isfinite(b->velocity.z)) return false;
    const float nn=dot3(n,n);
    if (fabsf(nn-1.0f)>1.0e-3f) return false;
    const float rel=dot3(sub3(a->velocity,b->velocity),n);
    const float j=rel<0.0f?-(1.0f+restitution)*rel/(1.0f/a->mass+1.0f/b->mass):0.0f;
    a->velocity.x+=n.x*j/a->mass; a->velocity.y+=n.y*j/a->mass; a->velocity.z+=n.z*j/a->mass;
    b->velocity.x-=n.x*j/b->mass; b->velocity.y-=n.y*j/b->mass; b->velocity.z-=n.z*j/b->mass;
    *impulse=j;
    return true;
}

Nf18a2AdaptiveConfig nf18a2_default_adaptive(void) { return (Nf18a2AdaptiveConfig){2u,2u,6u}; }
Nf18a2AdaptiveResult nf18a2_solve_adaptive(
    uint32_t tick, uint32_t body_id, NfVec3 feet, NfVec3 velocity,
    Nf18aShape shape, float dt,
    const Nf18aCollider *colliders, size_t count,
    Nf18aConfig collision, Nf18a2AdaptiveConfig budget) {
    Nf18a2AdaptiveResult a={0};
    if (budget.cheap_iterations==0u || budget.hard_max_iterations<budget.cheap_iterations ||
        budget.reserve_iterations==0u || budget.hard_max_iterations>32u) {
        a.solve.status=NF18A_INVALID_INPUT;
        return a;
    }
    uint8_t iterations=budget.cheap_iterations;
    for (;;) {
        collision.iteration_budget=iterations;
        ++a.attempts;
        a.solve=nf18a_solve(tick,body_id,feet,velocity,shape,dt,colliders,count,collision);
        a.final_budget=iterations;
        if (a.solve.status!=NF18A_PENDING_BUDGET || iterations==budget.hard_max_iterations) break;
        const unsigned more=(unsigned)iterations+budget.reserve_iterations;
        iterations=(uint8_t)(more>budget.hard_max_iterations?budget.hard_max_iterations:more);
        a.reserve_used=1u;
    }
    return a;
}

void nf18a2_chunk_cache_init(Nf18a2ChunkCache *c,uint8_t capacity) {
    if (!c) return;
    memset(c,0,sizeof(*c));
    c->capacity=capacity>NF18A2_MAX_CHUNKS?NF18A2_MAX_CHUNKS:capacity;
    c->global_epoch=1u;
}
Nf18a2Chunk *nf18a2_chunk_touch(Nf18a2ChunkCache *c,
                                  int32_t x,int32_t y,int32_t z,
                                  Nf18a2ChunkResolution requested,uint32_t tick) {
    if (!c || requested<NF18A2_CANONICAL_1M || requested>NF18A2_REFINED_0_25M ||
        c->capacity==0u) return NULL;
    for(uint8_t i=0u;i<c->count;++i) {
        Nf18a2Chunk *p=&c->chunks[i];
        if(p->x!=x||p->y!=y||p->z!=z)continue;
        if(tick<p->last_active_tick)return NULL;
        if(requested!=NF18A2_CANONICAL_1M &&
           (!p->fine_resident || p->resolution<(uint8_t)requested))return NULL;
        p->last_active_tick=tick;
        return p;
    }
    if(c->count>=c->capacity)return NULL;
    Nf18a2Chunk *p=&c->chunks[c->count++];
    memset(p,0,sizeof(*p));
    p->x=x;p->y=y;p->z=z;p->version=1u;p->last_active_tick=tick;
    if(requested!=NF18A2_CANONICAL_1M){--c->count;return NULL;}
    return p;
}
bool nf18a2_chunk_load_canonical(Nf18a2ChunkCache *c,
                                   int32_t x,int32_t y,int32_t z,
                                   const uint8_t data[NF18A2_CANONICAL_CHUNK_CELLS],uint32_t tick) {
    if(!data)return false;
    Nf18a2Chunk *p=nf18a2_chunk_touch(c,x,y,z,NF18A2_CANONICAL_1M,tick);
    if(!p)return false;
    memcpy(p->canonical,data,NF18A2_CANONICAL_CHUNK_CELLS);
    p->canonical_resident=1u;
    p->fine_resident=0u;
    p->resolution=(uint8_t)NF18A2_CANONICAL_1M;
    ++p->version;
    return true;
}
bool nf18a2_chunk_load_fine(Nf18a2ChunkCache *c,
                              int32_t x,int32_t y,int32_t z,
                              Nf18a2ChunkResolution resolution,
                              const uint8_t *data,size_t count,uint32_t tick) {
    if(!data || (resolution!=NF18A2_REFINED_0_5M && resolution!=NF18A2_REFINED_0_25M))return false;
    const size_t expected=resolution==NF18A2_REFINED_0_5M?64u:256u;
    if(count!=expected)return false;
    Nf18a2Chunk *p=nf18a2_chunk_touch(c,x,y,z,NF18A2_CANONICAL_1M,tick);
    if(!p || !p->canonical_resident)return false;
    memcpy(p->fine,data,count);
    p->fine_resident=1u;
    p->resolution=(uint8_t)resolution;
    ++p->version;
    return true;
}
bool nf18a2_chunk_evict_idle(Nf18a2ChunkCache *c,uint32_t tick,uint32_t min_idle_ticks) {
    if(!c)return false;
    size_t candidate=c->count;
    for(size_t i=0u;i<c->count;++i) {
        Nf18a2Chunk *p=&c->chunks[i];
        if(p->pinned || tick<p->last_active_tick || tick-p->last_active_tick<min_idle_ticks)continue;
        if(candidate==c->count || p->last_active_tick<c->chunks[candidate].last_active_tick ||
           (p->last_active_tick==c->chunks[candidate].last_active_tick &&
            (p->x<c->chunks[candidate].x ||
            (p->x==c->chunks[candidate].x && p->y<c->chunks[candidate].y)))) candidate=i;
    }
    if(candidate==c->count)return false;
    /* Sort remaining entries by shifting, preserving deterministic index order. */
    for(size_t j=candidate+1u;j<c->count;++j)c->chunks[j-1u]=c->chunks[j];
    --c->count;
    ++c->global_epoch;
    return true;
}
int32_t nf18a2_chunk_coord(float v,float extent) {
    if(!isfinite(v)||!isfinite(extent)||extent<=0.0f)return INT32_MIN;
    const double n=floor((double)v/extent);
    if(n>INT32_MAX||n<INT32_MIN)return INT32_MIN;
    return (int32_t)n;
}

Nf18a2HistoryPolicy nf18a2_default_history_policy(void) {
    return (Nf18a2HistoryPolicy){15u,60u,20.0f,80.0f};
}
void nf18a2_condensed_history_init(Nf18a2CondensedHistory *h,Nf18a2HistoryPolicy p) {
    if(!h)return;
    memset(h,0,sizeof(*h));
    h->policy=p;
}
static bool policy_valid(Nf18a2HistoryPolicy p) {
    return p.summary_ticks>0u&&p.duration_ticks>=p.summary_ticks &&
        isfinite(p.peak_impulse)&&p.peak_impulse>0.0f &&
        isfinite(p.accumulated_impulse)&&p.accumulated_impulse>0.0f;
}
static void append_summary(Nf18a2CondensedHistory *h,Nf18a2ContactSummary s) {
    const uint32_t idx=h->summary_head;
    if(h->summary_count==NF18A2_SUMMARY_RING) {
        const Nf18a2ContactSummary old=h->summaries[idx];
        h->evicted_summary_digest=mix32(h->evicted_summary_digest ^ old.digest ^ old.samples);
        ++h->summary_evictions;
    } else ++h->summary_count;
    h->summaries[idx]=s;
    h->summary_head=(idx+1u)%NF18A2_SUMMARY_RING;
}
static void append_event(Nf18a2CondensedHistory *h,Nf18a2PersistentEvent e) {
    const uint32_t idx=h->event_head;
    if(h->event_count==NF18A2_EVENT_RING) {
        h->evicted_event_digest=mix32(h->evicted_event_digest ^ h->events[idx].digest);
        ++h->event_evictions;
    } else ++h->event_count;
    h->events[idx]=e;
    h->event_head=(idx+1u)%NF18A2_EVENT_RING;
}
static void flush_stream(Nf18a2CondensedHistory *h,Nf18a2HistoryStream *s) {
    if(s->window_samples==0u)return;
    Nf18a2ContactSummary summary={s->contact_id,s->window_start,s->last_tick,s->window_samples,
                                   s->window_sum,s->window_peak,s->window_speed_peak,s->digest};
    append_summary(h,summary);
    const uint8_t reason=(s->epoch_peak>=h->policy.peak_impulse?NF18A2_THRESHOLD_PEAK:0u) |
        (s->epoch_sum>=h->policy.accumulated_impulse?NF18A2_THRESHOLD_ACCUMULATED:0u) |
        (s->epoch_samples>=h->policy.duration_ticks?NF18A2_THRESHOLD_DURATION:0u);
    if(reason!=0u) {
        Nf18a2PersistentEvent e={s->contact_id,s->epoch_start,s->last_tick,s->epoch_samples,s->digest,
                                 reason,{0},s->epoch_sum,s->epoch_peak};
        append_event(h,e);
        /* Begin a new threshold-accumulation period; no double-counted samples. */
        s->epoch_start=s->last_tick+1u;
        s->epoch_samples=0u;
        s->epoch_sum=0.0f;
        s->epoch_peak=0.0f;
        ++s->events_promoted;
    }
    s->window_start=s->last_tick+1u;
    s->window_samples=0u;
    s->window_sum=0.0f;
    s->window_peak=0.0f;
    s->window_speed_peak=0.0f;
    s->digest=0u;
}
static bool advance_internal(Nf18a2CondensedHistory *h,uint32_t tick) {
    if(!h || !policy_valid(h->policy) || (h->initialized && tick<h->last_tick)) return false;
    for(size_t i=0u;i<NF18A2_MAX_CONTACT_STREAMS;++i) {
        Nf18a2HistoryStream *s=&h->streams[i];
        if(!s->active || s->window_samples==0u)continue;
        if(tick-s->window_start>=h->policy.summary_ticks)flush_stream(h,s);
    }
    h->last_tick=tick;
    return true;
}
bool nf18a2_history_advance(Nf18a2CondensedHistory *h,uint32_t tick) {
    return advance_internal(h,tick);
}
bool nf18a2_history_commit_samples(Nf18a2CondensedHistory *h,
                                    uint32_t tick,uint32_t version,uint32_t owner,
                                    const Nf18aContact *cs,size_t count) {
    if(!h || !policy_valid(h->policy) || owner==0u || version==0u ||
        (count>0u&&cs==NULL) || count>NF18A2_MAX_CONTACT_STREAMS ||
        (h->initialized && (h->owner_id!=owner||version<=h->last_version||tick<=h->last_tick)))return false;
    /* Prevalidate all input, including stream capacity, before any side effects. */
    uint32_t unique[NF18A2_MAX_CONTACT_STREAMS];size_t unique_count=0u;
    size_t missing=0u;
    for(size_t i=0u;i<count;++i) {
        if(cs[i].contact_id==0u || cs[i].tick!=tick ||
           !isfinite(cs[i].normal_impulse)||cs[i].normal_impulse<0.0f ||
           !isfinite(cs[i].approach_speed)||cs[i].approach_speed<0.0f ||
           cs[i].body_a!=owner) return false;
        for(size_t j=0u;j<unique_count;++j)if(unique[j]==cs[i].contact_id)return false;
        unique[unique_count++]=cs[i].contact_id;
        bool found=false;
        for(size_t j=0u;j<NF18A2_MAX_CONTACT_STREAMS;++j)
            if(h->streams[j].active && h->streams[j].contact_id==cs[i].contact_id)found=true;
        if(!found)++missing;
    }
    size_t free_slots=0u;
    for(size_t j=0u;j<NF18A2_MAX_CONTACT_STREAMS;++j)if(!h->streams[j].active)++free_slots;
    if(missing>free_slots)return false;
    if(!advance_internal(h,tick))return false;
    for(size_t i=0u;i<count;++i) {
        const Nf18aContact *c=&cs[i];Nf18a2HistoryStream *s=NULL;
        for(size_t j=0u;j<NF18A2_MAX_CONTACT_STREAMS;++j)
            if(h->streams[j].active&&h->streams[j].contact_id==c->contact_id){s=&h->streams[j];break;}
        if(!s)for(size_t j=0u;j<NF18A2_MAX_CONTACT_STREAMS;++j)
            if(!h->streams[j].active){s=&h->streams[j];break;}
        if(!s)return false; /* unreachable after prevalidation */
        if(!s->active) {
            memset(s,0,sizeof(*s));s->contact_id=c->contact_id;
            s->window_start=tick;s->epoch_start=tick;s->active=1u;
        } else if(tick>s->last_tick && tick-s->last_tick>1u) {
            /* Missing contact samples are not evidence of continuous support. */
            flush_stream(h,s);
            s->epoch_start=tick;s->epoch_samples=0u;
            s->epoch_sum=0.0f;s->epoch_peak=0.0f;
            s->window_start=tick;
        }
        s->last_tick=tick;
        ++s->window_samples;++s->epoch_samples;
        s->window_sum+=c->normal_impulse;s->epoch_sum+=c->normal_impulse;
        s->window_peak=fmaxf(s->window_peak,c->normal_impulse);
        s->epoch_peak=fmaxf(s->epoch_peak,c->normal_impulse);
        s->window_speed_peak=fmaxf(s->window_speed_peak,c->approach_speed);
        s->digest=mix32(s->digest^mix32(quantf(c->normal_impulse)^c->contact_id^tick));
    }
    h->last_tick=tick;h->last_version=version;h->owner_id=owner;h->initialized=1u;
    return true;
}
bool nf18a2_history_end_contact(Nf18a2CondensedHistory *h,uint32_t contact_id,uint32_t tick,
                                 uint32_t version,uint32_t owner_id) {
    if(!h||!h->initialized||owner_id!=h->owner_id||version<=h->last_version||
       tick<=h->last_tick)return false;
    for(size_t i=0u;i<NF18A2_MAX_CONTACT_STREAMS;++i) {
        Nf18a2HistoryStream *s=&h->streams[i];
        if(s->active&&s->contact_id==contact_id) {
            flush_stream(h,s);
            /* A subthreshold stream closes into the bounded short summary and
               cold digest only; no fictitious consequential event. */
            memset(s,0,sizeof(*s));
            h->last_tick=tick;h->last_version=version;
            return true;
        }
    }
    return false;
}
