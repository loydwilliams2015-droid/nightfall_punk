#include "nf_v18c2_a6_stage.h"
#include <stdio.h>
#include <string.h>
int main(void){
 Nf18c2A6Stage output;NfWorld w={0};Nf18a6Runtime r={0};
 NfMulti18c1World island={0};NfMulti18c1Proposal p={0};
 /* Unbound/invalid snapshots and missing B2 grants may never create
    successful physical-contractual authority or publish world state. */
 if(nf18c2_a6_stage(NULL,&r,&island,&p,1,NULL,0,1.f/60.f,5,NF18B3_PUSH,true,&output)!=NF18C2_STAGE_STALE)return 1;
 if(nf18c2_a6_stage(&w,&r,&island,&p,1,NULL,0,1.f/60.f,5,NF18B3_PUSH,true,&output)!=NF18C2_STAGE_STALE)return 2;
 if(w.tick||island.tick||r.last_commit_tick)return 3;
 puts("A6-bound staging refuses unbound/idle inputs; no source mutation PASS");
 return 0;
}
