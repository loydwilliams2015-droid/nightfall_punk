#include "nf_report.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

static bool nearly_equal(float a, float b) {
    return fabsf(a - b) < 0.001f;
}

int main(void) {
    NfReportBus bus;
    nf_report_bus_init(&bus);

    NfReport direct = {
        .subject_key = 77u,
        .kind = NF_REPORT_ACTOR_POSITION,
        .scope = NF_REPORT_SCOPE_RIVAL_CREW,
        .origin_channel = NF_INFO_CHANNEL_DIRECT_VISUAL,
        .origin = 10u,
        .reporter = 10u,
        .position = {3.12f, 1.13f, 7.88f},
        .confidence = 0.95f,
        .origin_precision_m = 0.25f,
        .precision_m = 0.25f,
        .origin_tick = 100u,
        .issued_tick = 100u,
        .deliver_tick = 110u,
        .expiry_tick = 300u
    };

    uint32_t direct_id = 0u;
    assert(nf_report_publish(&bus, direct, &direct_id));
    assert(direct_id != 0u);
    assert(bus.count == 1u);
    assert(!nf_report_is_live(&bus.reports[0], 109u));
    assert(nf_report_is_live(&bus.reports[0], 110u));
    assert(nearly_equal(bus.reports[0].precision_m, 0.25f));
    assert(nearly_equal(bus.reports[0].origin_precision_m, 0.25f));

    uint32_t impossible_gain_id = 0u;
    assert(nf_report_relay(
        &bus,
        &bus.reports[0],
        11u,
        0.90f,
        0.10f,
        120u,
        130u,
        300u,
        &impossible_gain_id));
    assert(impossible_gain_id != 0u);
    assert(bus.count == 2u);
    const NfReport *no_gain = &bus.reports[1];
    assert(no_gain->hops == 1u);
    assert(no_gain->precision_m + 0.001f >= bus.reports[0].precision_m);
    assert(no_gain->confidence < bus.reports[0].confidence);

    uint32_t coarse_id = 0u;
    assert(nf_report_relay(
        &bus,
        &bus.reports[0],
        12u,
        0.75f,
        4.0f,
        125u,
        150u,
        280u,
        &coarse_id));
    assert(coarse_id != 0u);
    assert(bus.count == 3u);
    const NfReport *coarse = &bus.reports[2];
    assert(nearly_equal(coarse->precision_m, 4.0f));
    assert(nearly_equal(coarse->position.x, roundf(coarse->position.x / 4.0f) * 4.0f));
    assert(nearly_equal(coarse->position.z, roundf(coarse->position.z / 4.0f) * 4.0f));
    assert(!nf_report_is_live(coarse, 149u));
    assert(nf_report_is_live(coarse, 150u));

    assert(nf_report_scope_allows(&bus.reports[0], 20u, NF_FACTION_RIVAL));
    assert(!nf_report_scope_allows(&bus.reports[0], 20u, NF_FACTION_TEAMMATE));

    NfReport best = {0};
    assert(nf_report_best(
        &bus,
        20u,
        NF_FACTION_RIVAL,
        160u,
        NF_REPORT_ACTOR_POSITION,
        77u,
        &best));
    assert(best.subject_key == 77u);
    assert(best.precision_m >= best.origin_precision_m);

    printf("v1.5 information report contract: PASS\n");
    printf("direct_precision=%.2f relay_no_gain=%.2f coarse=%.2f live=%zu\n",
        bus.reports[0].precision_m,
        no_gain->precision_m,
        coarse->precision_m,
        nf_report_live_count(&bus, 160u));
    return 0;
}
