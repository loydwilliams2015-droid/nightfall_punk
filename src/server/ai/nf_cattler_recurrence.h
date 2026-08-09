#ifndef NF_CATTLER_RECURRENCE_H
#define NF_CATTLER_RECURRENCE_H

#include "nf_cattler.h"
#include "nf_world.h"

#include <stdbool.h>
#include <stdint.h>

#define NF_CATTLER_RECOLONIZE_BASE_TICKS (NF_TICK_RATE * 8u)
#define NF_CATTLER_RECOLONIZE_VARIANCE_TICKS (NF_TICK_RATE * 8u)
#define NF_CATTLER_RESEED_BASE_TICKS (NF_TICK_RATE * 45u)
#define NF_CATTLER_RESEED_VARIANCE_TICKS (NF_TICK_RATE * 46u)
#define NF_CATTLER_PLAYER_OBSERVE_RANGE 55.0f

typedef enum NfCattlerReturnKind {
    NF_CATTLER_RETURN_NONE = 0,
    NF_CATTLER_RETURN_RECOLONIZE,
    NF_CATTLER_RETURN_EXTINCTION_RESEED
} NfCattlerReturnKind;

typedef struct NfCattlerRecurrenceAgent {
    bool dead_seen;
    uint8_t death_region;
    uint64_t dead_since_tick;
} NfCattlerRecurrenceAgent;

typedef struct NfCattlerRecurrenceSystem {
    uint32_t seed;
    uint64_t last_player_seen[NF_REGION_MAX];
    NfCattlerRecurrenceAgent agents[NF_CATTLER_MAX_AGENTS];
    bool global_zero_active;
    uint64_t global_zero_since_tick;
} NfCattlerRecurrenceSystem;

void nf_cattler_recurrence_init(
    NfCattlerRecurrenceSystem *recurrence,
    uint32_t seed);

void nf_cattler_recurrence_tick(
    NfCattlerRecurrenceSystem *recurrence,
    const NfCattlerSystem *cattlers,
    const NfWorld *world);

bool nf_cattler_recurrence_spawn_for(
    const NfCattlerRecurrenceSystem *recurrence,
    const NfCattlerSystem *cattlers,
    const NfWorld *world,
    NfEntityId actor_id,
    NfVec3 *out,
    NfCattlerReturnKind *kind_out,
    uint8_t *region_out);

void nf_cattler_recurrence_commit(
    NfCattlerRecurrenceSystem *recurrence,
    NfCattlerSystem *cattlers,
    NfEntityId actor_id,
    NfVec3 position,
    uint8_t region);

size_t nf_cattler_living_count(
    const NfCattlerSystem *cattlers,
    const NfWorld *world);

const char *nf_cattler_return_name(NfCattlerReturnKind kind);

#endif
