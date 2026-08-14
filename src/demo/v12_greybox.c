#include "nf_camera.h"
#include "nf_route.h"
#include "nf_teammate.h"
#include "nf_world.h"
#include "raylib.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

static void draw_actor(const NfActor *a, Color c) {
    if (a == NULL) return;
    Vector3 p = {a->transform.position.x, a->transform.position.y + 0.9f, a->transform.position.z};
    DrawCube(p, 0.8f, 1.8f, 0.8f, c);
    DrawCubeWires(p, 0.8f, 1.8f, 0.8f, RAYWHITE);
    if (a->faction == NF_FACTION_TEAMMATE) DrawSphere((Vector3){p.x, p.y + 1.15f, p.z}, 0.16f, c);
}

static void draw_world(const NfWorld *w, const NfRouteSystem *r) {
    for (size_t i = 0; i < w->collider_count; ++i) {
        const NfCollider *c = &w->colliders[i];
        const float cx = 0.5f * (c->min.x + c->max.x);
        const float cz = 0.5f * (c->min.z + c->max.z);
        if (fabsf(cx) > 42.0f || fabsf(cz) > 42.0f) continue;
        Vector3 p = {cx, 0.5f * (c->min.y + c->max.y), cz};
        Color color = (int)i == r->gate_collider ? (Color){170,95,185,255} : (Color){105,111,120,255};
        DrawCube(p, c->max.x-c->min.x, c->max.y-c->min.y, c->max.z-c->min.z, color);
        DrawCubeWires(p, c->max.x-c->min.x, c->max.y-c->min.y, c->max.z-c->min.z, GRAY);
    }
    DrawLine3D((Vector3){r->route_a_entry.x,.15f,r->route_a_entry.z},
               (Vector3){r->route_a_reconnect.x,.15f,r->route_a_reconnect.z}, LIGHTGRAY);
    DrawLine3D((Vector3){r->route_a_reconnect.x,.15f,r->route_a_reconnect.z},
               (Vector3){r->goal.x,.15f,r->goal.z}, LIGHTGRAY);
    DrawLine3D((Vector3){r->route_b_entry.x,.18f,r->route_b_entry.z},
               (Vector3){r->route_b_reconnect.x,.18f,r->route_b_reconnect.z}, SKYBLUE);
    DrawLine3D((Vector3){r->route_b_reconnect.x,.18f,r->route_b_reconnect.z},
               (Vector3){r->goal.x,.18f,r->goal.z}, SKYBLUE);
    DrawSphere((Vector3){r->goal.x,.3f,r->goal.z}, .35f, GOLD);
}

static void reset(NfWorld *w, NfRouteSystem *r, NfTeammateSystem *t, NfEntityId *player) {
    nf_world_init(w, 20260813u);
    nf_world_build_movement_lab(w);
    nf_route_init_v12_slice(r, w);
    *player = nf_world_spawn_actor(w, NF_FACTION_PLAYER, (NfVec3){0,.05f,-18});
    nf_teammate_init(t, w, true, w->seed ^ 0x12u);
    for (unsigned i = 0; i < 3u; ++i) nf_world_step(w, 1.0f/(float)NF_TICK_RATE);
}

static Camera3D build_raylib_camera(const NfCameraState *camera) {
    if (camera->mode == NF_CAMERA_DEBUG_OBSERVER) {
        return (Camera3D){
            .position={48,58,-58}, .target={0,0,6}, .up={0,1,0},
            .fovy=56, .projection=CAMERA_PERSPECTIVE
        };
    }
    if (camera->mode == NF_CAMERA_DEBUG_TOP) {
        return (Camera3D){
            .position={0,78,0.01f}, .target={0,0,0}, .up={0,0,1},
            .fovy=50, .projection=CAMERA_PERSPECTIVE
        };
    }
    const NfVec3 anchor = nf_camera_render_anchor(camera);
    const NfVec3 forward = nf_camera_forward(camera);
    return (Camera3D){
        .position={anchor.x,anchor.y,anchor.z},
        .target={anchor.x+forward.x,anchor.y+forward.y,anchor.z+forward.z},
        .up={0,1,0},
        .fovy=camera->current_vertical_fov_deg,
        .projection=CAMERA_PERSPECTIVE
    };
}

int main(void) {
    const int width = 1280, height = 720;
    InitWindow(width, height, "nightfall!punk v1.2b camera rebuild / causal greybox");
    SetTargetFPS(120);
    DisableCursor();

    NfWorld world;
    NfRouteSystem route;
    NfTeammateSystem teammate;
    NfEntityId player = 0;
    NfCameraState camera;
    reset(&world, &route, &teammate, &player);
    nf_camera_init(&camera, 82.0f); /* Explicit vertical-FOV baseline; ~114 deg horizontal at 16:9. */

    double acc = 0.0;
    const double dt = 1.0/(double)NF_TICK_RATE;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F1)) { nf_camera_set_mode(&camera, NF_CAMERA_FIRST_PERSON); DisableCursor(); }
        if (IsKeyPressed(KEY_F2)) { nf_camera_set_mode(&camera, NF_CAMERA_DEBUG_OBSERVER); EnableCursor(); }
        if (IsKeyPressed(KEY_F3)) { nf_camera_set_mode(&camera, NF_CAMERA_DEBUG_TOP); EnableCursor(); }
        if (IsKeyPressed(KEY_F5)) {
            reset(&world, &route, &teammate, &player);
            camera.initialized = false;
        }

        const double raw_frame = GetFrameTime();
        const double frame = raw_frame > 0.1 ? 0.1 : raw_frame;
        if (camera.mode == NF_CAMERA_FIRST_PERSON) {
            const Vector2 mouse = GetMouseDelta();
            nf_camera_apply_look_delta(&camera, mouse.x, mouse.y, 0.0022f);
        }

        NfMoveInput in = {0};
        if (camera.mode == NF_CAMERA_FIRST_PERSON) {
            in.forward = (IsKeyDown(KEY_W)?1.0f:0.0f) - (IsKeyDown(KEY_S)?1.0f:0.0f);
            in.strafe = (IsKeyDown(KEY_A)?1.0f:0.0f) - (IsKeyDown(KEY_D)?1.0f:0.0f);
            in.sprint_held = IsKeyDown(KEY_LEFT_SHIFT);
            in.interact_held = IsKeyDown(KEY_E);
            in.jump_pressed = IsKeyPressed(KEY_SPACE);
            in.crouch_held = IsKeyDown(KEY_LEFT_CONTROL);
            in.yaw_radians = camera.yaw;
        }

        acc += frame;
        while (acc >= dt) {
            nf_world_set_input(&world, player, in);
            (void)nf_route_try_alter(&route, &world, player);
            NfControlFrame ai = {0};
            if (nf_teammate_tick(&teammate, &world, &route, &ai, 1u) > 0u)
                nf_world_set_input(&world, teammate.actor_id, ai.move);
            nf_world_step(&world, (float)dt);
            in.jump_pressed = false;
            acc -= dt;
        }

        const NfActor *p = nf_world_find_actor_const(&world, player);
        const NfActor *a = nf_world_find_actor_const(&world, teammate.actor_id);
        if (p != NULL) {
            nf_camera_follow_actor(&camera, p, &world, (float)frame);
            nf_camera_step_presentation(&camera, (float)frame);
        }
        const Camera3D ray_camera = build_raylib_camera(&camera);

        BeginDrawing();
        ClearBackground((Color){18,20,24,255});
        BeginMode3D(ray_camera);
        DrawGrid(80,1);
        draw_world(&world,&route);
        if (camera.mode != NF_CAMERA_FIRST_PERSON) draw_actor(p,RAYWHITE);
        draw_actor(a,(Color){46,168,255,255});
        EndMode3D();

        char route_b_utility[32];
        if (route.open) snprintf(route_b_utility, sizeof(route_b_utility), "%.2f", teammate.trace.route_b_utility);
        else snprintf(route_b_utility, sizeof(route_b_utility), "BLOCKED");

        const float hfov = nf_camera_horizontal_fov_deg(&camera, (float)width/(float)height);
        DrawText("nightfall!punk v1.2b — CAMERA REBUILD / TEAMMATE CAUSALITY",20,18,22,RAYWHITE);
        DrawText("F1 PLAYER VIEW | F2 OBSERVER/LIVE | F3 DEBUG TOP/LIVE | F5 reset",20,46,16,GRAY);
        DrawText("PLAYER: WASD move | Shift sprint | Ctrl crouch | Space jump | hold E near purple ALTER_ROUTE gate",20,68,15,GRAY);
        DrawText(TextFormat("CAMERA %s | correction %s | anchor error %.3fm | FOV %.1fV / %.1fH@16:9",
                 nf_camera_mode_name(camera.mode), nf_camera_correction_name(camera.correction_class),
                 camera.anchor_error, camera.current_vertical_fov_deg, hfov),20,94,16,
                 camera.mode==NF_CAMERA_FIRST_PERSON?SKYBLUE:GOLD);
        DrawText(TextFormat("ALTER_ROUTE %s | gate %.1fm",nf_route_state_name(&route),
                 p?nf_route_distance_to_gate(&route,p->transform.position):0.0f),20,118,18,
                 route.open?SKYBLUE:(Color){200,170,210,255});
        DrawText(TextFormat("EVIDENCE %s | BELIEF %s | GRADIENT %.2f | COMMIT %s",
                 nf_teammate_evidence_name(teammate.trace.evidence),
                 teammate.memory.known_open?"OPEN":"UNCHANGED",
                 teammate.trace.hotspot_gradient,nf_teammate_commitment_name(teammate.commitment)),20,142,18,(Color){46,168,255,255});
        DrawText(TextFormat("ENERGY A %.1f B %.1f | UTILITY A %.2f B %s | OCCUPANCY %s",
                 teammate.trace.route_a_energy,teammate.trace.route_b_energy,teammate.trace.route_a_utility,
                 route_b_utility,teammate.trace.yielded_for_occupancy?"YIELD":"CLEAR"),20,166,16,RAYWHITE);
        DrawText("BODY/PHYSICS -> CAMERA PRESENTATION | camera never changes simulation or actor knowledge",20,190,16,GOLD);
        DrawText("EVIDENCE -> BELIEF -> GRADIENT -> HOTSPOT -> UTILITY -> COMMITMENT -> ACTION",20,212,15,GOLD);
        DrawText("v1.2b proof: FPS perception restored; observer views are explicit diagnostics; v1.2 causality remains authoritative",20,688,14,GRAY);
        EndDrawing();
    }

    EnableCursor();
    CloseWindow();
    return 0;
}
