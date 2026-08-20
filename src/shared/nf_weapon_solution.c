#include "nf_weapon_solution.h"

#include <math.h>
#include <stddef.h>

#define NF_PI 3.14159265358979323846f
#define NF_V16B_RELOAD_FOCUS_ACQUIRE_SCALE 0.55f

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

static float clampf(float value, float minimum, float maximum) {
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
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
    *runtime = (NfWeaponSolutionRuntime){0};
}

float nf_weapon_focus_target(bool focus_held, bool sprinting) {
    if (!focus_held) return 0.0f;
    return sprinting ? 0.35f : 1.0f;
}

void nf_weapon_solution_runtime_step_scaled(
    NfWeaponSolutionRuntime *runtime,
    NfWeaponId weapon,
    bool focus_held,
    bool sprinting,
    float acquisition_scale,
    float dt) {
    if (runtime == NULL || dt <= 0.0f) return;
    const NfWeaponSolutionTuning tuning = tuning_for(weapon);
    const float target = nf_weapon_focus_target(focus_held,sprinting);
    const bool acquiring = target > runtime->focus_amount;
    const float scale = clampf(acquisition_scale,0.0f,1.0f);
    const float focus_rate = acquiring ? 12.5f*scale : 18.0f;
    const float focus_step = clamp01(dt*focus_rate);
    runtime->focus_amount += (target-runtime->focus_amount)*focus_step;
    runtime->focus_amount = clamp01(runtime->focus_amount);

    const float recovery = tuning.recovery_deg_per_second *
        (1.0f+runtime->focus_amount*tuning.focus_recovery_multiplier);
    runtime->instability_deg -= recovery*dt;
    if (runtime->instability_deg < 0.0f) runtime->instability_deg = 0.0f;
}

void nf_weapon_solution_runtime_step(
    NfWeaponSolutionRuntime *runtime,
    NfWeaponId weapon,
    bool focus_held,
    bool sprinting,
    float dt) {
    nf_weapon_solution_runtime_step_scaled(
        runtime,weapon,focus_held,sprinting,1.0f,dt);
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

NfWeaponEnvelope nf_weapon_evaluate_envelope(NfWeaponEnvelopeInput input) {
    const NfWeaponSolutionTuning tuning = tuning_for(input.weapon);
    NfWeaponEnvelope result = {0};
    result.weapon = input.weapon;
    result.aim_yaw_radians = input.yaw_radians;
    result.aim_pitch_radians = input.pitch_radians;
    result.intended_direction = direction_from_angles(input.yaw_radians,input.pitch_radians);
    result.base_cone_deg = tuning.base_cone_deg;
    result.focus_amount = clamp01(input.runtime.focus_amount);

    const float horizontal_speed = sqrtf(
        input.velocity.x*input.velocity.x+input.velocity.z*input.velocity.z);
    const float speed_fraction = clamp01(horizontal_speed/7.2f);
    result.motion_cone_deg = tuning.move_cone_deg*speed_fraction;
    if (input.crouched) result.motion_cone_deg *= 0.78f;

    result.redirect_cone_deg = maxf(0.0f,input.runtime.redirect_stress_deg);
    if (input.movement_mode == NF_MOVE_AIR ||
        input.movement_mode == NF_MOVE_VAULT ||
        input.movement_mode == NF_MOVE_MANTLE) {
        result.airborne_cone_deg = tuning.air_cone_deg;
    }
    result.support_cone_deg = maxf(0.0f,input.runtime.support_stress_deg);
    result.contamination_cone_deg = tuning.contamination_cone_deg *
        clamp01(input.manipulator_contamination);
    result.recoil_cone_deg = maxf(0.0f,input.runtime.instability_deg);
    result.state_cone_deg = result.motion_cone_deg+
        result.redirect_cone_deg+result.airborne_cone_deg+
        result.support_cone_deg+result.contamination_cone_deg+
        result.recoil_cone_deg;

    /* v1.6B: Focus is not a universal percentage buff. It can organize recoil and
       active redirection more than it can abolish lost support or contamination. */
    const float f = result.focus_amount;
    const float focused_motion = result.motion_cone_deg*(1.0f-f*0.56f);
    const float focused_redirect = result.redirect_cone_deg*(1.0f-f*0.58f);
    const float focused_air = result.airborne_cone_deg*(1.0f-f*0.35f);
    const float focused_support = result.support_cone_deg*(1.0f-f*0.35f);
    const float focused_contamination = result.contamination_cone_deg*(1.0f-f*0.20f);
    const float focused_recoil = result.recoil_cone_deg*(1.0f-f*0.66f);
    const float focused_state = focused_motion+focused_redirect+focused_air+
        focused_support+focused_contamination+focused_recoil;
    result.focus_state_reduction = result.state_cone_deg > 0.000001f
        ? clamp01(1.0f-focused_state/result.state_cone_deg)
        : 0.0f;
    const float focused_base = result.base_cone_deg*(1.0f-f*0.10f);
    result.total_cone_deg = focused_base+focused_state;
    if (result.total_cone_deg < 0.0f) result.total_cone_deg = 0.0f;
    return result;
}

NfWeaponSolution nf_weapon_realize_shot(
    const NfWeaponEnvelope *envelope,
    uint32_t world_seed,
    NfEntityId shooter,
    uint32_t shot_sequence) {
    NfWeaponSolution result = {0};
    if (envelope == NULL) return result;

    result.weapon = envelope->weapon;
    result.shot_sequence = shot_sequence;
    result.intended_direction = envelope->intended_direction;
    result.base_cone_deg = envelope->base_cone_deg;
    result.motion_cone_deg = envelope->motion_cone_deg;
    result.redirect_cone_deg = envelope->redirect_cone_deg;
    result.airborne_cone_deg = envelope->airborne_cone_deg;
    result.support_cone_deg = envelope->support_cone_deg;
    result.contamination_cone_deg = envelope->contamination_cone_deg;
    result.recoil_cone_deg = envelope->recoil_cone_deg;
    result.state_cone_deg = envelope->state_cone_deg;
    result.total_cone_deg = envelope->total_cone_deg;
    result.focus_amount = envelope->focus_amount;
    result.focus_state_reduction = envelope->focus_state_reduction;

    const uint32_t ancestry = world_seed ^
        mix32(shooter*0x9e3779b9u) ^
        mix32(shot_sequence*0x85ebca6bu) ^
        mix32((uint32_t)envelope->weapon*0xc2b2ae35u);
    const float u = unit_from_hash(ancestry^0x68bc21ebu);
    const float v = unit_from_hash(ancestry^0x02e5be93u);
    const float radius = sqrtf(u);
    const float angle = v*2.0f*NF_PI;
    const float cone_rad = envelope->total_cone_deg*(NF_PI/180.0f);
    const float yaw_offset = cosf(angle)*radius*cone_rad;
    const float pitch_offset = sinf(angle)*radius*cone_rad;
    result.solved_direction = normalize3(direction_from_angles(
        envelope->aim_yaw_radians+yaw_offset,
        envelope->aim_pitch_radians+pitch_offset));
    return result;
}

NfWeaponSolution nf_weapon_solve(NfWeaponSolutionInput input) {
    NfWeaponEnvelopeInput envelope_input = {0};
    envelope_input.weapon = input.weapon;
    envelope_input.yaw_radians = input.yaw_radians;
    envelope_input.pitch_radians = input.pitch_radians;
    envelope_input.velocity = input.velocity;
    envelope_input.movement_mode = input.movement_mode;
    envelope_input.crouched = input.crouched;
    envelope_input.manipulator_contamination = input.manipulator_contamination;
    envelope_input.runtime = input.runtime;
    const NfWeaponEnvelope envelope = nf_weapon_evaluate_envelope(envelope_input);
    return nf_weapon_realize_shot(
        &envelope,input.world_seed,input.shooter,input.shot_sequence);
}

static bool support_sensitive_mode(NfMovementMode mode) {
    return mode==NF_MOVE_AIR || mode==NF_MOVE_VAULT || mode==NF_MOVE_MANTLE ||
        mode==NF_MOVE_LADDER || mode==NF_MOVE_PLATFORM;
}

void nf_weapon_authority_step_actor(NfActor *actor, bool focus_held, float dt) {
    if (actor == NULL || dt <= 0.0f ||
        actor->combat.weapon<=NF_WEAPON_NONE || actor->combat.weapon>=NF_WEAPON_COUNT) return;

    NfWeaponAuthorityState *authority=&actor->weapon_authority;
    if (authority->redirect_stress_deg > 0.0f) {
        authority->redirect_stress_deg -= 1.80f*dt;
        if (authority->redirect_stress_deg < 0.0f) authority->redirect_stress_deg=0.0f;
    }
    if (authority->support_stress_deg > 0.0f) {
        authority->support_stress_deg -= 1.55f*dt;
        if (authority->support_stress_deg < 0.0f) authority->support_stress_deg=0.0f;
    }

    if (authority->kinematic_initialized) {
        const float pvx=authority->previous_velocity.x;
        const float pvz=authority->previous_velocity.z;
        const float cvx=actor->transform.velocity.x;
        const float cvz=actor->transform.velocity.z;
        const float ps=sqrtf(pvx*pvx+pvz*pvz);
        const float cs=sqrtf(cvx*cvx+cvz*cvz);
        float turn01=0.0f;
        if (ps>0.20f && cs>0.20f) {
            const float dot=clampf((pvx*cvx+pvz*cvz)/(ps*cs),-1.0f,1.0f);
            turn01=clamp01((1.0f-dot)*0.5f);
        }
        const float speed_change01=clamp01(fabsf(cs-ps)/3.6f);
        const float speed_weight=clamp01(maxf(ps,cs)/7.2f);
        const float redirect_target=0.30f*turn01*speed_weight+0.08f*speed_change01;
        if (redirect_target>authority->redirect_stress_deg) {
            authority->redirect_stress_deg=redirect_target;
        }

        const bool support_changed=authority->previous_grounded!=actor->movement.grounded ||
            (authority->previous_mode!=actor->movement.mode &&
             (support_sensitive_mode(authority->previous_mode) || support_sensitive_mode(actor->movement.mode)));
        if (support_changed) {
            const float impulse=(actor->movement.mode==NF_MOVE_MANTLE || actor->movement.mode==NF_MOVE_VAULT)
                ?0.34f:0.24f;
            if (impulse>authority->support_stress_deg) authority->support_stress_deg=impulse;
        }
    } else {
        authority->kinematic_initialized=true;
    }
    authority->previous_velocity=actor->transform.velocity;
    authority->previous_mode=actor->movement.mode;
    authority->previous_grounded=actor->movement.grounded;

    const NfWeaponId active=actor->combat.weapon;
    NfWeaponSolutionRuntime runtime={
        .focus_amount=authority->focus_amount,
        .instability_deg=authority->instability_deg[active],
        .redirect_stress_deg=authority->redirect_stress_deg,
        .support_stress_deg=authority->support_stress_deg
    };
    const bool sprinting=actor->movement.mode==NF_MOVE_SPRINT;
    const float acquisition_scale=actor->combat.state==NF_WEAPON_RELOADING
        ?NF_V16B_RELOAD_FOCUS_ACQUIRE_SCALE:1.0f;
    nf_weapon_solution_runtime_step_scaled(
        &runtime,active,focus_held,sprinting,acquisition_scale,dt);
    authority->focus_amount=runtime.focus_amount;
    authority->instability_deg[active]=runtime.instability_deg;

    for (int i=1;i<NF_WEAPON_COUNT;++i) {
        if ((NfWeaponId)i==active) continue;
        const NfWeaponSolutionTuning tuning=tuning_for((NfWeaponId)i);
        authority->instability_deg[i]-=tuning.recovery_deg_per_second*dt;
        if (authority->instability_deg[i]<0.0f) authority->instability_deg[i]=0.0f;
    }
}

NfWeaponEnvelope nf_weapon_authority_envelope(
    const NfActor *actor,
    float yaw_radians,
    float pitch_radians) {
    if (actor==NULL) return (NfWeaponEnvelope){0};
    const NfWeaponId weapon=actor->combat.weapon;
    NfWeaponEnvelopeInput input={0};
    input.weapon=weapon;
    input.yaw_radians=yaw_radians;
    input.pitch_radians=pitch_radians;
    input.velocity=actor->transform.velocity;
    input.movement_mode=actor->movement.mode;
    input.crouched=actor->movement.crouched;
    input.manipulator_contamination=actor->contamination.manipulator;
    input.runtime.focus_amount=actor->weapon_authority.focus_amount;
    if (weapon>NF_WEAPON_NONE && weapon<NF_WEAPON_COUNT) {
        input.runtime.instability_deg=actor->weapon_authority.instability_deg[weapon];
    }
    input.runtime.redirect_stress_deg=actor->weapon_authority.redirect_stress_deg;
    input.runtime.support_stress_deg=actor->weapon_authority.support_stress_deg;
    return nf_weapon_evaluate_envelope(input);
}

bool nf_weapon_authority_try_fire(
    NfActor *actor,
    const NfCombatInput *input,
    uint32_t input_sequence,
    uint64_t server_tick,
    uint32_t world_seed,
    float yaw_radians,
    NfWeaponSolution *solution_out,
    NfCombatEvent *fire_event_out) {
    if (actor==NULL || input==NULL) return false;
    const NfWeaponEnvelope envelope=nf_weapon_authority_envelope(
        actor,yaw_radians,input->aim_pitch_radians);
    NfCombatEvent event={0};
    if (!nf_combat_try_fire(actor,input,input_sequence,server_tick,&event)) return false;

    ++actor->weapon_authority.accepted_shot_sequence;
    if (actor->weapon_authority.accepted_shot_sequence==0u) {
        actor->weapon_authority.accepted_shot_sequence=1u;
    }
    const uint32_t shot_sequence=actor->weapon_authority.accepted_shot_sequence;
    const NfWeaponSolution solution=nf_weapon_realize_shot(
        &envelope,world_seed,actor->id,shot_sequence);

    NfWeaponSolutionRuntime runtime={
        .focus_amount=actor->weapon_authority.focus_amount,
        .instability_deg=actor->weapon_authority.instability_deg[actor->combat.weapon]
    };
    nf_weapon_solution_record_shot(&runtime,actor->combat.weapon);
    actor->weapon_authority.instability_deg[actor->combat.weapon]=runtime.instability_deg;

    event.shot_sequence=shot_sequence;
    event.shot_direction=solution.solved_direction;
    event.shot_blocked=false;
    if (solution_out!=NULL) *solution_out=solution;
    if (fire_event_out!=NULL) *fire_event_out=event;
    return true;
}

float nf_weapon_reticle_radius_px_for_envelope(
    const NfWeaponEnvelope *envelope,
    float vertical_fov_deg,
    float viewport_height_px) {
    if (envelope == NULL) return 7.0f;
    const float fov = clampf(vertical_fov_deg,30.0f,140.0f)*(NF_PI/180.0f);
    const float height = viewport_height_px > 1.0f ? viewport_height_px : 720.0f;
    const float cone = maxf(0.0f,envelope->total_cone_deg)*(NF_PI/180.0f);
    float physical_px = tanf(cone)/tanf(fov*0.5f)*(height*0.5f);
    if (physical_px < 0.0f) physical_px = 0.0f;

    /* Mild nonlinear readability amplification: preserve order and FOV geometry
       while keeping small truthful cones visible on ordinary displays. */
    float radius = 5.0f+physical_px+2.0f*sqrtf(physical_px);
    radius = clampf(radius,5.0f,42.0f);
    return radius;
}

float nf_weapon_reticle_radius_px(const NfWeaponSolution *solution) {
    if (solution == NULL) return 7.0f;
    NfWeaponEnvelope envelope = {0};
    envelope.weapon = solution->weapon;
    envelope.intended_direction = solution->intended_direction;
    envelope.base_cone_deg = solution->base_cone_deg;
    envelope.motion_cone_deg = solution->motion_cone_deg;
    envelope.redirect_cone_deg = solution->redirect_cone_deg;
    envelope.airborne_cone_deg = solution->airborne_cone_deg;
    envelope.support_cone_deg = solution->support_cone_deg;
    envelope.contamination_cone_deg = solution->contamination_cone_deg;
    envelope.recoil_cone_deg = solution->recoil_cone_deg;
    envelope.state_cone_deg = solution->state_cone_deg;
    envelope.total_cone_deg = solution->total_cone_deg;
    envelope.focus_amount = solution->focus_amount;
    envelope.focus_state_reduction = solution->focus_state_reduction;
    return nf_weapon_reticle_radius_px_for_envelope(&envelope,82.0f,720.0f);
}
