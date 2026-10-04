#include "nf_contact18a2.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static uint32_t rand32(uint32_t *s) {
    uint32_t x=*s;
    x^=x<<13; x^=x>>17; x^=x<<5;
    *s=x;return x;
}
static float randomf(uint32_t *s,float min,float max) {
    return min+((float)(rand32(s)&0xffffffu)/16777215.0f)*(max-min);
}
static Nf18aCollider obstacle(uint32_t id,float x) {
    Nf18aCollider c={0};c.body_id=id;
    c.min=(NfVec3){x,-2.0f,-4.0f}; c.max=(NfVec3){x+0.025f,3.0f,4.0f};
    return c;
}
int main(int argc,char **argv) {
    const char *path=argc>1?argv[1]:"v18a2_samples.csv";
    FILE *f=fopen(path,"w");if(!f)return 2;
    fprintf(f,"seed,cohort,shape_separation,box_overlap,capsule_overlap,motor_accel,impulse,"
              "momentum_error,normal_relative_after,adaptive_status,adaptive_attempts,"
              "adaptive_final_budget,history_summaries,history_events,history_evictions\n");
    unsigned count=0,hash_fail=0,impulse_fail=0;
    const Nf18a2ShapePolicy shape={0.3f,1.8f,0.24f,0.35f,0.707f};
    for(unsigned cohort=0u;cohort<5u;++cohort)for(uint32_t seed=1u;seed<=200u;++seed) {
        uint32_t rs=seed*747796405u+cohort*2891336453u;
        if(rs==0u)rs=1u;
        const float x=randomf(&rs,0.05f,0.45f);
        const float height=randomf(&rs,0.01f,0.5f);
        Nf18aCollider step=obstacle(11u,x); step.min.y=-1.0f;step.max.y=height;
        const float sep=nf18a2_capsule_box_separation((NfVec3){0},shape,&step);
        const int box_overlap=x<shape.radius && height>0.0f;
        const int capsule_overlap=sep<0.0f;
        Nf18a2RigidBody motor={1u,{0.0f,0.0f,0.0f},randomf(&rs,40.0f,115.0f)};
        const float force=randomf(&rs,200.0f,1900.0f);
        const Nf18a2Motor target={randomf(&rs,1.0f,16.0f),0.0f,40.0f,force,9.8f,0.0f};
        const float dt=1.0f/60.0f;
        if(!nf18a2_motor_step(&motor,target,dt,true)){impulse_fail++;continue;}
        Nf18a2RigidBody a={2u,{randomf(&rs,0.1f,20.0f),0,0},randomf(&rs,50.0f,130.0f)};
        Nf18a2RigidBody b={3u,{randomf(&rs,-5.0f,0.0f),0,0},randomf(&rs,5.0f,85.0f)};
        const float initial_momentum=a.mass*a.velocity.x+b.mass*b.velocity.x;
        float impulse=0.0f;
        const float restitution=randomf(&rs,0,0.75f);
        if(!nf18a2_exchange_normal_impulse(&a,&b,(NfVec3){-1,0,0},restitution,&impulse)){
            ++impulse_fail;continue;
        }
        const float error=fabsf(a.mass*a.velocity.x+b.mass*b.velocity.x-initial_momentum);
        const float rel=a.velocity.x-b.velocity.x;
        if(error>0.003f)++impulse_fail;
        const Nf18aCollider wall=obstacle(13u,randomf(&rs,0.6f,1.3f));
        const NfVec3 input_velocity={randomf(&rs,50.0f,280.0f),0.0f,randomf(&rs,-8.0f,8.0f)};
        Nf18a2AdaptiveResult ar=nf18a2_solve_adaptive(seed,1u,(NfVec3){0},input_velocity,
           (Nf18aShape){0.3f,1.8f,80.0f},dt,&wall,1u,nf18a_default_config(),
           nf18a2_default_adaptive());
        Nf18a2AdaptiveResult repeat=nf18a2_solve_adaptive(seed,1u,(NfVec3){0},input_velocity,
           (Nf18aShape){0.3f,1.8f,80.0f},dt,&wall,1u,nf18a_default_config(),
           nf18a2_default_adaptive());
        if(nf18a_result_hash(&ar.solve)!=nf18a_result_hash(&repeat.solve))++hash_fail;
        Nf18a2CondensedHistory h;
        nf18a2_condensed_history_init(&h,nf18a2_default_history_policy());
        for(uint32_t tick=1u;tick<=90u;++tick) {
            Nf18aContact sample={0};sample.body_a=1u;sample.body_b=13u;sample.contact_id=321u;
            sample.tick=tick;
            sample.normal_impulse=(cohort==0u?0.25f:(cohort==1u?2.0f:
                  (cohort==2u&&tick%20u==0u?30.0f:
                  (cohort==3u?0.6f:randomf(&rs,0.0f,3.0f)))));
            sample.approach_speed=sample.normal_impulse*0.5f;
            if(!nf18a2_history_commit_samples(&h,tick,tick,1u,&sample,1u)){
                impulse_fail++;break;
            }
        }
        fprintf(f,"%u,%u,%.6f,%d,%d,%.6f,%.6f,%.9f,%.6f,%u,%u,%u,%u,%u,%u\n",
                seed,cohort,sep,box_overlap,capsule_overlap,motor.velocity.x/dt,
                impulse,error,rel,(unsigned)ar.solve.status,(unsigned)ar.attempts,
                (unsigned)ar.final_budget,h.summary_count,h.event_count,h.summary_evictions);
        ++count;
    }
    fclose(f);
    fprintf(stderr,"v1.8A.2 H1 prototype samples: rows=%u hash_mismatches=%u numerical_or_commit_failures=%u\n",
            count,hash_fail,impulse_fail);
    return count==1000u && hash_fail==0u && impulse_fail==0u?0:1;
}
