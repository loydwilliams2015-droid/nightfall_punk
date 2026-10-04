#include "nf_contact18a2.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static int total=0,fail=0;
static void gate(bool x,const char *label) {
    ++total; printf("%s,%s\n",label,x?"PASS":"FAIL");if(!x)++fail;
}
static int closef(float a,float b,float eps){return fabsf(a-b)<=eps;}
static Nf18aCollider box(uint32_t id,float x0,float y0,float z0,float x1,float y1,float z1) {
    Nf18aCollider c={0};c.body_id=id;c.min=(NfVec3){x0,y0,z0};c.max=(NfVec3){x1,y1,z1};return c;
}
static Nf18aContact contact(uint32_t id,uint32_t owner,uint32_t tick,float impulse) {
    Nf18aContact c={0};c.contact_id=id;c.body_a=owner;c.body_b=id;c.tick=tick;
    c.normal_impulse=impulse;c.approach_speed=impulse*0.5f;return c;
}
int main(void) {
    const Nf18a2ShapePolicy p={0.3f,1.8f,0.24f,0.35f,0.707f};
    gate(nf18a2_profile_valid(p),"P01_capsule_policy_valid");
    gate(nf18a2_choose_profile(p,(Nf18a2ShapeRequest){0})==NF18A2_PROFILE_CAPSULE,
         "P02_capsule_default");
    gate(nf18a2_choose_profile(p,(Nf18a2ShapeRequest){true,false,true,true,true})==
             NF18A2_PROFILE_ROUNDED_BOX_SUPPORT,"P03_support_profile_only_when_valid");
    gate(nf18a2_choose_profile(p,(Nf18a2ShapeRequest){true,false,true,false,true})==
             NF18A2_PROFILE_CAPSULE,"P04_no_clearance_no_transition");
    gate(nf18a2_choose_profile(p,(Nf18a2ShapeRequest){false,true,true,true,true})==
             NF18A2_PROFILE_ROUNDED_BOX_LADDER,"P05_ladder_authorized");
    gate(nf18a2_choose_profile(p,(Nf18a2ShapeRequest){false,true,true,true,false})==
             NF18A2_PROFILE_CAPSULE,"P06_no_affordance_authority_no_ladder");
    const Nf18aCollider side=box(2u,1.0f,0.0f,-1.0f,2.0f,2.0f,1.0f);
    gate(closef(nf18a2_capsule_box_separation((NfVec3){0.0f,0.0f,0.0f},p,&side),0.7f,1e-5f),
         "P07_capsule_box_separation_metric");
    gate(nf18a2_capsule_box_separation((NfVec3){0.8f,0.0f,0.0f},p,&side)<0.0f,
         "P08_capsule_box_overlap_detected");
    const Nf18aCollider step=box(3u,-0.5f,-1.0f,-0.5f,0.5f,0.10f,0.5f);
    gate(closef(nf18a2_capsule_box_separation((NfVec3){0.0f,0.4f,0.0f},p,&step),0.3f,1e-5f),
         "P09_capsule_bottom_rounding_is_real_geometry");

    Nf18a2RigidBody motor_body={1u,{0.0f,0.0f,0.0f},80.0f};
    const Nf18a2Motor m={9.0f,0.0f,12.0f,960.0f,9.8f,0.0f};
    gate(nf18a2_motor_step(&motor_body,m,0.1f,true)&&
         closef(motor_body.velocity.x,1.2f,0.0001f),"P10_motor_force_limited");
    gate(nf18a2_motor_step(&motor_body,m,0.1f,false)&&
         closef(motor_body.velocity.y,-0.98f,0.0001f),"P11_motor_gravity_airborne");
    const Nf18a2Motor limited={10.0f,0.0f,100.0f,80.0f,9.8f,0.0f};
    Nf18a2RigidBody heavy={4u,{0.0f,0.0f,0.0f},80.0f};
    gate(nf18a2_motor_step(&heavy,limited,1.0f,true)&&
         closef(heavy.velocity.x,1.0f,0.0001f),"P12_motor_force_per_mass");
    Nf18a2RigidBody a={10u,{5.0f,0.0f,0.0f},80.0f};
    Nf18a2RigidBody b={11u,{0.0f,0.0f,0.0f},20.0f};
    const float before=a.mass*a.velocity.x+b.mass*b.velocity.x;
    float j=-1.0f;
    gate(nf18a2_exchange_normal_impulse(&a,&b,(NfVec3){-1.0f,0.0f,0.0f},0.0f,&j)&&j>0.0f,
         "P13_dynamic_two_body_impulse");
    gate(closef(a.mass*a.velocity.x+b.mass*b.velocity.x,before,0.0001f),
         "P14_linear_momentum_conserved");
    gate(closef(a.velocity.x,b.velocity.x,0.0001f),"P15_inelastic_relative_normal_resolved");
    const float old_velocity=a.velocity.x;
    gate(nf18a2_exchange_normal_impulse(&a,&b,(NfVec3){-1.0f,0.0f,0.0f},0.0f,&j)&&
         j==0.0f&&a.velocity.x==old_velocity,"P16_resting_contact_not_reimpulsed");
    gate(!nf18a2_exchange_normal_impulse(&a,&a,(NfVec3){-1.0f,0.0f,0.0f},0.0f,&j),
         "P17_self_contact_rejected");

    const Nf18aCollider wall=box(22u,1.0f,-10.0f,-2.0f,1.02f,10.0f,2.0f);
    Nf18a2AdaptiveResult solved=nf18a2_solve_adaptive(17u,1u,(NfVec3){0},
       (NfVec3){240.0f,0.0f,0.0f},(Nf18aShape){0.3f,1.8f,80.0f},1.0f/60.0f,
       &wall,1u,nf18a_default_config(),nf18a2_default_adaptive());
    gate(solved.solve.status==NF18A_BLOCKED&&solved.solve.feet.x<=0.701f,
         "P18_adaptive_preserves_swept_wall");
    gate(solved.attempts==1u&&solved.final_budget==2u&&solved.reserve_used==0u,
         "P19_cheap_default_pass");
    gate(nf18a2_default_adaptive().hard_max_iterations==6u,
         "P20_bounded_deterministic_reserve");
    const Nf18a2AdaptiveResult invalid=nf18a2_solve_adaptive(17u,1u,(NfVec3){0},
       (NfVec3){0},(Nf18aShape){0.3f,1.8f,80.0f},1.0f/60.0f,
       NULL,0u,nf18a_default_config(),(Nf18a2AdaptiveConfig){0});
    gate(invalid.solve.status==NF18A_INVALID_INPUT,"P21_invalid_budget_explicit");
    const Nf18a2AdaptiveResult reserved=nf18a2_solve_adaptive(
        18u,1u,(NfVec3){0},(NfVec3){240.0f,0.0f,60.0f},
        (Nf18aShape){0.3f,1.8f,80.0f},1.0f/60.0f,
        &wall,1u,nf18a_default_config(),(Nf18a2AdaptiveConfig){1u,1u,2u});
    gate(reserved.attempts==2u&&reserved.final_budget==2u&&
         reserved.reserve_used==1u&&reserved.solve.feet.x<=0.701f&&
         reserved.solve.status==NF18A_BLOCKED,
         "P21a_extra_budget_only_after_pending");

    Nf18a2ChunkCache cache;
    nf18a2_chunk_cache_init(&cache,2u);
    Nf18a2Chunk *c0=nf18a2_chunk_touch(&cache,0,0,0,NF18A2_CANONICAL_1M,1u);
    gate(c0&&c0->version==1u&&cache.count==1u,"P22_canonical_chunk_initial");
    uint8_t canonical[NF18A2_CANONICAL_CHUNK_CELLS]={0};canonical[0]=7u;
    uint8_t fine[NF18A2_FINE_CHUNK_CELLS]={0};fine[0]=9u;
    gate(nf18a2_chunk_load_canonical(&cache,0,0,0,canonical,2u),
         "P23_authoritative_canonical_data_loaded");
    gate(nf18a2_chunk_touch(&cache,0,0,0,NF18A2_REFINED_0_25M,3u)==NULL,
         "P24_fine_request_pending_without_data");
    gate(nf18a2_chunk_load_fine(&cache,0,0,0,NF18A2_REFINED_0_25M,fine,256u,4u),
         "P25_explicit_fine_chunk_load");
    c0=nf18a2_chunk_touch(&cache,0,0,0,NF18A2_REFINED_0_25M,8u);
    gate(c0&&c0->resolution==NF18A2_REFINED_0_25M&&c0->fine[0]==9u&&c0->canonical[0]==7u,
         "P26_refined_and_canonical_data_correspond");
    gate(nf18a2_chunk_touch(&cache,1,0,0,NF18A2_CANONICAL_1M,6u)!=NULL,
         "P24_second_chunk_loaded");
    gate(nf18a2_chunk_touch(&cache,2,0,0,NF18A2_CANONICAL_1M,7u)==NULL,
         "P25_chunk_capacity_explicit_pending");
    c0=nf18a2_chunk_touch(&cache,0,0,0,NF18A2_REFINED_0_25M,8u);
    if(c0)c0->pinned=1u;
    gate(nf18a2_chunk_evict_idle(&cache,100u,20u)&&cache.count==1u,
         "P26_deterministic_idle_evict_respects_pin");
    gate(nf18a2_chunk_coord(-0.10f,1.0f)==-1&&
         nf18a2_chunk_coord(1.99f,1.0f)==1,"P27_negative_spatial_chunk_coordinates");

    Nf18a2CondensedHistory history;
    nf18a2_condensed_history_init(&history,nf18a2_default_history_policy());
    gate(history.policy.summary_ticks==15u&&history.policy.duration_ticks==60u,
         "P28_250ms_summary_and_one_sec_duration_prior");
    Nf18aContact bad=contact(22u,1u,1u,3.0f);
    gate(!nf18a2_history_commit_samples(&history,1u,1u,2u,&bad,1u)&&
         history.summary_count==0u,"P29_foreign_authority_rejected");
    for(uint32_t tick=1u;tick<=18u;++tick) {
        Nf18aContact c=contact(22u,1u,tick,3.0f);
        if(!nf18a2_history_commit_samples(&history,tick,tick,1u,&c,1u))++fail;
    }
    gate(history.summary_count==1u&&history.event_count==0u,
         "P30_raw_samples_aggregate_into_period_summary");
    gate(history.summaries[0].samples==15u&&closef(history.summaries[0].impulse_sum,45.0f,0.0001f),
         "P31_no_contact_sample_loss");
    for(uint32_t tick=19u;tick<=32u;++tick) {
        Nf18aContact c=contact(22u,1u,tick,3.0f);
        if(!nf18a2_history_commit_samples(&history,tick,tick,1u,&c,1u))++fail;
    }
    gate(history.event_count==1u&&history.events[0].reason_mask==NF18A2_THRESHOLD_ACCUMULATED&&
         history.events[0].contributing_samples==30u,
         "P32_two_summaries_cumulative_threshold_event");
    Nf18aContact spike=contact(22u,1u,33u,30.0f);
    gate(nf18a2_history_commit_samples(&history,33u,33u,1u,&spike,1u),
         "P33_spike_still_entered_as_sample");
    gate(nf18a2_history_advance(&history,50u)&&history.event_count==2u&&
         (history.events[1].reason_mask&NF18A2_THRESHOLD_PEAK)!=0u,
         "P34_peak_event_promotion_via_summary");
    gate(!nf18a2_history_commit_samples(&history,33u,33u,1u,&spike,1u),
         "P35_duplicate_committed_version_rejected");
    gate(!nf18a2_history_end_contact(&history,22u,51u,33u,1u),
         "P36_reject_nonmonotone_end_version");
    gate(nf18a2_history_end_contact(&history,22u,51u,51u,1u),
         "P36_explicit_contact_end_releases_stream");

    Nf18a2CondensedHistory tiny;
    const Nf18a2HistoryPolicy policy={1u,100000u,100000.0f,100000.0f};
    nf18a2_condensed_history_init(&tiny,policy);
    for(uint32_t t=1u;t<=100u;++t) {
        Nf18aContact c=contact(10u,5u,t,1.0f);
        if(!nf18a2_history_commit_samples(&tiny,t,t,5u,&c,1u))++fail;
    }
    gate(tiny.summary_count==NF18A2_SUMMARY_RING&&tiny.summary_evictions>0u&&
         tiny.event_count==0u&&tiny.evicted_summary_digest!=0u,
         "P37_bounded_summary_ring_subthreshold_no_fake_event");
    Nf18a2CondensedHistory gapped;
    const Nf18a2HistoryPolicy gap_policy={15u,15u,200.0f,200.0f};
    nf18a2_condensed_history_init(&gapped,gap_policy);
    Nf18aContact gap_first=contact(55u,7u,1u,0.1f);
    Nf18aContact gap_second=contact(55u,7u,40u,0.1f);
    gate(nf18a2_history_commit_samples(&gapped,1u,1u,7u,&gap_first,1u)&&
         nf18a2_history_commit_samples(&gapped,40u,2u,7u,&gap_second,1u)&&
         gapped.event_count==0u,"P39_missing_ticks_are_not_continuous_support");
    uint8_t too_short_fine[10]={0};
    gate(!nf18a2_chunk_load_fine(&cache,0,0,0,NF18A2_REFINED_0_25M,
                                  too_short_fine,10u,200u),
         "P40_fake_fine_resolution_rejected");
    Nf18a2CondensedHistory overflow;
    nf18a2_condensed_history_init(&overflow,nf18a2_default_history_policy());
    Nf18aContact many[NF18A2_MAX_CONTACT_STREAMS+1u];
    for(uint32_t i=0;i<NF18A2_MAX_CONTACT_STREAMS+1u;++i)
        many[i]=contact(i+1u,9u,1u,1.0f);
    gate(!nf18a2_history_commit_samples(&overflow,1u,1u,9u,many,
         NF18A2_MAX_CONTACT_STREAMS+1u)&&overflow.initialized==0u,
         "P38_no_silent_contact_history_overflow");
    /* No direct mutation of world actor state occurs in any of these functions. */
    gate(total==45,"P46_test_inventory_45_named_cases");
    fprintf(stderr,"nightfall v1.8A.2 collision policy: %d/%d %s\n",
            total-fail,total,fail?"FAIL":"PASS");
    return fail?1:0;
}
