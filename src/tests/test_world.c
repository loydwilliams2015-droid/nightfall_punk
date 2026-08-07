#include "nf_world.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    NfWorld world;
    nf_world_init(&world, 42u);
    assert(world.tick == 0u);
    assert(nf_world_active_actor_count(&world) == 0u);

    const NfEntityId id = nf_world_spawn_actor(&world, NF_FACTION_RIVAL, (NfVec3){1.0f, 2.0f, 3.0f});
    assert(id != 0u);
    assert(nf_world_active_actor_count(&world) == 1u);

    NfActor *actor = nf_world_find_actor(&world, id);
    assert(actor != NULL);
    actor->transform.velocity = (NfVec3){2.0f, 0.0f, 0.0f};
    nf_world_step(&world, 0.5f);
    assert(world.tick == 1u);
    assert(actor->transform.position.x == 2.0f);

    puts("world_smoke: PASS");
    return 0;
}
