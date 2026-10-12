#include "v18b3_fixtures.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static unsigned checks,failures;
static void check(bool value,const char *name){++checks;failures+=!value;printf("%s,%s\n",name,value?"PASS":"FAIL");}
static bool deny(const NfWorld *w,const Nf18b3State *s,void *ctx){(void)w;(void)s;(void)ctx;return false;}
static bool revoke(const NfWorld *w,const Nf18b3State *s,void *ctx){(void)w;(void)s;return nf18b2_set_contract(ctx,10,0,1);}
static bool conservation(B3Fixture *f){return fabs(f->r->state.power_joules+f->r->state.spent_joules-10000)<1e-7&&
    f->r->state.power_joules>=0&&f->r->state.salvage_units+f->r->state.objects[0].health==8;}
int main(void){
    B3Fixture f;if(!b3_allocate(&f))return 2;
    Nf18a5Integrated *before=malloc(sizeof(*before));if(!before)return 2;
    for(unsigned model=3;model<=4;++model){
        for(unsigned kind=0;kind<=NF18B3_DOOR_CLOSE;++kind){
            check(b3_fixture(&f,model,(Nf18b3Kind)kind,1,1,32),"real_owner_fixture");
            NfVec3 initial=f.r->state.objects[0].body.center;
            check(nf18b3_request(f.r,10,f.target,1,(Nf18b3Kind)kind)==NF18B3_PROPOSED,"necessary_permission_not_completion");
            check(f.r->state.event_count==0&&f.a->grant_count==0,"proposal_no_effect");
            check(b3_run(&f,kind==NF18B3_CARRY?24:120),"physical_owner_execution");
            Nf18b3Action *a=&f.r->state.actions[0];Nf18b3Object *o=&f.r->state.objects[0];
            if(kind==NF18B3_CARRY){
                check(a->status==NF18B3_EXECUTING&&o->carried_by==10&&o->body.center.y>initial.y&&
                    a->motor_receipts>0&&a->contact_receipts>0,"carry_finite_force_reaction_and_support");
                const NfVec3 pose=o->body.center;
                check(nf18b3_cancel(f.r,10,1,true)&&b3_step(&f)&&a->status==NF18B3_COMPLETED&&o->carried_by==0&&
                    fabsf(o->body.center.x-pose.x)<.04f&&fabsf(o->body.center.y-pose.y)<.04f,"carry_release_preserves_inertial_pose");
            }else{
                check(a->status==NF18B3_COMPLETED,"completion_requires_realization");
                if(kind==NF18B3_PICKUP)check(o->units==0&&!o->available&&f.r->state.inventory_units==1&&
                    f.w->collider_count==34,"pickup_conserved_transfer_and_collider_removal");
                if(kind==NF18B3_REPAIR)check(o->health==3&&f.r->state.salvage_units==5&&f.a->registry.accounts[0].balance==7,
                    "repair_actual_strokes_salvage_tokens");
                if(kind==NF18B3_PUSH)check(a->contact_receipts==1&&f.p->physical.world.history.raw_samples>0,
                    "push_real_A4_contact_A5_history");
                if(kind==NF18B3_SWITCH)check(o->on&&f.r->state.power_enabled,"switch_actual_travel_then_toggle");
                if(kind==NF18B3_DOOR_OPEN||kind==NF18B3_DOOR_CLOSE)check(fabsf(o->body.center.x-initial.x)>.29f&&
                    f.w->colliders[o->collider_index].min.x==o->body.center.x-o->body.box_half.x,"door_actual_pose_and_collision");
            }
            check(conservation(&f),"mechanical_power_and_salvage_conservation");
            check(f.a->registry.objects[0].object.occupant_count==0&&f.r->state.active==UINT32_MAX,"terminal_slot_released");
            check(nf18b3_request(f.r,10,f.target,1,(Nf18b3Kind)kind)==NF18B3_DUPLICATE,"published_identity_cannot_repeat");
        }
        check(b3_fixture(&f,model,NF18B3_DOOR_OPEN,1,1,0),"cancel_setup");
        check(nf18b3_request(f.r,10,f.target,2,NF18B3_DOOR_OPEN)==NF18B3_PROPOSED&&nf18b3_cancel(f.r,10,2,false),"cancel_before_tick");
        *before=f.p->physical;(void)b3_step(&f);
        check(f.r->state.actions[0].status==NF18B3_CANCELLED&&memcmp(before,&f.p->physical,sizeof(*before))==0&&f.a->grant_count==0,"cancel_no_physical_resource_effect");

        check(b3_fixture(&f,model,NF18B3_REPAIR,1,1,0),"interrupted_repair_setup");
        check(nf18b3_request(f.r,10,f.target,3,NF18B3_REPAIR)==NF18B3_PROPOSED&&b3_step(&f),"repair_started_real_motor");
        check(f.r->state.objects[0].health==0&&f.a->registry.accounts[0].balance==10,"prethreshold_no_material_debit");
        check(nf18b3_cancel(f.r,10,3,false)&&b3_step(&f)&&f.r->state.actions[0].status==NF18B3_COMPENSATION_REQUIRED&&
            f.r->state.objects[0].health==0&&f.a->registry.objects[0].object.occupant_count==0,"interruption_preserves_tool_pose_releases_lease");
        check(b3_fixture(&f,model,NF18B3_REPAIR,1,1,0),"postthreshold_repair_setup");
        (void)nf18b3_request(f.r,10,f.target,4,NF18B3_REPAIR);
        for(unsigned i=0;i<30&&!f.r->state.objects[0].health;++i)(void)b3_step(&f);
        const size_t events=f.r->state.event_count;
        check(f.r->state.objects[0].health==1&&nf18b3_cancel(f.r,10,4,false)&&b3_step(&f)&&
            f.r->state.objects[0].health==1&&f.r->state.salvage_units==7&&f.a->registry.accounts[0].balance==9&&
            f.r->state.event_count==events,"postthreshold_effect_and_history_not_refunded");

        check(b3_fixture(&f,model,NF18B3_PICKUP,1,1,0),"failed_publisher_setup");
        (void)nf18b3_request(f.r,10,f.target,5,NF18B3_PICKUP);*before=f.p->physical;f.r->publication_validator=deny;
        check(!b3_step(&f)&&f.r->state.actions[0].status==NF18B3_PUBLISH_FAILED&&f.r->state.inventory_units==0&&
            f.r->state.objects[0].available&&f.a->grant_count==0&&memcmp(before,&f.p->physical,sizeof(*before))==0,"failed_publication_whole_transaction_no_effect");
        f.r->publication_validator=NULL;
        check(nf18b3_request(f.r,10,f.target,5,NF18B3_PICKUP)==NF18B3_PROPOSED&&b3_step(&f)&&f.r->state.inventory_units==1,"fresh_retry_once_after_failed_publication");

        check(b3_fixture(&f,model,NF18B3_DOOR_OPEN,1,1,0),"stale_setup");
        (void)nf18b3_request(f.r,10,f.target,6,NF18B3_DOOR_OPEN);nf18b2_set_contract(f.a,10,0,1);
        check(!b3_step(&f)&&f.r->state.actions[0].status==NF18B3_EXPIRED&&f.r->state.event_count==0,"changed_authority_expires_old_proposal");
        check(b3_fixture(&f,model,NF18B3_PICKUP,1,1,0),"prepublication_change_setup");
        (void)nf18b3_request(f.r,10,f.target,7,NF18B3_PICKUP);f.r->publication_validator=revoke;f.r->publication_context=f.a;
        check(!b3_step(&f)&&f.r->state.inventory_units==0&&f.a->grant_count==0&&f.r->state.actions[0].status==NF18B3_EXPIRED,"late_contract_change_fence_refuses_transfer");
        check(b3_fixture(&f,model,NF18B3_DOOR_OPEN,1,1,0),"pending_material_setup");
        f.p->physical.world.grid.chunks[0].loaded=0;
        check(nf18b3_request(f.r,10,f.target,8,NF18B3_DOOR_OPEN)==NF18B3_PENDING&&!b3_step(&f)&&
            f.r->state.event_count==0&&f.a->grant_count==0,"unknown_material_not_free");
        check(b3_fixture(&f,model,NF18B3_DOOR_OPEN,1,1,0),"jam_setup");
        f.r->state.objects[0].jammed=1;(void)nf18b3_request(f.r,10,f.target,9,NF18B3_DOOR_OPEN);
        check(!b3_step(&f)&&f.r->state.actions[0].status==NF18B3_JAMMED&&f.r->state.event_count==0&&
            f.a->registry.objects[0].object.occupant_count==0,"jam_is_not_door_success_or_orphan");
        check(b3_fixture(&f,model,NF18B3_DOOR_OPEN,1,1,0),"observer_setup");
        (void)nf18b3_request(f.r,10,f.target,10,NF18B3_DOOR_OPEN);(void)b3_step(&f);
        uint32_t hash=nf18b3_semantic_hash(f.r);Nf18b3Action view;
        check(nf18b3_observe(f.r,0,&view)&&nf18b3_semantic_hash(f.r)==hash,"observer_does_not_change_material_authority");
        check(nf18b3_request(f.r,10,f.target,11,NF18B3_REPAIR)==NF18B3_INVALID,"wrong_action_object_type_refused");
        check(nf18b3_request(f.r,10,f.target,12,NF18B3_TRAVERSAL)==NF18B3_UNSUPPORTED,"unimplemented_traversal_explicitly_unsupported");
        check(nf18b3_ack_events(f.r,f.r->state.events[f.r->state.event_count-1].sequence)&&
            !nf18b3_ack_events(f.r,f.r->state.acknowledged_event),"event_ack_is_explicit_and_idempotent");
        check(b3_fixture(&f,model,NF18B3_SWITCH,1,1,0),"switch_reversal_setup");
        (void)nf18b3_request(f.r,10,f.target,20,NF18B3_SWITCH);check(b3_run(&f,120),"switch_on_stroke");
        (void)nf18b3_request(f.r,10,f.target,21,NF18B3_SWITCH);
        check(b3_run(&f,120)&&!f.r->state.objects[0].on&&!f.r->state.power_enabled&&
            fabsf(f.r->state.objects[0].body.center.x-f.r->state.objects[0].origin.x)<.0001f,"switch_off_requires_reverse_stroke");
        check(b3_fixture(&f,model,NF18B3_CARRY,1,1,0),"carry_load_setup");
        f.p->physical.object.inverse_mass=.01f;f.r->state.objects[0].body.inverse_mass=.01f;
        (void)nf18b3_request(f.r,10,f.target,22,NF18B3_CARRY);
        check(b3_step(&f)&&f.r->state.actions[0].status==NF18B3_JAMMED&&f.r->state.event_count==0,"overweight_load_no_fabricated_hold");
        check(b3_fixture(&f,model,NF18B3_CARRY,1,1,0),"lease_invalidation_setup");
        (void)nf18b3_request(f.r,10,f.target,23,NF18B3_CARRY);(void)b3_step(&f);
        f.a->registry.objects[0].object.occupant_count=0;
        check(b3_step(&f)&&f.r->state.actions[0].status==NF18B3_COMPENSATION_REQUIRED&&
            f.r->state.actions[0].stop_reason==NF18B3_EXPIRED&&!f.r->state.objects[0].carried_by&&f.r->state.active==UINT32_MAX,
            "invalidated_lease_releases_hand_constraint_preserves_effects");
        check(b3_fixture(&f,model,NF18B3_DOOR_OPEN,1,1,0),"exhausted_event_setup");
        (void)nf18b3_request(f.r,10,f.target,24,NF18B3_DOOR_OPEN);f.r->state.event_count=NF18B3_MAX_EVENTS;
        *before=f.p->physical;
        check(!b3_step(&f)&&f.r->state.actions[0].status==NF18B3_PENDING&&f.a->grant_count==0&&
            memcmp(before,&f.p->physical,sizeof(*before))==0,"full_event_storage_atomic_pending");
        check(b3_fixture(&f,model,NF18B3_DOOR_OPEN,1,1,0),"physical_wal_scope_setup");
        (void)nf18b3_request(f.r,10,f.target,25,NF18B3_DOOR_OPEN);f.p->tick_journal="physical-only-not-B3";
        check(!b3_step(&f)&&f.r->state.event_count==0&&f.a->grant_count==0,"physical_only_wal_refused_for_aggregate_state");
        check(b3_fixture(&f,model,NF18B3_DOOR_OPEN,1,1,0),"nonfinite_motor_setup");
        f.r->state.objects[0].max_force=NAN;(void)nf18b3_request(f.r,10,f.target,26,NF18B3_DOOR_OPEN);
        check(b3_step(&f)&&f.r->state.actions[0].status==NF18B3_INVALID&&f.r->state.event_count==0,"nonfinite_motor_cannot_write_material");
        check(nf18b3_request(f.r,10,f.target,27,(Nf18b3Kind)-1)==NF18B3_INVALID,"negative_action_kind_invalid");
        check(b3_fixture(&f,model,NF18B3_DOOR_OPEN,1,1,0),"real_door_obstruction_setup");
        nf_world_add_collider(f.w,NF_COLLIDER_SOLID,(NfVec3){2.34f,.5f,.7f},(NfVec3){2.5f,1.3f,1.3f});
        (void)nf18b3_request(f.r,10,f.target,28,NF18B3_DOOR_OPEN);
        check(b3_run(&f,120)&&f.r->state.actions[0].status==NF18B3_COMPENSATION_REQUIRED&&
            f.r->state.actions[0].stop_reason==NF18B3_JAMMED&&f.r->state.objects[0].body.center.x<2.34f&&
            f.r->state.event_count>0&&f.a->registry.objects[0].object.occupant_count==0,"actual_obstruction_stops_motor_preserves_partial_motion");
        check(b3_fixture(&f,model,NF18B3_PUSH,1,1,0),"denied_push_motor_setup");
        f.r->state.objects[0].jammed=1;(void)nf18b3_request(f.r,10,f.target,29,NF18B3_PUSH);*before=f.p->physical;
        check(!b3_step(&f)&&memcmp(before,&f.p->physical,sizeof(*before))==0&&f.a->grant_count==0,
            "known_denial_precedes_contact_generating_actor_motor");
        check(b3_fixture(&f,model,NF18B3_DOOR_OPEN,1,1,0),"camera_disabled_fixture");
        f.p->model=NF18A6_WORLD_BRIDGE;(void)nf18b3_request(f.r,10,f.target,30,NF18B3_DOOR_OPEN);
        (void)b3_run(&f,120);const uint32_t without_camera=nf18b3_semantic_hash(f.r);
        check(b3_fixture(&f,model,NF18B3_DOOR_OPEN,1,1,0)&&
            nf18b3_request(f.r,10,f.target,30,NF18B3_DOOR_OPEN)==NF18B3_PROPOSED&&b3_run(&f,120)&&
            nf18b3_semantic_hash(f.r)==without_camera,"camera_on_off_identical_material_and_event_results");
    }
    printf("checks=%u failures=%u\n",checks,failures);free(before);b3_free(&f);return failures?1:0;
}
