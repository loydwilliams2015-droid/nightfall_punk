#include "nf_world.h"
#include "nf_movement.h"

#include <math.h>
#include <string.h>

static NfMovementConfig nf_default_movement_config(void) {
    return (NfMovementConfig){
        .walk_speed = 4.8f,
        .sprint_speed = 7.2f,
        .crouch_speed = 2.8f,
        .ground_accel = 28.0f,
        .air_accel = 8.0f,
        .ground_brake = 34.0f,
        .gravity = 19.0f,
        .jump_speed = 7.0f,
        .second_jump_speed = 6.3f,
        .radius = 0.36f,
        .stand_height = 1.80f,
        .crouch_height = 1.15f,
        .stand_eye_height = 1.62f,
        .crouch_eye_height = 1.00f,
        .step_height = 0.48f,
        .vault_height = 1.05f,
        .mantle_height = 1.75f,
        .ladder_speed = 3.6f,
        .candidate_distance = 1.45f,
        .candidate_acquire_score = 0.46f,
        .candidate_retain_score = 0.34f
    };
}

void nf_world_init(NfWorld *world, uint32_t seed) {
    if (world == NULL) return;
    memset(world, 0, sizeof(*world));
    world->seed = seed;
    world->next_entity_id = 1u;
    world->movement = nf_default_movement_config();
}

int nf_world_add_collider(NfWorld *world, NfColliderKind kind, NfVec3 min, NfVec3 max) {
    if (world == NULL || world->collider_count >= NF_MAX_COLLIDERS) return -1;
    const size_t index = world->collider_count++;
    NfCollider *collider = &world->colliders[index];
    *collider = (NfCollider){
        .kind = kind,
        .min = min,
        .max = max,
        .previous_min = min,
        .previous_max = max,
        .base_min = min,
        .base_max = max,
        .velocity = {0},
        .motion_amplitude = 0.0f,
        .motion_period = 0.0f
    };
    return (int)index;
}

int nf_world_add_moving_platform(
    NfWorld *world, NfVec3 min, NfVec3 max, NfVec3 velocity_axis,
    float amplitude, float period) {
    const int index = nf_world_add_collider(world, NF_COLLIDER_MOVING_PLATFORM, min, max);
    if (index < 0) return index;
    NfCollider *collider = &world->colliders[index];
    collider->velocity = velocity_axis;
    collider->motion_amplitude = amplitude;
    collider->motion_period = period;
    return index;
}

int nf_world_add_ramp(NfWorld *world, NfVec3 min, NfVec3 max, NfRampAxis axis) {
    if (world == NULL || world->ramp_count >= NF_MAX_RAMPS) return -1;
    const size_t index = world->ramp_count++;
    world->ramps[index] = (NfRamp){.min = min, .max = max, .axis = axis};
    return (int)index;
}

void nf_world_build_movement_lab(NfWorld *world) {
    if (world == NULL) return;
    world->collider_count = 0u;
    world->ramp_count = 0u;

    nf_world_add_collider(world, NF_COLLIDER_SOLID, (NfVec3){-30.0f, -0.5f, -30.0f}, (NfVec3){30.0f, 0.0f, 30.0f});
    nf_world_add_collider(world, NF_COLLIDER_SOLID, (NfVec3){-30.0f, 0.0f, -30.0f}, (NfVec3){-29.2f, 4.0f, 30.0f});
    nf_world_add_collider(world, NF_COLLIDER_SOLID, (NfVec3){29.2f, 0.0f, -30.0f}, (NfVec3){30.0f, 4.0f, 30.0f});
    nf_world_add_collider(world, NF_COLLIDER_SOLID, (NfVec3){-30.0f, 0.0f, -30.0f}, (NfVec3){30.0f, 4.0f, -29.2f});
    nf_world_add_collider(world, NF_COLLIDER_SOLID, (NfVec3){-30.0f, 0.0f, 29.2f}, (NfVec3){30.0f, 4.0f, 30.0f});

    for (int i = 0; i < 6; ++i) {
        const float z0 = -8.0f + (float)i * 1.0f;
        const float top = 0.25f + (float)i * 0.25f;
        nf_world_add_collider(world, NF_COLLIDER_SOLID,
            (NfVec3){-12.0f, 0.0f, z0},
            (NfVec3){-7.5f, top, z0 + 1.0f});
    }

    nf_world_add_collider(world, NF_COLLIDER_SOLID, (NfVec3){-2.5f, 0.0f, -7.0f}, (NfVec3){2.5f, 0.42f, -6.4f});
    nf_world_add_collider(world, NF_COLLIDER_SOLID, (NfVec3){-2.5f, 0.0f, -2.5f}, (NfVec3){2.5f, 0.90f, -1.9f});
    nf_world_add_collider(world, NF_COLLIDER_SOLID, (NfVec3){-2.5f, 0.0f, 2.0f}, (NfVec3){2.5f, 1.45f, 2.6f});

    nf_world_add_collider(world, NF_COLLIDER_SOLID, (NfVec3){7.0f, 0.0f, -8.0f}, (NfVec3){13.0f, 2.0f, -2.0f});
    nf_world_add_collider(world, NF_COLLIDER_SOLID, (NfVec3){7.0f, 2.0f, -1.0f}, (NfVec3){13.0f, 2.35f, 5.0f});
    nf_world_add_collider(world, NF_COLLIDER_SOLID, (NfVec3){10.0f, 2.0f, 5.0f}, (NfVec3){11.0f, 2.35f, 13.0f});

    nf_world_add_collider(world, NF_COLLIDER_LADDER, (NfVec3){6.55f, 0.0f, -5.6f}, (NfVec3){7.05f, 2.4f, -4.4f});

    nf_world_add_collider(world, NF_COLLIDER_SOLID, (NfVec3){-15.0f, 1.25f, 7.0f}, (NfVec3){-8.0f, 1.65f, 11.0f});
    nf_world_add_collider(world, NF_COLLIDER_SOLID, (NfVec3){-15.0f, 0.0f, 6.8f}, (NfVec3){-14.5f, 1.25f, 11.2f});
    nf_world_add_collider(world, NF_COLLIDER_SOLID, (NfVec3){-8.5f, 0.0f, 6.8f}, (NfVec3){-8.0f, 1.25f, 11.2f});

    nf_world_add_moving_platform(world,
        (NfVec3){1.0f, 1.0f, 10.0f}, (NfVec3){5.0f, 1.35f, 13.0f},
        (NfVec3){1.0f, 0.0f, 0.0f}, 4.0f, 6.0f);

    nf_world_add_ramp(world, (NfVec3){-20.0f, 0.0f, -17.0f}, (NfVec3){-14.0f, 1.5f, -11.0f}, NF_RAMP_POS_Z);
    nf_world_add_ramp(world, (NfVec3){14.0f, 0.0f, -17.0f}, (NfVec3){20.0f, 3.0f, -11.0f}, NF_RAMP_POS_Z);
}

NfEntityId nf_world_spawn_actor(NfWorld *world, NfFaction faction, NfVec3 position) {
    if (world == NULL) return 0u;
    for (size_t i = 0; i < NF_MAX_ENTITIES; ++i) {
        NfActor *actor = &world->actors[i];
        if (!actor->active) {
            memset(actor, 0, sizeof(*actor));
            actor->active = true;
            actor->id = world->next_entity_id++;
            actor->faction = faction;
            actor->transform.position = position;
            actor->health = 100.0f;
            actor->movement.mode = NF_MOVE_AIR;
            actor->movement.body_height = world->movement.stand_height;
            actor->movement.eye_height = world->movement.stand_eye_height;
            actor->movement.attached_collider = -1;
            actor->movement.ground_collider = -1;
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

void nf_world_set_input(NfWorld *world, NfEntityId id, NfMoveInput input) {
    NfActor *actor = nf_world_find_actor(world, id);
    if (actor == NULL) return;
    actor->input = input;
}

static void nf_world_update_moving_colliders(NfWorld *world, float dt) {
    (void)dt;
    const float time = (float)world->tick / (float)NF_TICK_RATE;
    for (size_t i = 0; i < world->collider_count; ++i) {
        NfCollider *c = &world->colliders[i];
        c->previous_min = c->min;
        c->previous_max = c->max;
        if (c->kind != NF_COLLIDER_MOVING_PLATFORM || c->motion_period <= 0.0f) continue;
        const float phase = (time / c->motion_period) * 6.28318530718f;
        const float offset = sinf(phase) * c->motion_amplitude;
        c->min = c->base_min;
        c->max = c->base_max;
        c->min.x += c->velocity.x * offset;
        c->max.x += c->velocity.x * offset;
        c->min.y += c->velocity.y * offset;
        c->max.y += c->velocity.y * offset;
        c->min.z += c->velocity.z * offset;
        c->max.z += c->velocity.z * offset;
    }
}

void nf_world_step(NfWorld *world, float dt) {
    if (world == NULL || dt <= 0.0f) return;
    nf_world_update_moving_colliders(world, dt);

    for (size_t i = 0; i < NF_MAX_ENTITIES; ++i) {
        NfActor *actor = &world->actors[i];
        if (!actor->active) continue;
        nf_movement_step_actor(world, actor, dt);
        actor->input.jump_pressed = false;
    }
    ++world->tick;
}

size_t nf_world_active_actor_count(const NfWorld *world) {
    if (world == NULL) return 0u;
    size_t count = 0u;
    for (size_t i = 0; i < NF_MAX_ENTITIES; ++i) {
        if (world->actors[i].active) ++count;
    }
    return count;
}

const char *nf_movement_mode_name(NfMovementMode mode) {
    switch (mode) {
        case NF_MOVE_GROUND: return "GROUND";
        case NF_MOVE_SPRINT: return "SPRINT";
        case NF_MOVE_CROUCH: return "CROUCH";
        case NF_MOVE_AIR: return "AIR";
        case NF_MOVE_LADDER: return "LADDER";
        case NF_MOVE_VAULT: return "VAULT";
        case NF_MOVE_MANTLE: return "MANTLE";
        case NF_MOVE_PLATFORM: return "PLATFORM";
        default: return "UNKNOWN";
    }
}

const char *nf_traversal_type_name(NfTraversalType type) {
    switch (type) {
        case NF_TRAVERSAL_NONE: return "NONE";
        case NF_TRAVERSAL_STEP: return "STEP";
        case NF_TRAVERSAL_VAULT: return "VAULT";
        case NF_TRAVERSAL_MANTLE: return "MANTLE";
        case NF_TRAVERSAL_LADDER: return "LADDER";
        default: return "UNKNOWN";
    }
}
