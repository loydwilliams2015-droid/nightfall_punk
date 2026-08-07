#ifndef NF_WORLD_H
#define NF_WORLD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_MAX_ENTITIES 1024u
#define NF_TICK_RATE 60u

typedef uint32_t NfEntityId;

typedef enum NfFaction {
    NF_FACTION_NONE = 0,
    NF_FACTION_PLAYER,
    NF_FACTION_TEAMMATE,
    NF_FACTION_RIVAL,
    NF_FACTION_RANCHER
} NfFaction;

typedef struct NfVec3 { float x, y, z; } NfVec3;
typedef struct NfTransform { NfVec3 position; NfVec3 velocity; } NfTransform;
typedef struct NfActor {
    NfEntityId id;
    bool active;
    NfFaction faction;
    NfTransform transform;
    float health;
} NfActor;

typedef struct NfWorld {
    uint64_t tick;
    uint32_t seed;
    NfEntityId next_entity_id;
    NfActor actors[NF_MAX_ENTITIES];
} NfWorld;

void nf_world_init(NfWorld *world, uint32_t seed);
NfEntityId nf_world_spawn_actor(NfWorld *world, NfFaction faction, NfVec3 position);
NfActor *nf_world_find_actor(NfWorld *world, NfEntityId id);
const NfActor *nf_world_find_actor_const(const NfWorld *world, NfEntityId id);
void nf_world_step(NfWorld *world, float dt);
size_t nf_world_active_actor_count(const NfWorld *world);

#endif
