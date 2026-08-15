#ifndef NF_METABOLIC_H
#define NF_METABOLIC_H

#include "nf_energy.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_METABOLIC_LINK_CAP 8u

typedef struct NfMetabolicLink {
    bool active;
    uint32_t key;
    uint8_t source_region;
    uint8_t sink_region;
    double recent_outflow;
    double recent_return;
    double cumulative_outflow;
    double cumulative_return;
    double cumulative_loss;
    double restitution_gap;
    double restitution_ratio;
    double dependency;
    uint64_t last_flow_tick;
    uint64_t last_return_tick;
} NfMetabolicLink;

typedef struct NfMetabolicSystem {
    NfMetabolicLink links[NF_METABOLIC_LINK_CAP];
    uint32_t next_key;
} NfMetabolicSystem;

void nf_metabolic_init(NfMetabolicSystem *system);
uint32_t nf_metabolic_bind(
    NfMetabolicSystem *system, uint8_t source_region, uint8_t sink_region);
bool nf_metabolic_record_outflow(
    NfMetabolicSystem *system, uint32_t key, double amount,
    double loss, uint64_t tick);
bool nf_metabolic_record_return(
    NfMetabolicSystem *system, uint32_t key, double amount, uint64_t tick);
void nf_metabolic_tick(
    NfMetabolicSystem *system, NfEnergySystem *energy, double dt, uint64_t tick);
const NfMetabolicLink *nf_metabolic_link_const(
    const NfMetabolicSystem *system, uint32_t key);
double nf_metabolic_restitution_gap(
    const NfMetabolicSystem *system, uint32_t key);
const char *nf_metabolic_state_name(const NfMetabolicLink *link);

#endif
