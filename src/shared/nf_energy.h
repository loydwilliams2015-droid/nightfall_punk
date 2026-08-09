#ifndef NF_ENERGY_H
#define NF_ENERGY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_ENERGY_REGION_MAX 24u
#define NF_ENERGY_TOP_CANDIDATES 4u
#define NF_ENERGY_MAX_ACCOUNTS 32u
#define NF_ENERGY_REGION_INVALID 255u

typedef enum NfEnergyPostingType {
    NF_ENERGY_POST_TRANSFER = 0,
    NF_ENERGY_POST_CONVERSION,
    NF_ENERGY_POST_CAPACITY,
    NF_ENERGY_POST_EXTERNAL_INPUT,
    NF_ENERGY_POST_EXPENDITURE
} NfEnergyPostingType;

typedef enum NfEnergyHorizon {
    NF_ENERGY_HORIZON_IMMEDIATE = 0,
    NF_ENERGY_HORIZON_ROUND,
    NF_ENERGY_HORIZON_REGIONAL,
    NF_ENERGY_HORIZON_REGENERATIVE
} NfEnergyHorizon;

typedef struct NfEnergyRegion {
    double baseline_potential;
    double potential;
    double usable;
    double productive_capacity;
    double regenerative_capacity;
    double absorptive_capacity;
    double health;
    double externality_load;
    double terrain_resistance;
    double elevation_index;
    double structural_exposure;
    double prior_yield;
    double posterior_yield;
    double uncertainty;
    double last_extraction;
    double last_regeneration;
    double cumulative_extraction;
    double cumulative_regeneration;
    double center_x;
    double center_z;
} NfEnergyRegion;

typedef struct NfEnergyAccount {
    bool active;
    uint32_t entity_id;
    double reserves;
    double income;
    double expenditure;
    double productive_capacity;
} NfEnergyAccount;

typedef struct NfEnergyPosting {
    NfEnergyPostingType type;
    uint8_t source_region;
    uint8_t sink_region;
    uint32_t actor_id;
    double amount;
    double efficiency;
    uint64_t tick;
} NfEnergyPosting;

typedef struct NfEnergyOpportunity {
    uint8_t region;
    double immediate_return;
    double round_return;
    double regional_return;
    double regenerative_return;
    double topographic_cost;
    double posterior_mean;
    double uncertainty;
    double score;
} NfEnergyOpportunity;

typedef struct NfEnergyBelief {
    double mean[NF_ENERGY_REGION_MAX];
    double confidence[NF_ENERGY_REGION_MAX];
    uint64_t observed_tick[NF_ENERGY_REGION_MAX];
} NfEnergyBelief;

typedef struct NfEnergySystem {
    bool bound;
    NfEnergyRegion regions[NF_ENERGY_REGION_MAX];
    NfEnergyAccount accounts[NF_ENERGY_MAX_ACCOUNTS];
    NfEnergyOpportunity helicopter[NF_ENERGY_TOP_CANDIDATES];
    size_t helicopter_count;
    uint64_t last_search_tick;
    double total_external_input;
    double total_conversion_loss;
    double total_transferred;
    double total_expenditure;
    uint32_t seed;
} NfEnergySystem;

struct NfWorld;
struct NfRegionGraph;

void nf_energy_init(NfEnergySystem *system, uint32_t seed);
void nf_energy_bind_spatial_lab(
    NfEnergySystem *system, const struct NfRegionGraph *graph);
void nf_energy_tick(
    NfEnergySystem *system, const struct NfWorld *world, double dt);

const NfEnergyRegion *nf_energy_region_const(
    const NfEnergySystem *system, uint8_t region);
const NfEnergyAccount *nf_energy_account_const(
    const NfEnergySystem *system, uint32_t entity_id);
uint8_t nf_energy_nearest_region(
    const NfEnergySystem *system, double x, double z);

double nf_energy_total_stored(const NfEnergySystem *system);
double nf_energy_transfer(
    NfEnergySystem *system, uint8_t source_region, uint8_t sink_region,
    double amount);
double nf_energy_convert(
    NfEnergySystem *system, uint8_t region, double amount, double efficiency);
double nf_energy_change_capacity(
    NfEnergySystem *system, uint8_t region, double delta);

double nf_energy_topographic_cost(
    const NfEnergySystem *system, const struct NfRegionGraph *graph,
    uint8_t from_region, uint8_t to_region);
double nf_energy_horizon_value(
    const NfEnergySystem *system, uint8_t region, NfEnergyHorizon horizon);
size_t nf_energy_helicopter_search(
    const NfEnergySystem *system, const struct NfRegionGraph *graph,
    uint8_t origin_region, NfEnergyOpportunity *out, size_t cap);

void nf_energy_belief_init(
    NfEnergyBelief *belief, const NfEnergySystem *system);
void nf_energy_belief_observe(
    NfEnergyBelief *belief, const NfEnergySystem *system,
    uint8_t region, double evidence_confidence, uint64_t tick);
void nf_energy_belief_decay(
    NfEnergyBelief *belief, uint64_t now_tick);
double nf_energy_belief_candidate_score(
    const NfEnergyBelief *belief, const NfEnergySystem *system,
    const struct NfRegionGraph *graph, uint8_t origin_region,
    uint8_t candidate_region, NfEnergyHorizon horizon);
size_t nf_energy_belief_search(
    const NfEnergyBelief *belief, const NfEnergySystem *system,
    const struct NfRegionGraph *graph, uint8_t origin_region,
    NfEnergyHorizon horizon, NfEnergyOpportunity *out, size_t cap);

const char *nf_energy_horizon_name(NfEnergyHorizon horizon);

#endif
