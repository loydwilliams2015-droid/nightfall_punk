#include "nf_cattler_recurrence.h"

#include <float.h>
#include <math.h>
#include <string.h>

static uint32_t hash_u32(uint32_t value) {
    value ^= value >> 16;
    value *= 0x7feb352du;
    value ^= value >> 15;
    value *= 0x846ca68bu;
    value ^= value >> 16;
    return value;
}

static float distance_xz(NfVec3 a, NfVec3 b) {
    const float dx=a.x-b.x;
    const float dz=a.z-b.z;
    return sqrtf(dx*dx+dz*dz);
}

static int agent_index(const NfCattlerSystem *cattlers,NfEntityId actor_id) {
    if(cattlers==NULL)return -1;
    for(size_t i=0u;i<cattlers->count;++i) {
        if(cattlers->agents[i].actor_id==actor_id)return(int)i;
    }
    return -1;
}

size_t nf_cattler_living_count(
    const NfCattlerSystem *cattlers,const NfWorld *world) {
    if(cattlers==NULL||world==NULL)return 0u;
    size_t count=0u;
    for(size_t i=0u;i<cattlers->count;++i) {
        const NfActor *body=nf_world_find_actor_const(world,cattlers->agents[i].actor_id);
        if(body!=NULL&&body->combat.alive)++count;
    }
    return count;
}

void nf_cattler_recurrence_init(
    NfCattlerRecurrenceSystem *recurrence,uint32_t seed) {
    if(recurrence==NULL)return;
    memset(recurrence,0,sizeof(*recurrence));
    recurrence->seed=seed;
    for(size_t i=0u;i<NF_CATTLER_MAX_AGENTS;++i) {
        recurrence->agents[i].death_region=NF_REGION_INVALID;
    }
}

static bool player_observes_region(
    const NfWorld *world,const NfRegionGraph *graph,uint8_t region) {
    if(world==NULL||graph==NULL||region>=graph->count)return false;
    const NfVec3 center=graph->regions[region].center;
    for(size_t i=0u;i<NF_MAX_ENTITIES;++i) {
        const NfActor *actor=&world->actors[i];
        if(!actor->active||!actor->combat.alive)continue;
        if(actor->faction!=NF_FACTION_PLAYER&&actor->faction!=NF_FACTION_TEAMMATE)continue;
        if(distance_xz(actor->transform.position,center)<=NF_CATTLER_PLAYER_OBSERVE_RANGE)return true;
    }
    return false;
}

void nf_cattler_recurrence_tick(
    NfCattlerRecurrenceSystem *recurrence,
    const NfCattlerSystem *cattlers,
    const NfWorld *world) {
    if(recurrence==NULL||cattlers==NULL||world==NULL)return;

    for(uint8_t region=0u;region<cattlers->graph.count;++region) {
        if(player_observes_region(world,&cattlers->graph,region)) {
            recurrence->last_player_seen[region]=world->tick;
        }
    }

    const size_t living=nf_cattler_living_count(cattlers,world);
    if(living==0u) {
        if(!recurrence->global_zero_active) {
            recurrence->global_zero_active=true;
            recurrence->global_zero_since_tick=world->tick;
        }
    } else {
        recurrence->global_zero_active=false;
        recurrence->global_zero_since_tick=0u;
    }

    for(size_t i=0u;i<cattlers->count;++i) {
        const NfCattlerAgent *agent=&cattlers->agents[i];
        const NfActor *body=nf_world_find_actor_const(world,agent->actor_id);
        NfCattlerRecurrenceAgent *state=&recurrence->agents[i];
        if(body==NULL)continue;
        if(body->combat.alive) {
            state->dead_seen=false;
            state->death_region=NF_REGION_INVALID;
            state->dead_since_tick=0u;
            continue;
        }
        if(!state->dead_seen) {
            state->dead_seen=true;
            state->dead_since_tick=world->tick;
            state->death_region=nf_region_nearest(&cattlers->graph,body->transform.position);
        }
    }
}

static uint64_t recolonize_delay(
    const NfCattlerRecurrenceSystem *recurrence,NfEntityId actor_id) {
    const uint32_t roll=hash_u32(recurrence->seed^actor_id^0xC0110u);
    return NF_CATTLER_RECOLONIZE_BASE_TICKS+
        (uint64_t)(roll%NF_CATTLER_RECOLONIZE_VARIANCE_TICKS);
}

static uint64_t reseed_delay(
    const NfCattlerRecurrenceSystem *recurrence,uint8_t region) {
    const uint32_t roll=hash_u32(recurrence->seed^((uint32_t)region<<12)^0x5EEDu);
    return NF_CATTLER_RESEED_BASE_TICKS+
        (uint64_t)(roll%NF_CATTLER_RESEED_VARIANCE_TICKS);
}

static bool player_clear_for(
    const NfCattlerRecurrenceSystem *recurrence,
    const NfCattlerSystem *cattlers,
    const NfWorld *world,
    uint8_t region,
    uint64_t ticks) {
    if(region>=cattlers->graph.count||player_observes_region(world,&cattlers->graph,region))return false;
    const uint64_t seen=recurrence->last_player_seen[region];
    return world->tick>=seen&&world->tick-seen>=ticks;
}

static NfVec3 region_spawn(
    const NfCattlerRecurrenceSystem *recurrence,
    const NfRegionGraph *graph,
    uint8_t region,
    NfEntityId actor_id) {
    NfVec3 out=graph->regions[region].center;
    const uint32_t variation=hash_u32(recurrence->seed^actor_id^((uint32_t)region<<16));
    out.x+=(float)((int)(variation%9u)-4)*1.15f;
    out.z+=(float)((int)((variation>>8)%9u)-4)*1.15f;
    out.y=0.05f;
    return out;
}

static int nearest_living_steps(
    const NfCattlerSystem *cattlers,const NfWorld *world,uint8_t candidate) {
    int best=9999;
    for(size_t i=0u;i<cattlers->count;++i) {
        const NfActor *body=nf_world_find_actor_const(world,cattlers->agents[i].actor_id);
        if(body==NULL||!body->combat.alive)continue;
        const uint8_t source=nf_region_nearest(&cattlers->graph,body->transform.position);
        const int steps=nf_region_path_steps(&cattlers->graph,source,candidate);
        if(steps>=0&&steps<best)best=steps;
    }
    return best==9999?-1:best;
}

static bool normal_recolonization_candidate(
    const NfCattlerRecurrenceSystem *recurrence,
    const NfCattlerSystem *cattlers,
    const NfWorld *world,
    uint8_t death_region,
    uint8_t *region_out) {
    float best=-FLT_MAX;
    uint8_t best_region=NF_REGION_INVALID;
    for(uint8_t region=0u;region<cattlers->graph.count;++region) {
        if(region==death_region)continue;
        const int source_steps=nearest_living_steps(cattlers,world,region);
        if(source_steps<0||source_steps>2)continue;
        if(!player_clear_for(recurrence,cattlers,world,region,NF_TICK_RATE*2u))continue;
        const NfCattlerRegionState *state=&cattlers->regions[region];
        const float continuity=state->infestation;
        const float resource=state->resource_value;
        const float quiet=1.0f-state->disturbance;
        const float source_fit=1.0f-(float)source_steps/3.0f;
        const float score=continuity*0.36f+resource*0.32f+quiet*0.12f+source_fit*0.20f;
        if(score>best){best=score;best_region=region;}
    }
    if(best_region==NF_REGION_INVALID)return false;
    *region_out=best_region;
    return true;
}

static bool extinction_reseed_candidate(
    const NfCattlerRecurrenceSystem *recurrence,
    const NfCattlerSystem *cattlers,
    const NfWorld *world,
    uint8_t *region_out) {
    if(!recurrence->global_zero_active)return false;
    float best=-FLT_MAX;
    uint8_t best_region=NF_REGION_INVALID;
    for(uint8_t region=0u;region<cattlers->graph.count;++region) {
        const uint64_t required=reseed_delay(recurrence,region);
        if(world->tick<recurrence->global_zero_since_tick||
           world->tick-recurrence->global_zero_since_tick<required)continue;
        if(!player_clear_for(recurrence,cattlers,world,region,required))continue;
        const NfCattlerRegionState *state=&cattlers->regions[region];
        if(state->resource_value<0.28f&&state->infestation<0.10f)continue;
        const float score=state->resource_value*0.48f+state->infestation*0.34f+
            (1.0f-state->disturbance)*0.18f;
        if(score>best){best=score;best_region=region;}
    }
    if(best_region==NF_REGION_INVALID)return false;
    *region_out=best_region;
    return true;
}

bool nf_cattler_recurrence_spawn_for(
    const NfCattlerRecurrenceSystem *recurrence,
    const NfCattlerSystem *cattlers,
    const NfWorld *world,
    NfEntityId actor_id,
    NfVec3 *out,
    NfCattlerReturnKind *kind_out,
    uint8_t *region_out) {
    if(recurrence==NULL||cattlers==NULL||world==NULL||out==NULL)return false;
    const int index=agent_index(cattlers,actor_id);
    if(index<0)return false;
    const NfCattlerRecurrenceAgent *state=&recurrence->agents[index];
    if(!state->dead_seen)return false;

    uint8_t region=NF_REGION_INVALID;
    NfCattlerReturnKind kind=NF_CATTLER_RETURN_NONE;
    const size_t living=nf_cattler_living_count(cattlers,world);
    if(living>0u) {
        const uint64_t delay=recolonize_delay(recurrence,actor_id);
        if(world->tick<state->dead_since_tick||world->tick-state->dead_since_tick<delay)return false;
        if(!normal_recolonization_candidate(recurrence,cattlers,world,state->death_region,&region))return false;
        kind=NF_CATTLER_RETURN_RECOLONIZE;
    } else {
        if(!extinction_reseed_candidate(recurrence,cattlers,world,&region))return false;
        kind=NF_CATTLER_RETURN_EXTINCTION_RESEED;
    }

    *out=region_spawn(recurrence,&cattlers->graph,region,actor_id);
    if(kind_out!=NULL)*kind_out=kind;
    if(region_out!=NULL)*region_out=region;
    return true;
}

void nf_cattler_recurrence_commit(
    NfCattlerRecurrenceSystem *recurrence,
    NfCattlerSystem *cattlers,
    NfEntityId actor_id,
    NfVec3 position,
    uint8_t region) {
    if(recurrence==NULL||cattlers==NULL||region>=cattlers->graph.count)return;
    const int index=agent_index(cattlers,actor_id);
    if(index<0)return;
    NfCattlerAgent *agent=&cattlers->agents[index];
    agent->spawn=position;
    agent->home_region=region;
    agent->current_region=region;
    agent->target_region=region;
    agent->next_region=region;
    agent->claimed_watch_region=NF_REGION_INVALID;
    recurrence->agents[index].dead_seen=false;
    recurrence->agents[index].death_region=NF_REGION_INVALID;
    recurrence->agents[index].dead_since_tick=0u;
    recurrence->global_zero_active=false;
    recurrence->global_zero_since_tick=0u;
}

const char *nf_cattler_return_name(NfCattlerReturnKind kind) {
    switch(kind) {
        case NF_CATTLER_RETURN_RECOLONIZE:return "RECOLONIZE";
        case NF_CATTLER_RETURN_EXTINCTION_RESEED:return "EXTINCTION_RESEED";
        default:return "NONE";
    }
}
