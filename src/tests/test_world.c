#include "nf_world.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

static void step_n(NfWorld *world, unsigned ticks) {
    for (unsigned i = 0; i < ticks; ++i) nf_world_step(world, 1.0f / (float)NF_TICK_RATE);
}

static NfActor *spawn_test_actor(NfWorld *world, NfVec3 position) {
    const NfEntityId id = nf_world_spawn_actor(world, NF_FACTION_PLAYER, position);
    assert(id != 0u);
    NfActor *actor = nf_world_find_actor(world, id);
    assert(actor != NULL);
    return actor;
}

static void test_world_smoke(void) {
    NfWorld world; nf_world_init(&world, 42u);
    assert(world.tick == 0u); assert(nf_world_active_actor_count(&world) == 0u);
    NfActor *actor = spawn_test_actor(&world, (NfVec3){1.0f, 2.0f, 3.0f});
    actor->transform.velocity = (NfVec3){2.0f, 0.0f, 0.0f};
    nf_world_step(&world, 0.5f);
    assert(world.tick == 1u); assert(fabsf(actor->transform.position.x - 2.0f) < 0.01f);
}

static void test_gravity_and_ground(void) {
    NfWorld world; nf_world_init(&world, 1u);
    nf_world_add_collider(&world, NF_COLLIDER_SOLID, (NfVec3){-20.0f, -0.5f, -20.0f}, (NfVec3){20.0f, 0.0f, 20.0f});
    NfActor *actor = spawn_test_actor(&world, (NfVec3){0.0f, 3.0f, 0.0f});
    step_n(&world, 180u);
    assert(actor->movement.grounded); assert(fabsf(actor->transform.position.y) < 0.02f);
}

static void test_walk_and_sprint(void) {
    NfWorld walk_world; nf_world_init(&walk_world, 2u);
    nf_world_add_collider(&walk_world, NF_COLLIDER_SOLID, (NfVec3){-50.0f, -0.5f, -50.0f}, (NfVec3){50.0f, 0.0f, 50.0f});
    NfActor *walker = spawn_test_actor(&walk_world, (NfVec3){0.0f, 0.0f, 0.0f});
    walker->movement.grounded = true; walker->input.forward = 1.0f; step_n(&walk_world, 60u);
    const float walk_z = walker->transform.position.z;

    NfWorld sprint_world; nf_world_init(&sprint_world, 3u);
    nf_world_add_collider(&sprint_world, NF_COLLIDER_SOLID, (NfVec3){-50.0f, -0.5f, -50.0f}, (NfVec3){50.0f, 0.0f, 50.0f});
    NfActor *sprinter = spawn_test_actor(&sprint_world, (NfVec3){0.0f, 0.0f, 0.0f});
    sprinter->movement.grounded = true; sprinter->input.forward = 1.0f; sprinter->input.sprint_held = true; step_n(&sprint_world, 60u);
    assert(sprinter->transform.position.z > walk_z + 1.0f);
}

static void test_jump_and_crouch_second_jump(void) {
    NfWorld world; nf_world_init(&world, 4u);
    nf_world_add_collider(&world, NF_COLLIDER_SOLID, (NfVec3){-20.0f, -0.5f, -20.0f}, (NfVec3){20.0f, 0.0f, 20.0f});
    NfActor *actor = spawn_test_actor(&world, (NfVec3){0.0f, 0.0f, 0.0f}); actor->movement.grounded = true;
    actor->input.jump_pressed = true; nf_world_step(&world, 1.0f / (float)NF_TICK_RATE);
    assert(!actor->movement.grounded); assert(actor->movement.jumps_used == 1u); assert(actor->transform.velocity.y > 0.0f);
    actor->input.jump_pressed = true; actor->input.crouch_held = false; nf_world_step(&world, 1.0f / (float)NF_TICK_RATE);
    assert(actor->movement.jumps_used == 1u);
    actor->input.jump_pressed = true; actor->input.crouch_held = true; nf_world_step(&world, 1.0f / (float)NF_TICK_RATE);
    assert(actor->movement.jumps_used == 2u); assert(actor->movement.crouched);
}

static void test_fuzzy_rail_candidate(void) {
    NfWorld world; nf_world_init(&world, 5u);
    nf_world_add_collider(&world, NF_COLLIDER_SOLID, (NfVec3){-20.0f, -0.5f, -20.0f}, (NfVec3){20.0f, 0.0f, 20.0f});
    nf_world_add_collider(&world, NF_COLLIDER_SOLID, (NfVec3){-1.0f, 0.0f, 1.0f}, (NfVec3){1.0f, 0.9f, 1.5f});
    NfActor *actor = spawn_test_actor(&world, (NfVec3){0.0f, 0.0f, 0.0f}); actor->movement.grounded = true; actor->input.forward = 1.0f;
    nf_world_step(&world, 1.0f / (float)NF_TICK_RATE);
    assert(actor->movement.candidate.active); assert(actor->movement.candidate.type == NF_TRAVERSAL_VAULT);
}

static void test_crouch_clearance(void) {
    NfWorld world; nf_world_init(&world, 6u);
    nf_world_add_collider(&world, NF_COLLIDER_SOLID, (NfVec3){-20.0f, -0.5f, -20.0f}, (NfVec3){20.0f, 0.0f, 20.0f});
    nf_world_add_collider(&world, NF_COLLIDER_SOLID, (NfVec3){-2.0f, 1.25f, -2.0f}, (NfVec3){2.0f, 1.6f, 2.0f});
    NfActor *actor = spawn_test_actor(&world, (NfVec3){0.0f, 0.0f, 0.0f}); actor->movement.grounded = true; actor->input.crouch_held = true;
    nf_world_step(&world, 1.0f / (float)NF_TICK_RATE); assert(actor->movement.crouched);
    actor->input.crouch_held = false; nf_world_step(&world, 1.0f / (float)NF_TICK_RATE); assert(actor->movement.crouched);
}

int main(void) {
    test_world_smoke(); test_gravity_and_ground(); test_walk_and_sprint(); test_jump_and_crouch_second_jump(); test_fuzzy_rail_candidate(); test_crouch_clearance();
    puts("nightfall v0.2 simulation tests: PASS"); return 0;
}
