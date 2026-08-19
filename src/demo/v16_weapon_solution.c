#include "nf_camera.h"
#include "nf_combat.h"
#include "nf_weapon_solution.h"
#include "nf_world.h"
#include "raylib.h"

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define NF_V16_TARGETS 3u

typedef struct V16Demo {
    NfWorld world;
    NfEntityId player;
    NfEntityId targets[NF_V16_TARGETS];
    uint32_t player_fire_sequence;
    NfWeaponSolutionRuntime runtime;
    NfWeaponSolution last_solution;
    bool last_solution_valid;
    float hit_confirm;
    bool last_hit;
    bool last_head;
    NfVec3 last_impact;
    bool last_impact_valid;
} V16Demo;

static float clamp01f(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
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

static void reset_demo(V16Demo *demo) {
    nf_world_init(&demo->world,20260816u);
    nf_world_build_movement_lab(&demo->world);
    demo->player = nf_world_spawn_actor(
        &demo->world,NF_FACTION_PLAYER,(NfVec3){0.0f,0.05f,-18.0f});
    static const NfVec3 target_positions[NF_V16_TARGETS] = {
        {0.0f,0.05f,12.0f},{6.0f,0.05f,29.0f},{-8.0f,0.05f,48.0f}
    };
    for (size_t i = 0u; i < NF_V16_TARGETS; ++i) {
        demo->targets[i] = nf_world_spawn_actor(
            &demo->world,NF_FACTION_RIVAL,target_positions[i]);
    }
    demo->player_fire_sequence = 0u;
    nf_weapon_solution_runtime_init(&demo->runtime);
    demo->last_solution = (NfWeaponSolution){0};
    demo->last_solution_valid = false;
    demo->hit_confirm = 0.0f;
    demo->last_hit = false;
    demo->last_head = false;
    demo->last_impact_valid = false;
    for (unsigned i = 0u; i < 3u; ++i) {
        nf_world_step(&demo->world,1.0f/(float)NF_TICK_RATE);
    }
}

static void draw_world(const V16Demo *demo) {
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
    for (size_t i = 0u; i < NF_V16_TARGETS; ++i) {
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
        DrawCircle3D(
            (Vector3){body.x,body.y,body.z-0.38f},0.34f,(Vector3){1,0,0},90.0f,
            (Color){255,225,150,230});
    }
    if (demo->last_impact_valid) {
        DrawSphere((Vector3){demo->last_impact.x,demo->last_impact.y,demo->last_impact.z},
            0.08f,(Color){255,220,120,255});
    }
}

static void draw_reticle(const V16Demo *demo, int width, int height) {
    const float radius = demo->last_solution_valid
        ? nf_weapon_reticle_radius_px(&demo->last_solution)
        : 7.0f;
    const int x = width/2;
    const int y = height/2;
    const int r = (int)radius;
    const Color c = (Color){220,235,228,215};
    DrawLine(x-r-8,y,x-r,y,c);
    DrawLine(x+r,y,x+r+8,y,c);
    DrawLine(x,y-r-8,x,y-r,c);
    DrawLine(x,y+r,x,y+r+8,c);
    DrawCircle(x,y,2,(Color){235,246,238,225});
}

static void draw_viewmodel(const V16Demo *demo, const NfActor *player, int width, int height) {
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

static void fire_player(V16Demo *demo, NfActor *player, NfCombatInput input, float yaw, float pitch) {
    const uint32_t prospective = demo->player_fire_sequence+1u;
    NfWeaponSolutionInput solution_input = {0};
    solution_input.world_seed = demo->world.seed;
    solution_input.shooter = player->id;
    solution_input.shot_sequence = prospective;
    solution_input.weapon = player->combat.weapon;
    solution_input.yaw_radians = yaw;
    solution_input.pitch_radians = pitch;
    solution_input.velocity = player->transform.velocity;
    solution_input.movement_mode = player->movement.mode;
    solution_input.crouched = player->movement.crouched;
    solution_input.manipulator_contamination = clamp01f(player->contamination.manipulator);
    solution_input.runtime = demo->runtime;
    const NfWeaponSolution solution = nf_weapon_solve(solution_input);

    NfCombatEvent fire = {0};
    if (!nf_combat_try_fire(player,&input,prospective,demo->world.tick,&fire)) return;
    demo->player_fire_sequence = prospective;
    demo->last_solution = solution;
    demo->last_solution_valid = true;
    nf_weapon_solution_record_shot(&demo->runtime,player->combat.weapon);

    const NfWeaponSpec *spec = nf_weapon_spec(player->combat.weapon);
    const NfVec3 origin = physical_muzzle(player,yaw,pitch);
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
    InitWindow(width,height,"nightfall!punk v1.6 — Weapon Solution / Focus");
    SetTargetFPS(120);
    DisableCursor();

    V16Demo demo = {0};
    reset_demo(&demo);
    NfCameraState camera;
    nf_camera_init(&camera,82.0f);
    double accumulator = 0.0;
    const double dt = 1.0/(double)NF_TICK_RATE;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F5)) { reset_demo(&demo); camera.initialized=false; }
        const double raw = GetFrameTime();
        const double frame = raw > 0.1 ? 0.1 : raw;
        const Vector2 mouse = GetMouseDelta();
        nf_camera_apply_look_delta(&camera,mouse.x,mouse.y,0.0022f);

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
        const bool focus_held = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
        if (focus_held) move.sprint_held = false;

        accumulator += frame;
        while (accumulator >= dt) {
            nf_world_set_input(&demo.world,demo.player,move);
            NfActor *player = nf_world_find_actor(&demo.world,demo.player);
            if (player != NULL) {
                const bool sprinting = player->movement.mode == NF_MOVE_SPRINT;
                nf_weapon_solution_runtime_step(
                    &demo.runtime,player->combat.weapon,focus_held,sprinting,(float)dt);
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
        const NfActor *player = nf_world_find_actor_const(&demo.world,demo.player);
        if (player != NULL) {
            nf_camera_follow_actor(&camera,player,&demo.world,(float)frame);
            nf_camera_step_presentation(&camera,(float)frame);
        }
        const NfVec3 anchor = nf_camera_render_anchor(&camera);
        const NfVec3 forward = nf_camera_forward(&camera);
        Camera3D ray_camera = {
            .position={anchor.x,anchor.y,anchor.z},
            .target={anchor.x+forward.x,anchor.y+forward.y,anchor.z+forward.z},
            .up={0,1,0},
            .fovy=camera.current_vertical_fov_deg-demo.runtime.focus_amount*7.0f,
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

        DrawText("nightfall!punk v1.6 — WEAPON SOLUTION / FOCUS",20,14,21,RAYWHITE);
        DrawText("WASD Shift Ctrl Space | LMB fire | RMB Focus | R reload | 1/2 weapon | F5 reset",20,40,14,GRAY);
        if (player != NULL) {
            DrawText(TextFormat(
                "WEAPON %s | %u/%u | %s | FOCUS %.2f | recoil-instability %.2f deg",
                nf_weapon_name(player->combat.weapon),
                player->combat.ammo_mag[player->combat.weapon],
                player->combat.reserve_ammo[player->combat.weapon],
                nf_weapon_state_name(player->combat.state),
                demo.runtime.focus_amount,demo.runtime.instability_deg),20,66,15,RAYWHITE);
        }
        if (demo.last_solution_valid) {
            DrawText(TextFormat(
                "SOLUTION cone %.3f deg = base %.3f + state %.3f | move %.3f air %.3f contam %.3f recoil %.3f",
                demo.last_solution.total_cone_deg,demo.last_solution.base_cone_deg,
                demo.last_solution.state_cone_deg,demo.last_solution.motion_cone_deg,
                demo.last_solution.airborne_cone_deg,demo.last_solution.contamination_cone_deg,
                demo.last_solution.recoil_cone_deg),20,90,14,(Color){150,225,190,255});
            DrawText(TextFormat(
                "FOCUS state-reduction %.2f | intended(%.3f %.3f %.3f) -> solved(%.3f %.3f %.3f)",
                demo.last_solution.focus_state_reduction,
                demo.last_solution.intended_direction.x,demo.last_solution.intended_direction.y,
                demo.last_solution.intended_direction.z,demo.last_solution.solved_direction.x,
                demo.last_solution.solved_direction.y,demo.last_solution.solved_direction.z),
                20,112,14,(Color){130,195,255,255});
        }
        DrawText("CONTRACT: center = intention | brackets = embodied uncertainty | Focus recaptures instability, not targets",20,138,14,GOLD);
        DrawText("Proof: compare stationary hip, moving hip, moving RMB, stop while holding RMB, sustained fire, crouch/jump, near-wall muzzle feasibility.",20,160,13,GRAY);
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
