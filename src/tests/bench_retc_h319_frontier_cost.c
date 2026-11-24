/* O1 bounded frontier cost diagnostics (not a 60 Hz gameplay benchmark).
 * Deliberately exercises fixed-size 16-region scheduling without I/O in loop;
 * clock() records process CPU time, not wall time or game frame latency. */
#include "nf_retc_h319_simulation_frontier.h"
#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"O1 COST FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
    NfRetcH319Region regions[16]={0};
    for(size_t i=0;i<16u;++i) {
        regions[i].id=(uint32_t)(i+1u);
        regions[i].work_cost=1u+(uint32_t)(i%4u);
        regions[i].consequential=(uint8_t)(i%5u==0u);
        regions[i].observed=(uint8_t)(i%2u==0u);
        regions[i].active=(uint8_t)(i%3u==0u);
        regions[i].resident=(uint8_t)(i%7u!=0u);
        regions[i].distance_bucket=(uint8_t)i;
        regions[i].idle_ticks=(uint8_t)(15u-i);
    }
    enum {ITERATIONS=20000};
    uint64_t checksum=0u;
    unsigned pending_max=0u,work_max=0u;
    const clock_t start=clock();
    CK(start!=(clock_t)-1);
    for(unsigned i=0;i<ITERATIONS;++i){
        NfRetcH319Frontier plan={0};
        const uint32_t budget=i%33u;
        CK(nf_retc_h319_frontier_select(regions,16u,budget,&plan));
        CK(plan.spent<=budget);
        CK(plan.detailed+plan.pending+plan.summarized+plan.dormant==16u);
        if(plan.pending>pending_max)pending_max=plan.pending;
        if(plan.spent>work_max)work_max=plan.spent;
        checksum+=plan.spent+(uint64_t)plan.pending*17u+
                  (uint64_t)plan.detailed*37u+(uint64_t)plan.dormant*13u;
    }
    const clock_t finish=clock();
    CK(finish!=(clock_t)-1&&finish>=start);
    const double elapsed=(double)(finish-start)/(double)CLOCKS_PER_SEC;
    printf("H319 O1_COST_DIAGNOSTIC runs=%u max_regions=16 observed_max_work=%u max_pending=%u checksum=%" PRIu64 " cpu_ms=%.3f cpu_us_per_run=%.6f 60hz_certified=0 A6_publication=0\n",
        ITERATIONS,work_max,pending_max,checksum,elapsed*1e3,
        elapsed*1e6/(double)ITERATIONS);
    return 0;
}
