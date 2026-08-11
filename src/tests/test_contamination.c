#include "nf_combat.h"
#include "nf_contamination.h"
#include "nf_world.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

static bool nearf(float a, float b, float epsilon) {
    return fabsf(a - b) <= epsilon;
}

static void localized_damage_and_stasis_contract(void) {
    NfWorld w;
    nf_world_init(&w, 1101u);
    NfActor *attacker = nf_world_find_actor(&w, nf_world_spawn_actor(&w, NF_FACTION_PLAYER, (NfVec3){0}));
    NfActor *target = nf_world_find_actor(&w, nf_world_spawn_actor(&w, NF_FACTION_RIVAL,
        (NfVec3){NF_CONTAM_DEMO_WATER_CENTER_X, 0.05f, NF_CONTAM_DEMO_WATER_CENTER_Z}));
    assert(attacker != NULL && target != NULL);
    assert(target->agency == NF_AGENCY_ACTIVE);
    assert(nearf(target->contamination.systemic, 0.0f, 0.0001f));

    NfCombatEvent event = {0};
    assert(nf_combat_apply_damage(target, attacker->id, NF_WEAPON_CARBINE, NF_HIT_KNEE, 25.0f, 10u, &event));
    assert(event.type == NF_COMBAT_EVENT_DAMAGE);
    assert(nearf(target->contamination.systemic, 0.25f, 0.001f));
    assert(target->contamination.locomotor > 0.39f);
    assert(nf_contamination_move_scale(target) < 0.90f);

    assert(nf_combat_apply_damage(target, attacker->id, NF_WEAPON_CARBINE, NF_HIT_BODY, 30.0f, 11u, &event));
    assert(target->contamination.manipulator > 0.16f);
    assert(nf_contamination_manipulator_scale(target) < 0.96f);

    assert(nf_combat_apply_damage(target, attacker->id, NF_WEAPON_PISTOL, NF_HIT_BODY, 45.0f, 12u, &event));
    assert(event.type == NF_COMBAT_EVENT_STASIS);
    assert(target->agency == NF_AGENCY_STASIS);
    assert(!target->combat.alive);
    assert(target->health == 0.0f);
    assert(target->stasis_inventory.valid);
    assert(target->stasis_inventory.site_fate == NF_INVENTORY_FATE_SITE_PERSIST);
    assert(target->stasis_inventory.restock_fate == NF_INVENTORY_FATE_BASE_RESTOCK);
    assert(target->stasis_inventory.ammo_fate == NF_INVENTORY_FATE_ECO_ABSORB);
    assert(target->stasis_inventory.site_weapon == NF_WEAPON_CARBINE);
    assert(target->stasis_inventory.restock_weapon == NF_WEAPON_PISTOL);
    assert(target->stasis_inventory.absorbed_ammo_units > 0u);

    nf_world_step(&w, 1.0f / (float)NF_TICK_RATE);
    assert(nf_contamination_active_trace_count(&w) == 1u);
    const NfContaminationTrace *trace = nf_contamination_trace_at(&w, 0u);
    assert(trace != NULL && trace->active);
    assert(trace->source == target->id);
    assert(trace->source_faction == NF_FACTION_RIVAL);
    assert(trace->surface > 0.0f);
    assert(trace->site_weapon == NF_WEAPON_CARBINE);
    assert(trace->absorbed_ammo_units > 0u);
    assert(w.contamination.absorbed_inventory_sink > 0.0f);

    const uint32_t trace_id = trace->id;
    const float initial_surface = trace->surface;
    for (int i = 0; i < 600; ++i) nf_world_step(&w, 1.0f / (float)NF_TICK_RATE);
    trace = nf_contamination_trace_at(&w, 0u);
    assert(trace != NULL && trace->id == trace_id);
    assert(trace->surface < initial_surface);
    assert(trace->dissolved > 0.0f);
    assert(w.contamination.drained_sink > 0.0f);

    nf_combat_respawn(target, (NfVec3){-40.0f, 0.05f, -4.0f}, w.tick, &event);
    assert(event.type == NF_COMBAT_EVENT_REVIVAL);
    assert(target->combat.alive);
    assert(target->agency == NF_AGENCY_RETURNING);
    assert(target->health > 60.0f && target->health < 70.0f);
    assert(target->combat.weapon == NF_WEAPON_PISTOL);
    assert(target->combat.ammo_mag[NF_WEAPON_CARBINE] == 0u);
    assert(target->combat.reserve_ammo[NF_WEAPON_CARBINE] == 0u);
    assert(target->combat.ammo_mag[NF_WEAPON_PISTOL] > 0u);
    assert(nf_contamination_active_trace_count(&w) == 1u);
    trace = nf_contamination_trace_at(&w, 0u);
    assert(trace != NULL && trace->id == trace_id);

    printf("[contamination] stasis actor=%u agency=%s health_projection=%.1f trace=%u surface=%.3f dissolved=%.3f drained=%.3f inventory=%s/%s/%s\n",
        target->id, nf_agency_state_name(target->agency), target->health, trace->id,
        trace->surface, trace->dissolved, w.contamination.drained_sink,
        nf_inventory_fate_name(target->stasis_inventory.site_fate),
        nf_inventory_fate_name(target->stasis_inventory.restock_fate),
        nf_inventory_fate_name(target->stasis_inventory.ammo_fate));
}

static void contamination_is_not_infestation_contract(void) {
    NfWorld w;
    nf_world_init(&w, 1102u);
    NfActor *cattler = nf_world_find_actor(&w, nf_world_spawn_actor(&w, NF_FACTION_RANCHER, (NfVec3){2, 0.05f, 2}));
    assert(cattler != NULL);
    nf_contamination_apply_damage(cattler, NF_HIT_BODY, 30.0f, 20u);
    assert(cattler->contamination.systemic > 0.0f);
    assert((cattler->contamination.phenotype_mask & NF_CONTAM_PHENOTYPE_FILAMENTOUS) != 0u);
    assert((cattler->contamination.phenotype_mask & NF_CONTAM_PHENOTYPE_AIRBORNE) != 0u);
    /* No infestation state is created here: contamination is universal body damage;
       Cattler infestation remains owned by the separate habitat/ecology system. */
    assert(nf_contamination_active_trace_count(&w) == 0u);
    printf("[contamination] cattler universal-damage systemic=%.2f phenotype=0x%x infestation=SEPARATE\n",
        cattler->contamination.systemic, cattler->contamination.phenotype_mask);
}

int main(void) {
    localized_damage_and_stasis_contract();
    contamination_is_not_infestation_contract();
    puts("nightfall v1.1 contamination / stasis / transport tests: PASS");
    return 0;
}
