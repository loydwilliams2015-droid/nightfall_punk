#include "nf_ai.h"

#include "nf_movement.h"

#include <float.h>
#include <math.h>
#include <string.h>

#define NF_AI_PERCEPTION_INTERVAL 5u
#define NF_AI_DECISION_INTERVAL 10u
#define NF_AI_VIEW_RANGE 36.0f
#define NF_AI_REPORT_COOLDOWN_TICKS 30u
#define NF_AI_REPORT_DELAY_TICKS 18u

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
    const float dx = a.x-b.x;
    const float dz = a.z-b.z;
    return sqrtf(dx*dx + dz*dz);
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
    const float ov[3] = {origin.x, origin.y, origin.z};
    const float dv[3] = {direction.x, direction.y, direction.z};
    const float lo[3] = {minimum.x, minimum.y, minimum.z};
    const float hi[3] = {maximum.x, maximum.y, maximum.z};
    for (int i = 0; i < 3; ++i) {
        if (fabsf(dv[i]) < 1e-6f) {
            if (ov[i] < lo[i] || ov[i] > hi[i]) return false;
            continue;
        }
        float t1 = (lo[i]-ov[i])/dv[i];
        float t2 = (hi[i]-ov[i])/dv[i];
        if (t1 > t2) {
            const float t = t1;
            t1 = t2;
            t2 = t;
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
    const float length = vlen(delta);
    if (length < 0.001f) return false;
    const NfVec3 direction = {delta.x/length, delta.y/length, delta.z/length};
    for (size_t i = 0u; i < world->collider_count; ++i) {
        const NfCollider *collider = &world->colliders[i];
        if (collider->kind == NF_COLLIDER_LADDER) continue;
        float hit = 0.0f;
        if (ray_aabb(from, direction, collider->min, collider->max, &hit) &&
            hit > 0.02f && hit < length-0.08f) return true;
    }
    return false;
}

static bool is_adversary_target(const NfActor *actor) {
    if (actor == NULL || !actor->active || !actor->combat.alive) return false;
    return actor->faction == NF_FACTION_PLAYER ||
        actor->faction == NF_FACTION_TEAMMATE ||
        actor->faction == NF_FACTION_CATTLER;
}

static void release_cover(NfAiSystem *ai, NfAiAgent *agent) {
    if (agent->selected_affordance >= 0 &&
        (size_t)agent->selected_affordance < ai->affordance_count &&
        ai->claims != NULL) {
        const NfAiAffordance *affordance = &ai->affordances[agent->selected_affordance];
        (void)nf_claim_release(
            ai->claims, NF_CLAIM_COVER, agent->actor_id, affordance->id);
    }
    agent->selected_affordance = -1;
}

static void add_affordance(NfAiSystem *ai, const NfWorld *world, NfVec3 position) {
    if (ai->affordance_count >= NF_AI_MAX_AFFORDANCES) return;
    position.y = 0.05f;
    if (!nf_movement_space_is_free(
            world, position, world->movement.radius, world->movement.stand_height)) return;
    for (size_t i = 0u; i < ai->affordance_count; ++i) {
        if (dist_xz(ai->affordances[i].position, position) < 1.0f) return;
    }
    const size_t index = ai->affordance_count++;
    ai->affordances[index] = (NfAiAffordance){
        .id = (uint32_t)(index+1u), .position = position
    };
}

static void generate_cover_affordances(NfAiSystem *ai, const NfWorld *world) {
    ai->affordance_count = 0u;
    for (size_t i = 0u;
         i < world->collider_count && ai->affordance_count < NF_AI_MAX_AFFORDANCES;
         ++i) {
        const NfCollider *c = &world->colliders[i];
        if (c->kind == NF_COLLIDER_LADDER) continue;
        const float height = c->max.y-c->min.y;
        const float width = c->max.x-c->min.x;
        const float depth = c->max.z-c->min.z;
        if (height < 0.35f || height > 2.6f || width > 15.0f || depth > 15.0f) continue;
        const float cx = 0.5f*(c->min.x+c->max.x);
        const float cz = 0.5f*(c->min.z+c->max.z);
        const float ox = 0.5f*width+0.9f;
        const float oz = 0.5f*depth+0.9f;
        add_affordance(ai, world, (NfVec3){cx-ox,0.05f,cz});
        add_affordance(ai, world, (NfVec3){cx+ox,0.05f,cz});
        add_affordance(ai, world, (NfVec3){cx,0.05f,cz-oz});
        add_affordance(ai, world, (NfVec3){cx,0.05f,cz+oz});
    }
}

static int choose_cover(
    NfAiSystem *ai, const NfWorld *world, const NfAiAgent *agent, NfVec3 threat) {
    const NfActor *self = nf_world_find_actor_const(world, agent->actor_id);
    if (self == NULL) return -1;
    float best = -FLT_MAX;
    int best_index = -1;
    const NfVec3 threat_eye = {threat.x, threat.y+1.4f, threat.z};
    for (size_t i = 0u; i < ai->affordance_count; ++i) {
        const NfAiAffordance *affordance = &ai->affordances[i];
        if (ai->claims != NULL && nf_claim_is_blocked(
                ai->claims, NF_CLAIM_COVER, agent->actor_id,
                affordance->id, world->tick)) continue;
        const float travel = dist_xz(self->transform.position, affordance->position);
        if (travel > 18.0f) continue;
        const NfVec3 cover_eye = {
            affordance->position.x, affordance->position.y+1.0f, affordance->position.z
        };
        const bool blocks = line_blocked(world, threat_eye, cover_eye);
        const float exposure = nf_tactical_exposure(world, threat, affordance->position);
        const float score = nf_tactical_cover_utility(exposure, travel, 12.0f) +
            (blocks ? 0.12f : 0.0f);
        if (score > best) {
            best = score;
            best_index = (int)i;
        }
    }
    return best_index;
}

static uint32_t anonymous_subject(const NfSemanticAlert *event) {
    const uint32_t px = (uint32_t)lrintf((event->position.x+128.0f)*4.0f);
    const uint32_t pz = (uint32_t)lrintf((event->position.z+128.0f)*4.0f);
    const uint32_t raw = hash_u32(px ^ (pz<<11) ^ ((uint32_t)event->type<<24));
    return NF_BELIEF_ANON_MASK | (raw & 0x7fffffffu);
}

static void maybe_publish_sighting(
    NfAiSystem *ai, NfAiAgent *agent, NfEntityId target,
    NfVec3 position, uint64_t tick) {
    if (target == 0u) return;
    if (agent->last_report_subject == target && tick >= agent->last_report_tick &&
        tick-agent->last_report_tick < NF_AI_REPORT_COOLDOWN_TICKS) return;
    (void)nf_report_publish(&ai->reports, (NfReport){
        .subject_key = target,
        .kind = NF_REPORT_ACTOR_POSITION,
        .scope = NF_REPORT_SCOPE_RIVAL_CREW,
        .origin_channel = NF_INFO_CHANNEL_DIRECT_VISUAL,
        .origin = agent->actor_id,
        .reporter = agent->actor_id,
        .position = position,
        .confidence = 0.82f,
        .origin_precision_m = 0.5f,
        .precision_m = 3.0f,
        .origin_tick = tick,
        .issued_tick = tick,
        .deliver_tick = tick+NF_AI_REPORT_DELAY_TICKS,
        .expiry_tick = tick+NF_AI_REPORT_DELAY_TICKS+NF_TICK_RATE*3u
    }, NULL);
    agent->last_report_tick = tick;
    agent->last_report_subject = target;
}

static void ingest_reports(
    NfAiSystem *ai, NfAiAgent *agent, NfFaction faction, uint64_t tick) {
    for (size_t i = 0u; i < ai->reports.count; ++i) {
        const NfReport *report = &ai->reports.reports[i];
        if (report->reporter == agent->actor_id) continue;
        if (!nf_report_scope_allows(report, agent->actor_id, faction)) continue;
        (void)nf_belief_observe_report(&agent->beliefs, report, tick);
    }
}

static void project_knowledge(NfAiAgent *agent, uint64_t tick) {
    NfAiKnowledge knowledge = {0};
    const NfBeliefHypothesis *hypothesis = nf_belief_best_threat(&agent->beliefs, tick);
    if (hypothesis != NULL) {
        knowledge.target = hypothesis->identified_actor;
        knowledge.confidence = hypothesis->confidence;
        knowledge.precision_m = hypothesis->precision_m;
        knowledge.channel = hypothesis->channel;
        knowledge.hypothesis_id = hypothesis->hypothesis_id;
        const uint64_t age = tick >= hypothesis->updated_tick
            ? tick-hypothesis->updated_tick : UINT64_MAX;
        knowledge.visible_now =
            hypothesis->kind == NF_BELIEF_SUBJECT_ACTOR &&
            hypothesis->channel == NF_INFO_CHANNEL_DIRECT_VISUAL &&
            age <= NF_AI_PERCEPTION_INTERVAL+2u;
        if (knowledge.visible_now) {
            knowledge.last_seen_position = hypothesis->position;
            knowledge.last_seen_tick = hypothesis->updated_tick;
        } else {
            knowledge.last_heard_position = hypothesis->position;
            knowledge.last_heard_tick = hypothesis->updated_tick;
        }
    }
    agent->knowledge = knowledge;
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

    for (size_t i = 0u; i < NF_MAX_ENTITIES; ++i) {
        const NfActor *other = &world->actors[i];
        if (!is_adversary_target(other)) continue;
        const NfRelationship relation = nf_relation_between(
            self->faction, other->faction, ai->rival_relationship);
        if (relation == NF_RELATION_COOPERATIVE || relation == NF_RELATION_NEUTRAL) continue;
        const float dx = other->transform.position.x-self->transform.position.x;
        const float dz = other->transform.position.z-self->transform.position.z;
        const float range = sqrtf(dx*dx+dz*dz);
        if (range > NF_AI_VIEW_RANGE || range < 0.001f) continue;
        const float facing = (facing_x*dx+facing_z*dz)/range;
        if (facing < -0.18f) continue;
        const NfVec3 target_eye = {
            other->transform.position.x,
            other->transform.position.y+other->movement.eye_height*0.75f,
            other->transform.position.z
        };
        if (line_blocked(world, eye, target_eye)) continue;
        if (range < best_distance) {
            best_distance = range;
            best_id = other->id;
            best_position = other->transform.position;
        }
    }

    if (best_id != 0u) {
        (void)nf_belief_observe(&agent->beliefs, &(NfBeliefObservation){
            .kind = NF_BELIEF_SUBJECT_ACTOR,
            .subject_key = best_id,
            .identified_actor = best_id,
            .position = best_position,
            .confidence = 1.0f,
            .precision_m = 0.45f,
            .channel = NF_INFO_CHANNEL_DIRECT_VISUAL,
            .evidence_id = hash_u32(best_id^(uint32_t)world->tick),
            .tick = world->tick
        });
        maybe_publish_sighting(ai, agent, best_id, best_position, world->tick);
        if (!agent->knowledge.visible_now || agent->knowledge.target != best_id) {
            const uint32_t delay = 15u +
                hash_u32(ai->seed^agent->actor_id^(uint32_t)world->tick)%7u;
            agent->reaction_ready_tick = world->tick+delay;
        }
    }

    NfSemanticAlert heard[8];
    const size_t heard_count = nf_semantic_collect_audible(
        semantics, self->transform.position, world->tick, heard, 8u);
    for (size_t i = 0u; i < heard_count; ++i) {
        const NfSemanticAlert *event = &heard[i];
        if (event->source == agent->actor_id) continue;
        NfInformationChannel channel = NF_INFO_CHANNEL_NONE;
        float confidence = 0.0f;
        float precision = 8.0f;
        if (event->type == NF_SEMANTIC_GUNFIRE) {
            channel = NF_INFO_CHANNEL_AUDIBLE_GUNFIRE;
            confidence = 0.52f;
            precision = 8.0f;
        } else if (event->type == NF_SEMANTIC_DAMAGE_TAKEN &&
                   event->subject == agent->actor_id) {
            channel = NF_INFO_CHANNEL_DAMAGE_RECEIVED;
            confidence = 0.72f;
            precision = 11.0f;
        } else if (event->type == NF_SEMANTIC_SUSPICIOUS_SOUND) {
            channel = NF_INFO_CHANNEL_AUDIBLE_SPEECH;
            confidence = 0.36f;
            precision = 10.0f;
        } else {
            continue;
        }
        (void)nf_belief_observe(&agent->beliefs, &(NfBeliefObservation){
            .kind = NF_BELIEF_SUBJECT_UNKNOWN_THREAT,
            .subject_key = anonymous_subject(event),
            .position = event->position,
            .confidence = confidence,
            .precision_m = precision,
            .channel = channel,
            .evidence_id = hash_u32((uint32_t)event->tick^(uint32_t)i^0xA11Du),
            .tick = world->tick
        });
    }

    ingest_reports(ai, agent, self->faction, world->tick);
    nf_belief_decay(&agent->beliefs, world->tick, world->movement.sprint_speed);
    project_knowledge(agent, world->tick);
}

static float range_fit(float range) {
    return clamp01(1.0f-fabsf(range-15.0f)/18.0f);
}

static NfTacticalCandidate make_candidate(
    NfTacticalAction action, uint32_t subject, NfVec3 position) {
    return (NfTacticalCandidate){
        .feasible = true,
        .action = action,
        .subject_key = subject,
        .position = position
    };
}

static NfAgentMode mode_from_action(NfTacticalAction action, int cover) {
    switch (action) {
        case NF_TACTICAL_ATTACK: return NF_AGENT_ENGAGE;
        case NF_TACTICAL_REPOSITION: return cover >= 0 ? NF_AGENT_SEEK_COVER : NF_AGENT_ADVANCE;
        case NF_TACTICAL_GUARD: return cover >= 0 ? NF_AGENT_SEEK_COVER : NF_AGENT_IDLE;
        case NF_TACTICAL_WITHDRAW: return NF_AGENT_RETREAT;
        case NF_TACTICAL_INVESTIGATE: return NF_AGENT_INVESTIGATE;
        default: return NF_AGENT_IDLE;
    }
}

static void decide(NfAiSystem *ai, NfAiAgent *agent, NfWorld *world) {
    NfActor *self = nf_world_find_actor(world, agent->actor_id);
    if (self == NULL) return;
    for (size_t i = 0u; i < NF_AGENT_MODE_COUNT; ++i) agent->mode_scores[i] = 0.0f;
    agent->tactical_candidate_count = 0u;

    if (!self->combat.alive) {
        agent->mode = NF_AGENT_IDLE;
        agent->selected_action = NF_TACTICAL_NONE;
        agent->current_score = 1.0f;
        return;
    }

    const NfWeaponSpec *spec = nf_weapon_spec(self->combat.weapon);
    const float ammo = spec != NULL && spec->magazine_size > 0u
        ? (float)self->combat.ammo_mag[self->combat.weapon]/(float)spec->magazine_size
        : 0.0f;
    if (self->combat.ammo_mag[self->combat.weapon] == 0u &&
        self->combat.reserve_ammo[self->combat.weapon] > 0u) {
        agent->mode = NF_AGENT_RELOAD;
        agent->selected_action = NF_TACTICAL_NONE;
        agent->current_score = 1.0f;
        return;
    }

    NfRelationship target_relation = ai->rival_relationship;
    if (agent->knowledge.target != 0u) {
        const NfActor *target_actor = nf_world_find_actor_const(world, agent->knowledge.target);
        if (target_actor != NULL) {
            target_relation = nf_relation_between(
                self->faction, target_actor->faction, ai->rival_relationship);
        }
    }
    if (agent->knowledge.target != 0u &&
        !nf_relation_can_damage(target_relation, false)) {
        agent->mode = NF_AGENT_TRUCE_HOLD;
        agent->selected_action = NF_TACTICAL_GUARD;
        agent->current_score = 1.0f;
        return;
    }

    const float systemic = clamp01(self->contamination.systemic);
    const float locomotor = clamp01(self->contamination.locomotor);
    const float manipulator = clamp01(self->contamination.manipulator);
    const float viability = clamp01(
        1.0f-systemic*0.65f-locomotor*0.20f-manipulator*0.15f);
    const float confidence = clamp01(agent->knowledge.confidence);
    const float uncertainty = 1.0f-confidence;
    const NfVec3 known = agent->knowledge.visible_now
        ? agent->knowledge.last_seen_position : agent->knowledge.last_heard_position;
    const float range = confidence > 0.0f
        ? dist_xz(self->transform.position, known) : NF_AI_VIEW_RANGE;
    const int cover = confidence > 0.0f ? choose_cover(ai, world, agent, known) : -1;
    float cover_value = 0.0f;
    if (cover >= 0) {
        const NfAiAffordance *affordance = &ai->affordances[cover];
        cover_value = nf_tactical_cover_utility(
            nf_tactical_exposure(world, known, affordance->position),
            dist_xz(self->transform.position, affordance->position), 12.0f);
    }

    agent->tactical_snapshot = (NfTacticalSnapshot){
        .actor_viability = viability,
        .relation_continuity = 0.50f,
        .crew_continuity = agent->role == NF_SQUAD_HOLD ? 0.72f : 0.58f,
        .locality_value = cover_value,
        .future_options = clamp01(0.35f+cover_value*0.35f+ammo*0.30f),
        .information_quality = agent->knowledge.visible_now ? 1.0f : confidence*0.60f,
        .uncertainty = uncertainty,
        .objective_urgency = 0.45f,
        .ammo_fraction = ammo,
        .metabolic_margin = clamp01(viability*0.55f+ammo*0.45f)
    };

    NfTacticalCandidate *candidates = agent->tactical_candidates;
    size_t count = 0u;
    if (agent->knowledge.visible_now && agent->knowledge.target != 0u && ammo > 0.0f) {
        NfTacticalCandidate c = make_candidate(
            NF_TACTICAL_ATTACK, agent->knowledge.target, known);
        c.actor_delta = -0.08f*(1.0f-viability);
        c.crew_delta = 0.12f;
        c.locality_delta = 0.04f;
        c.options_delta = -0.07f;
        c.immediate_gain = range_fit(range)*confidence;
        c.action_cost = 0.10f+(1.0f-ammo)*0.18f;
        c.uncertainty_cost = uncertainty*0.30f;
        c.role_bias = agent->role == NF_SQUAD_PRESSURE ? 0.20f : 0.0f;
        c.reason_bits = NF_TACTICAL_REASON_CREW;
        candidates[count++] = c;
    }
    if (confidence > 0.0f && !agent->knowledge.visible_now &&
        count < NF_TACTICAL_CANDIDATE_CAP) {
        NfTacticalCandidate c = make_candidate(
            NF_TACTICAL_INVESTIGATE, agent->knowledge.target, known);
        c.actor_delta = -0.03f;
        c.locality_delta = 0.08f;
        c.options_delta = 0.12f;
        c.immediate_gain = 0.14f*confidence;
        c.action_cost = clamp01(range/70.0f);
        c.uncertainty_cost = 0.08f;
        c.reason_bits = NF_TACTICAL_REASON_INFORMATION;
        candidates[count++] = c;
    }
    if (confidence > 0.0f && cover >= 0 && count < NF_TACTICAL_CANDIDATE_CAP) {
        NfTacticalCandidate c = make_candidate(
            NF_TACTICAL_REPOSITION, ai->affordances[cover].id,
            ai->affordances[cover].position);
        c.actor_delta = 0.12f*(1.0f-viability)+cover_value*0.08f;
        c.locality_delta = cover_value;
        c.options_delta = 0.16f;
        c.action_cost = clamp01(
            dist_xz(self->transform.position, ai->affordances[cover].position)/24.0f);
        c.uncertainty_cost = uncertainty*0.10f;
        c.reason_bits = NF_TACTICAL_REASON_LOCALITY|NF_TACTICAL_REASON_OPTIONS;
        candidates[count++] = c;
    }
    if (confidence > 0.0f && count < NF_TACTICAL_CANDIDATE_CAP) {
        NfTacticalCandidate c = make_candidate(
            NF_TACTICAL_GUARD, 0u, self->transform.position);
        c.actor_delta = 0.05f;
        c.crew_delta = agent->role == NF_SQUAD_HOLD ? 0.28f : 0.10f;
        c.locality_delta = 0.12f+cover_value*0.10f;
        c.options_delta = 0.05f;
        c.action_cost = 0.04f;
        c.role_bias = agent->role == NF_SQUAD_HOLD ? 0.18f : 0.0f;
        c.reason_bits = NF_TACTICAL_REASON_CREW|NF_TACTICAL_REASON_LOCALITY;
        candidates[count++] = c;
    }
    if (confidence > 0.0f && count < NF_TACTICAL_CANDIDATE_CAP) {
        NfTacticalCandidate c = make_candidate(NF_TACTICAL_WITHDRAW, 0u, known);
        c.actor_delta = (1.0f-viability)*0.42f+
            clamp01((7.0f-range)/7.0f)*0.24f;
        c.crew_delta = -0.05f;
        c.options_delta = 0.18f;
        c.action_cost = 0.08f;
        c.uncertainty_cost = uncertainty*0.04f;
        c.reason_bits = NF_TACTICAL_REASON_VIABILITY|NF_TACTICAL_REASON_OPTIONS;
        candidates[count++] = c;
    }
    if (count == 0u) {
        NfTacticalCandidate c = make_candidate(
            NF_TACTICAL_NONE, 0u, self->transform.position);
        c.immediate_gain = 0.02f;
        candidates[count++] = c;
    }

    agent->tactical_candidate_count = count;
    (void)nf_tactical_rank(&agent->tactical_snapshot, candidates, count);
    agent->selected_action = candidates[0].action;
    NfAgentMode best_mode = mode_from_action(agent->selected_action, cover);
    float best_score = candidates[0].score;

    const float current_score = agent->current_score*0.90f;
    if (agent->mode != best_mode && current_score+0.10f >= best_score &&
        current_score > 0.05f) {
        best_mode = agent->mode;
        best_score = current_score;
    }

    if (best_mode != NF_AGENT_SEEK_COVER) {
        release_cover(ai, agent);
    } else if (agent->selected_affordance < 0 && cover >= 0) {
        bool acquired = true;
        if (ai->claims != NULL) {
            const NfClaimResult result = nf_claim_try_acquire(
                ai->claims, NF_CLAIM_COVER, NF_CLAIM_HARD,
                agent->actor_id, ai->affordances[cover].id,
                world->tick, NF_TICK_RATE*2u, NULL);
            acquired = result == NF_CLAIM_RESULT_GRANTED ||
                result == NF_CLAIM_RESULT_SHARED;
        }
        if (acquired) agent->selected_affordance = cover;
    }

    agent->mode = best_mode;
    agent->current_score = best_score;
    if ((size_t)best_mode < NF_AGENT_MODE_COUNT) agent->mode_scores[best_mode] = best_score;
}

static NfVec3 known_position(const NfAiAgent *agent) {
    return agent->knowledge.visible_now
        ? agent->knowledge.last_seen_position : agent->knowledge.last_heard_position;
}

static void aim_at(
    NfAiAgent *agent, const NfActor *self, NfVec3 target,
    uint64_t tick, uint32_t seed) {
    const float dx = target.x-self->transform.position.x;
    const float dz = target.z-self->transform.position.z;
    const float horizontal = sqrtf(dx*dx+dz*dz);
    const float eye_y = self->transform.position.y+self->movement.eye_height;
    const float phase = (float)((tick+hash_u32(seed^agent->actor_id))%1000u);
    agent->yaw = atan2f(dx, dz);
    agent->pitch = atan2f(
        (target.y+1.05f)-eye_y, horizontal > 0.01f ? horizontal : 0.01f);
    agent->yaw += sinf(phase*0.071f)*0.022f+cosf(phase*0.037f)*0.012f;
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
    const float range = agent->knowledge.confidence > 0.0f
        ? dist_xz(self->transform.position, target) : 0.0f;
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
    } else if (agent->mode == NF_AGENT_RETREAT && agent->knowledge.confidence > 0.0f) {
        agent->yaw = atan2f(
            self->transform.position.x-target.x,
            self->transform.position.z-target.z);
        frame.move.forward = 1.0f;
        frame.move.sprint_held = true;
        moving = true;
    } else if (agent->mode == NF_AGENT_ENGAGE && agent->knowledge.visible_now) {
        aim_at(agent, self, target, world->tick, ai->seed);
        frame.move.forward = range > 17.0f ? 0.35f : 0.0f;
        if (agent->role == NF_SQUAD_FLANK_LEFT) frame.move.strafe = -0.65f;
        else if (agent->role == NF_SQUAD_FLANK_RIGHT) frame.move.strafe = 0.65f;
        else if (agent->role == NF_SQUAD_PRESSURE) frame.move.strafe = (float)agent->strafe_sign*0.28f;
        moving = fabsf(frame.move.forward) > 0.05f || fabsf(frame.move.strafe) > 0.05f;
        const uint32_t period = agent->role == NF_SQUAD_PRESSURE ? 90u :
            (agent->role == NF_SQUAD_HOLD ? 120u : 105u);
        const uint32_t burst = agent->role == NF_SQUAD_PRESSURE ? 18u :
            (agent->role == NF_SQUAD_HOLD ? 10u : 13u);
        const uint32_t phase = (uint32_t)(
            (world->tick+hash_u32(ai->seed^agent->actor_id))%period);
        if (world->tick >= agent->reaction_ready_tick && phase < burst) {
            frame.combat.fire_held = true;
            frame.combat.fire_pressed = phase == 0u;
        }
    } else if (agent->mode == NF_AGENT_TRUCE_HOLD && agent->knowledge.visible_now) {
        aim_at(agent, self, target, world->tick, ai->seed);
    }

    if (agent->mode == NF_AGENT_RELOAD && self->combat.state != NF_WEAPON_RELOADING) {
        frame.combat.reload_pressed = true;
    }
    frame.move.yaw_radians = agent->yaw;
    frame.combat.aim_pitch_radians = agent->pitch;
    agent->movement_requested = moving;
    return frame;
}

void nf_ai_init(NfAiSystem *ai, NfWorld *world, size_t count, uint32_t seed) {
    if (ai == NULL || world == NULL) return;
    static const NfVec3 spawns[NF_AI_MAX_AGENTS] = {
        {-12.0f,0.05f,-8.0f}, {10.0f,0.05f,-8.0f},
        {-12.0f,0.05f,10.0f}, {10.0f,0.05f,10.0f}
    };
    static const NfSquadRole roles[NF_AI_MAX_AGENTS] = {
        NF_SQUAD_PRESSURE, NF_SQUAD_FLANK_LEFT,
        NF_SQUAD_FLANK_RIGHT, NF_SQUAD_HOLD
    };
    memset(ai, 0, sizeof(*ai));
    ai->seed = seed;
    ai->rival_relationship = NF_RELATION_HOSTILE;
    nf_report_bus_init(&ai->reports);
    if (count > NF_AI_MAX_AGENTS) count = NF_AI_MAX_AGENTS;
    for (size_t i = 0u; i < count; ++i) {
        const NfEntityId id = nf_world_spawn_actor(world, NF_FACTION_RIVAL, spawns[i]);
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
        nf_belief_init(&agent->beliefs);
    }
    generate_cover_affordances(ai, world);
}

void nf_ai_set_rival_relationship(
    NfAiSystem *ai, NfRelationship relationship) {
    if (ai != NULL) ai->rival_relationship = relationship;
}

void nf_ai_bind_claims(NfAiSystem *ai, NfClaimTable *claims) {
    if (ai != NULL) ai->claims = claims;
}

size_t nf_ai_tick(
    NfAiSystem *ai, NfWorld *world, const NfSemanticBus *semantics,
    NfControlFrame *out, size_t cap) {
    if (ai == NULL || world == NULL || semantics == NULL || out == NULL) return 0u;
    if (ai->claims != NULL) nf_claim_expire(ai->claims, world->tick);
    size_t written = 0u;
    for (size_t i = 0u; i < ai->count && written < cap; ++i) {
        NfAiAgent *agent = &ai->agents[i];
        NfActor *self = nf_world_find_actor(world, agent->actor_id);
        if (self == NULL) continue;
        if (world->tick >= agent->next_perception_tick) {
            perceive(ai, agent, world, semantics);
            agent->next_perception_tick =
                world->tick+NF_AI_PERCEPTION_INTERVAL+(uint64_t)i%3u;
            if ((world->tick%30u) == (uint64_t)i%30u) {
                const float moved = dist_xz(
                    self->transform.position, agent->last_sample_position);
                if (agent->movement_requested && moved < 0.12f) agent->stuck_ticks += 30u;
                else agent->stuck_ticks = 0u;
                agent->last_sample_position = self->transform.position;
            }
        }
        if (world->tick >= agent->next_decision_tick) {
            decide(ai, agent, world);
            agent->next_decision_tick =
                world->tick+NF_AI_DECISION_INTERVAL+(uint64_t)i%4u;
        }
        out[written++] = build_control(ai, agent, world);
    }
    return written;
}

const NfAiAgent *nf_ai_find_agent_const(
    const NfAiSystem *ai, NfEntityId actor_id) {
    if (ai == NULL) return NULL;
    for (size_t i = 0u; i < ai->count; ++i) {
        if (ai->agents[i].actor_id == actor_id) return &ai->agents[i];
    }
    return NULL;
}

const NfReportBus *nf_ai_report_bus_const(const NfAiSystem *ai) {
    return ai != NULL ? &ai->reports : NULL;
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
    for (size_t i = 0u; i < ai->count; ++i) {
        NfAiAgent *agent = &ai->agents[i];
        if (agent->actor_id != actor_id) continue;
        release_cover(ai, agent);
        const NfSquadRole role = agent->role;
        const NfVec3 spawn = agent->spawn;
        const uint32_t sequence = agent->control_sequence;
        const uint64_t last_report_tick = agent->last_report_tick;
        memset(agent, 0, sizeof(*agent));
        agent->actor_id = actor_id;
        agent->mode = NF_AGENT_IDLE;
        agent->role = role;
        agent->spawn = spawn;
        agent->control_sequence = sequence;
        agent->last_report_tick = last_report_tick;
        agent->selected_affordance = -1;
        agent->strafe_sign = (i%2u) ? -1 : 1;
        agent->last_sample_position = spawn;
        nf_belief_init(&agent->beliefs);
        return;
    }
}
