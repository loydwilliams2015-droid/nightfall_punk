#include "nf_weapon_solution.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

static float length3(NfVec3 v) {
    return sqrtf(v.x*v.x+v.y*v.y+v.z*v.z);
}

static NfWeaponSolution solve(
    uint32_t seq, float speed, NfMovementMode mode,
    bool crouched, float contamination, float focus, float recoil) {
    NfWeaponSolutionInput input = {0};
    input.world_seed = 20260816u;
    input.shooter = 17u;
    input.shot_sequence = seq;
    input.weapon = NF_WEAPON_CARBINE;
    input.yaw_radians = 0.37f;
    input.pitch_radians = -0.08f;
    input.velocity = (NfVec3){speed,0.0f,0.0f};
    input.movement_mode = mode;
    input.crouched = crouched;
    input.manipulator_contamination = contamination;
    input.runtime.focus_amount = focus;
    input.runtime.instability_deg = recoil;
    return nf_weapon_solve(input);
}

static void deterministic_solution_contract(void) {
    const NfWeaponSolution a = solve(11u,0.0f,NF_MOVE_GROUND,false,0.0f,0.0f,0.0f);
    const NfWeaponSolution b = solve(11u,0.0f,NF_MOVE_GROUND,false,0.0f,0.0f,0.0f);
    const NfWeaponSolution c = solve(12u,0.0f,NF_MOVE_GROUND,false,0.0f,0.0f,0.0f);
    assert(a.total_cone_deg > 0.09f && a.total_cone_deg < 0.20f);
    assert(fabsf(a.solved_direction.x-b.solved_direction.x) < 1e-7f);
    assert(fabsf(a.solved_direction.y-b.solved_direction.y) < 1e-7f);
    assert(fabsf(a.solved_direction.z-b.solved_direction.z) < 1e-7f);
    assert(fabsf(a.solved_direction.x-c.solved_direction.x) > 1e-7f ||
           fabsf(a.solved_direction.y-c.solved_direction.y) > 1e-7f);
    assert(fabsf(length3(a.solved_direction)-1.0f) < 1e-5f);
}

static void state_produced_dispersion_contract(void) {
    const NfWeaponSolution calm = solve(21u,0.0f,NF_MOVE_GROUND,false,0.0f,0.0f,0.0f);
    const NfWeaponSolution moving = solve(21u,7.2f,NF_MOVE_GROUND,false,0.0f,0.0f,0.0f);
    const NfWeaponSolution crouched = solve(21u,4.0f,NF_MOVE_GROUND,true,0.0f,0.0f,0.0f);
    const NfWeaponSolution standing = solve(21u,4.0f,NF_MOVE_GROUND,false,0.0f,0.0f,0.0f);
    const NfWeaponSolution air = solve(21u,0.0f,NF_MOVE_AIR,false,0.0f,0.0f,0.0f);
    const NfWeaponSolution contaminated = solve(21u,0.0f,NF_MOVE_GROUND,false,0.75f,0.0f,0.0f);
    assert(moving.total_cone_deg > calm.total_cone_deg);
    assert(crouched.total_cone_deg < standing.total_cone_deg);
    assert(air.total_cone_deg > calm.total_cone_deg);
    assert(contaminated.total_cone_deg > calm.total_cone_deg);
}

static void context_sensitive_focus_contract(void) {
    const NfWeaponSolution calm_hip = solve(31u,0.0f,NF_MOVE_GROUND,false,0.0f,0.0f,0.0f);
    const NfWeaponSolution calm_focus = solve(31u,0.0f,NF_MOVE_GROUND,false,0.0f,1.0f,0.0f);
    const NfWeaponSolution unstable_hip = solve(31u,6.5f,NF_MOVE_GROUND,false,0.0f,0.0f,0.9f);
    const NfWeaponSolution unstable_focus = solve(31u,6.5f,NF_MOVE_GROUND,false,0.0f,1.0f,0.9f);
    const float calm_gain = calm_hip.total_cone_deg-calm_focus.total_cone_deg;
    const float unstable_gain = unstable_hip.total_cone_deg-unstable_focus.total_cone_deg;
    assert(calm_gain > 0.0f);
    assert(unstable_gain > calm_gain*10.0f);
    assert(unstable_focus.total_cone_deg < unstable_hip.total_cone_deg);
}

static void focus_recovery_contract(void) {
    NfWeaponSolutionRuntime hip;
    NfWeaponSolutionRuntime focus;
    nf_weapon_solution_runtime_init(&hip);
    nf_weapon_solution_runtime_init(&focus);
    nf_weapon_solution_record_shot(&hip,NF_WEAPON_CARBINE);
    nf_weapon_solution_record_shot(&focus,NF_WEAPON_CARBINE);
    assert(fabsf(hip.instability_deg-focus.instability_deg) < 1e-6f);
    for (int i = 0; i < 8; ++i) {
        nf_weapon_solution_runtime_step(&hip,NF_WEAPON_CARBINE,false,false,1.0f/60.0f);
        nf_weapon_solution_runtime_step(&focus,NF_WEAPON_CARBINE,true,false,1.0f/60.0f);
    }
    assert(focus.focus_amount > 0.60f);
    assert(focus.instability_deg < hip.instability_deg);

    NfWeaponSolutionRuntime sprint;
    nf_weapon_solution_runtime_init(&sprint);
    for (int i = 0; i < 30; ++i) {
        nf_weapon_solution_runtime_step(&sprint,NF_WEAPON_CARBINE,true,true,1.0f/60.0f);
    }
    assert(sprint.focus_amount <= 0.36f);
}

static void reticle_truth_contract(void) {
    const NfWeaponSolution calm = solve(41u,0.0f,NF_MOVE_GROUND,false,0.0f,0.0f,0.0f);
    const NfWeaponSolution unstable = solve(41u,6.0f,NF_MOVE_AIR,false,0.4f,0.0f,0.8f);
    const NfWeaponSolution focused = solve(41u,6.0f,NF_MOVE_AIR,false,0.4f,1.0f,0.8f);
    assert(nf_weapon_reticle_radius_px(&unstable) > nf_weapon_reticle_radius_px(&calm));
    assert(nf_weapon_reticle_radius_px(&focused) < nf_weapon_reticle_radius_px(&unstable));
}

int main(void) {
    deterministic_solution_contract();
    state_produced_dispersion_contract();
    context_sensitive_focus_contract();
    focus_recovery_contract();
    reticle_truth_contract();
    puts("nightfall v1.6 weapon solution / Focus contracts: PASS");
    puts("BASELINE physical_cone=PASS state_dispersion=PASS deterministic=PASS");
    puts("FOCUS context_sensitive=PASS recovery_advantage=PASS sprint_transition=PASS");
    puts("RETICLE truthful_projection=PASS");
    return 0;
}
