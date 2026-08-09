#include "nf_ai.h"
#include "nf_cattler.h"
#include "nf_cattler_recurrence.h"
#include "nf_lifeworld.h"
#include "nf_lifeworld_spatial.h"
#include "nf_semantics.h"
#include "nf_spatial.h"
#include "nf_world.h"

#include <assert.h>
#include <stdio.h>

static void ordinary_return_uses_ecological_source(void) {
    NfWorld world;
    nf_world_init(&world,20260808u);
    nf_world_build_movement_lab(&world);

    NfCattlerSystem cattlers;
    nf_cattler_init(&cattlers,&world,2u,NF_CATTLER_PROFILE_PACK,0xCA771Eu);
    NfCattlerRecurrenceSystem recurrence;
    nf_cattler_recurrence_init(&recurrence,0xA11CEu);

    NfActor *victim=nf_world_find_actor(&world,cattlers.agents[0].actor_id);
    NfActor *survivor=nf_world_find_actor(&world,cattlers.agents[1].actor_id);
    assert(victim!=NULL&&survivor!=NULL);
    const uint8_t death_region=nf_region_nearest(&cattlers.graph,victim->transform.position);
    victim->combat.alive=false;
    victim->health=0.0f;

    world.tick=10u;
    nf_cattler_recurrence_tick(&recurrence,&cattlers,&world);
    world.tick+=NF_CATTLER_RECOLONIZE_BASE_TICKS+
        NF_CATTLER_RECOLONIZE_VARIANCE_TICKS+NF_TICK_RATE*3u;
    nf_cattler_recurrence_tick(&recurrence,&cattlers,&world);

    NfVec3 spawn={0};
    NfCattlerReturnKind kind=NF_CATTLER_RETURN_NONE;
    uint8_t source=NF_REGION_INVALID;
    assert(nf_cattler_recurrence_spawn_for(
        &recurrence,&cattlers,&world,victim->id,&spawn,&kind,&source));
    assert(kind==NF_CATTLER_RETURN_RECOLONIZE);
    assert(source!=NF_REGION_INVALID&&source!=death_region);
    assert(nf_region_path_steps(
        &cattlers.graph,
        nf_region_nearest(&cattlers.graph,survivor->transform.position),source)<=2);

    nf_cattler_recurrence_commit(
        &recurrence,&cattlers,victim->id,spawn,source);
    assert(cattlers.agents[0].home_region==source);
}

static void extinction_reseed_requires_prolonged_player_absence(void) {
    NfWorld world;
    nf_world_init(&world,20260808u);
    nf_world_build_movement_lab(&world);

    NfCattlerSystem cattlers;
    nf_cattler_init(&cattlers,&world,1u,NF_CATTLER_PROFILE_PACK,0xCA771Eu);
    for(size_t i=0u;i<cattlers.graph.count;++i) {
        cattlers.regions[i].resource_value=0.0f;
        cattlers.regions[i].infestation=0.0f;
        cattlers.regions[i].disturbance=0.0f;
    }
    const uint8_t habitat=9u;
    cattlers.regions[habitat].resource_value=0.92f;

    NfActor *victim=nf_world_find_actor(&world,cattlers.agents[0].actor_id);
    assert(victim!=NULL);
    victim->combat.alive=false;
    victim->health=0.0f;

    const NfVec3 watched=cattlers.graph.regions[habitat].center;
    const NfEntityId player_id=nf_world_spawn_actor(
        &world,NF_FACTION_PLAYER,(NfVec3){watched.x,0.05f,watched.z});
    NfActor *player=nf_world_find_actor(&world,player_id);
    assert(player!=NULL);

    NfCattlerRecurrenceSystem recurrence;
    nf_cattler_recurrence_init(&recurrence,0x5EEDu);
    world.tick=10u;
    nf_cattler_recurrence_tick(&recurrence,&cattlers,&world);

    world.tick+=NF_TICK_RATE*100u;
    nf_cattler_recurrence_tick(&recurrence,&cattlers,&world);
    NfVec3 spawn={0};
    NfCattlerReturnKind kind=NF_CATTLER_RETURN_NONE;
    uint8_t source=NF_REGION_INVALID;
    assert(!nf_cattler_recurrence_spawn_for(
        &recurrence,&cattlers,&world,victim->id,&spawn,&kind,&source));

    player->transform.position=(NfVec3){10000.0f,0.05f,10000.0f};
    world.tick+=NF_TICK_RATE*100u;
    nf_cattler_recurrence_tick(&recurrence,&cattlers,&world);
    assert(nf_cattler_recurrence_spawn_for(
        &recurrence,&cattlers,&world,victim->id,&spawn,&kind,&source));
    assert(kind==NF_CATTLER_RETURN_EXTINCTION_RESEED);
    assert(source==habitat);
}

static void attention_biases_spatial_without_overriding_combat(void) {
    NfWorld world;
    nf_world_init(&world,20260808u);
    nf_world_build_movement_lab(&world);
    NfAiSystem ai;
    nf_ai_init(&ai,&world,1u,0xA105u);
    NfSpatialSystem spatial;
    nf_spatial_init(&spatial,&ai,&world,0x507u);

    NfLifeworldSystem life={0};
    life.attention_count=1u;
    life.attention[0]=(NfAttentionChoice){
        .actor_id=ai.agents[0].actor_id,
        .hotspot=NF_HOTSPOT_ACCESS_CHANGE,
        .region=14u,
        .activity=NF_LIFE_TRAVEL,
        .exploration=false
    };

    ai.agents[0].mode=NF_AGENT_IDLE;
    ai.agents[0].knowledge.visible_now=false;
    nf_lifeworld_apply_attention_bias(&spatial,&life,&ai,&world);
    const NfSpatialAgentState *state=nf_spatial_agent_state_const(
        &spatial,ai.agents[0].actor_id);
    assert(state!=NULL);
    assert(state->attention_hint_active);
    assert(state->target_region==14u);
    assert(state->commit_until_tick>world.tick);

    ai.agents[0].mode=NF_AGENT_ENGAGE;
    ai.agents[0].knowledge.visible_now=true;
    nf_lifeworld_apply_attention_bias(&spatial,&life,&ai,&world);
    state=nf_spatial_agent_state_const(&spatial,ai.agents[0].actor_id);
    assert(state!=NULL&&!state->attention_hint_active);
}

int main(void) {
    ordinary_return_uses_ecological_source();
    extinction_reseed_requires_prolonged_player_absence();
    attention_biases_spatial_without_overriding_combat();
    puts("nightfall v1.0 lifeworld runtime tests: PASS");
    return 0;
}
