#include "nf_weapon_solution.h"

#include <math.h>
#include <stddef.h>

#define NF_PI 3.14159265358979323846f

typedef struct NfWeaponSolutionTuning {
    float base_cone_deg;
    float move_cone_deg;
    float air_cone_deg;
    float contamination_cone_deg;
    float recoil_impulse_deg;
    float recoil_cap_deg;
    float recovery_deg_per_second;
    float focus_recovery_multiplier;
} NfWeaponSolutionTuning;

static float clamp01(float value) {
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static float maxf(float a, float b) { return a > b ? a : b; }

static NfWeaponSolutionTuning tuning_for(NfWeaponId weapon) {
    switch (weapon) {
        case NF_WEAPON_CARBINE:
            return (NfWeaponSolutionTuning){
                .base_cone_deg=0.12f,
                .move_cone_deg=0.58f,
                .air_cone_deg=0.92f,
                .contamination_cone_deg=0.70f,
                .recoil_impulse_deg=0.34f,
                .recoil_cap_deg=1.45f,
                .recovery_deg_per_second=2.35f,
                .focus_recovery_multiplier=1.10f
            };
        case NF_WEAPON_PISTOL:
            return (NfWeaponSolutionTuning){
                .base_cone_deg=0.18f,
                .move_cone_deg=0.44f,
                .air_cone_deg=0.78f,
                .contamination_cone_deg=0.64f,
                .recoil_impulse_deg=0.46f,
                .recoil_cap_deg=1.65f,
                .recovery_deg_per_second=2.80f,
                .focus_recovery_multiplier=0.90f
            };
        default:
            return (NfWeaponSolutionTuning){0};
    }
}

static uint32_t mix32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

static float unit_from_hash(uint32_t x) {
    return (float)(mix32(x) & 0x00ffffffu) / 16777215.0f;
}

static NfVec3 direction_from_angles(float yaw, float pitch) {
    const float cp = cosf(pitch);
    return (NfVec3){sinf(yaw)*cp,sinf(pitch),cosf(yaw)*cp};
}

static NfVec3 normalize3(NfVec3 v) {
    const float n = sqrtf(v.x*v.x+v.y*v.y+v.z*v.z);
    if (n <= 0.000001f) return (NfVec3){0.0f,0.0f,1.0f};
    return (NfVec3){v.x/n,v.y/n,v.z/n};
}

void nf_weapon_solution_runtime_init(NfWeaponSolutionRuntime *runtime) {
    if (runtime == NULL) return;
    runtime->focus_amount = 0.0f;
    runtime->instability_deg = 0.0f;
}

void nf_weapon_solution_runtime_step(
    NfWeaponSolutionRuntime *runtime,
    NfWeaponId weapon,
    bool focus_held,
    bool sprinting,
    float dt) {
    if (runtime == NULL || dt <= 0.0f) return;
    const NfWeaponSolutionTuning tuning = tuning_for(weapon);
    float target = focus_held ? 1.0f : 0.0f;
    if (sprinting && target > 0.35f) target = 0.35f;
    const float focus_rate = focus_held ? 7.5f : 10.0f;
    const float focus_step = clamp01(dt*focus_rate);
    runtime->focus_amount += (target-runtime->focus_amount)*focus_step;
    runtime->focus_amount = clamp01(runtime->focus_amount);

    const float recovery = tuning.recovery_deg_per_second *
        (1.0f+runtime->focus_amount*tuning.focus_recovery_multiplier);
    runtime->instability_deg -= recovery*dt;
    if (runtime->instability_deg < 0.0f) runtime->instability_deg = 0.0f;
}

void nf_weapon_solution_record_shot(
    NfWeaponSolutionRuntime *runtime,
    NfWeaponId weapon) {
    if (runtime == NULL) return;
    const NfWeaponSolutionTuning tuning = tuning_for(weapon);
    runtime->instability_deg += tuning.recoil_impulse_deg;
    if (runtime->instability_deg > tuning.recoil_cap_deg) {
        runtime->instability_deg = tuning.recoil_cap_deg;
    }
}

NfWeaponSolution nf_weapon_solve(NfWeaponSolutionInput input) {
    const NfWeaponSolutionTuning tuning = tuning_for(input.weapon);
    NfWeaponSolution result = {0};
    result.intended_direction = direction_from_angles(input.yaw_radians,input.pitch_radians);
    result.base_cone_deg = tuning.base_cone_deg;
    result.focus_amount = clamp01(input.runtime.focus_amount);

    const float horizontal_speed = sqrtf(
        input.velocity.x*input.velocity.x+input.velocity.z*input.velocity.z);
    const float speed_fraction = clamp01(horizontal_speed/7.2f);
    result.motion_cone_deg = tuning.move_cone_deg*speed_fraction;
    if (input.crouched) result.motion_cone_deg *= 0.78f;

    if (input.movement_mode == NF_MOVE_AIR ||
        input.movement_mode == NF_MOVE_VAULT ||
        input.movement_mode == NF_MOVE_MANTLE) {
        result.airborne_cone_deg = tuning.air_cone_deg;
    }
    result.contamination_cone_deg = tuning.contamination_cone_deg *
        clamp01(input.manipulator_contamination);
    result.recoil_cone_deg = maxf(0.0f,input.runtime.instability_deg);
    result.state_cone_deg = result.motion_cone_deg+
        result.airborne_cone_deg+result.contamination_cone_deg+
        result.recoil_cone_deg;

    const float state_reference = maxf(0.01f,
        tuning.move_cone_deg+tuning.air_cone_deg+tuning.recoil_cap_deg);
    const float state_pressure = clamp01(result.state_cone_deg/state_reference);
    result.focus_state_reduction = result.focus_amount*(0.48f+0.30f*state_pressure);
    const float focused_base = result.base_cone_deg*(1.0f-result.focus_amount*0.10f);
    const float focused_state = result.state_cone_deg*(1.0f-result.focus_state_reduction);
    result.total_cone_deg = focused_base+focused_state;
    if (result.total_cone_deg < 0.0f) result.total_cone_deg = 0.0f;

    const uint32_t ancestry = input.world_seed ^
        mix32(input.shooter*0x9e3779b9u) ^
        mix32(input.shot_sequence*0x85ebca6bu) ^
        mix32((uint32_t)input.weapon*0xc2b2ae35u);
    const float u = unit_from_hash(ancestry^0x68bc21ebu);
    const float v = unit_from_hash(ancestry^0x02e5be93u);
    const float radius = sqrtf(u);
    const float angle = v*2.0f*NF_PI;
    const float cone_rad = result.total_cone_deg*(NF_PI/180.0f);
    const float yaw_offset = cosf(angle)*radius*cone_rad;
    const float pitch_offset = sinf(angle)*radius*cone_rad;
    result.solved_direction = normalize3(direction_from_angles(
        input.yaw_radians+yaw_offset,
        input.pitch_radians+pitch_offset));
    return result;
}

float nf_weapon_reticle_radius_px(const NfWeaponSolution *solution) {
    if (solution == NULL) return 6.0f;
    float radius = 5.0f+solution->total_cone_deg*18.0f;
    if (radius < 5.0f) radius = 5.0f;
    if (radius > 42.0f) radius = 42.0f;
    return radius;
}
