#include "nf_retc_h319_simulation_frontier.h"
#include <stdio.h>
#include <string.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"H319 FRONTIER failure at %d: %s\n",__LINE__,#x);return 1;}}while(0)
static NfRetcH319Detail mode_for(const NfRetcH319Region *r,
                                 NfRetcH319Frontier f,uint32_t id){
   for(size_t i=0;i<f.count;++i)if(r[i].id==id)return f.decisions[i];
   return NF_RETC_H319_PENDING;
}
int main(void) {
   const NfRetcH319Region source[6]={
     {.id=10,.work_cost=4,.consequential=1,.observed=1,.active=1,.resident=1},
     {.id=20,.work_cost=2,.consequential=1,.resident=0},
     {.id=30,.work_cost=3,.observed=1,.active=1,.resident=1},
     {.id=40,.work_cost=5,.observed=1,.resident=1},
     {.id=50,.work_cost=1,.resident=1},
     {.id=60,.work_cost=8,.consequential=1,.resident=1,.distance_bucket=15}
   };
   NfRetcH319Region reversed[6];
   for(size_t i=0;i<6u;++i)reversed[i]=source[5u-i];
   unsigned permutations=0;
   for(uint32_t budget=0;budget<=16;budget++){
      NfRetcH319Frontier a={0},b={0};
      CK(nf_retc_h319_frontier_select(source,6u,budget,&a));
      CK(nf_retc_h319_frontier_select(reversed,6u,budget,&b));
      CK(a.spent<=budget&&b.spent<=budget&&a.spent==b.spent);
      CK(a.detailed==b.detailed&&a.pending==b.pending&&
         a.summarized==b.summarized&&a.dormant==b.dormant);
      CK(a.detailed+a.pending+a.summarized+a.dormant==6u);
      for(size_t i=0;i<6u;++i)
         CK(mode_for(source,a,source[i].id)==mode_for(reversed,b,source[i].id));
      CK(mode_for(source,a,20u)==NF_RETC_H319_PENDING);
      CK(mode_for(source,a,50u)==NF_RETC_H319_DORMANT);
      ++permutations;
   }
   NfRetcH319Frontier seven={0};
   CK(nf_retc_h319_frontier_select(source,6u,7u,&seven));
   CK(seven.spent==7u&&seven.detailed==2u&&
      seven.pending==2u&&seven.summarized==1u&&seven.dormant==1u);
   CK(mode_for(source,seven,10u)==NF_RETC_H319_DETAIL);
   CK(mode_for(source,seven,30u)==NF_RETC_H319_DETAIL);
   CK(mode_for(source,seven,60u)==NF_RETC_H319_PENDING);
   CK(mode_for(source,seven,40u)==NF_RETC_H319_SUMMARY);
   const NfRetcH319Frontier before=seven;
   NfRetcH319Region invalid[6];
   memcpy(invalid,source,sizeof source);
   invalid[1].id=invalid[0].id;
   CK(!nf_retc_h319_frontier_select(invalid,6u,7u,&seven));
   CK(memcmp(&seven,&before,sizeof before)==0);
   memcpy(invalid,source,sizeof source);invalid[2].resident=2u;
   CK(!nf_retc_h319_frontier_select(invalid,6u,7u,&seven));
   CK(memcmp(&seven,&before,sizeof before)==0);
   CK(!nf_retc_h319_frontier_select(source,17u,7u,&seven));
   CK(memcmp(&seven,&before,sizeof before)==0);
   printf("H319 FRONTIER_COMPLETE budget_sweeps=%u source_regions=6 detail_at_7=2 pending_at_7=2 summary_at_7=1 dormant_at_7=1 permutation_invariant=1 no_mutation=1 authority=0\n",permutations);
   return 0;
}
