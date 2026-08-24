#include "nf_spatial_review.h"

#include <math.h>
#include <string.h>

static uint32_t review_mix32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

static uint32_t review_rng_next(uint32_t *state) {
    *state = review_mix32(*state + 0x9e3779b9u);
    return *state;
}

static float review_rng01(uint32_t *state) {
    return (float)(review_rng_next(state) & 0x00ffffffu) / 16777215.0f;
}

static int review_x(const NfSpatialWorld *world, int cell) {
    return cell % world->width;
}

static int review_y(const NfSpatialWorld *world, int cell) {
    return cell / world->width;
}

static int review_manhattan(const NfSpatialWorld *world, int a, int b) {
    int dx = review_x(world, a) - review_x(world, b);
    int dy = review_y(world, a) - review_y(world, b);
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    return dx + dy;
}

static int review_neighbor(const NfSpatialWorld *world, int cell, int index) {
    static const int dx[4] = {1, -1, 0, 0};
    static const int dy[4] = {0, 0, 1, -1};
    return nf_spatial_cell_xy(
        world,
        review_x(world, cell) + dx[index],
        review_y(world, cell) + dy[index]);
}

static float quantize_cue(float value) {
    if (value < 0.20f) return 0.10f;
    if (value < 0.45f) return 0.32f;
    if (value < 0.70f) return 0.58f;
    return 0.86f;
}

static void update_perceptual_cues(
    const NfSpatialWorld *world,
    int origin,
    uint8_t *visible,
    uint8_t *known,
    float *cue_exposure,
    float *cue_ecology,
    float *cue_resource,
    float *cue_affordance,
    float *cue_contract) {
    const int count = world->width * world->height;
    memset(visible, 0, (size_t)count);
    for (int cell = 0; cell < count; ++cell) {
        if (!nf_spatial_is_valid_cell(world, cell)) continue;
        if (review_manhattan(world, origin, cell) > 3) continue;
        if (!nf_spatial_line_of_sight(world, origin, cell)) continue;
        visible[cell] = 1u;
        known[cell] = 1u;
        cue_exposure[cell] = quantize_cue(world->cells[cell].exposure);
        cue_ecology[cell] = quantize_cue(world->cells[cell].ecological_risk);
        cue_resource[cell] = quantize_cue(world->cells[cell].resource);
        cue_affordance[cell] = world->cells[cell].authored_forbid_affordance != 0u
            ? 0.0f
            : quantize_cue(world->cells[cell].affordance);
        cue_contract[cell] = world->cells[cell].contract_memory >= 0.60f
            ? 0.85f
            : (world->cells[cell].contract_memory >= 0.25f ? 0.40f : 0.0f);
    }
}

static bool known_path_exists(
    const NfSpatialWorld *world,
    int start,
    int goal,
    const uint8_t *known) {
    int queue[NF_SPATIAL_MAX_CELLS];
    uint8_t seen[NF_SPATIAL_MAX_CELLS];
    int head = 0;
    int tail = 0;
    const int count = world->width * world->height;
    memset(seen, 0, sizeof(seen));
    if (start < 0 || goal < 0 || start >= count || goal >= count) return false;
    if (known[start] == 0u || known[goal] == 0u) return false;
    if (!nf_spatial_is_valid_cell(world, start) || !nf_spatial_is_valid_cell(world, goal)) return false;
    queue[tail++] = start;
    seen[start] = 1u;
    while (head < tail) {
        int cell = queue[head++];
        if (cell == goal) return true;
        for (int k = 0; k < 4; ++k) {
            int next = review_neighbor(world, cell, k);
            if (next < 0 || next >= count) continue;
            if (known[next] == 0u || seen[next] != 0u) continue;
            if (!nf_spatial_is_valid_cell(world, next)) continue;
            seen[next] = 1u;
            queue[tail++] = next;
        }
    }
    return false;
}

static float review_score(
    const NfSpatialWorld *world,
    int current,
    int candidate,
    int goal,
    NfSpatialReviewMode mode,
    const uint8_t *known,
    const uint8_t *visits,
    const float *cue_exposure,
    const float *cue_ecology,
    const float *cue_resource,
    const float *cue_affordance,
    const float *cue_contract,
    uint32_t *rng) {
    if (mode == NF_SPATIAL_REVIEW_RANDOM) return review_rng01(rng);

    float score = -1.55f * (float)review_manhattan(world, candidate, goal);
    if (mode == NF_SPATIAL_REVIEW_GREEDY) {
        return score + 0.02f * review_rng01(rng);
    }

    if (known[candidate] == 0u) {
        return score - 0.35f + 0.02f * review_rng01(rng);
    }

    if (mode == NF_SPATIAL_REVIEW_INSTRUMENTED) {
        const NfSpatialCell *cell = &world->cells[candidate];
        score -= 4.20f * cell->exposure;
        score -= 3.60f * cell->ecological_risk;
        score += 0.55f * cell->resource;
        score += 0.12f * (float)nf_spatial_neighbor_count(world, candidate);
        score += 0.45f * nf_spatial_affordance_candidate(world, current, candidate);
        score += 0.10f * cell->contract_memory;
    } else {
        score -= 4.20f * cue_exposure[candidate];
        score -= 3.60f * cue_ecology[candidate];
        score += 0.55f * cue_resource[candidate];
        score += 0.10f * (float)nf_spatial_neighbor_count(world, candidate);
        score += 0.38f * cue_affordance[candidate];
        score += 0.08f * cue_contract[candidate];
    }

    if (visits[candidate] != 0u) {
        score -= 1.20f * (float)visits[candidate];
    }
    return score + 0.02f * review_rng01(rng);
}

static int choose_candidate(
    const NfSpatialWorld *world,
    int current,
    int goal,
    NfSpatialReviewMode mode,
    const uint8_t *known,
    const uint8_t *visits,
    const float *cue_exposure,
    const float *cue_ecology,
    const float *cue_resource,
    const float *cue_affordance,
    const float *cue_contract,
    uint32_t *rng) {
    int candidates[4];
    int candidate_count = 0;
    for (int k = 0; k < 4; ++k) {
        int next = review_neighbor(world, current, k);
        if (nf_spatial_is_valid_cell(world, next)) candidates[candidate_count++] = next;
    }
    if (candidate_count == 0) return -1;

    int chosen = candidates[0];
    float best = -1000000.0f;
    for (int i = 0; i < candidate_count; ++i) {
        float value = review_score(
            world,
            current,
            candidates[i],
            goal,
            mode,
            known,
            visits,
            cue_exposure,
            cue_ecology,
            cue_resource,
            cue_affordance,
            cue_contract,
            rng);
        if (value > best) {
            best = value;
            chosen = candidates[i];
        }
    }
    return chosen;
}

static uint32_t review_hash(const NfSpatialReviewResult *result) {
    uint32_t h = 2166136261u;
    const uint32_t values[] = {
        result->seed,
        result->lab,
        result->mode,
        result->success,
        result->steps,
        result->unique_cells,
        result->revisits,
        result->retreats,
        result->support_transitions,
        result->affordance_accepts,
        result->topology_updates,
        result->route_discovery_step,
        result->cue_disagreement_steps,
        result->path_invalid_steps,
        (uint32_t)lroundf(result->health_remaining * 1000.0f),
        (uint32_t)lroundf(result->exposure_accum * 1000.0f),
        (uint32_t)lroundf(result->ecology_cost_accum * 1000.0f),
        (uint32_t)lroundf(result->resource_collected * 1000.0f),
        (uint32_t)lroundf(result->contract_context_seen * 1000.0f)
    };
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        h ^= values[i];
        h *= 16777619u;
    }
    return h;
}

NfSpatialReviewResult nf_spatial_review_run(
    NfSpatialLabKind lab,
    NfSpatialReviewMode mode,
    uint32_t seed,
    uint8_t replicate,
    NfSpatialReviewTrace *trace) {
    NfSpatialWorld world;
    NfSpatialReviewResult result;
    uint8_t known[NF_SPATIAL_MAX_CELLS];
    uint8_t visible[NF_SPATIAL_MAX_CELLS];
    uint8_t visits[NF_SPATIAL_MAX_CELLS];
    uint8_t resources_taken[NF_SPATIAL_MAX_CELLS];
    float cue_exposure[NF_SPATIAL_MAX_CELLS];
    float cue_ecology[NF_SPATIAL_MAX_CELLS];
    float cue_resource[NF_SPATIAL_MAX_CELLS];
    float cue_affordance[NF_SPATIAL_MAX_CELLS];
    float cue_contract[NF_SPATIAL_MAX_CELLS];
    uint32_t rng = review_mix32(seed ^ ((uint32_t)lab << 16) ^ ((uint32_t)mode << 24));
    float health = 100.0f;
    int current;
    uint16_t updates = 0u;

    memset(&result, 0, sizeof(result));
    memset(known, 0, sizeof(known));
    memset(visible, 0, sizeof(visible));
    memset(visits, 0, sizeof(visits));
    memset(resources_taken, 0, sizeof(resources_taken));
    memset(cue_exposure, 0, sizeof(cue_exposure));
    memset(cue_ecology, 0, sizeof(cue_ecology));
    memset(cue_resource, 0, sizeof(cue_resource));
    memset(cue_affordance, 0, sizeof(cue_affordance));
    memset(cue_contract, 0, sizeof(cue_contract));

    nf_spatial_world_init(&world, lab, seed);
    current = world.start_cell;
    result.seed = seed;
    result.lab = (uint8_t)lab;
    result.mode = (uint8_t)mode;
    result.replicate = replicate;
    result.route_discovery_step = UINT16_MAX;

    if (trace != NULL) {
        memset(trace, 0, sizeof(*trace));
        trace->world_initial = world;
        trace->path[0] = current;
        trace->path_len = 1u;
    }

    if (!nf_spatial_is_valid_cell(&world, current) ||
        nf_spatial_shortest_path(&world, current, world.goal_cell) < 0) {
        result.deterministic_hash = review_hash(&result);
        if (trace != NULL) {
            trace->world_final = world;
            trace->result = result;
        }
        return result;
    }

    visits[current] = 1u;
    result.unique_cells = 1u;

    for (uint16_t step = 0u; step < NF_SPATIAL_MAX_STEPS && health > 0.0f; ++step) {
        update_perceptual_cues(
            &world,
            current,
            visible,
            known,
            cue_exposure,
            cue_ecology,
            cue_resource,
            cue_affordance,
            cue_contract);

        if (result.route_discovery_step == UINT16_MAX &&
            known_path_exists(&world, current, world.goal_cell, known)) {
            result.route_discovery_step = step;
        }

        if (current == world.goal_cell) {
            result.success = 1u;
            break;
        }

        if (lab == NF_SPATIAL_LAB_DYNAMIC && step > 0u && step % 12u == 0u) {
            int gate = nf_spatial_cell_xy(&world, 6, 5);
            world.cells[gate].solid = world.cells[gate].solid != 0u ? 0u : 1u;
            nf_spatial_mark_dirty(&world, gate, 2);
            updates = (uint16_t)(updates + nf_spatial_recompute_dirty(&world, 12u));
        }

        uint32_t reference_rng_a = review_mix32(seed ^ (uint32_t)step ^ 0x13579bdu);
        uint32_t reference_rng_b = reference_rng_a;
        int instrumented_choice = choose_candidate(
            &world, current, world.goal_cell, NF_SPATIAL_REVIEW_INSTRUMENTED,
            known, visits, cue_exposure, cue_ecology, cue_resource,
            cue_affordance, cue_contract, &reference_rng_a);
        int perceptual_choice = choose_candidate(
            &world, current, world.goal_cell, NF_SPATIAL_REVIEW_PERCEPTUAL,
            known, visits, cue_exposure, cue_ecology, cue_resource,
            cue_affordance, cue_contract, &reference_rng_b);
        if (instrumented_choice >= 0 && perceptual_choice >= 0 &&
            instrumented_choice != perceptual_choice) {
            ++result.cue_disagreement_steps;
        }

        int chosen = choose_candidate(
            &world,
            current,
            world.goal_cell,
            mode,
            known,
            visits,
            cue_exposure,
            cue_ecology,
            cue_resource,
            cue_affordance,
            cue_contract,
            &rng);
        if (chosen < 0) break;
        if (!nf_spatial_is_valid_cell(&world, chosen)) {
            ++result.path_invalid_steps;
            break;
        }

        if (review_manhattan(&world, chosen, world.goal_cell) >
            review_manhattan(&world, current, world.goal_cell)) {
            ++result.retreats;
        }
        if (visits[chosen] != 0u) {
            ++result.revisits;
            if (visits[chosen] < 255u) ++visits[chosen];
        } else {
            visits[chosen] = 1u;
            ++result.unique_cells;
        }
        if (world.cells[current].support_kind != world.cells[chosen].support_kind) {
            ++result.support_transitions;
        }
        if (nf_spatial_affordance_candidate(&world, current, chosen) > 0.30f) {
            ++result.affordance_accepts;
        }

        current = chosen;
        if (trace != NULL && trace->path_len < NF_SPATIAL_REVIEW_MAX_PATH) {
            trace->path[trace->path_len++] = current;
        }

        const NfSpatialCell *cell = &world.cells[current];
        result.exposure_accum += cell->exposure;
        result.ecology_cost_accum += cell->ecological_risk;
        result.contract_context_seen += cell->contract_memory;
        health -= 1.75f * cell->exposure + 1.45f * cell->ecological_risk;
        if (resources_taken[current] == 0u && cell->resource > 0.25f) {
            float gain = 3.0f * cell->resource;
            health += gain;
            if (health > 100.0f) health = 100.0f;
            result.resource_collected += cell->resource;
            resources_taken[current] = 1u;
        }
        result.steps = (uint16_t)(step + 1u);
    }

    if (current == world.goal_cell) result.success = 1u;
    result.health_remaining = health > 0.0f ? health : 0.0f;
    result.topology_updates = updates;
    if (result.route_discovery_step == UINT16_MAX) result.route_discovery_step = result.steps;
    result.deterministic_hash = review_hash(&result);

    if (trace != NULL) {
        trace->world_final = world;
        memcpy(trace->known_final, known, sizeof(trace->known_final));
        memcpy(trace->visible_final, visible, sizeof(trace->visible_final));
        trace->result = result;
    }
    return result;
}

const char *nf_spatial_review_mode_name(NfSpatialReviewMode mode) {
    static const char *names[NF_SPATIAL_REVIEW_MODE_COUNT] = {
        "instrumented", "perceptual", "greedy", "random"
    };
    return mode >= 0 && mode < NF_SPATIAL_REVIEW_MODE_COUNT ? names[mode] : "unknown";
}
