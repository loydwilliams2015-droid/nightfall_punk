#include "nf_belief.h"
#include "nf_comm_present.h"
#include "nf_metabolic.h"
#include "nf_region.h"
#include "nf_report.h"
#include "nf_tactical.h"
#include "nf_world.h"

#include <assert.h>
#include <stdio.h>

static void belief_contract(void) {
    NfBeliefStore store;
    nf_belief_init(&store);
    NfBeliefObservation heard = {
        .kind = NF_BELIEF_SUBJECT_UNKNOWN_THREAT,
        .subject_key = NF_BELIEF_ANON_MASK | 17u,
        .position = {8.0f,0.0f,4.0f},
        .confidence = 0.55f,
        .precision_m = 8.0f,
        .channel = NF_INFO_CHANNEL_AUDIBLE_GUNFIRE,
        .evidence_id = 1u,
        .tick = 100u
    };
    assert(nf_belief_observe(&store, &heard));
    const NfBeliefHypothesis *hypothesis = nf_belief_best_threat(&store, 100u);
    assert(hypothesis != NULL && hypothesis->identified_actor == 0u &&
        hypothesis->precision_m >= 8.0f);

    NfReportBus reports;
    nf_report_bus_init(&reports);
    uint32_t report_id = 0u;
    assert(nf_report_publish(&reports, (NfReport){
        .subject_key = 77u,
        .kind = NF_REPORT_ACTOR_POSITION,
        .scope = NF_REPORT_SCOPE_RIVAL_CREW,
        .origin_channel = NF_INFO_CHANNEL_DIRECT_VISUAL,
        .origin = 10u,
        .reporter = 10u,
        .position = {8.2f,0.0f,4.1f},
        .confidence = 0.80f,
        .origin_precision_m = 0.5f,
        .precision_m = 4.0f,
        .origin_tick = 110u,
        .issued_tick = 110u,
        .deliver_tick = 120u,
        .expiry_tick = 300u
    }, &report_id));
    assert(report_id != 0u);
    assert(nf_belief_observe_report(&store, &reports.reports[0], 120u));
    hypothesis = nf_belief_best(&store, NF_BELIEF_SUBJECT_ACTOR, 77u, 120u);
    assert(hypothesis != NULL && hypothesis->identified_actor == 77u &&
        hypothesis->precision_m >= 4.0f);

    const float before = hypothesis->confidence;
    assert(nf_belief_observe_report(&store, &reports.reports[0], 121u));
    hypothesis = nf_belief_best(&store, NF_BELIEF_SUBJECT_ACTOR, 77u, 121u);
    assert(hypothesis != NULL && hypothesis->confidence <= before+0.001f);
}

static void tactical_contract(void) {
    NfTacticalSnapshot snapshot = {
        .actor_viability = 0.78f,
        .crew_continuity = 0.65f,
        .relation_continuity = 0.50f,
        .locality_value = 0.50f,
        .future_options = 0.60f,
        .information_quality = 0.92f,
        .uncertainty = 0.08f,
        .objective_urgency = 0.70f,
        .ammo_fraction = 0.90f,
        .metabolic_margin = 0.75f
    };
    NfTacticalCandidate candidates[3] = {
        {.feasible=true,.action=NF_TACTICAL_ATTACK,.actor_delta=-0.03f,
         .crew_delta=0.12f,.immediate_gain=0.92f,.action_cost=0.10f,
         .uncertainty_cost=0.03f},
        {.feasible=true,.action=NF_TACTICAL_GUARD,.actor_delta=0.05f,
         .crew_delta=0.18f,.locality_delta=0.20f,.options_delta=0.05f,
         .action_cost=0.04f},
        {.feasible=true,.action=NF_TACTICAL_WITHDRAW,.actor_delta=0.10f,
         .options_delta=0.12f,.action_cost=0.08f}
    };
    assert(nf_tactical_rank(&snapshot, candidates, 3u) == 3u);
    assert(candidates[0].action == NF_TACTICAL_ATTACK);

    snapshot.actor_viability = 0.18f;
    snapshot.ammo_fraction = 0.08f;
    snapshot.objective_urgency = 0.20f;
    candidates[0] = (NfTacticalCandidate){
        .feasible=true,.action=NF_TACTICAL_ATTACK,.actor_delta=-0.30f,
        .immediate_gain=0.35f,.action_cost=0.30f};
    candidates[1] = (NfTacticalCandidate){
        .feasible=true,.action=NF_TACTICAL_GUARD,.actor_delta=0.12f,
        .crew_delta=0.16f,.locality_delta=0.18f,.action_cost=0.04f};
    candidates[2] = (NfTacticalCandidate){
        .feasible=true,.action=NF_TACTICAL_WITHDRAW,.actor_delta=0.42f,
        .options_delta=0.22f,.action_cost=0.06f};
    (void)nf_tactical_rank(&snapshot, candidates, 3u);
    assert(candidates[0].action == NF_TACTICAL_WITHDRAW ||
        candidates[0].action == NF_TACTICAL_GUARD);
}

static void metabolic_contract(void) {
    NfWorld world;
    NfRegionGraph graph;
    nf_world_init(&world, 515u);
    nf_region_graph_init_spatial_lab(&graph);
    nf_energy_bind_spatial_lab(&world.energy, &graph);

    NfMetabolicSystem metabolism;
    nf_metabolic_init(&metabolism);
    const uint32_t key = nf_metabolic_bind(&metabolism, 2u, 14u);
    assert(key != 0u);
    const double before = world.energy.regions[2u].productive_capacity;
    assert(nf_metabolic_record_outflow(&metabolism, key, 12.0, 1.0, 100u));
    for (unsigned i = 0u; i < 120u; ++i) {
        nf_metabolic_tick(&metabolism, &world.energy, 1.0/60.0, 100u+i);
    }
    const NfMetabolicLink *link = nf_metabolic_link_const(&metabolism, key);
    assert(link != NULL && link->restitution_gap > 0.0);
    assert(world.energy.regions[2u].productive_capacity < before);
    assert(nf_metabolic_record_return(&metabolism, key, 11.0, 220u));
    for (unsigned i = 0u; i < 30u; ++i) {
        nf_metabolic_tick(&metabolism, &world.energy, 1.0/60.0, 220u+i);
    }
    link = nf_metabolic_link_const(&metabolism, key);
    assert(link != NULL && link->restitution_ratio > 0.0);
}

static void presentation_contract(void) {
    NfCommPresentBus bus;
    nf_comm_present_init(&bus);
    NfReport report = {
        .id=1u,.ancestry_id=1u,.subject_key=77u,.kind=NF_REPORT_ACTOR_POSITION,
        .scope=NF_REPORT_SCOPE_CREW,.origin_channel=NF_INFO_CHANNEL_DIRECT_VISUAL,
        .origin=2u,.reporter=2u,.position={4.0f,0.0f,8.0f},
        .confidence=0.8f,.origin_precision_m=0.5f,.precision_m=6.0f,
        .origin_tick=10u,.issued_tick=10u,.deliver_tick=10u,.expiry_tick=200u
    };
    assert(nf_comm_present_from_report(
        &bus, &report, NF_COMM_MARK_DANGER, 20u, NULL));
    NfCommPresentEvent out[4];
    assert(nf_comm_present_collect(&bus, 20u, out, 4u) == 1u);
    assert(out[0].precision_m+0.001f >= report.precision_m);
}

int main(void) {
    belief_contract();
    tactical_contract();
    metabolic_contract();
    presentation_contract();
    puts("v1.5 vertical integration contracts: PASS");
    puts("BELIEF anonymous=PASS ancestry_no_gain=PASS");
    puts("TACTICAL nested_ordering=PASS viability_gate=PASS");
    puts("METABOLIC restitution_gap=PASS capacity_consequence=PASS");
    puts("COMM precision_boundary=PASS");
    return 0;
}
