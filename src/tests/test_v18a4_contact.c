#include "nf_contact18a4.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static unsigned total=0u,failed=0u;
static void gate(bool yes,const char *name){++total;printf("%s,%s\n",name,yes?"PASS":"FAIL");if(!yes)++failed;}
static bool near(float a,float b,float eps){return fabsf(a-b)<=eps;}
static Nf18a4Body body(uint32_t id,float mass,NfVec3 position,NfVec3 v){
    Nf18a4Body b={0};b.id=id;b.center=position;b.velocity=v;b.inverse_mass=mass>0?1.0f/mass:0.0f;
    if(mass>0)b.inverse_inertia=(NfVec3){1.0f,1.0f,1.0f};
    b.radius=0.3f;b.height=1.8f;
    b.box_half=(NfVec3){.5f,.5f,.5f};return b;
}
static Nf18a4Constraint con(uint32_t a,uint32_t b,NfVec3 point,float friction){
    return (Nf18a4Constraint){a,b,5,{1,0,0},point,0.0f,friction};
}
static float mom_x(Nf18a4Body a,Nf18a4Body b){return a.velocity.x/a.inverse_mass+b.velocity.x/b.inverse_mass;}
int main(void){
    Nf18a4Body a=body(1,80,(NfVec3){0,0,0},(NfVec3){-3,0,0});
    Nf18a4Body b=body(2,40,(NfVec3){-1,0,0},(NfVec3){0,0,0});
    Nf18a4Constraint c=con(1,2,(NfVec3){-.5f,0,0},0);
    gate(nf18a4_body_valid(&a)&&nf18a4_body_valid(&b),"D01_valid_body");
    Nf18a4Body bad=a;bad.inverse_mass=-1;
    gate(!nf18a4_body_valid(&bad),"D02_negative_inverse_mass");
    bad=a;bad.inverse_mass=0;
    gate(!nf18a4_motor_drive(&bad,(Nf18a4Motor){{5,0,0},200,20},.1f,&(Nf18a4MotorReceipt){0}),"D03_static_motor_refused");
    a.velocity=(NfVec3){0};Nf18a4MotorReceipt motor={0};
    gate(nf18a4_motor_drive(&a,(Nf18a4Motor){{5,0,0},80,10},.1f,&motor),"D04_motor_applies");
    gate(near(a.velocity.x,.1f,1e-5f)&&near(motor.applied_force,80,1e-3f),"D05_bounded_motor_force");
    gate(near(motor.work_joules,.4f,1e-3f),"D06_motor_work_accounted");
    gate(near(a.center.x,0,1e-6f),"D07_motor_no_teleport");
    a.velocity=(NfVec3){-3,0,0};
    const float before=mom_x(a,b);
    Nf18a4Receipt r=nf18a4_apply_contact(&a,&b,c,NF18A4_RECIPROCAL_LINEAR,17,9);
    gate(r.status==NF18A4_ACCEPTED&&r.impulses_applied==1,"D08_pair_impulses_applied");
    gate(near(mom_x(a,b),before,0.0001f),"D09_linear_momentum_conserved");
    gate(b.velocity.x<-.5f&&a.velocity.x> -3,"D10_both_bodies_react");
    gate(r.signed_energy_delta<0.001f,"D11_inelastic_energy_nonincrease");
    gate(near(r.normal_impulse.x,80.0f,1e-3f),"D12_normal_impulse_magnitude");
    Nf18a4Body stop=a;
    r=nf18a4_apply_contact(&a,&b,c,NF18A4_RECIPROCAL_LINEAR,18,9);
    gate(r.status==NF18A4_SEPARATING&&near(a.velocity.x,stop.velocity.x,1e-6f),"D13_separating_no_second_impulse");
    a=body(1,80,(NfVec3){0,0,0},(NfVec3){-3,0,0});b=body(2,40,(NfVec3){-1,0,0},(NfVec3){0,0,0});
    r=nf18a4_apply_contact(&a,&b,c,NF18A4_ESTIMATE_ONLY_CONTROL,17,9);
    gate(r.status==NF18A4_ACCEPTED&&r.impulses_applied==0&&b.velocity.x==0,"N01_estimate_only_does_not_move_B");
    a=body(1,80,(NfVec3){0,0,0},(NfVec3){-3,0,0});b=body(2,40,(NfVec3){-1,0,0},(NfVec3){0,0,0});
    c.restitution=1.0f;
    r=nf18a4_apply_contact(&a,&b,c,NF18A4_RECIPROCAL_LINEAR,17,9);
    gate(near(r.signed_energy_delta,0,0.001f),"D14_elastic_energy_conservation");
    a=body(1,80,(NfVec3){0,0,0},(NfVec3){-3,0,0});b=body(2,0,(NfVec3){-1,0,0},(NfVec3){0,0,0});
    c.restitution=0.0f;r=nf18a4_apply_contact(&a,&b,c,NF18A4_ANGULAR_NORMAL,17,9);
    gate(r.status==NF18A4_ACCEPTED&&near(a.velocity.x,0,1e-6f)&&near(b.velocity.x,0,1e-6f),"D15_static_wall_reacts");
    a=body(1,80,(NfVec3){0,0,0},(NfVec3){-3,0,0});b=body(2,40,(NfVec3){-1,0,0},(NfVec3){0,0,0});
    c=con(1,2,(NfVec3){-.5f,1,0},0);
    const float pm=mom_x(a,b);
    r=nf18a4_apply_contact(&a,&b,c,NF18A4_ANGULAR_NORMAL,17,9);
    gate(fabsf(a.omega.z)>0.1f && fabsf(b.omega.z)>0.1f,"D16_offcenter_angular_impulses");
    gate(near(mom_x(a,b),pm,0.001f),"D17_offcenter_linear_momentum");
    gate(r.signed_energy_delta<=.003f,"D18_offcenter_energy_nonincrease");
    a=body(1,80,(NfVec3){0,0,0},(NfVec3){-3,0,0});b=body(2,40,(NfVec3){-1,0,0},(NfVec3){0,0,0});
    r=nf18a4_apply_contact(&a,&b,c,NF18A4_RECIPROCAL_LINEAR,17,9);
    gate(near(a.omega.z,0,1e-8f)&&near(b.omega.z,0,1e-8f),"D19_linear_model_no_angular_response");
    a=body(1,80,(NfVec3){0,0,0},(NfVec3){-3,2,0});b=body(2,40,(NfVec3){-1,0,0},(NfVec3){0,0,0});
    c=con(1,2,(NfVec3){-.5f,0,0},0.75f);
    r=nf18a4_apply_contact(&a,&b,c,NF18A4_FRICTION_FIXED6,17,9);
    gate(fabsf(r.tangent_impulse.y)>0.0001f,"D20_friction_impulse_real");
    gate(fabsf(a.velocity.y-b.velocity.y)<2.0f,"D21_friction_reduces_relative_tangent");
    gate(fabsf(r.tangent_impulse.y)<=0.75f*fabsf(r.normal_impulse.x)+0.0002f,"D22_coulomb_friction_clamp");
    gate(r.signed_energy_delta<0.002f,"D23_contact_energy_nonincrease");
    a=body(1,80,(NfVec3){0,0,0},(NfVec3){-3,2,0});b=body(2,40,(NfVec3){-1,0,0},(NfVec3){0,0,0});
    r=nf18a4_apply_contact(&a,&b,c,NF18A4_ANGULAR_NORMAL,17,9);
    gate(near(r.tangent_impulse.y,0,0.0001f),"D24_angular_normal_no_friction");
    a=body(1,80,(NfVec3){0,0,0},(NfVec3){-3,0,0});b=body(2,40,(NfVec3){-1,0,0},(NfVec3){0,0,0});
    c=con(1,2,(NfVec3){-.5f,0,0},0);
    c.restitution=1.4f;
    const Nf18a4Body original=a;
    r=nf18a4_apply_contact(&a,&b,c,NF18A4_FRICTION_ADAPTIVE,17,9);
    gate(r.status==NF18A4_INVALID && near(a.velocity.x,original.velocity.x,1e-8f),"D25_invalid_restitution_no_mutation");
    c.restitution=0;c.body_a=2;
    r=nf18a4_apply_contact(&a,&b,c,NF18A4_FRICTION_ADAPTIVE,17,9);
    gate(r.status==NF18A4_INVALID,"D26_wrong_actor_identity_rejected");
    c.body_a=1;
    Nf18a4Body bs[2]={a,b};
    Nf18a4IslandResult ir=nf18a4_solve_island(bs,2,&c,1,NF18A4_FRICTION_ADAPTIVE,17,9);
    gate(ir.status==NF18A4_ACCEPTED&&ir.iterations<=2&&ir.contact_count==1,"D27_adaptive_cheap_single_contact");
    gate(bs[1].velocity.x<0,"D28_island_real_crate_velocity");
    Nf18a4Body pair1[2]={a,b},pair2[2]={a,b};
    Nf18a4IslandResult ir2=nf18a4_solve_island(pair1,2,&c,1,NF18A4_FRICTION_FIXED6,17,9);
    ir=nf18a4_solve_island(pair2,2,&c,1,NF18A4_FRICTION_ADAPTIVE,17,9);
    gate(ir2.status==ir.status&&near(pair1[1].velocity.x,pair2[1].velocity.x,1e-6f),"D29_fixed_vs_adaptive_same_single_contact");
    Nf18a4Constraint duplicates[2]={c,c};
    bs[0]=a;bs[1]=b;ir=nf18a4_solve_island(bs,2,duplicates,2,NF18A4_FRICTION_ADAPTIVE,17,9);
    gate(ir.status==NF18A4_INVALID && near(bs[1].velocity.x,0,1e-6f),"D30_duplicate_contact_atomic_refusal");
    const Nf18a2ShapePolicy sh={0.3f,1.8f,.1f,.4f,.7f};
    a=body(1,80,(NfVec3){0,.9f,0},(NfVec3){0,0,0});
    b=body(2,40,(NfVec3){1.2f,.9f,0},(NfVec3){0,0,0});b.box_half=(NfVec3){.5f,.9f,.8f};
    const Nf18a4Motor drive={{240,0,0},200000,10000};
    Nf18a4PairResult pr=nf18a4_pair_step(a,b,sh,drive,NF18A4_FRICTION_ADAPTIVE,1.0f/60.0f,17,9,0,0);
    gate(pr.swept_hit&&pr.status==NF18A4_ACCEPTED,"D31_capsule_dynamic_pair_contact");
    gate(pr.receipt.impulses_applied&&pr.object.velocity.x>0,"D32_pair_step_crate_velocity");
    gate(pr.actor.center.x<1.2f,"D33_pair_step_no_tunneling");
    const uint32_t h=nf18a4_pair_hash(&pr);
    const Nf18a4PairResult repeat=nf18a4_pair_step(a,b,sh,drive,NF18A4_FRICTION_ADAPTIVE,1.0f/60.0f,17,9,0,0);
    gate(h!=0 && h==nf18a4_pair_hash(&repeat),"D34_identical_pair_hash");
    const Nf18a4PairResult estimated=nf18a4_pair_step(a,b,sh,drive,NF18A4_ESTIMATE_ONLY_CONTROL,1.0f/60.0f,17,9,0,0);
    gate(estimated.swept_hit && near(estimated.object.velocity.x,0,1e-6f),"N02_kinematic_estimate_still_no_crate_response");
    const Nf18aCollider step={.body_id=41,.min={0,0,-2},.max={2,.3f,2}};
    const Nf18aCollider floor={.body_id=60,.min={-3,-1,-3},.max={0,0,3}};
    const Nf18aCollider solids[2]={floor,step};
    const Nf18a3Geometry g={solids,2,NULL,0,9,70};
    Nf18a3Contract ac={0};ac.actor_id=5;ac.feature_id=41;ac.world_version=9;ac.tick=70;
    ac.may_step=1;ac.material_support_valid=1;ac.support_stable=1;ac.current_support_id=60;
    ac.authoritative_step_limit=.4f;ac.authoritative_reach_limit=.8f;
    ac.authoritative_min_facing_cosine=.5f;ac.authoritative_min_landing_width=.07f;
    ac.authoritative_body_radius=.3f;ac.authoritative_body_height=1.8f;
    ac.authoritative_foot_flat_radius=.1f;ac.authoritative_feet=(NfVec3){-.31f,0,0};ac.actor_grounded=1;
    const Nf18a3Request rq={.transition=NF18A3_STEP,.policy=NF18A3_HYBRID_DUAL,
       .actor_id=5,.feature_id=41,.feet={-.31f,0,0},.destination={.8f,.3f,0},
       .facing={1,0,0},.body_radius=.3f,.body_height=1.8f,.foot_flat_radius=.1f,
       .max_step=.4f,.max_ladder_reach=.8f,.min_facing_cosine=.5f,.min_landing_width=.07f,
       .grounded=1};
    const Nf18a3Decision authorized=nf18a3_adjudicate(g,ac,rq);
    a=body(5,80,(NfVec3){-.31f,.9f,0},(NfVec3){0,0,0});
    gate(authorized.commit_eligible,"D35_traversal_dual_eligible");
    gate(nf18a4_stage_traversal(&a,g,ac,rq,(Nf18a4Motor){{2,2,0},400,20},.016f,70,9,&motor)==NF18A4_ACCEPTED,"D36_stage_authorized_motor");
    gate(near(a.center.y,.9f,1e-6f)&&a.velocity.y>0,"D37_stage_changes_velocity_not_position");
    const Nf18a4Body accepted=a;
    gate(nf18a4_stage_traversal(&a,g,ac,rq,(Nf18a4Motor){{2,2,0},400,20},.016f,71,9,&motor)==NF18A4_GATED&&
         near(a.velocity.x,accepted.velocity.x,1e-8f),"D38_stale_tick_staging_rejected");
    Nf18a3Contract denied=ac;denied.may_step=0;
    gate(nf18a4_stage_traversal(&a,g,denied,rq,(Nf18a4Motor){{2,2,0},400,20},.016f,70,9,&motor)==NF18A4_GATED,"D39_missing_contract_rejected");
    Nf18aContact evidence={0};
    gate(!nf18a4_pair_to_sample(&estimated,&evidence),"D40_estimated_impulse_cannot_be_history");
    gate(nf18a4_pair_to_sample(&pr,&evidence)&&
         evidence.normal_impulse>0.0f&&evidence.body_a==1&&evidence.body_b==2,
         "D41_real_impulse_history_adapter");
    Nf18a2CondensedHistory history;
    nf18a2_condensed_history_init(&history,nf18a2_default_history_policy());
    gate(nf18a2_history_commit_samples(&history,17,9,1,&evidence,1),"D42_authoritative_summary_bridge");
    gate(!nf18a2_history_commit_samples(&history,17,9,1,&evidence,1),"D43_history_duplicate_version_reject");
    Nf18a4Body chain[3]={body(1,40,(NfVec3){0,0,0},(NfVec3){-8,0,0}),
        body(2,40,(NfVec3){-1,0,0},(NfVec3){0,0,0}),
        body(3,40,(NfVec3){-2,0,0},(NfVec3){0,0,0})};
    Nf18a4Constraint nc[2]={con(1,2,(NfVec3){-.5f,0,0},0),
                            con(2,3,(NfVec3){-1.5f,0,0},0)};
    ir=nf18a4_solve_island(chain,3,nc,2,NF18A4_FRICTION_ADAPTIVE,22,9);
    gate(ir.status==NF18A4_ACCEPTED&&ir.reserve_used==1u&&ir.iterations>2,
         "D44_adaptive_reserve_dense_chain");
    gate(chain[2].velocity.x<-.1f&&near(chain[0].velocity.x+
         chain[1].velocity.x+chain[2].velocity.x,-8,.0003f),"D45_chain_reciprocal_impulse");
    Nf18a4Body reordered[3]={body(1,40,(NfVec3){0,0,0},(NfVec3){-8,0,0}),
        body(2,40,(NfVec3){-1,0,0},(NfVec3){0,0,0}),
        body(3,40,(NfVec3){-2,0,0},(NfVec3){0,0,0})};
    Nf18a4Constraint reversed[2]={nc[1],nc[0]};
    ir2=nf18a4_solve_island(reordered,3,reversed,2,NF18A4_FRICTION_ADAPTIVE,22,9);
    gate(ir2.status==NF18A4_ACCEPTED&&near(reordered[0].velocity.x,chain[0].velocity.x,1e-6f),
         "D46_contact_order_invariance");
    Nf18a4Body overcrowded[8];Nf18a4Constraint many[7];
    for(unsigned k=0;k<8;++k){
        overcrowded[k]=body(k+1,40,(NfVec3){-(float)k,0,0},
                            (NfVec3){k==0?-100.0f:0,0,0});
        if(k<7){many[k]=con(k+1,k+2,(NfVec3){-(float)k-.5f,0,0},0);
            many[k].feature_id=k+1;}
    }
    ir=nf18a4_solve_island(overcrowded,8,many,7,NF18A4_FRICTION_ADAPTIVE,22,9);
    gate(ir.status==NF18A4_PENDING_CONTACT,"D47_explicit_pending_deep_chain");
    gate(near(overcrowded[0].velocity.x,-100,1e-6f)&&
         near(overcrowded[7].velocity.x,0,1e-6f),"D48_pending_island_no_partial_commit");
    Nf18a4Body single=body(1,80,(NfVec3){0,0,0},(NfVec3){2,0,0});
    gate(nf18a4_motor_drive(&single,(Nf18a4Motor){{0,0,0},1000,1000},.1f,&motor)
         &&motor.work_joules<0,"D49_motor_braking_work_signed");
    Nf18a4Body free_actor=body(1,80,(NfVec3){0,.9f,0},(NfVec3){0,0,0});
    Nf18a4Body distant=body(2,40,(NfVec3){25,.9f,0},(NfVec3){0,0,0});
    distant.box_half=(NfVec3){.5f,.9f,.8f};
    const Nf18a4PairResult no_hit=nf18a4_pair_step(free_actor,distant,sh,drive,
                            NF18A4_FRICTION_ADAPTIVE,1.0f/60.0f,25,9,0,0);
    gate(no_hit.status==NF18A4_ACCEPTED&&!no_hit.swept_hit&&
         no_hit.actor.center.x>0&&near(no_hit.object.velocity.x,0,1e-6f),
         "D50_no_contact_no_crate_impulse");
    gate(pr.motor_receipt.applied_force>0&&pr.motor_receipt.work_joules>0,
         "D51_pair_motor_work_receipt");
    a=body(5,80,(NfVec3){-.31f,.9f,0},(NfVec3){0,0,0});
    gate(nf18a4_stage_traversal(&a,g,ac,rq,(Nf18a4Motor){{2,2,0},400,20},.016f,70,9,&motor)==NF18A4_ACCEPTED,
         "D52_first_stage_unique_tick");
    const NfVec3 first_stage_velocity=a.velocity;
    gate(nf18a4_stage_traversal(&a,g,ac,rq,(Nf18a4Motor){{2,2,0},400,20},.016f,70,9,&motor)==NF18A4_GATED &&
         near(a.velocity.x,first_stage_velocity.x,1e-8f),"D53_double_stage_same_tick_denied");
    a=body(1,80,(NfVec3){0,0,0},(NfVec3){-3,0,0});
    b=body(2,40,(NfVec3){-1,0,0},(NfVec3){0,0,0});
    c=con(1,2,(NfVec3){-.5f,1,0},0);
    const float am0=(a.omega.z/a.inverse_inertia.z)+(b.omega.z/b.inverse_inertia.z)
      +a.center.x*a.velocity.y/a.inverse_mass-a.center.y*a.velocity.x/a.inverse_mass
      +b.center.x*b.velocity.y/b.inverse_mass-b.center.y*b.velocity.x/b.inverse_mass;
    r=nf18a4_apply_contact(&a,&b,c,NF18A4_ANGULAR_NORMAL,26,9);
    const float am1=(a.omega.z/a.inverse_inertia.z)+(b.omega.z/b.inverse_inertia.z)
      +a.center.x*a.velocity.y/a.inverse_mass-a.center.y*a.velocity.x/a.inverse_mass
      +b.center.x*b.velocity.y/b.inverse_mass-b.center.y*b.velocity.x/b.inverse_mass;
    gate(r.status==NF18A4_ACCEPTED&&near(am0,am1,.0005f),"D54_pair_angular_momentum_about_origin");
    puts(failed?"v1.8A.4 dynamic interaction: FAIL":"v1.8A.4 dynamic interaction: PASS");
    fprintf(stderr,"TOTAL=%u FAILED=%u\n",total,failed);
    return failed?1:0;
}
