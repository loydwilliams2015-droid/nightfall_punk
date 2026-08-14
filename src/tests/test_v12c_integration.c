#include "nf_ai.h"
#include "nf_cattler.h"
#include "nf_route.h"
#include "nf_semantics.h"
#include "nf_teammate.h"
#include "nf_world.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

static float distance_xz(NfVec3 a, NfVec3 b) {
    const float dx = a.x-b.x;
    const float dz = a.z-b.z;
    return sqrtf(dx*dx+dz*dz);
}

static size_t faction_count(const NfWorld *world, NfFaction faction) {
    size_t count = 0u;
    for (size_t i = 0u; i < NF_MAX_ENTITIES; ++i) {
        const NfActor *actor = &world->actors[i];
        if (actor->active && actor->faction == faction) ++count;
    }
    return count;
}

int main(void) {
    NfWorld world;
    nf_world_init(&world,20260813u);
    nf_world_build_movement_lab(&world);

    NfRouteSystem route;
    nf_route_init_v12_slice(&route,&world);

    NfSemanticBus semantics;
    nf_semantic_bus_init(&semantics);

    const NfEntityId player_id = nf_world_spawn_actor(
        &world,NF_FACTION_PLAYER,(NfVec3){0.0f,0.05f,-18.0f});
    assert(player_id != 0u);

    NfTeammateSystem teammate;
    nf_teammate_init(&teammate,&world,true,world.seed^0x12u);
    assert(teammate.actor_id != 0u);

    NfAiSystem rivals;
    nf_ai_init(&rivals,&world,2u,world.seed^0xA105u);
    nf_ai_set_rival_relationship(&rivals,NF_RELATION_HOSTILE);
    assert(rivals.count == 2u);

    NfCattlerSystem cattlers;
    nf_cattler_init(&cattlers,&world,2u,NF_CATTLER_PROFILE_NORMAL,world.seed^0xCA771Eu);
    assert(cattlers.count == 2u);

    assert(faction_count(&world,NF_FACTION_PLAYER) == 1u);
    assert(faction_count(&world,NF_FACTION_TEAMMATE) == 1u);
    assert(faction_count(&world,NF_FACTION_RIVAL) == 2u);
    assert(faction_count(&world,NF_FACTION_RANCHER) == 2u);

    const NfActor *teammate_body = nf_world_find_actor_const(&world,teammate.actor_id);
    assert(teammate_body != NULL);
    const NfVec3 teammate_start = teammate_body->transform.position;

    NfVec3 rival_start[NF_AI_MAX_AGENTS] = {{0}};
    for (size_t i = 0u; i < rivals.count; ++i) {
        const NfActor *body = nf_world_find_actor_const(&world,rivals.agents[i].actor_id);
        assert(body != NULL);
        rival_start[i] = body->transform.position;
    }

    NfVec3 cattler_start[NF_CATTLER_MAX_AGENTS] = {{0}};
    for (size_t i = 0u; i < cattlers.count; ++i) {
        const NfActor *body = nf_world_find_actor_const(&world,cattlers.agents[i].actor_id);
        assert(body != NULL);
        cattler_start[i] = body->transform.position;
    }

    float max_teammate_control = 0.0f;
    const unsigned proof_ticks = NF_TICK_RATE * 5u;
    for (unsigned tick = 0u; tick < proof_ticks; ++tick) {
        nf_world_set_input(&world,player_id,(NfMoveInput){0});

        NfControlFrame teammate_control = {0};
        if (nf_teammate_tick(&teammate,&world,&route,&teammate_control,1u) == 1u) {
            const float magnitude = fabsf(teammate_control.move.forward) + fabsf(teammate_control.move.strafe);
            if (magnitude > max_teammate_control) max_teammate_control = magnitude;
            nf_world_set_input(&world,teammate_control.actor,teammate_control.move);
        }

        NfControlFrame rival_controls[NF_AI_MAX_AGENTS] = {{0}};
        const size_t rival_controls_count = nf_ai_tick(
            &rivals,&world,&semantics,rival_controls,NF_AI_MAX_AGENTS);
        for (size_t i = 0u; i < rival_controls_count; ++i) {
            nf_world_set_input(&world,rival_controls[i].actor,rival_controls[i].move);
        }

        NfControlFrame cattler_controls[NF_CATTLER_MAX_AGENTS] = {{0}};
        const size_t cattler_controls_count = nf_cattler_tick(
            &cattlers,&world,&semantics,cattler_controls,NF_CATTLER_MAX_AGENTS);
        for (size_t i = 0u; i < cattler_controls_count; ++i) {
            nf_world_set_input(&world,cattler_controls[i].actor,cattler_controls[i].move);
        }

        nf_world_step(&world,1.0f/(float)NF_TICK_RATE);
    }

    teammate_body = nf_world_find_actor_const(&world,teammate.actor_id);
    assert(teammate_body != NULL);
    const float teammate_distance = distance_xz(teammate_start,teammate_body->transform.position);
    assert(max_teammate_control > 0.25f);
    assert(teammate_distance > 1.0f);

    float max_rival_distance = 0.0f;
    for (size_t i = 0u; i < rivals.count; ++i) {
        const NfActor *body = nf_world_find_actor_const(&world,rivals.agents[i].actor_id);
        assert(body != NULL);
        const float d = distance_xz(rival_start[i],body->transform.position);
        if (d > max_rival_distance) max_rival_distance = d;
    }
    assert(max_rival_distance > 0.50f);

    float max_cattler_distance = 0.0f;
    for (size_t i = 0u; i < cattlers.count; ++i) {
        const NfActor *body = nf_world_find_actor_const(&world,cattlers.agents[i].actor_id);
        assert(body != NULL);
        const float d = distance_xz(cattler_start[i],body->transform.position);
        if (d > max_cattler_distance) max_cattler_distance = d;
    }
    assert(max_cattler_distance > 0.50f);

    printf("POPULATION PLAYER=1 TEAMMATE=1 RIVAL=%zu CATTLER=%zu\n",rivals.count,cattlers.count);
    printf("TEAMMATE CONTROL=%.2f DISPLACEMENT=%.2fm\n",max_teammate_control,teammate_distance);
    printf("RIVAL DISPLACEMENT=%.2fm CATTLER DISPLACEMENT=%.2fm\n",max_rival_distance,max_cattler_distance);
    puts("nightfall v1.2c integrated population / embodied action tests: PASS");
    return 0;
}
