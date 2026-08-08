#include "nf_ai.h"
#include "nf_relations.h"
#include "nf_semantics.h"

#include <assert.h>
#include <stdio.h>

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

int main(void) {
    relation_contract();
    semantic_contract();
    agent_contract();
    puts("nightfall v0.5 agent intelligence tests: PASS");
    return 0;
}
