#include "nf_spatial.h"

#include "nf_ai.h"
#include "nf_movement.h"

#include <float.h>
#include <math.h>
#include <string.h>

#define NF_SPATIAL_REGION_INTERVAL 30u
#define NF_SPATIAL_LOCAL_INTERVAL 12u
#define NF_SPATIAL_DECAY_INTERVAL 12u
#define NF_SPATIAL_TOP_REGIONS 4u

static float clamp01(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

static float distance_xz(NfVec3 a, NfVec3 b) {
    const float dx = a.x-b.x;
    const float dz = a.z-b.z;
    return sqrtf(dx*dx+dz*dz);
}

static uint32_t hash_u32(uint32_t value) {
    value ^= value >> 16;
    value *= 0x7feb352du;
    value ^= value >> 15;
    value *= 0x846ca68bu;
    value ^= value >> 16;
    return value;
}

static float personality_value(uint32_t value) {
    return 0.90f + (float)(hash_u32(value)%21u)*0.01f;
}

static bool ray_aabb(
    NfVec3 origin,
    NfVec3 direction,
    NfVec3 minimum,
    NfVec3 maximum,
    float *distance) {
    float tmin = 0.0f;
    float tmax = FLT_MAX;
    const float ov[3] = {origin.x,origin.y,origin.z};
    const float dv[3] = {direction.x,direction.y,direction.z};
    const float mn[3] = {minimum.x,minimum.y,minimum.z};
    const float mx[3] = {maximum.x,maximum.y,maximum.z};

    for (int i = 0; i < 3; ++i) {
        if (fabsf(dv[i]) < 1e-6f) {
            if (ov[i] < mn[i] || ov[i] > mx[i]) return false;
            continue;
        }
        float t1 = (mn[i]-ov[i])/dv[i];
        float t2 = (mx[i]-ov[i])/dv[i];
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
    const NfVec3 delta = {to.x-from.x,to.y-from.y,to.z-from.z};
    const float distance = sqrtf(delta.x*delta.x+delta.y*delta.y+delta.z*delta.z);
    if (distance < 0.001f) return false;
    const NfVec3 direction = {delta.x/distance,delta.y/distance,delta.z/distance};
    for (size_t i = 0u; i < world->collider_count; ++i) {
        const NfCollider *collider = &world->colliders[i];
        if (collider->kind == NF_COLLIDER_LADDER) continue;
        float hit = 0.0f;
        if (ray_aabb(from,direction,collider->min,collider->max,&hit) &&
            hit > 0.02f && hit < distance-0.08f) {
            return true;
        }
    }
    return false;
}

static NfSpatialAgentState *state_for(NfSpatialSystem *spatial, NfEntityId actor_id) {
    if (spatial == NULL) return NULL;
    for (size_t i = 0u; i < spatial->count; ++i) {
        if (spatial->agents[i].actor_id == actor_id) return &spatial->agents[i];
    }
    return NULL;
}

const NfSpatialAgentState *nf_spatial_agent_state_const(
    const NfSpatialSystem *spatial,
    NfEntityId actor_id) {
    if (spatial == NULL) return NULL;
    for (size_t i = 0u; i < spatial->count; ++i) {
        if (spatial->agents[i].actor_id == actor_id) return &spatial->agents[i];
    }
    return NULL;
}

float nf_spatial_field_preference(const NfSpatialFields *fields) {
    if (fields == NULL) return 0.0f;
    const float congestion_fit = 1.0f-clamp01(fields->route_congestion);
    const float support_fit = clamp01(fields->ally_support);
    const float threat_fit = 1.0f-clamp01(fields->enemy_threat);
    const float exposure_fit = 1.0f-clamp01(fields->exposure);
    const float pressure_fit = 1.0f-clamp01(fields->pressure);
    const float objective_fit = clamp01(fields->objective_value);

    return clamp01(
        congestion_fit*0.30f +
        support_fit*0.24f +
        threat_fit*0.18f +
        exposure_fit*0.12f +
        pressure_fit*0.10f +
        objective_fit*0.06f);
}

static int region_congestion(
    const NfSpatialSystem *spatial,
    uint8_t region,
    NfEntityId ignore) {
    int count = 0;
    for (size_t i = 0u; i < spatial->count; ++i) {
        const NfSpatialAgentState *other = &spatial->agents[i];
        if (other->actor_id == ignore) continue;
        if (other->current_region == region || other->target_region == region ||
            other->next_region == region) {
            ++count;
        }
    }
    return count;
}

static float region_role_pull(
    const NfSpatialSystem *spatial,
    const NfAiAgent *brain,
    uint8_t region) {
    if (brain == NULL || spatial == NULL || region >= spatial->graph.count) return 0.0f;
    if (brain->knowledge.target == 0u || brain->knowledge.confidence <= 0.05f) return 0.0f;

    const NfVec3 target = brain->knowledge.visible_now
        ? brain->knowledge.last_seen_position
        : brain->knowledge.last_heard_position;
    const uint8_t target_region = nf_region_nearest(&spatial->graph,target);
    if (target_region == NF_REGION_INVALID) return 0.0f;

    const NfVec3 candidate = spatial->graph.regions[region].center;
    const NfVec3 target_center = spatial->graph.regions[target_region].center;
    const float lateral = fabsf(candidate.x-target_center.x);
    const float longitudinal = fabsf(candidate.z-target_center.z);

    if (brain->role == NF_SQUAD_PRESSURE) {
        const int steps = nf_region_path_steps(&spatial->graph,region,target_region);
        return steps >= 0 ? clamp01(1.0f-(float)steps/5.0f) : 0.0f;
    }
    if (brain->role == NF_SQUAD_FLANK_LEFT || brain->role == NF_SQUAD_FLANK_RIGHT) {
        const bool correct_side = brain->role == NF_SQUAD_FLANK_LEFT
            ? candidate.x < target_center.x
            : candidate.x > target_center.x;
        return clamp01((correct_side ? 0.55f : 0.15f) +
            clamp01(lateral/180.0f)*0.30f - clamp01(longitudinal/280.0f)*0.10f);
    }
    return 0.25f;
}

static float region_pull(
    const NfSpatialSystem *spatial,
    const NfSpatialAgentState *state,
    const NfAiAgent *brain,
    const NfWorld *world,
    uint8_t region) {
    if (region >= spatial->graph.count) return -FLT_MAX;
    const int steps = nf_region_path_steps(&spatial->graph,state->current_region,region);
    if (steps < 0) return -FLT_MAX;

    const uint64_t last_visit = state->region_last_visit[region];
    const float novelty = last_visit == 0u
        ? 1.0f
        : clamp01((float)(world->tick-last_visit)/(float)(NF_TICK_RATE*24u));
    const float activity = clamp01(spatial->activity[region]);
    const float objective = clamp01(
        spatial->graph.regions[region].objective_value + spatial->objective_dynamic[region]);
    const float congestion = clamp01((float)region_congestion(spatial,region,state->actor_id)/3.0f);
    const float distribution = 1.0f-congestion;
    const float incident = clamp01(spatial->incident[region]);
    const float role = region_role_pull(spatial,brain,region);
    const float travel = clamp01((float)steps/8.0f);

    float score =
        novelty*0.26f*state->residency_bias +
        activity*0.23f +
        distribution*0.15f*state->support_bias +
        role*0.16f +
        objective*0.10f -
        congestion*0.18f -
        incident*0.12f*state->risk_bias -
        travel*0.08f;

    if (region == state->current_region && activity < 0.20f && objective < 0.40f) {
        score -= 0.14f;
    }
    return score;
}

static uint8_t choose_region(
    const NfSpatialSystem *spatial,
    const NfSpatialAgentState *state,
    const NfAiAgent *brain,
    const NfWorld *world) {
    float top_scores[NF_SPATIAL_TOP_REGIONS];
    uint8_t top_regions[NF_SPATIAL_TOP_REGIONS];
    for (size_t i = 0u; i < NF_SPATIAL_TOP_REGIONS; ++i) {
        top_scores[i] = -FLT_MAX;
        top_regions[i] = NF_REGION_INVALID;
    }

    for (uint8_t region = 0u; region < spatial->graph.count; ++region) {
        const float score = region_pull(spatial,state,brain,world,region);
        for (size_t slot = 0u; slot < NF_SPATIAL_TOP_REGIONS; ++slot) {
            if (score <= top_scores[slot]) continue;
            for (size_t shift = NF_SPATIAL_TOP_REGIONS-1u; shift > slot; --shift) {
                top_scores[shift] = top_scores[shift-1u];
                top_regions[shift] = top_regions[shift-1u];
            }
            top_scores[slot] = score;
            top_regions[slot] = region;
            break;
        }
    }

    size_t eligible = 1u;
    for (size_t i = 1u; i < NF_SPATIAL_TOP_REGIONS; ++i) {
        if (top_regions[i] == NF_REGION_INVALID) break;
        if (top_scores[0]-top_scores[i] <= 0.13f) ++eligible;
    }
    if (top_regions[0] == NF_REGION_INVALID) return state->current_region;
    const uint32_t epoch = (uint32_t)(world->tick/NF_SPATIAL_REGION_INTERVAL);
    const uint32_t roll = hash_u32(spatial->seed ^ state->actor_id ^ epoch);
    return top_regions[roll%(uint32_t)eligible];
}

static int soft_claim_count(
    const NfSpatialSystem *spatial,
    const NfSpatialAgentState *state,
    NfSpatialTask task,
    uint8_t region,
    uint64_t tick) {
    int count = 0;
    for (size_t i = 0u; i < spatial->count; ++i) {
        const NfSpatialAgentState *other = &spatial->agents[i];
        if (other->actor_id == state->actor_id || other->claim_until_tick < tick) continue;
        if (other->task == task && other->current_region == region) ++count;
    }
    return count;
}

static NfSpatialTask choose_task(
    const NfSpatialSystem *spatial,
    const NfSpatialAgentState *state,
    const NfAiAgent *brain,
    const NfActor *body,
    const NfWorld *world) {
    if (body != NULL && body->health < 32.0f) return NF_SPATIAL_TASK_REGROUP;
    if (state->current_region != state->target_region) return NF_SPATIAL_TASK_ROAM;

    NfSpatialTask task = NF_SPATIAL_TASK_OCCUPY;
    if (brain != NULL && brain->knowledge.visible_now) {
        task = brain->role == NF_SQUAD_PRESSURE
            ? NF_SPATIAL_TASK_PRESSURE
            : (brain->role == NF_SQUAD_FLANK_LEFT || brain->role == NF_SQUAD_FLANK_RIGHT
                ? NF_SPATIAL_TASK_FLANK
                : NF_SPATIAL_TASK_WATCH);
    } else if (brain != NULL && brain->knowledge.confidence > 0.20f) {
        task = NF_SPATIAL_TASK_INVESTIGATE;
    } else if (spatial->activity[state->current_region] > 0.38f) {
        task = NF_SPATIAL_TASK_INVESTIGATE;
    } else if (spatial->graph.regions[state->current_region].objective_value +
               spatial->objective_dynamic[state->current_region] > 0.58f) {
        task = NF_SPATIAL_TASK_CONTEST;
    } else {
        const uint32_t phase = hash_u32(
            spatial->seed ^ state->actor_id ^ (uint32_t)(world->tick/(NF_TICK_RATE*3u)));
        task = (phase%3u) == 0u ? NF_SPATIAL_TASK_WATCH
            : ((phase%3u) == 1u ? NF_SPATIAL_TASK_OCCUPY : NF_SPATIAL_TASK_ROAM);
    }

    if (soft_claim_count(spatial,state,task,state->current_region,world->tick) > 0 &&
        task != NF_SPATIAL_TASK_PRESSURE && task != NF_SPATIAL_TASK_INVESTIGATE) {
        task = task == NF_SPATIAL_TASK_WATCH
            ? NF_SPATIAL_TASK_OCCUPY
            : NF_SPATIAL_TASK_WATCH;
    }
    return task;
}

static NfSpatialFields fields_for_candidate(
    const NfSpatialSystem *spatial,
    const NfSpatialAgentState *state,
    const NfAiAgent *brain,
    const NfWorld *world,
    uint8_t region,
    NfVec3 candidate) {
    NfSpatialFields fields = {0};
    int congestion = 0;
    int support = 0;

    for (size_t i = 0u; i < spatial->count; ++i) {
        const NfSpatialAgentState *other_state = &spatial->agents[i];
        if (other_state->actor_id == state->actor_id) continue;
        const NfActor *other = nf_world_find_actor_const(world,other_state->actor_id);
        if (other == NULL || !other->combat.alive) continue;
        const float distance = distance_xz(other->transform.position,candidate);
        if (distance < 18.0f) ++congestion;
        if (distance < 36.0f) ++support;
        if (other_state->target_region == region) ++congestion;
    }

    fields.route_congestion = clamp01((float)congestion/4.0f);
    fields.ally_support = clamp01((float)support/2.0f);

    if (brain != NULL && brain->knowledge.target != 0u && brain->knowledge.confidence > 0.0f) {
        const NfVec3 threat = brain->knowledge.visible_now
            ? brain->knowledge.last_seen_position
            : brain->knowledge.last_heard_position;
        const float threat_distance = distance_xz(threat,candidate);
        fields.enemy_threat = clamp01(brain->knowledge.confidence*(1.0f-threat_distance/58.0f));

        const NfVec3 threat_eye = {threat.x,threat.y+1.35f,threat.z};
        const NfVec3 candidate_eye = {candidate.x,candidate.y+1.0f,candidate.z};
        const float visible_exposure = line_blocked(world,threat_eye,candidate_eye) ? 0.10f : 1.0f;
        fields.exposure = clamp01(
            spatial->graph.regions[region].structural_exposure*0.62f + visible_exposure*0.38f);
    } else {
        fields.enemy_threat = 0.0f;
        fields.exposure = spatial->graph.regions[region].structural_exposure;
    }

    fields.pressure = clamp01(
        spatial->activity[region]*0.55f + fields.route_congestion*0.25f +
        spatial->incident[region]*0.20f);
    fields.objective_value = clamp01(
        spatial->graph.regions[region].objective_value + spatial->objective_dynamic[region]);
    fields.preference_score = nf_spatial_field_preference(&fields);
    return fields;
}

static void choose_local_goal(
    NfSpatialSystem *spatial,
    NfSpatialAgentState *state,
    const NfAiAgent *brain,
    const NfWorld *world) {
    uint8_t goal_region = state->current_region;
    if (state->task == NF_SPATIAL_TASK_ROAM && state->current_region != state->target_region) {
        state->next_region = nf_region_next_hop(
            &spatial->graph,state->current_region,state->target_region);
        goal_region = state->next_region;
    } else {
        state->next_region = state->current_region;
    }
    if (goal_region == NF_REGION_INVALID || goal_region >= spatial->graph.count) {
        state->local_goal_valid = false;
        state->interrupt = NF_SPATIAL_INTERRUPT_AUTHORITATIVE;
        return;
    }

    static const NfVec3 offsets[NF_SPATIAL_LOCAL_SAMPLES] = {
        {0.0f,0.0f,0.0f},
        {8.0f,0.0f,0.0f},
        {-8.0f,0.0f,0.0f},
        {0.0f,0.0f,8.0f},
        {0.0f,0.0f,-8.0f}
    };

    float best_score = -FLT_MAX;
    NfVec3 best_goal = spatial->graph.regions[goal_region].center;
    NfSpatialFields best_fields = {0};
    bool found = false;

    for (size_t i = 0u; i < NF_SPATIAL_LOCAL_SAMPLES; ++i) {
        NfVec3 candidate = {
            spatial->graph.regions[goal_region].center.x + offsets[i].x,
            0.05f,
            spatial->graph.regions[goal_region].center.z + offsets[i].z
        };
        if (!nf_movement_space_is_free(
                world,candidate,world->movement.radius,world->movement.stand_height)) {
            continue;
        }
        const NfSpatialFields fields = fields_for_candidate(
            spatial,state,brain,world,goal_region,candidate);
        if (!found || fields.preference_score > best_score) {
            found = true;
            best_score = fields.preference_score;
            best_goal = candidate;
            best_fields = fields;
        }
    }

    state->local_goal_valid = found;
    if (found) {
        state->local_goal = best_goal;
        state->fields = best_fields;
    }
}

static void apply_interrupt(
    NfSpatialAgentState *state,
    NfSpatialInterrupt interrupt,
    uint64_t tick) {
    if (state == NULL || interrupt <= state->interrupt) return;
    state->interrupt = interrupt;
    if (interrupt >= NF_SPATIAL_INTERRUPT_TASK_BREAK) {
        state->commit_until_tick = tick;
        state->next_region_tick = tick;
        state->next_local_tick = tick;
    } else if (interrupt == NF_SPATIAL_INTERRUPT_REEVALUATE) {
        state->next_local_tick = tick;
    }
}

static void process_semantics(
    NfSpatialSystem *spatial,
    const NfWorld *world,
    const NfSemanticBus *semantics) {
    if (spatial == NULL || world == NULL || semantics == NULL) return;
    uint64_t newest = spatial->last_semantic_tick;

    for (size_t n = 0u; n < semantics->count; ++n) {
        const size_t index =
            (semantics->next + NF_SEMANTIC_CAPACITY - 1u - n) % NF_SEMANTIC_CAPACITY;
        const NfSemanticAlert *alert = &semantics->alerts[index];
        if (!nf_semantic_alert_is_live(alert,world->tick)) continue;
        if (alert->tick <= spatial->last_semantic_tick) continue;
        if (alert->tick > newest) newest = alert->tick;

        const uint8_t region = nf_region_nearest(&spatial->graph,alert->position);
        if (region == NF_REGION_INVALID) continue;

        switch (alert->type) {
            case NF_SEMANTIC_GUNFIRE:
            case NF_SEMANTIC_SUSPICIOUS_SOUND:
                spatial->activity[region] = clamp01(spatial->activity[region]+0.30f);
                break;
            case NF_SEMANTIC_DAMAGE_TAKEN:
                spatial->activity[region] = clamp01(spatial->activity[region]+0.42f);
                spatial->incident[region] = clamp01(spatial->incident[region]+0.18f);
                break;
            case NF_SEMANTIC_ACTOR_DIED:
                spatial->activity[region] = clamp01(spatial->activity[region]+0.50f);
                spatial->incident[region] = clamp01(spatial->incident[region]+0.44f);
                break;
            case NF_SEMANTIC_OBJECTIVE_CHANGED:
                spatial->objective_dynamic[region] = clamp01(
                    spatial->objective_dynamic[region]+0.50f*clamp01(alert->intensity));
                break;
            case NF_SEMANTIC_ROUTE_CHANGED:
            case NF_SEMANTIC_STORY_PHASE_CHANGED:
            default:
                break;
        }

        for (size_t i = 0u; i < spatial->count; ++i) {
            NfSpatialAgentState *state = &spatial->agents[i];
            const bool local = state->current_region == region ||
                nf_region_are_neighbors(&spatial->graph,state->current_region,region);
            if (alert->type == NF_SEMANTIC_ROUTE_CHANGED &&
                (state->target_region == region || state->next_region == region)) {
                apply_interrupt(state,NF_SPATIAL_INTERRUPT_AUTHORITATIVE,world->tick);
            } else if (alert->type == NF_SEMANTIC_STORY_PHASE_CHANGED ||
                       alert->type == NF_SEMANTIC_OBJECTIVE_CHANGED) {
                apply_interrupt(state,NF_SPATIAL_INTERRUPT_TASK_BREAK,world->tick);
            } else if (alert->type == NF_SEMANTIC_DAMAGE_TAKEN &&
                       alert->subject == state->actor_id) {
                apply_interrupt(state,NF_SPATIAL_INTERRUPT_EMERGENCY,world->tick);
            } else if (local && (alert->type == NF_SEMANTIC_GUNFIRE ||
                                 alert->type == NF_SEMANTIC_ACTOR_DIED ||
                                 alert->type == NF_SEMANTIC_SUSPICIOUS_SOUND)) {
                apply_interrupt(state,NF_SPATIAL_INTERRUPT_REEVALUATE,world->tick);
            }
        }
    }
    spatial->last_semantic_tick = newest;
}

static void decay_fields(NfSpatialSystem *spatial) {
    for (size_t i = 0u; i < spatial->graph.count; ++i) {
        spatial->activity[i] = clamp01(spatial->activity[i]-0.016f);
        spatial->incident[i] = clamp01(spatial->incident[i]-0.006f);
        spatial->objective_dynamic[i] = clamp01(spatial->objective_dynamic[i]-0.003f);
    }
}

void nf_spatial_init(
    NfSpatialSystem *spatial,
    const struct NfAiSystem *ai,
    const NfWorld *world,
    uint32_t seed) {
    if (spatial == NULL || ai == NULL || world == NULL) return;
    memset(spatial,0,sizeof(*spatial));
    spatial->seed = seed;
    nf_region_graph_init_spatial_lab(&spatial->graph);
    spatial->count = ai->count;
    if (spatial->count > NF_SPATIAL_MAX_AGENTS) spatial->count = NF_SPATIAL_MAX_AGENTS;

    for (size_t i = 0u; i < spatial->count; ++i) {
        const NfAiAgent *brain = &ai->agents[i];
        const NfActor *body = nf_world_find_actor_const(world,brain->actor_id);
        NfSpatialAgentState *state = &spatial->agents[i];
        state->actor_id = brain->actor_id;
        state->current_region = body != NULL
            ? nf_region_nearest(&spatial->graph,body->transform.position)
            : 0u;
        state->target_region = state->current_region;
        state->next_region = state->current_region;
        state->task = NF_SPATIAL_TASK_ROAM;
        state->interrupt = NF_SPATIAL_INTERRUPT_REEVALUATE;
        state->next_region_tick = (uint64_t)(i*7u);
        state->next_local_tick = (uint64_t)(i*3u);
        state->support_bias = personality_value(seed ^ brain->actor_id ^ 0x51A1u);
        state->risk_bias = personality_value(seed ^ brain->actor_id ^ 0xA713u);
        state->residency_bias = personality_value(seed ^ brain->actor_id ^ 0xC0DEu);
        state->region_last_visit[state->current_region] = world->tick;
    }
}

void nf_spatial_tick(
    NfSpatialSystem *spatial,
    const struct NfAiSystem *ai,
    const NfWorld *world,
    const NfSemanticBus *semantics) {
    if (spatial == NULL || ai == NULL || world == NULL || semantics == NULL) return;

    process_semantics(spatial,world,semantics);
    if (world->tick >= spatial->next_decay_tick) {
        decay_fields(spatial);
        spatial->next_decay_tick = world->tick + NF_SPATIAL_DECAY_INTERVAL;
    }

    for (size_t i = 0u; i < spatial->count && i < ai->count; ++i) {
        NfSpatialAgentState *state = &spatial->agents[i];
        const NfAiAgent *brain = &ai->agents[i];
        const NfActor *body = nf_world_find_actor_const(world,state->actor_id);
        if (body == NULL) continue;

        const uint8_t observed_region = nf_region_nearest(&spatial->graph,body->transform.position);
        if (observed_region != NF_REGION_INVALID && observed_region != state->current_region) {
            state->current_region = observed_region;
            state->region_last_visit[observed_region] = world->tick;
            state->interrupt = NF_SPATIAL_INTERRUPT_REEVALUATE;
            state->next_region_tick = world->tick;
            state->next_local_tick = world->tick;
        }

        if (!body->combat.alive) {
            state->local_goal_valid = false;
            continue;
        }
        if (body->health < 25.0f) {
            apply_interrupt(state,NF_SPATIAL_INTERRUPT_EMERGENCY,world->tick);
        }
        if (state->target_region >= spatial->graph.count ||
            nf_region_path_steps(&spatial->graph,state->current_region,state->target_region) < 0) {
            apply_interrupt(state,NF_SPATIAL_INTERRUPT_AUTHORITATIVE,world->tick);
        }

        if (world->tick >= state->next_region_tick && world->tick >= state->commit_until_tick) {
            state->target_region = choose_region(spatial,state,brain,world);
            state->task = choose_task(spatial,state,brain,body,world);
            const float residency = clamp01((state->residency_bias-0.90f)/0.20f);
            state->commit_until_tick = world->tick +
                (uint64_t)(NF_TICK_RATE + (uint64_t)(residency*(float)NF_TICK_RATE));
            state->claim_until_tick = state->commit_until_tick + NF_TICK_RATE/2u;
            state->next_region_tick = world->tick + NF_SPATIAL_REGION_INTERVAL + (uint64_t)(i*3u);
            state->interrupt = NF_SPATIAL_INTERRUPT_NONE;
            state->next_local_tick = world->tick;
        } else if (world->tick >= state->next_region_tick &&
                   state->task != NF_SPATIAL_TASK_ROAM &&
                   spatial->activity[state->current_region] < 0.08f &&
                   world->tick > state->commit_until_tick + NF_TICK_RATE*2u) {
            state->commit_until_tick = world->tick;
        }

        if (state->current_region != state->target_region) {
            state->task = NF_SPATIAL_TASK_ROAM;
        }

        if (world->tick >= state->next_local_tick || !state->local_goal_valid) {
            choose_local_goal(spatial,state,brain,world);
            state->next_local_tick = world->tick + NF_SPATIAL_LOCAL_INTERVAL + (uint64_t)(i%3u);
        }
    }
}

void nf_spatial_on_respawn(
    NfSpatialSystem *spatial,
    NfEntityId actor_id,
    const NfWorld *world) {
    if (spatial == NULL || world == NULL) return;
    NfSpatialAgentState *state = state_for(spatial,actor_id);
    const NfActor *body = nf_world_find_actor_const(world,actor_id);
    if (state == NULL || body == NULL) return;

    const float support_bias = state->support_bias;
    const float risk_bias = state->risk_bias;
    const float residency_bias = state->residency_bias;
    memset(state,0,sizeof(*state));
    state->actor_id = actor_id;
    state->current_region = nf_region_nearest(&spatial->graph,body->transform.position);
    state->target_region = state->current_region;
    state->next_region = state->current_region;
    state->task = NF_SPATIAL_TASK_ROAM;
    state->interrupt = NF_SPATIAL_INTERRUPT_REEVALUATE;
    state->support_bias = support_bias;
    state->risk_bias = risk_bias;
    state->residency_bias = residency_bias;
    state->region_last_visit[state->current_region] = world->tick;
}

const char *nf_spatial_task_name(NfSpatialTask task) {
    switch (task) {
        case NF_SPATIAL_TASK_ROAM: return "ROAM";
        case NF_SPATIAL_TASK_WATCH: return "WATCH";
        case NF_SPATIAL_TASK_INVESTIGATE: return "INVESTIGATE";
        case NF_SPATIAL_TASK_OCCUPY: return "OCCUPY";
        case NF_SPATIAL_TASK_PRESSURE: return "PRESSURE";
        case NF_SPATIAL_TASK_FLANK: return "FLANK";
        case NF_SPATIAL_TASK_REGROUP: return "REGROUP";
        case NF_SPATIAL_TASK_CONTEST: return "CONTEST";
        default: return "UNKNOWN";
    }
}

const char *nf_spatial_interrupt_name(NfSpatialInterrupt interrupt) {
    switch (interrupt) {
        case NF_SPATIAL_INTERRUPT_NONE: return "NONE";
        case NF_SPATIAL_INTERRUPT_REEVALUATE: return "REEVALUATE";
        case NF_SPATIAL_INTERRUPT_TASK_BREAK: return "TASK_BREAK";
        case NF_SPATIAL_INTERRUPT_EMERGENCY: return "EMERGENCY";
        case NF_SPATIAL_INTERRUPT_AUTHORITATIVE: return "AUTHORITATIVE";
        default: return "UNKNOWN";
    }
}
