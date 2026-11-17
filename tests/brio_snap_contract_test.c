/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "brio_snap_contract.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
static int checks=0, fails=0;
#define CHECK(x) do { ++checks; if (!(x)) { ++fails; fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x); } } while(0)
static nf_brio_surface surface(uint32_t id, int32_t x, int32_t z) {
    return (nf_brio_surface){id,x,z,1000,0,NF_BRIO_ALLOW,NF_BRIO_ALLOW,NF_BRIO_ALLOW};
}
static uint32_t rand32(uint32_t *st) { *st ^= *st<<13; *st ^= *st>>17; *st ^= *st<<5; return *st; }
int main(void) {
    nf_brio_actor a={0,0,1000,0,500,1}; nf_brio_choice choice={0,0,0};
    nf_brio_surface s=surface(5,300,0);
    CHECK(nf_brio_choose(&a,&s,1,&choice)==1 && choice.surface_id==5 && choice.distance_sq_mm==90000);
    s.x_mm=500; CHECK(nf_brio_choose(&a,&s,1,&choice)==1); /* inclusive boundary */
    s.x_mm=501; CHECK(nf_brio_choose(&a,&s,1,&choice)==0); /* outside */
    s=surface(5,0,0); s.body_clear=NF_BRIO_DENY; CHECK(nf_brio_choose(&a,&s,1,&choice)==0);
    s.body_clear=NF_BRIO_PENDING; CHECK(nf_brio_choose(&a,&s,1,&choice)==0);
    s.body_clear=NF_BRIO_ALLOW; s.material_clear=NF_BRIO_DENY; CHECK(nf_brio_choose(&a,&s,1,&choice)==0);
    s.material_clear=NF_BRIO_PENDING; CHECK(nf_brio_choose(&a,&s,1,&choice)==0);
    s.material_clear=NF_BRIO_ALLOW; s.grant_current=NF_BRIO_DENY; CHECK(nf_brio_choose(&a,&s,1,&choice)==0);
    s.grant_current=NF_BRIO_PENDING; CHECK(nf_brio_choose(&a,&s,1,&choice)==0);
    s.grant_current=NF_BRIO_ALLOW; s.tangent_x=-1000; CHECK(nf_brio_choose(&a,&s,1,&choice)==0);
    s.tangent_x=0; CHECK(nf_brio_choose(&a,&s,1,&choice)==0);
    s.tangent_x=1001; CHECK(nf_brio_choose(&a,&s,1,&choice)==0);
    s=surface(5,0,0); s.surface_id=0; CHECK(nf_brio_choose(&a,&s,1,&choice)==0);
    s=surface(5,0,0); CHECK(nf_brio_choose(NULL,&s,1,&choice)==-1);
    CHECK(nf_brio_choose(&a,NULL,1,&choice)==-1);
    CHECK(nf_brio_choose(&a,&s,1,NULL)==-1);
    a.snap_radius_mm=10001; CHECK(nf_brio_choose(&a,&s,1,&choice)==-1); a.snap_radius_mm=500;
    a.min_forward_dot=-1; CHECK(nf_brio_choose(&a,&s,1,&choice)==-1); a.min_forward_dot=1;
    CHECK(nf_brio_choose(&a,NULL,0,&choice)==0);
    nf_brio_surface list[3]={surface(30,100,0),surface(10,100,0),surface(20,50,0)};
    CHECK(nf_brio_choose(&a,list,3,&choice)==1 && choice.surface_id==20);
    list[2].body_clear=NF_BRIO_PENDING;
    CHECK(nf_brio_choose(&a,list,3,&choice)==1 && choice.surface_id==10);
    nf_brio_surface tmp=list[0];list[0]=list[1];list[1]=tmp;
    CHECK(nf_brio_choose(&a,list,3,&choice)==1 && choice.surface_id==10);
    a.x_mm=INT32_MAX; a.z_mm=INT32_MIN;
    CHECK(nf_brio_choose(&a,list,3,&choice)==0); /* extreme differences safely rejected */
    a=(nf_brio_actor){0,0,1000,0,500,1};
    uint32_t seed=0x15c2b319u;
    for (int i=0; i<10000; ++i) {
        nf_brio_surface k=surface(1, (int32_t)(rand32(&seed)%1501)-750,(int32_t)(rand32(&seed)%1501)-750);
        int result=nf_brio_choose(&a,&k,1,&choice);
        int64_t d2=(int64_t)k.x_mm*k.x_mm+(int64_t)k.z_mm*k.z_mm;
        CHECK(result==(d2<=250000?1:0));
        nf_brio_surface v[2]={k,surface(2,k.x_mm,k.z_mm)};
        CHECK(nf_brio_choose(&a,v,2,&choice)==result);
        if (result) CHECK(choice.surface_id==1); /* stable tie */
    }
    printf("BRIO snap contract checks=%d failed=%d seed=0x15c2b319\n",checks,fails);
    return fails?1:0;
}
