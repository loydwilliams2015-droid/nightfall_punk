#ifndef NF_RETC_M2_SUPPORT_TICK_H
#define NF_RETC_M2_SUPPORT_TICK_H
/* H203-H207: genuine A4 island support response and A5 resting load history
 * on a PRIVATE A6-owned copy. Requires real actor grounding on the owner's
 * static collider. No published tick, B3, frame or durable journal. */
#include "nf_retc_m2_pair_tick.h"
typedef struct NfRetcM2SupportEvidence {
 Nf18a5Integrated candidate;
 Nf18a5CloseResult result;
 uint32_t source_revision;
 uint64_t source_tick;
 bool staged;
} NfRetcM2SupportEvidence;
static inline bool nf_retc_m2_support_stage_in_tick(
 const NfWorld *world,const Nf18a6Runtime *owner,float dt,
 NfRetcM2SupportEvidence *out){
 if(!out)return false;
 out->staged=false;
 if(!world||!owner||!world->tick_in_progress||
    world->tick_owner_context!=owner||!world->tick_owner||
    !owner->require_pair_research||owner->require_dual_contact_preflight||
    !isfinite(dt)||dt<=0.f||world->tick>=UINT32_MAX||
    owner->last_commit_tick!=world->tick||
    owner->physical.world.tick!=world->tick)return false;
 const NfActor *actor=nf_world_find_actor_const(world,owner->actor_id);
 if(!actor||!actor->active||!actor->combat.alive||
    !actor->movement.grounded||
    actor->movement.ground_collider!=(int)owner->object_collider_index||
    actor->id!=owner->physical.actor.id||
    fabsf(actor->transform.position.x-owner->physical.actor.center.x)>.0001f||
    fabsf(actor->transform.position.z-owner->physical.actor.center.z)>.0001f||
    fabsf(actor->transform.velocity.y-owner->physical.actor.velocity.y)>.0001f||
    fabsf(actor->movement.body_height-owner->physical.actor.height)>.0001f)
    return false;
 const Nf18a4Body *body=&owner->physical.actor;
 const Nf18a4Body *object=&owner->physical.object;
 const float feet=body->center.y-body->height*.5f;
 const float top=object->center.y+object->box_half.y;
 if(object->inverse_mass!=0.f||
    fabsf(feet-top)>.001f||
    fabsf(body->center.x-object->center.x)+.05f>object->box_half.x+.0001f||
    fabsf(body->center.z-object->center.z)+.05f>object->box_half.z+.0001f)
    return false;
 const uint32_t version=owner->physical.world.revision;
 const uint32_t tick=(uint32_t)world->tick+1u;
 const Nf18a4Constraint support={
    body->id,object->id,object->id,
    {0.f,1.f,0.f},{body->center.x,top,body->center.z},0.f,.5f};
 Nf18a3Contract authority={0};
 authority.actor_id=body->id;
 authority.feature_id=object->id;
 authority.current_support_id=object->id;
 authority.world_version=version;
 authority.tick=tick;
 authority.actor_grounded=actor->movement.grounded?1u:0u;
 authority.material_support_valid=1u;
 authority.support_stable=1u;
 authority.authoritative_body_radius=body->radius;
 authority.authoritative_body_height=body->height;
 authority.authoritative_foot_flat_radius=.05f;
 authority.authoritative_feet=(NfVec3){body->center.x,feet,body->center.z};
 NfRetcM2SupportEvidence next={0};
 next.candidate=owner->physical;
 next.result=nf18a5_integrated_support_step(&next.candidate,version,tick,
       support,authority,(NfVec3){0.f,-9.81f,0.f},dt,NULL);
 if(next.result.status!=NF18A5_CLOSE_COMMITTED||
    !next.result.dynamic_contact||
    !next.result.applied.impulses_applied||
    next.result.external_impulse.y>=0.f||
    next.candidate.world.revision!=version+1u||
    next.candidate.world.tick!=tick||
    next.candidate.world.history.raw_samples!=
      owner->physical.world.history.raw_samples+1u||
    next.candidate.committed_receipts!=owner->physical.committed_receipts+1u)
    return false;
 next.source_revision=version;
 next.source_tick=world->tick;
 next.staged=true;
 *out=next;
 return true;
}
#endif
