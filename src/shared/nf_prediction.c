#include "nf_prediction.h"

#include <math.h>
#include <string.h>

static float state_error(const NfActorNetState *a, const NfActorNetState *b) {
    const float x = a->position.x - b->position.x;
    const float y = a->position.y - b->position.y;
    const float z = a->position.z - b->position.z;
    return sqrtf(x*x + y*y + z*z);
}

void nf_prediction_init(NfPrediction *p) {
    if (p == NULL) return;
    memset(p, 0, sizeof(*p));
    p->next_sequence = 1u;
}

NfInputCommand nf_prediction_make_command(
    NfPrediction *p, uint64_t tick, NfMoveInput move, NfCombatInput combat) {
    NfInputCommand command = {0};
    if (p == NULL) return command;
    command.sequence = p->next_sequence++;
    command.client_tick = tick;
    command.move = move;
    command.combat = combat;
    return command;
}

void nf_prediction_record(
    NfPrediction *p, const NfInputCommand *command, const NfActor *actor) {
    if (p == NULL || command == NULL || actor == NULL) return;
    NfPredictionSlot *slot = &p->history[command->sequence % NF_NET_HISTORY];
    slot->valid = true;
    slot->command = *command;
    nf_actor_to_net_state(actor, &slot->predicted);
}

uint8_t nf_prediction_build_redundant_bundle(
    const NfPrediction *p, uint32_t newest, NfInputBundle *bundle) {
    if (p == NULL || bundle == NULL || newest == 0u) return 0u;
    bundle->count = 0u;
    const uint32_t first = newest >= NF_NET_INPUT_REDUNDANCY - 1u
        ? newest - (NF_NET_INPUT_REDUNDANCY - 1u)
        : 1u;
    for (uint32_t seq = first; seq <= newest; ++seq) {
        const NfPredictionSlot *slot = &p->history[seq % NF_NET_HISTORY];
        if (slot->valid && slot->command.sequence == seq) {
            bundle->commands[bundle->count++] = slot->command;
        }
    }
    return bundle->count;
}

bool nf_prediction_reconcile(
    NfPrediction *p,
    NfWorld *world,
    NfEntityId id,
    const NfActorNetState *authoritative,
    uint32_t acknowledged,
    uint64_t server_tick,
    float threshold) {
    if (p == NULL || world == NULL || authoritative == NULL) return false;

    NfActor *actor = nf_world_find_actor(world, id);
    if (actor == NULL) return false;

    NfPredictionSlot *ack_slot = &p->history[acknowledged % NF_NET_HISTORY];
    const bool have_prediction = ack_slot->valid &&
        ack_slot->command.sequence == acknowledged;
    const float error = have_prediction
        ? state_error(&ack_slot->predicted, authoritative)
        : 0.0f;

    p->last_error = error;
    if (error > p->max_error) p->max_error = error;
    p->last_acknowledged = acknowledged;

    /* Combat remains server truth even when movement needs no replay. */
    nf_actor_apply_combat_net_state(actor, authoritative);

    const bool positional_correction = have_prediction && error > threshold;
    const bool structural_resync = have_prediction &&
        (ack_slot->predicted.mode != authoritative->mode ||
         ack_slot->predicted.attached_collider != authoritative->attached_collider);
    const bool needs_resync = !have_prediction || positional_correction || structural_resync;
    if (!needs_resync) return false;

    const uint32_t newest = p->next_sequence > 0u ? p->next_sequence - 1u : 0u;
    nf_actor_apply_net_state(actor, authoritative, &world->movement);
    world->tick = server_tick;
    nf_world_sync_dynamic_geometry(world);

    for (uint32_t seq = acknowledged + 1u; seq <= newest; ++seq) {
        NfPredictionSlot *slot = &p->history[seq % NF_NET_HISTORY];
        if (!slot->valid || slot->command.sequence != seq) continue;
        nf_world_set_input(world, id, slot->command.move);
        nf_world_step(world, 1.0f / (float)NF_TICK_RATE);
        nf_actor_to_net_state(actor, &slot->predicted);
    }

    ++p->resyncs;
    if (positional_correction) ++p->corrections;
    return true;
}
