#include "nf_movement.h"

#include <math.h>
#include <stddef.h>

static float nf_clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static float nf_absf(float v) {
    return v < 0.0f ? -v : v;
}

static float nf_len2(float x, float z) {
    return sqrtf(x * x + z * z);
}

static bool nf_xz_overlap(NfVec3 p, float radius, const NfCollider *c) {
    return p.x + radius > c->min.x && p.x - radius < c->max.x &&
           p.z + radius > c->min.z && p.z - radius < c->max.z;
}

static bool nf_body_overlaps(NfVec3 feet, float radius, float height, const NfCollider *c) {
    if (c->kind == NF_COLLIDER_LADDER) return false;
    if (!nf_xz_overlap(feet, radius, c)) return false;
    return feet.y < c->max.y && feet.y + height > c->min.y;
}

bool nf_movement_space_is_free(
    const NfWorld *world, NfVec3 feet_position, float radius, float height) {
    if (world == NULL) return false;
    for (size_t i = 0; i < world->collider_count; ++i) {
        if (nf_body_overlaps(feet_position, radius, height, &world->colliders[i])) return false;
    }
    return true;
}

static float nf_ramp_height(const NfRamp *ramp, float x, float z, bool *inside) {
    *inside = x >= ramp->min.x && x <= ramp->max.x &&
              z >= ramp->min.z && z <= ramp->max.z;
    if (!*inside) return -100000.0f;

    float t = 0.0f;
    switch (ramp->axis) {
        case NF_RAMP_POS_X: t = (x - ramp->min.x) / (ramp->max.x - ramp->min.x); break;
        case NF_RAMP_NEG_X: t = (ramp->max.x - x) / (ramp->max.x - ramp->min.x); break;
        case NF_RAMP_POS_Z: t = (z - ramp->min.z) / (ramp->max.z - ramp->min.z); break;
        case NF_RAMP_NEG_Z: t = (ramp->max.z - z) / (ramp->max.z - ramp->min.z); break;
    }
    t = nf_clampf(t, 0.0f, 1.0f);
    return ramp->min.y + (ramp->max.y - ramp->min.y) * t;
}

static float nf_support_height(
    const NfWorld *world, NfVec3 p, float radius, float current_y,
    int *support_collider) {
    float best = -100000.0f;
    int best_collider = -1;

    for (size_t i = 0; i < world->collider_count; ++i) {
        const NfCollider *c = &world->colliders[i];
        if (c->kind == NF_COLLIDER_LADDER) continue;
        if (!nf_xz_overlap(p, radius * 0.82f, c)) continue;
        if (c->max.y <= current_y + world->movement.step_height + 0.08f && c->max.y > best) {
            best = c->max.y;
            best_collider = (int)i;
        }
    }

    for (size_t i = 0; i < world->ramp_count; ++i) {
        bool inside = false;
        const float h = nf_ramp_height(&world->ramps[i], p.x, p.z, &inside);
        if (inside && h <= current_y + world->movement.step_height + 0.08f && h > best) {
            best = h;
            best_collider = -2;
        }
    }

    if (support_collider != NULL) *support_collider = best_collider;
    return best;
}

static void nf_apply_platform_delta(const NfWorld *world, NfActor *actor) {
    const int index = actor->movement.ground_collider;
    if (index < 0 || (size_t)index >= world->collider_count) return;
    const NfCollider *c = &world->colliders[index];
    if (c->kind != NF_COLLIDER_MOVING_PLATFORM) return;
    actor->transform.position.x += c->min.x - c->previous_min.x;
    actor->transform.position.y += c->min.y - c->previous_min.y;
    actor->transform.position.z += c->min.z - c->previous_min.z;
}

static void nf_approach(float *value, float target, float max_delta) {
    const float d = target - *value;
    if (d > max_delta) *value += max_delta;
    else if (d < -max_delta) *value -= max_delta;
    else *value = target;
}

static void nf_input_wish(const NfMoveInput *input, float *wish_x, float *wish_z, float *magnitude) {
    float f = nf_clampf(input->forward, -1.0f, 1.0f);
    float s = nf_clampf(input->strafe, -1.0f, 1.0f);
    const float input_len = nf_len2(f, s);
    if (input_len > 1.0f) { f /= input_len; s /= input_len; }
    const float sy = sinf(input->yaw_radians);
    const float cy = cosf(input->yaw_radians);
    *wish_x = sy * f + cy * s;
    *wish_z = cy * f - sy * s;
    *magnitude = nf_clampf(input_len, 0.0f, 1.0f);
}

static void nf_set_candidate(NfActor *actor, NfTraversalCandidate candidate) {
    actor->movement.candidate = candidate;
}

void nf_movement_update_candidate(const NfWorld *world, NfActor *actor) {
    if (world == NULL || actor == NULL) return;

    float wish_x = 0.0f, wish_z = 0.0f, wish_mag = 0.0f;
    nf_input_wish(&actor->input, &wish_x, &wish_z, &wish_mag);
    if (wish_mag < 0.05f) {
        wish_x = sinf(actor->input.yaw_radians);
        wish_z = cosf(actor->input.yaw_radians);
    }

    NfTraversalCandidate best = {0};
    best.feature_index = -1;

    for (size_t i = 0; i < world->collider_count; ++i) {
        const NfCollider *c = &world->colliders[i];
        const float cx = nf_clampf(actor->transform.position.x, c->min.x, c->max.x);
        const float cz = nf_clampf(actor->transform.position.z, c->min.z, c->max.z);
        const float dx = cx - actor->transform.position.x;
        const float dz = cz - actor->transform.position.z;
        const float distance = nf_len2(dx, dz);
        if (distance > world->movement.candidate_distance || distance < 0.001f) continue;

        const float nx = dx / distance;
        const float nz = dz / distance;
        const float alignment = nx * wish_x + nz * wish_z;
        if (alignment < 0.18f) continue;

        NfTraversalType type = NF_TRAVERSAL_NONE;
        float height = c->max.y - actor->transform.position.y;
        if (c->kind == NF_COLLIDER_LADDER) { type = NF_TRAVERSAL_LADDER; height = 0.0f; }
        else if (height > 0.04f && height <= world->movement.step_height) type = NF_TRAVERSAL_STEP;
        else if (height <= world->movement.vault_height) type = NF_TRAVERSAL_VAULT;
        else if (height <= world->movement.mantle_height) type = NF_TRAVERSAL_MANTLE;
        if (type == NF_TRAVERSAL_NONE) continue;

        float score = 0.58f * (1.0f - distance / world->movement.candidate_distance) + 0.42f * alignment;
        if (actor->movement.candidate.active && actor->movement.candidate.feature_index == (int)i) score += 0.08f;

        if (!best.active || score > best.score) {
            best.active = true;
            best.type = type;
            best.feature_index = (int)i;
            best.point = (NfVec3){cx, c->max.y, cz};
            best.normal = (NfVec3){-nx, 0.0f, -nz};
            best.score = score;
        }
    }

    const float threshold = actor->movement.candidate.active ? world->movement.candidate_retain_score : world->movement.candidate_acquire_score;
    if (best.active && best.score >= threshold) nf_set_candidate(actor, best);
    else { NfTraversalCandidate none = {0}; none.feature_index = -1; nf_set_candidate(actor, none); }
}

static bool nf_try_enter_ladder(NfActor *actor) {
    if (!actor->movement.candidate.active || actor->movement.candidate.type != NF_TRAVERSAL_LADDER) return false;
    if (actor->input.forward < 0.20f && !actor->input.interact_held) return false;
    actor->movement.mode = NF_MOVE_LADDER;
    actor->movement.attached_collider = actor->movement.candidate.feature_index;
    actor->movement.grounded = false;
    actor->transform.velocity = (NfVec3){0};
    return true;
}

static void nf_ladder_step(const NfWorld *world, NfActor *actor, float dt) {
    const int index = actor->movement.attached_collider;
    if (index < 0 || (size_t)index >= world->collider_count) { actor->movement.mode = NF_MOVE_AIR; actor->movement.attached_collider = -1; return; }
    const NfCollider *ladder = &world->colliders[index];
    if (ladder->kind != NF_COLLIDER_LADDER) { actor->movement.mode = NF_MOVE_AIR; actor->movement.attached_collider = -1; return; }

    const float center_x = 0.5f * (ladder->min.x + ladder->max.x);
    const float center_z = 0.5f * (ladder->min.z + ladder->max.z);
    nf_approach(&actor->transform.position.x, center_x, 3.0f * dt);
    nf_approach(&actor->transform.position.z, center_z, 3.0f * dt);
    actor->transform.velocity = (NfVec3){0.0f, actor->input.forward * world->movement.ladder_speed, 0.0f};
    actor->transform.position.y += actor->transform.velocity.y * dt;

    if (actor->input.jump_pressed) {
        actor->movement.mode = NF_MOVE_AIR;
        actor->movement.attached_collider = -1;
        actor->transform.velocity.y = world->movement.jump_speed * 0.75f;
        return;
    }
    if (actor->transform.position.y > ladder->max.y - 0.15f) {
        actor->transform.position.y = ladder->max.y;
        actor->movement.mode = NF_MOVE_GROUND;
        actor->movement.grounded = true;
        actor->movement.attached_collider = -1;
        actor->transform.velocity = (NfVec3){0};
    } else if (actor->transform.position.y < ladder->min.y) {
        actor->transform.position.y = ladder->min.y;
        actor->movement.mode = NF_MOVE_GROUND;
        actor->movement.grounded = true;
        actor->movement.attached_collider = -1;
        actor->transform.velocity = (NfVec3){0};
    }
}

static bool nf_begin_traversal(const NfWorld *world, NfActor *actor, float wish_x, float wish_z) {
    if (!actor->input.jump_pressed || !actor->movement.candidate.active) return false;
    const NfTraversalCandidate *c = &actor->movement.candidate;
    if (c->type != NF_TRAVERSAL_VAULT && c->type != NF_TRAVERSAL_MANTLE) return false;
    const float mag = nf_len2(wish_x, wish_z);
    if (mag < 0.05f) return false;
    wish_x /= mag; wish_z /= mag;
    if (c->type == NF_TRAVERSAL_VAULT) {
        actor->movement.mode = NF_MOVE_VAULT;
        actor->movement.traversal_timer = 0.22f;
        actor->transform.velocity.x = wish_x * world->movement.sprint_speed;
        actor->transform.velocity.z = wish_z * world->movement.sprint_speed;
        actor->transform.velocity.y = 3.6f;
    } else {
        actor->movement.mode = NF_MOVE_MANTLE;
        actor->movement.traversal_timer = 0.30f;
        actor->transform.velocity.x = wish_x * 3.2f;
        actor->transform.velocity.z = wish_z * 3.2f;
        actor->transform.velocity.y = 5.0f;
    }
    actor->movement.grounded = false;
    return true;
}

static void nf_try_horizontal_axis(const NfWorld *world, NfActor *actor, float delta, bool x_axis) {
    if (nf_absf(delta) < 0.000001f) return;
    NfVec3 next = actor->transform.position;
    if (x_axis) next.x += delta; else next.z += delta;
    if (nf_movement_space_is_free(world, next, world->movement.radius, actor->movement.body_height)) { actor->transform.position = next; return; }
    if (!actor->movement.grounded) { if (x_axis) actor->transform.velocity.x = 0.0f; else actor->transform.velocity.z = 0.0f; return; }
    NfVec3 stepped = next;
    stepped.y += world->movement.step_height;
    if (nf_movement_space_is_free(world, stepped, world->movement.radius, actor->movement.body_height)) { actor->transform.position = stepped; actor->movement.grounded = false; return; }
    if (x_axis) actor->transform.velocity.x = 0.0f; else actor->transform.velocity.z = 0.0f;
}

static void nf_vertical_move(const NfWorld *world, NfActor *actor, float dt) {
    const float delta = actor->transform.velocity.y * dt;
    if (nf_absf(delta) < 0.000001f) return;
    NfVec3 next = actor->transform.position; next.y += delta;
    if (nf_movement_space_is_free(world, next, world->movement.radius, actor->movement.body_height)) { actor->transform.position = next; return; }
    if (delta > 0.0f) {
        float ceiling = 100000.0f;
        for (size_t i = 0; i < world->collider_count; ++i) {
            const NfCollider *c = &world->colliders[i];
            if (c->kind == NF_COLLIDER_LADDER) continue;
            if (!nf_xz_overlap(actor->transform.position, world->movement.radius, c)) continue;
            if (c->min.y >= actor->transform.position.y && c->min.y < ceiling) ceiling = c->min.y;
        }
        if (ceiling < 99999.0f) actor->transform.position.y = ceiling - actor->movement.body_height - 0.001f;
        actor->transform.velocity.y = 0.0f;
    } else {
        int support = -1;
        const float h = nf_support_height(world, actor->transform.position, world->movement.radius, actor->transform.position.y, &support);
        if (h > -99999.0f) {
            actor->transform.position.y = h;
            actor->movement.grounded = true;
            actor->movement.ground_collider = support;
            actor->movement.jumps_used = 0u;
        }
        actor->transform.velocity.y = 0.0f;
    }
}

static void nf_refresh_ground(const NfWorld *world, NfActor *actor) {
    int support = -1;
    const float support_y = nf_support_height(world, actor->transform.position, world->movement.radius, actor->transform.position.y, &support);
    const float delta = actor->transform.position.y - support_y;
    if (support_y > -99999.0f && delta >= -0.02f && delta <= 0.12f && actor->transform.velocity.y <= 0.25f) {
        actor->transform.position.y = support_y;
        actor->transform.velocity.y = 0.0f;
        actor->movement.grounded = true;
        actor->movement.ground_collider = support;
        actor->movement.jumps_used = 0u;
    } else { actor->movement.grounded = false; actor->movement.ground_collider = -1; }
}

static void nf_update_crouch(const NfWorld *world, NfActor *actor) {
    if (actor->input.crouch_held) actor->movement.crouched = true;
    else if (actor->movement.crouched && nf_movement_space_is_free(world, actor->transform.position, world->movement.radius, world->movement.stand_height)) actor->movement.crouched = false;
    actor->movement.body_height = actor->movement.crouched ? world->movement.crouch_height : world->movement.stand_height;
    actor->movement.eye_height = actor->movement.crouched ? world->movement.crouch_eye_height : world->movement.stand_eye_height;
}

void nf_movement_step_actor(NfWorld *world, NfActor *actor, float dt) {
    if (world == NULL || actor == NULL || dt <= 0.0f) return;
    nf_apply_platform_delta(world, actor);
    nf_update_crouch(world, actor);
    nf_movement_update_candidate(world, actor);

    if (actor->movement.mode == NF_MOVE_LADDER) { nf_ladder_step(world, actor, dt); return; }
    if (nf_try_enter_ladder(actor)) { nf_ladder_step(world, actor, dt); return; }

    float wish_x = 0.0f, wish_z = 0.0f, wish_mag = 0.0f;
    nf_input_wish(&actor->input, &wish_x, &wish_z, &wish_mag);

    if (actor->movement.mode == NF_MOVE_VAULT || actor->movement.mode == NF_MOVE_MANTLE) {
        actor->movement.traversal_timer -= dt;
        if (actor->movement.traversal_timer <= 0.0f) actor->movement.mode = NF_MOVE_AIR;
    } else (void)nf_begin_traversal(world, actor, wish_x, wish_z);

    if (actor->movement.mode != NF_MOVE_VAULT && actor->movement.mode != NF_MOVE_MANTLE) {
        float max_speed = world->movement.walk_speed;
        if (actor->movement.crouched) max_speed = world->movement.crouch_speed;
        else if (actor->input.sprint_held && wish_mag > 0.15f) max_speed = world->movement.sprint_speed;
        const float target_x = wish_x * max_speed;
        const float target_z = wish_z * max_speed;
        const float accel = actor->movement.grounded ? world->movement.ground_accel : world->movement.air_accel;
        if (wish_mag > 0.02f) {
            nf_approach(&actor->transform.velocity.x, target_x, accel * dt);
            nf_approach(&actor->transform.velocity.z, target_z, accel * dt);
        } else if (actor->movement.grounded) {
            nf_approach(&actor->transform.velocity.x, 0.0f, world->movement.ground_brake * dt);
            nf_approach(&actor->transform.velocity.z, 0.0f, world->movement.ground_brake * dt);
        }
        if (actor->input.jump_pressed) {
            if (actor->movement.grounded) {
                actor->transform.velocity.y = world->movement.jump_speed;
                actor->movement.grounded = false;
                actor->movement.jumps_used = 1u;
            } else if (actor->input.crouch_held && actor->movement.jumps_used == 1u) {
                actor->transform.velocity.y = world->movement.second_jump_speed;
                actor->movement.jumps_used = 2u;
            }
        }
    }

    if (!actor->movement.grounded) actor->transform.velocity.y -= world->movement.gravity * dt;
    nf_try_horizontal_axis(world, actor, actor->transform.velocity.x * dt, true);
    nf_try_horizontal_axis(world, actor, actor->transform.velocity.z * dt, false);
    nf_vertical_move(world, actor, dt);
    nf_refresh_ground(world, actor);

    if (actor->movement.mode != NF_MOVE_VAULT && actor->movement.mode != NF_MOVE_MANTLE) {
        if (!actor->movement.grounded) actor->movement.mode = NF_MOVE_AIR;
        else if (actor->movement.ground_collider >= 0 && world->colliders[actor->movement.ground_collider].kind == NF_COLLIDER_MOVING_PLATFORM) actor->movement.mode = NF_MOVE_PLATFORM;
        else if (actor->movement.crouched) actor->movement.mode = NF_MOVE_CROUCH;
        else if (actor->input.sprint_held && wish_mag > 0.15f) actor->movement.mode = NF_MOVE_SPRINT;
        else actor->movement.mode = NF_MOVE_GROUND;
    }
}
