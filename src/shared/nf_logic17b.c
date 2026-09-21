#include "nf_logic17b.h"

#include <math.h>
#include <string.h>

static uint32_t mix32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

static float clamp01(float value) {
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static bool additive_kind(uint8_t kind) {
    return kind == (uint8_t)NF17B_TXN_FORCE ||
           kind == (uint8_t)NF17B_TXN_DAMAGE ||
           kind == (uint8_t)NF17B_TXN_SUPPORT_LOAD ||
           kind == (uint8_t)NF17B_TXN_RESOURCE_DELTA;
}

Nf17bFixed nf17b_fixed_from_float(float value) {
    return (Nf17bFixed)lroundf(value * (float)NF17B_FIXED_SCALE);
}

float nf17b_fixed_to_float(Nf17bFixed value) {
    return (float)value / (float)NF17B_FIXED_SCALE;
}

void nf17b_state_init(Nf17bAuthoritativeState *state) {
    if (state == NULL) return;
    memset(state, 0, sizeof(*state));
    state->state_version = 1u;
    state->topology_version = 1u;
    state->material_version = 1u;
    state->contract_version = 1u;
    state->occupancy_version = 1u;
    state->resource_version = 1u;
    state->support_version = 1u;
}

void nf17b_advance_tick(Nf17bAuthoritativeState *state) {
    if (state != NULL) ++state->tick;
}

void nf17b_commit_write_mask(Nf17bAuthoritativeState *state, Nf17bComponentMask write_mask) {
    if (state == NULL || write_mask == 0u) return;
    ++state->state_version;
    if ((write_mask & NF17B_COMPONENT_TOPOLOGY) != 0u) ++state->topology_version;
    if ((write_mask & NF17B_COMPONENT_MATERIAL) != 0u) ++state->material_version;
    if ((write_mask & NF17B_COMPONENT_CONTRACT) != 0u) ++state->contract_version;
    if ((write_mask & NF17B_COMPONENT_OCCUPANCY) != 0u) ++state->occupancy_version;
    if ((write_mask & NF17B_COMPONENT_RESOURCE) != 0u) ++state->resource_version;
    if ((write_mask & NF17B_COMPONENT_SUPPORT) != 0u) ++state->support_version;
}

uint32_t nf17b_state_hash(const Nf17bAuthoritativeState *state) {
    if (state == NULL) return 0u;
    const uint32_t values[] = {
        state->tick,
        state->state_version,
        state->topology_version,
        state->material_version,
        state->contract_version,
        state->occupancy_version,
        state->resource_version,
        state->support_version
    };
    uint32_t hash = 2166136261u;
    for (size_t i = 0u; i < sizeof(values) / sizeof(values[0]); ++i) {
        hash ^= mix32(values[i] + (uint32_t)i * 0x9e3779b9u);
        hash *= 16777619u;
    }
    return hash;
}

Nf17bSnapshot nf17b_snapshot(const Nf17bAuthoritativeState *state) {
    Nf17bSnapshot snapshot;
    memset(&snapshot, 0, sizeof(snapshot));
    if (state == NULL) return snapshot;
    snapshot.state = *state;
    snapshot.deterministic_hash = nf17b_state_hash(state);
    return snapshot;
}

Nf17bConflictKind nf17b_classify_conflict(const Nf17bTransaction *a, const Nf17bTransaction *b) {
    if (a == NULL || b == NULL) return NF17B_CONFLICT_EXCLUSIVE;

    const Nf17bComponentMask hazard =
        (a->write_mask & (b->read_mask | b->write_mask)) |
        (b->write_mask & (a->read_mask | a->write_mask));

    if (hazard == 0u || a->target_id != b->target_id) return NF17B_CONFLICT_COMPATIBLE;

    if (a->kind == (uint8_t)NF17B_TXN_DESTROY || b->kind == (uint8_t)NF17B_TXN_DESTROY) {
        return NF17B_CONFLICT_TRANSFORMING;
    }

    if (a->kind == b->kind && additive_kind(a->kind)) {
        return NF17B_CONFLICT_MERGEABLE;
    }

    return NF17B_CONFLICT_EXCLUSIVE;
}

bool nf17b_transaction_valid_for_snapshot(const Nf17bTransaction *txn, const Nf17bSnapshot *snapshot) {
    if (txn == NULL || snapshot == NULL) return false;
    if (txn->tick != snapshot->state.tick) return false;
    if (txn->kind >= (uint8_t)NF17B_TXN_COUNT) return false;
    if (txn->domain >= (uint8_t)NF17B_DOMAIN_COUNT) return false;
    if ((txn->precondition_mask & ~(txn->read_mask | txn->material_dependency_mask)) != 0u) return false;
    return true;
}

Nf17bCancelResult nf17b_cancel_agreement(Nf17bAgreement *agreement) {
    if (agreement == NULL) return NF17B_CANCEL_COMPENSATION_REQUIRED;

    const bool provisional =
        agreement->stage == (uint8_t)NF17B_AGREEMENT_PROPOSED ||
        agreement->stage == (uint8_t)NF17B_AGREEMENT_PROVISIONAL ||
        agreement->stage == (uint8_t)NF17B_AGREEMENT_RESERVED;

    const Nf17bCancelResult result =
        provisional && agreement->surfaced_consequence == 0u
            ? NF17B_CANCEL_ROLLED_BACK
            : NF17B_CANCEL_COMPENSATION_REQUIRED;

    agreement->stage = (uint8_t)NF17B_AGREEMENT_CANCELLED;
    return result;
}

bool nf17b_cross_nexus_surfaces(const Nf17bCrossNexusTransfer *transfer) {
    if (transfer == NULL) return false;
    if (transfer->source_nexus == transfer->target_nexus) return false;
    if (transfer->material_channel_mask == 0u || transfer->magnitude == 0) return false;
    return transfer->target_state_changed != 0u || transfer->threshold_crossed != 0u;
}

void nf17b_frontier_init(Nf17bFrontier *frontier) {
    if (frontier != NULL) memset(frontier, 0, sizeof(*frontier));
}

bool nf17b_frontier_push(Nf17bFrontier *frontier, Nf17bFrontierItem item) {
    if (frontier == NULL || frontier->count >= NF17B_MAX_FRONTIER) return false;
    frontier->items[frontier->count++] = item;
    return true;
}

static bool better_item(const Nf17bFrontierItem *a, const Nf17bFrontierItem *b, Nf17bFrontierPolicy policy) {
    if (policy == NF17B_FRONTIER_CAUSAL) {
        if (a->causal_depth != b->causal_depth) return a->causal_depth < b->causal_depth;
    } else if (policy == NF17B_FRONTIER_SEVERITY) {
        if (a->severity != b->severity) return a->severity > b->severity;
    } else if (policy == NF17B_FRONTIER_LOCALITY) {
        if (a->locality != b->locality) return a->locality > b->locality;
    } else {
        if (a->causal_depth != b->causal_depth) return a->causal_depth < b->causal_depth;
        if (a->consequential != b->consequential) return a->consequential > b->consequential;
        if (a->locality != b->locality) return a->locality > b->locality;
        if (a->age != b->age) return a->age > b->age;
    }

    if (a->age != b->age) return a->age > b->age;
    return a->id < b->id;
}

bool nf17b_frontier_pop(Nf17bFrontier *frontier, Nf17bFrontierPolicy policy, Nf17bFrontierItem *out) {
    if (frontier == NULL || out == NULL || frontier->count == 0u) return false;
    size_t best = 0u;
    for (size_t i = 1u; i < frontier->count; ++i) {
        if (better_item(&frontier->items[i], &frontier->items[best], policy)) best = i;
    }
    *out = frontier->items[best];
    frontier->items[best] = frontier->items[frontier->count - 1u];
    --frontier->count;
    return true;
}

void nf17b_frontier_age(Nf17bFrontier *frontier) {
    if (frontier == NULL) return;
    for (size_t i = 0u; i < frontier->count; ++i) {
        if (frontier->items[i].age < UINT16_MAX) ++frontier->items[i].age;
    }
}

Nf17bRefinementDecision nf17b_refinement_decide(
    Nf17bRefinementMetrics metrics,
    float refine_threshold,
    float reaggregate_threshold) {

    const float refine_value =
        0.30f * clamp01(metrics.causal_recall) +
        0.25f * clamp01(metrics.fidelity_gain) +
        0.20f * clamp01(metrics.horizon_gain) +
        0.15f * clamp01(metrics.cost_efficiency) +
        0.10f * clamp01(metrics.causal_precision);

    if (refine_value >= refine_threshold) return NF17B_REFINEMENT_REFINE;

    const float persistence =
        0.45f * clamp01(metrics.fidelity_gain) +
        0.30f * clamp01(metrics.horizon_gain) +
        0.25f * clamp01(metrics.causal_recall);

    if (persistence <= reaggregate_threshold) return NF17B_REFINEMENT_REAGGREGATE;
    return NF17B_REFINEMENT_KEEP;
}

static bool support_allowed(const Nf17bTraversalProfile *profile, uint8_t support_kind) {
    if (support_kind >= 32u) return false;
    return (profile->allowed_support_mask & (1u << support_kind)) != 0u;
}

typedef struct RouteNode {
    int cell;
    int first;
    uint8_t depth;
    float cost;
    uint16_t branches;
} RouteNode;

Nf17bRouteHypothesis nf17b_route_hypothesis(
    const NfSpatialWorld *world,
    int start,
    int goal,
    const Nf17bTraversalProfile *profile) {

    Nf17bRouteHypothesis result;
    memset(&result, 0, sizeof(result));
    result.next_cell = -1;
    result.frontier_cell = -1;
    result.cost = 1000000.0f;

    if (world == NULL || profile == NULL) return result;
    if (!nf_spatial_is_valid_cell(world, start) || !nf_spatial_is_valid_cell(world, goal)) return result;

    RouteNode queue[NF_SPATIAL_MAX_CELLS];
    uint8_t seen[NF_SPATIAL_MAX_CELLS];
    memset(seen, 0, sizeof(seen));
    size_t head = 0u, tail = 0u;
    queue[tail++] = (RouteNode){start, start, 0u, 0.0f, 0u};
    seen[start] = 1u;

    int best_distance = nf_spatial_shortest_path(world, start, goal);
    if (best_distance < 0) best_distance = NF_SPATIAL_MAX_CELLS;
    RouteNode best = queue[0];

    while (head < tail) {
        const RouteNode cur = queue[head++];
        const int cur_distance = nf_spatial_shortest_path(world, cur.cell, goal);
        if (cur.cell == goal) {
            best = cur;
            result.reached_goal = 1u;
            break;
        }
        if (cur.depth >= profile->horizon) {
            if (cur_distance >= 0 && cur_distance < best_distance) {
                best_distance = cur_distance;
                best = cur;
            }
            continue;
        }

        int local_branches = 0;
        for (int y = 0; y < world->height; ++y) {
            (void)y;
        }
        const int x = cur.cell % world->width;
        const int y = cur.cell / world->width;
        const int dx[4] = {1, -1, 0, 0};
        const int dy[4] = {0, 0, 1, -1};

        for (int k = 0; k < 4; ++k) {
            const int next = nf_spatial_cell_xy(world, x + dx[k], y + dy[k]);
            if (!nf_spatial_is_valid_cell(world, next)) continue;
            if (!support_allowed(profile, world->cells[next].support_kind)) continue;
            ++local_branches;
        }

        for (int k = 0; k < 4; ++k) {
            const int next = nf_spatial_cell_xy(world, x + dx[k], y + dy[k]);
            if (!nf_spatial_is_valid_cell(world, next)) continue;
            if (!support_allowed(profile, world->cells[next].support_kind)) continue;
            if (seen[next] != 0u) continue;
            seen[next] = 1u;

            const NfSpatialCell *cell = &world->cells[next];
            const float step_cost =
                1.0f +
                profile->exposure_weight * cell->exposure +
                profile->ecology_weight * cell->ecological_risk -
                profile->resource_reward * cell->resource -
                profile->branch_reward * (float)local_branches -
                profile->affordance_reward * cell->affordance -
                profile->contract_context_weight * cell->contract_memory;

            RouteNode child;
            child.cell = next;
            child.first = cur.depth == 0u ? next : cur.first;
            child.depth = (uint8_t)(cur.depth + 1u);
            child.cost = cur.cost + step_cost;
            child.branches = (uint16_t)(cur.branches + (uint16_t)local_branches);
            if (tail < NF_SPATIAL_MAX_CELLS) queue[tail++] = child;

            const int d = nf_spatial_shortest_path(world, next, goal);
            if (d >= 0 && (d < best_distance || (d == best_distance && child.cost < best.cost))) {
                best_distance = d;
                best = child;
            }
        }
    }

    result.next_cell = best.first == start ? -1 : best.first;
    result.frontier_cell = best.cell;
    result.depth = best.depth;
    result.branches_preserved = best.branches;
    result.cost = best.cost;
    return result;
}

void nf17b_trail_init(Nf17bTrailField *field, uint16_t owner_actor, bool shared) {
    if (field == NULL) return;
    memset(field, 0, sizeof(*field));
    field->owner_actor = owner_actor;
    field->shared = shared ? 1u : 0u;
}

void nf17b_trail_deposit(Nf17bTrailField *field, int cell, float amount) {
    if (field == NULL || cell < 0 || cell >= NF17B_MAX_TRAIL_CELLS) return;
    field->value[cell] = clamp01(field->value[cell] + amount * (1.0f - field->value[cell]));
}

void nf17b_trail_decay(Nf17bTrailField *field, float retain) {
    if (field == NULL) return;
    const float r = clamp01(retain);
    for (size_t i = 0u; i < NF17B_MAX_TRAIL_CELLS; ++i) field->value[i] *= r;
}

const char *nf17b_conflict_name(Nf17bConflictKind conflict) {
    static const char *names[] = {"compatible", "mergeable", "exclusive", "transforming"};
    return conflict >= NF17B_CONFLICT_COMPATIBLE && conflict <= NF17B_CONFLICT_TRANSFORMING
        ? names[conflict] : "unknown";
}

const char *nf17b_frontier_policy_name(Nf17bFrontierPolicy policy) {
    static const char *names[] = {"causal", "severity", "locality", "uisr-hierarchical"};
    return policy >= NF17B_FRONTIER_CAUSAL && policy <= NF17B_FRONTIER_UISR_HIERARCHICAL
        ? names[policy] : "unknown";
}
