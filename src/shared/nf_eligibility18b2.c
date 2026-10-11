#include "nf_eligibility18b2.h"
#include <string.h>
#include <math.h>

static uint32_t mix(uint32_t h,uint32_t v) {
    h ^= v; h *= UINT32_C(16777619); return h;
}
Nf18b2Witness nf18b2_adjudicate(Nf18a5World *material,
        Nf18a3Geometry geometry, Nf18a3Contract contract,
        Nf18a3Request request, NfVec3 probe) {
    Nf18b2Witness w;
    memset(&w,0,sizeof(w));
    w.status=NF18B2_INVALID;
    if (!material || !isfinite(probe.x) || !isfinite(probe.y) || !isfinite(probe.z)
        || request.actor_id==0 || request.feature_id==0 ||
        geometry.solid_count>NF_MAX_COLLIDERS || geometry.ladder_count>NF_MAX_COLLIDERS) return w;
    w.tick=geometry.tick;
    w.world_revision=material->revision;
    w.material_epoch=material->grid.global_revision;
    w.actor_id=request.actor_id;
    w.feature_id=request.feature_id;
    if (geometry.world_version != material->revision ||
        geometry.tick < material->tick ||
        geometry.world_version != contract.world_version ||
        geometry.tick != contract.tick ||
        request.actor_id != contract.actor_id ||
        request.feature_id != contract.feature_id) {
        w.status=NF18B2_STALE; return w;
    }
    /* Never resolve pending canonical/fine data to positive clearance.
       No provider is trusted implicitly; streaming must occur beforehand. */
    const Nf18a5Query q=nf18a5_query(&material->grid,probe.x,probe.y,probe.z,
                                      geometry.tick,true);
    if (q==NF18A5_Q_PENDING) { w.status=NF18B2_PENDING; return w; }
    if (q==NF18A5_Q_INVALID) return w;
    if (q==NF18A5_Q_SOLID) { w.status=NF18B2_BLOCKED; return w; }
    /* Reject deliberately unsafe scientific negative-control policies. */
    if (request.policy==NF18A3_CAPSULE_EASY_NEGATIVE ||
        request.policy==NF18A3_HYBRID_AUTH_BYPASS_NEGATIVE) return w;
    w.traversal=nf18a3_adjudicate(geometry,contract,request);
    w.status=w.traversal.commit_eligible ? NF18B2_ELIGIBLE : NF18B2_BLOCKED;
    if(w.status==NF18B2_ELIGIBLE){
        NfVec3 points[4]={request.feet,request.destination,{0,0,0},{0,0,0}};
        size_t segments=1;
        if(request.transition==NF18A3_STEP){
            points[1]=(NfVec3){request.feet.x,request.destination.y+.001f,request.feet.z};
            points[2]=(NfVec3){request.destination.x,points[1].y,request.destination.z};
            points[3]=request.destination;segments=3;
        }
        const Nf18a2ShapePolicy shape={request.body_radius,request.body_height,
            request.foot_flat_radius,request.max_step,.70710678f};
        for(size_t i=0;i<segments;++i){
            const NfVec3 delta={points[i+1].x-points[i].x,
                points[i+1].y-points[i].y,points[i+1].z-points[i].z};
            Nf18a5Query route=nf18a5_sweep_query(&material->grid,points[i],delta,
                shape,1,geometry.tick,NULL,NULL,NULL);
            if(route!=NF18A5_Q_FREE){
                w.status=route==NF18A5_Q_SOLID?NF18B2_BLOCKED:
                    route==NF18A5_Q_PENDING?NF18B2_PENDING:NF18B2_INVALID;
                w.traversal.commit_eligible=0;break;
            }
        }
    }
    uint32_t hash=UINT32_C(2166136261);
    hash=mix(hash,w.world_revision); hash=mix(hash,w.material_epoch);
    hash=mix(hash,w.tick); hash=mix(hash,w.actor_id);
    hash=mix(hash,w.feature_id);
    hash=mix(hash,nf18a3_decision_hash(&w.traversal));
    w.witness_hash=hash;
    return w;
}
Nf18b2Status nf18b2_publish(Nf18b2Authority *authority,
        const Nf18b2Witness *w, Nf18b2Publish publisher,void *context) {
    if (!authority || !w || !publisher) return NF18B2_INVALID;
    if (w->status!=NF18B2_ELIGIBLE || !w->traversal.commit_eligible
        || !w->witness_hash) return w->status==NF18B2_ELIGIBLE
                               ? NF18B2_INVALID : w->status;
    (void)context;
    /* Legacy unbound publication cannot validate a claimed witness. */
    return NF18B2_INVALID;
}
Nf18b2Status nf18b2_publish_checked(Nf18b2Authority *authority,
    const Nf18b2Witness *w,Nf18a5World *material,Nf18a3Geometry geometry,
    Nf18a3Contract contract,Nf18a3Request request,NfVec3 probe,
    Nf18b2Publish publisher,void *context){
    if(!authority||!w||!publisher||authority->receipt_count>64)return NF18B2_INVALID;
    if(authority->publishing)return NF18B2_PENDING;
    Nf18b2Witness fresh=nf18b2_adjudicate(material,geometry,contract,request,probe);
    if(fresh.status!=NF18B2_ELIGIBLE)return fresh.status;
    if(w->status!=NF18B2_ELIGIBLE||w->tick!=fresh.tick||
       w->actor_id!=fresh.actor_id||w->feature_id!=fresh.feature_id||
       w->world_revision!=fresh.world_revision||w->material_epoch!=fresh.material_epoch||
       w->witness_hash!=fresh.witness_hash)return NF18B2_STALE;
    for(unsigned i=0;i<authority->receipt_count;++i)
        if(authority->ticks[i]==fresh.tick&&authority->actors[i]==fresh.actor_id&&
           authority->features[i]==fresh.feature_id)return NF18B2_DUPLICATE;
    if(authority->receipt_count==64)return NF18B2_PENDING;
    authority->publishing=1;
    const bool published=publisher(context,&fresh);
    authority->publishing=0;
    if(!published)return NF18B2_PUBLISH_FAILED;
    const unsigned index=authority->receipt_count++;
    authority->ticks[index]=fresh.tick;authority->actors[index]=fresh.actor_id;
    authority->features[index]=fresh.feature_id;
    authority->committed_tick=fresh.tick;
    authority->committed_actor=fresh.actor_id;
    authority->committed_feature=fresh.feature_id;
    authority->committed_hash=fresh.witness_hash;
    authority->has_commit=1;
    return NF18B2_ELIGIBLE;
}

static bool finite_vec(NfVec3 p){return isfinite(p.x)&&isfinite(p.y)&&isfinite(p.z);}
static Nf18b2UseDecision decision(Nf18b2Status status,Nf18b2Reason reason){
    Nf18b2UseDecision d={0};d.status=status;d.reason=reason;
    d.physical=status;d.contract=status;return d;
}
static bool contains(const uint32_t *members,unsigned count,uint32_t id){
    for(unsigned i=0;i<count;++i)if(members[i]==id)return true;
    return false;
}
static const Nf18b2Actor *actor_for(const Nf18b2Scene *s,uint32_t id){
    for(size_t i=0;i<s->actor_count;++i)if(s->actors[i].id==id)return &s->actors[i];
    return NULL;
}
static const Nf18b2Object *object_for(const Nf18b2Scene *s,uint32_t id){
    for(size_t i=0;i<s->object_count;++i)if(s->objects[i].id==id)return &s->objects[i];
    return NULL;
}
static bool valid_scene(const Nf18b2Scene *s){
    if(!s||!s->material||!s->geometry.tick||!s->geometry.world_version||
       s->actor_count>NF18B2_MAX_REQUESTS||s->object_count>NF18B2_MAX_REQUESTS||
       (s->actor_count&&!s->actors)||(s->object_count&&!s->objects)||
       s->geometry.solid_count>NF_MAX_COLLIDERS||
       (s->geometry.solid_count&&!s->geometry.solids))return false;
    for(size_t i=0;i<s->actor_count;++i){
        if(!s->actors[i].id||!s->actors[i].revision||
           s->actors[i].active>1||s->actors[i].grounded>1)return false;
        for(size_t j=0;j<i;++j)if(s->actors[i].id==s->actors[j].id)return false;
    }
    for(size_t i=0;i<s->object_count;++i){
        const Nf18b2Object *o=&s->objects[i];
        if(!o->id||!o->revision||!o->cell_id||!o->nexus_id||!o->slot_id||
           o->preconditions>1||o->requires_support>1||
           !o->capacity||o->capacity>NF18B2_MAX_MEMBERS||
           o->occupant_count>o->capacity||o->member_count>NF18B2_MAX_MEMBERS||
           !finite_vec(o->target)||!isfinite(o->max_reach)||o->max_reach<0)return false;
        for(size_t j=0;j<i;++j)if(o->id==s->objects[j].id)return false;
        for(unsigned j=0;j<o->occupant_count;++j){
            if(!o->occupants[j])return false;
            for(unsigned k=0;k<j;++k)if(o->occupants[j]==o->occupants[k])return false;
        }
        for(unsigned j=0;j<o->member_count;++j){
            if(!o->members[j])return false;
            for(unsigned k=0;k<j;++k)if(o->members[j]==o->members[k])return false;
        }
    }
    for(size_t i=0;i<s->geometry.solid_count;++i){
        const Nf18aCollider *c=&s->geometry.solids[i];
        if(!c->body_id||!finite_vec(c->min)||!finite_vec(c->max)||!finite_vec(c->velocity)||
           c->min.x>c->max.x||c->min.y>c->max.y||c->min.z>c->max.z)return false;
        for(size_t j=0;j<i;++j)if(c->body_id==s->geometry.solids[j].body_id)return false;
    }
    return true;
}
static bool line_hits(NfVec3 start,NfVec3 end,const Nf18aCollider *c){
    double near=0,far=1;
    const double a[3]={start.x,start.y,start.z},b[3]={end.x,end.y,end.z};
    const double lo[3]={c->min.x,c->min.y,c->min.z},hi[3]={c->max.x,c->max.y,c->max.z};
    for(unsigned i=0;i<3;++i){
        const double delta=b[i]-a[i];
        if(fabs(delta)<1e-12){if(a[i]<lo[i]||a[i]>hi[i])return false;}
        else{
            double t=(lo[i]-a[i])/delta,u=(hi[i]-a[i])/delta;
            if(t>u){const double v=t;t=u;u=v;}
            if(t>near)near=t;
            if(u<far)far=u;
            if(near>far)return false;
        }
    }
    /* A surface hit at the target endpoint does not hide its own handle. */
    return near<1.0-1e-6&&far>=0;
}
static Nf18b2Reason contract_reason(const Nf18b2Actor *a,const Nf18b2Object *o){
    if((a->credentials&o->required_credentials)!=o->required_credentials)return NF18B2_CREDENTIAL;
    if((a->inventory&o->required_inventory)!=o->required_inventory)return NF18B2_INVENTORY;
    if(!o->preconditions)return NF18B2_PRECONDITION;
    if(o->member_count&&!contains(o->members,o->member_count,a->id))return NF18B2_MEMBERSHIP;
    if(o->occupant_count>=o->capacity&&!contains(o->occupants,o->occupant_count,a->id))return NF18B2_CAPACITY;
    return NF18B2_OK;
}
Nf18b2UseDecision nf18b2_evaluate(const Nf18b2Scene *s,Nf18b2Use use,Nf18b2Model model){
    if(!valid_scene(s)||!use.action_id||
       (model!=NF18B2_M3_EXHAUSTIVE&&model!=NF18B2_M4_MATERIAL_FIRST))
        return decision(NF18B2_INVALID,NF18B2_BAD_INPUT);
    const Nf18b2Actor *a=actor_for(s,use.actor_id);
    const Nf18b2Object *o=object_for(s,use.object_id);
    if(!a||!o)return decision(NF18B2_INVALID,NF18B2_BAD_INPUT);
    const Nf18a2ShapePolicy shape={a->radius,a->height,0,0,1};
    if(!finite_vec(a->feet)||!nf18a2_profile_valid(shape)||
       !isfinite(a->eye_height)||a->eye_height<0||a->eye_height>a->height)
        return decision(NF18B2_INVALID,NF18B2_BAD_INPUT);
    if(use.tick!=s->geometry.tick||use.world_revision!=s->material->revision||
       s->geometry.world_version!=s->material->revision||use.tick<s->material->tick||
       use.material_epoch!=s->material->grid.global_revision||
       use.actor_revision!=a->revision||use.object_revision!=o->revision)
        return decision(NF18B2_STALE,NF18B2_FRAME_CHANGED);
    Nf18b2UseDecision d={0};
    const Nf18b2Reason cr=contract_reason(a,o);
    d.contract=cr==NF18B2_OK?NF18B2_ELIGIBLE:NF18B2_BLOCKED;
    const NfVec3 eye={a->feet.x,a->feet.y+a->eye_height,a->feet.z};
    const NfVec3 delta={o->target.x-eye.x,o->target.y-eye.y,o->target.z-eye.z};
    const double distance=(double)delta.x*delta.x+(double)delta.y*delta.y+(double)delta.z*delta.z;
    Nf18b2Reason pr=NF18B2_OK;
    if(!a->active)pr=NF18B2_POSE;
    else if(distance>(double)o->max_reach*o->max_reach)pr=NF18B2_REACH;
    else if(o->requires_support){
        bool support=false;
        for(size_t i=0;i<s->geometry.solid_count;++i){
            const Nf18aCollider *c=&s->geometry.solids[i];
            if(c->body_id==a->support_id&&!c->dynamic_body&&a->grounded&&
               fabsf(a->feet.y-c->max.y)<.0001f&&
               a->feet.x-a->radius>=c->min.x&&a->feet.x+a->radius<=c->max.x&&
               a->feet.z-a->radius>=c->min.z&&a->feet.z+a->radius<=c->max.z)support=true;
        }
        if(!support)pr=NF18B2_SUPPORT;
    }
    if(pr==NF18B2_OK||model==NF18B2_M3_EXHAUSTIVE){
        ++d.geometry_queries;
        const bool body_clear=nf18a3_capsule_clear(s->geometry,a->feet,shape);
        bool visible=true;
        for(size_t i=0;i<s->geometry.solid_count;++i){
            ++d.geometry_queries;
            if(line_hits(eye,o->target,&s->geometry.solids[i]))visible=false;
        }
        ++d.material_queries;
        const Nf18a5Query body=nf18a5_sweep_query(&s->material->grid,a->feet,
            (NfVec3){0,0,0},shape,1,use.tick,NULL,NULL,NULL);
        ++d.material_queries;
        /* Finite-radius conservative visibility witness, not a visual ray. */
        const Nf18a2ShapePolicy ray={.0001f,.0002f,0,0,1};
        const NfVec3 rayfeet={eye.x,eye.y-.0001f,eye.z};
        const Nf18a5Query line=nf18a5_sweep_query(&s->material->grid,rayfeet,
            delta,ray,1,use.tick,NULL,NULL,NULL);
        if(pr==NF18B2_OK){
            if(!body_clear||body==NF18A5_Q_SOLID)pr=NF18B2_BODY_BLOCKED;
            else if(!visible||line==NF18A5_Q_SOLID)pr=NF18B2_OCCLUDED;
            else if(body==NF18A5_Q_INVALID||line==NF18A5_Q_INVALID)pr=NF18B2_BAD_INPUT;
            else if(body==NF18A5_Q_PENDING||line==NF18A5_Q_PENDING)pr=NF18B2_MATERIAL_PENDING;
        }
    }
    d.physical=pr==NF18B2_OK?NF18B2_ELIGIBLE:pr==NF18B2_MATERIAL_PENDING?
        NF18B2_PENDING:pr==NF18B2_BAD_INPUT?NF18B2_INVALID:NF18B2_BLOCKED;
    if(d.physical==NF18B2_INVALID){d.status=d.physical;d.reason=pr;}
    else if(d.physical==NF18B2_BLOCKED){d.status=d.physical;d.reason=pr;}
    else if(d.contract==NF18B2_BLOCKED){d.status=d.contract;d.reason=cr;}
    else {d.status=d.physical;d.reason=pr;}
    return d;
}
bool nf18b2_resolve(const Nf18b2Scene *s,const Nf18b2Use *uses,size_t count,
    Nf18b2Model model,Nf18b2UseDecision *out){
    if(!uses||!out||count>NF18B2_MAX_REQUESTS||!valid_scene(s))return false;
    size_t order[NF18B2_MAX_REQUESTS];
    for(size_t i=0;i<count;++i){out[i]=nf18b2_evaluate(s,uses[i],model);order[i]=i;}
    for(size_t i=0;i<count;++i)for(size_t j=0;j<i;++j)
        if(uses[i].actor_id==uses[j].actor_id&&uses[i].action_id==uses[j].action_id&&
           uses[i].object_id!=uses[j].object_id){
            out[i]=decision(NF18B2_INVALID,NF18B2_BAD_INPUT);
            out[j]=decision(NF18B2_INVALID,NF18B2_BAD_INPUT);
        }
    for(size_t i=1;i<count;++i){
        const size_t id=order[i];size_t j=i;
        while(j&& (uses[id].actor_id<uses[order[j-1]].actor_id||
            (uses[id].actor_id==uses[order[j-1]].actor_id&&
             uses[id].action_id<uses[order[j-1]].action_id))){order[j]=order[j-1];--j;}
        order[j]=id;
    }
    for(size_t k=0;k<count;++k){
        const size_t i=order[k];
        if(out[i].status!=NF18B2_ELIGIBLE)continue;
        const Nf18b2Object *o=object_for(s,uses[i].object_id);
        unsigned occupied=o->occupant_count;
        bool duplicate=false;
        for(size_t j=0;j<k;++j){
            const size_t prev=order[j];
            if(out[prev].status!=NF18B2_ELIGIBLE||uses[prev].object_id!=uses[i].object_id)continue;
            if(uses[prev].actor_id==uses[i].actor_id)duplicate=true;
            else if(!contains(o->occupants,o->occupant_count,uses[prev].actor_id))++occupied;
        }
        if(duplicate){out[i].status=NF18B2_DUPLICATE;out[i].reason=NF18B2_REPLAY;}
        else if(!contains(o->occupants,o->occupant_count,uses[i].actor_id)&&occupied>=o->capacity){
            out[i].status=NF18B2_BLOCKED;out[i].contract=NF18B2_BLOCKED;out[i].reason=NF18B2_CAPACITY;
        }
    }
    return true;
}
const char *nf18b2_reason_name(Nf18b2Reason r){
    static const char *const names[]={"OK","BAD_INPUT","FRAME_CHANGED","POSE","REACH",
        "BODY_BLOCKED","OCCLUDED","MATERIAL_PENDING","SUPPORT","CREDENTIAL","INVENTORY",
        "PRECONDITION","MEMBERSHIP","CAPACITY","REPLAY"};
    return r>=NF18B2_OK&&r<=NF18B2_REPLAY?names[r]:"UNKNOWN";
}
