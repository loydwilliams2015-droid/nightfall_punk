#include "nf_encounter18b2.h"
#include <math.h>
#include <string.h>

static Nf18b2Account *account(Nf18b2Registry *r,uint32_t id){
    for(size_t i=0;i<r->account_count;++i)if(r->accounts[i].actor_id==id)return &r->accounts[i];
    return NULL;
}
static Nf18b2RegisteredObject *object(Nf18b2Registry *r,uint32_t id){
    for(size_t i=0;i<r->object_count;++i)if(r->objects[i].object.id==id)return &r->objects[i];
    return NULL;
}
static void transition(Nf18b2EncounterTrace *t,Nf18b2EncounterState state){
    t->state=state;
    if(t->transition_count<12)t->transitions[t->transition_count++]=state;
}
static bool increment(uint32_t *value){if(*value==UINT32_MAX)return false;++*value;return true;}
static bool mutable_boundary(const Nf18b2EncounterRuntime *r){
    return r&&r->world&&r->owner&&!r->world->tick_in_progress;
}
bool nf18b2_encounter_init(Nf18b2EncounterRuntime *r,NfWorld *w,Nf18a6Runtime *owner,Nf18b2Model model){
    if(!r||!w||!owner||(model!=NF18B2_M3_EXHAUSTIVE&&model!=NF18B2_M4_MATERIAL_FIRST)||
       w->tick_in_progress||owner->owner_hooks.context||
       (owner->model!=NF18A6_WORLD_BRIDGE&&owner->model!=NF18A6_WORLD_CAMERA))return false;
    memset(r,0,sizeof(*r));r->world=w;r->owner=owner;r->model=model;
    r->registry.revision=r->registry.contract_epoch=r->registry.geometry_epoch=1;
    return true;
}
bool nf18b2_register_actor(Nf18b2EncounterRuntime *r,uint32_t id,uint32_t credentials,uint32_t inventory,uint32_t balance){
    if(!mutable_boundary(r)||!nf_world_find_actor_const(r->world,id)||
       r->registry.account_count==NF18B2_MAX_REQUESTS||account(&r->registry,id)||
       r->registry.revision==UINT32_MAX)return false;
    Nf18b2Account *a=&r->registry.accounts[r->registry.account_count++];
    a->actor_id=id;a->revision=1;a->credentials=credentials;a->inventory=inventory;a->balance=balance;
    ++r->registry.revision;return true;
}
bool nf18b2_register_object(Nf18b2EncounterRuntime *r,Nf18b2Object o,uint32_t identity,uint32_t cost,uint32_t consume){
    if(!mutable_boundary(r)||!identity||!o.id||!o.revision||!o.cell_id||!o.nexus_id||!o.slot_id||
       !o.capacity||o.capacity>16||o.occupant_count>o.capacity||o.member_count>16||
       !isfinite(o.target.x)||!isfinite(o.target.y)||!isfinite(o.target.z)||
       !isfinite(o.max_reach)||o.max_reach<0||o.preconditions>1||o.requires_support>1||
       r->registry.object_count==NF18B2_MAX_REQUESTS||object(&r->registry,o.id)||
       r->registry.revision==UINT32_MAX)return false;
    for(size_t i=0;i<r->registry.object_count;++i){
        const Nf18b2Object *p=&r->registry.objects[i].object;
        if(p->cell_id==o.cell_id&&p->nexus_id==o.nexus_id&&p->slot_id==o.slot_id)return false;
    }
    if(o.id==r->owner->physical.object.id&&identity!=r->owner->physical.world.grid.global_revision)return false;
    Nf18b2RegisteredObject *entry=&r->registry.objects[r->registry.object_count++];
    entry->object=o;entry->material_identity_epoch=identity;entry->cost=cost;entry->consume_inventory=consume;
    entry->follows_physical_object=o.id==r->owner->physical.object.id;
    entry->target_offset=(NfVec3){o.target.x-r->owner->physical.object.center.x,
        o.target.y-r->owner->physical.object.center.y,o.target.z-r->owner->physical.object.center.z};
    ++r->registry.revision;return true;
}
bool nf18b2_set_contract(Nf18b2EncounterRuntime *r,uint32_t id,uint32_t credentials,uint32_t inventory){
    if(!r||!r->world||!r->owner)return false;
    Nf18b2Account *a=account(&r->registry,id);
    if(!a||a->revision==UINT32_MAX||r->registry.contract_epoch==UINT32_MAX||
       r->registry.revision==UINT32_MAX)return false;
    a->credentials=credentials;a->inventory=inventory;++a->revision;
    ++r->registry.contract_epoch;++r->registry.revision;return true;
}
bool nf18b2_touch_object(Nf18b2EncounterRuntime *r,uint32_t id){
    if(!r)return false;
    Nf18b2RegisteredObject *o=object(&r->registry,id);
    if(!o||o->object.revision==UINT32_MAX||r->registry.revision==UINT32_MAX)return false;
    ++o->object.revision;++r->registry.revision;return true;
}
bool nf18b2_live_frame(Nf18b2EncounterRuntime *r,Nf18b2LiveFrame *f){
    if(!r||!f||!r->world||!r->owner||r->world->tick>=UINT32_MAX||
       r->world->collider_count>NF_MAX_COLLIDERS||r->registry.account_count>64||
       r->registry.object_count>64)return false;
    memset(f,0,sizeof(*f));
    size_t ns=0,nl=0;
    for(size_t i=0;i<r->world->collider_count;++i){
        const NfCollider *c=&r->world->colliders[i];
        Nf18aCollider *dst=c->kind==NF_COLLIDER_LADDER?&f->ladders[nl++]:&f->solids[ns++];
        dst->body_id=(uint32_t)i+100000u;dst->min=c->min;dst->max=c->max;dst->velocity=c->velocity;
        dst->dynamic_body=c->kind==NF_COLLIDER_MOVING_PLATFORM||i==r->owner->object_collider_index;
    }
    if(ns!=r->observed_count||memcmp(f->solids,r->observed_solids,ns*sizeof(f->solids[0]))||
       nl!=r->observed_ladder_count||memcmp(f->ladders,r->observed_ladders,nl*sizeof(f->ladders[0]))){
        if(!increment(&r->registry.geometry_epoch)||!increment(&r->registry.revision))return false;
        memcpy(r->observed_solids,f->solids,ns*sizeof(f->solids[0]));r->observed_count=ns;
        memcpy(r->observed_ladders,f->ladders,nl*sizeof(f->ladders[0]));r->observed_ladder_count=nl;
    }
    for(size_t i=0;i<r->registry.account_count;++i){
        Nf18b2Account *entry=&r->registry.accounts[i];
        const NfActor *live=nf_world_find_actor_const(r->world,entry->actor_id);
        if(!live)return false;
        Nf18b2Actor *a=&f->actors[i];a->id=live->id;a->revision=entry->revision;
        a->credentials=entry->credentials;a->inventory=entry->inventory;a->feet=live->transform.position;
        a->radius=r->world->movement.radius;a->height=live->movement.body_height;
        a->eye_height=live->movement.eye_height;a->active=live->active&&live->combat.alive;
        a->grounded=live->movement.grounded;
        for(size_t j=0;j<ns;++j){
            const Nf18aCollider *c=&f->solids[j];
            if(fabsf(a->feet.y-c->max.y)<.0001f&&a->feet.x-a->radius>=c->min.x&&
               a->feet.x+a->radius<=c->max.x&&a->feet.z-a->radius>=c->min.z&&
               a->feet.z+a->radius<=c->max.z){a->support_id=c->body_id;break;}
        }
        if(memcmp(a,&entry->last_pose,sizeof(*a))){
            if(!increment(&entry->revision)||!increment(&r->registry.revision))return false;
            a->revision=entry->revision;entry->last_pose=*a;
        }
    }
    for(size_t i=0;i<r->registry.object_count;++i){
        Nf18b2RegisteredObject *entry=&r->registry.objects[i];
        if(entry->follows_physical_object){
            const NfVec3 c=r->owner->physical.object.center;
            const NfVec3 t={c.x+entry->target_offset.x,c.y+entry->target_offset.y,c.z+entry->target_offset.z};
            if(memcmp(&t,&entry->object.target,sizeof t)){
                if(!increment(&entry->object.revision)||!increment(&r->registry.revision))return false;
                entry->object.target=t;
            }
        }
        f->objects[i]=entry->object;
    }
    f->scene.material=&r->owner->physical.world;
    f->scene.geometry=(Nf18a3Geometry){.solids=f->solids,.solid_count=ns,.ladders=f->ladders,.ladder_count=nl,
        .world_version=r->owner->physical.world.revision,.tick=(uint32_t)r->world->tick+1};
    f->scene.actors=f->actors;f->scene.objects=f->objects;
    f->scene.actor_count=r->registry.account_count;f->scene.object_count=r->registry.object_count;
    return true;
}
static bool already_granted(const Nf18b2EncounterRuntime *r,Nf18b2Intent in){
    for(size_t i=0;i<r->grant_count;++i)
        if(r->grant_actors[i]==in.actor_id&&r->grant_actions[i]==in.action_id)return true;
    return false;
}
Nf18b2Status nf18b2_propose(Nf18b2EncounterRuntime *r,Nf18b2Intent in){
    if(!mutable_boundary(r)||!in.actor_id||!in.object_id||!in.action_id||
       in.kind<NF18B2_USE||in.kind>NF18B2_LADDER||
       (in.kind==NF18B2_PUSH&&in.object_id!=r->owner->physical.object.id))
        return NF18B2_INVALID;
    if(already_granted(r,in))return NF18B2_DUPLICATE;
    if(r->queue_count==64||r->trace_count==NF18B2_MAX_TRACES||r->grant_count==64)return NF18B2_PENDING;
    for(size_t i=0;i<r->queue_count;++i)
        if(r->queue[i].intent.actor_id==in.actor_id&&r->queue[i].intent.action_id==in.action_id)
            return r->queue[i].intent.object_id==in.object_id?NF18B2_DUPLICATE:NF18B2_INVALID;
    if(!nf18b2_live_frame(r,&r->frame))return NF18B2_INVALID;
    Nf18b2Account *a=account(&r->registry,in.actor_id);
    Nf18b2RegisteredObject *o=object(&r->registry,in.object_id);
    if(!a||!o)return NF18B2_INVALID;
    Nf18b2QueuedIntent *q=&r->queue[r->queue_count++];memset(q,0,sizeof(*q));q->intent=in;
    q->witness=(Nf18b2Use){in.actor_id,in.object_id,in.action_id,r->frame.scene.geometry.tick,
        r->owner->physical.world.revision,r->owner->physical.world.grid.global_revision,a->revision,o->object.revision};
    q->registry_revision=r->registry.revision;q->contract_epoch=r->registry.contract_epoch;
    q->geometry_epoch=r->registry.geometry_epoch;q->trace_index=r->trace_count++;
    Nf18b2EncounterTrace *t=&r->traces[q->trace_index];memset(t,0,sizeof(*t));t->intent=in;
    t->assessed_tick=q->witness.tick;t->world_before=q->witness.world_revision;
    t->material_epoch=q->witness.material_epoch;t->material_identity_epoch=o->material_identity_epoch;
    t->contract_epoch=q->contract_epoch;t->actor_revision=a->revision;t->object_revision=o->object.revision;
    t->balance_before=t->balance_after=a->balance;t->history_before=t->history_after=nf18a5_history_hash(&r->owner->physical.world.history);
    t->feet_before=t->feet_after=nf_world_find_actor_const(r->world,in.actor_id)->transform.position;
    transition(t,NF18B2_PROPOSED);transition(t,NF18B2_EVALUATING);
    t->authorization=nf18b2_evaluate(&r->frame.scene,q->witness,r->model);
    /* A moving support is an unsupported encounter domain, not demonstrated
       static infeasibility. Preserve the raw evaluator's support reason. */
    for(size_t i=0;i<r->frame.scene.actor_count;++i)if(r->frame.actors[i].id==in.actor_id)
        for(size_t j=0;j<r->frame.scene.geometry.solid_count;++j)
            if(r->frame.solids[j].body_id==r->frame.actors[i].support_id&&r->frame.solids[j].dynamic_body&&
               o->object.requires_support){
                t->authorization.status=NF18B2_PENDING;t->authorization.physical=NF18B2_PENDING;
                t->deferred_milestone=3;
            }
    transition(t,t->authorization.status==NF18B2_ELIGIBLE?NF18B2_ENCOUNTER_ELIGIBLE:
        t->authorization.status==NF18B2_PENDING?NF18B2_ENCOUNTER_PENDING:NF18B2_ENCOUNTER_BLOCKED);
    return t->authorization.status;
}
bool nf18b2_withdraw(Nf18b2EncounterRuntime *r,uint32_t actor,uint32_t action){
    if(!mutable_boundary(r))return false;
    for(size_t i=0;i<r->queue_count;++i)if(r->queue[i].intent.actor_id==actor&&r->queue[i].intent.action_id==action){
        r->queue[i].withdrawn=1;transition(&r->traces[r->queue[i].trace_index],NF18B2_CANCELLED);return true;
    }
    return false;
}
static Nf18a6OwnerGate begin(const NfWorld *w,uint32_t tick,void *ctx){
    Nf18b2EncounterRuntime *r=ctx;r->prepared=0;memset(r->granted,0,sizeof r->granted);
    if(w!=r->world||!w->tick_in_progress)return NF18A6_GATE_INVALID;
    if(!r->queue_count)return NF18A6_GATE_READY;
    if(!nf18b2_live_frame(r,&r->frame))return NF18A6_GATE_INVALID;
    Nf18b2Use requests[64],active_requests[64];
    Nf18b2UseDecision results[64],active_results[64];size_t active_indices[64],active_count=0;
    for(size_t i=0;i<r->queue_count;++i){
        requests[i]=r->queue[i].witness;
        results[i]=r->traces[r->queue[i].trace_index].authorization;
        /* Withdrawn and initially negative proposals cannot hold a slot or
           suppress a competing fresh request during prospective resolution. */
        if(!r->queue[i].withdrawn&&results[i].status==NF18B2_ELIGIBLE){
            active_indices[active_count]=i;active_requests[active_count++]=requests[i];
        }
    }
    if(active_count&&!nf18b2_resolve(&r->frame.scene,active_requests,active_count,r->model,active_results))return NF18A6_GATE_INVALID;
    for(size_t i=0;i<active_count;++i)results[active_indices[i]]=active_results[i];
    r->staged=r->registry;
    size_t order[64];for(size_t i=0;i<r->queue_count;++i)order[i]=i;
    for(size_t i=1;i<r->queue_count;++i){size_t id=order[i],j=i;
        while(j&&(requests[id].actor_id<requests[order[j-1]].actor_id||
            (requests[id].actor_id==requests[order[j-1]].actor_id&&requests[id].action_id<requests[order[j-1]].action_id))){
            order[j]=order[j-1];--j;
        }order[j]=id;
    }
    bool pending=false,stale=false;size_t granted=0;
    for(size_t k=0;k<r->queue_count;++k){
        const size_t i=order[k];Nf18b2QueuedIntent *q=&r->queue[i];Nf18b2EncounterTrace *t=&r->traces[q->trace_index];
        if(q->withdrawn)continue;
        /* A negative assessment is not a permit. Streaming/retry requires a
           new proposal rather than promoting this old witness implicitly. */
        if(t->authorization.status!=NF18B2_ELIGIBLE){
            pending|=t->authorization.status==NF18B2_PENDING;continue;
        }
        transition(t,NF18B2_REVALIDATING);t->revalidation=results[i];
        if(q->witness.tick!=tick||q->registry_revision!=r->registry.revision||
           q->contract_epoch!=r->registry.contract_epoch||q->geometry_epoch!=r->registry.geometry_epoch){
            t->revalidation.status=NF18B2_STALE;t->revalidation.reason=NF18B2_FRAME_CHANGED;
        }
        if(t->revalidation.status!=NF18B2_ELIGIBLE){
            pending|=t->revalidation.status==NF18B2_PENDING;stale|=t->revalidation.status==NF18B2_STALE;
            transition(t,t->revalidation.status==NF18B2_STALE?NF18B2_EXPIRED:
                t->revalidation.status==NF18B2_PENDING?NF18B2_ENCOUNTER_PENDING:NF18B2_ENCOUNTER_BLOCKED);
            t->visual_correction=1;continue;
        }
        if(q->intent.kind==NF18B2_LADDER){t->deferred_milestone=3;transition(t,NF18B2_ENCOUNTER_PENDING);pending=true;continue;}
        Nf18b2Account *a=account(&r->staged,q->intent.actor_id);
        Nf18b2RegisteredObject *o=object(&r->staged,q->intent.object_id);
        if(!a||!o||r->grant_count+granted>=64||a->revision==UINT32_MAX||o->object.revision==UINT32_MAX||
           r->staged.revision==UINT32_MAX){
            transition(t,NF18B2_ENCOUNTER_PENDING);pending=true;continue;
        }
        if(a->balance<o->cost||(a->inventory&o->consume_inventory)!=o->consume_inventory){
            t->revalidation.status=NF18B2_BLOCKED;t->revalidation.contract=NF18B2_BLOCKED;
            t->revalidation.reason=NF18B2_INVENTORY;transition(t,NF18B2_ENCOUNTER_BLOCKED);continue;
        }
        bool member=false;for(unsigned j=0;j<o->object.occupant_count;++j)member|=o->object.occupants[j]==a->actor_id;
        if(!member){if(o->object.occupant_count==o->object.capacity){transition(t,NF18B2_ENCOUNTER_BLOCKED);continue;}
            o->object.occupants[o->object.occupant_count++]=a->actor_id;}
        a->balance-=o->cost;a->inventory&=~o->consume_inventory;++a->revision;++o->object.revision;++r->staged.revision;
        r->granted[i]=1;++granted;transition(t,NF18B2_RESERVED);transition(t,NF18B2_COMMIT_PENDING);
    }
    if(granted){r->prepared=1;return NF18A6_GATE_READY;}
    return pending?NF18A6_GATE_PENDING:stale?NF18A6_GATE_STALE:NF18A6_GATE_BLOCKED;
}
static bool prepublish(const NfWorld *w,const Nf18a5Integrated *candidate,void *ctx){
    Nf18b2EncounterRuntime *r=ctx;
    if(w!=r->world||!w->tick_in_progress)return false;
    if(!r->prepared)return true;
    if(r->publication_validator&&!r->publication_validator(candidate,r->publication_context))return false;
    if(!nf18b2_live_frame(r,&r->frame))return false;
    for(size_t i=0;i<r->queue_count;++i)if(r->granted[i]){
        const Nf18b2QueuedIntent *q=&r->queue[i];Nf18b2EncounterTrace *t=&r->traces[q->trace_index];
        t->revalidation=nf18b2_evaluate(&r->frame.scene,q->witness,r->model);
        if(q->registry_revision!=r->registry.revision||q->contract_epoch!=r->registry.contract_epoch||
           q->geometry_epoch!=r->registry.geometry_epoch||candidate->world.tick!=q->witness.tick||
           candidate->world.revision!=q->witness.world_revision+1u||
           candidate->world.grid.global_revision!=q->witness.material_epoch||
           t->revalidation.status!=NF18B2_ELIGIBLE){
            t->revalidation.status=NF18B2_STALE;t->revalidation.reason=NF18B2_FRAME_CHANGED;
            t->visual_correction=1;return false;
        }
    }
    return true;
}
static void finish(const NfWorld *w,const Nf18a6Outcome *out,void *ctx){
    Nf18b2EncounterRuntime *r=ctx;
    if(w!=r->world)return;
    if(out->status==NF18A6_COMMITTED&&r->prepared){
        /* A5 has published the body/history (and optional physical WAL). Under
           the single owner, these prevalidated assignments cannot fail. Registry
           crash persistence is deliberately not claimed by this local interface. */
        r->registry=r->staged;
        for(size_t i=0;i<r->queue_count;++i)if(r->granted[i]){
            Nf18b2QueuedIntent *q=&r->queue[i];Nf18b2EncounterTrace *t=&r->traces[q->trace_index];
            const size_t index=r->grant_count++;
            r->grant_actors[index]=q->intent.actor_id;r->grant_objects[index]=q->intent.object_id;
            r->grant_actions[index]=q->intent.action_id;t->published=1;t->commit_tick=out->tick;
            t->physical_wal=r->owner->tick_journal!=NULL;transition(t,NF18B2_COMMITTED);transition(t,NF18B2_EXECUTING);
        }
    }
    for(size_t i=0;i<r->queue_count;++i){Nf18b2EncounterTrace *t=&r->traces[r->queue[i].trace_index];
        t->physical_status=out->status;t->world_after=r->owner->physical.world.revision;
        if(r->granted[i]&&out->status!=NF18A6_COMMITTED)transition(t,NF18B2_FAILED_NO_EFFECT);
    }
    for(size_t i=0;i<r->trace_count;++i){Nf18b2EncounterTrace *t=&r->traces[i];
        if(!t->published)continue;
        const Nf18b2Account *a=account(&r->registry,t->intent.actor_id);
        const NfActor *actor=nf_world_find_actor_const(w,t->intent.actor_id);
        t->balance_after=a?a->balance:t->balance_after;t->feet_after=actor?actor->transform.position:t->feet_after;
        t->history_after=nf18a5_history_hash(&r->owner->physical.world.history);
        if(t->state==NF18B2_EXECUTING&&t->contract_epoch!=r->registry.contract_epoch){
            transition(t,NF18B2_INTERRUPTED);transition(t,NF18B2_COMPENSATION_REQUIRED);
        }
    }
    /* One applied contact may not be attributed to multiple authorization
       events. The most recent active push handoff receives this real receipt. */
    if(out->status==NF18A6_COMMITTED&&out->actual_impulse)
        for(size_t i=r->trace_count;i>0;--i){Nf18b2EncounterTrace *t=&r->traces[i-1];
            if(t->published&&t->state==NF18B2_EXECUTING&&t->intent.kind==NF18B2_PUSH&&
               t->intent.object_id==r->owner->physical.object.id){
                ++t->applied_receipts;transition(t,NF18B2_COMPLETED);break;
            }
        }
    r->queue_count=0;r->prepared=0;
}
bool nf18b2_attach(Nf18b2EncounterRuntime *r){
    if(!mutable_boundary(r)||r->attached||!r->world->tick_owner||
       r->world->tick_owner_context!=r->owner||r->owner->owner_hooks.context)return false;
    r->owner->owner_hooks=(Nf18a6OwnerHooks){.begin=begin,.prepublish=prepublish,.finish=finish,.context=r};r->attached=1;return true;
}
bool nf18b2_detach(Nf18b2EncounterRuntime *r){
    if(!mutable_boundary(r)||!r->attached||r->queue_count||r->owner->owner_hooks.context!=r)return false;
    memset(&r->owner->owner_hooks,0,sizeof r->owner->owner_hooks);r->attached=0;return true;
}
bool nf18b2_interrupt(Nf18b2EncounterRuntime *r,uint32_t actor,uint32_t action){
    if(!mutable_boundary(r))return false;
    for(size_t i=0;i<r->trace_count;++i){Nf18b2EncounterTrace *t=&r->traces[i];
        if(t->intent.actor_id==actor&&t->intent.action_id==action&&t->published&&
           (t->state==NF18B2_EXECUTING||t->state==NF18B2_COMPLETED)){
            transition(t,NF18B2_INTERRUPTED);transition(t,NF18B2_COMPENSATION_REQUIRED);return true;
        }
    }
    return false;
}
bool nf18b2_observe(const Nf18b2EncounterRuntime *r,size_t i,Nf18b2EncounterTrace *out){
    if(!r||!out||i>=r->trace_count)return false;
    *out=r->traces[i];return true;
}
const char *nf18b2_encounter_state_name(Nf18b2EncounterState s){
    static const char *const names[]={"PROPOSED","EVALUATING","BLOCKED","PENDING","ELIGIBLE","EXPIRED",
        "RESERVED","REVALIDATING","CANCELLED","COMMIT_PENDING","COMMITTED","FAILED_NO_EFFECT",
        "EXECUTING","COMPLETED","INTERRUPTED","COMPENSATION_REQUIRED"};
    return s>=NF18B2_PROPOSED&&s<=NF18B2_COMPENSATION_REQUIRED?names[s]:"INVALID";
}
