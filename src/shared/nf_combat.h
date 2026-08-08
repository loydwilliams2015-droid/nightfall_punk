#ifndef NF_COMBAT_H
#define NF_COMBAT_H

#include "nf_world.h"

typedef struct NfWeaponSpec {
    NfWeaponId id;
    const char *name;
    bool automatic;
    float damage;
    float head_multiplier;
    float rounds_per_second;
    uint16_t magazine_size;
    uint16_t starting_reserve;
    float reload_seconds;
    float switch_seconds;
    float range;
} NfWeaponSpec;

const NfWeaponSpec *nf_weapon_spec(NfWeaponId weapon);
void nf_combat_init_actor(NfActor *actor);
void nf_combat_step_actor(NfActor *actor, float dt);
bool nf_combat_can_damage(NfFaction attacker, NfFaction target, bool friendly_fire);
bool nf_combat_select_weapon(NfActor *actor, NfWeaponId weapon);
bool nf_combat_start_reload(NfActor *actor);
void nf_combat_cancel_reload(NfActor *actor);
bool nf_combat_try_fire(NfActor *actor, const NfCombatInput *input, uint32_t command_sequence, uint64_t server_tick, NfCombatEvent *event_out);
bool nf_combat_apply_damage(NfActor *target, NfEntityId source, NfWeaponId weapon, NfHitZone zone, float amount, uint64_t server_tick, NfCombatEvent *event_out);
void nf_combat_respawn(NfActor *actor, NfVec3 position, uint64_t server_tick, NfCombatEvent *event_out);
const char *nf_weapon_name(NfWeaponId weapon);
const char *nf_weapon_state_name(NfWeaponState state);
const char *nf_combat_event_name(NfCombatEventType type);

#endif
