#define _POSIX_C_SOURCE 200809L

#include "nf_agent.h"
#include "nf_ai.h"
#include "nf_cattler.h"
#include "nf_cattler_recurrence.h"
#include "nf_combat.h"
#include "nf_encounter.h"
#include "nf_hitbox.h"
#include "nf_lifeworld.h"
#include "nf_lifeworld_spatial.h"
#include "nf_net.h"
#include "nf_prediction.h"
#include "nf_region.h"
#include "nf_relations.h"
#include "nf_security.h"
#include "nf_semantics.h"
#include "nf_spatial.h"
#include "nf_weapon_geometry.h"
#include "nf_weapon_solution.h"
#include "nf_world.h"

#include <float.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NF_SERVER_CLIENTS NF_NET_MAX_PLAYERS
#define NF_HISTORY_FRAMES 64u

typedef struct NfServerClient {
    bool occupied;
    bool connected;
    void *peer;
    NfEntityId entity_id;
    uint8_t session_token[NF_NET_TOKEN_BYTES];
    uint32_t last_input_sequence;
    uint64_t current_client_tick;
    NfMoveInput current_input;
    NfCombatInput combat_input;
    uint32_t last_fresh_input_ms;
    uint32_t reserved_until_ms;
} NfServerClient;

typedef struct NfHistoryActor {
    NfEntityId id;
    NfVec3 position;
    bool crouched;
    bool alive;
} NfHistoryActor;

typedef struct NfHistoryFrame {
    bool valid;
    uint64_t tick;
    uint8_t count;
    NfHistoryActor actors[NF_NET_MAX_SNAPSHOT_ACTORS];
} NfHistoryFrame;

static volatile sig_atomic_t g_running = 1;
static uint32_t g_event_sequence = 1u;

static void on_signal(int sig) {
    (void)sig;
    g_running = 0;
}

static void sleep_ms(unsigned ms) {
    struct timespec ts = {
        (time_t)(ms / 1000u),
        (long)(ms % 1000u) * 1000000L
    };
    nanosleep(&ts, NULL);
}

static bool seq_newer(uint32_t a, uint32_t b) {
    return (int32_t)(a - b) > 0;
}

static NfVec3 vsub(NfVec3 a, NfVec3 b) {
    return (NfVec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static float vdot(NfVec3 a, NfVec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

static bool input_valid(const NfInputCommand *cmd) {
    return cmd != NULL &&
        isfinite(cmd->move.forward) &&
        isfinite(cmd->move.strafe) &&
        isfinite(cmd->move.yaw_radians) &&
        isfinite(cmd->combat.aim_pitch_radians) &&
        fabsf(cmd->move.forward) <= 1.01f &&
        fabsf(cmd->move.strafe) <= 1.01f &&
        fabsf(cmd->move.yaw_radians) < 100000.0f &&
        fabsf(cmd->combat.aim_pitch_radians) <= 1.50f &&
        cmd->combat.weapon_slot < NF_WEAPON_COUNT;
}

static NfServerClient *by_peer(NfServerClient clients[], void *peer) {
    for (size_t i = 0; i < NF_SERVER_CLIENTS; ++i) {
        if (clients[i].occupied &&
            clients[i].connected &&
            clients[i].peer == peer) {
            return &clients[i];
        }
    }
    return NULL;
}

static NfServerClient *by_token(
    NfServerClient clients[],
    const uint8_t token[NF_NET_TOKEN_BYTES],
    uint32_t now) {
    for (size_t i = 0; i < NF_SERVER_CLIENTS; ++i) {
        if (clients[i].occupied &&
            !clients[i].connected &&
            (int32_t)(clients[i].reserved_until_ms - now) > 0 &&
            nf_security_token_equal(clients[i].session_token, token)) {
            return &clients[i];
        }
    }
    return NULL;
}

static NfServerClient *free_slot(NfServerClient clients[]) {
    for (size_t i = 0; i < NF_SERVER_CLIENTS; ++i) {
        if (!clients[i].occupied) return &clients[i];
    }
    return NULL;
}

static size_t slot_index(NfServerClient clients[], NfServerClient *client) {
    return (size_t)(client - clients);
}

static NfVec3 spawn_for_slot(size_t slot) {
    static const NfVec3 positions[NF_SERVER_CLIENTS] = {
        {-3.0f, 0.05f, -18.0f},
        {3.0f, 0.05f, 18.0f},
        {3.0f, 0.05f, -18.0f},
        {-3.0f, 0.05f, 18.0f}
    };
    return positions[slot % NF_SERVER_CLIENTS];
}

static NfFaction faction_for_slot(size_t slot) {
    return slot == 0u ? NF_FACTION_PLAYER : NF_FACTION_TEAMMATE;
}

static void reset_movement_after_respawn(NfWorld *world, NfActor *actor) {
    const bool cattler = actor->faction == NF_FACTION_RANCHER;
    actor->movement = (NfMovementState){0};
    actor->movement.mode = NF_MOVE_AIR;
    actor->movement.body_height = cattler
        ? NF_CATTLER_BODY_HEIGHT
        : world->movement.stand_height;
    actor->movement.eye_height = cattler
        ? NF_CATTLER_EYE_HEIGHT
        : world->movement.stand_eye_height;
    actor->movement.attached_collider = -1;
    actor->movement.ground_collider = -1;
    actor->movement.candidate.feature_index = -1;
    actor->weapon_authority.kinematic_initialized = false;
}

static bool ordinary_respawn_position(
    const NfServerClient clients[],
    const NfAiSystem *ai,
    NfEntityId id,
    NfVec3 *out) {
    for (size_t i = 0; i < NF_SERVER_CLIENTS; ++i) {
        if (clients[i].occupied && clients[i].entity_id == id) {
            *out = spawn_for_slot(i);
            return true;
        }
    }
    return nf_ai_spawn_for(ai, id, out);
}

static void expire_reservations(
    NfWorld *world,
    NfServerClient clients[],
    uint32_t now) {
    for (size_t i = 0; i < NF_SERVER_CLIENTS; ++i) {
        NfServerClient *client = &clients[i];
        if (client->occupied &&
            !client->connected &&
            (int32_t)(now - client->reserved_until_ms) >= 0) {
            nf_world_despawn_actor(world, client->entity_id);
            memset(client, 0, sizeof(*client));
        }
    }
}

static void send_reject(NfNetHost *net, void *peer, uint8_t reason) {
    uint8_t buffer[32];
    const size_t encoded = nf_protocol_encode_reject(
        buffer, sizeof(buffer), reason);
    if (encoded != 0u) {
        nf_net_send(
            net,
            peer,
            NF_NET_CHANNEL_RELIABLE,
            buffer,
            encoded,
            true);
    }
}

static void broadcast_event(
    NfNetHost *net,
    NfServerClient clients[],
    NfSemanticBus *semantics,
    NfCombatEvent event) {
    event.sequence = g_event_sequence++;
    const NfSemanticAlert alert = nf_semantic_from_combat(&event);
    if (alert.type != NF_SEMANTIC_NONE) {
        nf_semantic_bus_publish(semantics, alert);
    }

    NfCombatEventMessage message = {.event = event};
    uint8_t buffer[128];
    const size_t encoded = nf_protocol_encode_combat_event(
        buffer, sizeof(buffer), &message);
    if (encoded == 0u) return;

    const bool reliable =
        event.type == NF_COMBAT_EVENT_DEATH ||
        event.type == NF_COMBAT_EVENT_RESPAWN ||
        event.type == NF_COMBAT_EVENT_RELOAD ||
        event.type == NF_COMBAT_EVENT_WEAPON_SWITCH;
    for (size_t i = 0; i < NF_SERVER_CLIENTS; ++i) {
        if (clients[i].occupied && clients[i].connected) {
            nf_net_send(
                net,
                clients[i].peer,
                reliable ? NF_NET_CHANNEL_RELIABLE : NF_NET_CHANNEL_STATE,
                buffer,
                encoded,
                reliable);
        }
    }
}

static void handle_hello(
    NfNetHost *net,
    NfWorld *world,
    NfServerClient clients[],
    void *peer,
    const uint8_t *data,
    size_t size) {
    NfHelloMessage hello_message;
    if (!nf_protocol_decode_hello(data, size, &hello_message)) {
        send_reject(net, peer, 1u);
        return;
    }

    const uint32_t now = nf_net_now_ms();
    NfServerClient *client = NULL;
    bool resumed = false;
    if (!nf_security_token_is_zero(hello_message.resume_token)) {
        client = by_token(clients, hello_message.resume_token, now);
        resumed = client != NULL;
    }
    if (client == NULL) client = free_slot(clients);
    if (client == NULL) {
        send_reject(net, peer, 2u);
        return;
    }

    if (!resumed) {
        const size_t index = slot_index(clients, client);
        memset(client, 0, sizeof(*client));
        client->occupied = true;
        client->entity_id = nf_world_spawn_actor(
            world, faction_for_slot(index), spawn_for_slot(index));
        uint8_t server_nonce[NF_NET_NONCE_BYTES];
        nf_security_random(server_nonce, sizeof(server_nonce));
        nf_security_derive_token(
            hello_message.client_nonce,
            server_nonce,
            client->session_token);
    }

    client->connected = true;
    client->peer = peer;
    client->reserved_until_ms = 0u;
    client->last_fresh_input_ms = now;

    NfWelcomeMessage welcome = {
        .entity_id = client->entity_id,
        .server_tick = world->tick,
        .world_seed = world->seed,
        .strong_crypto = nf_security_is_strong()
    };
    memcpy(
        welcome.session_token,
        client->session_token,
        NF_NET_TOKEN_BYTES);
    uint8_t buffer[128];
    const size_t encoded = nf_protocol_encode_welcome(
        buffer, sizeof(buffer), &welcome);
    if (encoded != 0u) {
        nf_net_send(
            net,
            peer,
            NF_NET_CHANNEL_RELIABLE,
            buffer,
            encoded,
            true);
    }

    printf(
        "[v16b-server] %s entity=%u actors=%zu\n",
        resumed ? "resumed" : "welcome",
        client->entity_id,
        nf_world_active_actor_count(world));
}

static void handle_input(
    NfServerClient *client,
    const uint8_t *data,
    size_t size) {
    NfInputBundle bundle;
    if (client == NULL ||
        !nf_protocol_decode_input(data, size, &bundle) ||
        !nf_security_token_equal(
            client->session_token, bundle.session_token)) {
        return;
    }

    bool jump = false;
    bool fire_pressed = false;
    bool reload_pressed = false;
    uint8_t weapon_slot = 0u;
    bool accepted = false;

    for (uint8_t i = 0u; i < bundle.count; ++i) {
        NfInputCommand *command = &bundle.commands[i];
        if (!input_valid(command) ||
            !seq_newer(command->sequence, client->last_input_sequence)) {
            continue;
        }

        accepted = true;
        jump = jump || command->move.jump_pressed;
        fire_pressed = fire_pressed || command->combat.fire_pressed;
        reload_pressed = reload_pressed || command->combat.reload_pressed;
        if (command->combat.weapon_slot != 0u) {
            weapon_slot = command->combat.weapon_slot;
        }
        client->current_input = command->move;
        client->combat_input = command->combat;
        client->current_client_tick = command->client_tick;
        client->last_input_sequence = command->sequence;
    }

    if (jump) client->current_input.jump_pressed = true;
    if (fire_pressed) client->combat_input.fire_pressed = true;
    if (reload_pressed) client->combat_input.reload_pressed = true;
    if (weapon_slot != 0u) client->combat_input.weapon_slot = weapon_slot;
    if (accepted) client->last_fresh_input_ms = nf_net_now_ms();
}

static bool client_input_fresh(
    const NfServerClient *client,
    uint32_t now) {
    return client != NULL &&
        client->connected &&
        (uint32_t)(now - client->last_fresh_input_ms) <=
            NF_NET_STALE_HELD_INPUT_MS;
}

static void send_snapshot(
    NfNetHost *net,
    const NfWorld *world,
    NfServerClient *recipient) {
    NfSnapshotMessage snapshot = {
        .server_tick = world->tick,
        .acknowledged_input = recipient->last_input_sequence
    };

    for (size_t i = 0;
         i < NF_MAX_ENTITIES &&
         snapshot.actor_count < NF_NET_MAX_SNAPSHOT_ACTORS;
         ++i) {
        const NfActor *actor = &world->actors[i];
        if (actor->active) {
            nf_actor_to_net_state(
                actor, &snapshot.actors[snapshot.actor_count++]);
        }
    }

    const NfActor *owner = nf_world_find_actor_const(
        world, recipient->entity_id);
    if (owner != NULL) {
        snapshot.owner_authority_valid = true;
        nf_weapon_authority_to_net_state(
            owner, &snapshot.owner_authority);
    }

    uint8_t buffer[NF_NET_MAX_PACKET_BYTES];
    const size_t encoded = nf_protocol_encode_snapshot(
        buffer, sizeof(buffer), &snapshot);
    if (encoded != 0u) {
        nf_net_send(
            net,
            recipient->peer,
            NF_NET_CHANNEL_STATE,
            buffer,
            encoded,
            false);
    }
}

static void history_record(
    NfHistoryFrame history[],
    const NfWorld *world) {
    NfHistoryFrame *frame = &history[world->tick % NF_HISTORY_FRAMES];
    memset(frame, 0, sizeof(*frame));
    frame->valid = true;
    frame->tick = world->tick;

    for (size_t i = 0;
         i < NF_MAX_ENTITIES && frame->count < NF_NET_MAX_SNAPSHOT_ACTORS;
         ++i) {
        const NfActor *actor = &world->actors[i];
        if (actor->active) {
            frame->actors[frame->count++] = (NfHistoryActor){
                .id = actor->id,
                .position = actor->transform.position,
                .crouched = actor->movement.crouched,
                .alive = actor->combat.alive
            };
        }
    }
}

static const NfHistoryFrame *history_find(
    NfHistoryFrame history[],
    uint64_t desired,
    uint64_t now) {
    const uint64_t minimum = now > NF_NET_MAX_REWIND_TICKS
        ? now - NF_NET_MAX_REWIND_TICKS
        : 0u;
    if (desired < minimum) desired = minimum;
    if (desired > now) desired = now;

    const NfHistoryFrame *best = NULL;
    for (size_t i = 0; i < NF_HISTORY_FRAMES; ++i) {
        const NfHistoryFrame *frame = &history[i];
        if (!frame->valid || frame->tick > desired) continue;
        if (best == NULL || frame->tick > best->tick) best = frame;
    }
    return best;
}

static bool ray_aabb(
    NfVec3 origin,
    NfVec3 direction,
    NfVec3 minimum,
    NfVec3 maximum,
    float *distance) {
    float tmin = 0.0f;
    float tmax = FLT_MAX;
    const float origin_values[3] = {origin.x, origin.y, origin.z};
    const float direction_values[3] = {
        direction.x, direction.y, direction.z
    };
    const float minimum_values[3] = {minimum.x, minimum.y, minimum.z};
    const float maximum_values[3] = {maximum.x, maximum.y, maximum.z};

    for (int i = 0; i < 3; ++i) {
        if (fabsf(direction_values[i]) < 1e-6f) {
            if (origin_values[i] < minimum_values[i] ||
                origin_values[i] > maximum_values[i]) {
                return false;
            }
            continue;
        }

        float t1 = (minimum_values[i] - origin_values[i]) /
            direction_values[i];
        float t2 = (maximum_values[i] - origin_values[i]) /
            direction_values[i];
        if (t1 > t2) {
            const float temp = t1;
            t1 = t2;
            t2 = temp;
        }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return false;
    }

    if (tmax < 0.0f) return false;
    if (distance != NULL) *distance = tmin >= 0.0f ? tmin : tmax;
    return true;
}

static bool ray_sphere(
    NfVec3 origin,
    NfVec3 direction,
    NfVec3 center,
    float radius,
    float *distance) {
    const NfVec3 offset = vsub(origin, center);
    const float b = vdot(offset, direction);
    const float c = vdot(offset, offset) - radius*radius;
    const float discriminant = b*b - c;
    if (discriminant < 0.0f) return false;

    const float root = sqrtf(discriminant);
    float t = -b - root;
    if (t < 0.0f) t = -b + root;
    if (t < 0.0f) return false;
    if (distance != NULL) *distance = t;
    return true;
}

static void consider_zone(
    bool hit,
    float distance,
    NfHitZone candidate,
    float *nearest,
    NfEntityId id,
    NfEntityId *target,
    NfHitZone *zone) {
    if (hit && distance < *nearest) {
        *nearest = distance;
        *target = id;
        *zone = candidate;
    }
}

static bool hitscan_target(
    const NfWorld *world,
    const NfHistoryFrame *frame,
    NfEntityId shooter,
    NfFaction shooter_faction,
    NfVec3 origin,
    NfVec3 direction,
    float world_limit,
    NfRelationship rival_relation,
    bool friendly_fire,
    NfEntityId *target_out,
    NfHitZone *zone_out,
    float *distance_out) {
    float nearest = world_limit;
    NfEntityId target = 0u;
    NfHitZone zone = NF_HIT_NONE;
    if (frame == NULL) return false;

    for (uint8_t i = 0u; i < frame->count; ++i) {
        const NfHistoryActor *history_actor = &frame->actors[i];
        if (history_actor->id == shooter || !history_actor->alive) continue;

        const NfActor *current = nf_world_find_actor_const(
            world, history_actor->id);
        if (current == NULL) continue;

        const NfRelationship relation = nf_relation_between(
            shooter_faction, current->faction, rival_relation);
        if (!nf_relation_can_damage(relation, friendly_fire)) continue;

        if (current->faction == NF_FACTION_RANCHER) {
            float distance = FLT_MAX;
            const NfHitZone candidate = nf_cattler_hit_zone_for_ray(
                origin,
                direction,
                history_actor->position,
                nearest,
                &distance);
            consider_zone(
                candidate != NF_HIT_NONE,
                distance,
                candidate,
                &nearest,
                history_actor->id,
                &target,
                &zone);
        } else {
            const float height = history_actor->crouched
                ? world->movement.crouch_height
                : world->movement.stand_height;
            float head_distance = FLT_MAX;
            float body_distance = FLT_MAX;
            const NfVec3 head = {
                history_actor->position.x,
                history_actor->position.y + height - 0.20f,
                history_actor->position.z
            };
            const bool head_hit = ray_sphere(
                origin,
                direction,
                head,
                0.23f,
                &head_distance);
            const NfVec3 minimum = {
                history_actor->position.x - world->movement.radius,
                history_actor->position.y,
                history_actor->position.z - world->movement.radius
            };
            const NfVec3 maximum = {
                history_actor->position.x + world->movement.radius,
                history_actor->position.y + height*0.78f,
                history_actor->position.z + world->movement.radius
            };
            const bool body_hit = ray_aabb(
                origin,
                direction,
                minimum,
                maximum,
                &body_distance);
            consider_zone(
                head_hit,
                head_distance,
                NF_HIT_HEAD,
                &nearest,
                history_actor->id,
                &target,
                &zone);
            consider_zone(
                body_hit,
                body_distance,
                NF_HIT_BODY,
                &nearest,
                history_actor->id,
                &target,
                &zone);
        }
    }

    if (target == 0u) return false;
    *target_out = target;
    *zone_out = zone;
    *distance_out = nearest;
    return true;
}

static float damage_for_zone(
    const NfWeaponSpec *spec,
    NfHitZone zone) {
    if (zone == NF_HIT_HEAD) return spec->damage * spec->head_multiplier;
    if (zone == NF_HIT_KNEE) return spec->damage * 1.25f;
    if (zone == NF_HIT_FOOT) return spec->damage * 1.10f;
    return spec->damage;
}

static void publish_cattler_event(
    NfCattlerSystem *cattlers,
    NfWorld *world,
    NfCombatEvent *event) {
    if (cattlers != NULL && event != NULL) {
        nf_cattler_on_combat_event(cattlers, world, event);
    }
}

static void process_combat_control(
    NfNetHost *net,
    NfWorld *world,
    NfServerClient clients[],
    NfSemanticBus *semantics,
    NfCattlerSystem *cattlers,
    const NfControlFrame *control,
    NfHistoryFrame history[],
    NfRelationship rival_relation,
    bool friendly_fire) {
    NfActor *actor = nf_world_find_actor(world, control->actor);
    if (actor == NULL || !actor->combat.alive) return;

    if ((control->move.sprint_held || control->move.interact_held) &&
        actor->combat.state == NF_WEAPON_RELOADING) {
        nf_combat_cancel_reload(actor);
    }

    if (control->combat.weapon_slot > 0u &&
        control->combat.weapon_slot < NF_WEAPON_COUNT &&
        nf_combat_select_weapon(
            actor, (NfWeaponId)control->combat.weapon_slot)) {
        NfCombatEvent event = {
            .server_tick = world->tick,
            .type = NF_COMBAT_EVENT_WEAPON_SWITCH,
            .source = actor->id,
            .weapon = (NfWeaponId)control->combat.weapon_slot,
            .position = actor->transform.position
        };
        publish_cattler_event(cattlers, world, &event);
        broadcast_event(net, clients, semantics, event);
    }

    if (control->combat.reload_pressed && nf_combat_start_reload(actor)) {
        NfCombatEvent event = {
            .server_tick = world->tick,
            .type = NF_COMBAT_EVENT_RELOAD,
            .source = actor->id,
            .weapon = actor->combat.weapon,
            .position = actor->transform.position
        };
        publish_cattler_event(cattlers, world, &event);
        broadcast_event(net, clients, semantics, event);
    }

    nf_weapon_authority_step_actor(
        actor,
        control->combat.focus_held,
        1.0f/(float)NF_TICK_RATE);

    NfWeaponSolution solution = {0};
    NfCombatEvent fire_event = {0};
    if (!nf_weapon_authority_try_fire(
            actor,
            &control->combat,
            control->sequence,
            world->tick,
            world->seed,
            control->move.yaw_radians,
            &solution,
            &fire_event)) {
        return;
    }

    const NfWeaponSpec *spec = nf_weapon_spec(actor->combat.weapon);
    const NfVec3 origin = nf_weapon_muzzle_origin(
        actor, control->move.yaw_radians);
    NfWorldRayHit world_hit = {0};
    const bool has_world_hit = nf_weapon_world_first_hit(
        world,
        origin,
        solution.solved_direction,
        spec->range,
        &world_hit);
    const float world_limit = has_world_hit
        ? world_hit.distance
        : spec->range;

    fire_event.position = origin;
    fire_event.shot_blocked = has_world_hit && world_hit.distance <= 1.10f;
    publish_cattler_event(cattlers, world, &fire_event);
    broadcast_event(net, clients, semantics, fire_event);

    const NfHistoryFrame *frame = history_find(
        history, control->perceived_tick, world->tick);
    NfEntityId target_id = 0u;
    NfHitZone zone = NF_HIT_NONE;
    float distance = 0.0f;
    if (!hitscan_target(
            world,
            frame,
            actor->id,
            actor->faction,
            origin,
            solution.solved_direction,
            world_limit,
            rival_relation,
            friendly_fire,
            &target_id,
            &zone,
            &distance)) {
        return;
    }

    (void)distance;
    NfActor *target = nf_world_find_actor(world, target_id);
    const float damage = damage_for_zone(spec, zone);
    NfCombatEvent hit_event = {0};
    if (nf_combat_apply_damage(
            target,
            actor->id,
            actor->combat.weapon,
            zone,
            damage,
            world->tick,
            &hit_event)) {
        const uint64_t rewind = frame != NULL && world->tick > frame->tick
            ? world->tick - frame->tick
            : 0u;
        uint64_t rewind_ms = (rewind * 1000u) / NF_TICK_RATE;
        if (rewind_ms > 65535u) rewind_ms = 65535u;
        hit_event.rewind_ms = (uint16_t)rewind_ms;
        hit_event.shot_sequence = solution.shot_sequence;
        hit_event.shot_direction = solution.solved_direction;
        publish_cattler_event(cattlers, world, &hit_event);
        broadcast_event(net, clients, semantics, hit_event);
    }
}

static void process_respawns(
    NfNetHost *net,
    NfWorld *world,
    NfServerClient clients[],
    NfAiSystem *ai,
    NfSpatialSystem *spatial,
    NfCattlerSystem *cattlers,
    NfCattlerRecurrenceSystem *recurrence,
    NfSemanticBus *semantics) {
    for (size_t i = 0; i < NF_MAX_ENTITIES; ++i) {
        NfActor *actor = &world->actors[i];
        if (!actor->active ||
            actor->combat.alive ||
            actor->combat.respawn_timer > 0.0f) {
            continue;
        }

        NfVec3 spawn = {0};
        NfCattlerReturnKind return_kind = NF_CATTLER_RETURN_NONE;
        uint8_t return_region = NF_REGION_INVALID;
        const bool is_cattler = actor->faction == NF_FACTION_RANCHER;
        if (is_cattler) {
            if (!nf_cattler_recurrence_spawn_for(
                    recurrence,
                    cattlers,
                    world,
                    actor->id,
                    &spawn,
                    &return_kind,
                    &return_region)) {
                continue;
            }
        } else if (!ordinary_respawn_position(
                       clients, ai, actor->id, &spawn)) {
            continue;
        }

        NfCombatEvent event = {0};
        nf_combat_respawn(actor, spawn, world->tick, &event);
        reset_movement_after_respawn(world, actor);
        if (is_cattler) {
            nf_cattler_recurrence_commit(
                recurrence,
                cattlers,
                actor->id,
                spawn,
                return_region);
            nf_cattler_on_respawn(cattlers, actor->id, world);
        } else {
            nf_ai_on_respawn(ai, actor->id);
            nf_spatial_on_respawn(spatial, actor->id, world);
        }
        publish_cattler_event(cattlers, world, &event);
        broadcast_event(net, clients, semantics, event);
    }
}

static NfControlFrame client_control(
    const NfServerClient *client,
    NfMoveInput move,
    NfCombatInput combat) {
    return (NfControlFrame){
        .actor = client->entity_id,
        .source = NF_CONTROL_NETWORK,
        .move = move,
        .combat = combat,
        .perceived_tick = client->current_client_tick,
        .sequence = client->last_input_sequence
    };
}

static NfCattlerDebugProfile parse_cattler_profile(const char *value) {
    if (value == NULL) return NF_CATTLER_PROFILE_NORMAL;
    if (strcmp(value, "pack") == 0) return NF_CATTLER_PROFILE_PACK;
    if (strcmp(value, "loner") == 0) return NF_CATTLER_PROFILE_LONER;
    if (strcmp(value, "horde") == 0) return NF_CATTLER_PROFILE_HORDE;
    return NF_CATTLER_PROFILE_NORMAL;
}

static void seed_lifeworld_corner(
    NfCattlerSystem *cattlers,
    NfWorld *world) {
    if (cattlers == NULL || world == NULL || cattlers->count < 2u) return;

    static const NfVec3 seeded[2] = {
        {-51.0f, 0.05f, -12.0f},
        {-43.0f, 0.05f, -7.0f}
    };
    for (size_t i = 0u; i < 2u; ++i) {
        NfCattlerAgent *agent = &cattlers->agents[i];
        NfActor *body = nf_world_find_actor(world, agent->actor_id);
        if (body == NULL) continue;

        body->transform.position = seeded[i];
        body->transform.velocity = (NfVec3){0};
        agent->spawn = seeded[i];
        const uint8_t region = nf_region_nearest(&cattlers->graph, seeded[i]);
        agent->current_region = region;
        agent->target_region = region;
        agent->next_region = region;
        agent->home_region = region;
        agent->claimed_watch_region = NF_REGION_INVALID;
    }
}

int main(int argc, char **argv) {
    uint16_t port = NF_NET_DEFAULT_PORT;
    uint32_t sim_latency = 0u;
    uint32_t sim_jitter = 0u;
    float sim_loss = 0.0f;
    double duration = 0.0;
    bool friendly_fire = false;
    size_t ai_count = 4u;
    size_t pressure_slots = 2u;
    size_t cattler_count = NF_CATTLER_DEFAULT_AGENTS;
    NfRelationship rival_relation = NF_RELATION_HOSTILE;
    NfCattlerDebugProfile cattler_profile = NF_CATTLER_PROFILE_NORMAL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            port = (uint16_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--sim-latency") == 0 && i + 1 < argc) {
            sim_latency = (uint32_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--sim-jitter") == 0 && i + 1 < argc) {
            sim_jitter = (uint32_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--sim-loss") == 0 && i + 1 < argc) {
            sim_loss = (float)atof(argv[++i]);
        } else if (strcmp(argv[i], "--duration") == 0 && i + 1 < argc) {
            duration = atof(argv[++i]);
        } else if (strcmp(argv[i], "--ai-count") == 0 && i + 1 < argc) {
            const long value = strtol(argv[++i], NULL, 10);
            ai_count = value < 0 ? 0u : (size_t)value;
            if (ai_count > NF_AI_MAX_AGENTS) ai_count = NF_AI_MAX_AGENTS;
        } else if (strcmp(argv[i], "--pressure-slots") == 0 && i + 1 < argc) {
            const long value = strtol(argv[++i], NULL, 10);
            pressure_slots = value < 0 ? 0u : (size_t)value;
            if (pressure_slots > NF_ENCOUNTER_MAX_PRESSURE_SLOTS) {
                pressure_slots = NF_ENCOUNTER_MAX_PRESSURE_SLOTS;
            }
        } else if (strcmp(argv[i], "--cattler-count") == 0 && i + 1 < argc) {
            const long value = strtol(argv[++i], NULL, 10);
            cattler_count = value < 0 ? 0u : (size_t)value;
            if (cattler_count > NF_CATTLER_MAX_AGENTS) {
                cattler_count = NF_CATTLER_MAX_AGENTS;
            }
        } else if (strcmp(argv[i], "--cattler-profile") == 0 && i + 1 < argc) {
            cattler_profile = parse_cattler_profile(argv[++i]);
        } else if (strcmp(argv[i], "--rival-truce") == 0) {
            rival_relation = NF_RELATION_TRUCE;
        } else if (strcmp(argv[i], "--friendly-fire") == 0) {
            friendly_fire = true;
        }
    }

    if (cattler_profile == NF_CATTLER_PROFILE_HORDE && cattler_count < 4u) {
        cattler_count = NF_CATTLER_MAX_AGENTS;
    }
    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    if (!nf_security_init() || !nf_net_global_init()) {
        fprintf(stderr, "nightfall: network/security init failed\n");
        return 1;
    }

    NfNetHost net;
    if (!nf_net_server_open(&net, port, NF_SERVER_CLIENTS)) {
        fprintf(stderr, "nightfall: could not open UDP port %u\n", port);
        nf_net_global_shutdown();
        return 1;
    }
    nf_net_set_simulation(&net, sim_latency, sim_jitter, sim_loss);

    NfWorld world;
    nf_world_init(&world, 20260807u);
    nf_world_build_movement_lab(&world);
    NfServerClient clients[NF_SERVER_CLIENTS] = {0};
    NfHistoryFrame history[NF_HISTORY_FRAMES] = {0};
    NfSemanticBus semantics;
    nf_semantic_bus_init(&semantics);
    NfAiSystem ai;
    nf_ai_init(&ai, &world, ai_count, world.seed ^ 0xA105u);
    nf_ai_set_rival_relationship(&ai, rival_relation);
    NfEncounterState encounter;
    nf_encounter_init(
        &encounter,
        &ai,
        &world,
        pressure_slots,
        world.seed ^ 0xE06u);
    NfSpatialSystem spatial;
    nf_spatial_init(&spatial, &ai, &world, world.seed ^ 0x507u);
    NfCattlerSystem cattlers;
    nf_cattler_init(
        &cattlers,
        &world,
        cattler_count,
        cattler_profile,
        world.seed ^ 0xCA771Eu);
    seed_lifeworld_corner(&cattlers, &world);
    NfCattlerRecurrenceSystem recurrence;
    nf_cattler_recurrence_init(&recurrence, world.seed ^ 0xEC0106u);
    NfLifeworldSystem lifeworld;
    nf_lifeworld_init(&lifeworld, &spatial.graph, world.seed ^ 0x1F3u);
    history_record(history, &world);

    printf(
        "nightfall!punk v1.6B Weapon Authority / Mobility server | "
        "protocol=%u stale=%ums\n",
        NF_PROTOCOL_VERSION,
        NF_NET_STALE_HELD_INPUT_MS);

    const double fixed_ms = 1000.0/(double)NF_TICK_RATE;
    uint32_t last = nf_net_now_ms();
    const uint32_t start = last;
    double accumulator = 0.0;
    uint64_t next_log_tick = 0u;

    while (g_running) {
        NfNetEvent event;
        while (nf_net_service(&net, &event, 0) > 0) {
            if (event.type == NF_NET_EVENT_RECEIVE) {
                const NfMessageType type = nf_protocol_peek_type(
                    event.data, event.size);
                NfServerClient *client = by_peer(clients, event.peer);
                if (type == NF_MSG_HELLO) {
                    handle_hello(
                        &net,
                        &world,
                        clients,
                        event.peer,
                        event.data,
                        event.size);
                } else if (type == NF_MSG_INPUT) {
                    handle_input(client, event.data, event.size);
                } else if (type == NF_MSG_PING && client != NULL) {
                    NfPingMessage ping;
                    if (nf_protocol_decode_ping(
                            event.data,
                            event.size,
                            NF_MSG_PING,
                            &ping)) {
                        uint8_t buffer[32];
                        const size_t encoded = nf_protocol_encode_ping(
                            buffer,
                            sizeof(buffer),
                            NF_MSG_PONG,
                            &ping);
                        if (encoded != 0u) {
                            nf_net_send(
                                &net,
                                client->peer,
                                NF_NET_CHANNEL_RELIABLE,
                                buffer,
                                encoded,
                                true);
                        }
                    }
                }
            } else if (event.type == NF_NET_EVENT_DISCONNECT) {
                NfServerClient *client = by_peer(clients, event.peer);
                if (client != NULL) {
                    client->connected = false;
                    client->peer = NULL;
                    client->current_input = (NfMoveInput){0};
                    client->combat_input = (NfCombatInput){0};
                    client->reserved_until_ms =
                        nf_net_now_ms() + NF_NET_RECONNECT_WINDOW_MS;
                }
            }
        }

        const uint32_t now = nf_net_now_ms();
        uint32_t elapsed = now - last;
        last = now;
        if (elapsed > 250u) elapsed = 250u;
        accumulator += (double)elapsed;
        expire_reservations(&world, clients, now);

        while (accumulator >= fixed_ms) {
            nf_spatial_tick(&spatial, &ai, &world, &semantics);
            nf_lifeworld_apply_attention_bias(
                &spatial, &lifeworld, &ai, &world);

            NfControlFrame ai_controls[NF_AI_MAX_AGENTS];
            const size_t ai_count_now = nf_ai_tick(
                &ai,
                &world,
                &semantics,
                ai_controls,
                NF_AI_MAX_AGENTS);
            nf_spatial_filter_controls(
                &spatial, &ai, &world, ai_controls, ai_count_now);
            nf_encounter_filter_controls(
                &encounter, &ai, &world, ai_controls, ai_count_now);

            NfControlFrame cattler_controls[NF_CATTLER_MAX_AGENTS];
            const size_t cattler_count_now = nf_cattler_tick(
                &cattlers,
                &world,
                &semantics,
                cattler_controls,
                NF_CATTLER_MAX_AGENTS);

            NfControlFrame client_controls[NF_SERVER_CLIENTS];
            bool client_control_valid[NF_SERVER_CLIENTS] = {0};
            for (size_t i = 0; i < NF_SERVER_CLIENTS; ++i) {
                NfServerClient *client = &clients[i];
                if (!client->occupied) continue;

                const bool fresh = client_input_fresh(client, now);
                const NfMoveInput move = fresh
                    ? client->current_input
                    : (NfMoveInput){0};
                const NfCombatInput combat = fresh
                    ? client->combat_input
                    : (NfCombatInput){0};
                nf_world_set_input(&world, client->entity_id, move);
                if (client->connected) {
                    client_controls[i] = client_control(
                        client, move, combat);
                    client_control_valid[i] = true;
                }
            }

            for (size_t i = 0; i < ai_count_now; ++i) {
                nf_world_set_input(
                    &world, ai_controls[i].actor, ai_controls[i].move);
            }
            for (size_t i = 0; i < cattler_count_now; ++i) {
                nf_world_set_input(
                    &world,
                    cattler_controls[i].actor,
                    cattler_controls[i].move);
            }

            /* v1.6B Q76: realized mobility is integrated before Weapon Authority/fire. */
            nf_world_step(&world, 1.0f/(float)NF_TICK_RATE);
            history_record(history, &world);

            for (size_t i = 0; i < NF_SERVER_CLIENTS; ++i) {
                if (!client_control_valid[i]) continue;
                process_combat_control(
                    &net,
                    &world,
                    clients,
                    &semantics,
                    &cattlers,
                    &client_controls[i],
                    history,
                    rival_relation,
                    friendly_fire);
                NfServerClient *client = &clients[i];
                client->current_input.jump_pressed = false;
                client->combat_input.fire_pressed = false;
                client->combat_input.reload_pressed = false;
                client->combat_input.weapon_slot = 0u;
            }

            for (size_t i = 0; i < ai_count_now; ++i) {
                process_combat_control(
                    &net,
                    &world,
                    clients,
                    &semantics,
                    &cattlers,
                    &ai_controls[i],
                    history,
                    rival_relation,
                    friendly_fire);
            }
            for (size_t i = 0; i < cattler_count_now; ++i) {
                process_combat_control(
                    &net,
                    &world,
                    clients,
                    &semantics,
                    &cattlers,
                    &cattler_controls[i],
                    history,
                    rival_relation,
                    friendly_fire);
            }

            nf_cattler_recurrence_tick(&recurrence, &cattlers, &world);
            process_respawns(
                &net,
                &world,
                clients,
                &ai,
                &spatial,
                &cattlers,
                &recurrence,
                &semantics);
            nf_lifeworld_tick(&lifeworld, &world, &spatial.graph);

            if (world.tick % (NF_TICK_RATE/NF_NET_SNAPSHOT_HZ) == 0u) {
                for (size_t i = 0; i < NF_SERVER_CLIENTS; ++i) {
                    if (clients[i].occupied && clients[i].connected) {
                        send_snapshot(&net, &world, &clients[i]);
                    }
                }
            }

            if (world.tick >= next_log_tick) {
                for (size_t i = 0; i < NF_SERVER_CLIENTS; ++i) {
                    if (!clients[i].occupied) continue;
                    const NfActor *actor = nf_world_find_actor_const(
                        &world, clients[i].entity_id);
                    if (actor != NULL) {
                        printf(
                            "[v16b-authority] actor=%u focus=%.3f shot=%u "
                            "redirect=%.3f support=%.3f weapon=%s\n",
                            actor->id,
                            actor->weapon_authority.focus_amount,
                            actor->weapon_authority.accepted_shot_sequence,
                            actor->weapon_authority.redirect_stress_deg,
                            actor->weapon_authority.support_stress_deg,
                            nf_weapon_name(actor->combat.weapon));
                    }
                }
                next_log_tick = world.tick + NF_TICK_RATE*2u;
            }

            accumulator -= fixed_ms;
        }

        nf_net_flush(&net);
        if (duration > 0.0 && (double)(now - start) >= duration*1000.0) {
            break;
        }
        sleep_ms(1u);
    }

    for (size_t i = 0; i < NF_SERVER_CLIENTS; ++i) {
        if (clients[i].connected) {
            nf_net_disconnect(clients[i].peer, 0u);
        }
    }
    nf_net_flush(&net);
    nf_net_close(&net);
    nf_net_global_shutdown();
    printf(
        "[v16b-server] shutdown tick=%llu\n",
        (unsigned long long)world.tick);
    return 0;
}
