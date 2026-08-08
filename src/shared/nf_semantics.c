#include "nf_semantics.h"

#include <math.h>
#include <string.h>

static float distance3(NfVec3 a, NfVec3 b) {
    const float x = a.x - b.x;
    const float y = a.y - b.y;
    const float z = a.z - b.z;
    return sqrtf(x*x + y*y + z*z);
}

void nf_semantic_bus_init(NfSemanticBus *bus) {
    if (bus == NULL) return;
    memset(bus, 0, sizeof(*bus));
}

void nf_semantic_bus_publish(NfSemanticBus *bus, NfSemanticAlert alert) {
    if (bus == NULL || alert.type == NF_SEMANTIC_NONE) return;
    bus->alerts[bus->next] = alert;
    bus->next = (bus->next + 1u) % NF_SEMANTIC_CAPACITY;
    if (bus->count < NF_SEMANTIC_CAPACITY) ++bus->count;
}

bool nf_semantic_alert_is_live(const NfSemanticAlert *alert, uint64_t now_tick) {
    if (alert == NULL || alert->type == NF_SEMANTIC_NONE || now_tick < alert->tick) return false;
    return now_tick - alert->tick <= (uint64_t)alert->lifetime_ticks;
}

size_t nf_semantic_collect_audible(const NfSemanticBus *bus, NfVec3 observer, uint64_t now_tick, NfSemanticAlert *out, size_t cap) {
    if (bus == NULL || out == NULL || cap == 0u) return 0u;
    size_t written = 0u;
    for (size_t n = 0u; n < bus->count && written < cap; ++n) {
        const size_t index = (bus->next + NF_SEMANTIC_CAPACITY - 1u - n) % NF_SEMANTIC_CAPACITY;
        const NfSemanticAlert *alert = &bus->alerts[index];
        if (!nf_semantic_alert_is_live(alert, now_tick) || alert->radius <= 0.0f) continue;
        if (distance3(observer, alert->position) > alert->radius) continue;
        out[written++] = *alert;
    }
    return written;
}

NfSemanticAlert nf_semantic_from_combat(const NfCombatEvent *event) {
    if (event == NULL) return (NfSemanticAlert){0};
    NfSemanticAlert alert = {
        .source = event->source,
        .subject = event->target,
        .position = event->position,
        .intensity = 1.0f,
        .tick = event->server_tick,
        .lifetime_ticks = NF_TICK_RATE * 2u
    };
    switch (event->type) {
        case NF_COMBAT_EVENT_GUNFIRE:
            alert.type = NF_SEMANTIC_GUNFIRE;
            alert.radius = 30.0f;
            alert.lifetime_ticks = NF_TICK_RATE;
            break;
        case NF_COMBAT_EVENT_DAMAGE:
            alert.type = NF_SEMANTIC_DAMAGE_TAKEN;
            alert.radius = 18.0f;
            break;
        case NF_COMBAT_EVENT_DEATH:
            alert.type = NF_SEMANTIC_ACTOR_DIED;
            alert.radius = 24.0f;
            alert.lifetime_ticks = NF_TICK_RATE * 3u;
            break;
        case NF_COMBAT_EVENT_RESPAWN:
            alert.type = NF_SEMANTIC_ACTOR_RESPAWNED;
            alert.radius = 12.0f;
            break;
        default:
            alert.type = NF_SEMANTIC_NONE;
            alert.radius = 0.0f;
            break;
    }
    return alert;
}

const char *nf_semantic_type_name(NfSemanticType type) {
    switch (type) {
        case NF_SEMANTIC_GUNFIRE: return "GUNFIRE";
        case NF_SEMANTIC_DAMAGE_TAKEN: return "DAMAGE_TAKEN";
        case NF_SEMANTIC_ACTOR_DIED: return "ACTOR_DIED";
        case NF_SEMANTIC_ACTOR_RESPAWNED: return "ACTOR_RESPAWNED";
        case NF_SEMANTIC_ENEMY_SEEN: return "ENEMY_SEEN";
        case NF_SEMANTIC_ENEMY_LOST: return "ENEMY_LOST";
        case NF_SEMANTIC_SUSPICIOUS_SOUND: return "SUSPICIOUS_SOUND";
        case NF_SEMANTIC_AFFORDANCE_RESERVED: return "AFFORDANCE_RESERVED";
        case NF_SEMANTIC_AFFORDANCE_RELEASED: return "AFFORDANCE_RELEASED";
        default: return "NONE";
    }
}
