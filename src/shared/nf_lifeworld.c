#include "nf_lifeworld.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define NF_LIFEWORLD_UPDATE_TICKS 6u

static uint16_t clamp_q(int value){return(uint16_t)(value<0?0:(value>1000?1000:value));}
static float clamp01f(float value){return value<0.0f?0.0f:(value>1.0f?1.0f:value);}
static double clamp01d(double value){return value<0.0?0.0:(value>1.0?1.0:value);}
static uint32_t hash_u32(uint32_t value){value^=value>>16;value*=0x7feb352du;value^=value>>15;value*=0x846ca68bu;value^=value>>16;return value;}
static float dist_xz(NfVec3 a,NfVec3 b){float x=a.x-b.x,z=a.z-b.z;return sqrtf(x*x+z*z);}

bool nf_lifeworld_contains(const NfLifeworldSystem *system,NfVec3 position){
    if(system==NULL)return false;
    return fabsf(position.x-system->center.x)<=system->half_x&&
        fabsf(position.z-system->center.z)<=system->half_z;
}

static void add_hotspot(
    NfLifeworldSystem *system,NfEpistemicHotspotType type,
    uint16_t uncertainty,uint16_t consequence,uint16_t evidence_change,
    uint64_t tick){
    if(system==NULL||system->hotspot_count>=NF_LIFEWORLD_MAX_HOTSPOTS)return;
    NfEpistemicHotspot *h=&system->hotspots[system->hotspot_count++];
    *h=(NfEpistemicHotspot){
        .type=type,.position=system->center,.region=system->region,
        .uncertainty_q=uncertainty,.consequence_q=consequence,
        .evidence_change_q=evidence_change,.changed_tick=tick};
    if(type==NF_HOTSPOT_ACCESS_CHANGE){h->position=system->pump.position;}
}

static void update_processes(NfLifeworldSystem *system,bool contested){
    const int infestation=(int)system->memory.infestation_q;
    const int disturbance=(int)system->memory.disturbance_q;
    const int access=1000-(infestation*65)/100-(disturbance*20)/100;
    system->generator.access_q=clamp_q(access);
    system->pump.access_q=system->generator.access_q;

    if(contested&&disturbance>650){
        system->generator.condition_q=clamp_q((int)system->generator.condition_q-1);
        system->pump.condition_q=clamp_q((int)system->pump.condition_q-1);
    }else if(system->cattlers_present==0u&&system->humans_present>0u){
        const int repair=system->memory.maintenance_q>400u?2:1;
        system->generator.condition_q=clamp_q((int)system->generator.condition_q+repair);
        system->pump.condition_q=clamp_q((int)system->pump.condition_q+repair);
    }

    const uint32_t generator=(uint32_t)system->generator.condition_q*(uint32_t)system->generator.access_q/1000u;
    system->generator.throughput_q=(uint16_t)generator;
    const uint32_t pump_condition=(uint32_t)system->pump.condition_q*(uint32_t)system->pump.access_q/1000u;
    const uint32_t pump=(pump_condition*(uint32_t)system->generator.throughput_q)/1000u;
    system->pump.throughput_q=(uint16_t)pump;
    system->generator.flags=system->generator.throughput_q>120u?1u:0u;
    system->pump.flags=system->pump.throughput_q>100u?1u:0u;
}

static int attention_score(
    const NfActor *actor,const NfEpistemicHotspot *hotspot,
    unsigned already_covered){
    if(actor==NULL||hotspot==NULL)return -100000;
    const float distance=dist_xz(actor->transform.position,hotspot->position);
    const int proximity=(int)(clamp01f(1.0f-distance/120.0f)*300.0f);
    const int coverage_penalty=(int)already_covered*220;
    return(int)hotspot->consequence_q+(int)hotspot->uncertainty_q/2+
        (int)hotspot->evidence_change_q/2+proximity-coverage_penalty;
}

static NfLifeActivity activity_for(
    NfEpistemicHotspotType hotspot,float distance){
    if(distance>18.0f)return NF_LIFE_TRAVEL;
    if(hotspot==NF_HOTSPOT_ACCESS_CHANGE)return NF_LIFE_WORK;
    if(hotspot==NF_HOTSPOT_RECOVERY_EDGE)return NF_LIFE_MAINTAIN;
    return NF_LIFE_OCCUPY;
}

static void choose_attention(
    NfLifeworldSystem *system,const NfWorld *world){
    system->attention_count=0u;
    if(system->hotspot_count==0u)return;
    unsigned covered[NF_LIFEWORLD_MAX_HOTSPOTS]={0};
    for(size_t i=0u;i<NF_MAX_ENTITIES&&system->attention_count<NF_LIFEWORLD_MAX_ATTENTION;++i){
        const NfActor *actor=&world->actors[i];
        if(!actor->active||!actor->combat.alive||actor->faction!=NF_FACTION_RIVAL)continue;
        int best=-100000,second=-100000;size_t best_i=0u,second_i=0u;
        for(size_t h=0u;h<system->hotspot_count;++h){
            const int score=attention_score(actor,&system->hotspots[h],covered[h]);
            if(score>best){second=best;second_i=best_i;best=score;best_i=h;}
            else if(score>second){second=score;second_i=h;}
        }
        bool explore=false;size_t chosen=best_i;
        if(system->hotspot_count>1u&&best-second<=110){
            const uint32_t roll=hash_u32(system->seed^actor->id^(uint32_t)(world->tick/NF_LIFEWORLD_UPDATE_TICKS));
            if((roll&1u)!=0u){chosen=second_i;explore=true;}
        }
        const NfEpistemicHotspot *hotspot=&system->hotspots[chosen];
        const float distance=dist_xz(actor->transform.position,hotspot->position);
        system->attention[system->attention_count++]=(NfAttentionChoice){
            .actor_id=actor->id,.hotspot=hotspot->type,.region=hotspot->region,
            .activity=activity_for(hotspot->type,distance),.exploration=explore};
        ++covered[chosen];
    }
}

void nf_lifeworld_init(
    NfLifeworldSystem *system,const NfRegionGraph *graph,uint32_t seed){
    if(system==NULL)return;memset(system,0,sizeof(*system));
    system->center=(NfVec3){NF_LIFEWORLD_DEMO_CENTER_X,0.05f,NF_LIFEWORLD_DEMO_CENTER_Z};
    system->half_x=NF_LIFEWORLD_DEMO_HALF_X;system->half_z=NF_LIFEWORLD_DEMO_HALF_Z;system->seed=seed;
    system->region=graph!=NULL?nf_region_nearest(graph,system->center):NF_REGION_INVALID;
    system->memory=(NfLandbaseMemory){.human_use_q=80u,.maintenance_q=140u};
    system->generator=(NfProcessObject){.id=501u,.type=NF_PROCESS_GENERATOR,.position={-54.0f,0.05f,-16.0f},.condition_q=860u,.throughput_q=860u,.access_q=1000u,.flags=1u};
    system->pump=(NfProcessObject){.id=502u,.type=NF_PROCESS_PUMP,.position={-38.0f,0.05f,-4.0f},.condition_q=780u,.throughput_q=671u,.access_q=1000u,.flags=1u};
    system->previous_pump_throughput_q=system->pump.throughput_q;
}

void nf_lifeworld_tick(
    NfLifeworldSystem *system,const NfWorld *world,const NfRegionGraph *graph){
    if(system==NULL||world==NULL)return;
    if(world->tick<system->next_update_tick)return;
    system->next_update_tick=world->tick+NF_LIFEWORLD_UPDATE_TICKS;
    if(graph!=NULL)system->region=nf_region_nearest(graph,system->center);

    system->cattlers_present=0u;system->humans_present=0u;
    for(size_t i=0u;i<NF_MAX_ENTITIES;++i){
        const NfActor *actor=&world->actors[i];
        if(!actor->active||!actor->combat.alive||!nf_lifeworld_contains(system,actor->transform.position))continue;
        if(actor->faction==NF_FACTION_RANCHER){if(system->cattlers_present<255u)++system->cattlers_present;system->memory.last_cattler_tick=world->tick;}
        else if(actor->faction==NF_FACTION_RIVAL||actor->faction==NF_FACTION_PLAYER||actor->faction==NF_FACTION_TEAMMATE){if(system->humans_present<255u)++system->humans_present;system->memory.last_human_tick=world->tick;}
    }
    const bool contested=system->cattlers_present>0u&&system->humans_present>0u;

    if(system->cattlers_present>0u){
        const int gain=system->cattlers_present>=2u?8:4;
        system->memory.infestation_q=clamp_q((int)system->memory.infestation_q+gain);
    }else{
        int loss=1;if(system->memory.maintenance_q>500u)loss=2;
        system->memory.infestation_q=clamp_q((int)system->memory.infestation_q-loss);
    }
    if(system->humans_present>0u)system->memory.human_use_q=clamp_q((int)system->memory.human_use_q+2+(int)system->humans_present);
    else system->memory.human_use_q=clamp_q((int)system->memory.human_use_q-1);

    if(system->humans_present>0u&&system->cattlers_present==0u)system->memory.maintenance_q=clamp_q((int)system->memory.maintenance_q+3);
    else system->memory.maintenance_q=clamp_q((int)system->memory.maintenance_q-1);

    if(contested)system->memory.disturbance_q=clamp_q((int)system->memory.disturbance_q+10);
    else if(system->cattlers_present>0u)system->memory.disturbance_q=clamp_q((int)system->memory.disturbance_q+3);
    else if(system->humans_present>0u)system->memory.disturbance_q=clamp_q((int)system->memory.disturbance_q-3);
    else system->memory.disturbance_q=clamp_q((int)system->memory.disturbance_q-1);

    system->previous_pump_throughput_q=system->pump.throughput_q;
    update_processes(system,contested);
    const int throughput_change=abs((int)system->pump.throughput_q-(int)system->previous_pump_throughput_q);

    system->hotspot_count=0u;
    if(system->cattlers_present>=2u||contested){
        add_hotspot(system,NF_HOTSPOT_CONTESTED_OCCUPATION,
            contested?420u:620u,900u,
            clamp_q((int)system->memory.disturbance_q),world->tick);
    }
    if(throughput_change>=4||system->pump.access_q<700u){
        add_hotspot(system,NF_HOTSPOT_ACCESS_CHANGE,
            360u,720u,clamp_q(throughput_change*18),world->tick);
    }
    if(system->cattlers_present==0u&&system->memory.infestation_q>100u&&system->humans_present>0u){
        add_hotspot(system,NF_HOTSPOT_RECOVERY_EDGE,
            520u,650u,clamp_q((int)system->memory.infestation_q),world->tick);
    }
    choose_attention(system,world);
}

NfLifeworldSurface nf_lifeworld_predict_surface(const NfLifeworldSystem *system){
    if(system==NULL)return(NfLifeworldSurface){0};
    const float infest=(float)system->memory.infestation_q/1000.0f;
    const float human=(float)system->memory.human_use_q/1000.0f;
    const float maintenance=(float)system->memory.maintenance_q/1000.0f;
    const float disturbance=(float)system->memory.disturbance_q/1000.0f;
    return(NfLifeworldSurface){
        .darkness=clamp01f(infest*0.82f+disturbance*0.18f),
        .traffic=clamp01f(human*(1.0f-infest*0.55f)),
        .infrastructure_activity=(float)system->pump.throughput_q/1000.0f,
        .disturbance=disturbance,
        .recovery=clamp01f(maintenance*(1.0f-infest))};
}

double nf_lifeworld_situated_value(
    const NfActor *actor,const NfEnergyOpportunity *op,
    const NfRegionGraph *graph,bool cattler_form){
    if(actor==NULL||op==NULL||graph==NULL||op->region>=graph->count)return -1.0e30;
    const uint8_t origin=nf_region_nearest(graph,actor->transform.position);
    const int steps=origin<graph->count?nf_region_path_steps(graph,origin,op->region):-1;
    if(steps<0)return -1.0e30;
    const double access=(double)steps*1.10;
    const double exposure=(double)graph->regions[op->region].structural_exposure;
    if(cattler_form){
        const double habitat_fit=exposure*3.0;
        return op->score-access*0.65-exposure*0.55+habitat_fit;
    }
    const double social_friction=actor->faction==NF_FACTION_RIVAL?0.35:0.20;
    return op->score-access-exposure*2.20-social_friction;
}

size_t nf_lifeworld_write_primary_json(
    const NfLifeworldSystem *system,uint32_t world_seed,uint64_t tick,
    uint32_t round,char *out,size_t cap){
    if(system==NULL||out==NULL||cap==0u)return 0u;
    const int n=snprintf(out,cap,
        "{\"v\":%u,\"seed\":%u,\"tick\":%llu,\"round\":%u,"
        "\"land\":{\"region\":%u,\"memory\":{\"infest_q\":%u,\"human_use_q\":%u,"
        "\"maint_q\":%u,\"disturb_q\":%u,\"last_cattler\":%llu,\"last_human\":%llu},"
        "\"objects\":[{\"id\":%u,\"type\":%u,\"condition_q\":%u,\"flags\":%u},"
        "{\"id\":%u,\"type\":%u,\"condition_q\":%u,\"flags\":%u}]}}",
        NF_LIFEWORLD_PRIMARY_SCHEMA,world_seed,(unsigned long long)tick,round,
        system->region,system->memory.infestation_q,system->memory.human_use_q,
        system->memory.maintenance_q,system->memory.disturbance_q,
        (unsigned long long)system->memory.last_cattler_tick,
        (unsigned long long)system->memory.last_human_tick,
        system->generator.id,(unsigned)system->generator.type,system->generator.condition_q,system->generator.flags,
        system->pump.id,(unsigned)system->pump.type,system->pump.condition_q,system->pump.flags);
    if(n<0||(size_t)n>=cap){out[0]='\0';return 0u;}return(size_t)n;
}

const char *nf_life_activity_name(NfLifeActivity activity){
    switch(activity){case NF_LIFE_TRAVEL:return"TRAVEL";case NF_LIFE_WORK:return"WORK";case NF_LIFE_MAINTAIN:return"MAINTAIN";case NF_LIFE_OCCUPY:return"OCCUPY";default:return"UNKNOWN";}}
const char *nf_hotspot_name(NfEpistemicHotspotType hotspot){
    switch(hotspot){case NF_HOTSPOT_NONE:return"NONE";case NF_HOTSPOT_CONTESTED_OCCUPATION:return"CONTESTED_OCCUPATION";case NF_HOTSPOT_ACCESS_CHANGE:return"ACCESS_CHANGE";case NF_HOTSPOT_RECOVERY_EDGE:return"RECOVERY_EDGE";default:return"UNKNOWN";}}
