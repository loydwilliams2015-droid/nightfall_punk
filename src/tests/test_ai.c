#include "nf_ai.h"
#include "nf_cattler.h"
#include "nf_encounter.h"
#include "nf_region.h"
#include "nf_relations.h"
#include "nf_semantics.h"
#include "nf_spatial.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

static float distance_xz(NfVec3 a,NfVec3 b) {
    const float dx=a.x-b.x;
    const float dz=a.z-b.z;
    return sqrtf(dx*dx+dz*dz);
}

static void relation_contract(void) {
    assert(nf_relation_between(NF_FACTION_PLAYER,NF_FACTION_TEAMMATE,NF_RELATION_HOSTILE)==NF_RELATION_COOPERATIVE);
    assert(nf_relation_between(NF_FACTION_PLAYER,NF_FACTION_RIVAL,NF_RELATION_TRUCE)==NF_RELATION_TRUCE);
    assert(!nf_relation_can_damage(NF_RELATION_TRUCE,false));
    assert(nf_relation_can_damage(NF_RELATION_HOSTILE,false));
    assert(nf_relation_between(NF_FACTION_PLAYER,NF_FACTION_RANCHER,NF_RELATION_TRUCE)==NF_RELATION_NONNEGOTIABLE_HOSTILE);
    assert(!nf_relationship_is_negotiable(NF_RELATION_NONNEGOTIABLE_HOSTILE));

    NfRancherProfile ordinary=nf_rancher_profile_from_roll(900u,10u,1000u);
    NfRancherProfile rare=nf_rancher_profile_from_roll(3u,10u,1000u);
    assert(ordinary.negotiation==NF_NEGOTIATION_NEVER&&ordinary.disposition==NF_RANCHER_PREDATORY);
    assert(rare.negotiation==NF_NEGOTIATION_NEVER&&rare.disposition==NF_RANCHER_HOSTILE_NONPREDATORY);
    assert(rare.avoidance_preference>rare.predation_drive);
}

static void semantic_contract(void) {
    NfSemanticBus bus; nf_semantic_bus_init(&bus);
    NfSemanticAlert gun={.type=NF_SEMANTIC_GUNFIRE,.source=7,.position={0,0,0},.radius=10,.tick=5,.lifetime_ticks=60};
    nf_semantic_bus_publish(&bus,gun);
    NfSemanticAlert out[4];
    assert(nf_semantic_collect_audible(&bus,(NfVec3){4,0,0},10,out,4)==1u);
    assert(nf_semantic_collect_audible(&bus,(NfVec3){14,0,0},10,out,4)==0u);
    assert(nf_semantic_collect_audible(&bus,(NfVec3){4,0,0},100,out,4)==0u);
}

static void agent_contract(void) {
    NfWorld world; nf_world_init(&world,20260807u); nf_world_build_movement_lab(&world);
    NfEntityId player=nf_world_spawn_actor(&world,NF_FACTION_PLAYER,(NfVec3){-3,0.05f,-18});
    assert(player!=0u);
    NfAiSystem ai; nf_ai_init(&ai,&world,2u,1234u);
    assert(ai.count==2u);
    assert(ai.affordance_count>0u);
    NfSemanticBus semantics; nf_semantic_bus_init(&semantics);

    bool perceived=false,engaged=false,fired=false;
    for(unsigned step=0;step<240u;++step) {
        NfControlFrame frames[NF_AI_MAX_AGENTS];
        size_t count=nf_ai_tick(&ai,&world,&semantics,frames,NF_AI_MAX_AGENTS);
        for(size_t i=0;i<count;++i) {
            const NfAiAgent *agent=nf_ai_find_agent_const(&ai,frames[i].actor);
            assert(agent!=NULL);
            if(agent->knowledge.target==player&&agent->knowledge.confidence>0.5f) perceived=true;
            if(agent->mode==NF_AGENT_ENGAGE) engaged=true;
            if(frames[i].combat.fire_held||frames[i].combat.fire_pressed) fired=true;
            nf_world_set_input(&world,frames[i].actor,frames[i].move);
        }
        nf_world_step(&world,1.0f/(float)NF_TICK_RATE);
    }
    assert(perceived&&engaged&&fired);

    nf_ai_set_rival_relationship(&ai,NF_RELATION_TRUCE);
    bool held=false,shot_during_truce=false;
    for(unsigned step=0;step<60u;++step) {
        NfControlFrame frames[NF_AI_MAX_AGENTS];
        size_t count=nf_ai_tick(&ai,&world,&semantics,frames,NF_AI_MAX_AGENTS);
        for(size_t i=0;i<count;++i) {
            const NfAiAgent *agent=nf_ai_find_agent_const(&ai,frames[i].actor);
            if(agent!=NULL&&agent->mode==NF_AGENT_TRUCE_HOLD) held=true;
            if(frames[i].combat.fire_held||frames[i].combat.fire_pressed) shot_during_truce=true;
            nf_world_set_input(&world,frames[i].actor,frames[i].move);
        }
        nf_world_step(&world,1.0f/(float)NF_TICK_RATE);
    }
    assert(held&&!shot_during_truce);
}

static void encounter_contract(void) {
    NfWorld world; nf_world_init(&world,20260807u); nf_world_build_movement_lab(&world);
    NfEntityId player=nf_world_spawn_actor(&world,NF_FACTION_PLAYER,(NfVec3){-3,0.05f,-18});
    assert(player!=0u);

    NfAiSystem ai; nf_ai_init(&ai,&world,4u,9876u);
    NfSemanticBus semantics; nf_semantic_bus_init(&semantics);
    NfEncounterState encounter;
    nf_encounter_init(&encounter,&ai,&world,2u,world.seed^0xE06u);

    bool pressure_seen=false;
    bool filtered_fire_seen=false;
    float suppression_peak=0.0f;

    for(unsigned step=0;step<360u;++step) {
        NfControlFrame frames[NF_AI_MAX_AGENTS];
        size_t count=nf_ai_tick(&ai,&world,&semantics,frames,NF_AI_MAX_AGENTS);

        if(step==150u) {
            NfActor *victim=nf_world_find_actor(&world,ai.agents[0].actor_id);
            NfCombatEvent event={0};
            assert(victim!=NULL);
            assert(nf_combat_apply_damage(
                victim,player,NF_WEAPON_CARBINE,NF_HIT_BODY,18.0f,
                world.tick,&event));
        }

        nf_encounter_filter_controls(&encounter,&ai,&world,frames,count);
        const size_t pressure=nf_encounter_pressure_count(&encounter);
        assert(pressure<=2u);
        if(pressure>0u) pressure_seen=true;

        for(size_t i=0;i<count;++i) {
            const NfEncounterAgentState *state=
                nf_encounter_agent_state_const(&encounter,frames[i].actor);
            assert(state!=NULL);
            assert(state->aim_settle>=0.0f&&state->aim_settle<=1.0f);
            assert(state->suppression>=0.0f&&state->suppression<=1.0f);
            if(state->suppression>suppression_peak) suppression_peak=state->suppression;
            if(frames[i].combat.fire_held||frames[i].combat.fire_pressed) {
                filtered_fire_seen=true;
            }
            nf_world_set_input(&world,frames[i].actor,frames[i].move);
        }
        nf_world_step(&world,1.0f/(float)NF_TICK_RATE);
    }

    assert(pressure_seen);
    assert(filtered_fire_seen);
    assert(suppression_peak>0.25f);
}

static void spatial_contract(void) {
    NfRegionGraph graph;
    nf_region_graph_init_spatial_lab(&graph);
    assert(graph.count==NF_REGION_MAX);
    assert(nf_region_path_steps(&graph,0u,23u)>0);
    const uint8_t first_hop=nf_region_next_hop(&graph,0u,23u);
    assert(first_hop!=NF_REGION_INVALID);
    assert(nf_region_are_neighbors(&graph,0u,first_hop));

    NfSpatialFields clear={
        .route_congestion=0.0f,.ally_support=0.6f,.enemy_threat=0.3f,
        .exposure=0.4f,.pressure=0.4f,.objective_value=0.2f
    };
    NfSpatialFields crowded=clear;
    crowded.route_congestion=1.0f;
    crowded.objective_value=1.0f;
    assert(nf_spatial_field_preference(&clear)>nf_spatial_field_preference(&crowded));

    NfWorld world; nf_world_init(&world,20260807u); nf_world_build_movement_lab(&world);
    NfAiSystem ai; nf_ai_init(&ai,&world,2u,4242u);
    nf_ai_set_rival_relationship(&ai,NF_RELATION_TRUCE);
    NfSemanticBus semantics; nf_semantic_bus_init(&semantics);
    NfSpatialSystem spatial; nf_spatial_init(&spatial,&ai,&world,world.seed^0x507u);
    assert(spatial.count==2u);

    const NfActor *first=nf_world_find_actor_const(&world,ai.agents[0].actor_id);
    assert(first!=NULL);
    const NfVec3 start=first->transform.position;
    bool remote_target_seen=false;
    bool moving_seen=false;

    for(unsigned step=0;step<720u;++step) {
        nf_spatial_tick(&spatial,&ai,&world,&semantics);
        NfControlFrame frames[NF_AI_MAX_AGENTS];
        const size_t count=nf_ai_tick(&ai,&world,&semantics,frames,NF_AI_MAX_AGENTS);
        nf_spatial_filter_controls(&spatial,&ai,&world,frames,count);
        for(size_t i=0;i<count;++i) {
            const NfSpatialAgentState *state=
                nf_spatial_agent_state_const(&spatial,frames[i].actor);
            assert(state!=NULL);
            assert(state->current_region<NF_REGION_MAX);
            assert(state->target_region<NF_REGION_MAX);
            assert(state->fields.route_congestion>=0.0f&&state->fields.route_congestion<=1.0f);
            assert(state->fields.ally_support>=0.0f&&state->fields.ally_support<=1.0f);
            assert(state->fields.enemy_threat>=0.0f&&state->fields.enemy_threat<=1.0f);
            assert(state->fields.exposure>=0.0f&&state->fields.exposure<=1.0f);
            assert(state->fields.pressure>=0.0f&&state->fields.pressure<=1.0f);
            assert(state->fields.objective_value>=0.0f&&state->fields.objective_value<=1.0f);
            assert(state->fields.preference_score>=0.0f&&state->fields.preference_score<=1.0f);
            if(state->target_region!=state->current_region) remote_target_seen=true;
            if(fabsf(frames[i].move.forward)>0.1f||fabsf(frames[i].move.strafe)>0.1f) moving_seen=true;
            nf_world_set_input(&world,frames[i].actor,frames[i].move);
        }
        nf_world_step(&world,1.0f/(float)NF_TICK_RATE);
    }

    first=nf_world_find_actor_const(&world,ai.agents[0].actor_id);
    assert(first!=NULL);
    assert(remote_target_seen&&moving_seen);
    assert(distance_xz(start,first->transform.position)>5.0f);

    NfSemanticAlert story={
        .type=NF_SEMANTIC_STORY_PHASE_CHANGED,.position=first->transform.position,
        .intensity=1.0f,.radius=0.0f,.tick=world.tick,.lifetime_ticks=NF_TICK_RATE
    };
    nf_semantic_bus_publish(&semantics,story);
    nf_spatial_tick(&spatial,&ai,&world,&semantics);
    const NfSpatialAgentState *state=nf_spatial_agent_state_const(&spatial,ai.agents[0].actor_id);
    assert(state!=NULL);
    assert(state->interrupt==NF_SPATIAL_INTERRUPT_TASK_BREAK||
           state->interrupt==NF_SPATIAL_INTERRUPT_NONE);
}

static void cattler_contract(void) {
    NfWorld world; nf_world_init(&world,20260807u); nf_world_build_movement_lab(&world);
    NfSemanticBus semantics; nf_semantic_bus_init(&semantics);
    NfCattlerSystem cattlers;
    nf_cattler_init(&cattlers,&world,3u,NF_CATTLER_PROFILE_PACK,world.seed^0xCA771Eu);
    assert(cattlers.count==3u);
    for(size_t i=0;i<cattlers.count;++i) {
        const NfActor *body=nf_world_find_actor_const(&world,cattlers.agents[i].actor_id);
        assert(body!=NULL&&body->faction==NF_FACTION_RANCHER);
        assert(cattlers.agents[i].social==NF_CATTLER_PACK);
        assert(cattlers.agents[i].locomotor_integrity==1.0f);
        const float score=nf_cattler_habitat_score(&cattlers,&cattlers.agents[i],&world,cattlers.agents[i].home_region);
        assert(score>=0.0f&&score<=1.0f);
    }

    NfActor *first=nf_world_find_actor(&world,cattlers.agents[0].actor_id);
    assert(first!=NULL);
    NfEntityId player_id=nf_world_spawn_actor(
        &world,NF_FACTION_PLAYER,
        (NfVec3){first->transform.position.x,0.05f,first->transform.position.z+16.0f});
    assert(player_id!=0u);

    bool report_seen=false;
    bool predation_control_seen=false;
    bool moving_seen=false;
    for(unsigned step=0;step<1800u;++step) {
        NfControlFrame frames[NF_CATTLER_MAX_AGENTS];
        const size_t count=nf_cattler_tick(
            &cattlers,&world,&semantics,frames,NF_CATTLER_MAX_AGENTS);
        assert(count==3u);
        for(size_t i=0;i<count;++i) {
            if(frames[i].combat.fire_held||frames[i].combat.fire_pressed) predation_control_seen=true;
            if(fabsf(frames[i].move.forward)>0.1f||fabsf(frames[i].move.strafe)>0.1f) moving_seen=true;
            nf_world_set_input(&world,frames[i].actor,frames[i].move);
        }
        nf_world_step(&world,1.0f/(float)NF_TICK_RATE);
    }
    for(size_t i=0;i<NF_CATTLER_BLACKBOARD_REPORTS;++i) {
        if(cattlers.reports[i].active) report_seen=true;
    }
    bool infestation_seen=false;
    for(uint8_t r=0u;r<NF_REGION_MAX;++r) {
        const NfCattlerRegionState *region=nf_cattler_region_state_const(&cattlers,r);
        assert(region!=NULL);
        if(region->infestation>0.20f) infestation_seen=true;
    }
    assert(report_seen&&predation_control_seen&&moving_seen&&infestation_seen);

    const float locomotor_before=cattlers.agents[0].locomotor_integrity;
    NfCombatEvent knee={
        .server_tick=world.tick,.type=NF_COMBAT_EVENT_DAMAGE,
        .source=player_id,.target=cattlers.agents[0].actor_id,
        .weapon=NF_WEAPON_CARBINE,.hit_zone=NF_HIT_KNEE,.amount=22.5f,
        .position=first->transform.position
    };
    nf_cattler_on_combat_event(&cattlers,&world,&knee);
    assert(cattlers.agents[0].locomotor_integrity<locomotor_before);

    NfCombatEvent prey={
        .server_tick=world.tick,.type=NF_COMBAT_EVENT_DAMAGE,
        .source=cattlers.agents[0].actor_id,.target=player_id,
        .weapon=NF_WEAPON_CARBINE,.hit_zone=NF_HIT_BODY,.amount=18.0f,
        .position=nf_world_find_actor_const(&world,player_id)->transform.position
    };
    nf_cattler_on_combat_event(&cattlers,&world,&prey);
    assert(cattlers.agents[0].score.prey>0.0f);
    assert(cattlers.ecology_score.prey>0.0f);

    NfWorld loner_world; nf_world_init(&loner_world,20260807u); nf_world_build_movement_lab(&loner_world);
    NfCattlerSystem loner; nf_cattler_init(&loner,&loner_world,1u,NF_CATTLER_PROFILE_LONER,123u);
    assert(loner.agents[0].social==NF_CATTLER_LONER);

    NfWorld horde_world; nf_world_init(&horde_world,20260807u); nf_world_build_movement_lab(&horde_world);
    NfSemanticBus horde_semantics; nf_semantic_bus_init(&horde_semantics);
    NfCattlerSystem horde; nf_cattler_init(&horde,&horde_world,5u,NF_CATTLER_PROFILE_HORDE,456u);
    NfControlFrame horde_frames[NF_CATTLER_MAX_AGENTS];
    assert(nf_cattler_tick(&horde,&horde_world,&horde_semantics,horde_frames,NF_CATTLER_MAX_AGENTS)==5u);
    assert(horde.event_mode==NF_CATTLER_EVENT_HUNT_SURGE);
}

int main(void) {
    relation_contract();
    semantic_contract();
    agent_contract();
    encounter_contract();
    spatial_contract();
    cattler_contract();
    puts("nightfall v0.8 dream cattler habitat ecology tests: PASS");
    return 0;
}
