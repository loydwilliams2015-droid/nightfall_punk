#include "nf_contact18a.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static uint32_t seed_rng(uint32_t *s) {*s^=*s<<13;*s^=*s>>17;*s^=*s<<5;return *s;}
static float uniform(uint32_t *s,float low,float high) {return low+(high-low)*(float)(seed_rng(s)%10001u)/10000.0f;}
static Nf18aCollider box(uint32_t id,float a,float b,float c,float d,float e,float f) {
    Nf18aCollider r={0};r.body_id=id;r.min=(NfVec3){a,b,c};r.max=(NfVec3){d,e,f};r.material_channel=1;return r;
}
static bool old_endpoint_free(NfVec3 p,Nf18aShape sh,NfVec3 velocity,float dt,
                              const Nf18aCollider *colliders,size_t n) {
    const NfVec3 next={p.x+velocity.x*dt,p.y+velocity.y*dt,p.z+velocity.z*dt};
    for(size_t k=0;k<n;++k) {
        const Nf18aCollider *c=&colliders[k];
        if(next.x+sh.radius>c->min.x && next.x-sh.radius<c->max.x &&
           next.y+sh.height>c->min.y && next.y<c->max.y &&
           next.z+sh.radius>c->min.z && next.z-sh.radius<c->max.z) return false;
    }
    return true;
}
static double stamp(void) {return (double)clock()/(double)CLOCKS_PER_SEC;}
static void fixture(int scene,uint32_t *rng,unsigned density,NfVec3 *p,NfVec3 *v,
                    Nf18aCollider *b,size_t *n) {
    float jitter=uniform(rng,-0.06f,0.06f);
    float speed=uniform(rng,170.0f,330.0f);
    *p=(NfVec3){jitter,0,jitter};
    *n=0;
    switch(scene) {
        case 0: /* thin barrier, jumping past final position */
            *v=(NfVec3){speed,0,0};
            b[(*n)++]=box(10,1.0f,-10,-10,1.01f,10,10);
            break;
        case 1: /* diagonal along wall, strictly positive tangential residual */
            *v=(NfVec3){speed,0,uniform(rng,15,45)};
            b[(*n)++]=box(10,1,-10,-10,1.01f,10,10);
            break;
        case 2: /* corner, two independent normals */
            *v=(NfVec3){speed,0,speed};
            b[(*n)++]=box(10,1,-10,-10,1.01f,10,10);
            b[(*n)++]=box(11,-10,-10,1,10,10,1.01f);
            break;
        default: /* high-speed floor, support normal */
            p->y=uniform(rng,0.8f,1.6f);
            *v=(NfVec3){0,-speed,0};
            b[(*n)++]=box(12,-10,-1,-10,10,0,10);
            break;
    }
    for(unsigned j=0;j<density;++j) {
        if(*n>=128)break;
        /* Deliberately irrelevant spatial bodies: broadphase should exclude. */
        const float x=15.0f+(float)j*2.0f;
        b[(*n)++]=box(100+j,x,-3.0f,18.0f,x+0.5f,4.0f,19.0f);
    }
}
int main(int argc,char **argv) {
    if(argc!=2) {fprintf(stderr,"usage: sample_v18a_contact output.csv\n");return 2;}
    FILE *out=fopen(argv[1],"w");if(!out)return 2;
    fprintf(out,"stratum,seed,scene,budget,density,replicate,old_endpoint_free,old_endpoint_crossed,"
            "contact_tunnel,stable_hash,pending,contacts,support,broadphase,narrowphase,"
            "speed,toi,impulse,solver_cpu_us\n");
    unsigned rows=0,fail=0;
    const unsigned densities[3]={0u,16u,64u};
    const unsigned budgets[4]={1u,2u,4u,8u};
    for(unsigned cls=0;cls<5;++cls)
    for(unsigned seed=0;seed<50;++seed)
    for(unsigned scene=0;scene<4;++scene)
    for(unsigned bi=0;bi<4;++bi)
    for(unsigned di=0;di<3;++di) {
        uint32_t rng=(0x51eedd13u+cls*0x9e3779b9u+seed*1831u+scene*12347u);
        NfVec3 feet,velocity;
        Nf18aCollider b[128];size_t count=0;
        fixture((int)scene,&rng,densities[di],&feet,&velocity,b,&count);
        Nf18aConfig cfg=nf18a_default_config();cfg.iteration_budget=(uint8_t)budgets[bi];
        Nf18aShape sh={0.3f,1.8f,80.0f};
        const bool old=old_endpoint_free(feet,sh,velocity,1.0f/60.0f,b,count);
        const int old_crossed=old && ((scene==3)?feet.y>0.0f:feet.x<0.7f);
        const double start=stamp();
        Nf18aSolveResult r=nf18a_solve(1u,1u,feet,velocity,sh,1.0f/60.0f,b,count,cfg);
        const double cost=(stamp()-start)*1.0e6;
        Nf18aSolveResult r2=nf18a_solve(1u,1u,feet,velocity,sh,1.0f/60.0f,b,count,cfg);
        bool tunnel=(scene==3)?r.feet.y < -0.001f:r.feet.x>0.701f;
        if(scene==2 && r.feet.z>0.701f)tunnel=true;
        const bool stable=nf18a_result_hash(&r)==nf18a_result_hash(&r2);
        if(tunnel||!stable||r.status==NF18A_INVALID_START||r.status==NF18A_INVALID_INPUT) ++fail;
        /* Strata are reproducible challenge seeds; actual classification is based on fixture outputs,
           not assumed to be the median or worst run until data analysis. */
        fprintf(out,"%u,%u,%u,%u,%u,0,%u,%u,%u,%u,%u,%u,%u,%u,%u,%.3f,%.6f,%.3f,%.3f\n",
            cls,seed,scene,budgets[bi],densities[di],old?1u:0u,old_crossed?1u:0u,
            tunnel?1u:0u,stable?1u:0u,r.status==NF18A_PENDING_BUDGET?1u:0u,r.contact_count,
            r.support,r.broadphase_candidates,r.narrowphase_tests,
            sqrtf(velocity.x*velocity.x+velocity.y*velocity.y+velocity.z*velocity.z),
            r.contact_count>0?r.contacts[0].toi:-1.0f,
            r.contact_count>0?r.contacts[0].normal_impulse:0.0f,cost);
        ++rows;
    }
    fclose(out);
    printf("native_collision_sample_rows=%u critical_failures=%u\n",rows,fail);
    return fail?1:0;
}