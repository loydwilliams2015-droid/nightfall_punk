/* SPDX-License-Identifier: GPL-3.0-or-later */
/* BRIO research observation: serial and parallel results must be byte-equivalent
 * in semantic fields. This checks PURE prefilter calls only, not live A6 threads. */
#include "brio_snap_contract.h"
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#define CASES 4096u
#define THREADS 4u
#define ROUNDS 8u
typedef struct { int result; uint32_t id; int64_t distance_sq; int32_t dot; } observation;
typedef struct { size_t begin,end; observation *out; unsigned round; } worker;
static uint32_t rnd(uint32_t *s) { *s^=*s<<13;*s^=*s>>17;*s^=*s<<5;return *s; }
static observation examine(size_t index, unsigned round) {
    uint32_t seed=0x19c32026u ^ ((uint32_t)index*0x9e3779b9u) ^ (round*0x85ebca6bu);
    if (!seed) seed=1;
    nf_brio_actor actor={(int32_t)(rnd(&seed)%501)-250, (int32_t)(rnd(&seed)%501)-250,
        (int32_t)(rnd(&seed)%2001)-1000,(int32_t)(rnd(&seed)%2001)-1000, 500, 0};
    nf_brio_surface s[4];
    for (size_t j=0;j<4;j++) {
        s[j]=(nf_brio_surface){(uint32_t)(j+1),
            (int32_t)(rnd(&seed)%1501)-750,(int32_t)(rnd(&seed)%1501)-750,
            1000,0,NF_BRIO_ALLOW,NF_BRIO_ALLOW,NF_BRIO_ALLOW};
        unsigned gate=rnd(&seed)%12;
        if (gate==0) s[j].body_clear=NF_BRIO_PENDING;
        if (gate==1) s[j].material_clear=NF_BRIO_DENY;
        if (gate==2) s[j].grant_current=NF_BRIO_PENDING;
        if (gate==3) s[j].grant_current=NF_BRIO_DENY;
    }
    nf_brio_choice c={0,0,0};
    int result=nf_brio_choose(&actor,s,4,&c);
    observation o={result,result==1?c.surface_id:0,
        result==1?c.distance_sq_mm:0,result==1?c.forward_dot:0};
    return o;
}
static void *task(void *p) {
    worker *w=p;
    for(size_t i=w->begin;i<w->end;i++) w->out[i]=examine(i,w->round);
    return NULL;
}
static int equal(observation a,observation b) {
    return a.result==b.result && a.id==b.id &&
           a.distance_sq==b.distance_sq && a.dot==b.dot;
}
int main(void) {
    static observation forward[CASES],reverse[CASES],parallel[CASES];
    size_t checks=0,failures=0;
    uint64_t signature=1469598103934665603ull;
    for(unsigned round=0;round<ROUNDS;round++) {
        for(size_t i=0;i<CASES;i++) forward[i]=examine(i,round);
        for(size_t i=CASES;i>0;i--) reverse[i-1]=examine(i-1,round);
        pthread_t threads[THREADS];
        worker w[THREADS];
        unsigned started=0;
        for(unsigned t=0;t<THREADS;t++) {
            w[t]=(worker){(size_t)t*CASES/THREADS,
                (size_t)(t+1)*CASES/THREADS,parallel,round};
            if (pthread_create(&threads[t],NULL,task,&w[t])!=0) {
                fprintf(stderr,"pthread_create failure round=%u thread=%u\n",round,t);
                for(unsigned k=0;k<started;k++) pthread_join(threads[k],NULL);
                return 2;
            }
            started++;
        }
        for(unsigned t=0;t<THREADS;t++)
            if(pthread_join(threads[t],NULL)!=0) { fprintf(stderr,"pthread_join failed\n");return 2; }
        for(size_t i=0;i<CASES;i++) {
            checks+=2;
            if(!equal(forward[i],reverse[i]) || !equal(forward[i],parallel[i])) {
                failures++;
                if(failures<8) fprintf(stderr,"mismatch round=%u case=%zu\n",round,i);
            }
            signature=(signature ^ (uint32_t)(forward[i].result+2))*1099511628211ull;
            signature=(signature ^ forward[i].id)*1099511628211ull;
            signature=(signature ^ (uint64_t)forward[i].distance_sq)*1099511628211ull;
            signature=(signature ^ (uint32_t)forward[i].dot)*1099511628211ull;
        }
    }
    printf("BRIO serial/parallel checks=%zu failures=%zu cases=%u rounds=%u workers=%u signature=%016" PRIx64 "\n",
         checks,failures,CASES,ROUNDS,THREADS,signature);
    return failures?1:0;
}
