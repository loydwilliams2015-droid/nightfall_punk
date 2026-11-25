#ifndef NF_RETC_H319_SHARED_CONTACT_H
#define NF_RETC_H319_SHARED_CONTACT_H
/* Narrow research candidate: two translating capsules, one translating AABB.
 * Discover both contacts on the original snapshot. Only simultaneous TOIs
 * enter the existing bounded island; staggered events require a later solver.
 * No A5 clearance, owner authentication, B2/B3 or world publication. */
#include "nf_contact18a4.h"
#include <math.h>
#include <string.h>
typedef struct NfRetcH319SharedContact {
    Nf18aContact hits[2];
    Nf18a4Constraint constraints[2];
    Nf18a4Body bodies[3];
    Nf18a4IslandResult island;
} NfRetcH319SharedContact;
static inline Nf18a4Status nf_retc_h319_shared_contact(
    const Nf18a4Body source[3],float dt,uint32_t tick,uint32_t revision,
    NfRetcH319SharedContact *out){
    if(!source||!out||!isfinite(dt)||dt<=0.f||!tick||!revision)
        return NF18A4_INVALID;
    for(size_t i=0;i<3u;++i){
        if(!nf18a4_body_valid(&source[i]))return NF18A4_INVALID;
        for(size_t j=0;j<i;++j)if(source[i].id==source[j].id)
            return NF18A4_INVALID;
    }
    const Nf18a4Body *box=&source[2];
    if(box->box_half.x<=0.f||box->box_half.y<=0.f||
       box->box_half.z<=0.f||box->inverse_mass<=0.f)
        return NF18A4_INVALID;
    NfRetcH319SharedContact next={0};
    memcpy(next.bodies,source,sizeof next.bodies);
    const Nf18aCollider obstacle={
        .body_id=box->id,
        .min={box->center.x-box->box_half.x,box->center.y-box->box_half.y,box->center.z-box->box_half.z},
        .max={box->center.x+box->box_half.x,box->center.y+box->box_half.y,box->center.z+box->box_half.z},
        .velocity=box->velocity,.dynamic_body=1u};
    for(size_t i=0;i<2u;++i){
        const Nf18a4Body *a=&source[i];
        if(a->radius<=0.f||a->height<2.f*a->radius||a->inverse_mass<=0.f)
            return NF18A4_INVALID;
        const Nf18a2ShapePolicy shape={a->radius,a->height,0.f,0.f,1.f};
        const NfVec3 feet={a->center.x,a->center.y-.5f*a->height,a->center.z};
        const NfVec3 delta={a->velocity.x*dt,a->velocity.y*dt,a->velocity.z*dt};
        Nf18aContact *hit=&next.hits[i];
        if(!nf18a3_capsule_sweep(feet,shape,delta,obstacle,dt,hit)||
           hit->kind==NF18A_CONTACT_INITIAL_OVERLAP||
           !isfinite(hit->toi)||hit->toi<0.f||hit->toi>1.f)
            return NF18A4_PENDING_CONTACT;
        const float h=dt*hit->toi;
        next.constraints[i]=(Nf18a4Constraint){
            a->id,box->id,box->id,hit->normal,
            {hit->point.x+box->velocity.x*h,hit->point.y+box->velocity.y*h,
             hit->point.z+box->velocity.z*h},0.f,0.f};
    }
    if(fabsf(next.hits[0].toi-next.hits[1].toi)>1e-5f)
        return NF18A4_PENDING_CONTACT;
    const float h=dt*fminf(next.hits[0].toi,next.hits[1].toi);
    for(size_t i=0;i<3u;++i){
        next.bodies[i].center.x+=next.bodies[i].velocity.x*h;
        next.bodies[i].center.y+=next.bodies[i].velocity.y*h;
        next.bodies[i].center.z+=next.bodies[i].velocity.z*h;
    }
    next.island=nf18a4_solve_island(next.bodies,3u,next.constraints,2u,
        NF18A4_RECIPROCAL_LINEAR,tick,revision);
    if(next.island.status!=NF18A4_ACCEPTED)
        return (Nf18a4Status)next.island.status;
    if(next.island.contact_count!=2u||
       next.island.receipts[0].contact_id==next.island.receipts[1].contact_id)
        return NF18A4_PENDING_CONTACT;
    for(size_t i=0;i<2u;++i){
        Nf18aContact sample={0};
        if(!nf18a4_receipt_to_sample(&next.island.receipts[i],
            next.constraints[i].body_a,box->id,&sample))
            return NF18A4_PENDING_CONTACT;
    }
    *out=next;
    return NF18A4_ACCEPTED;
}
#endif
