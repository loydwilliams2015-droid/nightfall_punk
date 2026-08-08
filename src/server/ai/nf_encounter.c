#include "nf_encounter.h"

#include "nf_combat.h"

#include <math.h>
#include <string.h>

#define NF_ENCOUNTER_ROLE_INTERVAL 30u
#define NF_ENCOUNTER_YAW_STEP 0.040f
#define NF_ENCOUNTER_PITCH_STEP 0.030f
#define NF_ENCOUNTER_SUPPRESSION_DECAY 0.0045f

static float clamp01(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

static float absf_local(float value) {
    return value < 0.0f ? -value : value;
}

static float wrap_angle(float value) {
    const float pi = 3.14159265358979323846f;
    const float tau = 6.28318530717958647692f;
    while (value > pi) value -= tau;
    while (value < -pi) value += tau;
    return value;
}

static float approach_angle(float current, float target, float max_step) {
    const float delta = wrap_angle(target-current);
    if (delta > max_step) return wrap_angle(current+max_step);
    if (delta < -max_step) return wrap_angle(current-max_step);
    return wrap_angle(target);
}

static float approach_linear(float current, float target, float max_step) {
    const float delta = target-current;
    if (delta > max_step) return current+max_step;
    if (delta < -max_step) return current-max_step;
    return target;
}

static float distance_xz(NfVec3 a, NfVec3 b) {
    const float dx = a.x-b.x;
    const float dz = a.z-b.z;
    return sqrtf(dx*dx+dz*dz);
}

static float speed_xz(NfVec3 velocity) {
    return sqrtf(velocity.x*velocity.x+velocity.z*velocity.z);
}

static uint32_t hash_u32(uint32_t value) {
    value ^= value >> 16;
    value *= 0x7feb352du;
    value ^= value >> 15;
    value *= 0x846ca68bu;
    value ^= value >> 16;
    return value;
}

static NfEncounterAgentState *state_for(
    NfEncounterState *encounter, NfEntityId actor_id) {
    if (encounter == NULL) return NULL;
    for (size_t i = 0; i < encounter->count; ++i) {
        if (encounter->agents[i].actor_id == actor_id) return &encounter->agents[i];
    }
    return NULL;
}

const NfEncounterAgentState *nf_encounter_agent_state_const(
    const NfEncounterState *encounter, NfEntityId actor_id) {
    if (encounter == NULL) return NULL;
    for (size_t i = 0; i < encounter->count; ++i) {
        if (encounter->agents[i].actor_id == actor_id) return &encounter->agents[i];
    }
    return NULL;
}

static NfVec3 known_position(const NfAiAgent *agent) {
    if (agent->knowledge.last_seen_tick >= agent->knowledge.last_heard_tick &&
        agent->knowledge.last_seen_tick != 0u) {
        return agent->knowledge.last_seen_position;
    }
    return agent->knowledge.last_heard_position;
}

static float pressure_bid(
    const NfAiAgent *agent, const NfActor *body, const NfWorld *world) {
    if (agent == NULL || body == NULL || !body->combat.alive ||
        agent->knowledge.target == 0u || agent->knowledge.confidence <= 0.05f) {
        return 0.0f;
    }
    const NfWeaponSpec *spec = nf_weapon_spec(body->combat.weapon);
    const float ammo = spec->magazine_size > 0u
        ? (float)body->combat.ammo_mag[body->combat.weapon]/(float)spec->magazine_size
        : 0.0f;
    const float health = clamp01(body->health/100.0f);
    const float range = distance_xz(body->transform.position, known_position(agent));
    const float range_score = clamp01(1.0f-absf_local(range-15.0f)/22.0f);
    const float visible = agent->knowledge.visible_now ? 1.0f : 0.45f;
    (void)world;
    return agent->knowledge.confidence*visible*(0.30f+0.70f*health)*
        (0.30f+0.70f*ammo)*(0.45f+0.55f*range_score);
}

static void assign_roles(
    NfEncounterState *encounter, NfAiSystem *ai, const NfWorld *world) {
    float bids[NF_AI_MAX_AGENTS] = {0};
    bool used[NF_AI_MAX_AGENTS] = {0};
    size_t order[NF_AI_MAX_AGENTS] = {0};

    for (size_t i = 0; i < ai->count; ++i) {
        const NfActor *body = nf_world_find_actor_const(world, ai->agents[i].actor_id);
        bids[i] = pressure_bid(&ai->agents[i], body, world);
        order[i] = i;
        NfEncounterAgentState *state = state_for(encounter, ai->agents[i].actor_id);
        if (state != NULL) state->pressure_authorized = false;
    }

    for (size_t rank = 0; rank < ai->count; ++rank) {
        size_t best = ai->count;
        float best_bid = -1.0f;
        for (size_t i = 0; i < ai->count; ++i) {
            if (!used[i] && bids[i] > best_bid) {
                best = i;
                best_bid = bids[i];
            }
        }
        if (best == ai->count) break;
        order[rank] = best;
        used[best] = true;
    }

    if (ai->count == 0u) return;

    for (size_t i = 0; i < ai->count; ++i) ai->agents[i].role = NF_SQUAD_HOLD;

    const size_t first = order[0];
    ai->agents[first].role = NF_SQUAD_PRESSURE;

    if (ai->count > 1u) {
        const size_t second = order[1];
        const NfVec3 target = known_position(&ai->agents[second]);
        const NfActor *body = nf_world_find_actor_const(world, ai->agents[second].actor_id);
        ai->agents[second].role = body != NULL && body->transform.position.x < target.x
            ? NF_SQUAD_FLANK_LEFT
            : NF_SQUAD_FLANK_RIGHT;
    }

    if (ai->count > 2u) {
        const size_t third = order[2];
        const NfSquadRole second_role = ai->agents[order[1]].role;
        ai->agents[third].role = second_role == NF_SQUAD_FLANK_LEFT
            ? NF_SQUAD_FLANK_RIGHT
            : NF_SQUAD_FLANK_LEFT;
    }

    for (size_t rank = 0; rank < ai->count && rank < encounter->pressure_slots; ++rank) {
        if (bids[order[rank]] <= 0.05f) continue;
        NfEncounterAgentState *state = state_for(
            encounter, ai->agents[order[rank]].actor_id);
        if (state != NULL) state->pressure_authorized = true;
    }
}

static void update_suppression(
    NfEncounterAgentState *state, const NfActor *body) {
    if (state == NULL || body == NULL) return;

    if (!body->combat.alive) {
        state->suppression = clamp01(state->suppression-NF_ENCOUNTER_SUPPRESSION_DECAY);
        state->aim_settle = 0.0f;
        state->last_health = body->health;
        return;
    }

    if (body->health > state->last_health+20.0f) {
        state->suppression = 0.0f;
        state->aim_settle = 0.0f;
    } else if (body->health+0.01f < state->last_health) {
        const float damage = state->last_health-body->health;
        state->suppression = clamp01(
            state->suppression+0.20f+clamp01(damage/50.0f)*0.50f);
    } else {
        state->suppression = clamp01(
            state->suppression-NF_ENCOUNTER_SUPPRESSION_DECAY);
    }
    state->last_health = body->health;
}

static void filter_aim_and_fire(
    NfEncounterState *encounter,
    NfAiAgent *agent,
    NfActor *body,
    NfControlFrame *control,
    NfWorld *world) {
    NfEncounterAgentState *state = state_for(encounter, agent->actor_id);
    if (state == NULL || body == NULL) return;

    update_suppression(state, body);

    if (state->last_target != agent->knowledge.target) {
        state->last_target = agent->knowledge.target;
        state->aim_settle = 0.0f;
    }

    const float desired_yaw = control->move.yaw_radians;
    const float desired_pitch = control->combat.aim_pitch_radians;
    const float yaw_step = NF_ENCOUNTER_YAW_STEP*(1.0f-0.28f*state->suppression);
    const float pitch_step = NF_ENCOUNTER_PITCH_STEP*(1.0f-0.24f*state->suppression);

    state->tracked_yaw = approach_angle(
        state->tracked_yaw, desired_yaw, yaw_step > 0.012f ? yaw_step : 0.012f);
    state->tracked_pitch = approach_linear(
        state->tracked_pitch, desired_pitch, pitch_step > 0.010f ? pitch_step : 0.010f);
    state->last_aim_error = absf_local(wrap_angle(desired_yaw-state->tracked_yaw))+
        absf_local(desired_pitch-state->tracked_pitch);

    control->move.yaw_radians = state->tracked_yaw;
    control->combat.aim_pitch_radians = state->tracked_pitch;
    agent->yaw = state->tracked_yaw;
    agent->pitch = state->tracked_pitch;

    float target_speed = 0.0f;
    if (agent->knowledge.visible_now && agent->knowledge.target != 0u) {
        const NfActor *target = nf_world_find_actor_const(world, agent->knowledge.target);
        if (target != NULL) target_speed = speed_xz(target->transform.velocity);
    }
    const float own_motion = absf_local(control->move.forward)+absf_local(control->move.strafe);

    if (agent->mode == NF_AGENT_ENGAGE && agent->knowledge.visible_now) {
        const float tolerance = 0.105f+clamp01(target_speed/8.0f)*0.040f;
        if (state->last_aim_error <= tolerance) {
            float gain = 0.032f;
            gain *= 1.0f-0.45f*clamp01(target_speed/9.0f);
            gain *= 1.0f-0.38f*state->suppression;
            gain *= 1.0f-0.10f*clamp01(own_motion);
            state->aim_settle = clamp01(state->aim_settle+gain);
        } else {
            state->aim_settle = clamp01(state->aim_settle-0.040f);
        }
    } else {
        state->aim_settle = clamp01(state->aim_settle-0.055f);
    }

    if (state->suppression > 0.78f && body->health < 75.0f &&
        agent->knowledge.target != 0u) {
        const NfVec3 threat = known_position(agent);
        const float dx = body->transform.position.x-threat.x;
        const float dz = body->transform.position.z-threat.z;
        const float retreat_yaw = atan2f(dx, dz);
        state->tracked_yaw = approach_angle(state->tracked_yaw, retreat_yaw, 0.055f);
        control->move.yaw_radians = state->tracked_yaw;
        control->move.forward = 1.0f;
        control->move.strafe = 0.0f;
        control->move.sprint_held = true;
        control->combat.fire_held = false;
        control->combat.fire_pressed = false;
        agent->mode = NF_AGENT_RETREAT;
    }

    if (control->combat.fire_held || control->combat.fire_pressed) {
        const float required_settle = 0.42f+
            0.16f*clamp01(target_speed/8.0f)+
            0.14f*state->suppression+
            0.05f*clamp01(own_motion);
        bool allow = agent->knowledge.visible_now &&
            state->aim_settle >= required_settle && state->suppression < 0.82f;

        if (allow && !state->pressure_authorized) {
            const uint32_t phase = (uint32_t)(
                (world->tick+hash_u32(encounter->seed^agent->actor_id))%180u);
            const NfActor *target = nf_world_find_actor_const(world, agent->knowledge.target);
            const float range = target != NULL
                ? distance_xz(body->transform.position, target->transform.position)
                : 999.0f;
            allow = range < 11.0f || phase < 6u;
        }

        if (!allow) {
            control->combat.fire_held = false;
            control->combat.fire_pressed = false;
        }
    }
}

void nf_encounter_init(
    NfEncounterState *encounter,
    const NfAiSystem *ai,
    const NfWorld *world,
    size_t pressure_slots,
    uint32_t seed) {
    if (encounter == NULL || ai == NULL || world == NULL) return;
    memset(encounter, 0, sizeof(*encounter));
    encounter->seed = seed;
    encounter->count = ai->count;
    if (encounter->count > NF_AI_MAX_AGENTS) encounter->count = NF_AI_MAX_AGENTS;
    encounter->pressure_slots = pressure_slots;
    if (encounter->pressure_slots > NF_ENCOUNTER_MAX_PRESSURE_SLOTS) {
        encounter->pressure_slots = NF_ENCOUNTER_MAX_PRESSURE_SLOTS;
    }
    if (encounter->pressure_slots > encounter->count) {
        encounter->pressure_slots = encounter->count;
    }

    for (size_t i = 0; i < encounter->count; ++i) {
        const NfAiAgent *agent = &ai->agents[i];
        const NfActor *body = nf_world_find_actor_const(world, agent->actor_id);
        encounter->agents[i] = (NfEncounterAgentState){
            .actor_id = agent->actor_id,
            .tracked_yaw = agent->yaw,
            .tracked_pitch = agent->pitch,
            .last_health = body != NULL ? body->health : 100.0f
        };
    }
}

void nf_encounter_filter_controls(
    NfEncounterState *encounter,
    NfAiSystem *ai,
    NfWorld *world,
    NfControlFrame *controls,
    size_t control_count) {
    if (encounter == NULL || ai == NULL || world == NULL || controls == NULL) return;

    if (world->tick >= encounter->next_role_tick) {
        assign_roles(encounter, ai, world);
        encounter->next_role_tick = world->tick+NF_ENCOUNTER_ROLE_INTERVAL;
    }

    for (size_t i = 0; i < control_count; ++i) {
        NfAiAgent *agent = NULL;
        for (size_t j = 0; j < ai->count; ++j) {
            if (ai->agents[j].actor_id == controls[i].actor) {
                agent = &ai->agents[j];
                break;
            }
        }
        if (agent == NULL) continue;
        NfActor *body = nf_world_find_actor(world, agent->actor_id);
        filter_aim_and_fire(encounter, agent, body, &controls[i], world);
    }
}

size_t nf_encounter_pressure_count(const NfEncounterState *encounter) {
    if (encounter == NULL) return 0u;
    size_t count = 0u;
    for (size_t i = 0; i < encounter->count; ++i) {
        if (encounter->agents[i].pressure_authorized) ++count;
    }
    return count;
}
