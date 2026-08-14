#ifndef NF_CONTAMINATION_H
#define NF_CONTAMINATION_H

#include "nf_world.h"

#define NF_CONTAM_DEMO_WATER_CENTER_X (-49.0f)
#define NF_CONTAM_DEMO_WATER_CENTER_Z (-10.0f)
#define NF_CONTAM_DEMO_WATER_HALF_X 5.5f
#define NF_CONTAM_DEMO_WATER_HALF_Z 3.5f

void nf_contamination_init_actor(NfActor *actor);
void nf_contamination_apply_damage(NfActor *actor, NfHitZone zone, float amount, uint64_t server_tick);
void nf_contamination_step_actor(NfActor *actor, float dt);
void nf_contamination_enter_stasis(NfActor *actor, uint64_t server_tick);
void nf_contamination_revive_actor(NfActor *actor, bool ecological_replacement);

float nf_contamination_health_projection(const NfActor *actor);
float nf_contamination_move_scale(const NfActor *actor);
float nf_contamination_manipulator_scale(const NfActor *actor);
float nf_contamination_sensory_scale(const NfActor *actor);
bool nf_contamination_is_critical(const NfActor *actor);
bool nf_contamination_requires_stasis(const NfActor *actor);

void nf_contamination_world_init(NfContaminationSystem *system);
void nf_contamination_world_step(NfWorld *world, float dt);
size_t nf_contamination_active_trace_count(const NfWorld *world);
const NfContaminationTrace *nf_contamination_trace_at(const NfWorld *world, size_t active_index);
bool nf_contamination_point_in_demo_water(NfVec3 position);

const char *nf_agency_state_name(NfAgencyState state);
const char *nf_inventory_fate_name(NfInventoryFate fate);

#endif
