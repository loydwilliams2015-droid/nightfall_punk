#include "nf_contamination.h"

#include <math.h>
#include <string.h>

static float nf_clampf(float value, float minimum, float maximum) {
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
}

static float nf_trace_total(const NfContaminationTrace *trace) {
    if (trace == NULL || !trace->active) return 0.0f;
    return trace->surface + trace->dissolved + trace->airborne;
}

void nf_contamination_init_actor(NfActor *actor) {
    if (actor == NULL) return;
    actor->agency = NF_AGENCY_ACTIVE;
    memset(&actor->contamination, 0, sizeof(actor->contamination));
    memset(&actor->stasis_inventory, 0, sizeof(actor->stasis_inventory));
    actor->health = 100.0f;
}

void nf_contamination_apply_damage(NfActor *actor, NfHitZone zone, float amount, uint64_t server_tick) {
    if (actor == NULL || amount <= 0.0f || actor->agency == NF_AGENCY_STASIS) return;

    const float dose = amount / 100.0f;
    NfBodyContamination *c = &actor->contamination;
    c->systemic = nf_clampf(c->systemic + dose, 0.0f, 1.25f);
    c->surface = nf_clampf(c->surface + dose * 0.65f, 0.0f, 1.50f);
    c->last_change_tick = server_tick;
    c->phenotype_mask |= NF_CONTAM_PHENOTYPE_SURFACE;

    switch (zone) {
        case NF_HIT_HEAD:
            c->sensory = nf_clampf(c->sensory + dose * 1.35f, 0.0f, 1.0f);
            c->manipulator = nf_clampf(c->manipulator + dose * 0.15f, 0.0f, 1.0f);
            break;
        case NF_HIT_KNEE:
            c->locomotor = nf_clampf(c->locomotor + dose * 1.60f, 0.0f, 1.0f);
            break;
        case NF_HIT_FOOT:
            c->locomotor = nf_clampf(c->locomotor + dose * 1.40f, 0.0f, 1.0f);
            break;
        case NF_HIT_BODY:
        default:
            c->locomotor = nf_clampf(c->locomotor + dose * 0.35f, 0.0f, 1.0f);
            c->manipulator = nf_clampf(c->manipulator + dose * 0.55f, 0.0f, 1.0f);
            c->sensory = nf_clampf(c->sensory + dose * 0.15f, 0.0f, 1.0f);
            break;
    }

    if (actor->faction == NF_FACTION_RANCHER) {
        c->airborne = nf_clampf(c->airborne + dose * 0.18f, 0.0f, 1.0f);
        c->phenotype_mask |= NF_CONTAM_PHENOTYPE_AIRBORNE | NF_CONTAM_PHENOTYPE_FILAMENTOUS;
    } else {
        c->phenotype_mask |= NF_CONTAM_PHENOTYPE_SIGNAL;
    }

    actor->health = nf_contamination_health_projection(actor);
    actor->agency = nf_contamination_is_critical(actor) ? NF_AGENCY_CRITICAL : NF_AGENCY_ACTIVE;
}

void nf_contamination_step_actor(NfActor *actor, float dt) {
    if (actor == NULL || dt <= 0.0f || actor->agency == NF_AGENCY_STASIS) return;

    NfBodyContamination *c = &actor->contamination;
    if (actor->agency == NF_AGENCY_REVIVING || actor->agency == NF_AGENCY_RETURNING) {
        c->agency_timer -= dt;
        if (c->agency_timer <= 0.0f) {
            c->agency_timer = 0.0f;
            actor->agency = nf_contamination_is_critical(actor) ? NF_AGENCY_CRITICAL : NF_AGENCY_ACTIVE;
        }
    }

    /* Slow bounded natural recovery. Deliberate treatment remains a future owner. */
    const float systemic_recovery = 0.0015f * dt;
    const float domain_recovery = 0.0025f * dt;
    const float surface_recovery = 0.0010f * dt;
    c->systemic = nf_clampf(c->systemic - systemic_recovery, 0.0f, 1.25f);
    c->locomotor = nf_clampf(c->locomotor - domain_recovery, 0.0f, 1.0f);
    c->manipulator = nf_clampf(c->manipulator - domain_recovery, 0.0f, 1.0f);
    c->sensory = nf_clampf(c->sensory - domain_recovery, 0.0f, 1.0f);
    c->surface = nf_clampf(c->surface - surface_recovery, 0.0f, 1.5f);
    c->airborne = nf_clampf(c->airborne - 0.0040f * dt, 0.0f, 1.0f);
    c->dissolved = nf_clampf(c->dissolved - 0.0010f * dt, 0.0f, 1.0f);

    actor->health = nf_contamination_health_projection(actor);
    if (actor->agency == NF_AGENCY_ACTIVE || actor->agency == NF_AGENCY_CRITICAL) {
        actor->agency = nf_contamination_is_critical(actor) ? NF_AGENCY_CRITICAL : NF_AGENCY_ACTIVE;
    }
}

void nf_contamination_enter_stasis(NfActor *actor, uint64_t server_tick) {
    if (actor == NULL) return;
    actor->agency = NF_AGENCY_STASIS;
    actor->combat.alive = false;
    actor->contamination.stasis_tick = server_tick;
    actor->contamination.last_change_tick = server_tick;
    actor->contamination.trace_pending = true;
    actor->contamination.surface = nf_clampf(actor->contamination.surface + 0.25f, 0.0f, 1.50f);
    actor->transform.velocity = (NfVec3){0};
    actor->health = 0.0f;
}

void nf_contamination_revive_actor(NfActor *actor, bool ecological_replacement) {
    if (actor == NULL) return;

    if (ecological_replacement) {
        nf_contamination_init_actor(actor);
        actor->agency = NF_AGENCY_RETURNING;
        actor->contamination.agency_timer = 0.35f;
        actor->combat.alive = true;
        return;
    }

    memset(&actor->contamination, 0, sizeof(actor->contamination));
    actor->contamination.systemic = 0.35f;
    actor->contamination.locomotor = 0.28f;
    actor->contamination.manipulator = 0.24f;
    actor->contamination.sensory = 0.22f;
    actor->contamination.surface = 0.18f;
    actor->contamination.phenotype_mask = NF_CONTAM_PHENOTYPE_SURFACE | NF_CONTAM_PHENOTYPE_SIGNAL;
    actor->contamination.agency_timer = 0.75f;
    actor->agency = NF_AGENCY_RETURNING;
    actor->combat.alive = true;
    actor->health = nf_contamination_health_projection(actor);
}

float nf_contamination_health_projection(const NfActor *actor) {
    if (actor == NULL) return 0.0f;
    return 100.0f * (1.0f - nf_clampf(actor->contamination.systemic, 0.0f, 1.0f));
}

float nf_contamination_move_scale(const NfActor *actor) {
    if (actor == NULL) return 1.0f;
    return nf_clampf(1.0f - 0.35f * nf_clampf(actor->contamination.locomotor, 0.0f, 1.0f), 0.65f, 1.0f);
}

float nf_contamination_manipulator_scale(const NfActor *actor) {
    if (actor == NULL) return 1.0f;
    return nf_clampf(1.0f - 0.30f * nf_clampf(actor->contamination.manipulator, 0.0f, 1.0f), 0.70f, 1.0f);
}

float nf_contamination_sensory_scale(const NfActor *actor) {
    if (actor == NULL) return 1.0f;
    return nf_clampf(1.0f - 0.35f * nf_clampf(actor->contamination.sensory, 0.0f, 1.0f), 0.65f, 1.0f);
}

bool nf_contamination_is_critical(const NfActor *actor) {
    if (actor == NULL) return false;
    const NfBodyContamination *c = &actor->contamination;
    return c->systemic >= 0.70f || c->locomotor >= 0.85f || c->manipulator >= 0.85f || c->sensory >= 0.85f;
}

bool nf_contamination_requires_stasis(const NfActor *actor) {
    return actor != NULL && actor->contamination.systemic >= 1.0f;
}

void nf_contamination_world_init(NfContaminationSystem *system) {
    if (system == NULL) return;
    memset(system, 0, sizeof(*system));
    system->next_trace_id = 1u;
}

bool nf_contamination_point_in_demo_water(NfVec3 position) {
    return fabsf(position.x - NF_CONTAM_DEMO_WATER_CENTER_X) <= NF_CONTAM_DEMO_WATER_HALF_X &&
           fabsf(position.z - NF_CONTAM_DEMO_WATER_CENTER_Z) <= NF_CONTAM_DEMO_WATER_HALF_Z;
}

static NfContaminationTrace *nf_trace_slot(NfContaminationSystem *system) {
    if (system == NULL) return NULL;
    for (size_t i = 0u; i < NF_MAX_CONTAMINATION_TRACES; ++i) {
        if (!system->traces[i].active) return &system->traces[i];
    }

    size_t weakest = 0u;
    float weakest_total = nf_trace_total(&system->traces[0]);
    for (size_t i = 1u; i < NF_MAX_CONTAMINATION_TRACES; ++i) {
        const float total = nf_trace_total(&system->traces[i]);
        if (total < weakest_total || (total == weakest_total && system->traces[i].id < system->traces[weakest].id)) {
            weakest = i;
            weakest_total = total;
        }
    }
    return &system->traces[weakest];
}

static void nf_emit_pending_trace(NfWorld *world, NfActor *actor) {
    if (world == NULL || actor == NULL || !actor->contamination.trace_pending) return;
    NfContaminationTrace *trace = nf_trace_slot(&world->contamination);
    if (trace == NULL) return;

    memset(trace, 0, sizeof(*trace));
    trace->active = true;
    trace->id = world->contamination.next_trace_id++;
    trace->source = actor->id;
    trace->source_faction = actor->faction;
    trace->position = actor->transform.position;
    trace->surface = nf_clampf(actor->contamination.surface, 0.35f, 1.50f);
    trace->dissolved = actor->contamination.dissolved;
    trace->airborne = actor->contamination.airborne;
    trace->phenotype_mask = actor->contamination.phenotype_mask | NF_CONTAM_PHENOTYPE_SURFACE;
    trace->created_tick = world->tick;
    trace->last_change_tick = world->tick;
    trace->site_weapon = actor->stasis_inventory.site_weapon;
    trace->absorbed_ammo_units = actor->stasis_inventory.absorbed_ammo_units;
    world->contamination.absorbed_inventory_sink += (float)trace->absorbed_ammo_units;
    actor->contamination.trace_pending = false;
}

static void nf_step_trace(NfContaminationSystem *system, NfContaminationTrace *trace, float dt, uint64_t tick) {
    if (system == NULL || trace == NULL || !trace->active || dt <= 0.0f) return;

    if (nf_contamination_point_in_demo_water(trace->position)) {
        const float transfer = fminf(trace->surface, 0.030f * dt);
        trace->surface -= transfer;
        trace->dissolved += transfer;
        if (transfer > 0.0f) trace->phenotype_mask |= NF_CONTAM_PHENOTYPE_DISSOLVED;

        const float drained = fminf(trace->dissolved, 0.006f * dt);
        trace->dissolved -= drained;
        system->drained_sink += drained;
    } else {
        const float surface_decay = fminf(trace->surface, 0.0015f * dt);
        trace->surface -= surface_decay;
        system->decayed_sink += surface_decay;
    }

    const float airborne_decay = fminf(trace->airborne, 0.010f * dt);
    trace->airborne -= airborne_decay;
    system->decayed_sink += airborne_decay;
    trace->last_change_tick = tick;

    const float age_seconds = (float)(tick - trace->created_tick) / (float)NF_TICK_RATE;
    if (nf_trace_total(trace) <= 0.01f && age_seconds >= 20.0f && trace->site_weapon == NF_WEAPON_NONE) {
        memset(trace, 0, sizeof(*trace));
    }
}

void nf_contamination_world_step(NfWorld *world, float dt) {
    if (world == NULL || dt <= 0.0f) return;

    for (size_t i = 0u; i < NF_MAX_ENTITIES; ++i) {
        NfActor *actor = &world->actors[i];
        if (actor->active && actor->contamination.trace_pending) nf_emit_pending_trace(world, actor);
    }

    for (size_t i = 0u; i < NF_MAX_CONTAMINATION_TRACES; ++i) {
        nf_step_trace(&world->contamination, &world->contamination.traces[i], dt, world->tick);
    }
}

size_t nf_contamination_active_trace_count(const NfWorld *world) {
    if (world == NULL) return 0u;
    size_t count = 0u;
    for (size_t i = 0u; i < NF_MAX_CONTAMINATION_TRACES; ++i) {
        if (world->contamination.traces[i].active) ++count;
    }
    return count;
}

const NfContaminationTrace *nf_contamination_trace_at(const NfWorld *world, size_t active_index) {
    if (world == NULL) return NULL;
    size_t seen = 0u;
    for (size_t i = 0u; i < NF_MAX_CONTAMINATION_TRACES; ++i) {
        if (!world->contamination.traces[i].active) continue;
        if (seen == active_index) return &world->contamination.traces[i];
        ++seen;
    }
    return NULL;
}

const char *nf_agency_state_name(NfAgencyState state) {
    switch (state) {
        case NF_AGENCY_ACTIVE: return "ACTIVE";
        case NF_AGENCY_CRITICAL: return "CRITICAL";
        case NF_AGENCY_STASIS: return "STASIS";
        case NF_AGENCY_REVIVING: return "REVIVING";
        case NF_AGENCY_RETURNING: return "RETURNING";
        default: return "UNKNOWN";
    }
}

const char *nf_inventory_fate_name(NfInventoryFate fate) {
    switch (fate) {
        case NF_INVENTORY_FATE_BASE_RESTOCK: return "BASE_RESTOCK";
        case NF_INVENTORY_FATE_SITE_PERSIST: return "SITE_PERSIST";
        case NF_INVENTORY_FATE_ECO_ABSORB: return "ECO_ABSORB";
        case NF_INVENTORY_FATE_NONE:
        default: return "NONE";
    }
}
