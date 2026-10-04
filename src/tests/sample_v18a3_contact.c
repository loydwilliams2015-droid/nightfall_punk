#include "nf_contact18a3.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static uint32_t rng(uint32_t *x){*x^=*x<<13;*x^=*x>>17;*x^=*x<<5;return *x;}
static float uniform(uint32_t *x,float a,float b){return a+(b-a)*(float)(rng(x)&0xffffffu)/16777215.0f;}
static Nf18aCollider box(uint32_t id,float x0,float y0,float z0,float x1,float y1,float z1){Nf18aCollider b={0};b.body_id=id;b.min=(NfVec3){x0,y0,z0};b.max=(NfVec3){x1,y1,z1};return b;}
static Nf18a3Contract good_contract(uint32_t feature,NfVec3 feet){
 Nf18a3Contract c={0};c.actor_id=5;c.feature_id=feature;c.world_version=9;c.tick=70;
 c.may_step=1;c.may_attach_ladder=1;c.may_exit_ladder=1;
 c.material_support_valid=1;c.support_stable=1;c.authoritative_step_limit=.4f;
 c.authoritative_reach_limit=.8f;c.authoritative_min_facing_cosine=.5f;c.authoritative_min_landing_width=.07f;
 c.authoritative_body_radius=.3f;c.authoritative_body_height=1.8f;
 c.authoritative_foot_flat_radius=.1f;c.authoritative_feet=feet;c.actor_grounded=1;
 c.current_support_id=60;return c;
}
static Nf18a3Request good_request(uint32_t feature){
 Nf18a3Request r={0};r.transition=NF18A3_STEP;r.actor_id=5;r.feature_id=feature;
 r.feet=(NfVec3){-.31f,0,0};r.destination=(NfVec3){.8f,.3f,0};r.facing=(NfVec3){1,0,0};
 r.body_radius=.3f;r.body_height=1.8f;r.foot_flat_radius=.1f;r.max_step=.4f;
 r.max_ladder_reach=.8f;r.min_facing_cosine=.5f;r.min_landing_width=.07f;r.grounded=1;
 return r;
}
static long double sqdist(long double v,long double a,long double b){
 long double d=v<a?a-v:(v>b?v-b:0.0L);return d*d;
}
static long double capsule_dist2(NfVec3 p,NfVec3 delta,Nf18a2ShapePolicy sh,
                                  Nf18aCollider b,long double t){
 long double x=p.x+(long double)delta.x*t;
 long double y=p.y+(long double)delta.y*t;
 long double z=p.z+(long double)delta.z*t;
 long double bot=y+sh.radius,top=y+sh.height-sh.radius;
 long double dy=bot>b.max.y?bot-b.max.y:(top<b.min.y?b.min.y-top:0.0L);
 return sqdist(x,b.min.x,b.max.x)+sqdist(z,b.min.z,b.max.z)+dy*dy;
}
static int oracle_toi(NfVec3 p,NfVec3 d,Nf18a2ShapePolicy sh,Nf18aCollider b,
                      long double *toi){
 const long double r2=(long double)sh.radius*sh.radius;
 if(capsule_dist2(p,d,sh,b,0.0L)<r2-1e-9L)return -1; /* start overlap */
 long double a=0.0L,c=1.0L;
 /* squared distance to a translating convex capsule against a convex box
    is convex; ternary gives an independent minimum, then binary first entry. */
 for(int i=0;i<80;++i){
   long double m1=a+(c-a)/3.0L,m2=c-(c-a)/3.0L;
   if(capsule_dist2(p,d,sh,b,m1)<capsule_dist2(p,d,sh,b,m2))c=m2;
   else a=m1;
 }
 const long double tmin=(a+c)*0.5L;
 if(capsule_dist2(p,d,sh,b,tmin)>r2+1e-10L)return 0;
 long double lo=0.0L,hi=tmin;
 for(int i=0;i<80;++i){
   long double mid=(lo+hi)*0.5L;
   if(capsule_dist2(p,d,sh,b,mid)<=r2)hi=mid;else lo=mid;
 }
 if(tmin<1e-10L && capsule_dist2(p,d,sh,b,1e-7L)>=r2)return 0;
 *toi=(lo+hi)*0.5L;
 return 1;
}
static void trajectories(FILE *out){
 uint32_t state=9137u;
 fprintf(out,"sample,oracle,analytic,toi_oracle,toi_analytic,absolute_error,body_x,body_z,displace_x,displace_z\n");
 Nf18a2ShapePolicy sh={.3f,1.8f,.1f,.4f,.7f};
 Nf18aCollider obs=box(99,0,-1,0,1,2,1);
 for(unsigned i=0;i<2400u;++i){
   const NfVec3 p={uniform(&state,-2.3f,-.32f),uniform(&state,-.1f,.1f),uniform(&state,-1.7f,2.2f)};
   const NfVec3 d={uniform(&state,.15f,4.1f),uniform(&state,-.25f,.25f),uniform(&state,-2.1f,2.1f)};
   Nf18aContact hit={0};const bool got=nf18a3_capsule_sweep(p,sh,d,obs,1.0f,&hit);
   long double want=0.0L;const int expected=oracle_toi(p,d,sh,obs,&want);
   long double err=expected==1 && got?fabsl((long double)hit.toi-want):0.0L;
   fprintf(out,"%u,%d,%u,%.12Lf,%.12f,%.12Lf,%.6f,%.6f,%.6f,%.6f\n",
     i,expected,got?1u:0u,want,got?hit.toi:0.0f,err,p.x,p.z,d.x,d.z);
 }
}
int main(int argc,char **argv){
 const char *p=argc>1?argv[1]:"build/v18a3/shape_samples.csv";
 const char *q=argc>2?argv[2]:"build/v18a3/sweep_oracle.csv";
 const char *edge_path=argc>3?argv[3]:"build/v18a3/heldout_support.csv";
 FILE *out=fopen(p,"w");if(!out)return 2;
 fprintf(out,"case,cohort,scenario,policy,expected,geometry_ok,contract_ok,commit_eligible,correct,witness_hash,queries\n");
 const Nf18a3Policy policies[5]={NF18A3_BOX_BASELINE,NF18A3_CAPSULE_STRICT,
     NF18A3_HYBRID_DUAL,NF18A3_CAPSULE_EASY_NEGATIVE,NF18A3_HYBRID_AUTH_BYPASS_NEGATIVE};
 uint32_t state=719281u;
 for(unsigned case_id=0;case_id<4000u;++case_id){
    const unsigned scenario=case_id%16u;
    const unsigned cohort=(case_id/16u)%5u;
    const float h=uniform(&state,.08f,.38f);
    Nf18aCollider solids[3]={box(60,-3,-1,-3,0,0,3),box(41,0,0,-2,2,h,2),box(0,0,0,0,0,0,0)};
    Nf18aCollider ladder=box(70,0,0,-.05f,.15f,2,.05f);
    size_t n=2u,ln=0u;
    Nf18a3Request r=good_request(41);
    Nf18a3Contract c=good_contract(41,r.feet);
    r.destination.y=h;
    int expected=0;
    switch(scenario){
        case 0: expected=1;break;
        case 1: solids[1].max.y=uniform(&state,.45f,1.1f);r.destination.y=solids[1].max.y;break;
        case 2: solids[2]=box(51,-1,1.8f+h*0.45f,-2,2,2.3f,2);n=3;break;
        case 3: r.destination.x=uniform(&state,.01f,.06f);break;
        case 4: c.may_step=0;break;
        case 5: c.world_version=8;break;
        case 6: r.feet.x=-.55f;break;
        case 7: r.body_radius=.15f;break;
        case 8: case 9: case 10: case 11: case 12: case 13:
            ln=1;n=1;r=good_request(70);c=good_contract(70,r.feet);
            if(scenario==12 || scenario==13) {
                r.transition=NF18A3_LADDER_EXIT;r.ladder_attached=1;
                r.feet=(NfVec3){-.35f,1,0};r.destination=(NfVec3){.9f,1,0};
                c.actor_ladder_attached=1;
                if(scenario==12){solids[1]=box(72,0,0,-2,2,1,2);n=2;expected=1;}
            } else {
                r.transition=NF18A3_LADDER_ATTACH;
                r.feet=(NfVec3){-.65f,0,0};r.destination=(NfVec3){-.35f,0,0};
                if(scenario==8)expected=1;
                if(scenario==9)r.facing=(NfVec3){-1,0,0};
                if(scenario==10)r.feet.x=-1.5f;
                if(scenario==11){solids[1]=box(44,-.5f,0,-.5f,-.32f,2,.5f);n=2;}
            }
            c.authoritative_feet=r.feet;
            break;
        case 14: r.grounded=1;c.actor_grounded=0;break;
        case 15: r.max_step=uniform(&state,.41f,.8f);break;
        default:break;
    }
    Nf18a3Geometry w={solids,n,ln?&ladder:NULL,ln,9,70};
    for(unsigned k=0;k<5u;++k){
       r.policy=policies[k];
       Nf18a3Decision d=nf18a3_adjudicate(w,c,r);
       const int correct=(int)d.commit_eligible==expected;
       fprintf(out,"%u,%u,%u,%u,%d,%u,%u,%u,%d,%u,%u\n",case_id,cohort,scenario,k,
          expected,d.geometry_approved,d.contract_approved,d.commit_eligible,correct,d.witness_hash,d.shape_queries);
    }
 }
 fclose(out);
 FILE *edge=fopen(edge_path,"w");if(!edge)return 2;
 fprintf(edge,"case,policy,landing_x,foot_patch_expected,commit_eligible,correct\n");
 uint32_t heldout=844177u;
 for(unsigned i=0;i<1200u;++i){
    const float h=uniform(&heldout,.12f,.36f);
    const float x=uniform(&heldout,.03f,.18f);
    Nf18aCollider solids[2]={box(60,-3,-1,-3,0,0,3),box(41,0,0,-2,2,h,2)};
    Nf18a3Geometry w={solids,2,NULL,0,9,70};
    Nf18a3Request r=good_request(41);r.destination.x=x;r.destination.y=h;
    Nf18a3Contract c=good_contract(41,r.feet);
    const int expect=x>=.0999f;
    for(unsigned k=0;k<5u;++k){
       r.policy=policies[k];
       Nf18a3Decision d=nf18a3_adjudicate(w,c,r);
       fprintf(edge,"%u,%u,%.7f,%d,%u,%d\n",i,k,x,expect,
           d.commit_eligible,(int)d.commit_eligible==expect);
    }
 }
 fclose(edge);
 FILE *oracle=fopen(q,"w");if(!oracle)return 2;
 trajectories(oracle);fclose(oracle);
 printf("1.8A.3 samples: 4000 matched fixtures; 1200 held-out foot-patch fixtures; 2400 numerical-oracle sweeps\n");
 return 0;
}
