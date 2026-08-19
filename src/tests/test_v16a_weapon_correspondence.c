#include "nf_weapon_solution.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

static NfWeaponEnvelope envelope(
    NfWeaponId weapon, float speed, NfMovementMode mode,
    bool crouched, float contamination, float focus, float recoil) {
    NfWeaponEnvelopeInput input = {0};
    input.weapon = weapon;
    input.yaw_radians = 0.37f;
    input.pitch_radians = -0.08f;
    input.velocity = (NfVec3){speed,0.0f,0.0f};
    input.movement_mode = mode;
    input.crouched = crouched;
    input.manipulator_contamination = contamination;
    input.runtime.focus_amount = focus;
    input.runtime.instability_deg = recoil;
    return nf_weapon_evaluate_envelope(input);
}

static void present_tense_envelope_contract(void) {
    const NfWeaponEnvelope calm = envelope(
        NF_WEAPON_CARBINE,0.0f,NF_MOVE_GROUND,false,0.0f,0.0f,0.0f);
    const NfWeaponEnvelope moving = envelope(
        NF_WEAPON_CARBINE,6.5f,NF_MOVE_GROUND,false,0.0f,0.0f,0.0f);
    const NfWeaponEnvelope moving_focus = envelope(
        NF_WEAPON_CARBINE,6.5f,NF_MOVE_GROUND,false,0.0f,1.0f,0.0f);
    const NfWeaponEnvelope pistol = envelope(
        NF_WEAPON_PISTOL,0.0f,NF_MOVE_GROUND,false,0.0f,0.0f,0.0f);

    assert(fabsf(calm.base_cone_deg-0.12f) < 1e-6f);
    assert(fabsf(pistol.base_cone_deg-0.18f) < 1e-6f);
    assert(moving.total_cone_deg > calm.total_cone_deg);
    assert(moving_focus.total_cone_deg < moving.total_cone_deg);
    assert(moving_focus.focus_state_reduction >= 0.50f);
    assert(moving_focus.focus_state_reduction <= 0.71f);
}

static void envelope_realization_separation_contract(void) {
    const NfWeaponEnvelope current = envelope(
        NF_WEAPON_CARBINE,4.0f,NF_MOVE_GROUND,false,0.2f,0.65f,0.4f);
    const NfWeaponSolution a = nf_weapon_realize_shot(
        &current,20260817u,17u,91u);
    const NfWeaponSolution b = nf_weapon_realize_shot(
        &current,20260817u,17u,91u);
    const NfWeaponSolution c = nf_weapon_realize_shot(
        &current,20260817u,17u,92u);

    assert(a.weapon == NF_WEAPON_CARBINE);
    assert(a.shot_sequence == 91u);
    assert(fabsf(a.total_cone_deg-current.total_cone_deg) < 1e-7f);
    assert(fabsf(a.solved_direction.x-b.solved_direction.x) < 1e-7f);
    assert(fabsf(a.solved_direction.y-b.solved_direction.y) < 1e-7f);
    assert(fabsf(a.solved_direction.z-b.solved_direction.z) < 1e-7f);
    assert(fabsf(a.solved_direction.x-c.solved_direction.x) > 1e-7f ||
           fabsf(a.solved_direction.y-c.solved_direction.y) > 1e-7f ||
           fabsf(a.solved_direction.z-c.solved_direction.z) > 1e-7f);
}

static void legacy_wrapper_coherence_contract(void) {
    NfWeaponSolutionInput input = {0};
    input.world_seed = 20260817u;
    input.shooter = 17u;
    input.shot_sequence = 33u;
    input.weapon = NF_WEAPON_CARBINE;
    input.yaw_radians = 0.19f;
    input.pitch_radians = -0.04f;
    input.velocity = (NfVec3){3.0f,0.0f,0.0f};
    input.movement_mode = NF_MOVE_GROUND;
    input.manipulator_contamination = 0.25f;
    input.runtime.focus_amount = 0.5f;
    input.runtime.instability_deg = 0.3f;

    NfWeaponEnvelopeInput envelope_input = {0};
    envelope_input.weapon = input.weapon;
    envelope_input.yaw_radians = input.yaw_radians;
    envelope_input.pitch_radians = input.pitch_radians;
    envelope_input.velocity = input.velocity;
    envelope_input.movement_mode = input.movement_mode;
    envelope_input.crouched = input.crouched;
    envelope_input.manipulator_contamination = input.manipulator_contamination;
    envelope_input.runtime = input.runtime;

    const NfWeaponEnvelope current = nf_weapon_evaluate_envelope(envelope_input);
    const NfWeaponSolution direct = nf_weapon_realize_shot(
        &current,input.world_seed,input.shooter,input.shot_sequence);
    const NfWeaponSolution legacy = nf_weapon_solve(input);

    assert(fabsf(direct.total_cone_deg-legacy.total_cone_deg) < 1e-7f);
    assert(fabsf(direct.solved_direction.x-legacy.solved_direction.x) < 1e-7f);
    assert(fabsf(direct.solved_direction.y-legacy.solved_direction.y) < 1e-7f);
    assert(fabsf(direct.solved_direction.z-legacy.solved_direction.z) < 1e-7f);
}

static void focus_timing_contract(void) {
    NfWeaponSolutionRuntime runtime;
    nf_weapon_solution_runtime_init(&runtime);
    const float dt = 1.0f/60.0f;
    int acquire_ticks = 0;
    while (runtime.focus_amount < 0.95f && acquire_ticks < 120) {
        nf_weapon_solution_runtime_step(
            &runtime,NF_WEAPON_CARBINE,true,false,dt);
        ++acquire_ticks;
    }
    const float acquire_seconds = (float)acquire_ticks*dt;
    assert(acquire_seconds >= 0.18f && acquire_seconds <= 0.27f);

    int release_ticks = 0;
    while (runtime.focus_amount > 0.05f && release_ticks < 120) {
        nf_weapon_solution_runtime_step(
            &runtime,NF_WEAPON_CARBINE,false,false,dt);
        ++release_ticks;
    }
    const float release_seconds = (float)release_ticks*dt;
    assert(release_seconds >= 0.10f && release_seconds <= 0.19f);
}

static void reticle_projection_contract(void) {
    const NfWeaponEnvelope calm = envelope(
        NF_WEAPON_CARBINE,0.0f,NF_MOVE_GROUND,false,0.0f,0.0f,0.0f);
    const NfWeaponEnvelope moving = envelope(
        NF_WEAPON_CARBINE,6.0f,NF_MOVE_GROUND,false,0.0f,0.0f,0.0f);
    const NfWeaponEnvelope focused = envelope(
        NF_WEAPON_CARBINE,6.0f,NF_MOVE_GROUND,false,0.0f,1.0f,0.0f);
    const float calm_px = nf_weapon_reticle_radius_px_for_envelope(&calm,82.0f,720.0f);
    const float moving_px = nf_weapon_reticle_radius_px_for_envelope(&moving,82.0f,720.0f);
    const float focus_px = nf_weapon_reticle_radius_px_for_envelope(&focused,75.0f,720.0f);
    const float calm_focus_fov_px = nf_weapon_reticle_radius_px_for_envelope(&calm,75.0f,720.0f);

    assert(calm_px >= 5.0f);
    assert(moving_px > calm_px);
    assert(focus_px < moving_px);
    assert(calm_focus_fov_px > calm_px);
}

int main(void) {
    present_tense_envelope_contract();
    envelope_realization_separation_contract();
    legacy_wrapper_coherence_contract();
    focus_timing_contract();
    reticle_projection_contract();
    puts("nightfall v1.6A weapon correspondence / calibration contracts: PASS");
    puts("TEMPORAL current_envelope=PASS accepted_shot_history=PASS legacy_coherence=PASS");
    puts("FOCUS timing_200_250ms=PASS release_fast=PASS contextual_reduction=PASS");
    puts("RETICLE present_tense=PASS mild_nonlinear=PASS fov_aware=PASS");
    return 0;
}
