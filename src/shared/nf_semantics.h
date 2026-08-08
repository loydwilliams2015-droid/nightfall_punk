#ifndef NF_SEMANTICS_H
#define NF_SEMANTICS_H

#include "nf_world.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_SEMANTIC_CAPACITY 128u

typedef enum NfSemanticType {
    NF_SEMANTIC_NONE = 0,
    NF_SEMANTIC_GUNFIRE,
    NF_SEMANTIC_DAMAGE_TAKEN,
    NF_SEMANTIC_ACTOR_DIED,
    NF_SEMANTIC_ACTOR_RESPAWNED,
    NF_SEMANTIC_ENEMY_SEEN,
    NF_SEMANTIC_ENEMY_LOST,
    NF_SEMANTIC_SUSPICIOUS_SOUND,
    NF_SEMANTIC_AFFORDANCE_RESERVED,
    NF_SEMANTIC_AFFORDANCE_RELEASED
} NfSemanticType;

typedef struct NfSemanticAlert {
    NfSemanticType type;
    NfEntityId source;
    NfEntityId subject;
    NfVec3 position;
    float intensity;
    float radius;
    uint64_t tick;
    uint32_t lifetime_ticks;
} NfSemanticAlert;

typedef struct NfSemanticBus {
    NfSemanticAlert alerts[NF_SEMANTIC_CAPACITY];
    size_t count;
    size_t next;
} NfSemanticBus;

void nf_semantic_bus_init(NfSemanticBus *bus);
void nf_semantic_bus_publish(NfSemanticBus *bus, NfSemanticAlert alert);
bool nf_semantic_alert_is_live(const NfSemanticAlert *alert, uint64_t now_tick);
size_t nf_semantic_collect_audible(const NfSemanticBus *bus, NfVec3 observer, uint64_t now_tick, NfSemanticAlert *out, size_t cap);
NfSemanticAlert nf_semantic_from_combat(const NfCombatEvent *event);
const char *nf_semantic_type_name(NfSemanticType type);

#endif
