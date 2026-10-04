#include "nf_contact18a3.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static unsigned total=0,fail=0;
static void gate(bool ok,const char *name){++total;printf("%s,%s\n",name,ok?"PASS":"FAIL");if(!ok)++fail;}
static Nf18aCollider box(uint32_t id,float x0,float y0,float z0,float x1,float y1,float z1){
    Nf18aCollider b={0};b.body_id=id;b.min=(NfVec3){x0,y0,z0};b.max=(NfVec3){x1,y1,z1};return b;
}
static Nf18a2ShapePolicy shape(void){return (Nf18a2ShapePolicy){0.3f,1.8f,0.10f,0.4f,0.7f};}
static Nf18a3Contract contract(uint32_t id){Nf18a3Contract c={0};c.actor_id=5;c.feature_id=id;c.world_version=9;c.tick=70;c.may_step=1;c.may_attach_ladder=1;c.may_exit_ladder=1;c.material_support_valid=1;c.support_stable=1;c.authoritative_step_limit=.4f;c.authoritative_reach_limit=.8f;c.authoritative_min_facing_cosine=.5f;c.authoritative_min_landing_width=.07f;
 c.authoritative_body_radius=.3f;
 c.authoritative_body_height=1.8f;c.authoritative_foot_flat_radius=.1f;
 c.authoritative_feet=(NfVec3){-.31f,0,0};c.actor_grounded=1;c.current_support_id=60;return c;}
static Nf18a3Request req(uint32_t id){Nf18a3Request r={0};r.transition=NF18A3_STEP;r.policy=NF18A3_HYBRID_DUAL;r.actor_id=5;r.feature_id=id;r.feet=(NfVec3){-.31f,0,0};r.destination=(NfVec3){.8f,.3f,0};r.facing=(NfVec3){1,0,0};r.body_radius=.3f;r.body_height=1.8f;r.foot_flat_radius=.1f;r.max_step=.4f;r.max_ladder_reach=.8f;r.min_facing_cosine=.5f;r.min_landing_width=.07f;r.grounded=1;return r;}
static Nf18a3Geometry geom(const Nf18aCollider *solids,size_t n,const Nf18aCollider *lad,size_t m){Nf18a3Geometry w={0};w.solids=solids;w.solid_count=n;w.ladders=lad;w.ladder_count=m;w.world_version=9;w.tick=70;return w;}
static bool near(float x,float y,float eps){return fabsf(x-y)<=eps;}
int main(void){
    const Nf18aCollider wall=box(9,1,-4,-3,1.01f,4,3);
    Nf18aContact hit={0};
    gate(nf18a3_capsule_sweep((NfVec3){0,0,0},shape(),(NfVec3){4,0,0},wall,1,&hit)&&
         hit.kind==NF18A_CONTACT_TOUCH&&near(hit.toi,.175f,.0005f),"S01_thin_wall_TOI");
    gate(hit.normal.x<-.99f&&near(hit.point.x,1,.001f)&&hit.approach_speed>3.9f,"S02_normal_point_approach");
    gate(!nf18a3_capsule_sweep((NfVec3){0,0,0},shape(),(NfVec3){-4,0,0},wall,1,&hit),"S03_move_away");
    gate(!nf18a3_capsule_sweep((NfVec3){.7f,0,0},shape(),(NfVec3){0,0,2},wall,1,&hit),"S04_tangent_not_wall");
    gate(nf18a3_capsule_sweep((NfVec3){1.05f,0,0},shape(),(NfVec3){0,0,1},wall,1,&hit)&&
         hit.kind==NF18A_CONTACT_INITIAL_OVERLAP,"S05_initial_overlap");
    const Nf18aCollider shelf=box(10,-2,2.0f,-2,2,2.2f,2);
    gate(nf18a3_capsule_sweep((NfVec3){0,0,0},shape(),(NfVec3){0,1,0},shelf,1,&hit)&&
         hit.normal.y<-.99f&&near(hit.toi,.2f,.0005f),"S06_head_ceiling_TOI");
    Nf18aCollider moving=wall;moving.velocity=(NfVec3){1,0,0};
    gate(nf18a3_capsule_sweep((NfVec3){0,0,0},shape(),(NfVec3){4,0,0},moving,1,&hit)&&
         near(hit.toi,.7f/3.0f,.0005f),"S07_relative_moving_box");
    Nf18aCollider corner=box(22,1.0f,-2,1.0f,3,3,3);
    const bool corner_touch=nf18a3_capsule_sweep((NfVec3){0,0,0},shape(),(NfVec3){2,0,2},corner,1,&hit);
    gate(corner_touch&&near(hit.toi,(1-.3f/sqrtf(2.0f))/2,.001f)&&
         hit.normal.x<-.6f&&hit.normal.z<-.6f,"S08_corner_diagonal_normal");
    const Nf18aCollider step=box(41,0,0,-2,2,.3f,2);
    const Nf18aCollider floor=box(60,-3,-1,-3,0,0,3);
    Nf18aCollider step_world[2]={floor,step};
    Nf18a3Geometry w=geom(step_world,2,NULL,0);
    Nf18a3Contract c=contract(41);
    Nf18a3Request r=req(41);
    Nf18a3Decision d=nf18a3_adjudicate(w,c,r);
    gate(d.geometry_approved&&d.contract_approved&&d.commit_eligible,"J01_valid_step_dual");
    gate(d.used_flat_support&&d.shape_queries>0,"J02_flat_support_requires_sweep");
    const uint32_t hash=d.witness_hash;
    gate(nf18a3_adjudicate(w,c,r).witness_hash==hash,"J03_repeat_hash");
    r.policy=NF18A3_BOX_BASELINE;
    gate(nf18a3_adjudicate(w,c,r).commit_eligible,"J04_box_baseline_valid_step");
    r.policy=NF18A3_CAPSULE_STRICT;
    gate(nf18a3_adjudicate(w,c,r).commit_eligible,"J05_capsule_strict_valid_step");
    r.policy=NF18A3_HYBRID_DUAL;
    r.max_step=0.10f;
    gate(!nf18a3_adjudicate(w,c,r).commit_eligible,"J06_step_height_limit");
    r=req(41);r.max_step=.9f;
    d=nf18a3_adjudicate(w,c,r);
    gate(!d.commit_eligible && d.contract_reason==NF18A3_NO_AUTHORITY,"J07_client_cannot_increase_step_limit");
    r=req(41);r.grounded=0;c.actor_grounded=0;
    gate(!nf18a3_adjudicate(w,c,r).commit_eligible,"J08_airborne_no_step");
    r=req(41);r.destination.x=.05f;
    gate(!nf18a3_adjudicate(w,c,r).commit_eligible,"J09_no_narrow_ledge_support");
    Nf18aCollider blocks[3]={floor,step,box(51,-1,1.9f,-2,2,2.2f,2)};
    w=geom(blocks,3,NULL,0);r=req(41);
    d=nf18a3_adjudicate(w,c,r);
    gate(!d.commit_eligible&&d.geometry_reason==NF18A3_NO_CLEARANCE,"J10_low_ceiling");
    w=geom(step_world,2,NULL,0);
    c=contract(41);c.may_step=0;
    d=nf18a3_adjudicate(w,c,r);
    gate(d.geometry_approved&&!d.contract_approved&&!d.commit_eligible,"J11_geometry_yes_contract_no");
    c=contract(41);r=req(41);r.max_step=.1f;
    d=nf18a3_adjudicate(w,c,r);
    gate(!d.geometry_approved&&d.contract_approved&&!d.commit_eligible,"J12_geometry_no_contract_yes");
    r=req(41);c=contract(41);c.world_version=8;
    d=nf18a3_adjudicate(w,c,r);
    gate(d.geometry_approved&&!d.contract_approved&&!d.commit_eligible,"J13_stale_world_reject");
    c=contract(41);c.actor_id=10;
    gate(!nf18a3_adjudicate(w,c,r).commit_eligible,"J14_actor_jurisdiction");
    c=contract(41);c.feature_id=11;
    gate(!nf18a3_adjudicate(w,c,r).commit_eligible,"J15_feature_jurisdiction");
    c=contract(41);c.material_support_valid=0;
    gate(!nf18a3_adjudicate(w,c,r).commit_eligible,"J16_material_support_jurisdiction");
    c=contract(41);c.support_stable=0;
    gate(!nf18a3_adjudicate(w,c,r).commit_eligible,"J17_support_stability_jurisdiction");
    Nf18aCollider dynamic=step;dynamic.dynamic_body=1;
    Nf18aCollider moving_step[2]={floor,dynamic};w=geom(moving_step,2,NULL,0);c=contract(41);
    gate(!nf18a3_adjudicate(w,c,r).commit_eligible,"J18_dynamic_support_requires_new_solver");
    w=geom(step_world,2,NULL,0);c=contract(41);c.may_step=0;r.policy=NF18A3_HYBRID_AUTH_BYPASS_NEGATIVE;
    gate(nf18a3_adjudicate(w,c,r).commit_eligible,"N01_auth_bypass_negative_control");
    c=contract(41);r=req(41);r.max_step=.1f;r.policy=NF18A3_CAPSULE_EASY_NEGATIVE;
    gate(nf18a3_adjudicate(w,c,r).commit_eligible,"N02_easy_climb_negative_control");
    const Nf18aCollider ground=box(61,-3,-1,-3,3,0,3);
    const Nf18aCollider ladder=box(70,0,0,-.05f,.15f,2,.05f);
    w=geom(&ground,1,&ladder,1);c=contract(70);r=req(70);
    r.transition=NF18A3_LADDER_ATTACH;r.feet=(NfVec3){-.65f,0,0};
    r.destination=(NfVec3){-.35f,0,0};r.grounded=1;
    c.authoritative_feet=r.feet;
    d=nf18a3_adjudicate(w,c,r);
    gate(d.geometry_approved&&d.contract_approved&&d.commit_eligible,"L01_ladder_attach_valid");
    r.facing=(NfVec3){-1,0,0};
    gate(!nf18a3_adjudicate(w,c,r).commit_eligible,"L02_ladder_wrong_facing");
    r=req(70);r.transition=NF18A3_LADDER_ATTACH;r.feet=(NfVec3){-2,0,0};r.destination=(NfVec3){-.35f,0,0};c.authoritative_feet=r.feet;
    gate(!nf18a3_adjudicate(w,c,r).commit_eligible,"L03_ladder_out_of_reach");
    r=req(70);r.transition=NF18A3_LADDER_ATTACH;r.feet=(NfVec3){-.65f,0,0};r.destination=(NfVec3){-.35f,0,0};r.max_ladder_reach=100.0f;c.authoritative_feet=r.feet;
    d=nf18a3_adjudicate(w,c,r);
    gate(!d.commit_eligible&&d.contract_reason==NF18A3_NO_AUTHORITY,"L04_client_cannot_increase_reach");
    r=req(70);r.transition=NF18A3_LADDER_ATTACH;r.feet=(NfVec3){-.65f,0,0};r.destination=(NfVec3){-.35f,0,0};
    c=contract(70);c.authoritative_feet=r.feet;c.may_attach_ladder=0;
    d=nf18a3_adjudicate(w,c,r);
    gate(d.geometry_approved&&!d.contract_approved&&!d.commit_eligible,"L05_ladder_contract_denial");
    Nf18aCollider blocker[2]={ground,box(44,-.5f,0,-.5f,-.32f,2,.5f)};
    w=geom(blocker,2,&ladder,1);c=contract(70);c.authoritative_feet=r.feet;
    gate(!nf18a3_adjudicate(w,c,r).commit_eligible,"L06_ladder_wall_blocked");
    w=geom(&ground,1,&ladder,1);c=contract(70);
    r=req(70);r.transition=NF18A3_LADDER_EXIT;r.ladder_attached=1;
    r.feet=(NfVec3){-.35f,1.0f,0};r.destination=(NfVec3){.9f,1.0f,0};c.authoritative_feet=r.feet;c.actor_ladder_attached=1;
    gate(!nf18a3_adjudicate(w,c,r).commit_eligible,"L07_exit_without_landing");
    Nf18aCollider landing[2]={ground,box(72,0,0,-2,2,1.0f,2)};
    w=geom(landing,2,&ladder,1);
    d=nf18a3_adjudicate(w,c,r);
    gate(d.commit_eligible&&d.used_flat_support,"L08_exit_valid_support");
    r.ladder_attached=0;c.actor_ladder_attached=0;
    gate(!nf18a3_adjudicate(w,c,r).commit_eligible,"L09_exit_not_attached");
    r.ladder_attached=1;c.actor_ladder_attached=1;w.world_version=10;
    gate(!nf18a3_adjudicate(w,c,r).commit_eligible,"L10_exit_stale_version");
    puts(fail?"v1.8A.3 shape/traversal: FAIL":"v1.8A.3 shape/traversal: PASS");
    fprintf(stderr,"TOTAL=%u FAILED=%u\n",total,fail);
    return fail?1:0;
}
