#include "nf_combat.h"
#include "nf_world.h"

#include <assert.h>
#include <stdio.h>

static void weapon_state_contract(void){
    NfWorld w;nf_world_init(&w,44);NfEntityId id=nf_world_spawn_actor(&w,NF_FACTION_PLAYER,(NfVec3){0,0,0});NfActor *a=nf_world_find_actor(&w,id);assert(a&&a->combat.alive&&a->health==100.0f);assert(a->combat.weapon==NF_WEAPON_CARBINE);assert(a->combat.ammo_mag[NF_WEAPON_CARBINE]==30);assert(a->combat.reserve_ammo[NF_WEAPON_CARBINE]==120);
    NfCombatInput fire={.fire_held=true,.fire_pressed=true};NfCombatEvent e={0};assert(nf_combat_try_fire(a,&fire,1,1,&e));assert(e.type==NF_COMBAT_EVENT_GUNFIRE&&a->combat.ammo_mag[NF_WEAPON_CARBINE]==29);assert(!nf_combat_try_fire(a,&fire,2,1,&e));nf_combat_step_actor(a,0.11f);assert(a->combat.state==NF_WEAPON_READY);assert(nf_combat_try_fire(a,&fire,2,2,&e));
    nf_combat_step_actor(a,0.11f);assert(nf_combat_start_reload(a));nf_combat_step_actor(a,1.30f);assert(a->combat.reload_committed);assert(a->combat.ammo_mag[NF_WEAPON_CARBINE]==30);uint16_t reserve=a->combat.reserve_ammo[NF_WEAPON_CARBINE];nf_combat_cancel_reload(a);assert(a->combat.ammo_mag[NF_WEAPON_CARBINE]==30&&a->combat.reserve_ammo[NF_WEAPON_CARBINE]==reserve);
    assert(nf_combat_select_weapon(a,NF_WEAPON_PISTOL));assert(a->combat.state==NF_WEAPON_SWITCHING);nf_combat_step_actor(a,0.25f);assert(a->combat.weapon==NF_WEAPON_PISTOL&&a->combat.state==NF_WEAPON_READY);
}

static void damage_death_respawn_contract(void){
    NfWorld w;nf_world_init(&w,45);NfActor *attacker=nf_world_find_actor(&w,nf_world_spawn_actor(&w,NF_FACTION_PLAYER,(NfVec3){0,0,0}));NfActor *target=nf_world_find_actor(&w,nf_world_spawn_actor(&w,NF_FACTION_RIVAL,(NfVec3){0,0,4}));assert(attacker&&target);assert(nf_combat_can_damage(attacker->faction,target->faction,false));assert(!nf_combat_can_damage(attacker->faction,NF_FACTION_PLAYER,false));
    NfCombatEvent e={0};assert(nf_combat_apply_damage(target,attacker->id,NF_WEAPON_CARBINE,NF_HIT_BODY,18.0f,10,&e));assert(e.type==NF_COMBAT_EVENT_DAMAGE&&target->health==82.0f);assert(nf_combat_apply_damage(target,attacker->id,NF_WEAPON_CARBINE,NF_HIT_HEAD,82.0f,11,&e));assert(e.type==NF_COMBAT_EVENT_DEATH&&!target->combat.alive&&target->health==0.0f&&target->combat.respawn_timer==3.0f);nf_combat_step_actor(target,3.1f);assert(target->combat.respawn_timer==0.0f);nf_combat_respawn(target,(NfVec3){2,0,2},12,&e);assert(target->combat.alive&&target->health==100.0f&&target->transform.position.x==2.0f&&e.type==NF_COMBAT_EVENT_RESPAWN);
}

static void semi_auto_contract(void){
    NfWorld w;nf_world_init(&w,46);NfActor *a=nf_world_find_actor(&w,nf_world_spawn_actor(&w,NF_FACTION_PLAYER,(NfVec3){0}));assert(nf_combat_select_weapon(a,NF_WEAPON_PISTOL));nf_combat_step_actor(a,0.25f);NfCombatInput held={.fire_held=true};NfCombatEvent e={0};assert(!nf_combat_try_fire(a,&held,1,1,&e));held.fire_pressed=true;assert(nf_combat_try_fire(a,&held,1,1,&e));assert(a->combat.ammo_mag[NF_WEAPON_PISTOL]==11);
}

int main(void){weapon_state_contract();damage_death_respawn_contract();semi_auto_contract();puts("nightfall v0.4 combat contract tests: PASS");return 0;}
