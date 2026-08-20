#ifndef NF_PREDICTION_H
#define NF_PREDICTION_H
#include "nf_protocol.h"

typedef struct NfPredictionSlot {
    bool valid;
    NfInputCommand command;
    NfActorNetState predicted;
    NfWeaponAuthorityNetState predicted_authority;
} NfPredictionSlot;

typedef struct NfPrediction {
    uint32_t next_sequence;
    uint32_t last_acknowledged;
    uint32_t corrections;
    uint32_t resyncs;
    uint32_t authority_corrections;
    float last_error;
    float max_error;
    float last_authority_error;
    float max_authority_error;
    NfPredictionSlot history[NF_NET_HISTORY];
} NfPrediction;

void nf_prediction_init(NfPrediction *prediction);
NfInputCommand nf_prediction_make_command(NfPrediction *prediction, uint64_t client_tick, NfMoveInput move, NfCombatInput combat);
/* Canonical v1.6B local simulation order: movement first, then Weapon Authority/combat. */
void nf_prediction_simulate_command(NfWorld *world, NfEntityId entity_id, const NfInputCommand *command);
void nf_prediction_record(NfPrediction *prediction, const NfInputCommand *command, const NfActor *actor);
uint8_t nf_prediction_build_redundant_bundle(const NfPrediction *prediction, uint32_t newest_sequence, NfInputBundle *bundle);
/* Compatibility wrapper: movement/combat-only authority, used by inherited callers. */
bool nf_prediction_reconcile(NfPrediction *prediction, NfWorld *world, NfEntityId entity_id, const NfActorNetState *authoritative, uint32_t acknowledged_input, uint64_t server_tick, float correction_threshold);
/* v1.6B owner reconciliation includes Focus/recoil/accepted-shot ancestry and full local combat state. */
bool nf_prediction_reconcile_v16b(
    NfPrediction *prediction,
    NfWorld *world,
    NfEntityId entity_id,
    const NfActorNetState *authoritative,
    const NfWeaponAuthorityNetState *authority,
    uint32_t acknowledged_input,
    uint64_t server_tick,
    float position_threshold,
    float authority_threshold);
#endif
