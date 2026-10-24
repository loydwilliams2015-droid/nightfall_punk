#include "nf_b6_push18b6.h"
#include "nf_v18c2_h9_auth.h"
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

/* Reconstruct the B2 material/geometry/pose proof from the existing A6
 * authority. A fabricated, omitted or stale static collider is rejected.
 * The trusted provider alone owns credentials and membership policy.
 */
static Nf18b2Status b2_check(Nf18b6Push *ctx,const NfWorld *w,uint32_t tick){
    const Nf18a6Runtime *r=ctx->runtime;
    if(!ctx->b2_snapshot||!r||!w||w->tick>=UINT32_MAX)return NF18B2_INVALID;
    Nf18b2Scene scene={0};
    Nf18b2Use use={0};
    if(!ctx->b2_snapshot(ctx->b2_context,w,r,ctx->event.action,&scene,&use))
        return NF18B2_PENDING;
    if(scene.material!=&r->physical.world||scene.actor_count!=1||
       scene.object_count!=1||!scene.actors||!scene.objects||
       scene.geometry.tick!=tick||
       scene.geometry.world_version!=r->physical.world.revision||
       use.tick!=tick||use.world_revision!=r->physical.world.revision||
       use.material_epoch!=r->physical.world.grid.global_revision||
       use.actor_id!=ctx->event.actor||use.object_id!=ctx->event.object||
       use.action_id!=ctx->event.action||scene.actors[0].id!=use.actor_id||
       scene.objects[0].id!=use.object_id)return NF18B2_STALE;
    const NfActor *a=nf_world_find_actor_const(w,ctx->event.actor);
    const Nf18b2Actor *ba=&scene.actors[0];
    const Nf18b2Object *bo=&scene.objects[0];
    if(!a||!a->active||!a->combat.alive||
       ba->active!=1||ba->grounded!=(uint8_t)a->movement.grounded||
       use.actor_revision!=ba->revision||use.object_revision!=bo->revision||
       fabsf(ba->feet.x-a->transform.position.x)>.0001f||
       fabsf(ba->feet.y-a->transform.position.y)>.0001f||
       fabsf(ba->feet.z-a->transform.position.z)>.0001f||
       fabsf(ba->radius-r->physical.actor.radius)>.0001f||
       fabsf(ba->height-r->physical.actor.height)>.0001f||
       fabsf(ba->eye_height-a->movement.eye_height)>.0001f)
        return NF18B2_STALE;
    /* B2 handle must refer to the same physical crate; the gameplay handle
     * can be anywhere on the crate surface, not arbitrary remote space. */
    const Nf18a4Body *ob=&r->physical.object;
    if(!isfinite(bo->target.x)||!isfinite(bo->target.y)||
       !isfinite(bo->target.z)||bo->target.x<ob->center.x-ob->box_half.x-.0001f||
       bo->target.x>ob->center.x+ob->box_half.x+.0001f||
       bo->target.y<ob->center.y-ob->box_half.y-.0001f||
       bo->target.y>ob->center.y+ob->box_half.y+.0001f||
       bo->target.z<ob->center.z-ob->box_half.z-.0001f||
       bo->target.z>ob->center.z+ob->box_half.z+.0001f)
        return NF18B2_STALE;
    size_t static_count=0;
    if(scene.geometry.solid_count>NF_MAX_COLLIDERS||
       (scene.geometry.solid_count&&!scene.geometry.solids))
        return NF18B2_INVALID;
    for(size_t i=0;i<w->collider_count;++i){
        if(i==r->object_collider_index)continue;
        const NfCollider *wc=&w->colliders[i];
        if(wc->kind!=NF_COLLIDER_SOLID)continue;
        if(static_count>=scene.geometry.solid_count)return NF18B2_STALE;
        const Nf18aCollider *c=&scene.geometry.solids[static_count++];
        if(c->dynamic_body||
           c->min.x!=wc->min.x||c->min.y!=wc->min.y||
           c->min.z!=wc->min.z||c->max.x!=wc->max.x||
           c->max.y!=wc->max.y||c->max.z!=wc->max.z)
            return NF18B2_STALE;
    }
    if(static_count!=scene.geometry.solid_count)return NF18B2_STALE;
    return nf18b2_evaluate(&scene,use,ctx->b2_model).status;
}

static Nf18a6OwnerGate begin(const NfWorld *world,uint32_t tick,void *arg){
    Nf18b6Push *ctx=arg;
    if(!ctx || !ctx->runtime || !world)return NF18A6_GATE_INVALID;
    if(!ctx->armed)return NF18A6_GATE_READY;
    const Nf18b3Event *e=&ctx->event;
    if(!e->authorized||e->phase!=NF18B3_WAITING)return NF18A6_GATE_BLOCKED;
    if(e->actor!=ctx->runtime->actor_id ||
       e->object!=ctx->runtime->physical.object.id ||
       e->kind!=NF18B3_PUSH ||
       e->tick!=tick ||
       e->world_epoch!=(uint32_t)world->tick ||
       e->material_epoch!=ctx->runtime->physical.world.grid.global_revision ||
       e->contract_epoch!=ctx->contract_epoch)
        return NF18A6_GATE_STALE;
    if(ctx->require_h9 && !nf_c2_h9_valid(world,ctx->runtime,e->action))
        return NF18A6_GATE_BLOCKED;
    if(ctx->b2_snapshot){
        ctx->last_b2_status=b2_check(ctx,world,tick);
        if(ctx->last_b2_status!=NF18B2_ELIGIBLE)
            return ctx->last_b2_status==NF18B2_PENDING?NF18A6_GATE_PENDING:
                   ctx->last_b2_status==NF18B2_STALE?NF18A6_GATE_STALE:
                   NF18A6_GATE_BLOCKED;
    }
    return NF18A6_GATE_READY;
}
static bool prepublish(const NfWorld *world,const Nf18a5Integrated *candidate,void *arg){
    const Nf18b6Push *ctx=arg;
    if(!ctx||!world||!candidate||!ctx->runtime)return false;
    /* Actual A6 dispatcher is the sole transaction owner. No external
     * prepublication callback may confer authority outside its tick. */
    if(!world->tick_in_progress ||
       world->tick_owner_context!=ctx->runtime ||
       candidate->world.tick!=world->tick+1u ||
       ctx->runtime->physical.world.tick!=world->tick)
        return false;
    if(!ctx->armed)return true;
    if(ctx->require_h9 && !nf_c2_h9_take_prepublication(world,ctx->runtime,ctx->event.action))
        return false;
    /* Fail closed before physics publication if the bound material target or
     * frame has changed.  This is a staged A5 body, not a client projection.
     */
    const Nf18b3Event *e=&ctx->event;
    return (!ctx->b2_snapshot || b2_check((Nf18b6Push *)ctx,world,candidate->world.tick)==NF18B2_ELIGIBLE) &&
        e->authorized && e->phase==NF18B3_WAITING &&
        e->tick==candidate->world.tick && candidate->world.tick==world->tick+1u &&
        e->world_epoch==(uint32_t)world->tick &&
        e->material_epoch==ctx->runtime->physical.world.grid.global_revision &&
        e->contract_epoch==ctx->contract_epoch &&
        e->actor==candidate->actor.id && e->object==candidate->object.id &&
        candidate->committed_receipts>=ctx->before_contacts &&
        candidate->world.history.raw_samples>=ctx->before_history;
}
typedef struct Nf18b6ReceiptGate {
    const Nf18b6Push *ctx;
    const NfWorld *world;
    const Nf18a6Outcome *out;
} Nf18b6ReceiptGate;
static bool applied_receipt(void *arg,const Nf18b3Event *e){
    const Nf18b6ReceiptGate *gate=arg;
    const Nf18b6Push *ctx=gate->ctx;
    const Nf18a6Runtime *runtime=ctx->runtime;
    /* A real accepted A4 reciprocal impulse AND A5 contact history AND
     * the already published A6 world tick are each mandatory.
     * This callback is read-only: it does not apply another impulse.
     */
    return gate->out->status==NF18A6_COMMITTED &&
        gate->out->actual_impulse &&
        gate->world->tick==e->tick &&
        e->actor==runtime->actor_id &&
        e->object==runtime->physical.object.id &&
        runtime->physical.world.tick==e->tick &&
        runtime->physical.committed_receipts>ctx->before_contacts &&
        runtime->physical.world.history.raw_samples>ctx->before_history;
}
static void finish(const NfWorld *world,const Nf18a6Outcome *out,void *arg){
    Nf18b6Push *ctx=arg;
    if(!ctx||!ctx->armed)return;
    ctx->armed=false;
    if(ctx->require_h9)nf_c2_h9_revoke(ctx->runtime);
    ctx->last_result=NF18B3_PENDING;
    if(!world||!out||out->status!=NF18A6_COMMITTED)return;
    if(!out->actual_impulse)return;
    if(ctx->receipt_count>=NF18B6_PUSH_RECEIPTS){
        /* The capacity gate was checked during arming; this cannot silently
         * evict a receipt.  A6 motion is already committed independently.
         */
        ctx->last_result=NF18B3_INVALID;return;
    }
    const Nf18b6ReceiptGate gate={ctx,world,out};
    ctx->last_result=nf18b3_realize(&ctx->event,
        ctx->event.world_epoch,ctx->event.material_epoch,ctx->contract_epoch,
        applied_receipt,(void *)&gate);
    if(ctx->last_result!=NF18B3_OK)return;
    ctx->receipts[ctx->receipt_count++]=(Nf18b6PushReceipt){
        ctx->event.actor,ctx->event.action,ctx->event.tick};
    if(ctx->h20_receipt_journal){
        /* Evidence after actual A6 physical publication and B3 realization.
           Not crash-atomic with the A5 WAL; failure leaves an open gate. */
        const uint32_t fields[7]={
            ctx->event.tick,ctx->event.actor,ctx->event.action,
            runtime->physical.committed_receipts,
            runtime->physical.world.history.raw_samples,
            ctx->receipt_count,
            ctx->event.tick^ctx->event.actor^ctx->event.action^
             runtime->physical.committed_receipts^
             runtime->physical.world.history.raw_samples^
             ctx->receipt_count^0xA5B318C2u
        };
        FILE *fp=fopen(ctx->h20_receipt_journal,"ab");
        if(!fp){ctx->last_result=NF18B3_PENDING;return;}
        const bool wrote=fwrite(fields,sizeof(fields),1,fp)==1;
        const bool flushed=wrote&&fflush(fp)==0;
        const bool synced=flushed&&fsync(fileno(fp))==0;
        const bool closed=fclose(fp)==0;
        if(!synced||!closed)ctx->last_result=NF18B3_PENDING;
    }
}
bool nf18b6_push_attach(Nf18b6Push *ctx,Nf18a6Runtime *r,
                        const NfWorld *w,uint32_t epoch){
    if(!ctx||!r||!w||!epoch||w->tick_owner||w->tick_in_progress||
       r->owner_hooks.begin||r->owner_hooks.prepublish||
       r->owner_hooks.finish||r->owner_hooks.context||
       r->model<NF18A6_WORLD_BRIDGE||r->model>NF18A6_WORLD_CAMERA||
       r->actor_id==0||r->physical.object.id==0||
       !nf_world_find_actor_const(w,r->actor_id)||w->tick!=r->last_commit_tick)
        return false;
    memset(ctx,0,sizeof *ctx);
    ctx->runtime=r;
    ctx->contract_epoch=epoch;
    ctx->last_result=NF18B3_PENDING;
    r->owner_hooks.begin=begin;
    r->owner_hooks.prepublish=prepublish;
    r->owner_hooks.finish=finish;
    r->owner_hooks.context=ctx;
    return true;
}
static Nf18b3Result arm_internal(Nf18b6Push *ctx,const NfWorld *w,
                  uint32_t action,bool physical,bool contract,bool uncertain){
    if(!ctx||!w||!ctx->runtime||!action||ctx->armed||
       w->tick_in_progress||w->tick_owner_context!=ctx->runtime||
       w->tick!=ctx->runtime->last_commit_tick||w->tick>=UINT32_MAX-1u)
        return NF18B3_INVALID;
    if(ctx->receipt_count>=NF18B6_PUSH_RECEIPTS)return NF18B3_INVALID;
    for(uint32_t j=0;j<ctx->receipt_count;++j)
        if(ctx->receipts[j].actor==ctx->runtime->actor_id &&
           ctx->receipts[j].action==action)return NF18B3_DUPLICATE;
    Nf18b3Event proposal={0};
    proposal.actor=ctx->runtime->actor_id;
    proposal.object=ctx->runtime->physical.object.id;
    proposal.action=action;
    proposal.tick=(uint32_t)w->tick+1u;
    proposal.world_epoch=(uint32_t)w->tick;
    proposal.material_epoch=ctx->runtime->physical.world.grid.global_revision;
    proposal.contract_epoch=ctx->contract_epoch;
    proposal.kind=NF18B3_PUSH;
    proposal.phase=NF18B3_PROPOSED;
    Nf18b3Result res=nf18b3_authorize(&proposal,physical,contract,uncertain);
    ctx->last_result=res;
    ctx->event=proposal;
    if(res!=NF18B3_OK)return res;
    ctx->before_contacts=ctx->runtime->physical.committed_receipts;
    ctx->before_history=ctx->runtime->physical.world.history.raw_samples;
    ctx->armed=true;
    return res;
}
Nf18b3Result nf18b6_push_arm(Nf18b6Push *ctx,const NfWorld *w,
                  uint32_t action,bool physical,bool contract,bool uncertain){
    if(ctx&&ctx->b2_snapshot)return NF18B3_INVALID;
    return arm_internal(ctx,w,action,physical,contract,uncertain);
}
bool nf18b6_push_h20_journal(Nf18b6Push *ctx,const char *path){
    if(!ctx||!ctx->runtime||ctx->armed)return false;
    ctx->h20_receipt_journal=path;return true;
}
bool nf18b6_push_require_h9(Nf18b6Push *ctx){
    if(!ctx||!ctx->runtime||!ctx->b2_snapshot||ctx->armed)return false;
    ctx->require_h9=true;return true;
}
bool nf18b6_push_require_b2(Nf18b6Push *ctx,Nf18b6B2Snapshot provider,
    void *context,Nf18b2Model model){
    if(!ctx||!ctx->runtime||!provider||ctx->armed||ctx->b2_snapshot||
       (model!=NF18B2_M3_EXHAUSTIVE&&model!=NF18B2_M4_MATERIAL_FIRST))
        return false;
    ctx->b2_snapshot=provider;
    ctx->b2_context=context;
    ctx->b2_model=model;
    ctx->last_b2_status=NF18B2_PENDING;
    return true;
}
Nf18b3Result nf18b6_push_arm_b2(Nf18b6Push *ctx,const NfWorld *w,
                                  uint32_t action){
    if(!ctx||!ctx->b2_snapshot||!ctx->runtime||!w||!action||
       ctx->armed||w->tick_owner_context!=ctx->runtime||
       w->tick_in_progress||w->tick>=UINT32_MAX-1u)return NF18B3_INVALID;
    /* Use a transient proposed identity during read-only B2 adjudication. */
    Nf18b3Event prior=ctx->event;
    ctx->event=(Nf18b3Event){.actor=ctx->runtime->actor_id,
       .object=ctx->runtime->physical.object.id,.action=action,
       .tick=(uint32_t)w->tick+1u};
    ctx->last_b2_status=b2_check(ctx,w,(uint32_t)w->tick+1u);
    ctx->event=prior;
    if(ctx->last_b2_status!=NF18B2_ELIGIBLE){
        ctx->last_result=ctx->last_b2_status==NF18B2_PENDING?
                          NF18B3_PENDING:NF18B3_DENIED;
        return ctx->last_result;
    }
    return arm_internal(ctx,w,action,true,true,false);
}
bool nf18b6_push_detach(Nf18b6Push *ctx,Nf18a6Runtime *r,const NfWorld *w){
    if(!ctx||!r||!w||ctx->runtime!=r||w->tick_owner||
       w->tick_in_progress||r->owner_hooks.context!=ctx||ctx->armed)return false;
    memset(&r->owner_hooks,0,sizeof r->owner_hooks);
    ctx->runtime=NULL;
    return true;
}
