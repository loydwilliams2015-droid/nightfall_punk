#include "nf_embody18a6.h"
#include "nf_movement.h"
#include <math.h>
#include <string.h>
#include <stdint.h>

static NfVec3 vsub(NfVec3 a,NfVec3 b){return (NfVec3){a.x-b.x,a.y-b.y,a.z-b.z};}
static float vdot(NfVec3 a,NfVec3 b){return a.x*b.x+a.y*b.y+a.z*b.z;}
static bool finite3(NfVec3 a){return isfinite(a.x)&&isfinite(a.y)&&isfinite(a.z);}
static uint32_t mix(uint32_t a){a^=a>>16;a*=0x7feb352du;a^=a>>15;a*=0x846ca68bu;a^=a>>16;return a;}
static uint32_t quant(float x){return isfinite(x)?(uint32_t)(int32_t)llround((double)x*10000.0):UINT32_MAX;}
static uint32_t hash_body(const Nf18a4Body *a,const Nf18a4Body *b,uint32_t tick,uint32_t rev){
    uint32_t h=mix(tick^rev);
    const float f[]={a->center.x,a->center.y,a->center.z,a->velocity.x,a->velocity.y,a->velocity.z,
        b->center.x,b->center.y,b->center.z,b->velocity.x,b->velocity.y,b->velocity.z};
    for(size_t i=0;i<sizeof(f)/sizeof(f[0]);++i)h=mix(h^quant(f[i]));
    return h;
}
static bool actor_consistent(const NfActor *a,const Nf18a4Body *b){
    if(!a||!b||a->id!=b->id||!a->active)return false;
    const NfVec3 feet={b->center.x,b->center.y-b->height*.5f,b->center.z};
    NfVec3 d=vsub(a->transform.position,feet),dv=vsub(a->transform.velocity,b->velocity);
    return vdot(d,d)<1.0e-8f && vdot(dv,dv)<1.0e-8f &&
        fabsf(a->movement.body_height-b->height)<.0001f;
}
static Nf18a6Outcome outcome(Nf18a6Status s,const Nf18a6Runtime *r){
    Nf18a6Outcome o={0};o.status=s;
    if(r){o.tick=r->physical.world.tick;o.world_revision=r->physical.world.revision;
        o.material_epoch=r->physical.world.grid.global_revision;
        o.raw_samples=r->physical.world.history.raw_samples;
        o.event_count=r->physical.world.history.generated_events;
        o.authoritative_hash=r->last_hash;
        o.realized_feet=(NfVec3){r->physical.actor.center.x,
          r->physical.actor.center.y-r->physical.actor.height*.5f,r->physical.actor.center.z};}
    return o;
}
bool nf18a6_init(Nf18a6Runtime *r,NfWorld *w,NfEntityId id,
                Nf18a4Body object,uint8_t cap,Nf18a6Model model){
#ifndef NF18A6_TEST_CONTROLS
    /* Scientific negative controls must not bypass authoritative material
       history in production even when callers can name their enum values. */
    if(model==NF18A6_LEGACY_REFERENCE || model==NF18A6_LAB_ONLY)return false;
#endif
    if(!r||!w||!id||model>NF18A6_WORLD_CAMERA||!nf18a4_body_valid(&object)||
       object.id==id||!cap)return false;
    const NfActor *a=nf_world_find_actor_const(w,id);
    if(!a||!a->combat.alive||w->tick>UINT32_MAX-1u)return false;
    Nf18a4Body body={0};body.id=id;body.center=a->transform.position;
    body.center.y+=a->movement.body_height*.5f;
    body.velocity=a->transform.velocity;body.inverse_mass=1.0f/80.0f;
    body.inverse_inertia=(NfVec3){.2f,.2f,.2f};
    body.radius=w->movement.radius;body.height=a->movement.body_height;
    if(!nf18a4_body_valid(&body)||w->collider_count>=NF_MAX_COLLIDERS)return false;
    const NfVec3 low=vsub(object.center,object.box_half);
    const NfVec3 high={object.center.x+object.box_half.x,
                       object.center.y+object.box_half.y,
                       object.center.z+object.box_half.z};
    const int collider=nf_world_add_collider(w,NF_COLLIDER_SOLID,low,high);
    if(collider<0)return false;
    memset(r,0,sizeof(*r));
    r->object_collider_index=(uint32_t)collider;
    nf18a5_integrated_init(&r->physical,cap,body,object);
    r->actor_id=id;r->model=(uint32_t)model;
    nf_camera_init(&r->camera,82.0f);
    r->physical.world.tick=(uint32_t)w->tick;
    r->last_commit_tick=(uint32_t)w->tick;
    r->last_hash=hash_body(&body,&object,r->last_commit_tick,r->physical.world.revision);
    return true;
}
/* Disjoint authority check: legacy static solids must not contradict the
   authoritative A5 voxel witnesses. Moving platforms/ramps are deliberately
   unsupported here rather than guessed to be stationary or non-solid. */
static Nf18a6Status legacy_geometry_check(const NfWorld *w,uint32_t mirror,NfVec3 feet,
    NfVec3 intended,Nf18a2ShapePolicy shape,float dt){
    if(w->ramp_count)return NF18A6_UNSUPPORTED;
    for(size_t i=0;i<w->collider_count;++i){
        if(i==mirror)continue; /* pair solver owns this dynamic material body */
        const NfCollider *c=&w->colliders[i];
        if(c->kind==NF_COLLIDER_LADDER)continue; /* affordance, not solid */
        if(c->kind==NF_COLLIDER_MOVING_PLATFORM)return NF18A6_UNSUPPORTED;
        Nf18aCollider box={0};box.body_id=(uint32_t)i+100000u;
        box.min=c->min;box.max=c->max;
        Nf18aContact hit={0};
        if(nf18a3_capsule_sweep(feet,shape,intended,box,dt,&hit) &&
           (hit.kind==NF18A_CONTACT_INITIAL_OVERLAP ||
           vdot(intended,hit.normal)<-1.0e-6f))return NF18A6_BLOCKED;
    }
    return NF18A6_COMMITTED;
}
typedef struct Nf18a6GateContext {
    const NfWorld *world;
    uint32_t mirror;
    Nf18a2ShapePolicy shape;
    float dt;
    NfVec3 start;
} Nf18a6GateContext;
static bool prepublish_world_gate(const Nf18a5Integrated *next,void *context){
    const Nf18a6GateContext *g=context;
    const NfWorld *w=g->world;
    const NfVec3 end={next->actor.center.x,
       next->actor.center.y-next->actor.height*.5f,next->actor.center.z};
    bool supported=false;
    for(size_t ci=0;ci<w->collider_count;++ci){
        if(ci==g->mirror)continue;
        const NfCollider *floor=&w->colliders[ci];
        const float foot=.10f;
        if(floor->kind==NF_COLLIDER_SOLID && fabsf(floor->max.y-end.y)<.001f &&
            floor->min.x+foot<=end.x&&floor->max.x-foot>=end.x&&
            floor->min.z+foot<=end.z&&floor->max.z-foot>=end.z){
            supported=true;break;
        }
    }
    if(!supported)return false;
    const NfVec3 actual=vsub(end,g->start);
    if(legacy_geometry_check(w,g->mirror,g->start,actual,g->shape,g->dt)!=NF18A6_COMMITTED)return false;
    /* World representation of the crate must not penetrate any other
       finite solid collider (apart from the ground contact at its base). */
    const NfVec3 low=vsub(next->object.center,next->object.box_half);
    const NfVec3 high={next->object.center.x+next->object.box_half.x,
                       next->object.center.y+next->object.box_half.y,
                       next->object.center.z+next->object.box_half.z};
    for(size_t ci=0;ci<w->collider_count;++ci){
        if(ci==g->mirror)continue;
        const NfCollider *ob=&w->colliders[ci];
        if(ob->kind!=NF_COLLIDER_SOLID)continue;
        if(low.x<ob->max.x-.0001f&&high.x>ob->min.x+.0001f &&
           low.y<ob->max.y-.0001f&&high.y>ob->min.y+.0001f &&
           low.z<ob->max.z-.0001f&&high.z>ob->min.z+.0001f)return false;
    }
    return true;
}
static bool invalid_nearby_actors(const NfWorld *w,NfEntityId own,NfVec3 feet){
    for(size_t i=0;i<NF_MAX_ENTITIES;++i){
        const NfActor *a=&w->actors[i];
        if(!a->active || a->id==own)continue;
        const NfVec3 d=vsub(feet,a->transform.position);
        if(vdot(d,d)<36.0f)return true; /* do not invent actor-actor solve */
    }
    return false;
}
Nf18a6Outcome nf18a6_step(Nf18a6Runtime *r,NfWorld *w,NfMoveInput in,
                      Nf18a5FineProvider loader,void *ctx,float dt,const char *journal){
    if(!r||!w)return outcome(NF18A6_INVALID,r);
    /* A bound world tick must be reached through its single dispatcher.
       Direct invocation while idle would bypass ownership and event ordering. */
    if(w->tick_owner && !w->tick_in_progress)
        return outcome(NF18A6_STALE,r);
    if(!isfinite(dt)||fabsf(dt-1.0f/(float)NF_TICK_RATE)>1.0e-6f||
       !isfinite(in.forward)||!isfinite(in.strafe)||!isfinite(in.yaw_radians))
        return outcome(NF18A6_INVALID,r);
    const NfActor *a=nf_world_find_actor_const(w,r->actor_id);
    if(!a||!a->combat.alive||!actor_consistent(a,&r->physical.actor)||
       r->model>NF18A6_WORLD_CAMERA)return outcome(NF18A6_STALE,r);
    if(r->object_collider_index>=w->collider_count ||
       w->colliders[r->object_collider_index].kind!=NF_COLLIDER_SOLID ||
       fabsf(w->colliders[r->object_collider_index].min.x-(r->physical.object.center.x-r->physical.object.box_half.x))>.0001f ||
       fabsf(w->colliders[r->object_collider_index].min.y-(r->physical.object.center.y-r->physical.object.box_half.y))>.0001f ||
       fabsf(w->colliders[r->object_collider_index].min.z-(r->physical.object.center.z-r->physical.object.box_half.z))>.0001f ||
       fabsf(w->colliders[r->object_collider_index].max.x-(r->physical.object.center.x+r->physical.object.box_half.x))>.0001f ||
       fabsf(w->colliders[r->object_collider_index].max.y-(r->physical.object.center.y+r->physical.object.box_half.y))>.0001f ||
       fabsf(w->colliders[r->object_collider_index].max.z-(r->physical.object.center.z+r->physical.object.box_half.z))>.0001f)
        return outcome(NF18A6_STALE,r);
    if(w->tick!=r->last_commit_tick||r->physical.world.tick!=r->last_commit_tick||
       r->last_commit_tick==UINT32_MAX)return outcome(NF18A6_STALE,r);
    const uint32_t tick=r->last_commit_tick+1u,version=r->physical.world.revision;
    const float f=fmaxf(-1.0f,fminf(1.0f,in.forward)),s=fmaxf(-1.0f,fminf(1.0f,in.strafe));
    float x=sinf(in.yaw_radians)*f+cosf(in.yaw_radians)*s;
    float z=cosf(in.yaw_radians)*f-sinf(in.yaw_radians)*s;
    const float mag=sqrtf(x*x+z*z);if(mag>1.0f){x/=mag;z/=mag;}
    const float speed=in.crouch_held?w->movement.crouch_speed:
                      in.sprint_held?w->movement.sprint_speed:w->movement.walk_speed;
    /* Scope guard: no unvalidated stance/jump/ladder mode transition here.
       A6 exercises grounded horizontal dynamic motion only; the 1.8A.3
       verifier must become the runtime authority for expanded traversal. */
    if(in.crouch_held||in.jump_pressed||in.interact_held||
       a->movement.mode==NF_MOVE_LADDER||a->movement.mode==NF_MOVE_VAULT||
       a->movement.mode==NF_MOVE_MANTLE||
       !a->movement.grounded)return outcome(NF18A6_UNSUPPORTED,r);
    const NfVec3 start=a->transform.position;
    if(invalid_nearby_actors(w,r->actor_id,start))return outcome(NF18A6_UNSUPPORTED,r);
    /* Standing requires a material support witness, not a cached bool. */
    bool material_support=false;
    for(size_t ci=0;ci<w->collider_count;++ci){
        if(ci==r->object_collider_index)continue;
        const NfCollider *floor=&w->colliders[ci];
        const float foot=.10f;
        if(floor->kind==NF_COLLIDER_SOLID && fabsf(floor->max.y-start.y)<.001f&&
           floor->min.x+foot<=start.x&&floor->max.x-foot>=start.x&&
           floor->min.z+foot<=start.z&&floor->max.z-foot>=start.z){
            material_support=true;break;
        }
    }
    if(!material_support)return outcome(NF18A6_UNSUPPORTED,r);
    const Nf18a2ShapePolicy shape={r->physical.actor.radius,r->physical.actor.height,
       .1f,w->movement.step_height,.7071067f};
    const float mass=1.0f/r->physical.actor.inverse_mass;
    Nf18a4Motor motor={{x*speed,r->physical.actor.velocity.y,z*speed},
       mass*w->movement.ground_accel,w->movement.ground_accel};
    const NfVec3 intended={motor.target_velocity.x*dt,motor.target_velocity.y*dt,motor.target_velocity.z*dt};
    const Nf18a6Status legacy=legacy_geometry_check(w,r->object_collider_index,start,intended,shape,dt);
    if(legacy!=NF18A6_COMMITTED){if(legacy==NF18A6_BLOCKED)++r->blocked;return outcome(legacy,r);}
    if(r->model==NF18A6_LEGACY_REFERENCE){
        /* Scientific control: actual old movement, but NO A5 physical truth. */
        NfActor trial=*a;trial.input=in;
        nf_movement_step_actor(w,&trial,dt);
        NfActor *dst=nf_world_find_actor(w,r->actor_id);
        if(!dst)return outcome(NF18A6_INVALID,r);
        *dst=trial;w->tick=tick;
        r->physical.actor.center=(NfVec3){trial.transform.position.x,
          trial.transform.position.y+r->physical.actor.height*.5f,trial.transform.position.z};
        r->physical.actor.velocity=trial.transform.velocity;
        r->physical.world.tick=tick;r->physical.world.revision++;
        r->last_commit_tick=tick;r->last_hash=hash_body(&r->physical.actor,
             &r->physical.object,tick,r->physical.world.revision);
        ++r->accepted;return outcome(NF18A6_COMMITTED,r);
    }
    Nf18a5Integrated next=r->physical;
    const Nf18a6GateContext gate={w,r->object_collider_index,shape,dt,start};
    Nf18a5CloseResult result=nf18a5_integrated_pair_step_checked(&next,version,tick,start,
                    loader,ctx,shape,motor,dt,0.0f,0.5f,journal,
                    prepublish_world_gate,(void *)&gate);
    if(result.status!=NF18A5_CLOSE_COMMITTED){
        Nf18a6Status st=NF18A6_PENDING;
        if(result.status==NF18A5_CLOSE_BLOCKED)st=NF18A6_BLOCKED;
        if(result.status==NF18A5_CLOSE_STALE)st=NF18A6_STALE;
        if(result.status==NF18A5_CLOSE_GATED)st=NF18A6_UNSUPPORTED;
        if(result.status==NF18A5_CLOSE_DISK_FAILED)st=NF18A6_DISK_FAILED;
        if(result.status==NF18A5_CLOSE_INVALID)st=NF18A6_INVALID;
        if(st==NF18A6_PENDING)++r->pending;
        if(st==NF18A6_BLOCKED)++r->blocked;
        return outcome(st,r);
    }
    /* Verify positions before publishing NfActor. Physically accepted body
       must be representable as the exact existing actor pose. */
    if(!finite3(next.actor.center)||!finite3(next.actor.velocity))
        return outcome(NF18A6_INVALID,r);
    if(r->model==NF18A6_LAB_ONLY){
        /* Scientific negative control: dynamics happened but no NfActor commit.
           Keep its next tick locally; do NOT claim live embodied state. */
        r->physical=next;r->last_commit_tick=tick;
        r->last_hash=hash_body(&next.actor,&next.object,tick,next.world.revision);
        ++r->accepted;return outcome(NF18A6_COMMITTED,r);
    }
    NfActor *dst=nf_world_find_actor(w,r->actor_id);
    if(!dst)return outcome(NF18A6_INVALID,r);
    r->physical=next;
    NfCollider *cr=&w->colliders[r->object_collider_index];
    cr->previous_min=cr->min;cr->previous_max=cr->max;
    cr->min=vsub(next.object.center,next.object.box_half);
    cr->max=(NfVec3){next.object.center.x+next.object.box_half.x,
       next.object.center.y+next.object.box_half.y,
       next.object.center.z+next.object.box_half.z};
    dst->transform.position=(NfVec3){next.actor.center.x,
            next.actor.center.y-next.actor.height*.5f,next.actor.center.z};
    dst->transform.velocity=next.actor.velocity;
    dst->input=in;dst->input.jump_pressed=false;
    dst->movement.mode=in.sprint_held?NF_MOVE_SPRINT:NF_MOVE_GROUND;
    dst->movement.grounded=true;
    dst->movement.body_height=next.actor.height;
    w->tick=tick;
    r->last_commit_tick=tick;
    r->last_hash=hash_body(&next.actor,&next.object,tick,next.world.revision);
    if(r->model==NF18A6_WORLD_CAMERA){
        nf_camera_follow_actor(&r->camera,dst,w,dt);
        nf_camera_step_presentation(&r->camera,dt);
    }
    ++r->accepted;
    Nf18a6Outcome out=outcome(NF18A6_COMMITTED,r);
    out.actual_impulse=result.dynamic_contact;out.cache_loads=(uint8_t)result.cache_loads;
    out.camera_updated=r->model==NF18A6_WORLD_CAMERA;
    return out;
}
bool nf18a6_recover(Nf18a6Runtime *r,NfWorld *w,const char *journal){
    if(!r||!w||!journal||r->object_collider_index>=w->collider_count)return false;
    Nf18a5Integrated restored={0};
    if(!nf18a5_restore(journal,&restored)||restored.actor.id!=r->actor_id||
       restored.object.id!=r->physical.object.id||
       restored.world.tick<=r->last_commit_tick||
       !nf18a4_body_valid(&restored.actor)||!nf18a4_body_valid(&restored.object))return false;
    NfActor *a=nf_world_find_actor(w,r->actor_id);
    if(!a||w->tick!=r->last_commit_tick)return false;
    r->physical=restored;
    a->transform.position=(NfVec3){restored.actor.center.x,
       restored.actor.center.y-restored.actor.height*.5f,restored.actor.center.z};
    a->transform.velocity=restored.actor.velocity;
    a->movement.body_height=restored.actor.height;
    NfCollider *cr=&w->colliders[r->object_collider_index];
    cr->min=vsub(restored.object.center,restored.object.box_half);
    cr->max=(NfVec3){restored.object.center.x+restored.object.box_half.x,
       restored.object.center.y+restored.object.box_half.y,
       restored.object.center.z+restored.object.box_half.z};
    cr->previous_min=cr->min;cr->previous_max=cr->max;
    w->tick=restored.world.tick;
    r->last_commit_tick=restored.world.tick;
    r->last_hash=hash_body(&restored.actor,&restored.object,
       restored.world.tick,restored.world.revision);
    nf_camera_follow_actor(&r->camera,a,w,1.f/NF_TICK_RATE);
    return true;
}
Nf18a6Snapshot nf18a6_snapshot(const Nf18a6Runtime *r,const NfWorld *w){
    Nf18a6Snapshot n={0};
    if(!r||!w)return n;
    const NfActor *a=nf_world_find_actor_const(w,r->actor_id);
    if(!a||!actor_consistent(a,&r->physical.actor))return n;
    n.tick=(uint32_t)w->tick;n.revision=r->physical.world.revision;
    n.material_epoch=r->physical.world.grid.global_revision;
    n.contact_digest=nf18a5_history_hash(&r->physical.world.history);
    n.actor_id=r->actor_id;n.state_hash=r->last_hash;
    n.feet=a->transform.position;n.velocity=a->transform.velocity;
    return n;
}
bool nf18a6_snapshot_matches(const Nf18a6Snapshot *a,const Nf18a6Snapshot *b){
    if(!a||!b||!a->tick||!b->tick)return false;
    return a->tick==b->tick&&a->revision==b->revision&&
       a->material_epoch==b->material_epoch&&a->contact_digest==b->contact_digest&&
       a->actor_id==b->actor_id&&a->state_hash==b->state_hash&&
       quant(a->feet.x)==quant(b->feet.x)&&quant(a->feet.y)==quant(b->feet.y)&&
       quant(a->feet.z)==quant(b->feet.z)&&quant(a->velocity.x)==quant(b->velocity.x)&&
       quant(a->velocity.y)==quant(b->velocity.y)&&quant(a->velocity.z)==quant(b->velocity.z);
}
const char *nf18a6_status_name(Nf18a6Status s){
    switch(s){case NF18A6_COMMITTED:return "COMMITTED";
        case NF18A6_BLOCKED:return "BLOCKED";
        case NF18A6_PENDING:return "PENDING";
        case NF18A6_STALE:return "STALE";
        case NF18A6_UNSUPPORTED:return "UNSUPPORTED";
        case NF18A6_INVALID:return "INVALID";
        case NF18A6_DISK_FAILED:return "DISK_FAILED";
        default:return "UNKNOWN";}
}


/* One-way or cross-target dependencies cannot create Purple. A validated
   two-way dependency remains scoped to its material target and dissolves with
   the tick; it never merges simulation domains or increases solver island size. */
Nf18a6TickTrace nf18a6_classify_tick(uint32_t tick,uint32_t red_target,
         uint32_t blue_target,bool red_to_blue,bool blue_to_red,
         uint32_t red_material_mask,uint32_t blue_material_mask){
    Nf18a6TickTrace tr={0};
    tr.tick=tick;tr.target_id=red_target;
    tr.red_proposed=red_to_blue?1u:0u;
    tr.blue_material_checked=blue_to_red?1u:0u;
    if(!tick||!red_target||red_target!=blue_target||!red_to_blue||
       !blue_to_red||!(red_material_mask&blue_material_mask))return tr;
    const uint32_t channel=red_material_mask&blue_material_mask;
    const Nf17cDomainDependency dep[2]={
        {NF17B_DOMAIN_ACTOR,NF17B_DOMAIN_MATERIAL,0,red_target,channel},
        {NF17B_DOMAIN_MATERIAL,NF17B_DOMAIN_ACTOR,0,red_target,channel}};
    Nf17cPurpleEnvelope env[NF17C_MAX_PURPLE_ENVELOPES]={0};
    const size_t n=nf17c_build_purple_envelopes(dep,2,tick,env,
                                              NF17C_MAX_PURPLE_ENVELOPES);
    for(size_t i=0;i<n&&i<NF17C_MAX_PURPLE_ENVELOPES;++i){
        const uint32_t both=(1u<<NF17B_DOMAIN_ACTOR)|
                            (1u<<NF17B_DOMAIN_MATERIAL);
        if((env[i].domain_mask&both)==both){
            tr.purple_present=1u;
            tr.purple_status=(uint8_t)env[i].status;
            tr.dependency_hash=env[i].dependency_hash;
            break;
        }
    }
    return tr;
}
static NfWorldTickStatus nf18a6_tick_dispatch(NfWorld *world,float dt,void *context){
    Nf18a6Runtime *runtime=(Nf18a6Runtime *)context;
    if(!world||!runtime)return NF_WORLD_TICK_INVALID;
    /* No remote actor is silently starved by a one-actor lab scheduler. */
    if(nf_world_active_actor_count(world)!=1u)return NF_WORLD_TICK_REJECTED;
    const NfActor *actor=nf_world_find_actor_const(world,runtime->actor_id);
    if(!actor)return NF_WORLD_TICK_REJECTED;
    const uint32_t candidate_tick=(uint32_t)(world->tick+1u);
    Nf18a6TickTrace trace={0};
    trace.tick=candidate_tick;trace.target_id=runtime->physical.object.id;
    trace.red_proposed=1u;trace.blue_material_checked=1u;
    const Nf18a6Outcome result=nf18a6_step(runtime,world,actor->input,
                              runtime->tick_provider,runtime->tick_provider_context,
                              dt,runtime->tick_journal);
    trace.embodiment_status=result.status;
    NfWorldTickStatus status=NF_WORLD_TICK_REJECTED;
    if(result.status==NF18A6_COMMITTED)status=NF_WORLD_TICK_COMMITTED;
    else if(result.status==NF18A6_PENDING)status=NF_WORLD_TICK_PENDING;
    if(status==NF_WORLD_TICK_COMMITTED&&result.actual_impulse){
        /* Reciprocal APPLIED impulse: actor and object are materially coupled.
           Do not infer Purple merely from proximity, wish, or a camera ray. */
        trace=nf18a6_classify_tick(candidate_tick,runtime->physical.object.id,
                runtime->physical.object.id,true,true,1u,1u);
        trace.embodiment_status=result.status;
        if(trace.purple_present)trace.purple_status=NF17C_PURPLE_COMMITTED;
    }
    trace.world_status=status;
    runtime->tick_trace=trace;
    return status;
}
bool nf18a6_bind_world_tick(NfWorld *world,Nf18a6Runtime *runtime,
             Nf18a5FineProvider provider,void *context,const char *journal){
    if(!world||!runtime||nf_world_active_actor_count(world)!=1u||
       runtime->model<NF18A6_WORLD_BRIDGE||
       runtime->model>NF18A6_WORLD_CAMERA||
       world->tick!=runtime->last_commit_tick||
       !nf_world_find_actor_const(world,runtime->actor_id))return false;
    if(!nf_world_bind_tick_owner(world,nf18a6_tick_dispatch,runtime))return false;
    runtime->tick_provider=provider;
    runtime->tick_provider_context=context;
    runtime->tick_journal=journal;
    memset(&runtime->tick_trace,0,sizeof(runtime->tick_trace));
    return true;
}
bool nf18a6_unbind_world_tick(NfWorld *world,Nf18a6Runtime *runtime){
    if(!world||!runtime||
       !nf_world_unbind_tick_owner(world,nf18a6_tick_dispatch,runtime))return false;
    runtime->tick_provider=NULL;
    runtime->tick_provider_context=NULL;
    runtime->tick_journal=NULL;
    return true;
}
