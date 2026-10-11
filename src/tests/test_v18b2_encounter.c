#define _POSIX_C_SOURCE 200809L
/* SIGNAL: constructed outcomes, not a second call to the implementation as oracle.
   These fixtures enter nf_world_step_checked and the real A6/A4/A5 owner. */
#include "nf_encounter18b2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static unsigned checks,failures;
static void check(bool ok,const char *name){++checks;failures+=!ok;printf("%s,%s\n",name,ok?"PASS":"FAIL");}
typedef struct Fixture {NfWorld *world;Nf18a6Runtime *physical;Nf18b2EncounterRuntime *encounter;} Fixture;
static Nf18b2Intent intent(uint32_t action){return (Nf18b2Intent){10,2010,action,NF18B2_PUSH};}
static bool setup(Fixture *f,Nf18b2Model model){
    nf_world_init(f->world,42);
    nf_world_add_collider(f->world,NF_COLLIDER_SOLID,(NfVec3){0,-.5f,0},(NfVec3){4,0,4});
    if(nf_world_spawn_actor_with_id(f->world,10,NF_FACTION_PLAYER,(NfVec3){1,0,1})!=10)return false;
    NfActor *a=nf_world_find_actor(f->world,10);a->movement.grounded=true;a->movement.mode=NF_MOVE_GROUND;
    Nf18a4Body crate={.id=2010,.center={2.2f,.9f,1},.inverse_mass=1.f/24,
        .inverse_inertia={.3f,.3f,.3f},.box_half={.2f,.5f,.35f}};
    if(!nf18a6_init(f->physical,f->world,10,crate,3,NF18A6_WORLD_CAMERA))return false;
    uint8_t voxels[NF18A5_CANON_VOXELS]={0};
    if(!nf18a5_load_canonical(&f->physical->physical.world.grid,0,0,0,voxels,1))return false;
    if(!nf18a6_bind_world_tick(f->world,f->physical,NULL,NULL,NULL)||
       !nf18b2_encounter_init(f->encounter,f->world,f->physical,model)||
       !nf18b2_register_actor(f->encounter,10,1,1,10))return false;
    Nf18b2Object o={.id=2010,.revision=1,.cell_id=1,.nexus_id=2,.slot_id=3,
        .target={1.999f,1.62f,1},.max_reach=3,.required_credentials=1,.required_inventory=1,
        .capacity=1,.preconditions=1,.requires_support=1};
    return nf18b2_register_object(f->encounter,o,f->physical->physical.world.grid.global_revision,1,1)&&nf18b2_attach(f->encounter);
}
static bool no_effect(Fixture *f,const Nf18a5Integrated *p,uint32_t balance){
    return f->world->tick==p->world.tick&&memcmp(&f->physical->physical,p,sizeof(*p))==0&&
        f->encounter->registry.accounts[0].balance==balance&&!f->encounter->grant_count&&
        f->encounter->registry.objects[0].object.occupant_count==0;
}
static bool deny(const Nf18a5Integrated *p,void *ctx){(void)p;(void)ctx;return false;}
/* Deliberately violates the read-only validator obligation to inject a changed
   authoritative contract between begin and publication. Revalidation must win. */
static bool revoke_at_publication(const Nf18a5Integrated *p,void *ctx){
    (void)p;return nf18b2_set_contract(ctx,10,0,1);
}
int main(void){
    Fixture f={malloc(sizeof(NfWorld)),malloc(sizeof(Nf18a6Runtime)),malloc(sizeof(Nf18b2EncounterRuntime))};
    Nf18a5Integrated *before=malloc(sizeof(*before));
    if(!f.world||!f.physical||!f.encounter||!before)return 2;
    for(unsigned model=3;model<=4;++model){
        check(setup(&f,(Nf18b2Model)model),"EEC_setup_real_A6_owner");
        check(nf18b2_propose(f.encounter,intent(1))==NF18B2_ELIGIBLE,"EEC01_initially_eligible");
        *before=f.physical->physical;
        /* A canonical obstruction closes in the actual actor hull, after assessment. */
        f.physical->physical.world.grid.chunks[0].canonical[(1*4+1)*4+1]=NF18A5_SOLID;
        *before=f.physical->physical;
        check(nf_world_step_checked(f.world,1.f/60)!=NF_WORLD_TICK_COMMITTED&&no_effect(&f,before,10)&&
              f.encounter->traces[0].revalidation.status==NF18B2_BLOCKED,"EEC01_obstacle_closes_no_movement_debit");

        check(setup(&f,(Nf18b2Model)model),"EEC02_setup");
        check(nf18b2_propose(f.encounter,intent(2))==NF18B2_ELIGIBLE&&
              nf18b2_withdraw(f.encounter,10,2),"EEC02_actor_withdraws");
        *before=f.physical->physical;
        check(nf_world_step_checked(f.world,1.f/60)!=NF_WORLD_TICK_COMMITTED&&no_effect(&f,before,10)&&
            f.encounter->traces[0].state==NF18B2_CANCELLED,"EEC02_cancelled_no_receipt");

        check(setup(&f,(Nf18b2Model)model),"EEC03_setup");
        /* Real body registry snapshots; prospective same-frame arbitration only.
           A6's guarded one-moving-actor owner is not rebranded as B4 multiplayer. */
        for(unsigned i=11;i<=12;++i){
            nf_world_spawn_actor_with_id(f.world,i,NF_FACTION_TEAMMATE,(NfVec3){1,0,1.5f+(i-11)*.4f});
            nf_world_find_actor(f.world,i)->movement.grounded=true;
            nf18b2_register_actor(f.encounter,i,1,1,10);
        }
        f.encounter->registry.objects[0].object.capacity=2;
        Nf18b2LiveFrame frame;check(nf18b2_live_frame(f.encounter,&frame),"EEC03_server_registry_frame");
        Nf18b2Use requests[3];Nf18b2UseDecision results[3];
        for(unsigned i=0;i<3;++i)requests[i]=(Nf18b2Use){12-i,2010,20+i,1,
            frame.scene.material->revision,frame.scene.material->grid.global_revision,frame.actors[2-i].revision,frame.objects[0].revision};
        check(nf18b2_resolve(&frame.scene,requests,3,(Nf18b2Model)model,results)&&
            results[0].status==NF18B2_BLOCKED&&results[1].status==NF18B2_ELIGIBLE&&results[2].status==NF18B2_ELIGIBLE,
            "EEC03_exactly_two_prospective_grants_lower_actor_IDs");
        check(nf_world_step_checked(f.world,1.f/60)==NF_WORLD_TICK_REJECTED&&f.world->tick==0,
              "EEC03_live_multi_actor_execution_explicitly_out_of_scope");

        check(setup(&f,(Nf18b2Model)model),"EEC04_setup");
        f.encounter->registry.objects[0].object.requires_support=1;
        nf_world_find_actor(f.world,10)->movement.grounded=false;
        Nf18b2Intent ladder=intent(4);ladder.kind=NF18B2_LADDER;
        check(nf18b2_propose(f.encounter,ladder)==NF18B2_BLOCKED,"EEC04_contract_alone_no_support");
        *before=f.physical->physical;
        check(nf_world_step_checked(f.world,1.f/60)!=NF_WORLD_TICK_COMMITTED&&no_effect(&f,before,10),
              "EEC04_no_synthesized_climb");

        check(setup(&f,(Nf18b2Model)model),"EEC05_setup");
        check(nf18b2_propose(f.encounter,intent(5))==NF18B2_ELIGIBLE,"EEC05_permission_without_contact");
        nf_world_set_input(f.world,10,(NfMoveInput){.strafe=1});
        check(nf_world_step_checked(f.world,1.f/60)==NF_WORLD_TICK_COMMITTED&&f.encounter->traces[0].published&&
            f.encounter->traces[0].state==NF18B2_EXECUTING&&f.encounter->traces[0].applied_receipts==0&&
            f.encounter->registry.accounts[0].balance==9,"EEC05_commit_does_not_invent_completion");
        for(unsigned i=0;i<40&&!f.encounter->traces[0].applied_receipts;++i)
            (void)nf_world_step_checked(f.world,1.f/60);
        const uint32_t raw=f.physical->physical.world.history.raw_samples;
        const uint32_t physical_receipts=f.physical->physical.committed_receipts;
        check(raw>0&&physical_receipts>0&&f.encounter->traces[0].applied_receipts==1&&
            f.encounter->traces[0].state==NF18B2_COMPLETED,"EEC05_actual_A4_receipt_and_A5_history");
        check(nf18b2_interrupt(f.encounter,10,5)&&f.encounter->traces[0].state==NF18B2_COMPENSATION_REQUIRED&&
            f.physical->physical.world.history.raw_samples==raw&&
            f.physical->physical.committed_receipts==physical_receipts,
            "EEC05_interruption_preserves_fact_compensation_deferred");
        check(nf18b2_propose(f.encounter,intent(5))==NF18B2_DUPLICATE&&f.encounter->registry.accounts[0].balance==9,
            "EEC05_same_published_action_cannot_debit_again");

        check(setup(&f,(Nf18b2Model)model),"EEC06_setup");
        check(nf18b2_propose(f.encounter,intent(6))==NF18B2_ELIGIBLE&&nf18b2_touch_object(f.encounter,2010),
            "EEC06_object_state_changes");
        *before=f.physical->physical;
        check(nf_world_step_checked(f.world,1.f/60)!=NF_WORLD_TICK_COMMITTED&&no_effect(&f,before,10)&&
            f.encounter->traces[0].state==NF18B2_EXPIRED&&
            f.encounter->registry.objects[0].material_identity_epoch==before->world.grid.global_revision&&
            f.physical->physical.world.grid.global_revision==before->world.grid.global_revision,
            "EEC06_old_grant_expires_material_identity_preserved");

        check(setup(&f,(Nf18b2Model)model),"EEC07_setup");
        check(nf18b2_propose(f.encounter,intent(7))==NF18B2_ELIGIBLE,"EEC07_assess_before_failure");
        f.encounter->publication_validator=deny;*before=f.physical->physical;
        check(nf_world_step_checked(f.world,1.f/60)!=NF_WORLD_TICK_COMMITTED&&no_effect(&f,before,10)&&
            f.encounter->traces[0].state==NF18B2_FAILED_NO_EFFECT,"EEC07_failed_publication_atomic_no_effect");
        f.encounter->publication_validator=NULL;
        check(nf18b2_propose(f.encounter,intent(7))==NF18B2_ELIGIBLE,"EEC10_retry_rejected_action_new_witness");
        check(nf_world_step_checked(f.world,1.f/60)==NF_WORLD_TICK_COMMITTED&&
              f.encounter->grant_count==1&&f.encounter->registry.accounts[0].balance==9,
              "EEC10_retry_commits_once_no_carried_reservation");
        check(setup(&f,(Nf18b2Model)model),"EEC07_full_setup");
        check(nf18b2_propose(f.encounter,intent(70))==NF18B2_ELIGIBLE,"EEC07_assess_before_exhaustion");
        f.encounter->grant_count=64;*before=f.physical->physical;
        check(nf_world_step_checked(f.world,1.f/60)==NF_WORLD_TICK_PENDING&&
            memcmp(before,&f.physical->physical,sizeof(*before))==0&&f.encounter->registry.accounts[0].balance==10&&
            f.encounter->registry.objects[0].object.occupant_count==0,"EEC07_receipt_capacity_pending_no_eviction");

        check(setup(&f,(Nf18b2Model)model),"EEC08_setup");
        check(nf18b2_propose(f.encounter,intent(8))==NF18B2_ELIGIBLE,"EEC08_predicted_eligible");
        check(nf18b2_set_contract(f.encounter,10,0,1),"EEC08_contract_revoked");
        *before=f.physical->physical;(void)nf_world_step_checked(f.world,1.f/60);
        Nf18b2EncounterTrace view;Nf18b2Registry registry=f.encounter->registry;
        check(nf18b2_observe(f.encounter,0,&view)&&view.visual_correction&&view.state==NF18B2_EXPIRED&&
            no_effect(&f,before,10)&&memcmp(&registry,&f.encounter->registry,sizeof registry)==0,
            "EEC08_visual_prediction_corrected_observer_noninterference");

        check(setup(&f,(Nf18b2Model)model),"EEC09_setup");
        f.world->colliders[0].kind=NF_COLLIDER_MOVING_PLATFORM;f.world->colliders[0].velocity.x=1;
        check(nf18b2_propose(f.encounter,intent(9))==NF18B2_PENDING&&f.encounter->traces[0].deferred_milestone==3,
              "EEC09_dynamic_support_never_claimed_static");
        *before=f.physical->physical;
        check(nf_world_step_checked(f.world,1.f/60)!=NF_WORLD_TICK_COMMITTED&&no_effect(&f,before,10),
              "EEC09_no_B3_moving_support_fabrication");

        check(setup(&f,(Nf18b2Model)model),"publication_revision_setup");
        check(nf18b2_propose(f.encounter,intent(11))==NF18B2_ELIGIBLE,"publication_revision_assess");
        f.encounter->publication_validator=revoke_at_publication;f.encounter->publication_context=f.encounter;
        *before=f.physical->physical;
        check(nf_world_step_checked(f.world,1.f/60)!=NF_WORLD_TICK_COMMITTED&&no_effect(&f,before,10)&&
            f.encounter->traces[0].revalidation.status==NF18B2_STALE&&
            f.encounter->traces[0].state==NF18B2_FAILED_NO_EFFECT,"prepublication_contract_change_no_grant");
        check(!nf18a6_recover(f.physical,f.world,"unused"),"physical_only_recovery_rejected_with_B2_ledger");
        Nf18b2Object alias=f.encounter->registry.objects[0].object;alias.id=999;
        check(!nf18b2_register_object(f.encounter,alias,1,0,0),"typed_resource_alias_refused");
        check(setup(&f,(Nf18b2Model)model),"journal_failure_setup");
        check(nf18b2_propose(f.encounter,intent(12))==NF18B2_ELIGIBLE,"journal_failure_assess");
        char directory[]="/tmp/nightfall-b2-wal-XXXXXX";
        check(mkdtemp(directory)!=NULL,"journal_unwritable_target_directory_created");
        f.physical->tick_journal=directory;*before=f.physical->physical;
        check(nf_world_step_checked(f.world,1.f/60)!=NF_WORLD_TICK_COMMITTED&&
            f.physical->tick_trace.embodiment_status==NF18A6_DISK_FAILED&&no_effect(&f,before,10),
            "journal_write_failure_no_partial_body_balance_slots");
        rmdir(directory);
        check(setup(&f,(Nf18b2Model)model),"torn_physical_journal_setup");
        char path[]="/tmp/nightfall-b2-torn-XXXXXX";int fd=mkstemp(path);
        check(fd>=0,"torn_journal_file_created");if(fd>=0)close(fd);
        check(nf18a5_checkpoint(path,&f.physical->physical),"physical_snapshot_checkpoint");
        check(truncate(path,3)==0,"physical_journal_torn_tail_injected");
        *before=f.physical->physical;
        check(!nf18a5_restore(path,before),"torn_physical_journal_fails_closed_B5_not_claimed");
        unlink(path);
        check(setup(&f,(Nf18b2Model)model),"bounded_queue_setup");
        bool queued=true;
        for(unsigned i=1;i<=64;++i)queued&=nf18b2_propose(f.encounter,intent(100+i))==NF18B2_ELIGIBLE;
        check(queued&&f.encounter->queue_count==64&&nf18b2_propose(f.encounter,intent(200))==NF18B2_PENDING,
              "bounded_64_proposals_overflow_pending");
        check(nf_world_step_checked(f.world,1.f/60)==NF_WORLD_TICK_COMMITTED&&
            f.encounter->grant_count==1&&f.encounter->registry.accounts[0].balance==9&&
            f.encounter->registry.objects[0].object.occupant_count==1,
            "same_tick_same_actor_conflicts_one_grant_debit_slot");
        check(setup(&f,(Nf18b2Model)model),"withdrawn_contention_setup");
        check(nf18b2_propose(f.encounter,intent(1))==NF18B2_ELIGIBLE&&
            nf18b2_propose(f.encounter,intent(2))==NF18B2_ELIGIBLE&&nf18b2_withdraw(f.encounter,10,1),
            "withdrawn_lower_action_before_resolution");
        check(nf_world_step_checked(f.world,1.f/60)==NF_WORLD_TICK_COMMITTED&&
            f.encounter->grant_count==1&&f.encounter->grant_actions[0]==2&&
            f.encounter->traces[0].state==NF18B2_CANCELLED&&f.encounter->traces[1].published&&
            f.encounter->registry.accounts[0].balance==9,"withdrawal_releases_same_tick_contention");
        check(setup(&f,(Nf18b2Model)model),"moving_ladder_epoch_setup");
        const int ladder_index=nf_world_add_collider(f.world,NF_COLLIDER_LADDER,
            (NfVec3){3,0,3},(NfVec3){3.1f,2,3.1f});
        check(ladder_index>=0&&nf18b2_propose(f.encounter,intent(201))==NF18B2_ELIGIBLE,"ladder_epoch_assessed");
        f.world->colliders[ladder_index].max.y+=1;*before=f.physical->physical;
        check(nf_world_step_checked(f.world,1.f/60)!=NF_WORLD_TICK_COMMITTED&&no_effect(&f,before,10)&&
            f.encounter->traces[0].state==NF18B2_EXPIRED,"ladder_geometry_change_expires_old_snapshot");
    }
    printf("checks=%u failures=%u\n",checks,failures);
    free(before);free(f.encounter);free(f.physical);free(f.world);return failures?1:0;
}
