#define _POSIX_C_SOURCE 200809L
#include "v18b2_fixtures.h"
#include <math.h>
#include <stdio.h>
#include <time.h>
static uint32_t random32(uint32_t *s){*s^=*s<<13;*s^=*s>>17;*s^=*s<<5;return *s;}
static uint64_t now_ns(void){
    struct timespec t;if(clock_gettime(CLOCK_MONOTONIC,&t))abort();
    return (uint64_t)t.tv_sec*UINT64_C(1000000000)+(uint64_t)t.tv_nsec;
}
static Nf18b2UseDecision model(B2Fixture *f,unsigned m){
    if(m>=3)return nf18b2_evaluate(&f->scene,f->use,(Nf18b2Model)m);
    /* Scientific ablations only, absent from the runtime module. These
       implement actual omissions, not artificial timing/busy-work proxies. */
    Nf18b2UseDecision d={.status=NF18B2_ELIGIBLE,.physical=NF18B2_ELIGIBLE,
        .contract=NF18B2_ELIGIBLE,.reason=NF18B2_OK};
    const Nf18b2Actor *a=&f->actors[0];const Nf18b2Object *o=&f->objects[0];
    if(!m)return d;
    if(m==1){
        if(!a->active)d.reason=NF18B2_POSE;
        else {
            const double dx=(double)o->target.x-a->feet.x;
            const double dy=(double)o->target.y-a->feet.y-a->eye_height;
            const double dz=(double)o->target.z-a->feet.z;
            if(dx*dx+dy*dy+dz*dz>(double)o->max_reach*o->max_reach)d.reason=NF18B2_REACH;
        }
    }
    if(d.reason==NF18B2_OK&&(a->credentials&o->required_credentials)!=o->required_credentials)d.reason=NF18B2_CREDENTIAL;
    if(m==2&&d.reason==NF18B2_OK&&(a->inventory&o->required_inventory)!=o->required_inventory)d.reason=NF18B2_INVENTORY;
    if(d.reason==NF18B2_OK&&!o->preconditions)d.reason=NF18B2_PRECONDITION;
    if(m==2&&d.reason==NF18B2_OK&&o->member_count&&o->members[0]!=a->id)d.reason=NF18B2_MEMBERSHIP;
    if(m==2&&d.reason==NF18B2_OK&&o->occupant_count>=o->capacity&&o->occupants[0]!=a->id)d.reason=NF18B2_CAPACITY;
    if(d.reason!=NF18B2_OK)d.status=NF18B2_BLOCKED;
    return d;
}
static volatile uint32_t sink;
int main(int argc,char **argv){
    if(argc!=3){fprintf(stderr,"usage: %s calibration|held-out output.csv\n",argv[0]);return 2;}
    const bool held=strcmp(argv[1],"held-out")==0;
    if(!held&&strcmp(argv[1],"calibration")!=0)return 2;
    FILE *fp=fopen(argv[2],"w");if(!fp)return 2;
    fputs("corpus,case,seed,flags,model,expected_status,status,expected_reason,reason,unsafe,correct,replay,ns,geometry_queries,material_queries\n",fp);
    static const char *const names[]={"canonical","randomized","pathological","boundary","held-out"};
    static const unsigned counts[]={2048,8192,1024,2048,8192};
    B2Fixture f;uint32_t rng=held?UINT32_C(0xB218CAFE):UINT32_C(0x18312345);
    unsigned tested=0,errors=0;
    for(unsigned c=held?4:0;c<(held?5u:4u);++c)for(unsigned i=0;i<counts[c];++i){
        const uint32_t seed=rng;
        unsigned flags=c==0?i:random32(&rng)&4095u;
        if((c==1||c==4)&&i%4==0)flags=0; /* include positive controls */
        if(c==3)flags=i%2?F_REACH:0;
        fixture(&f,flags);
        Nf18b2Status truth=expected_status(flags);Nf18b2Reason reason=expected_reason(flags);
        if(c==1||c==4){
            f.actors[0].feet.x+=.05f*(float)(random32(&rng)%1000)/1000;
            f.actors[0].feet.z+=.05f*(float)(random32(&rng)%1000)/1000;
            f.actors[0].radius+=.05f*(float)(random32(&rng)%1000)/1000;
            f.objects[0].target.x-=.1f*(float)(random32(&rng)%1000)/1000;
            f.objects[0].target.z=f.actors[0].feet.z;
        }
        if(c==3)f.objects[0].max_reach=i%2?nextafterf(2,0):2;
        if(c==2){
            switch(i%8){
            case 0:f.actors[0].feet.x=NAN;truth=NF18B2_INVALID;reason=NF18B2_BAD_INPUT;break;
            case 1:f.objects[0].max_reach=INFINITY;truth=NF18B2_INVALID;reason=NF18B2_BAD_INPUT;break;
            case 2:f.objects[0].occupant_count=17;truth=NF18B2_INVALID;reason=NF18B2_BAD_INPUT;break;
            case 3:f.use.actor_id=0;truth=NF18B2_INVALID;reason=NF18B2_BAD_INPUT;break;
            default:break;
            }
        }
        /* Rotate measurement order; each timing is an eight-call native batch
           average. File output and fixture construction are outside timing. */
        for(unsigned k=0;k<5;++k){
            const unsigned m=(k+i)%5;
            Nf18b2UseDecision d=model(&f,m),repeat=model(&f,m);
            const bool replay=d.status==repeat.status&&d.reason==repeat.reason&&
                d.physical==repeat.physical&&d.contract==repeat.contract;
            const uint64_t begin=now_ns();
            for(unsigned j=0;j<8;++j){Nf18b2UseDecision v=model(&f,m);sink+=(uint32_t)v.status;}
            const double ns=(double)(now_ns()-begin)/8;
            const bool correct=d.status==truth&&d.reason==reason;
            const bool unsafe=d.status==NF18B2_ELIGIBLE&&truth!=NF18B2_ELIGIBLE;
            fprintf(fp,"%s,%u,%u,%u,M%u,%d,%d,%d,%d,%d,%d,%d,%.3f,%u,%u\n",
                names[c],i,seed,flags,m,truth,d.status,reason,d.reason,unsafe,correct,replay,ns,
                d.geometry_queries,d.material_queries);
            ++tested;if(m>=3&&(!correct||!replay))++errors;
        }
    }
    if(fclose(fp))return 2;
    printf("evaluations=%u admissible_errors=%u sink=%u\n",tested,errors,sink);
    return errors?1:0;
}
