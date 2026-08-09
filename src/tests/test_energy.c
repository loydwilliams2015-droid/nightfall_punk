#include "nf_energy.h"
#include "nf_region.h"
#include "nf_world.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

static bool nearly_equal(double a, double b, double epsilon) {
    return fabs(a - b) <= epsilon;
}

static void accounting_invariants(void) {
    NfWorld world;
    nf_world_init(&world, 20260808u);
    nf_world_build_movement_lab(&world);
    assert(world.energy.bound);

    const double before_transfer = nf_energy_total_stored(&world.energy);
    const double moved = nf_energy_transfer(&world.energy, 0u, 1u, 5.0);
    const double after_transfer = nf_energy_total_stored(&world.energy);
    assert(moved > 0.0);
    assert(nearly_equal(before_transfer, after_transfer, 1e-8));

    const double before_conversion = nf_energy_total_stored(&world.energy);
    const double loss_before = world.energy.total_conversion_loss;
    const double useful = nf_energy_convert(&world.energy, 0u, 5.0, 0.80);
    const double after_conversion = nf_energy_total_stored(&world.energy);
    const double conversion_loss = world.energy.total_conversion_loss - loss_before;
    assert(useful > 0.0);
    assert(conversion_loss > 0.0);
    assert(nearly_equal(before_conversion - after_conversion, conversion_loss, 1e-8));

    const double before_capacity = nf_energy_total_stored(&world.energy);
    const double capacity_delta = nf_energy_change_capacity(&world.energy, 1u, 0.10);
    const double after_capacity = nf_energy_total_stored(&world.energy);
    assert(capacity_delta > 0.0);
    assert(nearly_equal(before_capacity, after_capacity, 1e-8));
}

static void topography_and_history_contract(void) {
    NfWorld world;
    nf_world_init(&world, 20260808u);
    nf_world_build_movement_lab(&world);

    NfRegionGraph graph;
    nf_region_graph_init_spatial_lab(&graph);

    const uint8_t lower_commons = 9u;
    const uint8_t south_works = 2u;
    const uint8_t signal_court = 14u;

    const double south_cost = nf_energy_topographic_cost(
        &world.energy, &graph, lower_commons, south_works);
    const double signal_cost = nf_energy_topographic_cost(
        &world.energy, &graph, lower_commons, signal_court);
    assert(signal_cost < south_cost);

    const NfEnergyRegion *south_before = nf_energy_region_const(&world.energy, south_works);
    const NfEnergyRegion *signal_before = nf_energy_region_const(&world.energy, signal_court);
    assert(south_before != NULL && signal_before != NULL);
    const double south_usable_before = south_before->usable;
    const double south_health_before = south_before->health;
    const double signal_usable_before = signal_before->usable;

    NfEnergyBelief stale;
    nf_energy_belief_init(&stale, &world.energy);
    nf_energy_belief_observe(&stale, &world.energy, south_works, 1.0, 1u);

    for (size_t i = 0u; i < 3u; ++i) {
        NfVec3 spawn = graph.regions[south_works].center;
        spawn.x += (float)i * 1.2f;
        spawn.y = 0.05f;
        const NfEntityId id = nf_world_spawn_actor(&world, NF_FACTION_RIVAL, spawn);
        assert(id != 0u);
    }

    for (unsigned step = 0u; step < NF_TICK_RATE * 30u; ++step) {
        nf_world_step(&world, 1.0f / (float)NF_TICK_RATE);
    }

    const NfEnergyRegion *south_after = nf_energy_region_const(&world.energy, south_works);
    const NfEnergyRegion *signal_after = nf_energy_region_const(&world.energy, signal_court);
    assert(south_after != NULL && signal_after != NULL);
    assert(south_after->usable < south_usable_before * 0.35);
    assert(south_after->health < south_health_before);
    assert(signal_after->usable > signal_usable_before);
    assert(south_after->cumulative_extraction > south_after->cumulative_regeneration);

    const NfEnergyAccount *first_account = nf_energy_account_const(&world.energy, 1u);
    assert(first_account != NULL);
    assert(first_account->income > 0.0);

    NfEnergyBelief fresh;
    nf_energy_belief_init(&fresh, &world.energy);
    nf_energy_belief_observe(
        &fresh, &world.energy, south_works, 1.0, world.tick);
    nf_energy_belief_observe(
        &fresh, &world.energy, signal_court, 1.0, world.tick);
    nf_energy_belief_decay(&stale, world.tick);

    const double stale_south = nf_energy_belief_candidate_score(
        &stale, &world.energy, &graph, lower_commons,
        south_works, NF_ENERGY_HORIZON_REGIONAL);
    const double fresh_south = nf_energy_belief_candidate_score(
        &fresh, &world.energy, &graph, lower_commons,
        south_works, NF_ENERGY_HORIZON_REGIONAL);
    const double fresh_signal = nf_energy_belief_candidate_score(
        &fresh, &world.energy, &graph, lower_commons,
        signal_court, NF_ENERGY_HORIZON_REGIONAL);
    assert(stale_south > fresh_south);
    assert(fresh_signal > fresh_south);

    NfEnergyOpportunity top[NF_ENERGY_TOP_CANDIDATES] = {0};
    const size_t count = nf_energy_helicopter_search(
        &world.energy, &graph, lower_commons, top, NF_ENERGY_TOP_CANDIDATES);
    assert(count == NF_ENERGY_TOP_CANDIDATES);
    for (size_t i = 0u; i < count; ++i) {
        assert(top[i].region < NF_ENERGY_REGION_MAX);
        if (i > 0u) assert(top[i - 1u].score >= top[i].score);
    }

    printf("[energy] South Works usable %.2f -> %.2f health %.3f -> %.3f topographic-cost=%.2f\n",
        south_usable_before, south_after->usable,
        south_health_before, south_after->health, south_cost);
    printf("[energy] Signal Court usable %.2f -> %.2f topographic-cost=%.2f\n",
        signal_usable_before, signal_after->usable, signal_cost);
    printf("[belief] stale South=%.2f fresh South=%.2f fresh Signal=%.2f\n",
        stale_south, fresh_south, fresh_signal);
    for (size_t i = 0u; i < count; ++i) {
        printf("[helicopter] rank=%zu region=%u:%s score=%.2f posterior=%.3f uncertainty=%.3f cost=%.2f\n",
            i + 1u, top[i].region, nf_region_name(top[i].region),
            top[i].score, top[i].posterior_mean, top[i].uncertainty,
            top[i].topographic_cost);
    }
}

int main(void) {
    accounting_invariants();
    topography_and_history_contract();
    puts("nightfall v0.9 topographical energy accounting tests: PASS");
    return 0;
}
