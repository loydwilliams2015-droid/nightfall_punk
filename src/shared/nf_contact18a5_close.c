#include "nf_contact18a5_close.h"
#include <math.h>
#include <limits.h>
#include <string.h>

static bool axis(float v,int32_t *c){
    if(!isfinite(v))return false;
    const double n=floor((double)v/4.0);
    if(n<(double)INT32_MIN || n>(double)INT32_MAX)return false;
    *c=(int32_t)n;return true;
}
static Nf18a5Chunk *get(Nf18a5Grid *g,int32_t cx,int32_t cy,int32_t cz){
    for(unsigned i=0;i<g->count;++i)if(g->chunks[i].loaded&&g->chunks[i].x==cx&&
        g->chunks[i].y==cy&&g->chunks[i].z==cz)return &g->chunks[i];
    return NULL;
}
void nf18a5_integrated_init(Nf18a5Integrated *s,uint8_t cap,Nf18a4Body a,Nf18a4Body b){
    if(!s)return;
    memset(s,0,sizeof(*s));nf18a5_world_init(&s->world,cap);s->actor=a;s->object=b;
}
Nf18a5Query nf18a5_resolve_exact(Nf18a5Grid *g,NfVec3 p,uint32_t tick,
                                  Nf18a5FineProvider provider,void *ctx,uint32_t *loads){
    if(loads)*loads=0;
    if(!g)return NF18A5_Q_INVALID;
    Nf18a5Query q=nf18a5_query(g,p.x,p.y,p.z,tick,true);
    if(q!=NF18A5_Q_PENDING || !provider)return q;
    int32_t x,y,z;
    if(!axis(p.x,&x)||!axis(p.y,&y)||!axis(p.z,&z))return NF18A5_Q_INVALID;
    Nf18a5Chunk *c=get(g,x,y,z);
    /* Canonical state is authoritative; provider cannot create FREE from an
       absent cell, or silently revise material on a cache fill. */
    if(!c||!c->loaded||!c->canonical_epoch||tick<c->last_access_tick)
        return NF18A5_Q_PENDING;
    const uint32_t material_revision=g->global_revision;
    const uint32_t parent=c->canonical_epoch;
    uint8_t scale=0,voxels[NF18A5_FINE_MAX_VOXELS];size_t count=0;
    if(!provider(ctx,x,y,z,parent,&scale,voxels,&count))return NF18A5_Q_PENDING;
    if(g->global_revision!=material_revision)return NF18A5_Q_PENDING;
    if(!nf18a5_load_fine(g,x,y,z,scale,voxels,count,parent,tick))
        return NF18A5_Q_PENDING;
    if(loads)*loads=1;
    return nf18a5_query(g,p.x,p.y,p.z,tick,true);
}
/* Conservative swept-volume voxel witness. Checks every canonical voxel
   intersected by the capsule's swept AABB, and casts the actual capsule
   against each SOLID canonical or fine subvoxel. No point-query shortcut can
   authorize the whole actor path. Static material only: dynamic objects go
   through nf18a4_pair_step separately. */
static Nf18a5Query world_sweep(Nf18a5Grid *g,NfVec3 feet,
        NfVec3 displacement,Nf18a2ShapePolicy shape,float dt,uint32_t tick,
        Nf18a5FineProvider provider,void *ctx,uint32_t *loads){
    if(!g||!isfinite(feet.x)||!isfinite(feet.y)||!isfinite(feet.z)||
       !isfinite(displacement.x)||!isfinite(displacement.y)||!isfinite(displacement.z)||
       !(dt>0.0f)||!isfinite(dt)||!(shape.radius>0.0f)||!(shape.height>0.0f))
        return NF18A5_Q_INVALID;
    double lo[3]={fmin((double)feet.x,(double)feet.x+displacement.x)-shape.radius,
                  fmin((double)feet.y,(double)feet.y+displacement.y),
                  fmin((double)feet.z,(double)feet.z+displacement.z)-shape.radius};
    double hi[3]={fmax((double)feet.x,(double)feet.x+displacement.x)+shape.radius,
                  fmax((double)feet.y,(double)feet.y+displacement.y)+shape.height,
                  fmax((double)feet.z,(double)feet.z+displacement.z)+shape.radius};
    int32_t mn[3],mx[3];
    for(int i=0;i<3;++i){
        if(!isfinite(lo[i])||!isfinite(hi[i])||
           floor(lo[i])<(double)INT32_MIN||floor(hi[i])>(double)INT32_MAX)
            return NF18A5_Q_INVALID;
        mn[i]=(int32_t)floor(lo[i]);mx[i]=(int32_t)floor(hi[i]);
    }
    uint64_t span=(uint64_t)((int64_t)mx[0]-mn[0]+1)*
                  (uint64_t)((int64_t)mx[1]-mn[1]+1)*
                  (uint64_t)((int64_t)mx[2]-mn[2]+1);
    if(span>4096u)return NF18A5_Q_PENDING; /* explicit bounded frontier */
    bool blocked=false,pending=false;
    for(int64_t y=mn[1];y<=mx[1];++y)for(int64_t z=mn[2];z<=mx[2];++z)
        for(int64_t x=mn[0];x<=mx[0];++x){
        int32_t cx=(int32_t)floor((double)x/4.0);
        int32_t cy=(int32_t)floor((double)y/4.0);
        int32_t cz=(int32_t)floor((double)z/4.0);
        Nf18a5Chunk *c=get(g,cx,cy,cz);
        if(!c){pending=true;continue;}
        unsigned lx=(unsigned)(x-(int64_t)cx*4);
        unsigned ly=(unsigned)(y-(int64_t)cy*4);
        unsigned lz=(unsigned)(z-(int64_t)cz*4);
        const uint8_t v=c->canonical[(ly*4u+lz)*4u+lx];
        if(v==NF18A5_FREE)continue;
        if(v==NF18A5_MIXED && (!c->fine_valid||c->fine_parent_epoch!=c->canonical_epoch)){
            NfVec3 probe={(float)x+.25f,(float)y+.25f,(float)z+.25f};
            uint32_t loaded=0;
            Nf18a5Query q=nf18a5_resolve_exact(g,probe,tick,provider,ctx,&loaded);
            if(loads)*loads+=loaded;
            if(q==NF18A5_Q_PENDING){pending=true;continue;}
            if(q==NF18A5_Q_INVALID)return q;
            c=get(g,cx,cy,cz);
        }
        unsigned sub=v==NF18A5_SOLID?1u:c->fine_resolution;
        if(v==NF18A5_MIXED && (!c||!c->fine_valid||sub==0u)){
            pending=true;continue;
        }
        for(unsigned dy=0;dy<sub;++dy)for(unsigned dz=0;dz<sub;++dz)
            for(unsigned dx=0;dx<sub;++dx){
            if(v==NF18A5_MIXED){
                unsigned sx=lx*sub+dx,sy=ly*sub+dy,sz=lz*sub+dz,n=4u*sub;
                if(c->fine[(sy*n+sz)*n+sx]!=NF18A5_SOLID)continue;
            }
            float step=1.0f/(float)sub;
            Nf18aCollider obstacle={0};obstacle.body_id=UINT32_MAX;
            obstacle.min=(NfVec3){(float)x+(float)dx*step,
                                   (float)y+(float)dy*step,(float)z+(float)dz*step};
            obstacle.max=(NfVec3){obstacle.min.x+step,obstacle.min.y+step,
                                   obstacle.min.z+step};
            Nf18aContact hit={0};
            if(nf18a3_capsule_sweep(feet,shape,displacement,obstacle,dt,&hit))
                blocked=true;
        }
    }
    /* A known obstruction cannot be overridden by a pending neighbor. */
    if(blocked)return NF18A5_Q_SOLID;
    return pending?NF18A5_Q_PENDING:NF18A5_Q_FREE;
}
static Nf18a5CloseResult result(Nf18a5CloseStatus status,Nf18a5Query q,
                                uint32_t rev){
    Nf18a5CloseResult r={0};r.status=status;r.query=q;
    r.material_revision_before=rev;r.material_revision_after=rev;return r;
}
static bool publish(Nf18a5Integrated *dst,const Nf18a5Integrated *candidate,
                    const char *journal){
    /* Failure leaves caller unmodified. For a supplied journal the COMPLETE
       authoritative snapshot reaches the WAL before in-memory publication. */
    if(journal && !nf18a5_checkpoint(journal,candidate))return false;
    *dst=*candidate;return true;
}
Nf18a5CloseResult nf18a5_integrated_pair_step(
    Nf18a5Integrated *s,uint32_t version,uint32_t tick,NfVec3 probe,
    Nf18a5FineProvider loader,void *ctx,Nf18a2ShapePolicy shape,
    Nf18a4Motor motor,float dt,float restitution,float friction,const char *journal){
    if(!s || !nf18a4_body_valid(&s->actor)||!nf18a4_body_valid(&s->object)||
       s->actor.id==s->object.id || version!=s->world.revision || tick<=s->world.tick)
        return result(NF18A5_CLOSE_STALE,NF18A5_Q_INVALID,s?s->world.grid.global_revision:0);
    Nf18a5Integrated next=*s;
    Nf18a5CloseResult out=result(NF18A5_CLOSE_PENDING,NF18A5_Q_PENDING,
                                 s->world.grid.global_revision);
    out.query=nf18a5_resolve_exact(&next.world.grid,probe,tick,loader,ctx,&out.cache_loads);
    if(out.query==NF18A5_Q_INVALID){out.status=NF18A5_CLOSE_INVALID;return out;}
    if(out.query==NF18A5_Q_PENDING){out.status=NF18A5_CLOSE_PENDING;return out;}
    if(out.query==NF18A5_Q_SOLID){out.status=NF18A5_CLOSE_BLOCKED;return out;}
    /* Coverage check: the entire swept capsule against canonical/fine solid
       geometry. The source motor is applied only on a throwaway body. */
    Nf18a4Body preview=next.actor;
    Nf18a4MotorReceipt preview_motor={0};
    if(!nf18a4_motor_drive(&preview,motor,dt,&preview_motor)){
        out.status=NF18A5_CLOSE_INVALID;return out;
    }
    const NfVec3 feet={next.actor.center.x,
                       next.actor.center.y-0.5f*shape.height,next.actor.center.z};
    const NfVec3 desired={preview.velocity.x*dt,preview.velocity.y*dt,preview.velocity.z*dt};
    Nf18a5Query route=world_sweep(&next.world.grid,feet,desired,shape,dt,tick,
                                   loader,ctx,&out.cache_loads);
    if(route!=NF18A5_Q_FREE){
        out.query=route;
        out.status=route==NF18A5_Q_SOLID?NF18A5_CLOSE_BLOCKED:
                   route==NF18A5_Q_PENDING?NF18A5_CLOSE_PENDING:NF18A5_CLOSE_INVALID;
        return out;
    }
    /* Strictly tested capsule-to-translating-box CCD and bounded reciprocal
       response. Neither FREE probe nor a contact receipt bypasses CCD. */
    Nf18a4PairResult pair=nf18a4_pair_step(next.actor,next.object,shape,motor,
                        NF18A4_FRICTION_ADAPTIVE,dt,tick,version,restitution,friction);
    if(pair.status!=NF18A4_ACCEPTED){
        out.status=pair.status==NF18A4_PENDING_CONTACT?NF18A5_CLOSE_PENDING:NF18A5_CLOSE_INVALID;
        return out;
    }
    /* A physical contact may redirect motion. Check both pre-impact and
       post-impact paths against the same authoritative STATIC geometry. */
    const NfVec3 actual={pair.actor.center.x-next.actor.center.x,
                          pair.actor.center.y-next.actor.center.y,
                          pair.actor.center.z-next.actor.center.z};
    Nf18a5Query realized=world_sweep(&next.world.grid,feet,actual,shape,dt,tick,
                                      loader,ctx,&out.cache_loads);
    if(realized!=NF18A5_Q_FREE){
        out.query=realized;
        out.status=realized==NF18A5_Q_SOLID?NF18A5_CLOSE_BLOCKED:
                   realized==NF18A5_Q_PENDING?NF18A5_CLOSE_PENDING:NF18A5_CLOSE_INVALID;
        return out;
    }
    Nf18a5Sample sample={0};const Nf18a5Sample *src=NULL;size_t n=0;
    if(pair.swept_hit && pair.receipt.impulses_applied &&
       nf18a5_receipt_sample(&pair.receipt,pair.actor.id,pair.object.id,0,
                             next.world.grid.global_revision,&sample)){
        src=&sample;n=1;out.dynamic_contact=1;out.applied=pair.receipt;
    } else if(pair.swept_hit && pair.receipt.impulses_applied){
        out.status=NF18A5_CLOSE_INVALID;return out;
    }
    /* A staged pair is NOT a historical contact until this owner + tick
       commits body state and history together. */
    if(!nf18a5_world_commit(&next.world,version,tick,next.actor.id,src,n)){
        out.status=NF18A5_CLOSE_INVALID;return out;
    }
    next.actor=pair.actor;next.object=pair.object;
    if(out.dynamic_contact)++next.committed_receipts;
    if(!publish(s,&next,journal)){
        out.status=NF18A5_CLOSE_DISK_FAILED;return out;
    }
    out.status=NF18A5_CLOSE_COMMITTED;
    out.material_revision_after=s->world.grid.global_revision;return out;
}
Nf18a5CloseResult nf18a5_integrated_support_step(
    Nf18a5Integrated *s,uint32_t version,uint32_t tick,
    Nf18a4Constraint support,Nf18a3Contract auth,
    NfVec3 gravity,float dt,const char *journal){
    if(!s||!isfinite(dt)||dt<=0.0f||
       !isfinite(gravity.x)||!isfinite(gravity.y)||!isfinite(gravity.z)||
       version!=s->world.revision||tick<=s->world.tick)
        return result(NF18A5_CLOSE_STALE,NF18A5_Q_INVALID,s?s->world.grid.global_revision:0);
    Nf18a5CloseResult out=result(NF18A5_CLOSE_INVALID,NF18A5_Q_FREE,
                                 s->world.grid.global_revision);
    if(support.body_a!=s->actor.id||support.body_b!=s->object.id||
       support.normal_b_to_a.y<0.7071067f)return out;
    /* Authority is checked against the same authoritative world frame and
       the solver's ACTUAL support box and actor capsule. A supplied normal or
       alleged resting force cannot manufacture grounding. */
    const Nf18a4Body a=s->actor,b=s->object;
    const float fy=a.center.y-a.height*.5f;
    const float top=b.center.y+b.box_half.y;
    bool auth_ok=auth.actor_id==a.id&&auth.feature_id==b.id&&
       auth.current_support_id==b.id&&auth.world_version==version&&
       auth.tick==tick&&auth.actor_grounded&&auth.material_support_valid&&
       auth.support_stable&&isfinite(fy)&&fabsf(auth.authoritative_feet.y-fy)<=.0001f&&
       fabsf(auth.authoritative_feet.x-a.center.x)<=.0001f&&
       fabsf(auth.authoritative_feet.z-a.center.z)<=.0001f&&
       fabsf(auth.authoritative_body_radius-a.radius)<=.0001f&&
       fabsf(auth.authoritative_body_height-a.height)<=.0001f&&
       b.box_half.x>0&&b.box_half.y>0&&b.box_half.z>0&&
       fabsf(fy-top)<=.001f&&
       fabsf(support.contact_point.y-top)<=.001f&&
       fabsf(support.contact_point.x-a.center.x)<=.001f&&
       fabsf(support.contact_point.z-a.center.z)<=.001f&&
       fabsf(a.center.x-b.center.x)+auth.authoritative_foot_flat_radius<=b.box_half.x+.0001f&&
       fabsf(a.center.z-b.center.z)+auth.authoritative_foot_flat_radius<=b.box_half.z+.0001f&&
       support.normal_b_to_a.y>=.999f&&
       (!b.inverse_mass || auth.allow_dynamic_support);
    if(!auth_ok){out.status=NF18A5_CLOSE_GATED;return out;}

    Nf18a5Integrated next=*s;
    Nf18a4Body bs[2]={next.actor,next.object};
    /* External field is server-supplied; demonstrate real gravitational
       approach against a real contact constraint rather than fake load. */
    const NfVec3 dv={gravity.x*dt,gravity.y*dt,gravity.z*dt};
    bs[0].velocity.x+=dv.x;bs[0].velocity.y+=dv.y;bs[0].velocity.z+=dv.z;
    if(!nf18a4_body_valid(&bs[0]))return out;
    if(bs[0].inverse_mass>0.0f){
        const float mass=1.0f/bs[0].inverse_mass;
        out.external_impulse=(NfVec3){mass*dv.x,mass*dv.y,mass*dv.z};
    }
    Nf18a4IslandResult ir=nf18a4_solve_island(bs,2,&support,1,
                          NF18A4_FRICTION_ADAPTIVE,tick,version);
    if(ir.status==NF18A4_PENDING_CONTACT){out.status=NF18A5_CLOSE_PENDING;return out;}
    if(ir.status!=NF18A4_ACCEPTED)return out;
    Nf18a5Sample sample={0};const Nf18a5Sample *sp=NULL;size_t n=0;
    for(unsigned k=0;k<ir.contact_count;++k){
        const Nf18a4Receipt *rc=&ir.receipts[k];
        if(rc->status!=NF18A4_ACCEPTED || !rc->impulses_applied)continue;
        const NfVec3 j=rc->normal_impulse;
        float magnitude=sqrtf(j.x*j.x+j.y*j.y+j.z*j.z);
        if(!isfinite(magnitude))return out;
        /* Actual applied solver impulse balances modeled gravity/motor load;
           report its force-equivalent as support load, NOT violent impact. */
        sample=(Nf18a5Sample){tick,rc->contact_id,bs[0].id,bs[1].id,
            0.0f,magnitude/dt,0.0f,next.world.grid.global_revision};
        sp=&sample;n=1;out.applied=*rc;out.dynamic_contact=1;break;
    }
    if(!nf18a5_world_commit(&next.world,version,tick,next.actor.id,sp,n))return out;
    next.actor=bs[0];next.object=bs[1];if(n)++next.committed_receipts;
    if(!publish(s,&next,journal)){out.status=NF18A5_CLOSE_DISK_FAILED;return out;}
    out.status=NF18A5_CLOSE_COMMITTED;return out;
}
