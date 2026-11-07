#ifndef NF_RETC_M2_CARRY_TICK_H
#define NF_RETC_M2_CARRY_TICK_H
/* H213-H217: A6 owner-bounded, private kinematic CARRY candidate.
 * A source NfActor interaction plus actual actor/object/mirror evidence
 * is needed for every attempt. Both swept paths require canonical/fine
 * A5 static clearance; a single private A5 tick contains no fabricated
 * collision receipt. This is NOT B2 entitlement, B3 possession,
 * multi-actor arbitration or an accepted/persistent game tick. */
#include "nf_retc_m2_push_tick.h"
typedef struct NfRetcM2CarryEvidence {
 Nf18a5Integrated candidate;
 NfVec3 source_offset;
 uint64_t source_tick;
 uint32_t source_revision;
 bool staged;
} NfRetcM2CarryEvidence;
static inline bool nf_retc_m2_carry_stage_in_tick(
 const NfWorld *w,const Nf18a6Runtime *owner,float dt,
 NfRetcM2CarryEvidence *out){
 if(!out)return false;
 out->staged=false;
 if(!w||!owner||!w->tick_in_progress||
    w->tick_owner_context!=owner||!w->tick_owner||
    !owner->require_pair_research||!owner->research_pair_stage||
    owner->require_dual_contact_preflight||
    !isfinite(dt)||dt<=0.f||dt>.25f||w->tick>=UINT32_MAX||
    owner->last_commit_tick!=w->tick||
    owner->physical.world.tick!=w->tick||
    owner->physical.world.revision==UINT32_MAX||
    owner->object_collider_index>=w->collider_count)return false;
 const NfActor *actor=nf_world_find_actor_const(w,owner->actor_id);
 const Nf18a4Body *a=&owner->physical.actor;
 const Nf18a4Body *b=&owner->physical.object;
 if(!actor||!actor->active||!actor->combat.alive||
    !actor->input.interact_held||
    !isfinite(actor->input.forward)||actor->input.forward<=.1f||
    !isfinite(actor->input.strafe)||fabsf(actor->input.strafe)>.05f||
    !isfinite(actor->input.yaw_radians)||fabsf(actor->input.yaw_radians)>.05f||
    a->id!=owner->actor_id||a->inverse_mass<=0.f||
    b->id==a->id||b->inverse_mass<=0.f||
    fabsf(actor->transform.position.x-a->center.x)>.0001f||
    fabsf(actor->transform.position.z-a->center.z)>.0001f||
    fabsf(actor->transform.velocity.x-a->velocity.x)>.0001f||
    fabsf(actor->transform.velocity.y-a->velocity.y)>.0001f||
    fabsf(actor->transform.velocity.z-a->velocity.z)>.0001f||
    fabsf(actor->movement.body_height-a->height)>.0001f||
    a->velocity.x<=.1f||
    fabsf(a->velocity.y)>.001f||fabsf(a->velocity.z)>.001f||
    fabsf(b->center.y-a->center.y)>.2f||
    b->center.x-a->center.x<.75f||
    b->center.x-a->center.x>1.25f||
    fabsf(b->center.z-a->center.z)>.2f||
    b->box_half.x<=0.f||b->box_half.y<=0.f||b->box_half.z<=0.f||
    b->box_half.x>.5f||b->box_half.z>.5f)return false;
 const NfCollider *c=&w->colliders[owner->object_collider_index];
 if(c->kind!=NF_COLLIDER_SOLID||
    fabsf(c->min.x-(b->center.x-b->box_half.x))>.0001f||
    fabsf(c->min.y-(b->center.y-b->box_half.y))>.0001f||
    fabsf(c->min.z-(b->center.z-b->box_half.z))>.0001f||
    fabsf(c->max.x-(b->center.x+b->box_half.x))>.0001f||
    fabsf(c->max.y-(b->center.y+b->box_half.y))>.0001f||
    fabsf(c->max.z-(b->center.z+b->box_half.z))>.0001f)return false;
 const NfVec3 delta={a->velocity.x*dt,0.f,0.f};
 if(!isfinite(delta.x)||delta.x<=0.f||delta.x>1.f)return false;
 Nf18a5Grid clearance=owner->physical.world.grid;
 const Nf18a2ShapePolicy ashape={
    a->radius,a->height,.05f,.4f,.65f};
 /* Conservative capsule around the crate's box (using maximum horizontal
    half extent); rotational carry and angular CCD are unsupported. */
 const float crate_r=fmaxf(b->box_half.x,b->box_half.z);
 const float crate_h=fmaxf(2.f*b->box_half.y,2.f*crate_r);
 const Nf18a2ShapePolicy bshape={
    crate_r,crate_h,.05f,.4f,.65f};
 const uint32_t target=(uint32_t)w->tick+1u;
 uint32_t loads=0u;
 const NfVec3 af={a->center.x,a->center.y-a->height*.5f,a->center.z};
 const NfVec3 bf={b->center.x,b->center.y-crate_h*.5f,b->center.z};
 if(nf18a5_sweep_query(&clearance,af,delta,ashape,dt,target,
        NULL,NULL,&loads)!=NF18A5_Q_FREE||
    nf18a5_sweep_query(&clearance,bf,delta,bshape,dt,target,
        NULL,NULL,&loads)!=NF18A5_Q_FREE)return false;
 NfRetcM2CarryEvidence next={0};
 next.candidate=owner->physical;
 const uint32_t revision=next.candidate.world.revision;
 if(!nf18a5_world_commit(&next.candidate.world,revision,target,
                          a->id,NULL,0u))return false;
 next.source_offset=(NfVec3){
    b->center.x-a->center.x,b->center.y-a->center.y,
    b->center.z-a->center.z};
 next.candidate.actor.center.x+=delta.x;
 next.candidate.object.center.x+=delta.x;
 next.candidate.object.velocity=a->velocity;
 if(fabsf((next.candidate.object.center.x-
           next.candidate.actor.center.x)-next.source_offset.x)>.0001f||
    next.candidate.world.revision!=revision+1u||
    next.candidate.world.tick!=target||
    next.candidate.world.history.raw_samples!=
        owner->physical.world.history.raw_samples||
    next.candidate.committed_receipts!=owner->physical.committed_receipts)
    return false;
 next.source_tick=w->tick;
 next.source_revision=revision;
 next.staged=true;
 *out=next;
 return true;
}
#endif
