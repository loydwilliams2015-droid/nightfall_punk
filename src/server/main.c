#include "nf_world.h"

#include <stdio.h>

int main(void) {
    NfWorld world;
    nf_world_init(&world, 20260807u);
    nf_world_spawn_actor(&world, NF_FACTION_PLAYER, (NfVec3){0.0f, 0.0f, 0.0f});

    printf("nightfall!punk dedicated server v0.1.0\n");
    printf("tick_rate=%u seed=%u actors=%zu\n", NF_TICK_RATE, world.seed, nf_world_active_actor_count(&world));

    for (unsigned i = 0; i < NF_TICK_RATE; ++i) {
        nf_world_step(&world, 1.0f / (float)NF_TICK_RATE);
    }

    printf("simulation_tick=%llu\n", (unsigned long long)world.tick);
    return 0;
}
