#include "nf_claim.h"
#include "nf_route.h"
#include "nf_tactical.h"
#include "nf_teammate.h"
#include "nf_world.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

static void claim_contract(void) {
    NfClaimTable table;
    nf_claim_table_init(&table);
    const uint32_t cover=0xC0000042u;
    assert(nf_claim_try_acquire(&table,NF_CLAIM_COVER,NF_CLAIM_SOFT,1u,cover,10u,30u,NULL)==NF_CLAIM_RESULT_GRANTED);
    assert(nf_claim_try_acquire(&table,NF_CLAIM_COVER,NF_CLAIM_SOFT,2u,cover,10u,30u,NULL)==NF_CLAIM_RESULT_SHARED);
    assert(nf_claim_try_acquire(&table,NF_CLAIM_COVER,NF_CLAIM_HARD,3u,cover,10u,30u,NULL)==NF_CLAIM_RESULT_BLOCKED);
    assert(nf_claim_release(&table,NF_CLAIM_COVER,1u,cover));
    assert(nf_claim_release(&table,NF_CLAIM_COVER,2u,cover));
    assert(nf_claim_try_acquire(&table,NF_CLAIM_COVER,NF_CLAIM_HARD,3u,cover,20u,10u,NULL)==NF_CLAIM_RESULT_GRANTED);
    assert(nf_claim_hard_owner(&table,NF_CLAIM_COVER,cover,25u)==3u);
    assert(nf_claim_is_blocked(&table,NF_CLAIM_COVER,4u,cover,25u));
    nf_claim_expire(&table,31u);
    assert(nf_claim_hard_owner(&table,NF_CLAIM_COVER,cover,31u)==0u);
}

static void route_revision_contract(void) {
    NfWorld world;
    nf_world_init(&world,20260814u);
    nf_world_build_movement_lab(&world);
    NfRouteSystem route;
    nf_route_init_v12_slice(&route,&world);
    assert(route.configured);
    assert(route.gate_key==NF_ROUTE_GATE_KEY);
    assert(route.revision==1u);

    const NfEntityId player=nf_world_spawn_actor(&world,NF_FACTION_PLAYER,(NfVec3){0.0f,0.05f,2.2f});
    NfActor *body=nf_world_find_actor(&world,player);
    assert(body!=NULL);
    body->input.interact_held=true;
    assert(nf_route_try_alter(&route,&world,player));
    assert(route.open);
    assert(route.revision==2u);
    assert(route.changed_by==player);

    nf_route_apply_replica(&route,&world,false,world.tick+1u,player);
    assert(!route.open);
    assert(route.revision==3u);
}

static void exposure_contract(void) {
    NfWorld world;
    nf_world_init(&world,20260814u);
    const NfVec3 threat={0.0f,0.0f,0.0f};
    const NfVec3 open={0.0f,0.0f,8.0f};
    const float open_exposure=nf_tactical_exposure(&world,threat,open);
    assert(open_exposure>0.99f);

    const int wall=nf_world_add_collider(&world,NF_COLLIDER_SOLID,(NfVec3){-2.0f,0.0f,3.0f},(NfVec3){2.0f,2.0f,4.0f});
    assert(wall>=0);
    const float covered_exposure=nf_tactical_exposure(&world,threat,open);
    assert(covered_exposure<open_exposure);
    const float covered_utility=nf_tactical_cover_utility(covered_exposure,5.0f,12.0f);
    const float open_utility=nf_tactical_cover_utility(open_exposure,5.0f,12.0f);
    assert(covered_utility>open_utility);
}

static void integrated_direct_evidence_contract(void) {
    NfWorld world;
    nf_world_init(&world,20260814u);
    nf_world_build_movement_lab(&world);
    NfRouteSystem route;
    nf_route_init_v12_slice(&route,&world);
    const NfEntityId player=nf_world_spawn_actor(&world,NF_FACTION_PLAYER,(NfVec3){0.0f,0.05f,2.0f});
    NfTeammateSystem teammate;
    nf_teammate_init(&teammate,&world,true,world.seed^0x14u);
    NfActor *player_body=nf_world_find_actor(&world,player);
    NfActor *team_body=nf_world_find_actor(&world,teammate.actor_id);
    assert(player_body&&team_body);
    team_body->transform.position=(NfVec3){0.0f,0.05f,-12.0f};
    team_body->transform.velocity=(NfVec3){0};
    teammate.commitment=NF_TEAMMATE_COMMIT_ROUTE_A;
    teammate.waypoint_index=0u;
    teammate.next_decision_tick=world.tick;

    NfControlFrame frame={0};
    assert(nf_teammate_tick(&teammate,&world,&route,&frame,1u)==1u);
    assert(!teammate.memory.known_open);

    player_body->input.interact_held=true;
    assert(nf_route_try_alter(&route,&world,player));
    teammate.next_decision_tick=world.tick;
    assert(nf_teammate_tick(&teammate,&world,&route,&frame,1u)==1u);
    assert(teammate.memory.known_open);
    assert(teammate.memory.source==NF_TEAMMATE_EVIDENCE_DIRECT);
    assert(teammate.trace.hotspot_gradient>0.0f);
}

int main(void) {
    claim_contract();
    route_revision_contract();
    exposure_contract();
    integrated_direct_evidence_contract();
    puts("nightfall v1.4 high-medium integration tests: PASS");
    puts("[v1.4] CLAIM SOFT_SHARE=PASS HARD_BLOCK=PASS EXPIRY=PASS");
    puts("[v1.4] OBJECT STABLE_ID=PASS REVISION=PASS");
    puts("[v1.4] EXPOSURE GRADIENT=PASS UTILITY=PASS");
    puts("[v1.4] WORLD_CHANGE DIRECT_EVIDENCE=PASS BELIEF=PASS GRADIENT=PASS");
    return 0;
}
