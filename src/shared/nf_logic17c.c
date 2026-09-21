#include "nf_logic17c.h"

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

static size_t calibration_bin(float confidence) {
    const float c = clamp01(confidence);
    size_t bin = (size_t)(c * (float)NF17C_CALIBRATION_BINS);
    if (bin >= NF17C_CALIBRATION_BINS) bin = NF17C_CALIBRATION_BINS - 1u;
    return bin;
}

void nf17c_calibrator_init(Nf17cCalibrator *calibrator, float prior_correct, float prior_total) {
    if (calibrator == NULL) return;
    memset(calibrator, 0, sizeof(*calibrator));
    calibrator->prior_correct = prior_correct < 0.0f ? 0.0f : prior_correct;
    calibrator->prior_total = prior_total < 0.0f ? 0.0f : prior_total;
    if (calibrator->prior_correct > calibrator->prior_total) {
        calibrator->prior_correct = calibrator->prior_total;
    }
}

void nf17c_calibrator_observe(Nf17cCalibrator *calibrator, float raw_confidence, bool correct) {
    if (calibrator == NULL) return;
    Nf17cCalibrationBin *bin = &calibrator->bins[calibration_bin(raw_confidence)];
    if (bin->count < UINT32_MAX) ++bin->count;
    if (correct && bin->correct < UINT32_MAX) ++bin->correct;
}

float nf17c_calibrator_predict(const Nf17cCalibrator *calibrator, float raw_confidence) {
    if (calibrator == NULL) return clamp01(raw_confidence);
    const Nf17cCalibrationBin *bin = &calibrator->bins[calibration_bin(raw_confidence)];
    const float denom = calibrator->prior_total + (float)bin->count;
    if (denom <= 0.0f) return clamp01(raw_confidence);
    return clamp01((calibrator->prior_correct + (float)bin->correct) / denom);
}

float nf17c_brier(float confidence, bool correct) {
    const float target = correct ? 1.0f : 0.0f;
    const float d = clamp01(confidence) - target;
    return d * d;
}

bool nf17c_utility_better(const Nf17cUtilityCandidate *a, const Nf17cUtilityCandidate *b) {
    if (a == NULL) return false;
    if (b == NULL) return true;

    if (a->materially_admissible != b->materially_admissible)
        return a->materially_admissible > b->materially_admissible;
    if (a->contract_admissible != b->contract_admissible)
        return a->contract_admissible > b->contract_admissible;
    if (a->safety_class != b->safety_class)
        return a->safety_class > b->safety_class;
    if (a->authority_class != b->authority_class)
        return a->authority_class > b->authority_class;

    const float a_soft = a->preference - 0.5f * a->risk - 0.25f * a->energy_cost;
    const float b_soft = b->preference - 0.5f * b->risk - 0.25f * b->energy_cost;
    if (fabsf(a_soft - b_soft) > 0.00001f) return a_soft > b_soft;
    return a->action_id < b->action_id;
}

int nf17c_choose_utility(const Nf17cUtilityCandidate *candidates, size_t count) {
    if (candidates == NULL || count == 0u) return -1;
    size_t best = 0u;
    for (size_t i = 1u; i < count; ++i)
        if (nf17c_utility_better(&candidates[i], &candidates[best])) best = i;
    return (int)best;
}

size_t nf17c_build_purple_envelopes(
    const Nf17cDomainDependency *deps,
    size_t dep_count,
    uint32_t tick,
    Nf17cPurpleEnvelope *out,
    size_t out_capacity) {

    bool reach[NF17B_DOMAIN_COUNT][NF17B_DOMAIN_COUNT];
    bool assigned[NF17B_DOMAIN_COUNT];
    memset(reach, 0, sizeof(reach));
    memset(assigned, 0, sizeof(assigned));

    for (size_t i = 0u; i < NF17B_DOMAIN_COUNT; ++i) reach[i][i] = true;

    for (size_t i = 0u; i < dep_count; ++i) {
        const Nf17cDomainDependency *d = &deps[i];
        if (d->from_domain >= (uint8_t)NF17B_DOMAIN_COUNT ||
            d->to_domain >= (uint8_t)NF17B_DOMAIN_COUNT) continue;
        if (d->from_domain == d->to_domain || d->material_channel_mask == 0u) continue;
        reach[d->from_domain][d->to_domain] = true;
    }

    for (size_t k = 0u; k < NF17B_DOMAIN_COUNT; ++k)
        for (size_t i = 0u; i < NF17B_DOMAIN_COUNT; ++i)
            for (size_t j = 0u; j < NF17B_DOMAIN_COUNT; ++j)
                reach[i][j] = reach[i][j] || (reach[i][k] && reach[k][j]);

    size_t total = 0u;
    for (size_t i = 0u; i < NF17B_DOMAIN_COUNT; ++i) {
        if (assigned[i]) continue;

        uint32_t mask = 0u;
        uint8_t members = 0u;
        for (size_t j = 0u; j < NF17B_DOMAIN_COUNT; ++j) {
            if (reach[i][j] && reach[j][i]) {
                mask |= 1u << j;
                ++members;
            }
        }

        if (members <= 1u) {
            assigned[i] = true;
            continue;
        }

        for (size_t j = 0u; j < NF17B_DOMAIN_COUNT; ++j)
            if ((mask & (1u << j)) != 0u) assigned[j] = true;

        if (out != NULL && total < out_capacity) {
            Nf17cPurpleEnvelope *e = &out[total];
            memset(e, 0, sizeof(*e));
            e->tick = tick;
            e->domain_mask = mask;
            e->member_count = members;
            e->status = (uint8_t)NF17C_PURPLE_ACTIVE;

            uint32_t h = 2166136261u;
            for (size_t d = 0u; d < dep_count; ++d) {
                const Nf17cDomainDependency *edge = &deps[d];
                if (edge->from_domain >= (uint8_t)NF17B_DOMAIN_COUNT ||
                    edge->to_domain >= (uint8_t)NF17B_DOMAIN_COUNT) continue;
                if ((mask & (1u << edge->from_domain)) == 0u ||
                    (mask & (1u << edge->to_domain)) == 0u ||
                    edge->material_channel_mask == 0u) continue;
                h ^= mix32(edge->target_id);
                h *= 16777619u;
                h ^= mix32(edge->material_channel_mask);
                h *= 16777619u;
            }
            e->dependency_hash = h;
        }
        ++total;
    }

    return total;
}

void nf17c_set_purple_status(Nf17cPurpleEnvelope *envelope, Nf17cPurpleStatus status) {
    if (envelope == NULL) return;
    if (status < NF17C_PURPLE_ACTIVE || status > NF17C_PURPLE_PENDING) return;
    envelope->status = (uint8_t)status;
}

size_t nf17c_fractional_attribution_design(
    Nf17cAttributionRow *out,
    size_t out_capacity,
    uint8_t replicates) {

    static const uint32_t design[8] = {
        0u,
        NF17C_FACTOR_ARBITRATION | NF17C_FACTOR_TOPOLOGY,
        NF17C_FACTOR_ARBITRATION | NF17C_FACTOR_CALIBRATION,
        NF17C_FACTOR_ARBITRATION | NF17C_FACTOR_APPRAISAL,
        NF17C_FACTOR_TOPOLOGY | NF17C_FACTOR_CALIBRATION,
        NF17C_FACTOR_TOPOLOGY | NF17C_FACTOR_APPRAISAL,
        NF17C_FACTOR_CALIBRATION | NF17C_FACTOR_APPRAISAL,
        NF17C_FACTOR_ARBITRATION | NF17C_FACTOR_TOPOLOGY |
            NF17C_FACTOR_CALIBRATION | NF17C_FACTOR_APPRAISAL
    };

    size_t total = 0u;
    for (uint8_t rep = 0u; rep < replicates; ++rep) {
        for (size_t i = 0u; i < sizeof(design) / sizeof(design[0]); ++i) {
            if (out != NULL && total < out_capacity) {
                out[total].enabled_factors = design[i];
                out[total].replicate = rep;
                out[total].reserved[0] = 0u;
                out[total].reserved[1] = 0u;
                out[total].reserved[2] = 0u;
            }
            ++total;
        }
    }
    return total;
}

uint32_t nf17c_reason_trace_hash(const Nf17cReasonTrace *trace) {
    if (trace == NULL) return 0u;
    uint32_t h = 2166136261u;
    const uint32_t values[] = {
        trace->tick,
        trace->actor_id,
        trace->evidence_hash,
        trace->belief_hash,
        trace->contract_hash,
        trace->affordance_hash,
        trace->transaction_hash,
        (uint32_t)lroundf(clamp01(trace->raw_confidence) * 1000000.0f),
        (uint32_t)lroundf(clamp01(trace->calibrated_confidence) * 1000000.0f),
        trace->outcome_known,
        trace->outcome_success
    };
    for (size_t i = 0u; i < sizeof(values) / sizeof(values[0]); ++i) {
        h ^= mix32(values[i] + (uint32_t)i * 0x9e3779b9u);
        h *= 16777619u;
    }
    return h;
}

const char *nf17c_purple_status_name(Nf17cPurpleStatus status) {
    static const char *names[] = {"active", "committed", "decomposed", "split", "expanded", "pending"};
    return status >= NF17C_PURPLE_ACTIVE && status <= NF17C_PURPLE_PENDING
        ? names[status] : "unknown";
}
