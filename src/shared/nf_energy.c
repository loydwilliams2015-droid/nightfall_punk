#include "nf_energy.h"

#include "nf_region.h"
#include "nf_world.h"

#include <float.h>
#include <math.h>
#include <string.h>

#define NF_ENERGY_SEARCH_INTERVAL_TICKS NF_TICK_RATE

static double clamp01d(double value) {
    return value < 0.0 ? 0.0 : (value > 1.0 ? 1.0 : value);
}

static double clampd(double value, double lo, double hi) {
    return value < lo ? lo : (value > hi ? hi : value);
}

static double min_d(double a, double b) {
    return a < b ? a : b;
}

static double horizontal_distance(const NfRegion *a, const NfRegion *b) {
    const double dx = (double)a->center.x - (double)b->center.x;
    const double dz = (double)a->center.z - (double)b->center.z;
    return sqrt(dx*dx + dz*dz);
}

static NfEnergyAccount *account_mutable(NfEnergySystem *system, uint32_t entity_id) {
    if (system == NULL || entity_id == 0u) return NULL;
    for (size_t i = 0u; i < NF_ENERGY_MAX_ACCOUNTS; ++i) {
        if (system->accounts[i].active && system->accounts[i].entity_id == entity_id) {
            return &system->accounts[i];
        }
    }
    for (size_t i = 0u; i < NF_ENERGY_MAX_ACCOUNTS; ++i) {
        if (!system->accounts[i].active) {
            system->accounts[i] = (NfEnergyAccount){
                .active = true,
                .entity_id = entity_id,
                .reserves = 0.0,
                .income = 0.0,
                .expenditure = 0.0,
                .productive_capacity = 0.0
            };
            return &system->accounts[i];
        }
    }
    return NULL;
}

static void update_posterior(NfEnergyRegion *region) {
    if (region == NULL || region->baseline_potential <= 0.0) return;
    const double stock_signal = clamp01d(
        (region->potential + region->usable) / (region->baseline_potential * 1.5));
    const double flow_signal = clamp01d(
        region->productive_capacity * 0.45 +
        region->regenerative_capacity * 1.65 +
        region->health * 0.30 -
        region->externality_load * 0.35);
    const double evidence = clamp01d(stock_signal * 0.52 + flow_signal * 0.48);
    const double volatility = clamp01d(
        fabs(region->last_extraction - region->last_regeneration) /
        (1.0 + region->last_extraction + region->last_regeneration));
    region->uncertainty = clamp01d(0.12 + volatility * 0.58 + region->externality_load * 0.25);

    const double prior_weight = 2.5;
    const double evidence_weight = 4.0 + (1.0 - region->uncertainty) * 6.0;
    region->posterior_yield =
        (region->prior_yield * prior_weight + evidence * evidence_weight) /
        (prior_weight + evidence_weight);
}

static NfEnergyOpportunity truth_opportunity(
    const NfEnergySystem *system, const NfRegionGraph *graph,
    uint8_t origin_region, uint8_t region) {
    NfEnergyOpportunity out = {0};
    out.region = region;
    out.immediate_return = nf_energy_horizon_value(
        system, region, NF_ENERGY_HORIZON_IMMEDIATE);
    out.round_return = nf_energy_horizon_value(
        system, region, NF_ENERGY_HORIZON_ROUND);
    out.regional_return = nf_energy_horizon_value(
        system, region, NF_ENERGY_HORIZON_REGIONAL);
    out.regenerative_return = nf_energy_horizon_value(
        system, region, NF_ENERGY_HORIZON_REGENERATIVE);
    out.topographic_cost = (graph != NULL && origin_region != NF_ENERGY_REGION_INVALID)
        ? nf_energy_topographic_cost(system, graph, origin_region, region)
        : 0.0;
    out.posterior_mean = system->regions[region].posterior_yield;
    out.uncertainty = system->regions[region].uncertainty;
    out.score =
        out.regional_return * 0.48 +
        out.regenerative_return * 0.32 +
        out.round_return * 0.20 -
        out.topographic_cost;
    return out;
}

static void insert_opportunity(
    NfEnergyOpportunity *out, size_t cap, size_t *count,
    NfEnergyOpportunity candidate) {
    if (out == NULL || cap == 0u || count == NULL) return;
    size_t used = *count < cap ? *count : cap;
    size_t slot = used;
    for (size_t i = 0u; i < used; ++i) {
        if (candidate.score > out[i].score) {
            slot = i;
            break;
        }
    }
    if (slot >= cap) return;
    if (used < cap) ++used;
    for (size_t i = used - 1u; i > slot; --i) out[i] = out[i - 1u];
    out[slot] = candidate;
    *count = used;
}

static void recompute_global_helicopter(NfEnergySystem *system) {
    if (system == NULL || !system->bound) return;
    NfEnergyOpportunity top[NF_ENERGY_TOP_CANDIDATES] = {0};
    size_t count = 0u;
    for (uint8_t region = 0u; region < NF_ENERGY_REGION_MAX; ++region) {
        insert_opportunity(
            top, NF_ENERGY_TOP_CANDIDATES, &count,
            truth_opportunity(system, NULL, NF_ENERGY_REGION_INVALID, region));
    }
    memset(system->helicopter, 0, sizeof(system->helicopter));
    for (size_t i = 0u; i < count; ++i) system->helicopter[i] = top[i];
    system->helicopter_count = count;
}

void nf_energy_init(NfEnergySystem *system, uint32_t seed) {
    if (system == NULL) return;
    memset(system, 0, sizeof(*system));
    system->seed = seed;
}

void nf_energy_bind_spatial_lab(
    NfEnergySystem *system, const NfRegionGraph *graph) {
    if (system == NULL || graph == NULL || graph->count < NF_ENERGY_REGION_MAX) return;

    static const double elevation[NF_ENERGY_REGION_MAX] = {
        0.18,0.12,0.42,0.78,
        0.22,0.28,0.16,0.30,
        0.20,0.10,0.34,0.22,
        0.62,0.14,0.18,0.66,
        0.36,0.12,0.24,0.40,
        0.20,0.16,0.34,0.82
    };
    static const double terrain[NF_ENERGY_REGION_MAX] = {
        0.52,0.38,0.78,0.64,
        0.48,0.58,0.26,0.44,
        0.42,0.24,0.56,0.40,
        0.61,0.22,0.24,0.62,
        0.54,0.25,0.46,0.56,
        0.49,0.29,0.47,0.68
    };

    memset(system->regions, 0, sizeof(system->regions));
    memset(system->accounts, 0, sizeof(system->accounts));
    system->total_external_input = 0.0;
    system->total_conversion_loss = 0.0;
    system->total_transferred = 0.0;
    system->total_expenditure = 0.0;

    for (uint8_t i = 0u; i < NF_ENERGY_REGION_MAX; ++i) {
        const NfRegion *r = &graph->regions[i];
        NfEnergyRegion *e = &system->regions[i];
        const double objective = clamp01d((double)r->objective_value);
        const double exposure = clamp01d((double)r->structural_exposure);
        e->baseline_potential = 42.0 + objective * 78.0 + (1.0 - exposure) * 18.0;
        e->potential = e->baseline_potential * 0.62;
        e->usable = e->baseline_potential * 0.30;
        e->productive_capacity = clamp01d(0.22 + objective * 0.58);
        e->regenerative_capacity = clamp01d(0.07 + (1.0 - exposure) * 0.15 + objective * 0.06);
        e->absorptive_capacity = clamp01d(0.30 + (1.0 - exposure) * 0.52);
        e->health = 1.0;
        e->externality_load = 0.0;
        e->terrain_resistance = terrain[i];
        e->elevation_index = elevation[i];
        e->structural_exposure = exposure;
        e->prior_yield = clamp01d(
            0.26 * objective +
            0.24 * e->productive_capacity +
            0.22 * e->regenerative_capacity * 3.2 +
            0.16 * (1.0 - e->terrain_resistance) +
            0.12 * (1.0 - exposure));
        e->posterior_yield = e->prior_yield;
        e->uncertainty = 0.20;
        e->center_x = (double)r->center.x;
        e->center_z = (double)r->center.z;
    }

    /* C22 proof pair: South Works starts rich but extractive/topographically
       expensive; Signal Court starts smaller but regenerative and productive. */
    NfEnergyRegion *south = &system->regions[2u];
    south->baseline_potential = 150.0;
    south->potential = 96.0;
    south->usable = 48.0;
    south->productive_capacity = 0.30;
    south->regenerative_capacity = 0.055;
    south->absorptive_capacity = 0.38;
    south->terrain_resistance = 0.82;
    south->elevation_index = 0.46;
    south->prior_yield = 0.72;

    NfEnergyRegion *signal = &system->regions[14u];
    signal->baseline_potential = 88.0;
    signal->potential = 48.0;
    signal->usable = 22.0;
    signal->productive_capacity = 0.82;
    signal->regenerative_capacity = 0.24;
    signal->absorptive_capacity = 0.78;
    signal->terrain_resistance = 0.22;
    signal->elevation_index = 0.18;
    signal->prior_yield = 0.76;

    for (uint8_t i = 0u; i < NF_ENERGY_REGION_MAX; ++i) update_posterior(&system->regions[i]);
    system->bound = true;
    recompute_global_helicopter(system);
}

const NfEnergyRegion *nf_energy_region_const(
    const NfEnergySystem *system, uint8_t region) {
    if (system == NULL || !system->bound || region >= NF_ENERGY_REGION_MAX) return NULL;
    return &system->regions[region];
}

const NfEnergyAccount *nf_energy_account_const(
    const NfEnergySystem *system, uint32_t entity_id) {
    if (system == NULL || entity_id == 0u) return NULL;
    for (size_t i = 0u; i < NF_ENERGY_MAX_ACCOUNTS; ++i) {
        if (system->accounts[i].active && system->accounts[i].entity_id == entity_id) {
            return &system->accounts[i];
        }
    }
    return NULL;
}

uint8_t nf_energy_nearest_region(
    const NfEnergySystem *system, double x, double z) {
    if (system == NULL || !system->bound) return NF_ENERGY_REGION_INVALID;
    uint8_t best = NF_ENERGY_REGION_INVALID;
    double best_distance = DBL_MAX;
    for (uint8_t i = 0u; i < NF_ENERGY_REGION_MAX; ++i) {
        const double dx = x - system->regions[i].center_x;
        const double dz = z - system->regions[i].center_z;
        const double distance = dx*dx + dz*dz;
        if (distance < best_distance) {
            best_distance = distance;
            best = i;
        }
    }
    return best;
}

double nf_energy_total_stored(const NfEnergySystem *system) {
    if (system == NULL || !system->bound) return 0.0;
    double total = 0.0;
    for (uint8_t i = 0u; i < NF_ENERGY_REGION_MAX; ++i) {
        total += system->regions[i].potential + system->regions[i].usable;
    }
    return total;
}

double nf_energy_transfer(
    NfEnergySystem *system, uint8_t source_region, uint8_t sink_region,
    double amount) {
    if (system == NULL || !system->bound || source_region >= NF_ENERGY_REGION_MAX ||
        sink_region >= NF_ENERGY_REGION_MAX || amount <= 0.0 || source_region == sink_region) {
        return 0.0;
    }
    NfEnergyRegion *source = &system->regions[source_region];
    NfEnergyRegion *sink = &system->regions[sink_region];
    const double moved = min_d(source->usable, amount);
    source->usable -= moved;
    sink->usable += moved;
    system->total_transferred += moved;
    return moved;
}

double nf_energy_convert(
    NfEnergySystem *system, uint8_t region, double amount, double efficiency) {
    if (system == NULL || !system->bound || region >= NF_ENERGY_REGION_MAX || amount <= 0.0) {
        return 0.0;
    }
    NfEnergyRegion *state = &system->regions[region];
    const double input = min_d(state->potential, amount);
    const double useful = input * clamp01d(efficiency);
    state->potential -= input;
    state->usable += useful;
    system->total_conversion_loss += input - useful;
    return useful;
}

double nf_energy_change_capacity(
    NfEnergySystem *system, uint8_t region, double delta) {
    if (system == NULL || !system->bound || region >= NF_ENERGY_REGION_MAX) return 0.0;
    NfEnergyRegion *state = &system->regions[region];
    const double before = state->productive_capacity;
    state->productive_capacity = clamp01d(state->productive_capacity + delta);
    return state->productive_capacity - before;
}

double nf_energy_topographic_cost(
    const NfEnergySystem *system, const NfRegionGraph *graph,
    uint8_t from_region, uint8_t to_region) {
    if (system == NULL || graph == NULL || !system->bound ||
        from_region >= graph->count || to_region >= graph->count ||
        from_region >= NF_ENERGY_REGION_MAX || to_region >= NF_ENERGY_REGION_MAX) {
        return 0.0;
    }
    if (from_region == to_region) return 0.0;
    const NfRegion *from = &graph->regions[from_region];
    const NfRegion *to = &graph->regions[to_region];
    const NfEnergyRegion *target = &system->regions[to_region];
    const int steps = nf_region_path_steps(graph, from_region, to_region);
    if (steps < 0) return DBL_MAX / 8.0;

    const double distance_cost = horizontal_distance(from, to) * 0.012;
    const double step_cost = (double)steps * 0.85;
    const double elevation_cost = fabs(
        system->regions[to_region].elevation_index -
        system->regions[from_region].elevation_index) * 6.0;
    const double terrain_cost = target->terrain_resistance * 4.5;
    const double exposure_cost = target->structural_exposure * 3.0;
    return distance_cost + step_cost + elevation_cost + terrain_cost + exposure_cost;
}

double nf_energy_horizon_value(
    const NfEnergySystem *system, uint8_t region, NfEnergyHorizon horizon) {
    if (system == NULL || !system->bound || region >= NF_ENERGY_REGION_MAX) return 0.0;
    const NfEnergyRegion *r = &system->regions[region];
    const double usable = r->usable;
    const double productive = r->productive_capacity * r->health;
    const double regenerative = r->regenerative_capacity * r->health;
    const double absorptive = r->absorptive_capacity * r->health;
    const double externality = r->externality_load;

    switch (horizon) {
        case NF_ENERGY_HORIZON_IMMEDIATE:
            return usable - externality * 8.0;
        case NF_ENERGY_HORIZON_ROUND:
            return usable * 0.72 + productive * 24.0 + regenerative * 28.0 - externality * 14.0;
        case NF_ENERGY_HORIZON_REGIONAL:
            return usable * 0.48 + productive * 58.0 + regenerative * 86.0 + r->health * 18.0 - externality * 30.0;
        case NF_ENERGY_HORIZON_REGENERATIVE:
            return productive * 34.0 + regenerative * 132.0 + absorptive * 44.0 + r->health * 30.0 - externality * 52.0;
        default:
            return 0.0;
    }
}

size_t nf_energy_helicopter_search(
    const NfEnergySystem *system, const NfRegionGraph *graph,
    uint8_t origin_region, NfEnergyOpportunity *out, size_t cap) {
    if (system == NULL || !system->bound || out == NULL || cap == 0u) return 0u;
    size_t count = 0u;
    for (uint8_t region = 0u; region < NF_ENERGY_REGION_MAX; ++region) {
        insert_opportunity(
            out, cap, &count,
            truth_opportunity(system, graph, origin_region, region));
    }
    return count;
}

void nf_energy_belief_init(
    NfEnergyBelief *belief, const NfEnergySystem *system) {
    if (belief == NULL) return;
    memset(belief, 0, sizeof(*belief));
    if (system == NULL || !system->bound) return;
    for (uint8_t region = 0u; region < NF_ENERGY_REGION_MAX; ++region) {
        belief->mean[region] = nf_energy_horizon_value(
            system, region, NF_ENERGY_HORIZON_REGIONAL) * 0.65;
        belief->confidence[region] = 0.12;
    }
}

void nf_energy_belief_observe(
    NfEnergyBelief *belief, const NfEnergySystem *system,
    uint8_t region, double evidence_confidence, uint64_t tick) {
    if (belief == NULL || system == NULL || !system->bound ||
        region >= NF_ENERGY_REGION_MAX) return;
    const double c = clamp01d(evidence_confidence);
    const double truth = nf_energy_horizon_value(
        system, region, NF_ENERGY_HORIZON_REGIONAL);
    const double prior_precision = 1.0 + belief->confidence[region] * 5.0;
    const double evidence_precision = 1.0 + c * 9.0;
    belief->mean[region] =
        (belief->mean[region] * prior_precision + truth * evidence_precision) /
        (prior_precision + evidence_precision);
    belief->confidence[region] = clamp01d(
        belief->confidence[region] + (1.0 - belief->confidence[region]) * c * 0.72);
    belief->observed_tick[region] = tick;
}

void nf_energy_belief_decay(
    NfEnergyBelief *belief, uint64_t now_tick) {
    if (belief == NULL) return;
    for (uint8_t region = 0u; region < NF_ENERGY_REGION_MAX; ++region) {
        if (belief->observed_tick[region] == 0u || now_tick <= belief->observed_tick[region]) continue;
        const uint64_t age = now_tick - belief->observed_tick[region];
        if (age <= NF_TICK_RATE * 5u) continue;
        const double seconds = (double)(age - NF_TICK_RATE * 5u) / (double)NF_TICK_RATE;
        const double factor = exp(-seconds * 0.006);
        belief->confidence[region] = clamp01d(belief->confidence[region] * factor);
    }
}

double nf_energy_belief_candidate_score(
    const NfEnergyBelief *belief, const NfEnergySystem *system,
    const NfRegionGraph *graph, uint8_t origin_region,
    uint8_t candidate_region, NfEnergyHorizon horizon) {
    if (belief == NULL || system == NULL || graph == NULL || !system->bound ||
        candidate_region >= NF_ENERGY_REGION_MAX) return -DBL_MAX;
    const double confidence = clamp01d(belief->confidence[candidate_region]);
    const double prior = system->regions[candidate_region].prior_yield * 100.0;
    double expected = belief->mean[candidate_region] * confidence + prior * (1.0 - confidence);
    if (horizon == NF_ENERGY_HORIZON_IMMEDIATE) expected *= 0.72;
    else if (horizon == NF_ENERGY_HORIZON_ROUND) expected *= 0.88;
    else if (horizon == NF_ENERGY_HORIZON_REGENERATIVE) expected *= 1.12;
    const double cost = nf_energy_topographic_cost(
        system, graph, origin_region, candidate_region);
    return expected - cost;
}

size_t nf_energy_belief_search(
    const NfEnergyBelief *belief, const NfEnergySystem *system,
    const NfRegionGraph *graph, uint8_t origin_region,
    NfEnergyHorizon horizon, NfEnergyOpportunity *out, size_t cap) {
    if (belief == NULL || system == NULL || graph == NULL || out == NULL || cap == 0u) {
        return 0u;
    }
    size_t count = 0u;
    for (uint8_t region = 0u; region < NF_ENERGY_REGION_MAX; ++region) {
        NfEnergyOpportunity candidate = truth_opportunity(system, graph, origin_region, region);
        candidate.posterior_mean = belief->mean[region];
        candidate.uncertainty = 1.0 - clamp01d(belief->confidence[region]);
        candidate.score = nf_energy_belief_candidate_score(
            belief, system, graph, origin_region, region, horizon);
        insert_opportunity(out, cap, &count, candidate);
    }
    return count;
}

void nf_energy_tick(
    NfEnergySystem *system, const NfWorld *world, double dt) {
    if (system == NULL || world == NULL || !system->bound || dt <= 0.0) return;

    for (uint8_t region = 0u; region < NF_ENERGY_REGION_MAX; ++region) {
        system->regions[region].last_extraction = 0.0;
        system->regions[region].last_regeneration = 0.0;
    }

    for (size_t i = 0u; i < NF_MAX_ENTITIES; ++i) {
        const NfActor *actor = &world->actors[i];
        if (!actor->active || !actor->combat.alive || actor->faction == NF_FACTION_NONE) continue;
        const uint8_t region = nf_energy_nearest_region(
            system, (double)actor->transform.position.x, (double)actor->transform.position.z);
        if (region >= NF_ENERGY_REGION_MAX) continue;
        NfEnergyRegion *place = &system->regions[region];
        NfEnergyAccount *account = account_mutable(system, actor->id);
        if (account == NULL) continue;

        const double vx = (double)actor->transform.velocity.x;
        const double vy = (double)actor->transform.velocity.y;
        const double vz = (double)actor->transform.velocity.z;
        const double horizontal_speed = sqrt(vx*vx + vz*vz);
        const double climb = vy > 0.0 ? vy : 0.0;
        const double movement_cost =
            horizontal_speed * dt * (0.012 + place->terrain_resistance * 0.020) +
            climb * dt * 0.030;
        account->expenditure += movement_cost;
        system->total_expenditure += movement_cost;
        account->reserves = account->reserves > movement_cost
            ? account->reserves - movement_cost : 0.0;

        const bool cattler = actor->faction == NF_FACTION_RANCHER;
        const double extraction_rate = cattler ? 0.12 : 0.78;
        const double requested = extraction_rate * dt * (0.85 + place->health * 0.15);
        const double extracted = min_d(place->usable, requested);
        place->usable -= extracted;
        place->last_extraction += extracted;
        place->cumulative_extraction += extracted;
        account->reserves += extracted;
        account->income += extracted;
        account->productive_capacity = clampd(
            account->productive_capacity + extracted * 0.0008, 0.0, 1.0);
        place->externality_load = clamp01d(
            place->externality_load + extracted * (1.0 - place->absorptive_capacity) * 0.0035);
    }

    for (uint8_t region = 0u; region < NF_ENERGY_REGION_MAX; ++region) {
        NfEnergyRegion *place = &system->regions[region];
        const double regeneration =
            place->regenerative_capacity * place->health * dt * 0.42;
        const double potential_room =
            place->baseline_potential * 1.55 - (place->potential + place->usable);
        const double accepted_regeneration = min_d(
            regeneration, potential_room > 0.0 ? potential_room : 0.0);
        place->potential += accepted_regeneration;
        place->last_regeneration += accepted_regeneration;
        place->cumulative_regeneration += accepted_regeneration;
        system->total_external_input += accepted_regeneration;

        const double conversion_input = min_d(
            place->potential,
            place->productive_capacity * place->health * dt * 0.52);
        if (conversion_input > 0.0) {
            const double efficiency = clampd(0.70 + place->health * 0.24, 0.0, 1.0);
            place->potential -= conversion_input;
            place->usable += conversion_input * efficiency;
            system->total_conversion_loss += conversion_input * (1.0 - efficiency);
        }

        const double overshoot = place->last_extraction - place->last_regeneration;
        if (overshoot > 0.0) {
            place->health = clamp01d(place->health - overshoot * 0.0010);
        } else {
            place->health = clamp01d(
                place->health + place->absorptive_capacity * dt * 0.0008);
        }
        place->externality_load = clamp01d(
            place->externality_load - place->absorptive_capacity * dt * 0.0018);
        update_posterior(place);
    }

    if (world->tick >= system->last_search_tick + NF_ENERGY_SEARCH_INTERVAL_TICKS) {
        recompute_global_helicopter(system);
        system->last_search_tick = world->tick;
    }
}

const char *nf_energy_horizon_name(NfEnergyHorizon horizon) {
    switch (horizon) {
        case NF_ENERGY_HORIZON_IMMEDIATE: return "IMMEDIATE";
        case NF_ENERGY_HORIZON_ROUND: return "ROUND";
        case NF_ENERGY_HORIZON_REGIONAL: return "REGIONAL";
        case NF_ENERGY_HORIZON_REGENERATIVE: return "REGENERATIVE";
        default: return "UNKNOWN";
    }
}
