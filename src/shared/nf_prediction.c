#include "nf_prediction.h"

#include "nf_combat.h"
#include "nf_weapon_solution.h"

#include <math.h>
#include <string.h>

static float state_error(const NfActorNetState *a, const NfActorNetState *b) {
    const float x = a->position.x - b->position.x;
    const float y = a->position.y - b->position.y;
    const float z = a->position.z - b->position.z;
    return sqrtf(x*x + y*y + z*z);
}

static float authority_error(
    const NfWeaponAuthorityNetState *a,
    const NfWeaponAuthorityNetState *b) {
    if (a == NULL || b == NULL) return 0.0f;

    float error = fabsf(a->focus_amount - b->focus_amount);
    for (int i = 1; i < NF_WEAPON_COUNT; ++i) {
        const float delta = fabsf(a->instability_deg[i] - b->instability_deg[i]);
        if (delta > error) error = delta;
    }

    const float redirect_delta = fabsf(
        a->redirect_stress_deg - b->redirect_stress_deg);
    const float support_delta = fabsf(
        a->support_stress_deg - b->support_stress_deg);
    if (redirect_delta > error) error = redirect_delta;
    if (support_delta > error) error = support_delta;

    if (a->accepted_shot_sequence != b->accepted_shot_sequence) error += 1.0f;
    if (a->weapon != b->weapon || a->weapon_state != b->weapon_state) {
        error += 1.0f;
    }
    return error;
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

void nf_prediction_simulate_command(
    NfWorld *world, NfEntityId id, const NfInputCommand *command) {
    if (world == NULL || command == NULL) return;
    NfActor *actor = nf_world_find_actor(world, id);
    if (actor == NULL) return;

    /* v1.6B Q76: the shot samples the newly integrated mobility state. */
    nf_world_set_input(world, id, command->move);
    nf_world_step(world, 1.0f/(float)NF_TICK_RATE);
    actor = nf_world_find_actor(world, id);
    if (actor == NULL || !actor->combat.alive) return;

    if ((command->move.sprint_held || command->move.interact_held) &&
        actor->combat.state == NF_WEAPON_RELOADING) {
        nf_combat_cancel_reload(actor);
    }
    if (command->combat.weapon_slot > 0u &&
        command->combat.weapon_slot < NF_WEAPON_COUNT) {
        (void)nf_combat_select_weapon(
            actor, (NfWeaponId)command->combat.weapon_slot);
    }
    if (command->combat.reload_pressed) {
        (void)nf_combat_start_reload(actor);
    }

    nf_weapon_authority_step_actor(
        actor, command->combat.focus_held, 1.0f/(float)NF_TICK_RATE);
    (void)nf_weapon_authority_try_fire(
        actor,
        &command->combat,
        command->sequence,
        world->tick,
        world->seed,
        command->move.yaw_radians,
        NULL,
        NULL);
}

void nf_prediction_record(
    NfPrediction *p, const NfInputCommand *command, const NfActor *actor) {
    if (p == NULL || command == NULL || actor == NULL) return;
    NfPredictionSlot *slot = &p->history[command->sequence % NF_NET_HISTORY];
    slot->valid = true;
    slot->command = *command;
    nf_actor_to_net_state(actor, &slot->predicted);
    nf_weapon_authority_to_net_state(actor, &slot->predicted_authority);
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

bool nf_prediction_reconcile_v16b(
    NfPrediction *p,
    NfWorld *world,
    NfEntityId id,
    const NfActorNetState *authoritative,
    const NfWeaponAuthorityNetState *authority,
    uint32_t acknowledged,
    uint64_t server_tick,
    float position_threshold,
    float authority_threshold) {
    if (p == NULL || world == NULL || authoritative == NULL) return false;

    NfActor *actor = nf_world_find_actor(world, id);
    if (actor == NULL) return false;

    NfPredictionSlot *ack_slot = &p->history[acknowledged % NF_NET_HISTORY];
    const bool have_prediction = ack_slot->valid &&
        ack_slot->command.sequence == acknowledged;
    const float error = have_prediction
        ? state_error(&ack_slot->predicted, authoritative)
        : 0.0f;
    const float auth_error = have_prediction && authority != NULL
        ? authority_error(&ack_slot->predicted_authority, authority)
        : 0.0f;

    p->last_error = error;
    if (error > p->max_error) p->max_error = error;
    p->last_authority_error = auth_error;
    if (auth_error > p->max_authority_error) {
        p->max_authority_error = auth_error;
    }
    p->last_acknowledged = acknowledged;

    /* Combat remains server truth even when movement needs no replay. */
    nf_actor_apply_combat_net_state(actor, authoritative);

    const bool positional_correction = have_prediction &&
        error > position_threshold;
    const bool structural_resync = have_prediction &&
        (ack_slot->predicted.mode != authoritative->mode ||
         ack_slot->predicted.attached_collider != authoritative->attached_collider);
    const bool authority_correction = have_prediction && authority != NULL &&
        auth_error > authority_threshold;
    const bool needs_resync = !have_prediction || positional_correction ||
        structural_resync || authority_correction;
    if (!needs_resync) return false;

    const uint32_t newest = p->next_sequence > 0u
        ? p->next_sequence - 1u
        : 0u;
    nf_actor_apply_net_state(actor, authoritative, &world->movement);
    if (authority != NULL) {
        nf_weapon_authority_apply_net_state(actor, authority);
    }
    world->tick = server_tick;
    nf_world_sync_dynamic_geometry(world);

    for (uint32_t seq = acknowledged + 1u; seq <= newest; ++seq) {
        NfPredictionSlot *slot = &p->history[seq % NF_NET_HISTORY];
        if (!slot->valid || slot->command.sequence != seq) continue;
        nf_prediction_simulate_command(world, id, &slot->command);
        actor = nf_world_find_actor(world, id);
        if (actor == NULL) break;
        nf_actor_to_net_state(actor, &slot->predicted);
        nf_weapon_authority_to_net_state(actor, &slot->predicted_authority);
    }

    ++p->resyncs;
    if (positional_correction) ++p->corrections;
    if (authority_correction) ++p->authority_corrections;
    return true;
}

bool nf_prediction_reconcile(
    NfPrediction *p,
    NfWorld *world,
    NfEntityId id,
    const NfActorNetState *authoritative,
    uint32_t acknowledged,
    uint64_t server_tick,
    float threshold) {
    return nf_prediction_reconcile_v16b(
        p,
        world,
        id,
        authoritative,
        NULL,
        acknowledged,
        server_tick,
        threshold,
        0.08f);
}
