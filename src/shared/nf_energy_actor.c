#include "nf_energy_actor.h"

#include <float.h>
#include <math.h>
#include <string.h>

static double clamp01(double value){return value<0.0?0.0:(value>1.0?1.0:value);}

static void insert_candidate(
    NfEnergyOpportunity *out,size_t cap,size_t *count,
    NfEnergyOpportunity candidate){
    if(out==NULL||count==NULL||cap==0u)return;
    size_t used=*count<cap?*count:cap;
    size_t slot=used;
    for(size_t i=0u;i<used;++i){
        if(candidate.score>out[i].score){slot=i;break;}
    }
    if(slot>=cap)return;
    if(used<cap)++used;
    for(size_t i=used-1u;i>slot;--i)out[i]=out[i-1u];
    out[slot]=candidate;
    *count=used;
}

void nf_actor_energy_belief_init(
    NfEnergyBelief *belief,const NfEnergySystem *system){
    if(belief==NULL)return;
    memset(belief,0,sizeof(*belief));
    if(system==NULL||!system->bound)return;
    for(uint8_t region=0u;region<NF_ENERGY_REGION_MAX;++region){
        /* prior_yield is seeded/static world knowledge, not current stock. */
        belief->mean[region]=system->regions[region].prior_yield*100.0;
        belief->confidence[region]=0.10;
    }
}

void nf_actor_energy_belief_observe(
    NfEnergyBelief *belief,uint8_t region,double estimated_regional_return,
    double evidence_confidence,uint64_t tick){
    if(belief==NULL||region>=NF_ENERGY_REGION_MAX)return;
    const double c=clamp01(evidence_confidence);
    const double prior_precision=1.0+belief->confidence[region]*5.0;
    const double evidence_precision=1.0+c*9.0;
    belief->mean[region]=
        (belief->mean[region]*prior_precision+
         estimated_regional_return*evidence_precision)/
        (prior_precision+evidence_precision);
    belief->confidence[region]=clamp01(
        belief->confidence[region]+(1.0-belief->confidence[region])*c*0.72);
    belief->observed_tick[region]=tick;
}

void nf_actor_energy_belief_decay(
    NfEnergyBelief *belief,uint64_t now_tick){
    if(belief==NULL)return;
    for(uint8_t region=0u;region<NF_ENERGY_REGION_MAX;++region){
        if(belief->observed_tick[region]==0u||now_tick<=belief->observed_tick[region])continue;
        const uint64_t age=now_tick-belief->observed_tick[region];
        if(age<=60u*5u)continue;
        const double seconds=(double)(age-60u*5u)/60.0;
        belief->confidence[region]=clamp01(
            belief->confidence[region]*exp(-seconds*0.006));
    }
}

static NfEnergyOpportunity belief_candidate(
    const NfEnergyBelief *belief,const NfEnergySystem *system,
    const NfRegionGraph *graph,uint8_t origin,uint8_t region,
    NfEnergyHorizon horizon){
    NfEnergyOpportunity candidate={0};
    candidate.region=region;
    candidate.posterior_mean=belief->mean[region];
    candidate.uncertainty=1.0-clamp01(belief->confidence[region]);
    const double mean=belief->mean[region];
    candidate.immediate_return=mean*0.72;
    candidate.round_return=mean*0.88;
    candidate.regional_return=mean;
    candidate.regenerative_return=mean*1.12;
    candidate.topographic_cost=nf_energy_topographic_cost(
        system,graph,origin,region);
    double expected=candidate.regional_return;
    if(horizon==NF_ENERGY_HORIZON_IMMEDIATE)expected=candidate.immediate_return;
    else if(horizon==NF_ENERGY_HORIZON_ROUND)expected=candidate.round_return;
    else if(horizon==NF_ENERGY_HORIZON_REGENERATIVE)expected=candidate.regenerative_return;
    candidate.score=expected-candidate.topographic_cost;
    return candidate;
}

size_t nf_actor_energy_belief_search(
    const NfEnergyBelief *belief,const NfEnergySystem *system,
    const NfRegionGraph *graph,uint8_t origin_region,NfEnergyHorizon horizon,
    NfEnergyOpportunity *out,size_t cap){
    if(belief==NULL||system==NULL||graph==NULL||out==NULL||cap==0u||
       !system->bound||origin_region>=graph->count)return 0u;
    size_t count=0u;
    for(uint8_t region=0u;region<NF_ENERGY_REGION_MAX;++region){
        if(region>=graph->count)break;
        const NfEnergyOpportunity candidate=belief_candidate(
            belief,system,graph,origin_region,region,horizon);
        if(candidate.score<=-DBL_MAX/4.0)continue;
        insert_candidate(out,cap,&count,candidate);
    }
    return count;
}
