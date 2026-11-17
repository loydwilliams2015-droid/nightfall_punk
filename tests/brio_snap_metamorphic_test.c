/* SPDX-License-Identifier: GPL-3.0-or-later */
/* BRIO SP05: metamorphic candidate ordering and invalid-candidate invariance.
 * Pure, isolated contract checks. No A4/A5/B2/A6 integration implied. */
#include "brio_snap_contract.h"
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#define SAMPLES 8192u
#define N 5u
static uint32_t next(uint32_t *x) { *x ^= *x<<13; *x ^= *x>>17; *x ^= *x<<5; return *x; }
static int same(int r,nf_brio_choice a,int q,nf_brio_choice b) {
    return r==q && (r!=1 || (a.surface_id==b.surface_id &&
        a.distance_sq_mm==b.distance_sq_mm && a.forward_dot==b.forward_dot));
}
int main(void) {
    uint32_t seed=0x51a71926u; unsigned failures=0; uint64_t checks=0;
    for(unsigned i=0;i<SAMPLES;i++) {
        nf_brio_actor actor={(int32_t)(next(&seed)%601)-300,
          (int32_t)(next(&seed)%601)-300,
          (int32_t)(next(&seed)%2001)-1000,
          (int32_t)(next(&seed)%2001)-1000,600,0};
        nf_brio_surface s[N];
        for(unsigned j=0;j<N;j++) {
            s[j]=(nf_brio_surface){j+1,
                (int32_t)(next(&seed)%1801)-900,
                (int32_t)(next(&seed)%1801)-900,
                1000,0,NF_BRIO_ALLOW,NF_BRIO_ALLOW,NF_BRIO_ALLOW};
            unsigned gate=next(&seed)%8;
            if (gate==0) s[j].body_clear=NF_BRIO_PENDING;
            if (gate==1) s[j].material_clear=NF_BRIO_DENY;
            if (gate==2) s[j].grant_current=NF_BRIO_PENDING;
            if (gate==3) s[j].grant_current=NF_BRIO_DENY;
        }
        nf_brio_choice gold={0},got={0};
        int expected=nf_brio_choose(&actor,s,N,&gold);
        if(expected<0) { fprintf(stderr,"invalid fixture\n");return 2; }
        for(unsigned k=0;k<N;k++) {
            nf_brio_surface v[N];
            for(unsigned j=0;j<N;j++) v[j]=s[(j+k)%N];
            int observed=nf_brio_choose(&actor,v,N,&got);checks++;
            if(!same(expected,gold,observed,got)) failures++;
        }
        nf_brio_surface extended[N+1];
        for(unsigned j=0;j<N;j++) extended[j]=s[j];
        for(unsigned gate=0;gate<3;gate++) {
            nf_brio_surface inv={UINT32_MAX,actor.x_mm,actor.z_mm,1000,0,
                NF_BRIO_ALLOW,NF_BRIO_ALLOW,NF_BRIO_ALLOW};
            if(gate==0) inv.body_clear=NF_BRIO_DENY;
            if(gate==1) inv.material_clear=NF_BRIO_PENDING;
            if(gate==2) inv.grant_current=NF_BRIO_DENY;
            extended[N]=inv;
            int observed=nf_brio_choose(&actor,extended,N+1,&got);checks++;
            if(!same(expected,gold,observed,got)) failures++;
        }
    }
    printf("BRIO metamorphic ordering checks=%" PRIu64 " failures=%u cases=%u\n",checks,failures,SAMPLES);
    return failures?1:0;
}
