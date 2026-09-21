#ifndef NF_LOGIC17C_H
#define NF_LOGIC17C_H

#include "nf_logic17b.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF17C_CALIBRATION_BINS 10u
#define NF17C_MAX_PURPLE_ENVELOPES 8u
#define NF17C_MAX_ATTRIBUTION_ROWS 16u

typedef struct Nf17cCalibrationBin {
    uint32_t count;
    uint32_t correct;
} Nf17cCalibrationBin;

typedef struct Nf17cCalibrator {
    Nf17cCalibrationBin bins[NF17C_CALIBRATION_BINS];
    float prior_correct;
    float prior_total;
} Nf17cCalibrator;

typedef struct Nf17cUtilityCandidate {
    uint32_t action_id;
    uint8_t materially_admissible;
    uint8_t contract_admissible;
    uint8_t safety_class;
    uint8_t authority_class;
    float preference;
    float risk;
    float energy_cost;
} Nf17cUtilityCandidate;

typedef struct Nf17cDomainDependency {
    uint8_t from_domain;
    uint8_t to_domain;
    uint16_t reserved;
    uint32_t target_id;
    uint32_t material_channel_mask;
} Nf17cDomainDependency;

typedef enum Nf17cPurpleStatus {
    NF17C_PURPLE_ACTIVE = 0,
    NF17C_PURPLE_COMMITTED = 1,
    NF17C_PURPLE_DECOMPOSED = 2,
    NF17C_PURPLE_SPLIT = 3,
    NF17C_PURPLE_EXPANDED = 4,
    NF17C_PURPLE_PENDING = 5
} Nf17cPurpleStatus;

typedef struct Nf17cPurpleEnvelope {
    uint32_t tick;
    uint32_t domain_mask;
    uint32_t dependency_hash;
    uint8_t member_count;
    uint8_t status;
    uint16_t reserved;
} Nf17cPurpleEnvelope;

enum {
    NF17C_FACTOR_ARBITRATION = 1u << 0,
    NF17C_FACTOR_TOPOLOGY = 1u << 1,
    NF17C_FACTOR_CALIBRATION = 1u << 2,
    NF17C_FACTOR_APPRAISAL = 1u << 3
};

typedef struct Nf17cAttributionRow {
    uint32_t enabled_factors;
    uint8_t replicate;
    uint8_t reserved[3];
} Nf17cAttributionRow;

typedef struct Nf17cReasonTrace {
    uint32_t tick;
    uint32_t actor_id;
    uint32_t evidence_hash;
    uint32_t belief_hash;
    uint32_t contract_hash;
    uint32_t affordance_hash;
    uint32_t transaction_hash;
    float raw_confidence;
    float calibrated_confidence;
    uint8_t outcome_known;
    uint8_t outcome_success;
    uint16_t reserved;
} Nf17cReasonTrace;

void nf17c_calibrator_init(Nf17cCalibrator *calibrator, float prior_correct, float prior_total);
void nf17c_calibrator_observe(Nf17cCalibrator *calibrator, float raw_confidence, bool correct);
float nf17c_calibrator_predict(const Nf17cCalibrator *calibrator, float raw_confidence);
float nf17c_brier(float confidence, bool correct);

bool nf17c_utility_better(const Nf17cUtilityCandidate *a, const Nf17cUtilityCandidate *b);
int nf17c_choose_utility(const Nf17cUtilityCandidate *candidates, size_t count);

size_t nf17c_build_purple_envelopes(
    const Nf17cDomainDependency *deps,
    size_t dep_count,
    uint32_t tick,
    Nf17cPurpleEnvelope *out,
    size_t out_capacity);
void nf17c_set_purple_status(Nf17cPurpleEnvelope *envelope, Nf17cPurpleStatus status);

size_t nf17c_fractional_attribution_design(
    Nf17cAttributionRow *out,
    size_t out_capacity,
    uint8_t replicates);

uint32_t nf17c_reason_trace_hash(const Nf17cReasonTrace *trace);

const char *nf17c_purple_status_name(Nf17cPurpleStatus status);

#endif
