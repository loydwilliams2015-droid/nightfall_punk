#include "nf_contact18a3.h"

#include <float.h>
#include <math.h>
#include <string.h>

static NfVec3 sub3(NfVec3 a,NfVec3 b){return (NfVec3){a.x-b.x,a.y-b.y,a.z-b.z};}
static NfVec3 mul3(NfVec3 v,float s){return (NfVec3){v.x*s,v.y*s,v.z*s};}
static float dot3(NfVec3 a,NfVec3 b){return a.x*b.x+a.y*b.y+a.z*b.z;}
static float clampf3(float x,float a,float b){return fmaxf(a,fminf(b,x));}
static bool finite3(NfVec3 a){return isfinite(a.x)&&isfinite(a.y)&&isfinite(a.z);}
static uint32_t mix32(uint32_t x){x^=x>>16;x*=0x7feb352du;x^=x>>15;x*=0x846ca68bu;x^=x>>16;return x;}

static float dist2_at(NfVec3 feet,Nf18a2ShapePolicy sh,NfVec3 delta,
                      const Nf18aCollider *c,double t,NfVec3 *dout,NfVec3 *point) {
    const double x=(double)feet.x+(double)delta.x*t;
    const double y=(double)feet.y+(double)delta.y*t;
    const double z=(double)feet.z+(double)delta.z*t;
    const double dx=x-(x<c->min.x?c->min.x:(x>c->max.x?c->max.x:x));
    const double dz=z-(z<c->min.z?c->min.z:(z>c->max.z?c->max.z:z));
    const double bot=y+(double)sh.radius;
    const double top=y+(double)sh.height-(double)sh.radius;
    const double dy=bot>c->max.y?bot-c->max.y:(top<c->min.y?top-c->min.y:0.0);
    if(dout) *dout=(NfVec3){(float)dx,(float)dy,(float)dz};
    if(point){
        const double cy=dy>0?bot:(dy<0?top:clampf3((float)(0.5*(bot+top)),c->min.y,c->max.y));
        *point=(NfVec3){(float)(x-dx),(float)(cy-dy),(float)(z-dz)};
    }
    return (float)(dx*dx+dy*dy+dz*dz);
}

static void breakpoint(double *times,size_t *n,double start,double v,double edge) {
    if(fabs(v)<1.0e-12)return;
    const double t=(edge-start)/v;
    if(t>0.0 && t<1.0 && *n<12u)times[(*n)++]=t;
}
static void sort_unique(double *times,size_t *count) {
    for(size_t i=1;i<*count;++i) {
        const double t=times[i];size_t j=i;
        while(j>0 && times[j-1]>t){times[j]=times[j-1];--j;}
        times[j]=t;
    }
    size_t n=0;
    for(size_t i=0;i<*count;++i) if(n==0u || times[i]-times[n-1]>1.0e-12)times[n++]=times[i];
    *count=n;
}

bool nf18a3_capsule_sweep(NfVec3 feet,Nf18a2ShapePolicy sh,
                          NfVec3 displacement,Nf18aCollider c,float dt,
                          Nf18aContact *out) {
    if(!out || !nf18a2_profile_valid(sh) || !finite3(feet) ||
       !finite3(displacement) || !finite3(c.min) || !finite3(c.max) ||
       !finite3(c.velocity) || !(isfinite(dt)&&dt>0.0f) ||
       c.min.x>c.max.x || c.min.y>c.max.y || c.min.z>c.max.z)return false;
    memset(out,0,sizeof(*out));
    NfVec3 d=sub3(displacement,mul3(c.velocity,dt));
    const double r2=(double)sh.radius*(double)sh.radius;
    const double initial=(double)dist2_at(feet,sh,d,&c,0.0,NULL,NULL);
    const double overlap_eps=fmax(1.0e-9,r2*1.0e-6);
    if(initial<r2-overlap_eps) {
        out->kind=NF18A_CONTACT_INITIAL_OVERLAP;
        out->body_b=c.body_id;out->toi=0.0f;return true;
    }
    double times[12]={0.0,1.0};size_t n=2;
    breakpoint(times,&n,feet.x,d.x,c.min.x);
    breakpoint(times,&n,feet.x,d.x,c.max.x);
    breakpoint(times,&n,feet.z,d.z,c.min.z);
    breakpoint(times,&n,feet.z,d.z,c.max.z);
    breakpoint(times,&n,feet.y+sh.radius,d.y,c.max.y);
    breakpoint(times,&n,feet.y+sh.height-sh.radius,d.y,c.min.y);
    sort_unique(times,&n);
    double best=2.0;
    for(size_t i=0;i+1<n;++i) {
        const double a=times[i],b=times[i+1],width=b-a;
        if(width<=0.0)continue;
        const double mid=(a+b)*0.5;
        const double x=feet.x+(double)d.x*mid;
        const double z=feet.z+(double)d.z*mid;
        const double bot=feet.y+sh.radius+(double)d.y*mid;
        const double top=feet.y+sh.height-sh.radius+(double)d.y*mid;
        const double xref=x<c.min.x?c.min.x:(x>c.max.x?c.max.x:x);
        const double zref=z<c.min.z?c.min.z:(z>c.max.z?c.max.z:z);
        const double yref=bot>c.max.y?c.max.y:(top<c.min.y?c.min.y:bot);
        const bool xoutside=x<c.min.x || x>c.max.x;
        const bool zoutside=z<c.min.z || z>c.max.z;
        const bool yabove=bot>c.max.y;
        const bool ybelow=top<c.min.y;
        const double ux=xoutside?(double)d.x:0.0;
        const double uz=zoutside?(double)d.z:0.0;
        const double uy=(yabove||ybelow)?(double)d.y:0.0;
        const double px=xoutside?(double)feet.x+(double)d.x*a-xref:0.0;
        const double pz=zoutside?(double)feet.z+(double)d.z*a-zref:0.0;
        const double py=yabove?(double)feet.y+sh.radius+(double)d.y*a-yref:
                        (ybelow?(double)feet.y+sh.height-sh.radius+(double)d.y*a-yref:0.0);
        const double A=ux*ux+uy*uy+uz*uz;
        const double B=2.0*(px*ux+py*uy+pz*uz);
        const double C=px*px+py*py+pz*pz-r2;
        double candidate=-1.0;
        if(C<=overlap_eps && B<-1.0e-12)candidate=a;
        else if(C>overlap_eps && A>1.0e-18) {
            const double disc=B*B-4.0*A*C;
            if(disc>=0.0 && B<0.0) {
                const double root=(-B-sqrt(disc))/(2.0*A);
                if(root>=-1.0e-8 && root<=width+1.0e-8) candidate=a+fmax(0.0,root);
            }
        }
        if(candidate>=0.0 && candidate<best)best=candidate;
    }
    if(best>1.0)return false;
    NfVec3 distance={0},closest={0};
    (void)dist2_at(feet,sh,d,&c,best,&distance,&closest);
    const float norm=sqrtf(dot3(distance,distance));
    NfVec3 normal={0};
    if(norm>1.0e-8f)normal=mul3(distance,1.0f/norm);
    else {
        const float adx=fabsf(d.x),ady=fabsf(d.y),adz=fabsf(d.z);
        if(adx>=ady && adx>=adz)normal.x=d.x>0?-1.0f:1.0f;
        else if(ady>=adz)normal.y=d.y>0?-1.0f:1.0f;
        else normal.z=d.z>0?-1.0f:1.0f;
    }
    if(dot3(d,normal)>=-1.0e-8f)return false;
    out->kind=NF18A_CONTACT_TOUCH;
    out->body_b=c.body_id;
    out->toi=(float)best;
    out->point=closest;
    out->normal=normal;
    out->material_channel=c.material_channel;
    out->approach_speed=fmaxf(0.0f,-dot3(d,normal)/dt);
    return true;
}

bool nf18a3_capsule_clear(Nf18a3Geometry world,NfVec3 feet,
                          Nf18a2ShapePolicy sh) {
    if(!nf18a2_profile_valid(sh)||!finite3(feet)||
       (world.solid_count>0u&&!world.solids))return false;
    for(size_t k=0;k<world.solid_count;++k) {
        const Nf18aCollider *box=&world.solids[k];
        const float sep=nf18a2_capsule_box_separation(feet,sh,box);
        if(!isfinite(sep)||sep<-0.0001f)return false;
    }
    return true;
}

static bool box_clear(Nf18a3Geometry world,NfVec3 p,Nf18a2ShapePolicy sh) {
    for(size_t i=0;i<world.solid_count;++i) {
        const Nf18aCollider *c=&world.solids[i];
        if(p.x+sh.radius>c->min.x+0.0001f && p.x-sh.radius<c->max.x-0.0001f &&
           p.z+sh.radius>c->min.z+0.0001f && p.z-sh.radius<c->max.z-0.0001f &&
           p.y+sh.height>c->min.y+0.0001f && p.y<c->max.y-0.0001f)
            return false;
    }
    return true;
}
static bool move_clear(Nf18a3Geometry world,NfVec3 a,NfVec3 b,
                       Nf18a2ShapePolicy sh,Nf18a3Policy policy,
                       uint16_t *queries,uint16_t *hits) {
    if(policy==NF18A3_BOX_BASELINE) {
        if(!box_clear(world,a,sh)||!box_clear(world,b,sh))return false;
    } else if(!nf18a3_capsule_clear(world,a,sh)||
              !nf18a3_capsule_clear(world,b,sh))return false;
    const NfVec3 delta=sub3(b,a);
    if(dot3(delta,delta)<1.0e-12f)return true;
    for(size_t i=0;i<world.solid_count;++i) {
        if(*queries<UINT16_MAX)++*queries;
        Nf18aContact hit={0};
        const bool found=policy==NF18A3_BOX_BASELINE ?
          nf18a_sweep_aabb(a,(Nf18aShape){sh.radius,sh.height,80.0f},delta,world.solids[i],1.0f,&hit) :
          nf18a3_capsule_sweep(a,sh,delta,world.solids[i],1.0f,&hit);
        if(found) {
            if(*hits<UINT16_MAX)++*hits;
            if(hit.kind==NF18A_CONTACT_INITIAL_OVERLAP)return false;
            if(hit.toi<0.9999f)return false;
        }
    }
    return true;
}

static const Nf18aCollider *find_solid(Nf18a3Geometry world,uint32_t id) {
    for(size_t i=0;i<world.solid_count;++i)
        if(world.solids[i].body_id==id)return &world.solids[i];
    return NULL;
}
static const Nf18aCollider *find_feature(Nf18a3Geometry world,uint32_t id,
                                          Nf18a3Transition transition) {
    if(transition==NF18A3_STEP)return find_solid(world,id);
    for(size_t i=0;i<world.ladder_count;++i)
        if(world.ladders[i].body_id==id)return &world.ladders[i];
    return NULL;
}

static bool projected_support(const Nf18aCollider *box,NfVec3 p,float rad,float width) {
    if(!box || !(width>=0.0f))return false;
    /* Require an entire minimal stable foot patch inside the support polygon.
       A flat-foot proxy does NOT change the primary collision volume. */
    const float minx=box->min.x+width, maxx=box->max.x-width;
    const float minz=box->min.z+width, maxz=box->max.z-width;
    if(maxx<minx || maxz<minz || rad<width)return false;
    return p.x>=minx-0.0001f && p.x<=maxx+0.0001f &&
           p.z>=minz-0.0001f && p.z<=maxz+0.0001f;
}

static bool params_valid(Nf18a3Request r,Nf18a3Geometry w) {
    return r.actor_id>0u && r.feature_id>0u &&
       finite3(r.feet)&&finite3(r.destination)&&finite3(r.facing)&&
       isfinite(r.body_radius)&&r.body_radius>0.0f&&
       isfinite(r.body_height)&&r.body_height>=r.body_radius*2.0f&&
       isfinite(r.foot_flat_radius)&&r.foot_flat_radius>=0.0f&&
       r.foot_flat_radius<=r.body_radius&&
       isfinite(r.max_step)&&r.max_step>=0.0f&&
       isfinite(r.max_ladder_reach)&&r.max_ladder_reach>=0.0f&&
       isfinite(r.min_facing_cosine)&&r.min_facing_cosine>=-1.0f&&r.min_facing_cosine<=1.0f&&
       isfinite(r.min_landing_width)&&r.min_landing_width>=0.0f&&
       (w.solid_count==0u || w.solids!=NULL) &&
       (w.ladder_count==0u || w.ladders!=NULL);
}

static Nf18a3Reason geometry_check(Nf18a3Geometry world,Nf18a3Request r,
                                   Nf18a3Decision *o) {
    const Nf18aCollider *feature=find_feature(world,r.feature_id,r.transition);
    if(!feature)return NF18A3_WRONG_FEATURE;
    const Nf18a2ShapePolicy sh={r.body_radius,r.body_height,r.foot_flat_radius,r.max_step,0.70710678f};
    if(r.transition==NF18A3_STEP) {
        if(!r.grounded)return NF18A3_UNSUPPORTED;
        const float step=feature->max.y-r.feet.y;
        o->measured_step_height=step;
        if(step<=0.0001f || step>r.max_step+1.0e-5f)return NF18A3_TOO_HIGH;
        const float dx=r.destination.x-r.feet.x;
        const float dz=r.destination.z-r.feet.z;
        if(dx*dx+dz*dz<1.0e-8f)return NF18A3_TOO_FAR;
        const float md=sqrtf(dx*dx+dz*dz), fd=sqrtf(r.facing.x*r.facing.x+r.facing.z*r.facing.z);
        if(fd<1.0e-5f || (dx*r.facing.x+dz*r.facing.z)/(md*fd)<r.min_facing_cosine)
            return NF18A3_BAD_ALIGNMENT;
        if(fabsf(r.destination.y-feature->max.y)>0.0005f)return NF18A3_NO_LANDING;
        const float patch=r.policy==NF18A3_HYBRID_DUAL || r.policy==NF18A3_HYBRID_AUTH_BYPASS_NEGATIVE
            ? fmaxf(r.foot_flat_radius,r.min_landing_width)
            : r.min_landing_width;
        if(!projected_support(feature,r.destination,r.body_radius,patch))return NF18A3_NO_LANDING;
        if(feature->dynamic_body && r.policy!=NF18A3_CAPSULE_EASY_NEGATIVE)return NF18A3_MOVING_SUPPORT;
        const float gap=0.001f;
        const NfVec3 raised={r.feet.x,feature->max.y+gap,r.feet.z};
        const NfVec3 traverse={r.destination.x,raised.y,r.destination.z};
        const NfVec3 land={r.destination.x,feature->max.y,r.destination.z};
        if(!move_clear(world,r.feet,raised,sh,r.policy,&o->shape_queries,&o->swept_contacts) ||
           !move_clear(world,raised,traverse,sh,r.policy,&o->shape_queries,&o->swept_contacts) ||
           !move_clear(world,traverse,land,sh,r.policy,&o->shape_queries,&o->swept_contacts))
            return NF18A3_NO_CLEARANCE;
        o->used_flat_support=r.policy==NF18A3_HYBRID_DUAL || r.policy==NF18A3_HYBRID_AUTH_BYPASS_NEGATIVE;
        return NF18A3_OK;
    }
    if(r.transition==NF18A3_LADDER_ATTACH) {
        if(r.ladder_attached)return NF18A3_INVALID;
        const float cx=clampf3(r.feet.x,feature->min.x,feature->max.x);
        const float cz=clampf3(r.feet.z,feature->min.z,feature->max.z);
        const float dx=cx-r.feet.x, dz=cz-r.feet.z;
        const float d=sqrtf(dx*dx+dz*dz);
        if(d>r.max_ladder_reach || d<0.00001f)return NF18A3_TOO_FAR;
        const float facing=sqrtf(r.facing.x*r.facing.x+r.facing.z*r.facing.z);
        if(facing<1.0e-5f || (dx*r.facing.x+dz*r.facing.z)/(d*facing)<r.min_facing_cosine)
            return NF18A3_BAD_ALIGNMENT;
        if(r.feet.y+r.body_height<feature->min.y || r.feet.y>feature->max.y)return NF18A3_TOO_FAR;
        if(!move_clear(world,r.feet,r.destination,sh,r.policy,&o->shape_queries,&o->swept_contacts))
            return NF18A3_NO_CLEARANCE;
        return NF18A3_OK;
    }
    if(r.transition==NF18A3_LADDER_EXIT) {
        if(!r.ladder_attached)return NF18A3_INVALID;
        /* An exit must have a distinct stable LANDING surface; the ladder
           trigger is not a load-bearing support. */
        bool landing=false;
        for(size_t i=0;i<world.solid_count;++i) {
            const Nf18aCollider *c=&world.solids[i];
            if(c->body_id==r.feature_id)continue;
            if(c->dynamic_body)continue;
            if(fabsf(r.destination.y-c->max.y)<=0.0005f &&
               projected_support(c,r.destination,r.body_radius,
                                 fmaxf(r.foot_flat_radius,r.min_landing_width))) landing=true;
        }
        if(!landing)return NF18A3_NO_LANDING;
        if(!move_clear(world,r.feet,r.destination,sh,r.policy,&o->shape_queries,&o->swept_contacts))
            return NF18A3_NO_CLEARANCE;
        o->used_flat_support=1u;
        return NF18A3_OK;
    }
    return NF18A3_INVALID;
}

static Nf18a3Reason contract_check(Nf18a3Geometry world,Nf18a3Contract c,Nf18a3Request r) {
    if(c.world_version!=world.world_version||c.tick!=world.tick)return NF18A3_STALE_WORLD;
    if(c.actor_id!=r.actor_id)return NF18A3_WRONG_ACTOR;
    if(c.feature_id!=r.feature_id)return NF18A3_WRONG_FEATURE;
    if(!c.material_support_valid || !c.support_stable)return NF18A3_UNSUPPORTED;
    if(!isfinite(c.authoritative_body_radius) || !isfinite(c.authoritative_body_height) ||
       !isfinite(c.authoritative_foot_flat_radius) ||
       !isfinite(c.authoritative_min_facing_cosine) ||
       !isfinite(c.authoritative_min_landing_width) || !finite3(c.authoritative_feet))
        return NF18A3_INVALID;
    if(fabsf(r.body_radius-c.authoritative_body_radius)>0.00001f ||
       fabsf(r.body_height-c.authoritative_body_height)>0.00001f ||
       fabsf(r.foot_flat_radius-c.authoritative_foot_flat_radius)>0.00001f ||
       fabsf(r.feet.x-c.authoritative_feet.x)>0.0001f ||
       fabsf(r.feet.y-c.authoritative_feet.y)>0.0001f ||
       fabsf(r.feet.z-c.authoritative_feet.z)>0.0001f ||
       r.grounded!=c.actor_grounded || r.ladder_attached!=c.actor_ladder_attached)
        return NF18A3_NO_AUTHORITY;
    if(r.min_facing_cosine+1.0e-6f<c.authoritative_min_facing_cosine ||
       r.min_landing_width+1.0e-6f<c.authoritative_min_landing_width)
        return NF18A3_NO_AUTHORITY;
    if(r.transition==NF18A3_STEP) {
        const Nf18aCollider *support=find_solid(world,c.current_support_id);
        if(!support || !c.actor_grounded || support->dynamic_body ||
           fabsf(r.feet.y-support->max.y)>0.0005f ||
           !projected_support(support,r.feet,r.body_radius,r.foot_flat_radius))
            return NF18A3_UNSUPPORTED;
    }
    if(!isfinite(c.authoritative_step_limit) || c.authoritative_step_limit<0.0f ||
       !isfinite(c.authoritative_reach_limit) || c.authoritative_reach_limit<0.0f)
        return NF18A3_INVALID;
    if(r.transition==NF18A3_STEP && r.max_step>c.authoritative_step_limit+1.0e-6f)
        return NF18A3_NO_AUTHORITY;
    if(r.transition!=NF18A3_STEP && r.max_ladder_reach>c.authoritative_reach_limit+1.0e-6f)
        return NF18A3_NO_AUTHORITY;
    const Nf18aCollider *feature=find_feature(world,r.feature_id,r.transition);
    if(!feature)return NF18A3_WRONG_FEATURE;
    if(feature->dynamic_body && !c.allow_dynamic_support)return NF18A3_MOVING_SUPPORT;
    if((r.transition==NF18A3_STEP && !c.may_step)||
       (r.transition==NF18A3_LADDER_ATTACH && !c.may_attach_ladder)||
       (r.transition==NF18A3_LADDER_EXIT && !c.may_exit_ladder))return NF18A3_NO_AUTHORITY;
    return NF18A3_OK;
}

static uint32_t hash_coord(uint32_t seed,float v){
    if(!isfinite(v))return mix32(seed^0xffffffffu);
    const double scaled=round((double)v*100000.0);
    if(scaled>2147483647.0||scaled<-2147483648.0)return mix32(seed^0xfffffffeu);
    return mix32(seed^(uint32_t)(int32_t)scaled);
}
uint32_t nf18a3_decision_hash(const Nf18a3Decision *d) {
    if(!d)return 0u;
    uint32_t h=mix32((uint32_t)d->geometry_reason*17u ^
       (uint32_t)d->contract_reason*37u ^
       (uint32_t)d->commit_eligible*13u ^ d->feature_id*1009u ^
       d->actor_id*179u ^ d->world_version*673u ^ d->tick*991u ^
       d->policy*71u ^ d->transition*103u ^
       (uint32_t)d->shape_queries*727u ^ (uint32_t)d->used_flat_support*71u);
    h=hash_coord(h,d->measured_step_height);
    h=hash_coord(h,d->original_feet.x);h=hash_coord(h,d->original_feet.y);
    h=hash_coord(h,d->original_feet.z);h=hash_coord(h,d->target_feet.x);
    h=hash_coord(h,d->target_feet.y);h=hash_coord(h,d->target_feet.z);
    return h;
}

Nf18a3Decision nf18a3_adjudicate(Nf18a3Geometry world,Nf18a3Contract contract,
                                 Nf18a3Request r) {
    Nf18a3Decision o={0};o.feature_id=r.feature_id;
    o.actor_id=r.actor_id;o.world_version=world.world_version;o.tick=world.tick;
    o.policy=(uint32_t)r.policy;o.transition=(uint32_t)r.transition;
    o.original_feet=r.feet;o.target_feet=r.destination;
    /* Unsafe controls are compiled only into explicitly designated test
       binaries. Production must not let client input select an unsafe policy. */
#ifndef NF18A3_TEST_CONTROLS
    if(r.policy==NF18A3_CAPSULE_EASY_NEGATIVE ||
       r.policy==NF18A3_HYBRID_AUTH_BYPASS_NEGATIVE) {
        o.geometry_reason=NF18A3_INVALID;o.contract_reason=NF18A3_INVALID;
        o.witness_hash=nf18a3_decision_hash(&o);return o;
    }
#endif
    if(!params_valid(r,world) || r.policy<NF18A3_BOX_BASELINE ||
       r.policy>NF18A3_HYBRID_AUTH_BYPASS_NEGATIVE) {
        o.geometry_reason=NF18A3_INVALID;o.contract_reason=NF18A3_INVALID;
        o.witness_hash=nf18a3_decision_hash(&o);return o;
    }
    /* Check independently.  Geometry cannot infer semantic authorization and
       a contract cannot infer valid collision geometry. */
    o.geometry_reason=geometry_check(world,r,&o);
    o.contract_reason=contract_check(world,contract,r);
    o.geometry_approved=o.geometry_reason==NF18A3_OK;
    o.contract_approved=o.contract_reason==NF18A3_OK;
    const bool negative_bypass=r.policy==NF18A3_HYBRID_AUTH_BYPASS_NEGATIVE;
    o.commit_eligible=o.geometry_approved && (o.contract_approved || negative_bypass);
    if(r.policy==NF18A3_CAPSULE_EASY_NEGATIVE &&
       r.transition==NF18A3_STEP && o.contract_approved &&
       o.geometry_reason==NF18A3_TOO_HIGH)o.commit_eligible=1u;
    o.witness_hash=nf18a3_decision_hash(&o);
    return o;
}

const char *nf18a3_reason_name(Nf18a3Reason r) {
    static const char *const names[]={"OK","INVALID","STALE_WORLD","WRONG_ACTOR",
        "WRONG_FEATURE","NO_AUTHORITY","UNSUPPORTED","TOO_HIGH",
        "NO_CLEARANCE","TOO_FAR","BAD_ALIGNMENT","MOVING_SUPPORT","NO_LANDING"};
    return r>=NF18A3_OK && r<=NF18A3_NO_LANDING?names[(unsigned)r]:"UNKNOWN";
}
