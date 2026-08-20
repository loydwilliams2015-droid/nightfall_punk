#ifndef NF_WEAPON_SOLUTION_H
#define NF_WEAPON_SOLUTION_H

#include "nf_combat.h"
#include "nf_world.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct NfWeaponSolutionRuntime {
    float focus_amount;
    float instability_deg;
    float redirect_stress_deg;
    float support_stress_deg;
} NfWeaponSolutionRuntime;

typedef struct NfWeaponEnvelopeInput {
    NfWeaponId weapon;
    float yaw_radians;
    float pitch_radians;
    NfVec3 velocity;
    NfMovementMode movement_mode;
    bool crouched;
    float manipulator_contamination;
    NfWeaponSolutionRuntime runtime;
} NfWeaponEnvelopeInput;

typedef struct NfWeaponEnvelope {
    NfWeaponId weapon;
    float aim_yaw_radians;
    float aim_pitch_radians;
    NfVec3 intended_direction;
    float base_cone_deg;
    float motion_cone_deg;
    float redirect_cone_deg;
    float airborne_cone_deg;
    float support_cone_deg;
    float contamination_cone_deg;
    float recoil_cone_deg;
    float state_cone_deg;
    float total_cone_deg;
    float focus_amount;
    float focus_state_reduction;
} NfWeaponEnvelope;

/* v1.6 compatibility input: retained so inherited code/tests keep one solver law. */
typedef struct NfWeaponSolutionInput {
    uint32_t world_seed;
    NfEntityId shooter;
    uint32_t shot_sequence;
    NfWeaponId weapon;
    float yaw_radians;
    float pitch_radians;
    NfVec3 velocity;
    NfMovementMode movement_mode;
    bool crouched;
    float manipulator_contamination;
    NfWeaponSolutionRuntime runtime;
} NfWeaponSolutionInput;

typedef struct NfWeaponSolution {
    NfWeaponId weapon;
    uint32_t shot_sequence;
    NfVec3 intended_direction;
    NfVec3 solved_direction;
    float base_cone_deg;
    float motion_cone_deg;
    float redirect_cone_deg;
    float airborne_cone_deg;
    float support_cone_deg;
    float contamination_cone_deg;
    float recoil_cone_deg;
    float state_cone_deg;
    float total_cone_deg;
    float focus_amount;
    float focus_state_reduction;
} NfWeaponSolution;

void nf_weapon_solution_runtime_init(NfWeaponSolutionRuntime *runtime);
float nf_weapon_focus_target(bool focus_held, bool sprinting);
void nf_weapon_solution_runtime_step(
    NfWeaponSolutionRuntime *runtime,
    NfWeaponId weapon,
    bool focus_held,
    bool sprinting,
    float dt);
/* v1.6B reload/handling seam: scale only Focus acquisition; release remains fast. */
void nf_weapon_solution_runtime_step_scaled(
    NfWeaponSolutionRuntime *runtime,
    NfWeaponId weapon,
    bool focus_held,
    bool sprinting,
    float acquisition_scale,
    float dt);
void nf_weapon_solution_record_shot(
    NfWeaponSolutionRuntime *runtime,
    NfWeaponId weapon);

/* Present-tense capability: pure, sequence-free, and side-effect-free. */
NfWeaponEnvelope nf_weapon_evaluate_envelope(NfWeaponEnvelopeInput input);

/* Historical event realization: accepted shot ancestry selects one bounded direction. */
NfWeaponSolution nf_weapon_realize_shot(
    const NfWeaponEnvelope *envelope,
    uint32_t world_seed,
    NfEntityId shooter,
    uint32_t shot_sequence);

/* v1.6 compatibility wrapper: evaluate envelope then realize accepted shot. */
NfWeaponSolution nf_weapon_solve(NfWeaponSolutionInput input);

/* v1.6B canonical actor seam. Call after movement integration and before fire. */
void nf_weapon_authority_step_actor(NfActor *actor, bool focus_held, float dt);
NfWeaponEnvelope nf_weapon_authority_envelope(
    const NfActor *actor,
    float yaw_radians,
    float pitch_radians);
bool nf_weapon_authority_try_fire(
    NfActor *actor,
    const NfCombatInput *input,
    uint32_t input_sequence,
    uint64_t server_tick,
    uint32_t world_seed,
    float yaw_radians,
    NfWeaponSolution *solution_out,
    NfCombatEvent *fire_event_out);

/* Legacy v1.6 projection retained for inherited tests. */
float nf_weapon_reticle_radius_px(const NfWeaponSolution *solution);

/* v1.6A/B live projection: FOV-aware, mildly nonlinear, monotonic, presentation-only. */
float nf_weapon_reticle_radius_px_for_envelope(
    const NfWeaponEnvelope *envelope,
    float vertical_fov_deg,
    float viewport_height_px);

#endif
