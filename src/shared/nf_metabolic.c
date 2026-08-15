#include "nf_metabolic.h"

#include <math.h>
#include <string.h>

static double clamp01(double v) {
    return v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v);
}
static double maxd(double a, double b) { return a > b ? a : b; }

static NfMetabolicLink *link_mut(
    NfMetabolicSystem *system, uint32_t key) {
    if (system == NULL || key == 0u) return NULL;
    for (size_t i = 0u; i < NF_METABOLIC_LINK_CAP; ++i)
        if (system->links[i].active && system->links[i].key == key)
            return &system->links[i];
    return NULL;
}

void nf_metabolic_init(NfMetabolicSystem *system) {
    if (system == NULL) return;
    memset(system, 0, sizeof(*system));
    system->next_key = 1u;
}

uint32_t nf_metabolic_bind(
    NfMetabolicSystem *system, uint8_t source_region, uint8_t sink_region) {
    if (system == NULL || source_region >= NF_ENERGY_REGION_MAX ||
        sink_region >= NF_ENERGY_REGION_MAX || source_region == sink_region) return 0u;
    for (size_t i = 0u; i < NF_METABOLIC_LINK_CAP; ++i) {
        NfMetabolicLink *link = &system->links[i];
        if (link->active && link->source_region == source_region &&
            link->sink_region == sink_region) return link->key;
    }
    for (size_t i = 0u; i < NF_METABOLIC_LINK_CAP; ++i) {
        NfMetabolicLink *link = &system->links[i];
        if (link->active) continue;
        if (system->next_key == 0u) system->next_key = 1u;
        *link = (NfMetabolicLink){
            .active = true,
            .key = system->next_key++,
            .source_region = source_region,
            .sink_region = sink_region,
            .restitution_ratio = 1.0
        };
        return link->key;
    }
    return 0u;
}

bool nf_metabolic_record_outflow(
    NfMetabolicSystem *system, uint32_t key, double amount,
    double loss, uint64_t tick) {
    NfMetabolicLink *link = link_mut(system, key);
    if (link == NULL || amount <= 0.0 || !isfinite(amount)) return false;
    const double bounded_loss = loss < 0.0 ? 0.0 : (loss > amount ? amount : loss);
    link->recent_outflow += amount;
    link->cumulative_outflow += amount;
    link->cumulative_loss += bounded_loss;
    link->last_flow_tick = tick;
    return true;
}

bool nf_metabolic_record_return(
    NfMetabolicSystem *system, uint32_t key, double amount, uint64_t tick) {
    NfMetabolicLink *link = link_mut(system, key);
    if (link == NULL || amount <= 0.0 || !isfinite(amount)) return false;
    link->recent_return += amount;
    link->cumulative_return += amount;
    link->last_return_tick = tick;
    return true;
}

void nf_metabolic_tick(
    NfMetabolicSystem *system, NfEnergySystem *energy, double dt, uint64_t tick) {
    if (system == NULL || energy == NULL || !energy->bound || dt <= 0.0) return;

    for (size_t i = 0u; i < NF_METABOLIC_LINK_CAP; ++i) {
        NfMetabolicLink *link = &system->links[i];
        if (!link->active) continue;
        NfEnergyRegion *source = &energy->regions[link->source_region];
        NfEnergyRegion *sink = &energy->regions[link->sink_region];

        const double local_regeneration = maxd(0.0, source->last_regeneration);
        const double restitution = link->recent_return + local_regeneration;
        link->restitution_gap = maxd(0.0, link->recent_outflow - restitution);
        link->restitution_ratio = link->recent_outflow > 0.0001
            ? clamp01(restitution / link->recent_outflow)
            : 1.0;

        const double age_seconds =
            link->last_return_tick > 0u && tick > link->last_return_tick
            ? (double)(tick - link->last_return_tick) / 60.0 : 0.0;
        link->dependency = clamp01(
            link->dependency*0.995 +
            (link->restitution_gap > 0.05 ? 0.003*dt*60.0 : -0.002*dt*60.0) +
            (age_seconds > 3.0 ? 0.001*dt*60.0 : 0.0));

        const double pressure = clamp01(
            link->restitution_gap / (1.0 + link->recent_outflow));
        const double overload = clamp01(
            sink->externality_load - sink->absorptive_capacity);

        source->productive_capacity = clamp01(
            source->productive_capacity - pressure*0.006*dt);
        source->regenerative_capacity = clamp01(
            source->regenerative_capacity - pressure*0.003*dt);
        sink->externality_load = clamp01(
            sink->externality_load + pressure*0.008*dt);
        if (overload > 0.0) {
            sink->absorptive_capacity = clamp01(
                sink->absorptive_capacity - overload*0.002*dt);
        }

        const double fade = exp(-dt*0.35);
        link->recent_outflow *= fade;
        link->recent_return *= fade;
    }
}

const NfMetabolicLink *nf_metabolic_link_const(
    const NfMetabolicSystem *system, uint32_t key) {
    if (system == NULL || key == 0u) return NULL;
    for (size_t i = 0u; i < NF_METABOLIC_LINK_CAP; ++i)
        if (system->links[i].active && system->links[i].key == key)
            return &system->links[i];
    return NULL;
}

double nf_metabolic_restitution_gap(
    const NfMetabolicSystem *system, uint32_t key) {
    const NfMetabolicLink *link = nf_metabolic_link_const(system, key);
    return link != NULL ? link->restitution_gap : 0.0;
}

const char *nf_metabolic_state_name(const NfMetabolicLink *link) {
    if (link == NULL || !link->active) return "NONE";
    if (link->restitution_ratio >= 0.85) return "CYCLE_CONTINUOUS";
    if (link->restitution_ratio >= 0.55) return "RETURN_DEFICIT";
    return "CYCLE_BREAK";
}
