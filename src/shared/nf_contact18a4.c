#include "nf_contact18a4.h"
#include <math.h>
#include <string.h>

static NfVec3 vadd(NfVec3 a,NfVec3 b) {return (NfVec3){a.x+b.x,a.y+b.y,a.z+b.z};}
static NfVec3 vsub(NfVec3 a,NfVec3 b) {return (NfVec3){a.x-b.x,a.y-b.y,a.z-b.z};}
static NfVec3 vmul(NfVec3 a,float s) {return (NfVec3){a.x*s,a.y*s,a.z*s};}
static float dot(NfVec3 a,NfVec3 b) {return a.x*b.x+a.y*b.y+a.z*b.z;}
static NfVec3 cross(NfVec3 a,NfVec3 b) {return (NfVec3){a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
static NfVec3 imul(NfVec3 a,NfVec3 inv) {return (NfVec3){a.x*inv.x,a.y*inv.y,a.z*inv.z};}
static float norm(NfVec3 a){return sqrtf(dot(a,a));}
static bool vfinite(NfVec3 v){return isfinite(v.x)&&isfinite(v.y)&&isfinite(v.z);}
static uint32_t mix(uint32_t x){x^=x>>16;x*=0x7feb352du;x^=x>>15;x*=0x846ca68bu;x^=x>>16;return x;}
static uint32_t q(float v){if(!isfinite(v))return UINT32_MAX;double a=round((double)v*10000.0);if(a>2147483647.0)a=2147483647.0;if(a< -2147483648.0)a= -2147483648.0;return (uint32_t)(int32_t)a;}

bool nf18a4_body_valid(const Nf18a4Body *b){
    if(!b || !b->id || !vfinite(b->center)||!vfinite(b->velocity)||!vfinite(b->omega)||
       !vfinite(b->inverse_inertia)||!isfinite(b->inverse_mass)||b->inverse_mass<0.0f||
       b->inverse_inertia.x<0||b->inverse_inertia.y<0||b->inverse_inertia.z<0 ||
       !isfinite(b->radius)||!isfinite(b->height)||!vfinite(b->box_half))return false;
    if(b->inverse_mass==0.0f && (b->inverse_inertia.x!=0.0f||b->inverse_inertia.y!=0.0f||b->inverse_inertia.z!=0.0f))return false;
    return true;
}

bool nf18a4_motor_drive(Nf18a4Body *actor,Nf18a4Motor m,float dt,Nf18a4MotorReceipt *out){
    if(!nf18a4_body_valid(actor)||actor->inverse_mass<=0.0f||!out||!vfinite(m.target_velocity)||
       !isfinite(dt)||dt<=0.0f||!isfinite(m.max_force)||m.max_force<0.0f||
       !isfinite(m.max_accel)||m.max_accel<0.0f)return false;
    const NfVec3 dv=vsub(m.target_velocity,actor->velocity);
    const float mass=1.0f/actor->inverse_mass;
    const float max_impulse=fminf(m.max_force*dt,m.max_accel*dt*mass);
    const float required=norm(dv)*mass;
    const NfVec3 j=required>max_impulse&&required>0.0f?vmul(dv,max_impulse/required*mass):vmul(dv,mass);
    const float before=dot(actor->velocity,actor->velocity);
    actor->velocity=vadd(actor->velocity,vmul(j,actor->inverse_mass));
    *out=(Nf18a4MotorReceipt){j,0.5f*mass*(dot(actor->velocity,actor->velocity)-before),norm(j)/dt};
    return vfinite(actor->velocity)&&isfinite(out->work_joules);
}

static bool cvalid(Nf18a4Constraint c){
    return c.body_a!=0u&&c.body_b!=0u&&c.body_a!=c.body_b &&
       vfinite(c.normal_b_to_a)&&vfinite(c.contact_point)&&
       fabsf(dot(c.normal_b_to_a,c.normal_b_to_a)-1.0f)<0.0003f&&
       isfinite(c.restitution)&&c.restitution>=0&&c.restitution<=1 &&
       isfinite(c.friction)&&c.friction>=0.0f;
}
static NfVec3 velocity_at(const Nf18a4Body *b,NfVec3 r,bool angular){
    return angular?vadd(b->velocity,cross(b->omega,r)):b->velocity;
}
static float inverse_effective(const Nf18a4Body *a,const Nf18a4Body *b,
                               NfVec3 ra,NfVec3 rb,NfVec3 axis,bool angular){
    float k=a->inverse_mass+b->inverse_mass;
    if(angular){
        NfVec3 ca=cross(ra,axis),cb=cross(rb,axis);
        k+=dot(cross(imul(ca,a->inverse_inertia),ra),axis);
        k+=dot(cross(imul(cb,b->inverse_inertia),rb),axis);
    }
    return k;
}
static void apply_impulse(Nf18a4Body *a,Nf18a4Body *b,NfVec3 ra,NfVec3 rb,
                          NfVec3 j,bool angular){
    a->velocity=vadd(a->velocity,vmul(j,a->inverse_mass));
    b->velocity=vsub(b->velocity,vmul(j,b->inverse_mass));
    if(angular){
        a->omega=vadd(a->omega,imul(cross(ra,j),a->inverse_inertia));
        b->omega=vsub(b->omega,imul(cross(rb,j),b->inverse_inertia));
    }
}
static float kinetic(const Nf18a4Body *b,bool angular){
    float e=b->inverse_mass>0?0.5f*dot(b->velocity,b->velocity)/b->inverse_mass:0.0f;
    if(angular){
        const float w[3]={b->omega.x,b->omega.y,b->omega.z};
        const float i[3]={b->inverse_inertia.x,b->inverse_inertia.y,b->inverse_inertia.z};
        for(int k=0;k<3;++k)if(i[k]>0.0f)e+=0.5f*w[k]*w[k]/i[k];
    }
    return e;
}

Nf18a4Receipt nf18a4_apply_contact(Nf18a4Body *a,Nf18a4Body *b,Nf18a4Constraint c,
                                    Nf18a4Policy policy,uint32_t tick,uint32_t world_version){
    Nf18a4Receipt r={0};r.status=NF18A4_INVALID;
    if(!nf18a4_body_valid(a)||!nf18a4_body_valid(b)||!cvalid(c)||
       a->id!=c.body_a||b->id!=c.body_b||a->inverse_mass+b->inverse_mass==0.0f||
       policy<NF18A4_ESTIMATE_ONLY_CONTROL||policy>NF18A4_FRICTION_ADAPTIVE ||
       !tick||!world_version)return r;
    r.tick=tick;r.world_version=world_version;r.contact_id=mix(c.body_a^mix(c.body_b^c.feature_id));
    r.policy=(uint8_t)policy;r.iterations=1u;
    const bool angular=policy>=NF18A4_ANGULAR_NORMAL;
    const float e0=kinetic(a,angular)+kinetic(b,angular);
    const NfVec3 ra=vsub(c.contact_point,a->center),rb=vsub(c.contact_point,b->center);
    NfVec3 relative=vsub(velocity_at(a,ra,angular),velocity_at(b,rb,angular));
    const float vn=dot(relative,c.normal_b_to_a);
    if(vn>=-1.0e-6f){r.status=NF18A4_SEPARATING;return r;}
    const float inv=inverse_effective(a,b,ra,rb,c.normal_b_to_a,angular);
    if(inv<=1.0e-10f)return r;
    float j=-(1.0f+c.restitution)*vn/inv;
    if(!isfinite(j))return r;
    NfVec3 jn=vmul(c.normal_b_to_a,j);
    r.normal_impulse=jn;
    if(policy==NF18A4_ESTIMATE_ONLY_CONTROL){r.status=NF18A4_ACCEPTED;return r;}
    apply_impulse(a,b,ra,rb,jn,angular);
    r.impulses_applied=1u;
    if(policy>=NF18A4_FRICTION_FIXED6 && c.friction>0.0f){
        relative=vsub(velocity_at(a,ra,true),velocity_at(b,rb,true));
        const NfVec3 tangent=vsub(relative,vmul(c.normal_b_to_a,dot(relative,c.normal_b_to_a)));
        const float tangential_speed=norm(tangent);
        if(tangential_speed>1.0e-7f){
            const NfVec3 dir=vmul(tangent,-1.0f/tangential_speed);
            const float kt=inverse_effective(a,b,ra,rb,dir,true);
            if(kt>1.0e-10f){
                const float jt=fminf(tangential_speed/kt,c.friction*j);
                r.tangent_impulse=vmul(dir,jt);
                apply_impulse(a,b,ra,rb,r.tangent_impulse,true);
            }
        }
    }
    r.signed_energy_delta=kinetic(a,angular)+kinetic(b,angular)-e0;
    r.status=NF18A4_ACCEPTED;
    return r;
}

static size_t body_index(const Nf18a4Body *b,size_t count,uint32_t id){
    for(size_t i=0;i<count;++i)if(b[i].id==id)return i;
    return count;
}
Nf18a4IslandResult nf18a4_solve_island(Nf18a4Body *b,size_t n,
                                        const Nf18a4Constraint *cs,size_t nc,
                                        Nf18a4Policy policy,uint32_t tick,uint32_t version){
    Nf18a4IslandResult o={0};o.status=NF18A4_INVALID;
    if(!b||!cs||!n||n>NF18A4_MAX_BODIES||nc>NF18A4_MAX_CONSTRAINTS ||
       !tick||!version||policy<NF18A4_ESTIMATE_ONLY_CONTROL||policy>NF18A4_FRICTION_ADAPTIVE)return o;
    for(size_t i=0;i<n;++i){if(!nf18a4_body_valid(&b[i]))return o;
        for(size_t j=0;j<i;++j)if(b[i].id==b[j].id)return o;}
    for(size_t i=0;i<nc;++i)if(!cvalid(cs[i])||body_index(b,n,cs[i].body_a)==n||
             body_index(b,n,cs[i].body_b)==n)return o;
    if(nc==0u){o.status=NF18A4_ACCEPTED;return o;}
    Nf18a4Constraint ordered[NF18A4_MAX_CONSTRAINTS];
    for(size_t i=0;i<nc;++i)ordered[i]=cs[i];
    /* Contact order is stable in ID/feature space rather than incidental array order. */
    for(size_t i=1;i<nc;++i){Nf18a4Constraint t=ordered[i];size_t j=i;
        while(j>0u&&(ordered[j-1].body_a>t.body_a ||
             (ordered[j-1].body_a==t.body_a&&ordered[j-1].body_b>t.body_b)||
             (ordered[j-1].body_a==t.body_a&&ordered[j-1].body_b==t.body_b&&
              ordered[j-1].feature_id>t.feature_id))){ordered[j]=ordered[j-1];--j;}
        ordered[j]=t;}
    for(size_t i=1;i<nc;++i)if(ordered[i].body_a==ordered[i-1].body_a &&
         ordered[i].body_b==ordered[i-1].body_b && ordered[i].feature_id==ordered[i-1].feature_id)
         return o; /* no duplicate contact ancestry */
    Nf18a4Body working[NF18A4_MAX_BODIES];
    memcpy(working,b,n*sizeof(*b));
    const unsigned cap=6u;
    const unsigned cheap=policy==NF18A4_FRICTION_ADAPTIVE?2u:cap;
    unsigned used=0u;
    for(unsigned phase=0;phase<cap;phase+=2u){
        const unsigned limit=(phase==0u?cheap:phase+2u);
        for(unsigned pass=used;pass<limit;++pass){
            ++o.iterations;
            bool progress=false;
            for(size_t i=0;i<nc;++i){
                const size_t ia=body_index(working,n,ordered[i].body_a),ib=body_index(working,n,ordered[i].body_b);
                /* restitution only on the first pass; never repeatedly rebounce. */
                Nf18a4Constraint c=ordered[i];if(pass!=0u)c.restitution=0.0f;
                const Nf18a4Receipt r=nf18a4_apply_contact(&working[ia],&working[ib],c,policy,tick,version);
                ++o.constraint_evaluations;
                if(r.status==NF18A4_INVALID){o.status=NF18A4_INVALID;return o;}
                if(r.impulses_applied)progress=true;
                if(pass==0u)o.receipts[i]=r;
                else if(r.impulses_applied){
                    o.receipts[i].normal_impulse=vadd(o.receipts[i].normal_impulse,r.normal_impulse);
                    o.receipts[i].tangent_impulse=vadd(o.receipts[i].tangent_impulse,r.tangent_impulse);
                    o.receipts[i].signed_energy_delta+=r.signed_energy_delta;
                    o.receipts[i].impulses_applied=1u;
                }
            }
            /* Test ALL constraints, including those invalidated by later pairs. */
            bool all_satisfied=true;
            for(size_t i=0;i<nc;++i){
                const size_t ia=body_index(working,n,ordered[i].body_a),ib=body_index(working,n,ordered[i].body_b);
                const NfVec3 ra=vsub(ordered[i].contact_point,working[ia].center);
                const NfVec3 rb=vsub(ordered[i].contact_point,working[ib].center);
                const bool angular=policy>=NF18A4_ANGULAR_NORMAL;
                const NfVec3 v=vsub(velocity_at(&working[ia],ra,angular),velocity_at(&working[ib],rb,angular));
                if(dot(v,ordered[i].normal_b_to_a)<-0.005f)all_satisfied=false;
            }
            used=pass+1u;
            if(all_satisfied && (policy==NF18A4_FRICTION_ADAPTIVE || used>=cap)){
                o.status=NF18A4_ACCEPTED;o.contact_count=(uint8_t)nc;
                o.reserve_used=(policy==NF18A4_FRICTION_ADAPTIVE&&used>2u)?1u:0u;
                memcpy(b,working,n*sizeof(*b));return o;}
            if(!progress && !all_satisfied)break;
        }
        if(used>=cap)break;
        if(used<limit)break;
    }
    o.status=NF18A4_PENDING_CONTACT;
    o.contact_count=(uint8_t)nc;
    o.reserve_used=(policy==NF18A4_FRICTION_ADAPTIVE&&used>2u)?1u:0u;
    return o;
}

Nf18a4PairResult nf18a4_pair_step(Nf18a4Body actor,Nf18a4Body crate,
                                  Nf18a2ShapePolicy shape,Nf18a4Motor motor,
                                  Nf18a4Policy policy,float dt,uint32_t tick,
                                  uint32_t version,float restitution,float friction){
    Nf18a4PairResult r={0};r.status=NF18A4_INVALID;r.actor=actor;r.object=crate;
    if(!nf18a4_body_valid(&actor)||!nf18a4_body_valid(&crate)||actor.id==crate.id||
       actor.inverse_mass<=0||actor.radius<=0||actor.height<2.0f*actor.radius||
       !nf18a2_profile_valid(shape)||!isfinite(dt)||dt<=0.0f || !tick || !version ||
       !isfinite(restitution)||restitution<0||restitution>1 || !isfinite(friction)||friction<0 ||
       crate.box_half.x<=0||crate.box_half.y<=0||crate.box_half.z<=0)return r;
    Nf18a4MotorReceipt motor_receipt;
    if(!nf18a4_motor_drive(&r.actor,motor,dt,&motor_receipt))return r;
    r.motor_receipt=motor_receipt;
    const Nf18aCollider obstacle={.body_id=crate.id,
       .min=vsub(crate.center,crate.box_half),.max=vadd(crate.center,crate.box_half),
       .velocity=crate.velocity,.dynamic_body=crate.inverse_mass>0?1u:0u};
    const NfVec3 feet={r.actor.center.x,r.actor.center.y-0.5f*shape.height,r.actor.center.z};
    const NfVec3 delta=vmul(r.actor.velocity,dt);
    Nf18aContact hit={0};
    if(!nf18a3_capsule_sweep(feet,shape,delta,obstacle,dt,&hit)){
        r.actor.center=vadd(actor.center,delta);
        r.object.center=vadd(crate.center,vmul(crate.velocity,dt));
        r.status=NF18A4_ACCEPTED;return r;
    }
    r.swept_hit=1u;r.geometric_contact=hit;r.toi=hit.toi;
    if(hit.kind==NF18A_CONTACT_INITIAL_OVERLAP){
        r.actor=actor;r.object=crate;r.status=NF18A4_PENDING_CONTACT;return r;}
    const float h=dt*hit.toi;
    r.actor.center=vadd(actor.center,vmul(r.actor.velocity,h));
    r.object.center=vadd(crate.center,vmul(crate.velocity,h));
    const Nf18a4Constraint c={r.actor.id,r.object.id,crate.id,
        hit.normal,vadd(hit.point,vmul(crate.velocity,h)),restitution,friction};
    r.receipt=nf18a4_apply_contact(&r.actor,&r.object,c,policy,tick,version);
    if(r.receipt.status==NF18A4_INVALID){r.status=NF18A4_INVALID;return r;}
    const float rem=dt-h;
    r.actor.center=vadd(r.actor.center,vmul(r.actor.velocity,rem));
    r.object.center=vadd(r.object.center,vmul(r.object.velocity,rem));
    /* Under an estimate-only negative control, the actor may not penetrate;
       mark the false-motion outcome explicit for the scientific comparison. */
    const Nf18aCollider now={.body_id=crate.id,
        .min=vsub(r.object.center,crate.box_half),.max=vadd(r.object.center,crate.box_half)};
    const NfVec3 endfeet={r.actor.center.x,r.actor.center.y-0.5f*shape.height,r.actor.center.z};
    const float sep=nf18a2_capsule_box_separation(endfeet,shape,&now);
    if(sep< -0.002f){r.actor=actor;r.object=crate;r.status=NF18A4_PENDING_CONTACT;return r;}
    r.status=NF18A4_ACCEPTED;
    return r;
}

Nf18a4Status nf18a4_stage_traversal(Nf18a4Body *actor,Nf18a3Geometry world,
                                    Nf18a3Contract contract, Nf18a3Request request,
                                    Nf18a4Motor motor,float dt,uint32_t tick,
                                    uint32_t version,Nf18a4MotorReceipt *receipt){
    if(!actor||!receipt||!nf18a4_body_valid(actor)||!isfinite(dt)||dt<=0||!tick||!version)
        return NF18A4_INVALID;
    /* Re-adjudicate from the authoritative inputs at the moment of staging.
       A caller-supplied YES/YES decision is never itself authority. */
    const Nf18a3Decision decision=nf18a3_adjudicate(world,contract,request);
    if((actor->last_traversal_stage_tick==tick && actor->last_traversal_stage_version==version)||
       !decision.geometry_approved||!decision.contract_approved||!decision.commit_eligible||
       decision.actor_id!=actor->id || decision.tick!=tick||decision.world_version!=version||
       fabsf(decision.original_feet.x-actor->center.x)>0.0001f ||
       fabsf(decision.original_feet.y-(actor->center.y-actor->height*0.5f))>0.0001f ||
       fabsf(decision.original_feet.z-actor->center.z)>0.0001f ||
       fabsf(request.body_radius-actor->radius)>0.0001f ||
       fabsf(request.body_height-actor->height)>0.0001f)return NF18A4_GATED;
    /* A positive adjudication authorizes a motor attempt; does not teleport. */
    if(!nf18a4_motor_drive(actor,motor,dt,receipt))return NF18A4_INVALID;
    actor->last_traversal_stage_tick=tick;
    actor->last_traversal_stage_version=version;
    return NF18A4_ACCEPTED;
}
bool nf18a4_pair_to_sample(const Nf18a4PairResult *pair,Nf18aContact *out){
    if(!pair || pair->status!=NF18A4_ACCEPTED || !pair->swept_hit)return false;
    return nf18a4_receipt_to_sample(&pair->receipt,pair->actor.id,pair->object.id,out);
}
bool nf18a4_receipt_to_sample(const Nf18a4Receipt *r,
                              uint32_t a,uint32_t b,Nf18aContact *out){
    if(!r||!out||!a||!b||a==b||r->status!=NF18A4_ACCEPTED||
       !r->impulses_applied||!r->tick||!r->world_version ||
       !vfinite(r->normal_impulse)||!vfinite(r->tangent_impulse))return false;
    const float j=norm(r->normal_impulse);
    if(j<1.0e-7f)return false;
    memset(out,0,sizeof(*out));
    out->body_a=a;out->body_b=b;out->contact_id=r->contact_id;
    out->tick=r->tick;out->normal=vmul(r->normal_impulse,1.0f/j);
    out->normal_impulse=j;
    out->consequential=j>=20.0f?1u:0u; /* provisional threshold; history owns policy */
    out->kind=NF18A_CONTACT_TOUCH;
    return true;
}
const char *nf18a4_policy_name(Nf18a4Policy p){
    switch(p){case NF18A4_ESTIMATE_ONLY_CONTROL:return "estimate_only_control";
        case NF18A4_RECIPROCAL_LINEAR:return "reciprocal_linear";
        case NF18A4_ANGULAR_NORMAL:return "angular_normal";
        case NF18A4_FRICTION_FIXED6:return "friction_fixed6";
        case NF18A4_FRICTION_ADAPTIVE:return "friction_adaptive";
        default:return "unknown";}
}
uint32_t nf18a4_pair_hash(const Nf18a4PairResult *r){
    if(!r)return 0u;
    uint32_t h=mix((uint32_t)r->status^r->receipt.contact_id);
    const float x[]={r->actor.center.x,r->actor.center.y,r->actor.center.z,
                     r->actor.velocity.x,r->actor.velocity.y,r->actor.velocity.z,
                     r->actor.omega.x,r->actor.omega.y,r->actor.omega.z,
                     r->object.center.x,r->object.center.y,r->object.center.z,
                     r->object.velocity.x,r->object.velocity.y,r->object.velocity.z,
                     r->object.omega.x,r->object.omega.y,r->object.omega.z,
                     r->receipt.normal_impulse.x,r->receipt.tangent_impulse.x};
    for(size_t i=0;i<sizeof(x)/sizeof(x[0]);++i)h=mix(h^q(x[i]));
    return h;
}
