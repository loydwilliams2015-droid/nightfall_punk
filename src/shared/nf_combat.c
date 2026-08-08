#include "nf_combat.h"

#include <string.h>

static const NfWeaponSpec k_weapons[NF_WEAPON_COUNT] = {
    {NF_WEAPON_NONE, "NONE", false, 0, 1, 0, 0, 0, 0, 0, 0},
    {NF_WEAPON_CARBINE, "CARBINE", true, 18.0f, 1.50f, 10.0f, 30, 120, 1.80f, 0.28f, 90.0f},
    {NF_WEAPON_PISTOL, "PISTOL", false, 30.0f, 1.50f, 4.0f, 12, 48, 1.35f, 0.20f, 70.0f}
};

const NfWeaponSpec *nf_weapon_spec(NfWeaponId weapon) {
    if (weapon <= NF_WEAPON_NONE || weapon >= NF_WEAPON_COUNT) return &k_weapons[NF_WEAPON_NONE];
    return &k_weapons[weapon];
}

void nf_combat_init_actor(NfActor *actor) {
    if (actor == NULL) return;
    memset(&actor->combat, 0, sizeof(actor->combat));
    actor->health = 100.0f;
    actor->combat.alive = true;
    actor->combat.weapon = NF_WEAPON_CARBINE;
    actor->combat.state = NF_WEAPON_READY;
    for (int i = 1; i < NF_WEAPON_COUNT; ++i) {
        const NfWeaponSpec *spec = nf_weapon_spec((NfWeaponId)i);
        actor->combat.ammo_mag[i] = spec->magazine_size;
        actor->combat.reserve_ammo[i] = spec->starting_reserve;
    }
}

static void finish_reload(NfActor *actor) {
    const NfWeaponSpec *spec = nf_weapon_spec(actor->combat.weapon);
    uint16_t *mag = &actor->combat.ammo_mag[actor->combat.weapon];
    uint16_t *reserve = &actor->combat.reserve_ammo[actor->combat.weapon];
    const uint16_t need = *mag < spec->magazine_size ? (uint16_t)(spec->magazine_size - *mag) : 0u;
    const uint16_t take = *reserve < need ? *reserve : need;
    *mag = (uint16_t)(*mag + take);
    *reserve = (uint16_t)(*reserve - take);
    actor->combat.reload_committed = true;
}

void nf_combat_step_actor(NfActor *actor, float dt) {
    if (actor == NULL || dt <= 0.0f) return;
    if (!actor->combat.alive) {
        if (actor->combat.respawn_timer > 0.0f) {
            actor->combat.respawn_timer -= dt;
            if (actor->combat.respawn_timer < 0.0f) actor->combat.respawn_timer = 0.0f;
        }
        return;
    }
    if (actor->combat.action_timer > 0.0f) {
        actor->combat.action_timer -= dt;
        if (actor->combat.state == NF_WEAPON_RELOADING && !actor->combat.reload_committed && actor->combat.reload_total > 0.0f && actor->combat.action_timer <= actor->combat.reload_total * 0.30f) {
            finish_reload(actor);
        }
        if (actor->combat.action_timer <= 0.0f) {
            actor->combat.action_timer = 0.0f;
            if (actor->combat.state == NF_WEAPON_SWITCHING && actor->combat.pending_weapon > NF_WEAPON_NONE) {
                actor->combat.weapon = actor->combat.pending_weapon;
                actor->combat.pending_weapon = NF_WEAPON_NONE;
            }
            const uint16_t mag = actor->combat.ammo_mag[actor->combat.weapon];
            actor->combat.state = mag > 0u ? NF_WEAPON_READY : NF_WEAPON_EMPTY;
            actor->combat.reload_total = 0.0f;
            actor->combat.reload_committed = false;
        }
    }
}

bool nf_combat_can_damage(NfFaction attacker, NfFaction target, bool friendly_fire) {
    if (attacker == NF_FACTION_NONE || target == NF_FACTION_NONE) return false;
    if (friendly_fire) return true;
    return attacker != target;
}

void nf_combat_cancel_reload(NfActor *actor) {
    if (actor == NULL || actor->combat.state != NF_WEAPON_RELOADING) return;
    actor->combat.action_timer = 0.0f;
    actor->combat.reload_total = 0.0f;
    actor->combat.state = actor->combat.ammo_mag[actor->combat.weapon] > 0u ? NF_WEAPON_READY : NF_WEAPON_EMPTY;
    actor->combat.reload_committed = false;
}

bool nf_combat_select_weapon(NfActor *actor, NfWeaponId weapon) {
    if (actor == NULL || !actor->combat.alive || weapon <= NF_WEAPON_NONE || weapon >= NF_WEAPON_COUNT || weapon == actor->combat.weapon) return false;
    if (actor->combat.state == NF_WEAPON_RELOADING) nf_combat_cancel_reload(actor);
    actor->combat.pending_weapon = weapon;
    actor->combat.state = NF_WEAPON_SWITCHING;
    actor->combat.action_timer = nf_weapon_spec(weapon)->switch_seconds;
    return true;
}

bool nf_combat_start_reload(NfActor *actor) {
    if (actor == NULL || !actor->combat.alive || actor->combat.state == NF_WEAPON_SWITCHING) return false;
    const NfWeaponSpec *spec = nf_weapon_spec(actor->combat.weapon);
    if (actor->combat.ammo_mag[actor->combat.weapon] >= spec->magazine_size || actor->combat.reserve_ammo[actor->combat.weapon] == 0u) return false;
    actor->combat.state = NF_WEAPON_RELOADING;
    actor->combat.action_timer = spec->reload_seconds;
    actor->combat.reload_total = spec->reload_seconds;
    actor->combat.reload_committed = false;
    return true;
}

bool nf_combat_try_fire(NfActor *actor, const NfCombatInput *input, uint32_t command_sequence, uint64_t server_tick, NfCombatEvent *event_out) {
    if (actor == NULL || input == NULL || !actor->combat.alive) return false;
    const NfWeaponSpec *spec = nf_weapon_spec(actor->combat.weapon);
    const bool wants_fire = spec->automatic ? (input->fire_held || input->fire_pressed) : input->fire_pressed;
    if (!wants_fire || command_sequence == 0u || command_sequence == actor->combat.last_fire_sequence) return false;
    if (actor->combat.state == NF_WEAPON_RELOADING) nf_combat_cancel_reload(actor);
    if (actor->combat.state != NF_WEAPON_READY) return false;
    uint16_t *mag = &actor->combat.ammo_mag[actor->combat.weapon];
    if (*mag == 0u) {
        actor->combat.state = NF_WEAPON_EMPTY;
        return false;
    }
    --(*mag);
    actor->combat.last_fire_sequence = command_sequence;
    actor->combat.last_fire_tick = server_tick;
    actor->combat.state = NF_WEAPON_RECOVERING;
    actor->combat.action_timer = 1.0f / spec->rounds_per_second;
    if (event_out != NULL) {
        *event_out = (NfCombatEvent){.server_tick=server_tick,.type=NF_COMBAT_EVENT_GUNFIRE,.source=actor->id,.weapon=actor->combat.weapon,.position=actor->transform.position};
    }
    return true;
}

bool nf_combat_apply_damage(NfActor *target, NfEntityId source, NfWeaponId weapon, NfHitZone zone, float amount, uint64_t server_tick, NfCombatEvent *event_out) {
    if (target == NULL || !target->combat.alive || amount <= 0.0f) return false;
    target->health -= amount;
    if (target->health <= 0.0f) {
        target->health = 0.0f;
        target->combat.alive = false;
        target->combat.respawn_timer = 3.0f;
        target->combat.action_timer = 0.0f;
        target->transform.velocity = (NfVec3){0};
    }
    if (event_out != NULL) {
        *event_out = (NfCombatEvent){
            .server_tick=server_tick,
            .type=target->combat.alive?NF_COMBAT_EVENT_DAMAGE:NF_COMBAT_EVENT_DEATH,
            .source=source,
            .target=target->id,
            .weapon=weapon,
            .hit_zone=zone,
            .amount=amount,
            .position=target->transform.position
        };
    }
    return true;
}

void nf_combat_respawn(NfActor *actor, NfVec3 position, uint64_t server_tick, NfCombatEvent *event_out) {
    if (actor == NULL) return;
    const NfFaction faction = actor->faction;
    const NfEntityId id = actor->id;
    actor->transform.position = position;
    actor->transform.velocity = (NfVec3){0};
    nf_combat_init_actor(actor);
    actor->id = id;
    actor->faction = faction;
    if (event_out != NULL) {
        *event_out = (NfCombatEvent){.server_tick=server_tick,.type=NF_COMBAT_EVENT_RESPAWN,.source=id,.target=id,.position=position};
    }
}

const char *nf_weapon_name(NfWeaponId weapon) { return nf_weapon_spec(weapon)->name; }
const char *nf_weapon_state_name(NfWeaponState state) {
    switch (state) {
        case NF_WEAPON_READY: return "READY";
        case NF_WEAPON_RECOVERING: return "RECOVERING";
        case NF_WEAPON_RELOADING: return "RELOADING";
        case NF_WEAPON_SWITCHING: return "SWITCHING";
        case NF_WEAPON_EMPTY: return "EMPTY";
        default: return "UNKNOWN";
    }
}
const char *nf_combat_event_name(NfCombatEventType type) {
    switch (type) {
        case NF_COMBAT_EVENT_GUNFIRE: return "GUNFIRE";
        case NF_COMBAT_EVENT_DAMAGE: return "DAMAGE";
        case NF_COMBAT_EVENT_DEATH: return "DEATH";
        case NF_COMBAT_EVENT_RESPAWN: return "RESPAWN";
        case NF_COMBAT_EVENT_RELOAD: return "RELOAD";
        case NF_COMBAT_EVENT_WEAPON_SWITCH: return "WEAPON_SWITCH";
        default: return "NONE";
    }
}
