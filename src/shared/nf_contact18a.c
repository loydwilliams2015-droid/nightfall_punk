#include "nf_contact18a.h"
#include <float.h>
#include <math.h>
#include <string.h>

static NfVec3 add(NfVec3 a, NfVec3 b) { return (NfVec3){a.x+b.x,a.y+b.y,a.z+b.z}; }
static NfVec3 sub(NfVec3 a, NfVec3 b) { return (NfVec3){a.x-b.x,a.y-b.y,a.z-b.z}; }
static NfVec3 mul(NfVec3 a,float k) { return (NfVec3){a.x*k,a.y*k,a.z*k}; }
static float dot(NfVec3 a,NfVec3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
static float len(NfVec3 a) { return sqrtf(dot(a,a)); }
static bool finite3(NfVec3 a) { return isfinite(a.x)&&isfinite(a.y)&&isfinite(a.z); }
static uint32_t mix(uint32_t x) {
    x^=x>>16; x*=0x7feb352du; x^=x>>15; x*=0x846ca68bu; x^=x>>16; return x;
}

Nf18aConfig nf18a_default_config(void) {
    return (Nf18aConfig){4u,NF18A_MAX_CONTACTS,0u,0.001f,0.00005f,0.70710678f,1.0f};
}

/* A 3D AABB shape cast using Minkowski expansion of the obstacle.
   This is a swept upright BOX approximation, not a true capsule or convex solver. */
bool nf18a_sweep_aabb(NfVec3 feet, Nf18aShape shape, NfVec3 displacement,
                      Nf18aCollider c, float dt, Nf18aContact *out) {
    if (out == NULL || !(shape.radius > 0.0f && shape.height > 0.0f) ||
        dt <= 0.0f || !finite3(feet) || !finite3(displacement) ||
        !finite3(c.min) || !finite3(c.max) || !finite3(c.velocity)) return false;
    memset(out,0,sizeof(*out));
    if (c.max.x < c.min.x || c.max.y < c.min.y || c.max.z < c.min.z) return false;

    const NfVec3 center = {feet.x,feet.y+shape.height*0.5f,feet.z};
    const NfVec3 half = {shape.radius,shape.height*0.5f,shape.radius};
    const NfVec3 low = sub(c.min,half), high = add(c.max,half);
    const NfVec3 motion = sub(displacement,mul(c.velocity,dt));
    const float p[3] = {center.x,center.y,center.z};
    const float d[3] = {motion.x,motion.y,motion.z};
    const float mn[3] = {low.x,low.y,low.z};
    const float mx[3] = {high.x,high.y,high.z};
    bool inside = true;
    for (int a=0;a<3;++a) {
        if (!(p[a] > mn[a] && p[a] < mx[a])) inside=false;
    }
    if (inside) {
        out->kind=NF18A_CONTACT_INITIAL_OVERLAP;
        out->body_b=c.body_id;
        out->toi=0.0f;
        return true;
    }
    float enter=0.0f, exit=1.0f;
    int enter_axis=-1, sign=0;
    for(int axis=0;axis<3;++axis) {
        if (fabsf(d[axis])<1.0e-9f) {
            if (p[axis] < mn[axis] || p[axis] > mx[axis]) return false;
            continue;
        }
        const float inv=1.0f/d[axis];
        const float ta=(mn[axis]-p[axis])*inv;
        const float tb=(mx[axis]-p[axis])*inv;
        const float near_t=fminf(ta,tb), far_t=fmaxf(ta,tb);
        if (near_t>=enter && near_t<=exit) {enter=near_t; enter_axis=axis; sign=d[axis]>0.0f?-1:1;}
        if (far_t<exit) exit=far_t;
        if (enter>exit) return false;
    }
    if (enter_axis<0 || enter>1.0f || exit<0.0f) return false;
    NfVec3 normal={0};
    if (enter_axis==0) normal.x=(float)sign;
    else if (enter_axis==1) normal.y=(float)sign;
    else normal.z=(float)sign;
    out->kind=NF18A_CONTACT_TOUCH;
    out->body_b=c.body_id;
    out->normal=normal;
    out->toi=enter;
    out->point=add(center,mul(displacement,enter));
    out->point=add(out->point,mul(normal,-(enter_axis==1 ? half.y : half.x)));
    out->material_channel=c.material_channel;
    out->approach_speed=fmaxf(0.0f,-dot(mul(motion,1.0f/dt),normal));
    return true;
}

static bool swept_candidate(NfVec3 feet, Nf18aShape sh, NfVec3 delta,
                            Nf18aCollider c, float dt, float skin) {
    const NfVec3 delta_c=mul(c.velocity,dt);
    const NfVec3 a0={fminf(feet.x,feet.x+delta.x)-sh.radius-skin,
                     fminf(feet.y,feet.y+delta.y)-skin,
                     fminf(feet.z,feet.z+delta.z)-sh.radius-skin};
    const NfVec3 a1={fmaxf(feet.x,feet.x+delta.x)+sh.radius+skin,
                     fmaxf(feet.y,feet.y+delta.y)+sh.height+skin,
                     fmaxf(feet.z,feet.z+delta.z)+sh.radius+skin};
    const NfVec3 b0={fminf(c.min.x,c.min.x+delta_c.x),
                     fminf(c.min.y,c.min.y+delta_c.y),fminf(c.min.z,c.min.z+delta_c.z)};
    const NfVec3 b1={fmaxf(c.max.x,c.max.x+delta_c.x),
                     fmaxf(c.max.y,c.max.y+delta_c.y),fmaxf(c.max.z,c.max.z+delta_c.z)};
    return a0.x<=b1.x && a1.x>=b0.x && a0.y<=b1.y && a1.y>=b0.y && a0.z<=b1.z && a1.z>=b0.z;
}

Nf18aSolveResult nf18a_solve(uint32_t tick,uint32_t body_id,NfVec3 feet,NfVec3 velocity,
                              Nf18aShape shape,float dt,const Nf18aCollider *colliders,
                              size_t collider_count,Nf18aConfig cfg) {
    Nf18aSolveResult r={0};
    r.feet=feet; r.velocity=velocity;
    if (!finite3(feet)||!finite3(velocity)||!(shape.radius>0&&shape.height>0&&shape.mass>0)||
        !(dt>0&&isfinite(dt))||(collider_count>0&&colliders==NULL)||
        cfg.iteration_budget==0||cfg.max_contacts==0||cfg.max_contacts>NF18A_MAX_CONTACTS||
        !(cfg.skin>=0&&cfg.simultaneous_toi_epsilon>=0)) {
        r.status=NF18A_INVALID_INPUT; return r;
    }
    r.intended_distance=len(mul(velocity,dt));
    NfVec3 position=feet, motion=mul(velocity,dt), v=velocity;
    NfVec3 planes[NF18A_MAX_PLANES]={0};
    unsigned plane_count=0;
    bool any=false;
    for(unsigned iter=0;iter<cfg.iteration_budget;++iter) {
        r.iterations=(uint8_t)(iter+1u);
        if (len(motion)<1.0e-7f) break;
        float best=FLT_MAX;
        Nf18aContact hits[NF18A_MAX_CONTACTS];
        size_t count=0;
        bool manifold_overflow=false;
        for(size_t k=0;k<collider_count;++k) {
            if (colliders[k].body_id==body_id) continue;
            if(!swept_candidate(position,shape,motion,colliders[k],dt,cfg.skin)) continue;
            if (r.broadphase_candidates<UINT16_MAX) ++r.broadphase_candidates;
            Nf18aContact hit;
            if (r.narrowphase_tests<UINT16_MAX) ++r.narrowphase_tests;
            if (!nf18a_sweep_aabb(position,shape,motion,colliders[k],dt,&hit)) continue;
            if (hit.kind==NF18A_CONTACT_INITIAL_OVERLAP) {
                r.status=NF18A_INVALID_START; r.feet=position; r.velocity=(NfVec3){0}; return r;
            }
            if (hit.toi < best-cfg.simultaneous_toi_epsilon) {
                best=hit.toi;count=0;manifold_overflow=false;
            }
            if (fabsf(hit.toi-best)<=cfg.simultaneous_toi_epsilon) {
                if(count>=NF18A_MAX_CONTACTS) {
                    manifold_overflow=true;
                    continue;
                }
                hit.body_a=body_id;
                hit.tick=tick;
                hit.contact_id=mix(body_id*0x9e3779b9u ^ hit.body_b);
                hit.walkable_support=hit.normal.y>=cfg.walkable_normal_y?1u:0u;
                hits[count++]=hit;
            }
        }
        if (count==0) {position=add(position,motion); motion=(NfVec3){0};break;}
        if(manifold_overflow) {
            /* Never treat unrepresented simultaneous constraints as free space. */
            r.status=NF18A_PENDING_BUDGET;
            r.feet=position;r.velocity=(NfVec3){0};
            r.realized_distance=len(sub(position,feet));return r;
        }
        any=true;
        const float fraction=fmaxf(0.0f,best-1.0e-6f);
        position=add(position,mul(motion,fraction));
        motion=mul(motion,1.0f-fraction);
        /* Stable collider-ID ordering of the same-time manifold. */
        for(size_t i=1;i<count;++i) {
            Nf18aContact t=hits[i];size_t j=i;
            while(j>0&&hits[j-1].body_b>t.body_b) {hits[j]=hits[j-1];--j;}
            hits[j]=t;
        }
        for(size_t i=0;i<count;++i) {
            Nf18aContact hit=hits[i];
            if (r.contact_count>=cfg.max_contacts) {
                r.status=NF18A_PENDING_BUDGET;
                r.feet=position;r.velocity=(NfVec3){0};r.realized_distance=len(sub(position,feet));return r;
            }
            const float into=dot(motion,hit.normal);
            if (into<0.0f) motion=sub(motion,mul(hit.normal,into));
            const float vn=dot(v,hit.normal);
            if (vn<0.0f) {
                hit.normal_impulse=-vn*shape.mass;
                v=sub(v,mul(hit.normal,vn));
            }
            hit.consequential=hit.normal_impulse>=cfg.hit_impulse_threshold ? 1u : 0u;
            r.support|=hit.walkable_support;
            r.contacts[r.contact_count++]=hit;
            bool new_plane=true;
            for(unsigned p=0;p<plane_count;++p)
                if(dot(planes[p],hit.normal)>0.999f) new_plane=false;
            if(new_plane&&plane_count<NF18A_MAX_PLANES) planes[plane_count++]=hit.normal;
        }
        /* Re-project on all accumulated contact planes; avoid corner leaks. */
        for(unsigned pass=0;pass<plane_count;++pass)
            for(unsigned p=0;p<plane_count;++p) {
                const float into=dot(motion,planes[p]);
                if(into<0.0f) motion=sub(motion,mul(planes[p],into));
            }
        if (len(motion)<1.0e-7f) break;
        if(iter+1u==cfg.iteration_budget) {
            r.status=NF18A_PENDING_BUDGET;motion=(NfVec3){0};
        }
    }
    r.feet=position;r.velocity=v;r.realized_distance=len(sub(position,feet));
    if(r.status!=NF18A_PENDING_BUDGET) r.status=any?NF18A_BLOCKED:NF18A_SOLVED;
    return r;
}

void nf18a_history_init(Nf18aHistory *h) {if(h) memset(h,0,sizeof(*h));}
static void append_event(Nf18aHistory *h,uint32_t tick,uint32_t id,Nf18aHistoryKind kind,float impulse) {
    Nf18aHistoryEvent e={tick,id,(uint8_t)kind,{0},impulse};
    if(h->event_count==NF18A_MAX_HISTORY) {
        const Nf18aHistoryEvent *old=&h->events[h->head];
        h->cold_hash=mix(h->cold_hash ^ old->contact_id ^ mix(old->tick+old->kind));
        ++h->cold_events;
    } else ++h->event_count;
    h->events[h->head]=e;
    h->head=(uint16_t)((h->head+1u)%NF18A_MAX_HISTORY);
}
void nf18a_history_record(Nf18aHistory *h,const Nf18aSolveResult *r,uint32_t tick) {
    if(!h||!r) return;
    uint32_t current[NF18A_MAX_CONTACTS];size_t count=0;
    for(unsigned i=0;i<r->contact_count;++i) {
        const Nf18aContact *c=&r->contacts[i];
        bool duplicate=false;
        for(size_t j=0;j<count;++j) if(current[j]==c->contact_id) duplicate=true;
        if(duplicate) continue;
        current[count++]=c->contact_id;
        bool was=false;
        for(unsigned j=0;j<h->active_count;++j) if(h->active[j]==c->contact_id) was=true;
        append_event(h,tick,c->contact_id,was?NF18A_EVENT_PERSIST:NF18A_EVENT_BEGIN,c->normal_impulse);
        if(c->consequential) append_event(h,tick,c->contact_id,NF18A_EVENT_HIT,c->normal_impulse);
    }
    for(unsigned i=0;i<h->active_count;++i) {
        bool live=false;
        for(size_t j=0;j<count;++j) if(current[j]==h->active[i]) live=true;
        if(!live) append_event(h,tick,h->active[i],NF18A_EVENT_END,0.0f);
    }
    h->active_count=(uint8_t)count;
    for(size_t i=0;i<count;++i)h->active[i]=current[i];
}
static uint32_t quant(float x) {
    if(!isfinite(x)) return 0xffffffffu;
    const double v=round((double)x*100000.0);
    if(v>2147483647.0) return 0x7fffffffu;
    if(v<-2147483648.0) return 0x80000000u;
    return (uint32_t)(int32_t)v;
}
uint32_t nf18a_result_hash(const Nf18aSolveResult *r) {
    if(!r)return 0;
    uint32_t h=mix((uint32_t)r->status ^ ((uint32_t)r->contact_count<<8));
    const float nums[]={r->feet.x,r->feet.y,r->feet.z,r->velocity.x,r->velocity.y,r->velocity.z};
    for(size_t i=0;i<sizeof(nums)/sizeof(nums[0]);++i)h=mix(h^quant(nums[i]));
    for(unsigned i=0;i<r->contact_count;++i) {
        const Nf18aContact *c=&r->contacts[i];
        h=mix(h^c->contact_id^quant(c->toi)^quant(c->normal_impulse));
    }
    return h;
}
const char *nf18a_status_name(Nf18aSolveStatus s) {
    switch(s) {
        case NF18A_SOLVED: return "SOLVED";
        case NF18A_BLOCKED: return "BLOCKED";
        case NF18A_PENDING_BUDGET: return "PENDING_BUDGET";
        case NF18A_INVALID_START: return "INVALID_START";
        case NF18A_INVALID_INPUT: return "INVALID_INPUT";
        default: return "UNKNOWN";
    }
}

/* No gameplay world mutation. Return SIZE_MAX on insufficient capacity to prevent
   silent omission of authoritative physical obstacles. Ramps need a later adapter. */
size_t nf18a_extract_world_colliders(const NfWorld *world, Nf18aCollider *out, size_t capacity) {
    if(!world || !out) return SIZE_MAX;
    size_t needed=0u;
    for(size_t i=0;i<world->collider_count;++i)
        if(world->colliders[i].kind!=NF_COLLIDER_LADDER) ++needed;
    if(needed>capacity) return SIZE_MAX;
    size_t count=0u;
    for(size_t i=0;i<world->collider_count;++i) {
        const NfCollider *c=&world->colliders[i];
        if(c->kind==NF_COLLIDER_LADDER)continue;
        Nf18aCollider *d=&out[count++];
        memset(d,0,sizeof(*d));
        d->body_id=(uint32_t)i+1u;
        d->min=c->min; d->max=c->max;d->velocity=c->velocity;
        d->dynamic_body=c->kind==NF_COLLIDER_MOVING_PLATFORM?1u:0u;
        d->material_channel=1u;
    }
    return count;
}

/* Per-owner, monotone commit marker. This does not commit world state itself: the
   caller must prove that the transaction reached the authoritative commit phase. */
bool nf18a_history_commit(Nf18aHistory *history, const Nf18aSolveResult *solve,
                          uint32_t tick, uint32_t state_version, uint32_t owner_body_id) {
    if (!history || !solve || state_version==0u || owner_body_id==0u) return false;
    if(solve->status==NF18A_INVALID_INPUT || solve->status==NF18A_INVALID_START) return false;
    if(history->has_committed) {
        if(history->owner_body_id!=owner_body_id || tick<=history->last_commit_tick ||
           state_version<=history->last_state_version) return false;
    }
    nf18a_history_record(history,solve,tick);
    history->owner_body_id=owner_body_id;
    history->last_commit_tick=tick;
    history->last_state_version=state_version;
    history->has_committed=1u;
    return true;
}
