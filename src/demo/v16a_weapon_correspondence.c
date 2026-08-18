#include "nf_camera.h"
#include "nf_combat.h"
#include "nf_weapon_solution.h"
#include "nf_world.h"
#include "raylib.h"

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_V16A_TARGETS 3u
#define NF_V16A_PI 3.14159265358979323846f

typedef struct V16ADemo {
    NfWorld world;
    NfEntityId player;
    NfEntityId targets[NF_V16A_TARGETS];
    uint32_t player_fire_sequence;
    NfWeaponSolutionRuntime runtime;

    NfWeaponEnvelope current_envelope;
    bool current_envelope_valid;
    float displayed_reticle_radius;
    bool current_muzzle_blocked;

    NfWeaponSolution last_solution;
    bool last_solution_valid;
    NfVec3 last_muzzle;
    NfVec3 last_impact;
    bool last_impact_valid;
    bool last_shot_blocked;

    bool raw_rmb;
    bool debug_focus;
    bool window_focused;
    bool previous_rmb;
    bool input_initialized;
    uint32_t rmb_transitions;
    float focus_target;

    float camera_recoil_pitch_deg;
    float hit_confirm;
    bool last_hit;
    bool last_head;
} V16ADemo;

static float clamp01f(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

static Vector3 rv3(NfVec3 value) {
    return (Vector3){value.x,value.y,value.z};
}

static NfVec3 forward_from_angles(float yaw, float pitch) {
    const float cp = cosf(pitch);
    return (NfVec3){sinf(yaw)*cp,sinf(pitch),cosf(yaw)*cp};
}

static NfVec3 physical_muzzle(const NfActor *actor, float yaw, float pitch) {
    const NfVec3 forward = forward_from_angles(yaw,pitch);
    return (NfVec3){
        actor->transform.position.x+forward.x*0.38f,
        actor->transform.position.y+actor->movement.eye_height-0.14f+forward.y*0.12f,
        actor->transform.position.z+forward.z*0.38f
    };
}

static bool ray_aabb(
    NfVec3 origin, NfVec3 direction, NfVec3 minimum, NfVec3 maximum,
    float *distance) {
    float tmin = 0.0f;
    float tmax = FLT_MAX;
    const float ov[3] = {origin.x,origin.y,origin.z};
    const float dv[3] = {direction.x,direction.y,direction.z};
    const float lo[3] = {minimum.x,minimum.y,minimum.z};
    const float hi[3] = {maximum.x,maximum.y,maximum.z};
    for (int i = 0; i < 3; ++i) {
        if (fabsf(dv[i]) < 1e-6f) {
            if (ov[i] < lo[i] || ov[i] > hi[i]) return false;
            continue;
        }
        float t1 = (lo[i]-ov[i])/dv[i];
        float t2 = (hi[i]-ov[i])/dv[i];
        if (t1 > t2) {
            const float temporary = t1;
            t1 = t2;
            t2 = temporary;
        }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return false;
    }
    if (tmax < 0.0f) return false;
    if (distance != NULL) *distance = tmin >= 0.0f ? tmin : tmax;
    return true;
}

static bool ray_sphere(
    NfVec3 origin, NfVec3 direction, NfVec3 center, float radius,
    float *distance) {
    const NfVec3 oc = {origin.x-center.x,origin.y-center.y,origin.z-center.z};
    const float b = oc.x*direction.x+oc.y*direction.y+oc.z*direction.z;
    const float c = oc.x*oc.x+oc.y*oc.y+oc.z*oc.z-radius*radius;
    const float discriminant = b*b-c;
    if (discriminant < 0.0f) return false;
    const float root = sqrtf(discriminant);
    float t = -b-root;
    if (t < 0.0f) t = -b+root;
    if (t < 0.0f) return false;
    if (distance != NULL) *distance = t;
    return true;
}

static bool nearest_world_hit(
    const NfWorld *world, NfVec3 origin, NfVec3 direction, float maximum_distance,
    float *distance_out, NfVec3 *impact_out) {
    float best = maximum_distance;
    bool hit = false;
    for (size_t i = 0u; i < world->collider_count; ++i) {
        const NfCollider *collider = &world->colliders[i];
        if (collider->kind == NF_COLLIDER_LADDER) continue;
        float t = 0.0f;
        if (ray_aabb(origin,direction,collider->min,collider->max,&t) &&
            t > 0.04f && t < best) {
            best = t;
            hit = true;
        }
    }
    if (!hit) return false;
    if (distance_out != NULL) *distance_out = best;
    if (impact_out != NULL) {
        *impact_out = (NfVec3){
            origin.x+direction.x*best,
            origin.y+direction.y*best,
            origin.z+direction.z*best
        };
    }
    return true;
}

static NfEntityId hitscan(
    const NfWorld *world, NfEntityId shooter, NfFaction shooter_faction,
    NfVec3 origin, NfVec3 direction, float range,
    NfHitZone *zone_out, NfVec3 *impact_out) {
    float best_distance = range;
    NfEntityId best = 0u;
    NfHitZone best_zone = NF_HIT_NONE;

    for (size_t i = 0u; i < world->collider_count; ++i) {
        const NfCollider *collider = &world->colliders[i];
        if (collider->kind == NF_COLLIDER_LADDER) continue;
        float t = 0.0f;
        if (ray_aabb(origin,direction,collider->min,collider->max,&t) &&
            t > 0.04f && t < best_distance) {
            best_distance = t;
            best = 0u;
            best_zone = NF_HIT_NONE;
        }
    }

    for (size_t i = 0u; i < NF_MAX_ENTITIES; ++i) {
        const NfActor *actor = &world->actors[i];
        if (!actor->active || !actor->combat.alive || actor->id == shooter) continue;
        if (!nf_combat_can_damage(shooter_faction,actor->faction,false)) continue;
        const NfVec3 head = {
            actor->transform.position.x,
            actor->transform.position.y+1.55f,
            actor->transform.position.z
        };
        const NfVec3 body = {
            actor->transform.position.x,
            actor->transform.position.y+0.90f,
            actor->transform.position.z
        };
        float t = 0.0f;
        if (ray_sphere(origin,direction,head,0.30f,&t) && t < best_distance) {
            best_distance = t;
            best = actor->id;
            best_zone = NF_HIT_HEAD;
        } else if (ray_sphere(origin,direction,body,0.58f,&t) && t < best_distance) {
            best_distance = t;
            best = actor->id;
            best_zone = NF_HIT_BODY;
        }
    }

    if (impact_out != NULL) {
        *impact_out = (NfVec3){
            origin.x+direction.x*best_distance,
            origin.y+direction.y*best_distance,
            origin.z+direction.z*best_distance
        };
    }
    if (zone_out != NULL) *zone_out = best_zone;
    return best;
}

static NfWeaponEnvelope evaluate_current(
    const NfActor *player, NfWeaponSolutionRuntime runtime,
    float yaw, float pitch) {
    NfWeaponEnvelopeInput input = {0};
    input.weapon = player->combat.weapon;
    input.yaw_radians = yaw;
    input.pitch_radians = pitch;
    input.velocity = player->transform.velocity;
    input.movement_mode = player->movement.mode;
    input.crouched = player->movement.crouched;
    input.manipulator_contamination = clamp01f(player->contamination.manipulator);
    input.runtime = runtime;
    return nf_weapon_evaluate_envelope(input);
}

static void reset_demo(V16ADemo *demo) {
    nf_world_init(&demo->world,20260817u);
    nf_world_build_movement_lab(&demo->world);
    demo->player = nf_world_spawn_actor(
        &demo->world,NF_FACTION_PLAYER,(NfVec3){0.0f,0.05f,-18.0f});
    static const NfVec3 target_positions[NF_V16A_TARGETS] = {
        {0.0f,0.05f,12.0f},{6.0f,0.05f,29.0f},{-8.0f,0.05f,48.0f}
    };
    for (size_t i = 0u; i < NF_V16A_TARGETS; ++i) {
        demo->targets[i] = nf_world_spawn_actor(
            &demo->world,NF_FACTION_RIVAL,target_positions[i]);
    }
    demo->player_fire_sequence = 0u;
    nf_weapon_solution_runtime_init(&demo->runtime);
    demo->current_envelope = (NfWeaponEnvelope){0};
    demo->current_envelope_valid = false;
    demo->displayed_reticle_radius = 7.0f;
    demo->current_muzzle_blocked = false;
    demo->last_solution = (NfWeaponSolution){0};
    demo->last_solution_valid = false;
    demo->last_impact_valid = false;
    demo->last_shot_blocked = false;
    demo->raw_rmb = false;
    demo->debug_focus = false;
    demo->window_focused = true;
    demo->previous_rmb = false;
    demo->input_initialized = false;
    demo->rmb_transitions = 0u;
    demo->focus_target = 0.0f;
    demo->camera_recoil_pitch_deg = 0.0f;
    demo->hit_confirm = 0.0f;
    demo->last_hit = false;
    demo->last_head = false;
    for (unsigned i = 0u; i < 3u; ++i) {
        nf_world_step(&demo->world,1.0f/(float)NF_TICK_RATE);
    }
}

static void draw_world(const V16ADemo *demo) {
    for (size_t i = 0u; i < demo->world.collider_count; ++i) {
        const NfCollider *c = &demo->world.colliders[i];
        const float cx = 0.5f*(c->min.x+c->max.x);
        const float cz = 0.5f*(c->min.z+c->max.z);
        if (fabsf(cx) > 55.0f || fabsf(cz) > 75.0f) continue;
        const Vector3 p = {cx,0.5f*(c->min.y+c->max.y),cz};
        DrawCube(p,c->max.x-c->min.x,c->max.y-c->min.y,c->max.z-c->min.z,
            (Color){68,76,82,255});
        DrawCubeWires(p,c->max.x-c->min.x,c->max.y-c->min.y,c->max.z-c->min.z,
            (Color){115,124,130,255});
    }
    for (size_t i = 0u; i < NF_V16A_TARGETS; ++i) {
        const NfActor *target = nf_world_find_actor_const(&demo->world,demo->targets[i]);
        if (target == NULL) continue;
        const Color color = target->combat.alive
            ? (Color){224,66,74,255} : (Color){90,55,60,255};
        const Vector3 body = {
            target->transform.position.x,
            target->transform.position.y+0.90f,
            target->transform.position.z
        };
        DrawCube(body,1.12f,1.80f,0.72f,color);
        DrawSphere((Vector3){body.x,body.y+0.78f,body.z},0.30f,(Color){245,175,120,255});
    }
    if (demo->last_impact_valid) {
        DrawSphere(rv3(demo->last_impact),0.08f,(Color){255,220,120,255});
        DrawLine3D(rv3(demo->last_muzzle),rv3(demo->last_impact),(Color){255,210,105,170});
    }
    if (demo->current_envelope_valid) {
        const NfActor *player = nf_world_find_actor_const(&demo->world,demo->player);
        if (player != NULL) {
            const NfVec3 muzzle = physical_muzzle(
                player,demo->current_envelope.aim_yaw_radians,
                demo->current_envelope.aim_pitch_radians);
            const NfVec3 end = {
                muzzle.x+demo->current_envelope.intended_direction.x*2.0f,
                muzzle.y+demo->current_envelope.intended_direction.y*2.0f,
                muzzle.z+demo->current_envelope.intended_direction.z*2.0f
            };
            DrawSphere(rv3(muzzle),0.045f,(Color){80,220,240,220});
            DrawLine3D(rv3(muzzle),rv3(end),(Color){80,220,240,180});
        }
    }
}

static void draw_reticle(const V16ADemo *demo, int width, int height) {
    const int x = width/2;
    const int y = height/2;
    const int r = (int)demo->displayed_reticle_radius;
    const Color normal = (Color){220,235,228,215};
    const Color blocked = (Color){245,190,90,230};
    const Color c = demo->current_muzzle_blocked ? blocked : normal;
    DrawLine(x-r-8,y,x-r,y,c);
    DrawLine(x+r,y,x+r+8,y,c);
    DrawLine(x,y-r-8,x,y-r,c);
    DrawLine(x,y+r,x,y+r+8,c);
    DrawCircle(x,y,2,(Color){235,246,238,225});
    if (demo->current_muzzle_blocked) {
        DrawLine(x-5,y-5,x-2,y-2,blocked);
        DrawLine(x+5,y-5,x+2,y-2,blocked);
    }
}

static void draw_viewmodel(const V16ADemo *demo, const NfActor *player, int width, int height) {
    if (player == NULL) return;
    const float speed = sqrtf(player->transform.velocity.x*player->transform.velocity.x+
        player->transform.velocity.z*player->transform.velocity.z);
    const float side = sinf((float)demo->player_fire_sequence*1.7f)*demo->runtime.instability_deg*7.0f;
    const float bob = speed*0.8f+demo->runtime.instability_deg*9.0f;
    const float center = (float)width*0.5f+side;
    const float base = (float)height-72.0f+bob-demo->runtime.focus_amount*32.0f;
    DrawRectangle((int)(center-54),(int)(base-18),108,30,(Color){62,76,72,255});
    DrawRectangle((int)(center+28),(int)(base-25),50,12,(Color){116,142,126,255});
    DrawRectangle((int)(center-18),(int)(base+10),28,35,(Color){48,54,52,255});
    DrawRectangleLines((int)(center-54),(int)(base-18),108,30,(Color){176,205,188,255});
    DrawText("ECO-TECH GUARD",(int)(center-48),(int)(base-14),10,(Color){185,220,195,255});
}

static void fire_player(
    V16ADemo *demo, NfActor *player, NfCombatInput input,
    float yaw, float pitch) {
    const uint32_t prospective = demo->player_fire_sequence+1u;
    const NfWeaponEnvelope current = evaluate_current(player,demo->runtime,yaw,pitch);

    NfCombatEvent fire = {0};
    if (!nf_combat_try_fire(player,&input,prospective,demo->world.tick,&fire)) return;

    const NfWeaponSolution solution = nf_weapon_realize_shot(
        &current,demo->world.seed,player->id,prospective);
    demo->player_fire_sequence = prospective;
    demo->last_solution = solution;
    demo->last_solution_valid = true;

    const NfVec3 origin = physical_muzzle(player,yaw,pitch);
    demo->last_muzzle = origin;
    demo->last_shot_blocked = nearest_world_hit(
        &demo->world,origin,current.intended_direction,1.10f,NULL,NULL);

    nf_weapon_solution_record_shot(&demo->runtime,player->combat.weapon);
    demo->camera_recoil_pitch_deg += 0.16f;
    if (demo->camera_recoil_pitch_deg > 0.55f) demo->camera_recoil_pitch_deg = 0.55f;

    const NfWeaponSpec *spec = nf_weapon_spec(player->combat.weapon);
    NfHitZone zone = NF_HIT_NONE;
    NfVec3 impact = {0};
    const NfEntityId hit = hitscan(
        &demo->world,player->id,player->faction,origin,solution.solved_direction,
        spec->range,&zone,&impact);
    demo->last_impact = impact;
    demo->last_impact_valid = true;
    demo->last_hit = false;
    demo->last_head = false;
    demo->hit_confirm = 0.0f;
    if (hit == 0u) return;

    NfActor *target = nf_world_find_actor(&demo->world,hit);
    if (target == NULL) return;
    const float amount = spec->damage*(zone == NF_HIT_HEAD ? spec->head_multiplier : 1.0f);
    NfCombatEvent damage = {0};
    if (nf_combat_apply_damage(
            target,player->id,player->combat.weapon,zone,amount,demo->world.tick,&damage)) {
        demo->last_hit = true;
        demo->last_head = zone == NF_HIT_HEAD;
        demo->hit_confirm = 0.16f;
    }
}

int main(void) {
    const int width = 1280;
    const int height = 720;
    InitWindow(width,height,"nightfall!punk v1.6A — Weapon Correspondence / Calibration");
    SetTargetFPS(120);
    DisableCursor();

    V16ADemo demo = {0};
    reset_demo(&demo);
    NfCameraState camera;
    nf_camera_init(&camera,82.0f);
    double accumulator = 0.0;
    const double dt = 1.0/(double)NF_TICK_RATE;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F5)) {
            reset_demo(&demo);
            camera.initialized = false;
        }
        const double raw_frame = GetFrameTime();
        const double frame = raw_frame > 0.1 ? 0.1 : raw_frame;
        const Vector2 mouse = GetMouseDelta();
        nf_camera_apply_look_delta(&camera,mouse.x,mouse.y,0.0022f);

        demo.window_focused = IsWindowFocused();
        demo.raw_rmb = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
        demo.debug_focus = IsKeyDown(KEY_F);
        if (!demo.input_initialized) {
            demo.previous_rmb = demo.raw_rmb;
            demo.input_initialized = true;
        } else if (demo.raw_rmb != demo.previous_rmb) {
            ++demo.rmb_transitions;
            demo.previous_rmb = demo.raw_rmb;
        }
        const bool focus_request = demo.window_focused && (demo.raw_rmb || demo.debug_focus);

        NfMoveInput move = {0};
        NfCombatInput combat = {0};
        move.forward = (IsKeyDown(KEY_W)?1.0f:0.0f)-(IsKeyDown(KEY_S)?1.0f:0.0f);
        move.strafe = (IsKeyDown(KEY_A)?1.0f:0.0f)-(IsKeyDown(KEY_D)?1.0f:0.0f);
        move.sprint_held = IsKeyDown(KEY_LEFT_SHIFT);
        move.crouch_held = IsKeyDown(KEY_LEFT_CONTROL);
        move.jump_pressed = IsKeyPressed(KEY_SPACE);
        move.yaw_radians = camera.yaw;
        combat.fire_held = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        combat.fire_pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        combat.reload_pressed = IsKeyPressed(KEY_R);
        combat.aim_pitch_radians = camera.pitch;
        if (focus_request) move.sprint_held = false;

        accumulator += frame;
        while (accumulator >= dt) {
            nf_world_set_input(&demo.world,demo.player,move);
            NfActor *player = nf_world_find_actor(&demo.world,demo.player);
            if (player != NULL) {
                const bool sprinting = player->movement.mode == NF_MOVE_SPRINT;
                demo.focus_target = nf_weapon_focus_target(focus_request,sprinting);
                nf_weapon_solution_runtime_step(
                    &demo.runtime,player->combat.weapon,focus_request,sprinting,(float)dt);
                if (combat.reload_pressed) (void)nf_combat_start_reload(player);
                if (IsKeyDown(KEY_ONE)) (void)nf_combat_select_weapon(player,NF_WEAPON_CARBINE);
                if (IsKeyDown(KEY_TWO)) (void)nf_combat_select_weapon(player,NF_WEAPON_PISTOL);
                if (combat.fire_held || combat.fire_pressed) {
                    fire_player(&demo,player,combat,camera.yaw,camera.pitch);
                }
            }
            nf_world_step(&demo.world,(float)dt);
            move.jump_pressed = false;
            combat.fire_pressed = false;
            combat.reload_pressed = false;
            accumulator -= dt;
        }

        if (demo.hit_confirm > 0.0f) demo.hit_confirm -= (float)frame;
        demo.camera_recoil_pitch_deg *= expf(-18.0f*(float)frame);
        if (fabsf(demo.camera_recoil_pitch_deg) < 0.001f) demo.camera_recoil_pitch_deg = 0.0f;

        const NfActor *player = nf_world_find_actor_const(&demo.world,demo.player);
        if (player != NULL) {
            nf_camera_follow_actor(&camera,player,&demo.world,(float)frame);
            nf_camera_step_presentation(&camera,(float)frame);
            demo.current_envelope = evaluate_current(player,demo.runtime,camera.yaw,camera.pitch);
            demo.current_envelope_valid = true;
            const NfVec3 muzzle = physical_muzzle(player,camera.yaw,camera.pitch);
            demo.current_muzzle_blocked = nearest_world_hit(
                &demo.world,muzzle,demo.current_envelope.intended_direction,1.10f,NULL,NULL);
        }

        const float render_fov = camera.current_vertical_fov_deg-demo.runtime.focus_amount*7.0f;
        if (demo.current_envelope_valid) {
            const float target_radius = nf_weapon_reticle_radius_px_for_envelope(
                &demo.current_envelope,render_fov,(float)height);
            const float alpha = clamp01f((float)frame/0.055f);
            demo.displayed_reticle_radius +=
                (target_radius-demo.displayed_reticle_radius)*alpha;
        }

        const NfVec3 anchor = nf_camera_render_anchor(&camera);
        const float render_pitch = camera.pitch+
            demo.camera_recoil_pitch_deg*(NF_V16A_PI/180.0f);
        const NfVec3 render_forward = forward_from_angles(camera.yaw,render_pitch);
        Camera3D ray_camera = {
            .position={anchor.x,anchor.y,anchor.z},
            .target={anchor.x+render_forward.x,anchor.y+render_forward.y,anchor.z+render_forward.z},
            .up={0,1,0},
            .fovy=render_fov,
            .projection=CAMERA_PERSPECTIVE
        };

        BeginDrawing();
        ClearBackground((Color){14,18,20,255});
        BeginMode3D(ray_camera);
        DrawGrid(120,1);
        draw_world(&demo);
        EndMode3D();
        draw_viewmodel(&demo,player,width,height);
        draw_reticle(&demo,width,height);

        DrawText("nightfall!punk v1.6A — WEAPON CORRESPONDENCE / CALIBRATION",20,14,20,RAYWHITE);
        DrawText("WASD Shift Ctrl Space | LMB fire | RMB Focus | F debug-Focus control | R reload | 1/2 weapon | F5 reset",20,39,13,GRAY);
        DrawText(TextFormat(
            "INPUT  RMB=%s  F=%s  window=%s  transitions=%u  target=%.2f  Focus=%.2f",
            demo.raw_rmb?"DOWN":"UP",demo.debug_focus?"DOWN":"UP",
            demo.window_focused?"YES":"NO",demo.rmb_transitions,
            demo.focus_target,demo.runtime.focus_amount),20,62,14,(Color){165,210,255,255});
        if (player != NULL && demo.current_envelope_valid) {
            DrawText(TextFormat(
                "NOW  %s  cone %.3f = base %.3f + state %.3f | move %.3f air %.3f contam %.3f recoil %.3f | blocked=%s",
                nf_weapon_name(player->combat.weapon),demo.current_envelope.total_cone_deg,
                demo.current_envelope.base_cone_deg,demo.current_envelope.state_cone_deg,
                demo.current_envelope.motion_cone_deg,demo.current_envelope.airborne_cone_deg,
                demo.current_envelope.contamination_cone_deg,demo.current_envelope.recoil_cone_deg,
                demo.current_muzzle_blocked?"YES":"NO"),20,85,13,(Color){150,225,190,255});
            DrawText(TextFormat(
                "NOW  Focus reduction %.2f | reticle %.1f px | FOV %.1f | camera recoil %.2f deg",
                demo.current_envelope.focus_state_reduction,demo.displayed_reticle_radius,
                render_fov,demo.camera_recoil_pitch_deg),20,106,13,(Color){150,225,190,255});
        }
        if (demo.last_solution_valid) {
            DrawText(TextFormat(
                "LAST SHOT  %s seq %u | cone %.3f | Focus %.2f reduction %.2f | blocked=%s",
                nf_weapon_name(demo.last_solution.weapon),demo.last_solution.shot_sequence,
                demo.last_solution.total_cone_deg,demo.last_solution.focus_amount,
                demo.last_solution.focus_state_reduction,demo.last_shot_blocked?"YES":"NO"),
                20,129,13,(Color){255,205,125,255});
        }
        DrawText("CONTRACT: center=intention | live brackets=NOW uncertainty | LAST SHOT=history | amber cue=near muzzle obstruction",20,153,13,GOLD);
        DrawText("Proof: no-fire move/stop; sky RMB hold; F control; move->stop Focus; pistol->carbine without fire; burst recovery; near-wall shot.",20,174,12,GRAY);
        if (demo.hit_confirm > 0.0f && demo.last_hit) {
            DrawCircleLines(width/2,height/2,16,
                demo.last_head ? (Color){255,225,120,225} : (Color){225,240,230,215});
        }
        EndDrawing();
    }

    EnableCursor();
    CloseWindow();
    return 0;
}
