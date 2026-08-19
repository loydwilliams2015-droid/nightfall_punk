#ifndef NF_WEAPON_SOLUTION_H
#define NF_WEAPON_SOLUTION_H

#include "nf_combat.h"
#include "nf_world.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct NfWeaponSolutionRuntime {
    float focus_amount;
    float instability_deg;
} NfWeaponSolutionRuntime;

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
    NfVec3 intended_direction;
    NfVec3 solved_direction;
    float base_cone_deg;
    float motion_cone_deg;
    float airborne_cone_deg;
    float contamination_cone_deg;
    float recoil_cone_deg;
    float state_cone_deg;
    float total_cone_deg;
    float focus_amount;
    float focus_state_reduction;
} NfWeaponSolution;

void nf_weapon_solution_runtime_init(NfWeaponSolutionRuntime *runtime);
void nf_weapon_solution_runtime_step(
    NfWeaponSolutionRuntime *runtime,
    NfWeaponId weapon,
    bool focus_held,
    bool sprinting,
    float dt);
void nf_weapon_solution_record_shot(
    NfWeaponSolutionRuntime *runtime,
    NfWeaponId weapon);
NfWeaponSolution nf_weapon_solve(NfWeaponSolutionInput input);
float nf_weapon_reticle_radius_px(const NfWeaponSolution *solution);

#endif
