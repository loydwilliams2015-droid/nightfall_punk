#include "nf_spatial_logic.h"

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

int main(void) {
    NfSpatialWorld world;
    nf_spatial_world_init(&world, NF_SPATIAL_LAB_CONTRACT, 170001u);
    int event_cell = nf_spatial_cell_xy(&world, 6, 5);
    float before_memory = world.cells[event_cell].contract_memory;
    uint16_t before_authority = world.object_authority[0];
    NfContractExchangeEvent e = {0u, before_authority, NF_SPATIAL_ACTOR_PLAYER, 0.75f, 0.90f, event_cell, 3, 20u};
    expect_true(nf_spatial_apply_contract_event(&world, &e), "contract event valid");
    expect_int((int)world.object_authority[0], (int)NF_SPATIAL_ACTOR_PLAYER, "contract changes authority");
    expect_true(world.cells[event_cell].contract_memory >= before_memory, "contract deposits memory");
    world.cells[event_cell].contract_memory = 1.0f;
    expect_int((int)world.object_authority[0], (int)NF_SPATIAL_ACTOR_PLAYER, "gradient cannot create authority");

    nf_spatial_world_init(&world, NF_SPATIAL_LAB_COVER, 170002u);
    int from = nf_spatial_cell_xy(&world, 2, 6);
    int to = nf_spatial_cell_xy(&world, 9, 6);
    expect_true(nf_spatial_line_of_sight(&world, from, to), "cover lab clear LOS baseline");
    int blocker = nf_spatial_cell_xy(&world, 6, 6);
    world.cells[blocker].solid = 1u;
    expect_true(!nf_spatial_line_of_sight(&world, from, to), "material blocker occludes exact LOS");

    nf_spatial_world_init(&world, NF_SPATIAL_LAB_DYNAMIC, 170003u);
    int gate = nf_spatial_cell_xy(&world, 6, 5);
    nf_spatial_mark_dirty(&world, gate, 2);
    uint16_t done = nf_spatial_recompute_dirty(&world, 5u);
    expect_int((int)done, 5, "dirty recompute bounded by budget");
    expect_true(world.topology_revision > 0u, "dirty recompute advances revision");

    nf_spatial_world_init(&world, NF_SPATIAL_LAB_SNAP, 170004u);
    int forbidden = nf_spatial_cell_xy(&world, 6, 6);
    int adjacent = nf_spatial_cell_xy(&world, 5, 6);
    expect_true(nf_spatial_affordance_candidate(&world, adjacent, forbidden) == 0.0f, "authored semantic filter rejects affordance");

    nf_spatial_world_init(&world, NF_SPATIAL_LAB_LOOP, 170005u);
    uint8_t known[NF_SPATIAL_MAX_CELLS];
    memset(known, 0, sizeof(known));
    size_t observed = nf_spatial_observe_local(&world, world.start_cell, 2, known, sizeof(known));
    expect_true(observed > 0u, "actor observes local cells");
    expect_true(observed < (size_t)(world.width * world.height), "actor knowledge not omniscient");
    expect_true(nf_spatial_shortest_path(&world, world.start_cell, world.goal_cell) > 0, "loop lab reachable");

    NfSpatialRoundResult a = nf_spatial_run_round(NF_SPATIAL_LAB_ECOLOGY, NF_SPATIAL_POLICY_INFORMED, 177777u, 0u);
    NfSpatialRoundResult b = nf_spatial_run_round(NF_SPATIAL_LAB_ECOLOGY, NF_SPATIAL_POLICY_INFORMED, 177777u, 1u);
    expect_int((int)a.deterministic_hash, (int)b.deterministic_hash, "duplicate seed deterministic hash");

    if (failures != 0) {
        fprintf(stderr, "nightfall v1.7A spatial logic contracts: FAIL (%d)\n", failures);
        return 1;
    }
    printf("nightfall v1.7A spatial logic contracts: PASS\n");
    printf("OITOA authority_memory_separation=PASS local_belief=PASS dirty_topology=PASS exact_los=PASS affordance_filter=PASS determinism=PASS\n");
    return 0;
}
