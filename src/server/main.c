#include "nf_world.h"

#include <stdio.h>

int main(void) {
    NfWorld world;
    nf_world_init(&world, 20260807u);
    nf_world_build_movement_lab(&world);
    const NfEntityId player = nf_world_spawn_actor(&world, NF_FACTION_PLAYER, (NfVec3){0.0f, 0.05f, -18.0f});

    printf("nightfall!punk dedicated server v0.2\n");
    printf("tick_rate=%u seed=%u actors=%zu colliders=%zu ramps=%zu\n", NF_TICK_RATE, world.seed, nf_world_active_actor_count(&world), world.collider_count, world.ramp_count);

    NfMoveInput input = {.forward = 1.0f, .sprint_held = true};
    nf_world_set_input(&world, player, input);
    for (unsigned i = 0; i < NF_TICK_RATE; ++i) nf_world_step(&world, 1.0f / (float)NF_TICK_RATE);

    const NfActor *actor = nf_world_find_actor_const(&world, player);
    printf("simulation_tick=%llu mode=%s pos=(%.2f, %.2f, %.2f)\n",
        (unsigned long long)world.tick,
        actor != NULL ? nf_movement_mode_name(actor->movement.mode) : "MISSING",
        actor != NULL ? actor->transform.position.x : 0.0f,
        actor != NULL ? actor->transform.position.y : 0.0f,
        actor != NULL ? actor->transform.position.z : 0.0f);
    return 0;
}
