#include "nf_contact18a4.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static uint32_t rng(uint32_t *s){*s^=*s<<13;*s^=*s>>17;*s^=*s<<5;return *s;}
static float unit(uint32_t *s){return (float)(rng(s)%100000u)/100000.0f;}
static Nf18a4Body bdy(uint32_t id,float mass,NfVec3 p,NfVec3 v){
    Nf18a4Body b={0};b.id=id;b.center=p;b.velocity=v;b.inverse_mass=1.0f/mass;
    b.inverse_inertia=(NfVec3){.1f,.1f,.1f};b.radius=.3f;b.height=1.8f;
    b.box_half=(NfVec3){.5f,.5f,.5f};return b;
}
static float vlen(NfVec3 v){return sqrtf(v.x*v.x+v.y*v.y+v.z*v.z);}
static float momx(Nf18a4Body a,Nf18a4Body b){return a.velocity.x/a.inverse_mass+b.velocity.x/b.inverse_mass;}
int main(int argc,char **argv){
    if(argc!=2){fprintf(stderr,"usage: sample_v18a4_contact <csv>\n");return 2;}
    FILE *f=fopen(argv[1],"w");if(!f)return 2;
    fprintf(f,"cohort,seed,policy,scenario,applied,normal_impulse,tangent_impulse,crate_delta_speed,angular_speed,linear_momentum_residual,energy_change,iterations,reserve,pending,replay_mismatch,status,constraint_evaluations\n");
    unsigned total=0u;
    for(unsigned cohort=0;cohort<5u;++cohort)for(unsigned seed=0;seed<200u;++seed){
        uint32_t s=20180804u+(cohort+1u)*7751u+(seed+1u)*11113u;
        const float mass_a=35.0f+130.0f*unit(&s),mass_b=10.0f+170.0f*unit(&s);
        const float speed=1.0f+13.0f*unit(&s), tangent=(unit(&s)-.5f)*8.0f;
        const float offset=(unit(&s)-.5f)*1.8f;
        const float restitution=unit(&s)*.75f, friction=.05f+unit(&s)*1.10f;
        for(int p=0;p<5;++p){
            Nf18a4Body a=bdy(1,mass_a,(NfVec3){0,0,0},(NfVec3){-speed,tangent,0});
            Nf18a4Body b=bdy(2,mass_b,(NfVec3){-1,0,0},(NfVec3){0,0,0});
            const Nf18a4Constraint c={1,2,41,{1,0,0},{-.5f,offset,0},restitution,friction};
            const float pi=momx(a,b);
            const Nf18a4Receipt r=nf18a4_apply_contact(&a,&b,c,(Nf18a4Policy)p,70,9);
            Nf18a4Body aa=bdy(1,mass_a,(NfVec3){0,0,0},(NfVec3){-speed,tangent,0});
            Nf18a4Body bb=bdy(2,mass_b,(NfVec3){-1,0,0},(NfVec3){0,0,0});
            const Nf18a4Receipt rr=nf18a4_apply_contact(&aa,&bb,c,(Nf18a4Policy)p,70,9);
            const int mismatch=fabsf(aa.velocity.x-a.velocity.x)>1e-6f ||
                fabsf(bb.velocity.y-b.velocity.y)>1e-6f ||
                fabsf(rr.normal_impulse.x-r.normal_impulse.x)>1e-6f;
            fprintf(f,"%u,%u,%s,pair,%u,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%u,%u,%u,%u,%u,%u\n",
                    cohort,seed,nf18a4_policy_name((Nf18a4Policy)p),r.impulses_applied,
                    vlen(r.normal_impulse),vlen(r.tangent_impulse),vlen(b.velocity),
                    vlen(a.omega)+vlen(b.omega),fabsf(momx(a,b)-pi),r.signed_energy_delta,
                    r.iterations,r.reserve_used,0u,mismatch,r.status,0u);
            ++total;
        }
        /* Same direct-contact inputs, but compare fixed vs adaptive ISLAND passes. */
        for(int p=3;p<5;++p){
            Nf18a4Body pair[2]={
                bdy(1,mass_a,(NfVec3){0,0,0},(NfVec3){-speed,tangent,0}),
                bdy(2,mass_b,(NfVec3){-1,0,0},(NfVec3){0,0,0})
            };
            const Nf18a4Constraint cc={1,2,41,{1,0,0},{-.5f,offset,0},restitution,friction};
            const float pi=momx(pair[0],pair[1]);
            const Nf18a4IslandResult ir=nf18a4_solve_island(pair,2,&cc,1,(Nf18a4Policy)p,70,9);
            Nf18a4Body repeat[2]={
                bdy(1,mass_a,(NfVec3){0,0,0},(NfVec3){-speed,tangent,0}),
                bdy(2,mass_b,(NfVec3){-1,0,0},(NfVec3){0,0,0})
            };
            const Nf18a4IslandResult again=nf18a4_solve_island(repeat,2,&cc,1,(Nf18a4Policy)p,70,9);
            const int mismatch=ir.status!=again.status ||
                fabsf(pair[0].velocity.x-repeat[0].velocity.x)>1e-6f ||
                fabsf(pair[1].velocity.y-repeat[1].velocity.y)>1e-6f;
            fprintf(f,"%u,%u,%s,island_single,%u,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%u,%u,%u,%u,%u,%u\n",
                cohort,seed,nf18a4_policy_name((Nf18a4Policy)p),
                ir.status==NF18A4_ACCEPTED,
                vlen(ir.receipts[0].normal_impulse),vlen(ir.receipts[0].tangent_impulse),
                vlen(pair[1].velocity),vlen(pair[0].omega)+vlen(pair[1].omega),
                fabsf(momx(pair[0],pair[1])-pi),ir.receipts[0].signed_energy_delta,
                ir.iterations,ir.reserve_used,ir.status==NF18A4_PENDING_CONTACT,
                mismatch,ir.status,ir.constraint_evaluations);
            ++total;
        }
    }
    /* A second held-out multi-contact chain corpus evaluates the 2/4/6 reserve
       without counting the paired contact-model rows as independent worlds. */
    for(unsigned cohort=0;cohort<5;++cohort)for(unsigned seed=0;seed<100u;++seed){
        uint32_t s=919187u+(cohort+1u)*119u+seed*4331u;
        const float speed=3.0f+6.0f*unit(&s),m=20.0f+40.0f*unit(&s);
        for(int p=3;p<5;++p){
            Nf18a4Body chain[3]={
                bdy(1,m,(NfVec3){0,0,0},(NfVec3){-speed,0,0}),
                bdy(2,m,(NfVec3){-1,0,0},(NfVec3){0,0,0}),
                bdy(3,m,(NfVec3){-2,0,0},(NfVec3){0,0,0})
            };
            const Nf18a4Constraint cs[2]={
                {1,2,10,{1,0,0},{-.5f,0,0},0,.1f},
                {2,3,11,{1,0,0},{-1.5f,0,0},0,.1f}
            };
            Nf18a4IslandResult ir=nf18a4_solve_island(chain,3,cs,2,(Nf18a4Policy)p,70,9);
            Nf18a4Body repeat[3]={
                bdy(1,m,(NfVec3){0,0,0},(NfVec3){-speed,0,0}),
                bdy(2,m,(NfVec3){-1,0,0},(NfVec3){0,0,0}),
                bdy(3,m,(NfVec3){-2,0,0},(NfVec3){0,0,0})
            };
            const Nf18a4IslandResult re=nf18a4_solve_island(repeat,3,cs,2,(Nf18a4Policy)p,70,9);
            int mismatch=ir.status!=re.status;
            for(int k=0;k<3;++k)mismatch|=fabsf(chain[k].velocity.x-repeat[k].velocity.x)>1e-6f;
            fprintf(f,"%u,%u,%s,chain,%u,0,0,%.8f,%.8f,%.8f,0,%u,%u,%u,%u,%u,%u\n",
                cohort,seed,nf18a4_policy_name((Nf18a4Policy)p),ir.status==NF18A4_ACCEPTED,
                fabsf(chain[2].velocity.x),0.0f,
                fabsf(chain[0].velocity.x+chain[1].velocity.x+chain[2].velocity.x+speed)*m,
                ir.iterations,ir.reserve_used,ir.status==NF18A4_PENDING_CONTACT,mismatch,ir.status,ir.constraint_evaluations);
            ++total;
        }
    }
    fclose(f);
    printf("samples=%u independent_pair_fixtures=1000 independent_chain_fixtures=500\n",total);
    return 0;
}
