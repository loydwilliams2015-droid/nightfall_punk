#include "nf_b6_push18b6.h"
#include <string.h>
#include <limits.h>

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
    return NF18A6_GATE_READY;
}
static bool prepublish(const NfWorld *world,const Nf18a5Integrated *candidate,void *arg){
    const Nf18b6Push *ctx=arg;
    if(!ctx||!world||!candidate||!ctx->runtime)return false;
    if(!ctx->armed)return true;
    /* Fail closed before physics publication if the bound material target or
     * frame has changed.  This is a staged A5 body, not a client projection.
     */
    const Nf18b3Event *e=&ctx->event;
    return e->authorized && e->phase==NF18B3_WAITING &&
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
Nf18b3Result nf18b6_push_arm(Nf18b6Push *ctx,const NfWorld *w,
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
bool nf18b6_push_detach(Nf18b6Push *ctx,Nf18a6Runtime *r,const NfWorld *w){
    if(!ctx||!r||!w||ctx->runtime!=r||w->tick_owner||
       w->tick_in_progress||r->owner_hooks.context!=ctx||ctx->armed)return false;
    memset(&r->owner_hooks,0,sizeof r->owner_hooks);
    ctx->runtime=NULL;
    return true;
}
