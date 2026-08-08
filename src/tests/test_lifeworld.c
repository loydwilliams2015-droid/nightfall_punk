#include "nf_energy_actor.h"
#include "nf_lifeworld.h"
#include "nf_region.h"
#include "nf_world.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static bool nearly_equal(double a,double b,double epsilon){return fabs(a-b)<=epsilon;}

static bool has_hotspot(const NfLifeworldSystem *system,NfEpistemicHotspotType type){
    for(size_t i=0u;i<system->hotspot_count;++i){
        if(system->hotspots[i].type==type)return true;
    }
    return false;
}

static void step_lifeworld(
    NfWorld *world,NfLifeworldSystem *life,const NfRegionGraph *graph,
    unsigned ticks){
    for(unsigned i=0u;i<ticks;++i){
        nf_lifeworld_tick(life,world,graph);
        nf_world_step(world,1.0f/(float)NF_TICK_RATE);
    }
    nf_lifeworld_tick(life,world,graph);
}

static void actor_belief_never_tracks_hidden_truth(void){
    NfWorld world;
    nf_world_init(&world,20260808u);
    nf_world_build_movement_lab(&world);
    NfRegionGraph graph;
    nf_region_graph_init_spatial_lab(&graph);

    NfEnergyBelief belief;
    nf_actor_energy_belief_init(&belief,&world.energy);
    NfEnergyOpportunity before[NF_ENERGY_TOP_CANDIDATES]={0};
    const size_t before_count=nf_actor_energy_belief_search(
        &belief,&world.energy,&graph,9u,NF_ENERGY_HORIZON_REGIONAL,
        before,NF_ENERGY_TOP_CANDIDATES);
    assert(before_count==NF_ENERGY_TOP_CANDIDATES);

    /* Change mutable authoritative truth without supplying evidence. */
    world.energy.regions[14u].usable=0.0;
    world.energy.regions[14u].potential=0.0;
    world.energy.regions[14u].health=0.05;
    world.energy.regions[2u].usable=5000.0;
    world.energy.regions[2u].potential=5000.0;

    NfEnergyOpportunity hidden_change[NF_ENERGY_TOP_CANDIDATES]={0};
    const size_t after_count=nf_actor_energy_belief_search(
        &belief,&world.energy,&graph,9u,NF_ENERGY_HORIZON_REGIONAL,
        hidden_change,NF_ENERGY_TOP_CANDIDATES);
    assert(after_count==before_count);
    for(size_t i=0u;i<before_count;++i){
        assert(hidden_change[i].region==before[i].region);
        assert(nearly_equal(hidden_change[i].score,before[i].score,1e-12));
        assert(nearly_equal(hidden_change[i].posterior_mean,before[i].posterior_mean,1e-12));
    }

    /* Legitimate evidence, not hidden truth, changes the actor estimate. */
    const double old_mean=belief.mean[14u];
    nf_actor_energy_belief_observe(&belief,14u,12.0,0.90,world.tick+1u);
    assert(belief.mean[14u]<old_mean);
    assert(belief.confidence[14u]>0.10);
}

static void contested_corner_and_recovery(void){
    NfWorld world;
    nf_world_init(&world,20260808u);
    nf_world_build_movement_lab(&world);
    NfRegionGraph graph;
    nf_region_graph_init_spatial_lab(&graph);
    NfLifeworldSystem life;
    nf_lifeworld_init(&life,&graph,world.seed^0x1F3u);

    const NfEntityId c1=nf_world_spawn_actor(
        &world,NF_FACTION_RANCHER,(NfVec3){-50.0f,0.05f,-12.0f});
    const NfEntityId c2=nf_world_spawn_actor(
        &world,NF_FACTION_RANCHER,(NfVec3){-43.0f,0.05f,-8.0f});
    const NfEntityId r1=nf_world_spawn_actor(
        &world,NF_FACTION_RIVAL,(NfVec3){-31.0f,0.05f,-12.0f});
    const NfEntityId r2=nf_world_spawn_actor(
        &world,NF_FACTION_RIVAL,(NfVec3){-34.0f,0.05f,-3.0f});
    assert(c1&&c2&&r1&&r2);

    const uint16_t pump_before=life.pump.throughput_q;
    step_lifeworld(&world,&life,&graph,NF_TICK_RATE*15u);
    assert(life.cattlers_present>=2u);
    assert(life.humans_present>=2u);
    assert(life.memory.infestation_q>700u);
    assert(life.memory.disturbance_q>700u);
    assert(life.pump.throughput_q<pump_before);
    assert(has_hotspot(&life,NF_HOTSPOT_CONTESTED_OCCUPATION));
    assert(life.attention_count>=1u);

    const NfLifeworldSurface occupied_surface=nf_lifeworld_predict_surface(&life);
    assert(occupied_surface.darkness>0.50f);

    const uint16_t infestation_at_displacement=life.memory.infestation_q;
    assert(nf_world_despawn_actor(&world,c1));
    assert(nf_world_despawn_actor(&world,c2));
    step_lifeworld(&world,&life,&graph,1u);
    assert(life.memory.infestation_q<infestation_at_displacement);
    assert(life.memory.infestation_q>0u);
    assert(has_hotspot(&life,NF_HOTSPOT_RECOVERY_EDGE));

    const uint16_t maintenance_at_displacement=life.memory.maintenance_q;
    step_lifeworld(&world,&life,&graph,NF_TICK_RATE*45u);
    assert(life.memory.infestation_q>0u);
    assert(life.memory.infestation_q<infestation_at_displacement);
    assert(life.memory.maintenance_q>maintenance_at_displacement);
    assert(life.pump.throughput_q>100u);

    char primary_a[512],primary_b[512];
    const size_t a=nf_lifeworld_write_primary_json(
        &life,world.seed,world.tick,2u,primary_a,sizeof(primary_a));
    const size_t b=nf_lifeworld_write_primary_json(
        &life,world.seed,world.tick,2u,primary_b,sizeof(primary_b));
    assert(a>0u&&a==b&&strcmp(primary_a,primary_b)==0);
    assert(strstr(primary_a,"\"infest_q\"")!=NULL);
    assert(strstr(primary_a,"darkness")==NULL);
    assert(strstr(primary_a,"traffic")==NULL);
    assert(strstr(primary_a,"hotspot")==NULL);

    const NfLifeworldSurface recovering=nf_lifeworld_predict_surface(&life);
    assert(recovering.darkness<occupied_surface.darkness);
    assert(recovering.recovery>0.0f);

    NfEnergyBelief belief;
    nf_actor_energy_belief_init(&belief,&world.energy);
    NfEnergyOpportunity opportunity[1]={0};
    assert(nf_actor_energy_belief_search(
        &belief,&world.energy,&graph,life.region,NF_ENERGY_HORIZON_REGIONAL,
        opportunity,1u)==1u);
    const NfActor *rival=nf_world_find_actor_const(&world,r1);
    assert(rival!=NULL);
    const double human_value=nf_lifeworld_situated_value(
        rival,&opportunity[0],&graph,false);
    const double cattler_value=nf_lifeworld_situated_value(
        rival,&opportunity[0],&graph,true);
    assert(!nearly_equal(human_value,cattler_value,1e-9));

    printf("[lifeworld] contested infestation=%u -> recovering=%u maintenance=%u pump=%u darkness=%.2f->%.2f\n",
        infestation_at_displacement,life.memory.infestation_q,
        life.memory.maintenance_q,life.pump.throughput_q,
        occupied_surface.darkness,recovering.darkness);
    printf("[primary] %s\n",primary_a);
}

int main(void){
    actor_belief_never_tracks_hidden_truth();
    contested_corner_and_recovery();
    puts("nightfall v1.0 topographic lifeworld tests: PASS");
    return 0;
}
