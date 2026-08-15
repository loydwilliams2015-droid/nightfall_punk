#include "nf_ai.h"
#include "nf_claim.h"
#include "nf_report.h"
#include "nf_route.h"
#include "nf_teammate.h"
#include "nf_world.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

static void settle(NfWorld *world, unsigned ticks) {
    for (unsigned i = 0u; i < ticks; ++i) {
        nf_world_step(world, 1.0f/(float)NF_TICK_RATE);
    }
}

static void report_contract(void) {
    NfReportBus bus;
    nf_report_bus_init(&bus);

    uint32_t first_id = 0u;
    assert(nf_report_publish(&bus, (NfReport){
        .subject_key = 77u,
        .kind = NF_REPORT_ENEMY_POSITION,
        .scope = NF_REPORT_SCOPE_RIVAL_CREW,
        .origin = 11u,
        .reporter = 11u,
        .coarse_position = {13.2f, 0.0f, -6.1f},
        .confidence = 0.80f,
        .origin_tick = 100u,
        .issued_tick = 100u,
        .deliver_tick = 112u,
        .expiry_tick = 280u
    }, &first_id));
    assert(first_id != 0u);
    assert(bus.count == 1u);
    assert(fabsf(bus.reports[0].coarse_position.x - 12.0f) < 0.001f);
    assert(fabsf(bus.reports[0].coarse_position.z + 8.0f) < 0.001f);

    NfReport out = {0};
    assert(!nf_report_best(
        &bus, 22u, NF_FACTION_RIVAL, 111u,
        NF_REPORT_ENEMY_POSITION, 77u, &out));
    assert(!nf_report_best(
        &bus, 22u, NF_FACTION_TEAMMATE, 120u,
        NF_REPORT_ENEMY_POSITION, 77u, &out));
    assert(nf_report_best(
        &bus, 22u, NF_FACTION_RIVAL, 120u,
        NF_REPORT_ENEMY_POSITION, 77u, &out));
    assert(out.id == first_id);
    assert(out.ancestry_id == first_id);
    const float first_weight = nf_report_weight(&out, 120u);
    assert(first_weight > 0.0f && first_weight < 0.81f);

    uint32_t relay_id = 0u;
    assert(nf_report_relay(
        &bus, &out, 33u, 0.90f, 121u, 130u, 260u, &relay_id));
    assert(relay_id != 0u && relay_id != first_id);
    const NfReport *relay = &bus.reports[1];
    assert(relay->ancestry_id == first_id);
    assert(relay->hops == 1u);
    assert(relay->confidence < out.confidence);
    assert(nf_report_live_count(&bus, 140u) == 2u);

    NfReport best = {0};
    assert(nf_report_best(
        &bus, 44u, NF_FACTION_RIVAL, 140u,
        NF_REPORT_ENEMY_POSITION, 77u, &best));
    assert(best.ancestry_id == first_id);
    assert(nf_report_live_count(&bus, 400u) == 0u);
}

static void claim_contract(void) {
    NfClaimTable claims;
    nf_claim_table_init(&claims);
    const uint32_t subject = 0xC0000011u;

    assert(nf_claim_try_acquire(
        &claims, NF_CLAIM_COVER, NF_CLAIM_SOFT, 1u, subject,
        10u, 30u, NULL) == NF_CLAIM_RESULT_GRANTED);
    assert(nf_claim_try_acquire(
        &claims, NF_CLAIM_COVER, NF_CLAIM_SOFT, 2u, subject,
        10u, 30u, NULL) == NF_CLAIM_RESULT_SHARED);
    assert(nf_claim_try_acquire(
        &claims, NF_CLAIM_COVER, NF_CLAIM_HARD, 3u, subject,
        10u, 30u, NULL) == NF_CLAIM_RESULT_BLOCKED);
    assert(nf_claim_live_count(&claims, 20u) == 2u);

    assert(nf_claim_release(&claims, NF_CLAIM_COVER, 1u, subject));
    assert(nf_claim_release(&claims, NF_CLAIM_COVER, 2u, subject));
    assert(nf_claim_try_acquire(
        &claims, NF_CLAIM_COVER, NF_CLAIM_HARD, 3u, subject,
        20u, 10u, NULL) == NF_CLAIM_RESULT_GRANTED);
    assert(nf_claim_hard_owner(&claims, NF_CLAIM_COVER, subject, 25u) == 3u);
    assert(nf_claim_is_blocked(&claims, NF_CLAIM_COVER, 4u, subject, 25u));
    nf_claim_expire(&claims, 31u);
    assert(nf_claim_hard_owner(&claims, NF_CLAIM_COVER, subject, 31u) == 0u);
    assert(nf_claim_try_acquire(
        &claims, NF_CLAIM_COVER, NF_CLAIM_HARD, 4u, subject,
        31u, 10u, NULL) == NF_CLAIM_RESULT_GRANTED);
}

static void route_revision_and_crew_report_contract(void) {
    NfWorld world;
    nf_world_init(&world, 20260814u);
    nf_world_build_movement_lab(&world);
    NfRouteSystem route;
    nf_route_init_v12_slice(&route, &world);
    assert(route.configured && route.gate_key == NF_ROUTE_GATE_KEY);
    assert(route.revision == 1u);

    const NfEntityId player_id = nf_world_spawn_actor(
        &world, NF_FACTION_PLAYER, (NfVec3){0.0f,0.05f,2.2f});
    NfActor *player = nf_world_find_actor(&world, player_id);
    assert(player != NULL);
    settle(&world, 2u);
    player->input.interact_held = true;
    assert(nf_route_try_alter(&route, &world, player_id));
    assert(route.open && route.revision == 2u);

    NfTeammateSystem teammate;
    nf_teammate_init(&teammate, &world, true, world.seed^0x14u);
    NfActor *body = nf_world_find_actor(&world, teammate.actor_id);
    assert(body != NULL);
    body->transform.position = (NfVec3){-38.0f,0.05f,-34.0f};
    body->transform.velocity = (NfVec3){0};

    assert(nf_teammate_publish_route_report(
        &teammate, &world, &route, player_id));
    assert(teammate.last_route_report_id != 0u);

    NfControlFrame frame = {0};
    teammate.next_decision_tick = world.tick;
    assert(nf_teammate_tick(&teammate, &world, &route, &frame, 1u) == 1u);
    assert(!teammate.memory.known_open);
    assert(teammate.trace.evidence == NF_TEAMMATE_EVIDENCE_NONE);

    world.tick += NF_TICK_RATE/5u + 1u;
    teammate.next_decision_tick = world.tick;
    assert(nf_teammate_tick(&teammate, &world, &route, &frame, 1u) == 1u);
    assert(teammate.memory.known_open);
    assert(teammate.memory.source == NF_TEAMMATE_EVIDENCE_CREW_REPORT);
    assert(teammate.last_report_confidence > 0.0f);

    nf_route_apply_replica(&route, &world, false, world.tick, player_id);
    assert(!route.open && route.revision == 3u);
}

static void rival_report_contract(void) {
    NfWorld world;
    nf_world_init(&world, 20260814u);
    nf_world_build_movement_lab(&world);
    NfAiSystem ai;
    nf_ai_init(&ai, &world, 2u, world.seed^0xA114u);
    NfSemanticBus semantics;
    nf_semantic_bus_init(&semantics);

    const NfEntityId player_id = nf_world_spawn_actor(
        &world, NF_FACTION_PLAYER, (NfVec3){51.3f,0.05f,50.7f});
    assert(player_id != 0u);
    const NfEntityId reporter = ai.agents[0].actor_id;
    const NfEntityId recipient = ai.agents[1].actor_id;

    uint32_t report_id = 0u;
    assert(nf_report_publish(&ai.reports, (NfReport){
        .subject_key = player_id,
        .kind = NF_REPORT_ENEMY_POSITION,
        .scope = NF_REPORT_SCOPE_RIVAL_CREW,
        .origin = reporter,
        .reporter = reporter,
        .coarse_position = {51.3f,0.05f,50.7f},
        .confidence = 0.78f,
        .origin_tick = 10u,
        .issued_tick = 10u,
        .deliver_tick = 30u,
        .expiry_tick = 180u
    }, &report_id));
    assert(report_id != 0u);

    world.tick = 29u;
    ai.agents[1].next_perception_tick = world.tick;
    NfControlFrame frames[NF_AI_MAX_AGENTS] = {{0}};
    assert(nf_ai_tick(&ai, &world, &semantics, frames, NF_AI_MAX_AGENTS) == 2u);
    const NfAiAgent *before = nf_ai_find_agent_const(&ai, recipient);
    assert(before != NULL);
    assert(before->knowledge.target == 0u);
    assert(before->knowledge.source == NF_AI_EVIDENCE_NONE);

    world.tick = 36u;
    ai.agents[1].next_perception_tick = world.tick;
    assert(nf_ai_tick(&ai, &world, &semantics, frames, NF_AI_MAX_AGENTS) == 2u);
    const NfAiAgent *after = nf_ai_find_agent_const(&ai, recipient);
    assert(after != NULL);
    assert(after->knowledge.target == player_id);
    assert(after->knowledge.source == NF_AI_EVIDENCE_REPORT);
    assert(after->knowledge.report_id == report_id);
    assert(after->knowledge.report_ancestry_id == report_id);
    assert(fabsf(after->knowledge.last_heard_position.x - 52.0f) < 0.001f);
    assert(fabsf(after->knowledge.last_heard_position.z - 52.0f) < 0.001f);
    assert(fabsf(after->knowledge.last_heard_position.x - 51.3f) > 0.1f);
}

int main(void) {
    report_contract();
    claim_contract();
    route_revision_and_crew_report_contract();
    rival_report_contract();
    puts("nightfall v1.4 bounded communications / claims / revisions tests: PASS");
    puts("[v1.4] REPORT DELAY=PASS PROVENANCE=PASS COARSE_POSITION=PASS SCOPE=PASS");
    puts("[v1.4] CLAIM SOFT_SHARE=PASS HARD_BLOCK=PASS EXPIRY=PASS");
    puts("[v1.4] ROUTE STABLE_ID=PASS REVISION=PASS CREW_REPORT=PASS RIVAL_REPORT=PASS");
    return 0;
}
