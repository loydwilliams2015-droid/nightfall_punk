#include "nf_contact18a3.h"
#include <stdio.h>

static Nf18aCollider box(uint32_t id,float x0,float y0,float z0,float x1,float y1,float z1){
 Nf18aCollider b={0};b.body_id=id;b.min=(NfVec3){x0,y0,z0};b.max=(NfVec3){x1,y1,z1};return b;
}
int main(void){
 Nf18aCollider boxes[2]={box(60,-3,-1,-3,0,0,3),box(41,0,0,-2,2,.3f,2)};
 Nf18a3Geometry world={boxes,2,NULL,0,9,70};
 Nf18a3Contract c={0};c.actor_id=5;c.feature_id=41;c.world_version=9;c.tick=70;
 c.may_step=1;c.material_support_valid=1;c.support_stable=1;
 c.authoritative_step_limit=.4f;c.authoritative_reach_limit=.8f;
 c.authoritative_min_facing_cosine=.5f;c.authoritative_min_landing_width=.07f;
 c.authoritative_body_radius=.3f;c.authoritative_body_height=1.8f;
 c.authoritative_foot_flat_radius=.1f;c.authoritative_feet=(NfVec3){-.31f,0,0};
 c.current_support_id=60;c.actor_grounded=1;
 Nf18a3Request r={0};r.actor_id=5;r.feature_id=41;r.transition=NF18A3_STEP;
 r.feet=(NfVec3){-.31f,0,0};r.destination=(NfVec3){.8f,.3f,0};r.facing=(NfVec3){1,0,0};
 r.body_radius=.3f;r.body_height=1.8f;r.foot_flat_radius=.1f;r.max_step=.4f;
 r.max_ladder_reach=.8f;r.min_facing_cosine=.5f;r.min_landing_width=.07f;r.grounded=1;
 r.policy=NF18A3_HYBRID_DUAL;
 Nf18a3Decision result=nf18a3_adjudicate(world,c,r);
 if(!result.commit_eligible){puts("PROD_FAIL: validated hybrid denied");return 1;}
 r.policy=NF18A3_HYBRID_AUTH_BYPASS_NEGATIVE;c.may_step=0;
 result=nf18a3_adjudicate(world,c,r);
 if(result.commit_eligible || result.geometry_reason!=NF18A3_INVALID){puts("PROD_FAIL: unsafe contract bypass allowed");return 1;}
 r.policy=NF18A3_CAPSULE_EASY_NEGATIVE;c.may_step=1;
 result=nf18a3_adjudicate(world,c,r);
 if(result.commit_eligible || result.geometry_reason!=NF18A3_INVALID){puts("PROD_FAIL: unsafe height bypass allowed");return 1;}
 r.policy=(Nf18a3Policy)99;
 if(nf18a3_adjudicate(world,c,r).commit_eligible){puts("PROD_FAIL: unknown policy allowed");return 1;}
 puts("PRODUCTION_POLICY_GATE=PASS normal hybrid eligible, unsafe controls compile-gated, unknown policy refused");
 return 0;
}
