#ifndef NF_ENERGY_ACTOR_H
#define NF_ENERGY_ACTOR_H

#include "nf_energy.h"
#include "nf_region.h"

#include <stddef.h>
#include <stdint.h>

/* Actor cognition must never sample mutable authoritative stock/flow truth.
   The system argument is used only for seeded priors and static topographic
   pricing already established by the map/seed. */
void nf_actor_energy_belief_init(
    NfEnergyBelief *belief,
    const NfEnergySystem *system);

/* Evidence is an actor-legitimate estimate derived from perception, reports,
   traffic, process state or traces. No NfEnergySystem argument is accepted. */
void nf_actor_energy_belief_observe(
    NfEnergyBelief *belief,
    uint8_t region,
    double estimated_regional_return,
    double evidence_confidence,
    uint64_t tick);

void nf_actor_energy_belief_decay(
    NfEnergyBelief *belief,
    uint64_t now_tick);

size_t nf_actor_energy_belief_search(
    const NfEnergyBelief *belief,
    const NfEnergySystem *system,
    const NfRegionGraph *graph,
    uint8_t origin_region,
    NfEnergyHorizon horizon,
    NfEnergyOpportunity *out,
    size_t cap);

#endif
