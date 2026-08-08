#include "nf_ai.h"

#include "nf_movement.h"

#include <float.h>
#include <math.h>
#include <string.h>

#define NF_AI_PERCEPTION_INTERVAL 5u
#define NF_AI_DECISION_INTERVAL 10u
#define NF_AI_MEMORY_TICKS (NF_TICK_RATE * 6u)
#define NF_AI_REPORT_TICKS (NF_TICK_RATE * 3u)
#define NF_AI_VIEW_RANGE 36.0f

static float clamp01(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

static NfVec3 vsub(NfVec3 a, NfVec3 b) {
    return (NfVec3){a.x-b.x, a.y-b.y, a.z-b.z};
}

static float vdot(NfVec3 a, NfVec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

static float vlen(NfVec3 value) {
    return sqrtf(vdot(value, value));
}

static float dist_xz(NfVec3 a, NfVec3 b) {
    const float x = a.x - b.x;
    const float z = a.z - b.z;
    return sqrtf(x*x + z*z);
}

static uint32_t hash_u32(uint32_t value) {
    value ^= value >> 16;
    value *= 0x7feb352du;
    value ^= value >> 15;
    value *= 0x846ca68bu;
    value ^= value >> 16;
    return value;
}

static bool ray_aabb(
    NfVec3 origin, NfVec3 direction, NfVec3 minimum, NfVec3 maximum,
    float *distance) {
    float tmin = 0.0f;
    float tmax = FLT_MAX;
    const float origin_values[3] = {origin.x, origin.y, origin.z};
    const float direction_values[3] = {direction.x, direction.y, direction.z};
    const float min_values[3] = {minimum.x, minimum.y, minimum.z};
    const float max_values[3] = {maximum.x, maximum.y, maximum.z};

    for (int i = 0; i < 3; ++i) {
        if (fabsf(direction_values[i]) < 1e-6f) {
            if (origin_values[i] < min_values[i] || origin_values[i] > max_values[i]) {
                return false;
            }
            continue;
        }
        float t1 = (min_values[i] - origin_values[i]) / direction_values[i];
        float t2 = (max_values[i] - origin_values[i]) / direction_values[i];
        if (t1 > t2) {
            const float temporary = t1;
            t1 = t2;
            t2 = temporary;
        }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return false;
    }

    if (tmax < 0.0f) return false;
    if (distance != NULL) *distance = tmin >= 0.0f ? tmin : tmax;
    return true;
}

static bool line_blocked(const NfWorld *world, NfVec3 from, NfVec3 to) {
    const NfVec3 delta = vsub(to, from);
    const float distance = vlen(delta);
    if (distance < 0.001f) return false;

    const NfVec3 direction = {
        delta.x / distance,
        delta.y / distance,
        delta.z / distance
    };
    for (size_t i = 0; i < world->collider_count; ++i) {
        const NfCollider *collider = &world->colliders[i];
        if (collider->kind == NF_COLLIDER_LADDER) continue;
        float hit_distance = 0.0f;
        if (ray_aabb(from, direction, collider->min, collider->max, &hit_distance) &&
            hit_distance > 0.02f && hit_distance < distance - 0.08f) {
            return true;
        }
    }
    return false;
}

static bool is_human_target(const NfActor *actor) {
    if (actor == NULL || !actor->active || !actor->combat.alive) return false;
    return actor->faction == NF_FACTION_PLAYER || actor->faction == NF_FACTION_TEAMMATE;
}

static void release_cover(NfAiSystem *ai, NfAiAgent *agent) {
    if (agent->selected_affordance >= 0 &&
        (size_t)agent->selected_affordance < ai->affordance_count) {
        NfAiAffordance *affordance = &ai->affordances[agent->selected_affordance];
        if (affordance->reserved_by == agent->actor_id) affordance->reserved_by = 0u;
    }
    agent->selected_affordance = -1;
}

static void add_affordance(NfAiSystem *ai, const NfWorld *world, NfVec3 position) {
    if (ai->affordance_count >= NF_AI_MAX_AFFORDANCES) return;
    position.y = 0.05f;
    if (!nf_movement_space_is_free(
            world, position, world->movement.radius, world->movement.stand_height)) {
        return;
    }
    for (size_t i = 0; i < ai->affordance_count; ++i) {
        if (dist_xz(ai->affordances[i].position, position) < 1.0f) return;
    }

    const size_t index = ai->affordance_count++;
    ai->affordances[index] = (NfAiAffordance){
        .id = (uint32_t)(index + 1u),
        .position = position
    };
}

static void generate_cover_affordances(NfAiSystem *ai, const NfWorld *world) {
    ai->affordance_count = 0u;
    for (size_t i = 0;
         i < world->collider_count && ai->affordance_count < NF_AI_MAX_AFFORDANCES;
         ++i) {
        const NfCollider *collider = &world->colliders[i];
        if (collider->kind == NF_COLLIDER_LADDER) continue;

        const float height = collider->max.y - collider->min.y;
        const float width = collider->max.x - collider->min.x;
        const float depth = collider->max.z - collider->min.z;
        if (height < 0.35f || height > 2.6f || width > 15.0f || depth > 15.0f) {
            continue;
        }

        const float center_x = 0.5f * (collider->min.x + collider->max.x);
        const float center_z = 0.5f * (collider->min.z + collider->max.z);
        const float offset_x = 0.5f * width + 0.9f;
        const float offset_z = 0.5f * depth + 0.9f;
        add_affordance(ai, world, (NfVec3){center_x-offset_x, 0.05f, center_z});
        add_affordance(ai, world, (NfVec3){center_x+offset_x, 0.05f, center_z});
        add_affordance(ai, world, (NfVec3){center_x, 0.05f, center_z-offset_z});
        add_affordance(ai, world, (NfVec3){center_x, 0.05f, center_z+offset_z});
    }
}

static int choose_cover(
    NfAiSystem *ai, const NfWorld *world, const NfAiAgent *agent, NfVec3 threat) {
    const NfActor *self = nf_world_find_actor_const(world, agent->actor_id);
    if (self == NULL) return -1;

    float best_score = -FLT_MAX;
    int best_index = -1;
    const NfVec3 threat_eye = {threat.x, threat.y+1.4f, threat.z};
    for (size_t i = 0; i < ai->affordance_count; ++i) {
        const NfAiAffordance *affordance = &ai->affordances[i];
        if (affordance->reserved_by != 0u && affordance->reserved_by != agent->actor_id) {
            continue;
        }
        const float travel = dist_xz(self->transform.position, affordance->position);
        if (travel > 18.0f) continue;

        const NfVec3 cover_eye = {
            affordance->position.x,
            affordance->position.y+1.0f,
            affordance->position.z
        };
        const bool blocks_threat = line_blocked(world, threat_eye, cover_eye);
        const float score = (blocks_threat ? 0.70f : 0.12f) +
            (1.0f-clamp01(travel/18.0f))*0.30f;
        if (score > best_score) {
            best_score = score;
            best_index = (int)i;
        }
    }
    return best_index;
}

static void squad_report(
    NfAiSystem *ai, NfEntityId target, NfVec3 position, uint64_t tick) {
    ai->blackboard.reported_target = target;
    ai->blackboard.reported_position = position;
    ai->blackboard.reported_tick = tick;
    ai->blackboard.confidence = 0.72f;
}

static void perceive(
    NfAiSystem *ai, NfAiAgent *agent, NfWorld *world,
    const NfSemanticBus *semantics) {
    NfActor *self = nf_world_find_actor(world, agent->actor_id);
    if (self == NULL || !self->combat.alive) return;

    NfEntityId best_id = 0u;
    NfVec3 best_position = {0};
    float best_distance = FLT_MAX;
    const float facing_x = sinf(agent->yaw);
    const float facing_z = cosf(agent->yaw);
    const NfVec3 eye = {
        self->transform.position.x,
        self->transform.position.y+self->movement.eye_height,
        self->transform.position.z
    };

    for (size_t i = 0; i < NF_MAX_ENTITIES; ++i) {
        const NfActor *other = &world->actors[i];
        if (!is_human_target(other)) continue;

        const NfRelationship relation = nf_relation_between(
            self->faction, other->faction, ai->rival_relationship);
        if (relation == NF_RELATION_COOPERATIVE || relation == NF_RELATION_NEUTRAL) {
            continue;
        }

        const float delta_x = other->transform.position.x - self->transform.position.x;
        const float delta_z = other->transform.position.z - self->transform.position.z;
        const float range = sqrtf(delta_x*delta_x + delta_z*delta_z);
        if (range > NF_AI_VIEW_RANGE || range < 0.001f) continue;

        const float facing = (facing_x*delta_x + facing_z*delta_z) / range;
        if (facing < -0.18f) continue;

        const NfVec3 target_eye = {
            other->transform.position.x,
            other->transform.position.y + other->movement.eye_height*0.75f,
            other->transform.position.z
        };
        if (line_blocked(world, eye, target_eye)) continue;

        if (range < best_distance) {
            best_distance = range;
            best_id = other->id;
            best_position = other->transform.position;
        }
    }

    const bool was_visible = agent->knowledge.visible_now;
    const NfEntityId previous_target = agent->knowledge.target;
    agent->knowledge.visible_now = best_id != 0u;

    if (best_id != 0u) {
        agent->knowledge.target = best_id;
        agent->knowledge.last_seen_position = best_position;
        agent->knowledge.last_seen_tick = world->tick;
        agent->knowledge.confidence = 1.0f;
        squad_report(ai, best_id, best_position, world->tick);
        if (!was_visible || previous_target != best_id) {
            const uint32_t reaction_delay = 15u +
                hash_u32(ai->seed ^ agent->actor_id ^ (uint32_t)world->tick) % 7u;
            agent->reaction_ready_tick = world->tick + reaction_delay;
        }
        return;
    }

    NfSemanticAlert heard[8];
    const size_t heard_count = nf_semantic_collect_audible(
        semantics, self->transform.position, world->tick, heard, 8u);
    for (size_t i = 0; i < heard_count; ++i) {
        const NfSemanticAlert *event = &heard[i];
        const NfActor *source = nf_world_find_actor_const(world, event->source);
        if (source == NULL || !is_human_target(source)) continue;
        const bool relevant_damage = event->type == NF_SEMANTIC_DAMAGE_TAKEN &&
            event->subject == agent->actor_id;
        if (event->type == NF_SEMANTIC_GUNFIRE || relevant_damage) {
            agent->knowledge.target = event->source;
            agent->knowledge.last_heard_position = event->position;
            agent->knowledge.last_heard_tick = world->tick;
            if (agent->knowledge.confidence < 0.65f) agent->knowledge.confidence = 0.65f;
            break;
        }
    }

    if (ai->blackboard.reported_target != 0u &&
        world->tick >= ai->blackboard.reported_tick &&
        world->tick - ai->blackboard.reported_tick <= NF_AI_REPORT_TICKS &&
        agent->knowledge.confidence < ai->blackboard.confidence*0.85f) {
        agent->knowledge.target = ai->blackboard.reported_target;
        agent->knowledge.last_heard_position = ai->blackboard.reported_position;
        agent->knowledge.last_heard_tick = ai->blackboard.reported_tick;
        agent->knowledge.confidence = ai->blackboard.confidence*0.85f;
    }

    const uint64_t evidence_tick = agent->knowledge.last_seen_tick > agent->knowledge.last_heard_tick
        ? agent->knowledge.last_seen_tick
        : agent->knowledge.last_heard_tick;
    if (evidence_tick == 0u || world->tick < evidence_tick ||
        world->tick - evidence_tick > NF_AI_MEMORY_TICKS) {
        agent->knowledge.confidence = 0.0f;
        agent->knowledge.target = 0u;
    } else {
        agent->knowledge.confidence = clamp01(agent->knowledge.confidence - 0.045f);
    }
}

static float range_fit(float range) {
    const float distance_from_ideal = fabsf(range - 15.0f);
    return clamp01(1.0f - distance_from_ideal/18.0f);
}

static void decide(NfAiSystem *ai, NfAiAgent *agent, NfWorld *world) {
    NfActor *self = nf_world_find_actor(world, agent->actor_id);
    if (self == NULL) return;

    for (size_t i = 0; i < NF_AGENT_MODE_COUNT; ++i) agent->mode_scores[i] = 0.0f;

    NfRelationship relation = ai->rival_relationship;
    if (agent->knowledge.target != 0u) {
        const NfActor *target = nf_world_find_actor_const(world, agent->knowledge.target);
        if (target != NULL) {
            relation = nf_relation_between(
                self->faction, target->faction, ai->rival_relationship);
        }
    }

    if (!self->combat.alive) {
        agent->mode_scores[NF_AGENT_IDLE] = 1.0f;
    } else if (!nf_relation_can_damage(relation, false)) {
        agent->mode_scores[NF_AGENT_TRUCE_HOLD] = 1.0f;
        agent->mode_scores[NF_AGENT_IDLE] = 0.2f;
    } else {
        const NfWeaponSpec *spec = nf_weapon_spec(self->combat.weapon);
        const float health = clamp01(self->health/100.0f);
        const float ammo = spec->magazine_size > 0u
            ? (float)self->combat.ammo_mag[self->combat.weapon] / (float)spec->magazine_size
            : 0.0f;
        const NfVec3 known = agent->knowledge.visible_now
            ? agent->knowledge.last_seen_position
            : agent->knowledge.last_heard_position;
        const float range = agent->knowledge.target != 0u
            ? dist_xz(self->transform.position, known)
            : NF_AI_VIEW_RANGE;

        agent->mode_scores[NF_AGENT_IDLE] = 0.08f;
        agent->mode_scores[NF_AGENT_INVESTIGATE] =
            (!agent->knowledge.visible_now ? agent->knowledge.confidence : 0.0f)*0.78f;
        agent->mode_scores[NF_AGENT_ADVANCE] =
            agent->knowledge.confidence*clamp01((range-16.0f)/18.0f)*0.82f;
        agent->mode_scores[NF_AGENT_ENGAGE] =
            (agent->knowledge.visible_now ? 1.0f : 0.0f)*range_fit(range)*
            (0.45f+0.55f*health)*(ammo > 0.0f ? 1.0f : 0.0f);
        agent->mode_scores[NF_AGENT_RETREAT] =
            (1.0f-health)*0.66f + clamp01((7.0f-range)/7.0f)*0.38f;
        agent->mode_scores[NF_AGENT_RELOAD] =
            (self->combat.ammo_mag[self->combat.weapon] == 0u &&
             self->combat.reserve_ammo[self->combat.weapon] > 0u)
                ? 1.0f
                : (1.0f-ammo)*0.22f;

        const int cover = choose_cover(ai, world, agent, known);
        agent->mode_scores[NF_AGENT_SEEK_COVER] = cover >= 0
            ? (1.0f-health)*0.58f + (1.0f-ammo)*0.20f +
                (agent->knowledge.visible_now ? 0.18f : 0.0f)
            : 0.0f;
    }

    NfAgentMode best_mode = NF_AGENT_IDLE;
    float best_score = agent->mode_scores[best_mode];
    for (int i = 1; i < (int)NF_AGENT_MODE_COUNT; ++i) {
        if (agent->mode_scores[i] > best_score) {
            best_mode = (NfAgentMode)i;
            best_score = agent->mode_scores[i];
        }
    }

    const float current_score = agent->mode_scores[agent->mode];
    if (agent->mode != best_mode && current_score + 0.10f >= best_score &&
        current_score > 0.05f) {
        best_mode = agent->mode;
        best_score = current_score;
    }

    if (best_mode != NF_AGENT_SEEK_COVER) {
        release_cover(ai, agent);
    } else if (agent->selected_affordance < 0) {
        const NfVec3 threat = agent->knowledge.visible_now
            ? agent->knowledge.last_seen_position
            : agent->knowledge.last_heard_position;
        const int cover = choose_cover(ai, world, agent, threat);
        if (cover >= 0) {
            agent->selected_affordance = cover;
            ai->affordances[cover].reserved_by = agent->actor_id;
        }
    }

    agent->mode = best_mode;
    agent->current_score = best_score;
}

static NfVec3 known_position(const NfAiAgent *agent) {
    return agent->knowledge.visible_now
        ? agent->knowledge.last_seen_position
        : agent->knowledge.last_heard_position;
}

static void aim_at(
    NfAiAgent *agent, const NfActor *self, NfVec3 target, uint64_t tick,
    uint32_t seed) {
    const float delta_x = target.x - self->transform.position.x;
    const float delta_z = target.z - self->transform.position.z;
    const float horizontal = sqrtf(delta_x*delta_x + delta_z*delta_z);
    const float eye_y = self->transform.position.y + self->movement.eye_height;
    const float phase = (float)((tick + hash_u32(seed ^ agent->actor_id)) % 1000u);

    agent->yaw = atan2f(delta_x, delta_z);
    agent->pitch = atan2f(
        (target.y+1.05f)-eye_y,
        horizontal > 0.01f ? horizontal : 0.01f);
    agent->yaw += sinf(phase*0.071f)*0.022f + cosf(phase*0.037f)*0.012f;
    agent->pitch += sinf(phase*0.053f)*0.012f;
}

static NfControlFrame build_control(
    NfAiSystem *ai, NfAiAgent *agent, NfWorld *world) {
    NfControlFrame frame = {
        .actor = agent->actor_id,
        .source = NF_CONTROL_AI,
        .perceived_tick = world->tick,
        .sequence = ++agent->control_sequence
    };
    NfActor *self = nf_world_find_actor(world, agent->actor_id);
    if (self == NULL || !self->combat.alive) return frame;

    const NfVec3 target = known_position(agent);
    const float range = agent->knowledge.target != 0u
        ? dist_xz(self->transform.position, target)
        : 0.0f;
    bool moving = false;

    if (agent->mode == NF_AGENT_ADVANCE || agent->mode == NF_AGENT_INVESTIGATE) {
        agent->yaw = atan2f(
            target.x-self->transform.position.x,
            target.z-self->transform.position.z);
        frame.move.forward = 1.0f;
        frame.move.sprint_held = range > 8.0f;
        moving = true;
    } else if (agent->mode == NF_AGENT_SEEK_COVER &&
               agent->selected_affordance >= 0 &&
               (size_t)agent->selected_affordance < ai->affordance_count) {
        const NfVec3 goal = ai->affordances[agent->selected_affordance].position;
        const float distance = dist_xz(self->transform.position, goal);
        agent->yaw = atan2f(
            goal.x-self->transform.position.x,
            goal.z-self->transform.position.z);
        frame.move.forward = distance > 0.7f ? 1.0f : 0.0f;
        frame.move.sprint_held = distance > 5.0f;
        moving = distance > 0.7f;
    } else if (agent->mode == NF_AGENT_RETREAT && agent->knowledge.target != 0u) {
        agent->yaw = atan2f(
            self->transform.position.x-target.x,
            self->transform.position.z-target.z);
        frame.move.forward = 1.0f;
        frame.move.sprint_held = true;
        moving = true;
    } else if (agent->mode == NF_AGENT_ENGAGE && agent->knowledge.visible_now) {
        aim_at(agent, self, target, world->tick, ai->seed);
        frame.move.forward = range > 17.0f ? 0.35f : 0.0f;
        if (agent->role == NF_SQUAD_FLANK_LEFT) {
            frame.move.strafe = -0.65f;
        } else if (agent->role == NF_SQUAD_FLANK_RIGHT) {
            frame.move.strafe = 0.65f;
        } else if (agent->role == NF_SQUAD_PRESSURE) {
            frame.move.strafe = (float)agent->strafe_sign*0.28f;
        }
        moving = fabsf(frame.move.forward) > 0.05f || fabsf(frame.move.strafe) > 0.05f;

        const uint32_t period = agent->role == NF_SQUAD_PRESSURE
            ? 90u
            : (agent->role == NF_SQUAD_HOLD ? 120u : 105u);
        const uint32_t burst = agent->role == NF_SQUAD_PRESSURE
            ? 18u
            : (agent->role == NF_SQUAD_HOLD ? 10u : 13u);
        const uint32_t phase = (uint32_t)(
            (world->tick + hash_u32(ai->seed ^ agent->actor_id)) % period);
        if (world->tick >= agent->reaction_ready_tick && phase < burst) {
            frame.combat.fire_held = true;
            frame.combat.fire_pressed = phase == 0u;
        }
    } else if (agent->mode == NF_AGENT_TRUCE_HOLD && agent->knowledge.visible_now) {
        aim_at(agent, self, target, world->tick, ai->seed);
    }

    if (agent->mode == NF_AGENT_RELOAD &&
        self->combat.state != NF_WEAPON_RELOADING) {
        frame.combat.reload_pressed = true;
    }

    frame.move.yaw_radians = agent->yaw;
    frame.combat.aim_pitch_radians = agent->pitch;

    if (moving && self->movement.candidate.active) {
        if (self->movement.candidate.type == NF_TRAVERSAL_LADDER) {
            frame.move.interact_held = true;
        } else if (self->movement.candidate.type == NF_TRAVERSAL_VAULT ||
                   self->movement.candidate.type == NF_TRAVERSAL_MANTLE) {
            frame.move.jump_pressed = true;
        }
    }

    if (agent->stuck_ticks > NF_TICK_RATE) {
        frame.move.jump_pressed = true;
        frame.move.strafe = (float)agent->strafe_sign;
        agent->strafe_sign = -agent->strafe_sign;
        agent->stuck_ticks = 0u;
    }

    agent->movement_requested = moving;
    return frame;
}

void nf_ai_init(NfAiSystem *ai, NfWorld *world, size_t count, uint32_t seed) {
    if (ai == NULL || world == NULL) return;

    static const NfVec3 spawns[NF_AI_MAX_AGENTS] = {
        {-12.0f,0.05f,-8.0f},
        {10.0f,0.05f,-8.0f},
        {-12.0f,0.05f,10.0f},
        {10.0f,0.05f,10.0f}
    };
    static const NfSquadRole roles[NF_AI_MAX_AGENTS] = {
        NF_SQUAD_PRESSURE,
        NF_SQUAD_FLANK_LEFT,
        NF_SQUAD_FLANK_RIGHT,
        NF_SQUAD_HOLD
    };

    memset(ai, 0, sizeof(*ai));
    ai->seed = seed;
    ai->rival_relationship = NF_RELATION_HOSTILE;
    if (count > NF_AI_MAX_AGENTS) count = NF_AI_MAX_AGENTS;

    for (size_t i = 0; i < count; ++i) {
        const NfEntityId id = nf_world_spawn_actor(
            world, NF_FACTION_RIVAL, spawns[i]);
        NfAiAgent *agent = &ai->agents[ai->count++];
        *agent = (NfAiAgent){
            .actor_id = id,
            .mode = NF_AGENT_IDLE,
            .role = roles[i],
            .yaw = atan2f(-3.0f-spawns[i].x, -18.0f-spawns[i].z),
            .next_perception_tick = (uint64_t)i,
            .next_decision_tick = (uint64_t)(i*2u),
            .selected_affordance = -1,
            .strafe_sign = (i%2u) ? -1 : 1,
            .spawn = spawns[i],
            .last_sample_position = spawns[i]
        };
    }
    generate_cover_affordances(ai, world);
}

void nf_ai_set_rival_relationship(
    NfAiSystem *ai, NfRelationship relationship) {
    if (ai != NULL) ai->rival_relationship = relationship;
}

size_t nf_ai_tick(
    NfAiSystem *ai, NfWorld *world, const NfSemanticBus *semantics,
    NfControlFrame *out, size_t cap) {
    if (ai == NULL || world == NULL || semantics == NULL || out == NULL) return 0u;

    if (ai->blackboard.reported_target != 0u &&
        world->tick >= ai->blackboard.reported_tick &&
        world->tick - ai->blackboard.reported_tick > NF_AI_REPORT_TICKS) {
        ai->blackboard.confidence = 0.0f;
    }

    size_t written = 0u;
    for (size_t i = 0; i < ai->count && written < cap; ++i) {
        NfAiAgent *agent = &ai->agents[i];
        NfActor *self = nf_world_find_actor(world, agent->actor_id);
        if (self == NULL) continue;

        if (world->tick >= agent->next_perception_tick) {
            perceive(ai, agent, world, semantics);
            agent->next_perception_tick =
                world->tick + NF_AI_PERCEPTION_INTERVAL + (uint64_t)i%3u;

            if ((world->tick%30u) == (uint64_t)i%30u) {
                const float moved = dist_xz(
                    self->transform.position, agent->last_sample_position);
                if (agent->movement_requested && moved < 0.12f) {
                    agent->stuck_ticks += 30u;
                } else {
                    agent->stuck_ticks = 0u;
                }
                agent->last_sample_position = self->transform.position;
            }
        }

        if (world->tick >= agent->next_decision_tick) {
            decide(ai, agent, world);
            agent->next_decision_tick =
                world->tick + NF_AI_DECISION_INTERVAL + (uint64_t)i%4u;
        }

        out[written++] = build_control(ai, agent, world);
    }
    return written;
}

const NfAiAgent *nf_ai_find_agent_const(
    const NfAiSystem *ai, NfEntityId actor_id) {
    if (ai == NULL) return NULL;
    for (size_t i = 0; i < ai->count; ++i) {
        if (ai->agents[i].actor_id == actor_id) return &ai->agents[i];
    }
    return NULL;
}

bool nf_ai_spawn_for(
    const NfAiSystem *ai, NfEntityId actor_id, NfVec3 *out) {
    if (out == NULL) return false;
    const NfAiAgent *agent = nf_ai_find_agent_const(ai, actor_id);
    if (agent == NULL) return false;
    *out = agent->spawn;
    return true;
}

void nf_ai_on_respawn(NfAiSystem *ai, NfEntityId actor_id) {
    if (ai == NULL) return;
    for (size_t i = 0; i < ai->count; ++i) {
        NfAiAgent *agent = &ai->agents[i];
        if (agent->actor_id != actor_id) continue;

        release_cover(ai, agent);
        const NfSquadRole role = agent->role;
        const NfVec3 spawn = agent->spawn;
        const uint32_t sequence = agent->control_sequence;
        memset(&agent->knowledge, 0, sizeof(agent->knowledge));
        agent->mode = NF_AGENT_IDLE;
        agent->role = role;
        agent->spawn = spawn;
        agent->control_sequence = sequence;
        agent->selected_affordance = -1;
        agent->strafe_sign = (i%2u) ? -1 : 1;
        agent->last_sample_position = spawn;
        agent->stuck_ticks = 0u;
        agent->movement_requested = false;
        return;
    }
}
