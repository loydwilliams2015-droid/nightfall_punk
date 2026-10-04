#define _POSIX_C_SOURCE 200809L
#include "nf_contact18a5_close.h"
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static unsigned total=0,fail=0;
static void gate(bool ok,const char *name){
    ++total;printf("%s,%s\n",name,ok?"PASS":"FAIL");if(!ok)++fail;
}
static Nf18a4Body body(uint32_t id,float mass,NfVec3 center){
    Nf18a4Body b={0};b.id=id;b.center=center;
    b.inverse_mass=mass>0?1.0f/mass:0.0f;
    if(mass>0)b.inverse_inertia=(NfVec3){1,1,1};
    b.radius=.3f;b.height=1.8f;b.box_half=(NfVec3){.5f,.9f,.8f};return b;
}
static int provider_calls;
static bool provider(void *ctx,int32_t x,int32_t y,int32_t z,uint32_t parent,
                     uint8_t *scale,uint8_t *voxels,size_t *count){
    ++provider_calls;
    int mode=*(int*)ctx;
    if(mode==0||x!=0||y!=0||z!=0)return false;
    (void)parent;
    *scale=2;*count=512;memset(voxels,0,512);
    voxels[27]=1; /* canonical (1,0,1) solid fine voxel at x>=1.5,z>=1.5 */
    if(mode==2)voxels[26]=1; /* extra fine obstruction for negative test */
    if(mode==3){*scale=4;*count=512;} /* invalid refinement length */
    if(mode==4){voxels[27]=0;voxels[19]=1;} /* direct route obstruction */
    return true;
}
static void canon(Nf18a5Integrated *s){
    uint8_t c[NF18A5_CANON_VOXELS]={0};c[5]=NF18A5_MIXED;
    if(!nf18a5_load_canonical(&s->world.grid,0,0,0,c,1))abort();
}
int main(void){
    Nf18a5Integrated a;
    nf18a5_integrated_init(&a,3,body(1,80,(NfVec3){1,.9f,1}),
                          body(2,40,(NfVec3){2.2f,.9f,1}));
    canon(&a);
    const uint32_t physical_epoch=a.world.grid.global_revision;
    const NfVec3 pass={1.1f,.1f,1.1f},blocked={1.6f,.1f,1.6f};
    const Nf18a2ShapePolicy shape={.3f,1.8f,.1f,.4f,.7f};
    const Nf18a4Motor motor={{120,0,0},200000,5000};
    int mode=0;uint32_t loads=99;
    Nf18a5Query q=nf18a5_resolve_exact(&a.world.grid,pass,17,provider,&mode,&loads);
    gate(q==NF18A5_Q_PENDING&&loads==0,"I01_cache_miss_is_pending_not_free");
    gate(a.world.grid.global_revision==physical_epoch,"I02_cache_miss_does_not_edit_material");
    mode=3;q=nf18a5_resolve_exact(&a.world.grid,pass,17,provider,&mode,&loads);
    gate(q==NF18A5_Q_PENDING,"I03_invalid_payload_fails_closed");
    mode=1;q=nf18a5_resolve_exact(&a.world.grid,pass,17,provider,&mode,&loads);
    gate(q==NF18A5_Q_FREE&&loads==1,"I04_loaded_fine_answers_exact_query");
    gate(a.world.grid.global_revision==physical_epoch&&a.world.grid.cache_revision>1,"I05_cache_and_material_epochs_disjoint");
    q=nf18a5_resolve_exact(&a.world.grid,blocked,17,provider,&mode,&loads);
    gate(q==NF18A5_Q_SOLID&&loads==0,"I06_loaded_fine_obstacle_not_erased");
    gate(nf18a5_release_fine(&a.world.grid,0,0,0),"I07_released_fine_cache");
    q=nf18a5_resolve_exact(&a.world.grid,blocked,17,NULL,NULL,&loads);
    gate(q==NF18A5_Q_PENDING,"I08_evicted_fine_never_false_free");
    Nf18a5Integrated before=a;
    Nf18a5CloseResult r=nf18a5_integrated_pair_step(&a,1,17,pass,NULL,NULL,shape,
                       motor,1.0f/60,0,0,NULL);
    gate(r.status==NF18A5_CLOSE_PENDING&&a.world.revision==before.world.revision,
         "I09_missing_exact_data_stalls_commit");
    gate(a.object.velocity.x==0&&a.world.history.raw_samples==0,
         "I10_pending_does_not_stage_impulse_or_history");
    r=nf18a5_integrated_pair_step(&a,1,17,blocked,provider,&mode,shape,
                       motor,1.0f/60,0,0,NULL);
    gate(r.status==NF18A5_CLOSE_BLOCKED&&a.world.revision==1,
         "I11_real_fine_solid_blocks_dynamic_step");
    gate(a.object.velocity.x==0,"I12_blocked_contact_no_false_impulse");
    /* Even if the point witness is clear, a solid neighbor inside the swept
       actor's hull must stop the full motion. */
    Nf18a5Integrated wall=a;
    uint8_t solid_cells[NF18A5_CANON_VOXELS]={0};solid_cells[5]=NF18A5_SOLID;
    gate(nf18a5_load_canonical(&wall.world.grid,0,0,0,solid_cells,17),
         "I37_material_edit_invalidates_fine");
    gate(nf18a5_query(&wall.world.grid,pass.x,pass.y,pass.z,17,true)==NF18A5_Q_SOLID,
         "I38_material_revision_replaces_previous_free_cache");
    NfVec3 clear_probe={2.1f,.1f,1.1f};
    r=nf18a5_integrated_pair_step(&wall,1,18,clear_probe,provider,&mode,
                                 shape,motor,1.0f/60,0,0,NULL);
    gate(r.status==NF18A5_CLOSE_BLOCKED&&wall.object.velocity.x==0,
         "I39_clear_point_cannot_authorize_swept_capsule_through_wall");
    Nf18a5Integrated fine_block=a;
    mode=4;
    r=nf18a5_integrated_pair_step(&fine_block,1,17,pass,provider,&mode,
                                  shape,motor,1.0f/60,0,0,NULL);
    gate(r.status==NF18A5_CLOSE_BLOCKED&&fine_block.world.revision==1,
         "I40_adjacent_fine_solid_blocks_capsule_even_when_probe_free");
    Nf18a5Integrated stale_load=a;
    mode=0;
    r=nf18a5_integrated_pair_step(&stale_load,1,17,clear_probe,provider,&mode,
                                  shape,motor,1.0f/60,0,0,NULL);
    gate(r.status==NF18A5_CLOSE_PENDING&&stale_load.world.revision==1,
         "I41_clear_probe_does_not_bypass_unloaded_MIXED_sweep");
    mode=1;
    char journal[]="/tmp/nf18a5_closure_XXXXXX";
    int fd=mkstemp(journal);if(fd<0)return 2;close(fd);
    r=nf18a5_integrated_pair_step(&a,1,17,pass,provider,&mode,shape,
                       motor,1.0f/60,0,0,journal);
    gate(r.status==NF18A5_CLOSE_COMMITTED&&r.dynamic_contact,
         "I13_loaded_clear_uses_actual_A4_dynamic_contact");
    gate(a.object.velocity.x>0&&a.actor.velocity.x>0,
         "I14_both_body_velocities_physically_updated");
    gate(a.world.revision==2&&a.world.history.raw_samples==1&&a.committed_receipts==1,
         "I15_body_and_receipt_history_commit_as_one");
    gate(r.material_revision_after==physical_epoch,
         "I16_fine_load_cannot_forge_material_revision");
    Nf18a5Integrated restored={0};
    gate(nf18a5_restore(journal,&restored),"I17_durable_world_snapshot_recovered");
    gate(restored.object.velocity.x==a.object.velocity.x&&
         restored.world.history.raw_samples==a.world.history.raw_samples&&
         restored.world.revision==a.world.revision,
         "I18_recovery_preserves_body_material_and_history");
    before=a;
    r=nf18a5_integrated_pair_step(&a,1,17,pass,provider,&mode,shape,
                       motor,1.0f/60,0,0,journal);
    gate(r.status==NF18A5_CLOSE_STALE&&a.world.revision==before.world.revision,
         "I19_duplicate_world_commit_rejected");
    gate(nf18a5_checkpoint(journal,&a),
         "I42_identical_post_fsync_replay_is_idempotent");
    Nf18a5Integrated conflicting=a;
    conflicting.actor.velocity.x+=1.0f;
    gate(!nf18a5_checkpoint(journal,&conflicting),
         "I43_same_version_conflicting_snapshot_rejected");
    gate(nf18a5_restore(journal,&restored)&&
         restored.actor.velocity.x==a.actor.velocity.x,
         "I44_conflicting_checkpoint_does_not_corrupt_recovery");
    FILE *f=fopen(journal,"ab");if(!f)return 3;fputs("torn",f);fclose(f);
    restored=before;
    gate(!nf18a5_restore(journal,&restored),"I22_torn_world_snapshot_fails_closed");
    gate(restored.world.revision==before.world.revision,
         "I23_bad_replay_leaves_state_unchanged");
    gate(!nf18a5_checkpoint(journal,&a),"I24_corrupt_log_rejects_new_append");
    unlink(journal);

    /* Continuous support: actual gravity-induced normal impulses are applied
       each tick, then condensed as FORCE-TIME, not repeated impact hits. */
    Nf18a5Integrated sup;
    nf18a5_integrated_init(&sup,1,body(5,80,(NfVec3){0,.9f,0}),
                           body(6,0,(NfVec3){0,-.5f,0}));
    sup.world.history.peak_threshold=20;
    sup.world.history.cumulative_threshold=100000;
    sup.world.history.load_threshold=100000;
    sup.object.box_half=(NfVec3){.5f,.5f,.5f};
    Nf18a4Constraint ground={5,6,21,{0,1,0},{0,0,0},0,.5f};
    Nf18a3Contract auth={0};
    auth.actor_id=5;auth.feature_id=6;auth.current_support_id=6;
    auth.actor_grounded=1;auth.material_support_valid=1;auth.support_stable=1;
    auth.authoritative_body_radius=.3f;auth.authoritative_body_height=1.8f;
    auth.authoritative_foot_flat_radius=.1f;
    auth.authoritative_feet=(NfVec3){0,0,0};
    auth.world_version=sup.world.revision;auth.tick=1;
    Nf18a3Contract spoof=auth;spoof.current_support_id=999;
    Nf18a5CloseResult bad_support=nf18a5_integrated_support_step(&sup,1,1,
                 ground,spoof,(NfVec3){0,-9.81f,0},1.0f/60,NULL);
    gate(bad_support.status==NF18A5_CLOSE_GATED&&sup.world.tick==0,
         "I45_spoofed_support_claim_rejected");
    Nf18a5Integrated floating=sup;floating.object.center.y=-2;
    bad_support=nf18a5_integrated_support_step(&floating,1,1,ground,auth,
                   (NfVec3){0,-9.81f,0},1.0f/60,NULL);
    gate(bad_support.status==NF18A5_CLOSE_GATED&&floating.world.tick==0,
         "I46_no_physical_contact_no_resting_load");
    before=sup;
    r=nf18a5_integrated_support_step(&sup,sup.world.revision,1,ground,auth,
            (NfVec3){0,-9.81f,0},1.0f/60,
            "/this_directory_must_not_exist/nf.log");
    gate(r.status==NF18A5_CLOSE_DISK_FAILED&&sup.world.revision==before.world.revision,
         "I20_disk_failure_rolls_back_body_and_history");
    gate(sup.actor.velocity.y==before.actor.velocity.y&&
         nf18a5_history_hash(&sup.world.history)==nf18a5_history_hash(&before.world.history),
         "I21_failed_checkpoint_no_partial_state");
    unsigned committed=0;
    for(uint32_t tick=1;tick<=61;++tick){
        auth.world_version=sup.world.revision;auth.tick=tick;
        r=nf18a5_integrated_support_step(&sup,sup.world.revision,tick,
                          ground,auth,(NfVec3){0,-9.81f,0},1.0f/60,NULL);
        if(r.status!=NF18A5_CLOSE_COMMITTED||!r.dynamic_contact)break;
        if(tick==1){
            gate(r.applied.impulses_applied&&r.external_impulse.y<0,
                 "I25_external_gravity_and_applied_support_impulse");
            gate(sup.world.history.raw_samples==1&&
                 sup.world.history.streams[0].window.impulse_integral==0.0,
                 "I26_support_impulse_not_duplicated_as_impact");
        }
        ++committed;
    }
    gate(committed==61,"I27_persistent_support_from_real_solver_61_ticks");
    gate(sup.world.history.outbox_count==1&&
         (sup.world.history.outbox[0].reason_mask & NF18A5_REASON_DURATION)!=0,
         "I28_cumulative_duration_creates_consequential_event");
    gate((sup.world.history.outbox[0].reason_mask & NF18A5_REASON_PEAK)==0&&
         sup.world.history.outbox[0].impulse_sum==0.0&&
         sup.world.history.outbox[0].force_time>700.0,
         "I29_resting_load_not_violent_impact");
    gate(sup.world.history.outbox[0].material_epoch==sup.world.grid.global_revision &&
         sup.committed_receipts==61,"I30_event_provenance_matches_61_applied_receipts");
    char sink[]="/tmp/nf18a5_closure_sink_XXXXXX";
    fd=mkstemp(sink);if(fd<0)return 4;close(fd);
    Nf18a5History crash_copy=sup.world.history;
    gate(nf18a5_persist_outbox(&sup.world.history,sink),
         "I31_consequential_event_fsync_then_ack");
    struct stat_unused_placeholder{int x;}; /* avoid platform-only stat requirement */
    FILE *sfile=fopen(sink,"rb");if(!sfile)return 5;
    fseek(sfile,0,SEEK_END);long first_size=ftell(sfile);fclose(sfile);
    gate(nf18a5_persist_outbox(&crash_copy,sink),
         "I32_crash_after_append_before_ack_replayed_safely");
    sfile=fopen(sink,"rb");if(!sfile)return 6;
    fseek(sfile,0,SEEK_END);long second_size=ftell(sfile);fclose(sfile);
    gate(first_size==second_size,"I33_replay_no_duplicate_durable_event");
    unlink(sink);

    /* Reserve exhausted means atomic rollback including material/history. */
    Nf18a5Integrated over;
    nf18a5_integrated_init(&over,1,body(5,80,(NfVec3){0,.9f,0}),
                           body(6,0,(NfVec3){0,-.5f,0}));
    over.object.box_half=(NfVec3){.5f,.5f,.5f};
    over.world.history.load_threshold=1.0f;
    for(uint32_t t=1;t<=15;++t){
      auth.world_version=over.world.revision;auth.tick=t;
      r=nf18a5_integrated_support_step(&over,over.world.revision,t,ground,auth,
                            (NfVec3){0,-9.81f,0},1.0f/60,NULL);
      if(r.status!=NF18A5_CLOSE_COMMITTED)break;
    }
    gate(over.world.tick==15,"I34_full_queue_setup_15_support_ticks");
    over.world.history.outbox_count=NF18A5_OUTBOX;
    Nf18a5Integrated rollback=over;
    auth.world_version=over.world.revision;auth.tick=16;
    r=nf18a5_integrated_support_step(&over,over.world.revision,16,ground,auth,
                                    (NfVec3){0,-9.81f,0},1.0f/60,NULL);
    gate(r.status==NF18A5_CLOSE_INVALID&&over.world.tick==15,
         "I35_full_outbox_refuses_physical_commit");
    gate(over.actor.velocity.y==rollback.actor.velocity.y&&
         nf18a5_history_hash(&over.world.history)==nf18a5_history_hash(&rollback.world.history),
         "I36_full_queue_no_history_or_body_loss");
    printf("CLOSURE,%u,%u\n",total,fail);
    return fail?1:0;
}
