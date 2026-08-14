#include "nf_movement.h"
#include "nf_route.h"
#include "nf_teammate.h"

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>

static void settle_world(NfWorld *world, unsigned ticks) {
    for (unsigned i=0u;i<ticks;++i) nf_world_step(world,1.0f/(float)NF_TICK_RATE);
}

static NfActor *spawn_player(NfWorld *world, NfVec3 position) {
    NfEntityId id=nf_world_spawn_actor(world,NF_FACTION_PLAYER,position);
    NfActor *player=nf_world_find_actor(world,id);
    assert(player!=NULL);
    settle_world(world,2u);
    return player;
}

static void biography_counterfactual(void) {
    NfTeammateBiographyHotspot base[NF_TEAMMATE_BIOGRAPHY_HOTSPOTS]={
        {NF_TEAMMATE_BIO_FORMATIVE_COOPERATION,0.90f},
        {NF_TEAMMATE_BIO_MENTORSHIP_INSTITUTION,0.82f},
        {NF_TEAMMATE_BIO_CREW_ENTRY,0.88f}
    };
    NfTeammateBiographyHotspot changed[NF_TEAMMATE_BIOGRAPHY_HOTSPOTS]={
        {NF_TEAMMATE_BIO_COMPETITIVE_DISRUPTION,0.90f},
        {NF_TEAMMATE_BIO_MENTORSHIP_INSTITUTION,0.82f},
        {NF_TEAMMATE_BIO_CREW_ENTRY,0.88f}
    };
    NfTeammateProfile a={0},b={0},a_repeat={0};
    nf_teammate_profile_from_hotspots(&a,base);
    nf_teammate_profile_from_hotspots(&a_repeat,base);
    nf_teammate_profile_from_hotspots(&b,changed);
    assert(fabsf(a.competition_avoidance-a_repeat.competition_avoidance)<0.0001f);
    assert(fabsf(a.reciprocity-a_repeat.reciprocity)<0.0001f);
    assert(a.competition_avoidance>0.55f&&a.reciprocity>0.55f&&a.independence>0.50f);
    assert(b.competition_avoidance<a.competition_avoidance);
    assert(b.independence>a.independence);
}

static void route_and_evidence_contract(void) {
    NfWorld world;nf_world_init(&world,20260813u);nf_world_build_movement_lab(&world);
    NfRouteSystem route;nf_route_init_v12_slice(&route,&world);assert(route.configured&&!route.open);
    NfActor *player=spawn_player(&world,(NfVec3){0.0f,0.05f,2.2f});
    NfTeammateSystem teammate;nf_teammate_init(&teammate,&world,true,world.seed^0x12u);assert(teammate.actor_id!=0u);
    NfActor *body=nf_world_find_actor(&world,teammate.actor_id);assert(body!=NULL&&body->faction==NF_FACTION_TEAMMATE);

    /* ALTER_ROUTE is material truth: the closed gate blocks its center. */
    assert(!nf_movement_space_is_free(&world,(NfVec3){0.0f,0.05f,4.5f},world.movement.radius,world.movement.stand_height));
    player->input.interact_held=true;
    assert(nf_route_try_alter(&route,&world,player->id));
    assert(route.open&&route.changed_by==player->id);
    assert(nf_movement_space_is_free(&world,(NfVec3){0.0f,0.05f,4.5f},world.movement.radius,world.movement.stand_height));

    /* Negative control: same world truth, no legitimate evidence, no belief. */
    body->transform.position=(NfVec3){-38.0f,0.05f,-34.0f};
    player->transform.position=(NfVec3){38.0f,0.05f,-34.0f};
    world.tick=route.changed_tick+25u;
    NfControlFrame control={0};
    assert(nf_teammate_tick(&teammate,&world,&route,&control,1u)==1u);
    assert(!teammate.memory.known_open);
    assert(teammate.commitment==NF_TEAMMATE_COMMIT_ROUTE_A);

    /* Direct evidence changes belief and route evaluation. */
    body->transform.position=(NfVec3){0.0f,0.05f,-10.0f};
    player->transform.position=(NfVec3){20.0f,0.05f,-20.0f};
    teammate.next_decision_tick=world.tick;
    assert(nf_teammate_tick(&teammate,&world,&route,&control,1u)==1u);
    assert(teammate.memory.known_open);
    assert(teammate.memory.source==NF_TEAMMATE_EVIDENCE_DIRECT);
    assert(teammate.trace.hotspot_gradient>0.50f);
    assert(teammate.commitment==NF_TEAMMATE_COMMIT_ROUTE_B);
    assert(nf_teammate_route_utility(&teammate,true)>nf_teammate_route_utility(&teammate,false));

    /* Persistent memory is causal, not decorative. */
    const uint64_t aged=teammate.memory.observed_tick+NF_TEAMMATE_RECENT_MEMORY_TICKS+1u;
    nf_teammate_force_memory_age(&teammate,aged);
    assert(teammate.memory.consolidated);

    /* Material override: remembered possibility cannot manufacture a closed gate. */
    nf_route_apply_replica(&route,&world,false,world.tick,player->id);
    teammate.next_decision_tick=world.tick;
    nf_teammate_tick(&teammate,&world,&route,&control,1u);
    assert(nf_teammate_route_utility(&teammate,true)<=-FLT_MAX*0.5f);
    assert(teammate.commitment==NF_TEAMMATE_COMMIT_ROUTE_A);

    /* Existing soft occupancy plus temporal projection produces yielding. */
    nf_route_apply_replica(&route,&world,true,world.tick,player->id);
    body->transform.position=(NfVec3){0.0f,0.05f,0.0f};
    player->transform.position=(NfVec3){0.4f,0.05f,0.2f};
    teammate.next_decision_tick=world.tick;
    nf_teammate_tick(&teammate,&world,&route,&control,1u);
    assert(teammate.trace.yielded_for_occupancy);
}

int main(void) {
    biography_counterfactual();
    route_and_evidence_contract();
    puts("nightfall v1.2 teammate causality / kinetic stigmergy tests: PASS");
    return 0;
}
