#include "nf_retc_h319_a5_frontier.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"H319 A5_FRONTIER FAIL %d: %s\n",__LINE__,#x);return 1;}}while(0)
static NfRetcH319Detail decision(const NfRetcH319Region *r,
      const NfRetcH319A5Frontier *p,uint32_t id) {
    for(size_t i=0;i<p->plan.count;++i)
        if(r[i].id==id)return p->plan.decisions[i];
    return NF_RETC_H319_PENDING;
}
static NfRetcH319A5Evidence evidence(const NfRetcH319Region *r,
      const NfRetcH319A5Frontier *p,uint32_t id) {
    for(size_t i=0;i<p->plan.count;++i)
        if(r[i].id==id)return p->evidence[i];
    return NF_RETC_H319_A5_NOT_QUERIED;
}
int main(void){
    Nf18a5Grid *g=malloc(sizeof *g);CK(g);
    nf18a5_grid_init(g,3u,NF18A5_SELECTIVE_FINE);
    uint8_t voxels[NF18A5_CANON_VOXELS]={0};
    CK(nf18a5_load_canonical(g,0,0,0,voxels,1u));
    memset(voxels,NF18A5_MIXED,sizeof voxels);
    CK(nf18a5_load_canonical(g,1,0,0,voxels,1u));
    memset(voxels,NF18A5_SOLID,sizeof voxels);
    CK(nf18a5_load_canonical(g,2,0,0,voxels,1u));
    const NfRetcH319Region r[5]={
        {.id=10u,.work_cost=2u,.consequential=1u,.observed=1u,.active=1u,.resident=1u},
        {.id=20u,.work_cost=2u,.consequential=1u,.observed=1u,.resident=1u},
        {.id=30u,.work_cost=2u,.observed=1u,.active=1u,.resident=1u},
        {.id=40u,.work_cost=2u,.resident=1u},
        {.id=50u,.work_cost=2u,.consequential=1u,.resident=1u}
    };
    const NfVec3 points[5]={
        {.5f,.5f,.5f},{4.5f,.5f,.5f},{8.5f,.5f,.5f},
        {16.5f,.5f,.5f},{12.5f,.5f,.5f}
    };
    NfRetcH319Region reverse[5];
    NfVec3 reverse_points[5];
    for(size_t i=0;i<5u;++i){
        reverse[i]=r[4u-i];reverse_points[i]=points[4u-i];
    }
    const uint32_t epoch=g->global_revision;
    unsigned permutations=0u;
    for(uint32_t budget=0;budget<=12u;++budget){
        NfRetcH319A5Frontier a={0},b={0};
        CK(nf_retc_h319_a5_frontier(g,2u,epoch,r,points,5u,budget,&a));
        CK(nf_retc_h319_a5_frontier(g,2u,epoch,reverse,reverse_points,5u,budget,&b));
        CK(a.plan.spent==b.plan.spent&&a.plan.spent<=budget);
        CK(a.plan.detailed==b.plan.detailed&&a.plan.pending==b.plan.pending&&
           a.plan.summarized==b.plan.summarized&&a.plan.dormant==b.plan.dormant);
        CK(a.plan.detailed+a.plan.pending+a.plan.summarized+a.plan.dormant==5u);
        for(size_t i=0;i<5u;++i) {
            CK(decision(r,&a,r[i].id)==decision(reverse,&b,r[i].id));
            CK(evidence(r,&a,r[i].id)==evidence(reverse,&b,r[i].id));
        }
        CK(decision(r,&a,40u)==NF_RETC_H319_DORMANT);
        ++permutations;
    }
    NfRetcH319A5Frontier a={0};
    CK(nf_retc_h319_a5_frontier(g,2u,epoch,r,points,5u,8u,&a));
    CK(a.plan.spent==8u&&a.queried==4u);
    CK(a.plan.detailed==1u&&a.plan.pending==2u&&
       a.plan.summarized==1u&&a.plan.dormant==1u);
    CK(a.blocked==1u&&a.unresolved==2u);
    CK(decision(r,&a,10u)==NF_RETC_H319_DETAIL);
    CK(evidence(r,&a,10u)==NF_RETC_H319_A5_FREE);
    CK(decision(r,&a,20u)==NF_RETC_H319_PENDING);
    CK(evidence(r,&a,20u)==NF_RETC_H319_A5_UNRESOLVED);
    CK(decision(r,&a,30u)==NF_RETC_H319_SUMMARY);
    CK(evidence(r,&a,30u)==NF_RETC_H319_A5_BLOCKED);
    CK(decision(r,&a,50u)==NF_RETC_H319_PENDING);
    CK(evidence(r,&a,50u)==NF_RETC_H319_A5_UNRESOLVED);
    CK(evidence(r,&a,40u)==NF_RETC_H319_A5_NOT_QUERIED);
    const NfRetcH319A5Frontier snapshot=a;
    CK(!nf_retc_h319_a5_frontier(g,2u,epoch+1u,r,points,5u,8u,&a));
    CK(memcmp(&a,&snapshot,sizeof a)==0);
    NfVec3 bad[5];memcpy(bad,points,sizeof points);
    bad[0].x=NAN;
    CK(!nf_retc_h319_a5_frontier(g,2u,epoch,r,bad,5u,8u,&a));
    CK(memcmp(&a,&snapshot,sizeof a)==0);
    NfRetcH319Region duplicate[5];memcpy(duplicate,r,sizeof r);
    duplicate[1].id=duplicate[0].id;
    CK(!nf_retc_h319_a5_frontier(g,2u,epoch,duplicate,points,5u,8u,&a));
    CK(memcmp(&a,&snapshot,sizeof a)==0);
    memset(voxels,NF18A5_FREE,sizeof voxels);
    CK(nf18a5_load_canonical(g,1,0,0,voxels,3u));
    CK(!nf_retc_h319_a5_frontier(g,4u,epoch,r,points,5u,8u,&a));
    CK(memcmp(&a,&snapshot,sizeof a)==0);
    CK(nf_retc_h319_a5_frontier(g,4u,g->global_revision,r,points,5u,8u,&a));
    CK(a.plan.detailed==2u&&a.plan.pending==1u);
    CK(evidence(r,&a,20u)==NF_RETC_H319_A5_FREE);
    printf("H319 A5_FRONTIER_COMPLETE budgets=%u material_chunks=3 regions=5 detail_after_refine=2 pending_after_refine=1 source_epoch_bound=1 reversed_order=1 no_publish=1\n",permutations);
    free(g);return 0;
}
