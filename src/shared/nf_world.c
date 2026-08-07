#include "nf_world.h"

#include <string.h>

void nf_world_init(NfWorld *world, uint32_t seed) {
    if (world == NULL) return;
    memset(world, 0, sizeof(*world));
    world->seed = seed;
    world->next_entity_id = 1u;
}

NfEntityId nf_world_spawn_actor(NfWorld *world, NfFaction faction, NfVec3 position) {
    if (world == NULL) return 0u;
    for (size_t i = 0; i < NF_MAX_ENTITIES; ++i) {
        NfActor *actor = &world->actors[i];
        if (!actor->active) {
            actor->active = true;
            actor->id = world->next_entity_id++;
            actor->faction = faction;
            actor->transform.position = position;
            actor->transform.velocity = (NfVec3){0};
            actor->health = 100.0f;
            return actor->id;
        }
    }
    return 0u;
}

NfActor *nf_world_find_actor(NfWorld *world, NfEntityId id) {
    if (world == NULL || id == 0u) return NULL;
    for (size_t i = 0; i < NF_MAX_ENTITIES; ++i) {
        if (world->actors[i].active && world->actors[i].id == id) return &world->actors[i];
    }
    return NULL;
}

const NfActor *nf_world_find_actor_const(const NfWorld *world, NfEntityId id) {
    if (world == NULL || id == 0u) return NULL;
    for (size_t i = 0; i < NF_MAX_ENTITIES; ++i) {
        if (world->actors[i].active && world->actors[i].id == id) return &world->actors[i];
    }
    return NULL;
}

void nf_world_step(NfWorld *world, float dt) {
    if (world == NULL || dt <= 0.0f) return;
    for (size_t i = 0; i < NF_MAX_ENTITIES; ++i) {
        NfActor *actor = &world->actors[i];
        if (!actor->active) continue;
        actor->transform.position.x += actor->transform.velocity.x * dt;
        actor->transform.position.y += actor->transform.velocity.y * dt;
        actor->transform.position.z += actor->transform.velocity.z * dt;
    }
    ++world->tick;
}

size_t nf_world_active_actor_count(const NfWorld *world) {
    if (world == NULL) return 0u;
    size_t count = 0u;
    for (size_t i = 0; i < NF_MAX_ENTITIES; ++i) if (world->actors[i].active) ++count;
    return count;
}
