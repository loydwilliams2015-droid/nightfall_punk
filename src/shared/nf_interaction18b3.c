#include "nf_interaction18b3.h"
#include <math.h>
#include <string.h>

static NfVec3 add(NfVec3 a,NfVec3 b){return (NfVec3){a.x+b.x,a.y+b.y,a.z+b.z};}
static NfVec3 sub(NfVec3 a,NfVec3 b){return (NfVec3){a.x-b.x,a.y-b.y,a.z-b.z};}
static NfVec3 scale(NfVec3 a,float s){return (NfVec3){a.x*s,a.y*s,a.z*s};}
static float length(NfVec3 v){return sqrtf(v.x*v.x+v.y*v.y+v.z*v.z);}
static bool finite3(NfVec3 v){return isfinite(v.x)&&isfinite(v.y)&&isfinite(v.z);}
static Nf18b3Object *object(Nf18b3State *s,uint32_t id){
    for(size_t i=0;i<s->object_count;++i)if(s->objects[i].id==id)return &s->objects[i];
    return NULL;
}
static Nf18b2RegisteredObject *descriptor(Nf18b2EncounterRuntime *a,uint32_t id){
    for(size_t i=0;i<a->registry.object_count;++i)if(a->registry.objects[i].object.id==id)return &a->registry.objects[i];
    return NULL;
}
static Nf18b2Account *account(Nf18b2EncounterRuntime *a,uint32_t id){
    for(size_t i=0;i<a->registry.account_count;++i)if(a->registry.accounts[i].actor_id==id)return &a->registry.accounts[i];
    return NULL;
}
static bool terminal(Nf18b3Status s){return s!=NF18B3_PROPOSED&&s!=NF18B3_EXECUTING&&s!=NF18B3_PENDING;}
static bool valid_counts(const Nf18b3Runtime *r){return r&&r->state.object_count<=NF18B3_MAX_OBJECTS&&
    r->state.action_count<=NF18B3_MAX_ACTIONS&&r->state.event_count<=NF18B3_MAX_EVENTS&&
    (r->state.active==UINT32_MAX||r->state.active<r->state.action_count);}
static bool idle_boundary(const Nf18b3Runtime *r){return valid_counts(r)&&r->world&&!r->world->tick_in_progress;}
bool nf18b3_init(Nf18b3Runtime *r,NfWorld *w,Nf18a6Runtime *p,Nf18b2EncounterRuntime *a,double power,uint32_t salvage){
    if(!r||!w||!p||!a||w->tick_in_progress||a->world!=w||a->owner!=p||!a->attached||a->queue_count||
       w->tick_owner_context!=p||!w->tick_owner||p->owner_hooks.context!=a||p->tick_journal||
       a->publication_validator||(a->model!=NF18B2_M3_EXHAUSTIVE&&a->model!=NF18B2_M4_MATERIAL_FIRST)||
       !isfinite(power)||power<0)return false;
    memset(r,0,sizeof(*r));r->world=w;r->owner=p;r->authority=a;r->model=a->model;
    r->physical_owner=w->tick_owner;r->state.active=UINT32_MAX;r->state.next_event=1;
    r->state.power_enabled=1;r->state.power_joules=power;r->state.salvage_units=salvage;return true;
}
bool nf18b3_register(Nf18b3Runtime *r,Nf18b3Object o,Nf18b2Object d){
    if(!idle_boundary(r)||r->state.active!=UINT32_MAX||!o.id||o.id!=d.id||!o.revision||
       o.kind<NF18B3_DOOR_OPEN||o.kind>NF18B3_REPAIR||!nf18a4_body_valid(&o.body)||o.body.id!=o.id||
       o.body.box_half.x<=0||o.body.box_half.y<=0||o.body.box_half.z<=0||
       !finite3(o.origin)||!isfinite(o.travel)||o.travel<=0||o.travel>2||
       !isfinite(o.speed)||o.speed<=0||o.speed>2||!isfinite(o.max_force)||o.max_force<=0||
       !isfinite(o.max_carry_mass)||o.max_carry_mass<=0||o.max_health>1000||o.health>o.max_health||
       object(&r->state,o.id)||r->state.object_count==NF18B3_MAX_OBJECTS||descriptor(r->authority,o.id))return false;
    const bool shared=o.id==r->owner->physical.object.id;
    if((o.kind==NF18B3_PUSH||o.kind==NF18B3_CARRY)!=shared)return false;
    if(!shared&&r->world->collider_count==NF_MAX_COLLIDERS)return false;
    if(!nf18b2_register_object(r->authority,d,r->owner->physical.world.grid.global_revision,0,0))return false;
    o.collider_index=shared?r->owner->object_collider_index:(uint32_t)nf_world_add_collider(r->world,NF_COLLIDER_SOLID,
        sub(o.body.center,o.body.box_half),add(o.body.center,o.body.box_half));
    o.available=1;r->state.objects[r->state.object_count++]=o;return true;
}
static NfWorldTickStatus owner_tick(NfWorld *,float,void *);
bool nf18b3_bind(Nf18b3Runtime *r){
    if(!idle_boundary(r)||r->bound||r->world->tick_owner!=r->physical_owner||
       r->world->tick_owner_context!=r->owner)return false;
    if(!nf_world_unbind_tick_owner(r->world,r->physical_owner,r->owner))return false;
    if(!nf_world_bind_tick_owner(r->world,owner_tick,r)){
        (void)nf_world_bind_tick_owner(r->world,r->physical_owner,r->owner);return false;
    }r->bound=1;return true;
}
Nf18b3Status nf18b3_request(Nf18b3Runtime *r,uint32_t actor,uint32_t target,uint32_t action,Nf18b3Kind kind){
    if(!idle_boundary(r)||!r->bound||actor!=r->owner->actor_id||!action||kind<NF18B3_DOOR_OPEN||kind>NF18B3_TRAVERSAL)return NF18B3_INVALID;
    if(kind==NF18B3_TRAVERSAL)return NF18B3_UNSUPPORTED;
    for(size_t i=0;i<r->state.action_count;++i)if(r->state.actions[i].actor_id==actor&&r->state.actions[i].action_id==action)
        if(r->state.actions[i].published||r->state.active==i)return NF18B3_DUPLICATE;
    Nf18b3Object *o=object(&r->state,target);
    if(!o||(kind!=o->kind&&!(kind==NF18B3_DOOR_CLOSE&&o->kind==NF18B3_DOOR_OPEN)&&
       !(kind==NF18B3_CARRY&&o->kind==NF18B3_PUSH)))return NF18B3_INVALID;
    if(!o->available)return NF18B3_BLOCKED;
    if(r->state.active!=UINT32_MAX||r->state.action_count==NF18B3_MAX_ACTIONS||
       r->state.event_count==NF18B3_MAX_EVENTS)return NF18B3_PENDING;
    const size_t trace_before=r->authority->trace_count;
    Nf18b2Status status=nf18b2_propose(r->authority,(Nf18b2Intent){actor,target,action,kind==NF18B3_PUSH?NF18B2_PUSH:NF18B2_USE});
    if(r->authority->trace_count==trace_before)return status==NF18B2_DUPLICATE?NF18B3_DUPLICATE:
        status==NF18B2_PENDING?NF18B3_PENDING:NF18B3_INVALID;
    Nf18b3Action *a=&r->state.actions[r->state.action_count];memset(a,0,sizeof(*a));
    a->actor_id=actor;a->object_id=target;a->action_id=action;a->kind=kind;a->status=NF18B3_PROPOSED;
    a->start_tick=(uint32_t)r->world->tick;a->authorization_trace=(uint32_t)trace_before;
    a->expected_object_revision=descriptor(r->authority,target)->object.revision;
    a->contract_epoch=r->authority->registry.contract_epoch;
    r->state.active=(uint32_t)r->state.action_count++;
    return status==NF18B2_ELIGIBLE?NF18B3_PROPOSED:status==NF18B2_PENDING?NF18B3_PENDING:NF18B3_BLOCKED;
}
bool nf18b3_cancel(Nf18b3Runtime *r,uint32_t actor,uint32_t action,bool release){
    if(!idle_boundary(r)||r->state.active==UINT32_MAX)return false;
    Nf18b3Action *a=&r->state.actions[r->state.active];
    if(a->actor_id!=actor||a->action_id!=action)return false;
    a->cancel_requested=1;a->release_requested=release&&a->kind==NF18B3_CARRY;return true;
}
static bool append(Nf18b3State *s,Nf18b3Event e){
    if(s->event_count==NF18B3_MAX_EVENTS||s->next_event==UINT64_MAX)return false;
    e.sequence=s->next_event++;s->events[s->event_count++]=e;return true;
}
static bool release_slot(Nf18b3Runtime *r,Nf18b3Action *a){
    Nf18b2RegisteredObject *d=descriptor(&r->trial_authority,a->object_id);
    if(!d||d->object.revision==UINT32_MAX||r->trial_authority.registry.revision==UINT32_MAX)return false;
    for(unsigned i=0;i<d->object.occupant_count;++i)if(d->object.occupants[i]==a->actor_id){
        memmove(&d->object.occupants[i],&d->object.occupants[i+1],(d->object.occupant_count-i-1)*sizeof(uint32_t));
        --d->object.occupant_count;++d->object.revision;++r->trial_authority.registry.revision;break;
    }
    Nf18b2EncounterTrace *t=&r->trial_authority.traces[a->authorization_trace];
    if(a->kind==NF18B3_CARRY)object(&r->staged,a->object_id)->carried_by=0;
    t->state=a->status==NF18B3_COMPLETED?NF18B2_COMPLETED:
        a->motor_receipts||a->contact_receipts||a->material_effects?NF18B2_COMPENSATION_REQUIRED:NF18B2_CANCELLED;
    if(t->transition_count<12)t->transitions[t->transition_count++]=t->state;
    r->staged.active=UINT32_MAX;a->completion_tick=(uint32_t)r->trial_world.tick;return true;
}
static Nf18b3Status query(Nf18b3Runtime *r,Nf18a4Body b,NfVec3 from,NfVec3 to,uint32_t skip){
    /* Circumscribed capsule conservatively encloses the axis-aligned box. */
    float radius=nextafterf((float)hypot((double)b.box_half.x,(double)b.box_half.z),INFINITY);
    const float extent=b.box_half.y+radius;
    Nf18a2ShapePolicy shape={radius,2*extent,0,0,1};
    NfVec3 feet={from.x,from.y-extent,from.z},delta=sub(to,from);
    ++r->material_queries;
    Nf18a5Query material=nf18a5_sweep_query(&r->trial_owner.physical.world.grid,feet,delta,shape,1.f/60,
        (uint32_t)r->trial_world.tick,NULL,NULL,NULL);
    if(material!=NF18A5_Q_FREE)return material==NF18A5_Q_PENDING?NF18B3_PENDING:
        material==NF18A5_Q_SOLID?NF18B3_JAMMED:NF18B3_INVALID;
    NfVec3 lo={fminf(from.x,to.x)-radius,fminf(from.y,to.y)-extent,fminf(from.z,to.z)-radius};
    NfVec3 hi={fmaxf(from.x,to.x)+radius,fmaxf(from.y,to.y)+extent,fmaxf(from.z,to.z)+radius};
    for(size_t i=0;i<r->trial_world.collider_count+1;++i){
        if(i==skip)continue;
        Nf18aCollider c={0};
        if(i==r->trial_world.collider_count){
            Nf18a4Body actor=r->trial_owner.physical.actor;c.body_id=actor.id;
            if(b.id==actor.id)continue; /* The modeled hand belongs to this actor. */
            c.min=(NfVec3){actor.center.x-actor.radius,actor.center.y-actor.height*.5f,actor.center.z-actor.radius};
            c.max=(NfVec3){actor.center.x+actor.radius,actor.center.y+actor.height*.5f,actor.center.z+actor.radius};
        }else{
            NfCollider src=r->trial_world.colliders[i];if(src.kind==NF_COLLIDER_LADDER)continue;
            if(src.kind==NF_COLLIDER_MOVING_PLATFORM)return NF18B3_UNSUPPORTED;
            c.body_id=(uint32_t)i+100000;c.min=src.min;c.max=src.max;
        }
        if(r->model==NF18B2_M4_MATERIAL_FIRST&&(hi.x<c.min.x||lo.x>c.max.x||hi.y<c.min.y||lo.y>c.max.y||hi.z<c.min.z||lo.z>c.max.z))continue;
        ++r->geometry_queries;Nf18aContact hit={0};
        if(nf18a3_capsule_sweep(feet,shape,delta,c,1.f/60,&hit)&&
           (hit.kind==NF18A_CONTACT_INITIAL_OVERLAP||delta.x*hit.normal.x+delta.y*hit.normal.y+delta.z*hit.normal.z<-1e-6f))return NF18B3_JAMMED;
    }return NF18B3_EXECUTING;
}
static bool sync_object(Nf18b3Runtime *r,Nf18b3Object *o,NfVec3 prior){
    if(o->revision==UINT32_MAX)return false;
    Nf18b2RegisteredObject *d=descriptor(&r->trial_authority,o->id);
    if(!d||d->object.revision==UINT32_MAX||r->trial_authority.registry.revision==UINT32_MAX)return false;
    if(memcmp(&prior,&o->body.center,sizeof(prior))){
        if(!d->follows_physical_object)d->object.target=add(d->object.target,sub(o->body.center,prior));
        ++o->revision;++d->object.revision;++r->trial_authority.registry.revision;
    }
    NfCollider *c=&r->trial_world.colliders[o->collider_index];c->previous_min=c->min;c->previous_max=c->max;
    c->min=sub(o->body.center,o->body.box_half);c->max=add(o->body.center,o->body.box_half);return true;
}
static Nf18b3Status manipulate(Nf18b3Runtime *r,Nf18b3Action *a,Nf18b3Object *o,Nf18a5Sample *support,size_t *support_count){
    Nf18b3State *s=&r->staged;const float dt=1.f/60;
    Nf18b2Account *bank=account(&r->trial_authority,a->actor_id);
    Nf18b3Event e={.tick=(uint32_t)r->trial_world.tick,.actor_id=a->actor_id,.object_id=o->id,.action_id=a->action_id,.kind=a->kind};
    e.before=o->body.center;
    if(o->rotated||length(o->body.omega)>1e-5f)return NF18B3_UNSUPPORTED;
    if(o->jammed||(o->requires_power&&!s->power_enabled))return NF18B3_JAMMED;
    if(a->kind==NF18B3_PUSH){
        o->body=r->trial_owner.physical.object;e.after=o->body.center;
        e.contact=r->trial_owner.last_applied_contact;
        if(e.contact.impulses_applied&&e.contact.tick==r->trial_world.tick){
            ++a->contact_receipts;if(!append(s,e))return NF18B3_PENDING;return NF18B3_COMPLETED;
        }return NF18B3_EXECUTING;
    }
    if(a->kind==NF18B3_PICKUP){
        NfVec3 actor=nf_world_find_actor_const(&r->trial_world,a->actor_id)->transform.position;
        NfVec3 hand={actor.x+.8f,actor.y+1.1f,actor.z};
        if(length(sub(o->body.center,hand))>.25f)return NF18B3_JAMMED;
        NfVec3 eye={actor.x,actor.y+nf_world_find_actor_const(&r->trial_world,a->actor_id)->movement.eye_height,actor.z};
        Nf18a4Body hand_body={.id=a->actor_id,.box_half={.04f,.04f,.04f}};
        Nf18b3Status hand_clear=query(r,hand_body,eye,hand,o->collider_index);
        if(hand_clear!=NF18B3_EXECUTING)return hand_clear;
        if(!o->units||o->units>INT32_MAX||UINT32_MAX-s->inventory_units<o->units)return NF18B3_BLOCKED;
        e.after=e.before;e.inventory_delta=(int32_t)o->units;e.ownership_transfer=1;
        if(!append(s,e))return NF18B3_PENDING;
        s->inventory_units+=o->units;o->units=0;o->available=0;++a->material_effects;
        const uint32_t removed=o->collider_index;
        memmove(&r->trial_world.colliders[removed],&r->trial_world.colliders[removed+1],
            (r->trial_world.collider_count-removed-1)*sizeof(NfCollider));--r->trial_world.collider_count;
        for(size_t i=0;i<s->object_count;++i)if(s->objects[i].collider_index>removed&&s->objects[i].collider_index!=UINT32_MAX)--s->objects[i].collider_index;
        if(r->trial_owner.object_collider_index>removed)--r->trial_owner.object_collider_index;
        o->collider_index=UINT32_MAX;descriptor(&r->trial_authority,o->id)->object.preconditions=0;return NF18B3_COMPLETED;
    }
    if(a->kind==NF18B3_REPAIR&&(!o->max_health||o->health==o->max_health))return NF18B3_COMPLETED;
    if(a->kind==NF18B3_REPAIR&&(!s->salvage_units||!bank->balance))return NF18B3_BLOCKED;
    NfVec3 target=add(o->origin,(NfVec3){a->kind==NF18B3_DOOR_CLOSE||a->return_stroke||(a->kind==NF18B3_SWITCH&&o->on)?0:o->travel,0,0});
    Nf18a4Body body=o->body;
    if(a->kind==NF18B3_CARRY){
        body=r->trial_owner.physical.object;
        if(!body.inverse_mass||1.f/body.inverse_mass>o->max_carry_mass||9.81f/body.inverse_mass>o->max_force)return NF18B3_JAMMED;
        /* Explicit external weight field for the carried assembly. A6 outside
           this scoped interaction does not promise general gravity/falling. */
        e.external_object_impulse=(NfVec3){0,-9.81f*dt/body.inverse_mass,0};
        body.velocity.y-=9.81f*dt;
        NfVec3 feet=nf_world_find_actor_const(&r->trial_world,a->actor_id)->transform.position;
        target=(NfVec3){feet.x+.8f,feet.y+1.1f,feet.z};
    }
    NfVec3 displacement=sub(target,body.center);const float distance=length(displacement);
    if(!isfinite(distance)||distance>10)return NF18B3_INVALID;
    const float accel=fminf(20,o->max_force*body.inverse_mass);
    const float safe_speed=sqrtf(accel*accel*dt*dt+2*accel*distance)-accel*dt;
    NfVec3 velocity=distance>.0001f?scale(displacement,fminf(o->speed,fminf(distance/dt,safe_speed))/distance):(NfVec3){0};
    Nf18a4MotorReceipt mr={0};
    if(!nf18a4_motor_drive(&body,(Nf18a4Motor){velocity,o->max_force,20},dt,&mr))return NF18B3_INVALID;
    NfVec3 next=a->kind==NF18B3_CARRY?body.center:add(body.center,scale(body.velocity,dt));
    Nf18b3Status clearance=query(r,body,e.before,next,o->collider_index);
    if(clearance!=NF18B3_EXECUTING)return clearance;
    const double debit=fmax(0,(double)mr.work_joules);
    if(!isfinite(debit)||s->power_joules<debit)return NF18B3_BLOCKED;
    body.center=next;e.after=next;e.motor=mr;
    if(a->kind==NF18B3_CARRY){
        /* The A6 pair already integrated this tick's position. Apply this
           tick's hand force to velocity, not a second position integration. */
        Nf18a4Body actor=r->trial_owner.physical.actor;
        e.external_actor_impulse=(NfVec3){0,-9.81f*dt/actor.inverse_mass,0};
        actor.velocity.y-=9.81f*dt;
        actor.velocity=sub(actor.velocity,scale(mr.impulse,actor.inverse_mass));
        if(actor.velocity.y<0){
            const NfVec3 feet={actor.center.x,actor.center.y-actor.height*.5f,actor.center.z};
            bool found=false;
            for(size_t i=0;i<r->trial_world.collider_count;++i){
                NfCollider c=r->trial_world.colliders[i];
                if(c.kind!=NF_COLLIDER_SOLID||i==o->collider_index||fabsf(c.max.y-feet.y)>.0001f||
                   feet.x-actor.radius<c.min.x||feet.x+actor.radius>c.max.x||feet.z-actor.radius<c.min.z||feet.z+actor.radius>c.max.z)continue;
                Nf18a4Body floor={.id=(uint32_t)i+100000,.center=scale(add(c.min,c.max),.5f),.box_half=scale(sub(c.max,c.min),.5f)};
                Nf18a4Constraint contact={actor.id,floor.id,floor.id,{0,1,0},feet,0,0};
                e.contact=nf18a4_apply_contact(&actor,&floor,contact,NF18A4_FRICTION_ADAPTIVE,(uint32_t)r->trial_world.tick,r->before_owner.physical.world.revision);
                if((e.contact.status==NF18A4_ACCEPTED||e.contact.status==NF18A4_SEPARATING)&&fabsf(actor.velocity.y)<1e-6f)actor.velocity.y=0;
                if(e.contact.impulses_applied){
                    /* The verified grounded constraint may remove roundoff;
                       history still requires a genuinely APPLIED impulse. */
                    if(!nf18a5_receipt_sample(&e.contact,actor.id,floor.id,0,r->trial_owner.physical.world.grid.global_revision,&support[*support_count]))return NF18B3_INVALID;
                    support[*support_count].resting_force=length(e.contact.normal_impulse)/dt;
                    support[*support_count].normal_impulse=0;++*support_count;++a->contact_receipts;
                }found=true;break;
            }if(!found)return NF18B3_UNSUPPORTED;
        }
        r->trial_owner.physical.actor=actor;
        nf_world_find_actor(&r->trial_world,a->actor_id)->transform.velocity=actor.velocity;
        r->trial_owner.physical.object=body;o->carried_by=a->actor_id;
    }
    const bool applied=length(mr.impulse)>0||memcmp(&e.before,&e.after,sizeof(e.before))!=0;
    if(applied){if(!append(s,e))return NF18B3_PENDING;
        s->power_joules-=debit;s->spent_joules+=debit;++a->motor_receipts;a->motor_work+=mr.work_joules;}
    o->body=body;if(!sync_object(r,o,e.before))return NF18B3_PENDING;
    if(a->kind==NF18B3_CARRY)return NF18B3_EXECUTING;
    if(length(sub(target,body.center))>.0001f||length(body.velocity)>.0001f)return NF18B3_EXECUTING;
    if(a->kind==NF18B3_REPAIR){
        if(!a->return_stroke){
            if(bank->revision==UINT32_MAX||r->trial_authority.registry.revision==UINT32_MAX)return NF18B3_PENDING;
            --s->salvage_units;--bank->balance;++bank->revision;++r->trial_authority.registry.revision;
            ++o->health;++a->material_effects;
            Nf18b3Event repair=e;repair.motor=(Nf18a4MotorReceipt){0};repair.health_delta=1;
            if(!append(s,repair))return NF18B3_PENDING;
        }a->return_stroke=!a->return_stroke;
        return o->health==o->max_health?NF18B3_COMPLETED:NF18B3_EXECUTING;
    }
    if(a->kind==NF18B3_SWITCH){
        o->on=!o->on;s->power_enabled=o->on;++a->material_effects;
        Nf18b3Event switched=e;switched.before=switched.after;switched.motor=(Nf18a4MotorReceipt){0};
        switched.switch_changed=1;switched.switch_state=o->on;switched.power_state=s->power_enabled;
        if(!append(s,switched))return NF18B3_PENDING;
    }
    return NF18B3_COMPLETED;
}
static void failed(Nf18b3Runtime *r,Nf18b3Status status){
    if(r->state.active==UINT32_MAX)return;
    Nf18b3Action *a=&r->state.actions[r->state.active];
    a->stop_reason=status;
    /* A retryable storage failure retains an already published action/lease.
       Initial failures require a fresh request/witness and release the queue. */
    if(a->published){a->status=NF18B3_PENDING;return;}
    a->status=status;r->state.active=UINT32_MAX;
    Nf18b2EncounterTrace *t=&r->authority->traces[a->authorization_trace];
    t->state=NF18B2_FAILED_NO_EFFECT;t->physical_status=NF18A6_PENDING;r->authority->queue_count=0;
}
static NfWorldTickStatus owner_tick(NfWorld *w,float dt,void *ctx){
    Nf18b3Runtime *r=ctx;++r->owner_calls;
    if(!valid_counts(r)||w!=r->world||!w->tick_in_progress||fabsf(dt-1.f/60)>1e-6f||nf_world_active_actor_count(w)!=1)return NF_WORLD_TICK_REJECTED;
    if(r->owner->tick_journal){failed(r,NF18B3_PENDING);return NF_WORLD_TICK_PENDING;}
    if(r->state.active!=UINT32_MAX){
        Nf18b3Action *initial=&r->state.actions[r->state.active];
        Nf18b3Object *target=object(&r->state,initial->object_id);
        /* A known failed first manipulation must not first execute an actor
           motor that can push its target through the underlying A4 pair. */
        if(!initial->published&&!initial->cancel_requested&&target&&
           (target->rotated||length(target->body.omega)>1e-5f||target->jammed)){
            failed(r,target->rotated||length(target->body.omega)>1e-5f?NF18B3_UNSUPPORTED:NF18B3_JAMMED);
            return NF_WORLD_TICK_REJECTED;
        }
    }
    r->before_world=*w;r->before_owner=*r->owner;r->before_registry=r->authority->registry;r->before_state=r->state;
    r->trial_world=*w;r->trial_world.tick_in_progress=false;r->trial_world.tick_owner=r->physical_owner;
    r->trial_owner=*r->owner;r->trial_world.tick_owner_context=&r->trial_owner;
    r->trial_authority=*r->authority;r->trial_authority.world=&r->trial_world;r->trial_authority.owner=&r->trial_owner;
    r->trial_owner.owner_hooks.context=&r->trial_authority;r->staged=r->state;
    Nf18b3Action *a=r->staged.active==UINT32_MAX?NULL:&r->staged.actions[r->staged.active];
    if(a&&a->cancel_requested&&!a->published){
        (void)nf18b2_withdraw(&r->trial_authority,a->actor_id,a->action_id);
        a->status=NF18B3_CANCELLED;r->state=r->staged;r->state.active=UINT32_MAX;
        r->authority->queue_count=0;r->authority->traces[a->authorization_trace].state=NF18B2_CANCELLED;return NF_WORLD_TICK_REJECTED;
    }
    NfWorldTickStatus physical=nf_world_step_checked(&r->trial_world,dt);
    if(physical!=NF_WORLD_TICK_COMMITTED){
        Nf18a6Status reason=r->trial_owner.tick_trace.embodiment_status;
        failed(r,physical==NF_WORLD_TICK_PENDING?NF18B3_PENDING:reason==NF18A6_STALE?NF18B3_EXPIRED:
            reason==NF18A6_INVALID?NF18B3_INVALID:NF18B3_BLOCKED);return physical;
    }
    Nf18a5Sample samples[2];size_t count=0;
    const Nf18a4Receipt rc=r->trial_owner.last_applied_contact;
    if(rc.impulses_applied&&rc.tick==r->trial_world.tick){
        if(!nf18a5_receipt_sample(&rc,r->trial_owner.actor_id,r->trial_owner.physical.object.id,0,
            r->trial_owner.physical.world.grid.global_revision,&samples[count++]))return NF_WORLD_TICK_REJECTED;
    }
    if(a){
        Nf18b3Object *o=object(&r->staged,a->object_id);Nf18b2RegisteredObject *d=descriptor(&r->trial_authority,a->object_id);
        if(!o||!d)return NF_WORLD_TICK_REJECTED;
        if(!a->published){a->published=r->trial_authority.traces[a->authorization_trace].published;
            if(!a->published){failed(r,NF18B3_BLOCKED);return NF_WORLD_TICK_REJECTED;}}
        bool held=false;for(unsigned i=0;i<d->object.occupant_count;++i)held|=d->object.occupants[i]==a->actor_id;
        Nf18b3Status status=NF18B3_EXECUTING;
        if(a->cancel_requested){status=a->release_requested?NF18B3_COMPLETED:
            a->motor_receipts||a->contact_receipts||a->material_effects?NF18B3_COMPENSATION_REQUIRED:NF18B3_CANCELLED;o->carried_by=0;}
        else if(!held||a->contract_epoch!=r->trial_authority.registry.contract_epoch||
            (r->state.actions[r->state.active].published&&d->object.revision!=a->expected_object_revision))status=NF18B3_EXPIRED;
        else{
            Nf18b2LiveFrame frame;
            if(!nf18b2_live_frame(&r->trial_authority,&frame))return NF_WORLD_TICK_REJECTED;
            Nf18b2Account *bank=account(&r->trial_authority,a->actor_id);
            frame.scene.geometry.tick=(uint32_t)r->trial_world.tick;
            Nf18b2Use witness={a->actor_id,a->object_id,a->action_id,(uint32_t)r->trial_world.tick,
                r->trial_owner.physical.world.revision,r->trial_owner.physical.world.grid.global_revision,bank->revision,d->object.revision};
            Nf18b2UseDecision eligibility=nf18b2_evaluate(&frame.scene,witness,r->model);
            if(eligibility.status!=NF18B2_ELIGIBLE)status=eligibility.status==NF18B2_PENDING?NF18B3_PENDING:NF18B3_BLOCKED;
            else status=manipulate(r,a,o,samples,&count);
        }
        if(status==NF18B3_PENDING){failed(r,status);return NF_WORLD_TICK_PENDING;}
        a->stop_reason=status;
        a->status=terminal(status)&&status!=NF18B3_COMPLETED&&
            (a->motor_receipts||a->contact_receipts||a->material_effects)?NF18B3_COMPENSATION_REQUIRED:status;
        if(terminal(status)&&!release_slot(r,a)){failed(r,NF18B3_PENDING);return NF_WORLD_TICK_PENDING;}
    }
    Nf18a5History history=r->before_owner.physical.world.history;
    if(!nf18a5_history_commit(&history,(uint32_t)r->trial_world.tick,r->trial_owner.physical.world.revision,
        r->trial_owner.actor_id,samples,count)){failed(r,NF18B3_PENDING);return NF_WORLD_TICK_PENDING;}
    r->trial_owner.physical.world.history=history;
    r->trial_owner.physical.committed_receipts=r->before_owner.physical.committed_receipts+(uint32_t)count;
    for(size_t i=0;i<r->staged.object_count;++i){
        Nf18b3Object *o=&r->staged.objects[i];
        if(o->id==r->trial_owner.physical.object.id){
            NfVec3 prior=o->body.center;o->body=r->trial_owner.physical.object;
            if(!sync_object(r,o,prior)){failed(r,NF18B3_PENDING);return NF_WORLD_TICK_PENDING;}
        }
    }
    r->trial_owner.last_hash=nf18a6_current_body_hash(&r->trial_owner);
    for(size_t i=0;i<r->trial_authority.trace_count;++i){
        Nf18b2EncounterTrace *trace=&r->trial_authority.traces[i];
        if(trace->published){trace->history_after=nf18a5_history_hash(&history);
            trace->balance_after=account(&r->trial_authority,trace->intent.actor_id)->balance;}
    }
    Nf18b2LiveFrame refresh;
    if(!nf18b2_live_frame(&r->trial_authority,&refresh)){failed(r,NF18B3_PENDING);return NF_WORLD_TICK_PENDING;}
    if(a&&!terminal(a->status))a->expected_object_revision=descriptor(&r->trial_authority,a->object_id)->object.revision;
    if(r->publication_validator&&!r->publication_validator(&r->trial_world,&r->staged,r->publication_context)){
        failed(r,NF18B3_PUBLISH_FAILED);return NF_WORLD_TICK_REJECTED;}
    if(memcmp(w,&r->before_world,sizeof(*w))||memcmp(&r->owner->physical,&r->before_owner.physical,sizeof(r->owner->physical))||
       memcmp(&r->authority->registry,&r->before_registry,sizeof(r->before_registry))||memcmp(&r->state,&r->before_state,sizeof(r->state))){
        failed(r,NF18B3_EXPIRED);return NF_WORLD_TICK_REJECTED;}
    /* The only LIVE publication, under the existing dispatcher. No journal or
       callback can fail between these prevalidated single-thread assignments. */
    r->trial_world.tick_owner=owner_tick;r->trial_world.tick_owner_context=r;r->trial_world.tick_in_progress=true;
    *w=r->trial_world;*r->owner=r->trial_owner;*r->authority=r->trial_authority;r->state=r->staged;
    r->authority->world=w;r->authority->owner=r->owner;r->owner->owner_hooks.context=r->authority;return NF_WORLD_TICK_COMMITTED;
}
bool nf18b3_observe(const Nf18b3Runtime *r,size_t index,Nf18b3Action *out){
    if(!r||!out||index>=r->state.action_count)return false;
    *out=r->state.actions[index];return true;
}
bool nf18b3_ack_events(Nf18b3Runtime *r,uint64_t through){
    if(!idle_boundary(r)||through<=r->state.acknowledged_event)return false;
    size_t n=0;while(n<r->state.event_count&&r->state.events[n].sequence<=through)++n;
    if(!n||r->state.events[n-1].sequence!=through)return false;
    memmove(r->state.events,r->state.events+n,(r->state.event_count-n)*sizeof(Nf18b3Event));
    r->state.event_count-=n;r->state.acknowledged_event=through;return true;
}
const char *nf18b3_status_name(Nf18b3Status s){
    static const char *const names[]={"PROPOSED","EXECUTING","COMPLETED","BLOCKED","PENDING","EXPIRED","JAMMED","CANCELLED",
        "COMPENSATION_REQUIRED","UNSUPPORTED","INVALID","DUPLICATE","PUBLISH_FAILED"};
    return s>=NF18B3_PROPOSED&&s<=NF18B3_PUBLISH_FAILED?names[s]:"INVALID";
}
static void hash_u32(uint32_t *h,uint32_t v){*h^=v;*h*=16777619u;}
static void hash_float(uint32_t *h,float f){uint32_t v;memcpy(&v,&f,sizeof(v));hash_u32(h,v);}
static void hash_double(uint32_t *h,double f){uint64_t v;memcpy(&v,&f,sizeof(v));hash_u32(h,(uint32_t)v);hash_u32(h,(uint32_t)(v>>32));}
static void hash_vec(uint32_t *h,NfVec3 v){hash_float(h,v.x);hash_float(h,v.y);hash_float(h,v.z);}
uint32_t nf18b3_semantic_hash(const Nf18b3Runtime *r){
    if(!r)return 0;
    uint32_t hash=2166136261u;
    const Nf18b3State *s=&r->state;
    hash_u32(&hash,(uint32_t)s->object_count);hash_u32(&hash,(uint32_t)s->action_count);hash_u32(&hash,(uint32_t)s->event_count);
    hash_u32(&hash,s->active);hash_u32(&hash,s->inventory_units);hash_u32(&hash,s->salvage_units);hash_u32(&hash,s->power_enabled);
    hash_double(&hash,s->power_joules);hash_double(&hash,s->spent_joules);
    for(size_t i=0;i<s->object_count;++i){const Nf18b3Object *o=&s->objects[i];
        hash_u32(&hash,o->id);hash_u32(&hash,o->revision);hash_u32(&hash,o->collider_index);
        hash_vec(&hash,o->body.center);hash_vec(&hash,o->body.velocity);hash_vec(&hash,o->body.omega);
        hash_u32(&hash,o->units);hash_u32(&hash,o->health);hash_u32(&hash,o->carried_by);hash_u32(&hash,o->available);hash_u32(&hash,o->on);
    }
    for(size_t i=0;i<s->action_count;++i){const Nf18b3Action *a=&s->actions[i];
        hash_u32(&hash,a->action_id);hash_u32(&hash,a->status);hash_u32(&hash,a->stop_reason);hash_u32(&hash,a->published);
        hash_u32(&hash,a->completion_tick);hash_u32(&hash,a->motor_receipts);hash_u32(&hash,a->contact_receipts);
        hash_u32(&hash,a->material_effects);hash_double(&hash,a->motor_work);
    }
    for(size_t i=0;i<s->event_count;++i){const Nf18b3Event *e=&s->events[i];
        hash_u32(&hash,(uint32_t)e->sequence);hash_u32(&hash,e->tick);hash_u32(&hash,e->action_id);hash_u32(&hash,e->kind);
        hash_vec(&hash,e->before);hash_vec(&hash,e->after);hash_vec(&hash,e->motor.impulse);
        hash_vec(&hash,e->contact.normal_impulse);hash_u32(&hash,e->contact.impulses_applied);
        hash_u32(&hash,(uint32_t)e->inventory_delta);hash_u32(&hash,(uint32_t)e->health_delta);
        hash_u32(&hash,e->ownership_transfer);hash_u32(&hash,e->switch_changed);hash_u32(&hash,e->switch_state);
    }
    hash_u32(&hash,nf18a6_current_body_hash(r->owner));hash_u32(&hash,nf18a5_history_hash(&r->owner->physical.world.history));
    hash_u32(&hash,r->authority->registry.accounts[0].balance);hash_u32(&hash,(uint32_t)r->authority->grant_count);return hash;
}
