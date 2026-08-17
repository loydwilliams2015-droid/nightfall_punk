#ifndef NF_BELIEF_H
#define NF_BELIEF_H

#include "nf_report.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_BELIEF_CAPACITY 24u
#define NF_BELIEF_MAX_COMPETING 3u
#define NF_BELIEF_ANON_MASK 0x80000000u

typedef enum NfBeliefSubjectKind {
    NF_BELIEF_SUBJECT_NONE = 0,
    NF_BELIEF_SUBJECT_ACTOR,
    NF_BELIEF_SUBJECT_UNKNOWN_THREAT,
    NF_BELIEF_SUBJECT_ROUTE,
    NF_BELIEF_SUBJECT_RESOURCE,
    NF_BELIEF_SUBJECT_STASIS
} NfBeliefSubjectKind;

typedef struct NfBeliefObservation {
    NfBeliefSubjectKind kind;
    uint32_t subject_key;
    NfEntityId identified_actor;
    NfVec3 position;
    float confidence;
    float precision_m;
    NfInformationChannel channel;
    uint32_t evidence_id;
    uint32_t ancestry_id;
    uint64_t tick;
} NfBeliefObservation;

typedef struct NfBeliefHypothesis {
    bool active;
    uint32_t hypothesis_id;
    NfBeliefSubjectKind kind;
    uint32_t subject_key;
    NfEntityId identified_actor;
    NfVec3 position;
    float confidence;
    float precision_m;
    NfInformationChannel channel;
    uint32_t last_evidence_id;
    uint32_t ancestry_id;
    uint64_t observed_tick;
    uint64_t updated_tick;
} NfBeliefHypothesis;

typedef struct NfBeliefStore {
    NfBeliefHypothesis hypotheses[NF_BELIEF_CAPACITY];
    uint32_t next_hypothesis_id;
} NfBeliefStore;

void nf_belief_init(NfBeliefStore *store);
bool nf_belief_observe(
    NfBeliefStore *store, const NfBeliefObservation *observation);
bool nf_belief_observe_report(
    NfBeliefStore *store, const NfReport *report, uint64_t now_tick);
void nf_belief_decay(
    NfBeliefStore *store, uint64_t now_tick, float mobility_m_per_second);
const NfBeliefHypothesis *nf_belief_best(
    const NfBeliefStore *store, NfBeliefSubjectKind kind,
    uint32_t subject_key, uint64_t now_tick);
const NfBeliefHypothesis *nf_belief_best_threat(
    const NfBeliefStore *store, uint64_t now_tick);
size_t nf_belief_active_count(const NfBeliefStore *store);
const char *nf_belief_subject_name(NfBeliefSubjectKind kind);

#endif
