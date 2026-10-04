#define _POSIX_C_SOURCE 200809L
#include "nf_embody18a6.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
static unsigned passed,failed;
static void check(bool ok,const char *name){printf("%s,%s\n",name,ok?"PASS":"FAIL");if(ok)++passed;else ++failed;}
static Nf18a4Body crate(float x){Nf18a4Body b={0};b.id=2001;b.center=(NfVec3){x,.9f,1};
 b.inverse_mass=1.f/24.f;b.inverse_inertia=(NfVec3){.3f,.3f,.3f};
 b.box_half=(NfVec3){.2f,.5f,.35f};return b;}
static bool setup(NfWorld *w,Nf18a6Runtime *r,Nf18a6Model model,bool load){
 nf_world_init(w,42);
 nf_world_add_collider(w,NF_COLLIDER_SOLID,(NfVec3){0,-.5f,0},(NfVec3){4,0,4});
 NfEntityId id=nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,(NfVec3){1,0,1});
 if(id!=10)return false;
 NfActor *a=nf_world_find_actor(w,id);
 a->movement.grounded=true;a->movement.mode=NF_MOVE_GROUND;
 if(!nf18a6_init(r,w,id,crate(2.2f),3,model))return false;
 if(load){uint8_t free_cells[NF18A5_CANON_VOXELS]={0};
   if(!nf18a5_load_canonical(&r->physical.world.grid,0,0,0,free_cells,1))return false;}
 return true;
}
static NfMoveInput go(void){NfMoveInput i={0};i.strafe=1;return i;}
int main(void){
 NfWorld *w=malloc(sizeof(*w));Nf18a6Runtime *r=malloc(sizeof(*r));
 Nf18a6Runtime *r2=malloc(sizeof(*r2));NfWorld *w2=malloc(sizeof(*w2));
 Nf18a6Runtime *r3=malloc(sizeof(*r3));NfWorld *w3=malloc(sizeof(*w3));
 if(!w||!r||!r2||!w2||!r3||!w3)return 2;
 check(setup(w,r,NF18A6_WORLD_CAMERA,true),"E01_real_world_actor_initialized");
 check(r->physical.actor.id==10 && r->physical.object.id==2001,"E02_real_dynamic_ids");
 check(r->physical.world.grid.global_revision==2,"E03_actual_canonical_data_loaded");
 Nf18a6Outcome a=nf18a6_step(r,w,go(),NULL,NULL,1.f/60,NULL);
 check(a.status==NF18A6_COMMITTED,"E04_first_world_tick_commits");
 check(w->tick==1 && r->physical.world.tick==1,"E05_world_physical_tick_sync");
 check(nf_world_find_actor(w,10)->transform.position.x>1.f,"E06_real_actor_transform_moves");
 check(fabsf(r->camera.desired_anchor.x-nf_world_find_actor(w,10)->transform.position.x)<1e-5,"E07_camera_follows_committed_pose");
 check(a.camera_updated && r->camera.initialized,"E08_presentation_initialized");
 check(!nf18a6_snapshot_matches(&(Nf18a6Snapshot){0},&(Nf18a6Snapshot){0}),"E09_uninitialized_snapshot_rejected");
 Nf18a6Snapshot snap=nf18a6_snapshot(r,w);
 check(snap.tick==1&&snap.actor_id==10,"E10_authoritative_snapshot");
 check(nf18a6_snapshot_matches(&snap,&snap),"E11_snapshot_self_agreement");
 Nf18a6Snapshot altered=snap;altered.material_epoch++;
 check(!nf18a6_snapshot_matches(&snap,&altered),"E12_material_mismatch_reconciles");
 altered=snap;altered.contact_digest++;
 check(!nf18a6_snapshot_matches(&snap,&altered),"E13_contact_digest_mismatch_reconciles");
 altered=snap;altered.feet.x+=.02f;
 check(!nf18a6_snapshot_matches(&snap,&altered),"E14_pose_mismatch_reconciles");
 Nf18a6Outcome status=nf18a6_step(r,w,go(),NULL,NULL,.04f,NULL);
 check(status.status==NF18A6_INVALID && w->tick==1,"E15_wrong_timestep_fail_closed");
 NfMoveInput illegal=go();illegal.jump_pressed=true;
 check(nf18a6_step(r,w,illegal,NULL,NULL,1.f/60,NULL).status==NF18A6_UNSUPPORTED,"E16_jump_not_falsely_integrated");
 illegal=go();illegal.crouch_held=true;
 check(nf18a6_step(r,w,illegal,NULL,NULL,1.f/60,NULL).status==NF18A6_UNSUPPORTED,"E17_crouch_not_falsely_integrated");
 check(setup(w2,r2,NF18A6_WORLD_CAMERA,true),"E18_duplicate_fixture_ready");
 Nf18a6Outcome a2=nf18a6_step(r2,w2,go(),NULL,NULL,1.f/60,NULL);
 check(a.status==a2.status&&nf18a6_snapshot_matches(&snap,&(Nf18a6Snapshot){
 .tick=1,.revision=r2->physical.world.revision,.material_epoch=r2->physical.world.grid.global_revision,
 .contact_digest=nf18a5_history_hash(&r2->physical.world.history),.actor_id=10,
 .state_hash=r2->last_hash,.feet=nf_world_find_actor(w2,10)->transform.position,
 .velocity=nf_world_find_actor(w2,10)->transform.velocity}),"E19_equal_seed_replay");
 w2->tick++;
 check(nf18a6_step(r2,w2,go(),NULL,NULL,1.f/60,NULL).status==NF18A6_STALE,
       "E20_conflicting_world_tick_rejected");
 check(setup(w2,r2,NF18A6_WORLD_BRIDGE,false),"E21_missing_canonical_fixture_ready");
 Nf18a6Outcome miss=nf18a6_step(r2,w2,go(),NULL,NULL,1.f/60,NULL);
 check(miss.status==NF18A6_PENDING && w2->tick==0&&r2->physical.world.revision==1,
       "E22_unloaded_canonical_never_free");
 check(nf_world_find_actor(w2,10)->transform.position.x==1,"E23_pending_preserves_actor");
 check(setup(w2,r2,NF18A6_WORLD_BRIDGE,true),"E24_colliding_legacy_fixture_ready");
 nf_world_add_collider(w2,NF_COLLIDER_SOLID,(NfVec3){1.30f,0,.5f},
                                    (NfVec3){1.4f,2,1.5f});
 status=nf18a6_step(r2,w2,go(),NULL,NULL,1.f/60,NULL);
 check(status.status==NF18A6_BLOCKED && w2->tick==0,"E25_world_collider_not_erased_by_free_cell");
 check(setup(w2,r2,NF18A6_WORLD_BRIDGE,true),"E26_ramp_fixture_ready");
 nf_world_add_ramp(w2,(NfVec3){1,0,1},(NfVec3){2,1,2},NF_RAMP_POS_X);
 check(nf18a6_step(r2,w2,go(),NULL,NULL,1.f/60,NULL).status==NF18A6_UNSUPPORTED,
       "E27_ramp_requires_explicit_narrowphase");
 check(setup(w2,r2,NF18A6_WORLD_BRIDGE,true),"E28_actor_collision_fixture_ready");
 nf_world_spawn_actor_with_id(w2,77,NF_FACTION_RIVAL,(NfVec3){1.8f,0,1});
 check(nf18a6_step(r2,w2,go(),NULL,NULL,1.f/60,NULL).status==NF18A6_UNSUPPORTED,
       "E29_actor_actor_not_silently_ignored");
 check(setup(w2,r2,NF18A6_WORLD_BRIDGE,true),"E30_platform_fixture_ready");
 nf_world_add_moving_platform(w2,(NfVec3){2,1,1},(NfVec3){3,1.3f,2},(NfVec3){0,1,0},.5f,2);
 check(nf18a6_step(r2,w2,go(),NULL,NULL,1.f/60,NULL).status==NF18A6_UNSUPPORTED,
       "E31_moving_platform_not_silently_static");
 check(setup(w2,r2,NF18A6_WORLD_BRIDGE,true),"E32_disk_fixture_ready");
 const NfVec3 pre=nf_world_find_actor(w2,10)->transform.position;
 status=nf18a6_step(r2,w2,go(),NULL,NULL,1.f/60,"/dev/null/a6-invalid");
 check(status.status==NF18A6_DISK_FAILED && w2->tick==0 &&
       nf_world_find_actor(w2,10)->transform.position.x==pre.x,
       "E33_durable_failure_preserves_actor_world");
 check(setup(w2,r2,NF18A6_LAB_ONLY,true),"E34_lab_only_fixture_ready");
 status=nf18a6_step(r2,w2,go(),NULL,NULL,1.f/60,NULL);
 check(status.status==NF18A6_COMMITTED && w2->tick==0,"E35_lab_only_cannot_claim_embodiment");
 check(nf18a6_snapshot(r2,w2).tick==0,"E36_uncommitted_actor_cannot_be_snapshotted");
 check(setup(w2,r2,NF18A6_LEGACY_REFERENCE,true),"E37_legacy_control_ready");
 status=nf18a6_step(r2,w2,go(),NULL,NULL,1.f/60,NULL);
 check(status.status==NF18A6_COMMITTED && r2->physical.world.history.raw_samples==0,
       "E38_legacy_controller_is_not_dynamic_history");
 check(setup(w2,r2,NF18A6_WORLD_BRIDGE,true),"E39_world_bridge_ready");
 unsigned dynamic=0;
 for(unsigned k=0;k<35;++k){
   status=nf18a6_step(r2,w2,go(),NULL,NULL,1.f/60,NULL);
   if(status.status!=NF18A6_COMMITTED)break;
   if(status.actual_impulse)++dynamic;
 }
 check(dynamic>0,"E40_actual_contact_reaches_world_bridge");
 check(r2->physical.object.velocity.x>0,"E41_real_crate_receives_impulse");
 check(r2->physical.world.history.raw_samples>0,"E42_impulses_enter_committed_history");
 check(nf18a6_snapshot(r2,w2).tick==w2->tick,"E43_world_snapshot_after_contact");
 check(nf18a6_status_name(NF18A6_PENDING)[0]=='P',"E44_status_names_machine_readable");
 check(setup(w2,r2,NF18A6_WORLD_CAMERA,false),"E45_unloaded_import_fixture_ready");
 check(nf18a6_step(r2,w2,go(),NULL,NULL,1.f/60,NULL).status==NF18A6_PENDING,
       "E46_unloaded_prevents_world_advance");
 uint8_t new_parent[NF18A5_CANON_VOXELS]={0};
 check(nf18a5_load_canonical(&r2->physical.world.grid,0,0,0,new_parent,1),
       "E47_explicit_authoritative_parent_loaded");
 check(nf18a6_step(r2,w2,go(),NULL,NULL,1.f/60,NULL).status==NF18A6_COMMITTED,
       "E48_pending_resolution_requeries_actual_world");
 check(setup(w2,r2,NF18A6_WORLD_CAMERA,true) &&
       setup(w3,r3,NF18A6_WORLD_CAMERA,true),"E49_recovery_fixtures_ready");
 char walpath[]="/tmp/nf18a6_world_XXXXXX";
 int descriptor=mkstemp(walpath);
 check(descriptor>=0,"E50_local_journal_path_created");
 if(descriptor>=0){close(descriptor);unlink(walpath);}
 const Nf18a6Outcome durable=nf18a6_step(r2,w2,go(),NULL,NULL,1.f/60,walpath);
 check(durable.status==NF18A6_COMMITTED,"E51_atomic_journal_success");
 check(nf18a6_recover(r3,w3,walpath),"E52_material_actor_history_recovered");
 check(nf18a6_snapshot_matches(&(Nf18a6Snapshot){.tick=(uint32_t)w2->tick,
    .revision=r2->physical.world.revision,.material_epoch=r2->physical.world.grid.global_revision,
    .contact_digest=nf18a5_history_hash(&r2->physical.world.history),.actor_id=10,
    .state_hash=r2->last_hash,.feet=nf_world_find_actor(w2,10)->transform.position,
    .velocity=nf_world_find_actor(w2,10)->transform.velocity},
    &(Nf18a6Snapshot){.tick=(uint32_t)w3->tick,.revision=r3->physical.world.revision,
    .material_epoch=r3->physical.world.grid.global_revision,
    .contact_digest=nf18a5_history_hash(&r3->physical.world.history),.actor_id=10,
    .state_hash=r3->last_hash,.feet=nf_world_find_actor(w3,10)->transform.position,
    .velocity=nf_world_find_actor(w3,10)->transform.velocity}),
    "E53_recovered_snapshot_agrees");
 check(!nf18a6_recover(r3,w3,walpath),"E54_no_duplicate_recovery_without_newer_commit");
 unlink(walpath);
 printf("TOTAL,%u,%u\n",passed,failed);
 free(w);free(w2);free(w3);free(r);free(r2);free(r3);
 return failed?1:0;
}
