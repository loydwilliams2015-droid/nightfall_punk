#include "nf_contact18a5.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static unsigned total=0u, failures=0u;
static void gate(bool ok,const char *name){
  ++total; printf("%s,%s\n",name,ok?"PASS":"FAIL");if(!ok)++failures;
}
static void canonical(uint8_t c[64]){memset(c,0,64);c[0]=NF18A5_MIXED;c[1]=NF18A5_SOLID;}
static void fine(uint8_t *f,unsigned scale){
    const unsigned n=4u*scale;
    memset(f,0,n*n*n);
    for(unsigned y=0;y<scale;++y)for(unsigned z=0;z<scale;++z)for(unsigned x=0;x<scale;++x){
        f[(y*n+z)*n+x]=(x==0u)?NF18A5_SOLID:NF18A5_FREE;
        f[(y*n+z)*n+x+scale]=NF18A5_SOLID;
    }
}
static Nf18a5Sample sample(uint32_t tick,uint32_t contact,float impulse,float load){
    return (Nf18a5Sample){tick,contact,42u,7u,impulse,load,0.0f,1u};
}
static void commit_range(Nf18a5History *h,unsigned start,unsigned end,uint32_t contact,float impulse,float load){
    for(unsigned tick=start;tick<=end;++tick){Nf18a5Sample s=sample(tick,contact,impulse,load);
      if(!nf18a5_history_commit(h,tick,tick,42u,&s,1u)){fprintf(stderr,"commit failure at %u\n",tick);++failures;}
    }
}
int main(void){
    Nf18a5Grid g;uint8_t c[64],f[4096];canonical(c);fine(f,4);
    nf18a5_grid_init(&g,2,NF18A5_HYSTERETIC_FINE);
    gate(g.capacity==2&&g.global_revision==1u,"G01_initial_canonical_state");
    gate(nf18a5_load_canonical(&g,0,0,0,c,1),"G02_load_authoritative_3d_chunk");
    gate(nf18a5_query(&g,0.9f,0.2f,0.2f,1,true)==NF18A5_Q_PENDING,"G03_unresident_mixed_is_pending");
    gate(nf18a5_query(&g,1.5f,0.5f,0.5f,1,true)==NF18A5_Q_SOLID,"G04_canonical_solid_never_free");
    gate(nf18a5_query(&g,3.2f,0.2f,0.2f,1,true)==NF18A5_Q_FREE,"G05_canonical_free_without_fine");
    gate(!nf18a5_load_fine(&g,0,0,0,4,f,4096,10,1),"G06_reject_stale_fine_parent");
    gate(nf18a5_load_fine(&g,0,0,0,4,f,4096,1,1),"G07_load_genuine_fine_witness");
    gate(nf18a5_query(&g,0.35f,0.2f,0.2f,2,true)==NF18A5_Q_FREE,"G08_fine_subvoxel_free");
    gate(nf18a5_query(&g,0.05f,0.2f,0.2f,2,true)==NF18A5_Q_SOLID,"G09_fine_subvoxel_solid");
    gate(nf18a5_query(&g,1.01f,0.2f,0.2f,2,true)==NF18A5_Q_SOLID,"G10_parent_solid_unchanged");
    gate(nf18a5_resident_bytes(&g)==4160u,"G11_resident_memory_count");
    gate(!nf18a5_load_fine(&g,0,0,0,4,f,4080,1,2),"G12_reject_truncated_fine");
    f[(0u*16u+0u)*16u+4u]=0u;
    gate(!nf18a5_load_fine(&g,0,0,0,4,f,4096,1,2),"G13_reject_solid_parent_contradiction");
    fine(f,4);
    gate(nf18a5_load_canonical(&g,0,0,0,c,3),"G14_canonical_rewrite_invalidate_fine");
    gate(nf18a5_query(&g,0.35f,0.2f,0.2f,3,true)==NF18A5_Q_PENDING,"G15_no_stale_fine_free_after_rewrite");
    gate(nf18a5_load_fine(&g,0,0,0,4,f,4096,2,3),"G16_reload_updated_fine");
    gate(nf18a5_query(&g,0.35f,0.2f,0.2f,3,true)==NF18A5_Q_FREE,"G17_updated_fine_usable");
    gate(nf18a5_query(&g,4.2f,0.2f,0.2f,4,true)==NF18A5_Q_PENDING,"G18_missing_canonical_not_free");
    gate(nf18a5_load_canonical(&g,-1,0,0,c,5),"G19_negative_chunk_coord_load");
    gate(nf18a5_query(&g,-3.65f,0.2f,0.2f,5,true)==NF18A5_Q_PENDING,"G20_negative_coord_correct_chunk");
    gate(!nf18a5_load_canonical(&g,2,0,0,c,5),"G21_capacity_exhaustion_explicit");
    gate(nf18a5_pin(&g,0,0,0,true),"G22_pin_active_chunk");
    gate(nf18a5_evict(&g,100,10)&&g.count==1,"G23_idle_evict_not_pinned");
    gate(nf18a5_query(&g,0.35f,0.2f,0.2f,101,true)==NF18A5_Q_FREE,"G24_pinned_chunk_preserved");
    gate(!nf18a5_evict(&g,120,10),"G25_no_pinned_eviction");
    gate(nf18a5_query(&g,NAN,0,0,101,true)==NF18A5_Q_INVALID,"G26_nonfinite_rejected");
    gate(nf18a5_load_canonical(&g,0,1,0,c,123),"G27_3d_y_chunk_distinct");
    gate(nf18a5_query(&g,0.3f,4.1f,0.3f,123,true)==NF18A5_Q_PENDING,"G28_3d_y_query_distinct");
    nf18a5_grid_init(&g,1,NF18A5_UNSAFE_FINE_CONTROL);
    gate(nf18a5_load_canonical(&g,0,0,0,c,1) &&
         nf18a5_query(&g,0.3f,0.2f,0.2f,1,true)==NF18A5_Q_FREE,
         "N01_unsafe_control_demonstrates_false_clearance");

    Nf18a5History h;
    nf18a5_history_init(&h,NF18A5_CUMULATIVE_DURABLE);
    gate(h.window_ticks==15u&&h.duration_ticks==60u&&h.next_sequence==1u,"H01_policy_initialization");
    commit_range(&h,1,60,101u,1.0f,0.0f);
    gate(h.raw_samples==60u,"H02_every_raw_sample_accounted");
    gate(h.summary_count==3u&&h.outbox_count==0u,"H03_periodic_windows_no_premature_event");
    gate(nf18a5_history_close(&h,61,61,42u,101u),"H04_contact_end_closes_window");
    gate(h.summary_count==4u&&h.outbox_count==1u,"H05_duration_promotes_after_summary");
    gate((h.outbox[0].reason_mask&NF18A5_REASON_DURATION)!=0u,"H06_duration_threshold_crosses_windows");
    gate(h.outbox[0].samples==60u&&h.outbox[0].last_tick==60u,"H07_event_provenance_range");
    gate(!nf18a5_history_commit(&h,61,61,42u,NULL,0u),"H08_duplicate_tick_rejected");
    gate(!nf18a5_history_close(&h,62,62,42u,999u),"H09_no_phantom_closure");
    gate(!nf18a5_ack(&h,2u),"H10_cannot_ack_unknown_event");
    gate(nf18a5_ack(&h,1u)&&h.outbox_count==0u,"H11_ack_contiguous_persisted_prefix");
    gate(!nf18a5_ack(&h,1u),"H12_duplicate_ack_rejected");

    nf18a5_history_init(&h,NF18A5_CUMULATIVE_DURABLE);
    commit_range(&h,1,10,111u,1.0f,0.0f);
    Nf18a5Sample gap=sample(20,111u,1.0f,0.0f);
    gate(nf18a5_history_commit(&h,20,20,42u,&gap,1u),"H13_gap_commit_accepted");
    gate(h.summary_count==1u&&h.streams[0].epoch_count==1u,"H14_gap_resets_duration");
    gate(nf18a5_history_close(&h,21,21,42u,111u)&&h.outbox_count==0,"H15_no_false_duration_through_gap");

    nf18a5_history_init(&h,NF18A5_CUMULATIVE_DURABLE);
    commit_range(&h,1,30,1001u,0.0f,120.0f);
    gate(nf18a5_history_close(&h,31,31,42u,1001u),"H16_resting_force_window_closes");
    gate(h.outbox_count==1u && (h.outbox[0].reason_mask&NF18A5_REASON_LOAD)!=0u,"H17_resting_load_promotes");
    gate(h.outbox[0].impulse_sum==0.0&&fabs(h.outbox[0].force_time-60.0)<0.001,"H18_resting_load_not_fake_impulse");
    nf18a5_history_init(&h,NF18A5_CUMULATIVE_DURABLE);
    Nf18a5Sample impact=sample(1,5u,25.0f,0.0f);
    gate(nf18a5_history_commit(&h,1,1,42u,&impact,1u),"H19_impact_raw_contact_committed");
    gate(h.outbox_count==0u,"H20_raw_impact_must_pass_summary_gate");
    gate(nf18a5_history_close(&h,2,2,42u,5u) && (h.outbox[0].reason_mask&NF18A5_REASON_PEAK),"H21_impact_threshold_after_summary");

    nf18a5_history_init(&h,NF18A5_CUMULATIVE_DURABLE);
    Nf18a5Sample bad=sample(1,55u,-1.0f,0.0f);
    uint32_t before=nf18a5_history_hash(&h);
    gate(!nf18a5_history_commit(&h,1,1,42u,&bad,1u) && nf18a5_history_hash(&h)==before,"H22_invalid_contact_atomic_rejection");
    Nf18a5Sample duplicate[2]={sample(1,5u,1.0f,0),sample(1,5u,1.0f,0)};
    gate(!nf18a5_history_commit(&h,1,1,42u,duplicate,2u),"H23_duplicate_contact_rejected");
    gate(!nf18a5_history_commit(&h,1,1,0u,&impact,1u),"H24_missing_owner_rejected");

    nf18a5_history_init(&h,NF18A5_CUMULATIVE_DURABLE);
    h.peak_threshold=0.1f;
    bool first32=true;
    for(unsigned t=1;t<=64u;++t){Nf18a5Sample s=sample(2u*t-1u,9u,1.0f,0);
        if(!nf18a5_history_commit(&h,2u*t-1u,2u*t-1u,42u,&s,1u))first32=false;
        if(!nf18a5_history_close(&h,2u*t,2u*t,42u,9u))first32=false;
        if(t==32u)break;
    }
    gate(first32&&h.outbox_count==NF18A5_OUTBOX,"H25_outbox_filled_without_losses");
    Nf18a5Sample overflow=sample(65,9u,1.0f,0);
    gate(nf18a5_history_commit(&h,65,65,42u,&overflow,1u),"H26_can_stage_contact_while_outbox_full");
    before=nf18a5_history_hash(&h);
    gate(!nf18a5_history_close(&h,66,66,42u,9u)&&nf18a5_history_hash(&h)==before,"H27_overflow_fail_closed_no_partial_history");
    gate(h.lost_records==0,"H28_no_silent_persistent_loss");
    gate(nf18a5_ack(&h,1u),"H29_ack_creates_outbox_capacity");
    gate(nf18a5_history_close(&h,66,66,42u,9u),"H30_resume_after_ack");

    Nf18a5World world;nf18a5_world_init(&world,1u);
    gate(world.revision==1u && world.grid.capacity==1u,"W01_authority_world_initialized");
    Nf18a5Sample stage=sample(1,77u,5.0f,0.0f);
    gate(nf18a5_world_commit(&world,1,1,42u,&stage,1u),"W02_atomic_local_world_history_commit");
    gate(world.revision==2u && world.history.raw_samples==1u,"W03_world_version_moves_with_sample");
    gate(!nf18a5_world_commit(&world,1,2,42u,&stage,1u),"W04_stale_world_snapshot_rejected");
    Nf18a5Sample stale=sample(2,77u,5.0f,0.0f);stale.material_epoch=2u;
    gate(!nf18a5_world_commit(&world,2,2,42u,&stale,1u)&&world.revision==2u,"W05_material_epoch_mismatch_rejected");
    stale.material_epoch=1u;
    gate(nf18a5_world_commit(&world,2,2,42u,&stale,1u),"W06_updated_world_frame_accepted");
    Nf18a5History order_a,order_b;
    nf18a5_history_init(&order_a,NF18A5_CUMULATIVE_DURABLE);
    nf18a5_history_init(&order_b,NF18A5_CUMULATIVE_DURABLE);
    bool ordered_ok=true;
    for(uint32_t t=1u;t<=30u;++t){
        Nf18a5Sample alpha=sample(t,12u,2.0f,0.0f);
        Nf18a5Sample beta=sample(t,99u,1.0f,0.0f);
        Nf18a5Sample left[2]={alpha,beta},right[2]={beta,alpha};
        if(!nf18a5_history_commit(&order_a,t,t,42u,left,2u) ||
           !nf18a5_history_commit(&order_b,t,t,42u,right,2u))ordered_ok=false;
    }
    gate(ordered_ok,"H31_two_contact_permutation_inputs_accepted");
    gate(nf18a5_history_hash(&order_a)==nf18a5_history_hash(&order_b),
         "H32_contact_set_order_invariance");
    gate(order_a.summary_count==order_b.summary_count &&
         order_a.outbox_count==order_b.outbox_count,
         "H33_sorted_contact_sequence_correspondence");
    Nf18a5History no_sample;nf18a5_history_init(&no_sample,NF18A5_CUMULATIVE_DURABLE);
    Nf18a5Sample live=sample(1u,333u,1.0f,0.0f);
    gate(nf18a5_history_commit(&no_sample,1,1,42,&live,1),"H34_active_support_sample");
    gate(nf18a5_history_commit(&no_sample,2,2,42,NULL,0),"H35_empty_tick_closes_omitted_contact");
    gate(no_sample.streams[0].active==0 && no_sample.summary_count==1,
         "H36_no_silent_persistent_resting_contact");
    Nf18a5World material_world;nf18a5_world_init(&material_world,1u);
    uint8_t material_patch[64]={0};material_patch[0]=NF18A5_SOLID;
    Nf18a5Sample patch_sample=sample(1,101u,4.0f,0.0f);
    patch_sample.material_epoch=2u;
    gate(nf18a5_world_material_commit(&material_world,1,1,42,0,0,0,
         material_patch,&patch_sample,1u),"W07_atomic_material_and_contact_commit");
    gate(material_world.revision==2u&&material_world.grid.global_revision==2u&&
         material_world.history.raw_samples==1u,"W08_material_history_version_correspondence");
    patch_sample.tick=2u;patch_sample.material_epoch=3u;patch_sample.normal_impulse=-1.0f;
    gate(!nf18a5_world_material_commit(&material_world,2,2,42,0,0,0,
         material_patch,&patch_sample,1u),"W09_material_edit_rejected_with_bad_contact");
    gate(material_world.revision==2u&&material_world.grid.global_revision==2u&&
         material_world.history.raw_samples==1u,"W10_failed_transaction_no_partial_material_edit");
    gate(nf18a5_query(&material_world.grid,0.2f,0.1f,0.1f,3,true)==NF18A5_Q_SOLID,
         "W11_postcommit_material_query");
    Nf18a5Grid epoch;nf18a5_grid_init(&epoch,1,NF18A5_HYSTERETIC_FINE);
    canonical(c);fine(f,4);
    gate(nf18a5_load_canonical(&epoch,0,0,0,c,1)&&epoch.global_revision==2u,
         "G29_material_epoch_advanced_by_canonical_data");
    gate(nf18a5_load_fine(&epoch,0,0,0,4,f,4096,2u,1)==false,
         "G30_fine_requires_correct_parent_epoch");
    gate(nf18a5_load_fine(&epoch,0,0,0,4,f,4096,1,2)==true &&
         epoch.global_revision==2u&&epoch.cache_revision>2u,
         "G31_fine_load_changes_cache_not_material_epoch");
    gate(nf18a5_release_fine(&epoch,0,0,0)&&epoch.global_revision==2u,
         "G32_fine_eviction_changes_cache_only");
    Nf18a4Receipt receipt={0};receipt.status=NF18A4_ACCEPTED;receipt.impulses_applied=1;
    receipt.tick=10;receipt.world_version=3;receipt.contact_id=312;receipt.normal_impulse=(NfVec3){3,4,0};
    Nf18a5Sample from;
    gate(nf18a5_receipt_sample(&receipt,42,7,0,1,&from),"B01_real_dynamic_receipt_bridge");
    gate(fabsf(from.normal_impulse-5.0f)<0.0001f,"B02_impulse_magnitude_from_applied_vector");
    receipt.impulses_applied=0;
    gate(!nf18a5_receipt_sample(&receipt,42,7,0,1,&from),"B03_estimate_only_receipt_rejected");
    printf("TOTAL,%u\nFAILED,%u\n",total,failures);
    return failures?1:0;
}
