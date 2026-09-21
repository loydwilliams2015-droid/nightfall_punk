#include "nf_logic17b.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static int failures = 0;

static void expect_true(bool value, const char *name) {
    if (!value) {
        fprintf(stderr, "FAIL: %s\n", name);
        ++failures;
    }
}

static void expect_int(int actual, int expected, const char *name) {
    if (actual != expected) {
        fprintf(stderr, "FAIL: %s actual=%d expected=%d\n", name, actual, expected);
        ++failures;
    }
}

static Nf17bTransaction txn(uint32_t tick, uint32_t target, Nf17bTxnKind kind,
                            Nf17bComponentMask read_mask, Nf17bComponentMask write_mask) {
    Nf17bTransaction t;
    memset(&t, 0, sizeof(t));
    t.tick = tick;
    t.target_id = target;
    t.kind = (uint8_t)kind;
    t.domain = (uint8_t)NF17B_DOMAIN_MATERIAL;
    t.read_mask = read_mask;
    t.write_mask = write_mask;
    t.precondition_mask = read_mask;
    return t;
}

int main(void) {
    Nf17bAuthoritativeState state;
    nf17b_state_init(&state);
    Nf17bSnapshot s0 = nf17b_snapshot(&state);
    expect_int((int)s0.state.tick, 0, "snapshot tick");
    expect_true(s0.deterministic_hash != 0u, "snapshot has deterministic hash");

    Nf17bTransaction open = txn(0u, 4u, NF17B_TXN_OPEN,
        NF17B_COMPONENT_TOPOLOGY, NF17B_COMPONENT_TOPOLOGY);
    Nf17bTransaction close = txn(0u, 4u, NF17B_TXN_CLOSE,
        NF17B_COMPONENT_TOPOLOGY, NF17B_COMPONENT_TOPOLOGY);
    Nf17bTransaction dmg_a = txn(0u, 7u, NF17B_TXN_DAMAGE,
        NF17B_COMPONENT_MATERIAL, NF17B_COMPONENT_MATERIAL);
    Nf17bTransaction dmg_b = txn(0u, 7u, NF17B_TXN_DAMAGE,
        NF17B_COMPONENT_MATERIAL, NF17B_COMPONENT_MATERIAL);
    Nf17bTransaction destroy = txn(0u, 7u, NF17B_TXN_DESTROY,
        NF17B_COMPONENT_MATERIAL, NF17B_COMPONENT_MATERIAL);

    expect_int((int)nf17b_classify_conflict(&open, &close),
               (int)NF17B_CONFLICT_EXCLUSIVE, "open close exclusive");
    expect_int((int)nf17b_classify_conflict(&dmg_a, &dmg_b),
               (int)NF17B_CONFLICT_MERGEABLE, "damage mergeable");
    expect_int((int)nf17b_classify_conflict(&dmg_a, &destroy),
               (int)NF17B_CONFLICT_TRANSFORMING, "destroy transforming");
    expect_true(nf17b_transaction_valid_for_snapshot(&open, &s0), "transaction valid against S_t");
    open.tick = 1u;
    expect_true(!nf17b_transaction_valid_for_snapshot(&open, &s0), "future transaction rejected");

    nf17b_commit_write_mask(&state, NF17B_COMPONENT_TOPOLOGY | NF17B_COMPONENT_OCCUPANCY);
    expect_int((int)state.state_version, 2, "state version increments");
    expect_int((int)state.topology_version, 2, "topology version increments");
    expect_int((int)state.occupancy_version, 2, "occupancy version increments");
    expect_int((int)state.material_version, 1, "unwritten material version stable");

    Nf17bAgreement provisional = {1u, (uint8_t)NF17B_AGREEMENT_RESERVED, 0u, 0u};
    expect_int((int)nf17b_cancel_agreement(&provisional),
               (int)NF17B_CANCEL_ROLLED_BACK, "unsurfaced reservation rolls back");
    Nf17bAgreement surfaced = {2u, (uint8_t)NF17B_AGREEMENT_COMMITTED, 1u, 0u};
    expect_int((int)nf17b_cancel_agreement(&surfaced),
               (int)NF17B_CANCEL_COMPENSATION_REQUIRED, "surfaced commit compensates");

    Nf17bCrossNexusTransfer semantic_only = {1u, 2u, 0u, 1000, 1u, 1u, 0u};
    expect_true(!nf17b_cross_nexus_surfaces(&semantic_only), "semantic adjacency cannot create CBC");
    Nf17bCrossNexusTransfer heat = {1u, 2u, 1u, 1000, 1u, 0u, 0u};
    expect_true(nf17b_cross_nexus_surfaces(&heat), "material crossing with target consequence surfaces");

    Nf17bFrontier frontier;
    nf17b_frontier_init(&frontier);
    expect_true(nf17b_frontier_push(&frontier,
        (Nf17bFrontierItem){10u, 1u, 2u, 0u, 8u, 5u, 0u, 1u, 0u}), "push deep consequential");
    expect_true(nf17b_frontier_push(&frontier,
        (Nf17bFrontierItem){11u, 2u, 1u, 0u, 2u, 3u, 0u, 0u, 0u}), "push shallow");
    Nf17bFrontierItem picked;
    expect_true(nf17b_frontier_pop(&frontier, NF17B_FRONTIER_UISR_HIERARCHICAL, &picked),
                "pop frontier");
    expect_int((int)picked.id, 11, "causality precedes consequence in UISR frontier");

    Nf17bRefinementMetrics high = {0.9f, 0.95f, 0.9f, 0.8f, 0.85f};
    expect_int((int)nf17b_refinement_decide(high, 0.75f, 0.20f),
               (int)NF17B_REFINEMENT_REFINE, "high-value refinement");
    Nf17bRefinementMetrics low = {0.1f, 0.1f, 0.05f, 0.1f, 0.05f};
    expect_int((int)nf17b_refinement_decide(low, 0.75f, 0.20f),
               (int)NF17B_REFINEMENT_REAGGREGATE, "low-value reaggregation");

    NfSpatialWorld world;
    nf_spatial_world_init(&world, NF_SPATIAL_LAB_LOOP, 210001u);
    Nf17bTraversalProfile profile;
    memset(&profile, 0, sizeof(profile));
    profile.allowed_support_mask =
        (1u << NF_SUPPORT_GROUND) |
        (1u << NF_SUPPORT_PLATFORM) |
        (1u << NF_SUPPORT_LADDER) |
        (1u << NF_SUPPORT_LEDGE);
    profile.horizon = 5u;
    profile.observation_radius = 3u;
    profile.exposure_weight = 1.0f;
    profile.ecology_weight = 1.0f;
    profile.resource_reward = 0.25f;
    profile.branch_reward = 0.08f;
    profile.affordance_reward = 0.2f;
    profile.contract_context_weight = 0.1f;
    Nf17bRouteHypothesis route = nf17b_route_hypothesis(&world, world.start_cell, world.goal_cell, &profile);
    expect_true(route.next_cell >= 0, "bounded route chooses legal first step");
    expect_true(nf_spatial_is_valid_cell(&world, route.next_cell), "bounded route respects material truth");

    Nf17bTrailField trail;
    nf17b_trail_init(&trail, NF_SPATIAL_ACTOR_PLAYER, false);
    nf17b_trail_deposit(&trail, world.start_cell, 0.8f);
    const float before = trail.value[world.start_cell];
    nf17b_trail_decay(&trail, 0.5f);
    expect_true(before > trail.value[world.start_cell], "trail decays");
    expect_true(trail.shared == 0u, "actor-private trail remains private");

    expect_true(fabsf(nf17b_fixed_to_float(nf17b_fixed_from_float(3.125f)) - 3.125f) < 0.0011f,
                "fixed-point roundtrip");

    if (failures != 0) {
        fprintf(stderr, "nightfall v1.7B hybrid contracts: FAIL (%d)\n", failures);
        return 1;
    }

    printf("nightfall v1.7B hybrid contracts: PASS\n");
    printf("LOCK snapshot=PASS transaction=PASS compensation=PASS CBC=PASS frontier=PASS refinement=PASS route=PASS trail=PASS fixed=PASS\n");
    return 0;
}
