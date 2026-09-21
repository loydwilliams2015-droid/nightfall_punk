#include "nf_logic17c.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static int failures = 0;

static void expect_true(bool value, const char *name) {
    if (!value) {
        fprintf(stderr, "FAIL: %s\n", name);
        ++failures;
    }
}

static void expect_int(int actual, int expected, const char *name) {
    if (actual != expected) {
        fprintf(stderr, "FAIL: %s actual=%d expected=%d\n", name, actual, expected);
        ++failures;
    }
}

int main(void) {
    Nf17cCalibrator cal;
    nf17c_calibrator_init(&cal, 1.0f, 2.0f);
    for (int i = 0; i < 8; ++i) nf17c_calibrator_observe(&cal, 0.85f, i < 6);
    const float calibrated = nf17c_calibrator_predict(&cal, 0.85f);
    expect_true(calibrated > 0.60f && calibrated < 0.80f, "calibration separates raw confidence from empirical reliability");
    expect_true(nf17c_brier(calibrated, true) < nf17c_brier(0.25f, true), "better calibrated confidence lowers Brier loss");

    Nf17cUtilityCandidate candidates[3] = {
        {1u, 1u, 1u, 2u, 2u, 0.60f, 0.20f, 0.20f},
        {2u, 1u, 0u, 3u, 3u, 1.00f, 0.00f, 0.00f},
        {3u, 1u, 1u, 2u, 2u, 0.75f, 0.20f, 0.15f}
    };
    expect_int(nf17c_choose_utility(candidates, 3u), 2, "lexicographic admissibility precedes soft preference");

    Nf17cDomainDependency one_way[1] = {
        {(uint8_t)NF17B_DOMAIN_MATERIAL, (uint8_t)NF17B_DOMAIN_STRUCTURE, 0u, 100u, 1u}
    };
    Nf17cPurpleEnvelope purple[NF17C_MAX_PURPLE_ENVELOPES];
    expect_int((int)nf17c_build_purple_envelopes(one_way, 1u, 10u, purple, NF17C_MAX_PURPLE_ENVELOPES),
               0, "one-way cross-domain transmission is not Purple");

    Nf17cDomainDependency mutual[2] = {
        {(uint8_t)NF17B_DOMAIN_MATERIAL, (uint8_t)NF17B_DOMAIN_STRUCTURE, 0u, 100u, 1u},
        {(uint8_t)NF17B_DOMAIN_STRUCTURE, (uint8_t)NF17B_DOMAIN_MATERIAL, 0u, 100u, 2u}
    };
    expect_int((int)nf17c_build_purple_envelopes(mutual, 2u, 11u, purple, NF17C_MAX_PURPLE_ENVELOPES),
               1, "mutual same-tick dependency creates Purple");
    expect_int((int)purple[0].member_count, 2, "Purple contains only coupled component");
    expect_true((purple[0].domain_mask & (1u << NF17B_DOMAIN_MATERIAL)) != 0u, "Purple contains material domain");
    expect_true((purple[0].domain_mask & (1u << NF17B_DOMAIN_STRUCTURE)) != 0u, "Purple contains structure domain");
    nf17c_set_purple_status(&purple[0], NF17C_PURPLE_PENDING);
    expect_int((int)purple[0].status, (int)NF17C_PURPLE_PENDING, "budget exhaustion may remain pending");

    Nf17cAttributionRow rows[NF17C_MAX_ATTRIBUTION_ROWS];
    const size_t design_n = nf17c_fractional_attribution_design(rows, NF17C_MAX_ATTRIBUTION_ROWS, 2u);
    expect_int((int)design_n, 16, "fractional attribution produces reproducible paired design");
    expect_int((int)rows[0].enabled_factors, 0, "attribution includes baseline");
    expect_true((rows[7].enabled_factors & NF17C_FACTOR_CALIBRATION) != 0u, "attribution includes full incumbent");

    Nf17cReasonTrace trace;
    memset(&trace, 0, sizeof(trace));
    trace.tick = 12u;
    trace.actor_id = 7u;
    trace.evidence_hash = 11u;
    trace.belief_hash = 13u;
    trace.contract_hash = 17u;
    trace.affordance_hash = 19u;
    trace.transaction_hash = 23u;
    trace.raw_confidence = 0.85f;
    trace.calibrated_confidence = calibrated;
    trace.outcome_known = 1u;
    trace.outcome_success = 1u;
    const uint32_t h1 = nf17c_reason_trace_hash(&trace);
    const uint32_t h2 = nf17c_reason_trace_hash(&trace);
    expect_true(h1 != 0u && h1 == h2, "reason trace is deterministic and reconstructible");

    if (failures != 0) {
        fprintf(stderr, "nightfall v1.7C calibration/coupling/attribution: FAIL (%d)\n", failures);
        return 1;
    }

    printf("nightfall v1.7C calibration/coupling/attribution: PASS\n");
    printf("LOCK calibration=PASS lexicographic=PASS purple=PASS attribution=PASS trace=PASS\n");
    return 0;
}
