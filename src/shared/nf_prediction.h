#ifndef NF_PREDICTION_H
#define NF_PREDICTION_H
#include "nf_protocol.h"

typedef struct NfPredictionSlot {
    bool valid;
    NfInputCommand command;
    NfActorNetState predicted;
} NfPredictionSlot;

typedef struct NfPrediction {
    uint32_t next_sequence;
    uint32_t last_acknowledged;
    uint32_t corrections;
    float last_error;
    float max_error;
    NfPredictionSlot history[NF_NET_HISTORY];
} NfPrediction;

void nf_prediction_init(NfPrediction *prediction);
NfInputCommand nf_prediction_make_command(NfPrediction *prediction, uint64_t client_tick, NfMoveInput move);
void nf_prediction_record(NfPrediction *prediction, const NfInputCommand *command, const NfActor *actor);
uint8_t nf_prediction_build_redundant_bundle(const NfPrediction *prediction, uint32_t newest_sequence, NfInputBundle *bundle);
bool nf_prediction_reconcile(NfPrediction *prediction, NfWorld *world, NfEntityId entity_id, const NfActorNetState *authoritative, uint32_t acknowledged_input, uint64_t server_tick, float correction_threshold);
#endif
