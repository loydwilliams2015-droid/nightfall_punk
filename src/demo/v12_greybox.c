#include "nf_ai.h"
#include "nf_camera.h"
#include "nf_cattler.h"
#include "nf_contamination.h"
#include "nf_hitbox.h"
#include "nf_route.h"
#include "nf_semantics.h"
#include "nf_teammate.h"
#include "nf_world.h"
#include "raylib.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define NF_V12C_RIVALS 2u
#define NF_V12C_CATTLERS 2u

static float distance_xz(NfVec3 a,NfVec3 b) {
    const float dx=a.x-b.x;
    const float dz=a.z-b.z;
    return sqrtf(dx*dx+dz*dz);
}

static Color faction_color(NfFaction faction) {
    switch(faction) {
        case NF_FACTION_PLAYER:return(Color){242,244,247,255};
        case NF_FACTION_TEAMMATE:return(Color){46,168,255,255};
        case NF_FACTION_RIVAL:return(Color){214,40,40,255};
        case NF_FACTION_RANCHER:return(Color){242,140,40,255};
        default:return GRAY;
    }
}

static void draw_actor(const NfActor *actor) {
    if(actor==NULL||!actor->active)return;
    const bool cattler=actor->faction==NF_FACTION_RANCHER;
    const float height=cattler?NF_CATTLER_BODY_HEIGHT:1.80f;
    const float width=cattler?NF_CATTLER_BODY_WIDTH:0.80f;
    const Color color=faction_color(actor->faction);
    Vector3 p={actor->transform.position.x,actor->transform.position.y+height*0.5f,actor->transform.position.z};
    DrawCube(p,width,height,width,color);
    DrawCubeWires(p,width,height,width,RAYWHITE);
    if(actor->faction==NF_FACTION_TEAMMATE) {
        DrawSphere((Vector3){p.x,p.y+height*0.62f,p.z},0.16f,color);
    } else if(cattler) {
        DrawCube((Vector3){p.x,actor->transform.position.y+NF_CATTLER_KNEE_BAND_CENTER_Y,p.z},
            NF_CATTLER_WEAK_WIDTH,NF_CATTLER_KNEE_BAND_HEIGHT,NF_CATTLER_WEAK_WIDTH,(Color){255,212,59,240});
        DrawCube((Vector3){p.x,actor->transform.position.y+NF_CATTLER_FOOT_BAND_CENTER_Y,p.z},
            NF_CATTLER_WEAK_WIDTH,NF_CATTLER_FOOT_BAND_HEIGHT,NF_CATTLER_WEAK_WIDTH,(Color){255,212,59,240});
    }
}

static void draw_world(const NfWorld *world,const NfRouteSystem *route) {
    for(size_t i=0u;i<world->collider_count;++i) {
        const NfCollider *c=&world->colliders[i];
        const float cx=0.5f*(c->min.x+c->max.x);
        const float cz=0.5f*(c->min.z+c->max.z);
        if(fabsf(cx)>42.0f||fabsf(cz)>42.0f)continue;
        Vector3 p={cx,0.5f*(c->min.y+c->max.y),cz};
        Color color=(int)i==route->gate_collider?(Color){170,95,185,255}:(Color){105,111,120,255};
        DrawCube(p,c->max.x-c->min.x,c->max.y-c->min.y,c->max.z-c->min.z,color);
        DrawCubeWires(p,c->max.x-c->min.x,c->max.y-c->min.y,c->max.z-c->min.z,GRAY);
    }
    DrawLine3D((Vector3){route->route_a_entry.x,.15f,route->route_a_entry.z},
               (Vector3){route->route_a_reconnect.x,.15f,route->route_a_reconnect.z},LIGHTGRAY);
    DrawLine3D((Vector3){route->route_a_reconnect.x,.15f,route->route_a_reconnect.z},
               (Vector3){route->goal.x,.15f,route->goal.z},LIGHTGRAY);
    DrawLine3D((Vector3){route->route_b_entry.x,.18f,route->route_b_entry.z},
               (Vector3){route->route_b_reconnect.x,.18f,route->route_b_reconnect.z},SKYBLUE);
    DrawLine3D((Vector3){route->route_b_reconnect.x,.18f,route->route_b_reconnect.z},
               (Vector3){route->goal.x,.18f,route->goal.z},SKYBLUE);
    DrawSphere((Vector3){route->goal.x,.3f,route->goal.z},.35f,GOLD);
}

static void place_cattlers_in_slice(NfCattlerSystem *cattlers,NfWorld *world) {
    static const NfVec3 positions[NF_V12C_CATTLERS]={
        {24.0f,0.05f,8.0f},
        {-24.0f,0.05f,14.0f}
    };
    for(size_t i=0u;i<cattlers->count&&i<NF_V12C_CATTLERS;++i) {
        NfCattlerAgent *agent=&cattlers->agents[i];
        NfActor *body=nf_world_find_actor(world,agent->actor_id);
        if(body==NULL)continue;
        body->transform.position=positions[i];
        body->transform.velocity=(NfVec3){0};
        agent->spawn=positions[i];
        const uint8_t region=nf_region_nearest(&cattlers->graph,positions[i]);
        if(region!=NF_REGION_INVALID) {
            agent->current_region=region;
            agent->target_region=region;
            agent->next_region=region;
            agent->home_region=region;
        }
    }
}

static void reset(
    NfWorld *world,NfRouteSystem *route,NfTeammateSystem *teammate,
    NfAiSystem *rivals,NfCattlerSystem *cattlers,NfSemanticBus *semantics,
    NfEntityId *player,NfVec3 *teammate_origin) {
    nf_world_init(world,20260813u);
    nf_world_build_movement_lab(world);
    nf_route_init_v12_slice(route,world);
    nf_semantic_bus_init(semantics);
    *player=nf_world_spawn_actor(world,NF_FACTION_PLAYER,(NfVec3){0,.05f,-18});
    nf_teammate_init(teammate,world,true,world->seed^0x12u);
    nf_ai_init(rivals,world,NF_V12C_RIVALS,world->seed^0xA105u);
    nf_ai_set_rival_relationship(rivals,NF_RELATION_HOSTILE);
    nf_cattler_init(cattlers,world,NF_V12C_CATTLERS,NF_CATTLER_PROFILE_NORMAL,world->seed^0xCA771Eu);
    place_cattlers_in_slice(cattlers,world);
    const NfActor *body=nf_world_find_actor_const(world,teammate->actor_id);
    *teammate_origin=body!=NULL?body->transform.position:(NfVec3){0};
    for(unsigned i=0u;i<3u;++i)nf_world_step(world,1.0f/(float)NF_TICK_RATE);
}

static Camera3D build_raylib_camera(const NfCameraState *camera) {
    if(camera->mode==NF_CAMERA_DEBUG_OBSERVER) {
        return(Camera3D){.position={48,58,-58},.target={0,0,6},.up={0,1,0},.fovy=56,.projection=CAMERA_PERSPECTIVE};
    }
    if(camera->mode==NF_CAMERA_DEBUG_TOP) {
        return(Camera3D){.position={0,78,0.01f},.target={0,0,0},.up={0,0,1},.fovy=50,.projection=CAMERA_PERSPECTIVE};
    }
    const NfVec3 anchor=nf_camera_render_anchor(camera);
    const NfVec3 forward=nf_camera_forward(camera);
    return(Camera3D){
        .position={anchor.x,anchor.y,anchor.z},
        .target={anchor.x+forward.x,anchor.y+forward.y,anchor.z+forward.z},
        .up={0,1,0},.fovy=camera->current_vertical_fov_deg,.projection=CAMERA_PERSPECTIVE};
}

static size_t faction_count(const NfWorld *world,NfFaction faction) {
    size_t count=0u;
    for(size_t i=0u;i<NF_MAX_ENTITIES;++i)if(world->actors[i].active&&world->actors[i].faction==faction)++count;
    return count;
}

static size_t moving_faction_count(const NfWorld *world,NfFaction faction) {
    size_t count=0u;
    for(size_t i=0u;i<NF_MAX_ENTITIES;++i) {
        const NfActor *actor=&world->actors[i];
        if(!actor->active||actor->faction!=faction)continue;
        const float speed=sqrtf(actor->transform.velocity.x*actor->transform.velocity.x+actor->transform.velocity.z*actor->transform.velocity.z);
        if(speed>0.15f)++count;
    }
    return count;
}

int main(void) {
    const int width=1280,height=720;
    InitWindow(width,height,"nightfall!punk v1.2c integrated population / embodied causality");
    SetTargetFPS(120);
    DisableCursor();

    NfWorld world;
    NfRouteSystem route;
    NfTeammateSystem teammate;
    NfAiSystem rivals;
    NfCattlerSystem cattlers;
    NfSemanticBus semantics;
    NfEntityId player=0u;
    NfVec3 teammate_origin={0};
    NfCameraState camera;
    NfControlFrame last_teammate_control={0};
    reset(&world,&route,&teammate,&rivals,&cattlers,&semantics,&player,&teammate_origin);
    nf_camera_init(&camera,82.0f);

    double acc=0.0;
    const double dt=1.0/(double)NF_TICK_RATE;

    while(!WindowShouldClose()) {
        if(IsKeyPressed(KEY_F1)){nf_camera_set_mode(&camera,NF_CAMERA_FIRST_PERSON);DisableCursor();}
        if(IsKeyPressed(KEY_F2)){nf_camera_set_mode(&camera,NF_CAMERA_DEBUG_OBSERVER);EnableCursor();}
        if(IsKeyPressed(KEY_F3)){nf_camera_set_mode(&camera,NF_CAMERA_DEBUG_TOP);EnableCursor();}
        if(IsKeyPressed(KEY_F5)) {
            reset(&world,&route,&teammate,&rivals,&cattlers,&semantics,&player,&teammate_origin);
            camera.initialized=false;
            last_teammate_control=(NfControlFrame){0};
        }

        const double raw_frame=GetFrameTime();
        const double frame=raw_frame>0.1?0.1:raw_frame;
        if(camera.mode==NF_CAMERA_FIRST_PERSON) {
            const Vector2 mouse=GetMouseDelta();
            nf_camera_apply_look_delta(&camera,mouse.x,mouse.y,0.0022f);
        }

        NfMoveInput player_input={0};
        if(camera.mode==NF_CAMERA_FIRST_PERSON) {
            player_input.forward=(IsKeyDown(KEY_W)?1.0f:0.0f)-(IsKeyDown(KEY_S)?1.0f:0.0f);
            player_input.strafe=(IsKeyDown(KEY_A)?1.0f:0.0f)-(IsKeyDown(KEY_D)?1.0f:0.0f);
            player_input.sprint_held=IsKeyDown(KEY_LEFT_SHIFT);
            player_input.interact_held=IsKeyDown(KEY_E);
            player_input.jump_pressed=IsKeyPressed(KEY_SPACE);
            player_input.crouch_held=IsKeyDown(KEY_LEFT_CONTROL);
            player_input.yaw_radians=camera.yaw;
        }

        acc+=frame;
        while(acc>=dt) {
            nf_world_set_input(&world,player,player_input);
            const bool changed=nf_route_try_alter(&route,&world,player);
            if(changed) {
                nf_semantic_bus_publish(&semantics,(NfSemanticAlert){
                    .type=NF_SEMANTIC_ROUTE_CHANGED,.source=player,.subject=player,
                    .position=route.gate_center,.intensity=1.0f,.radius=36.0f,
                    .tick=world.tick,.lifetime_ticks=NF_TICK_RATE*2u});
            }

            NfControlFrame teammate_control={0};
            if(nf_teammate_tick(&teammate,&world,&route,&teammate_control,1u)>0u) {
                last_teammate_control=teammate_control;
                nf_world_set_input(&world,teammate.actor_id,teammate_control.move);
            }

            NfControlFrame rival_controls[NF_AI_MAX_AGENTS]={{0}};
            const size_t rival_controls_count=nf_ai_tick(&rivals,&world,&semantics,rival_controls,NF_AI_MAX_AGENTS);
            for(size_t i=0u;i<rival_controls_count;++i)nf_world_set_input(&world,rival_controls[i].actor,rival_controls[i].move);

            NfControlFrame cattler_controls[NF_CATTLER_MAX_AGENTS]={{0}};
            const size_t cattler_controls_count=nf_cattler_tick(&cattlers,&world,&semantics,cattler_controls,NF_CATTLER_MAX_AGENTS);
            for(size_t i=0u;i<cattler_controls_count;++i)nf_world_set_input(&world,cattler_controls[i].actor,cattler_controls[i].move);

            nf_world_step(&world,(float)dt);
            player_input.jump_pressed=false;
            acc-=dt;
        }

        const NfActor *player_body=nf_world_find_actor_const(&world,player);
        const NfActor *teammate_body=nf_world_find_actor_const(&world,teammate.actor_id);
        if(player_body!=NULL) {
            nf_camera_follow_actor(&camera,player_body,&world,(float)frame);
            nf_camera_step_presentation(&camera,(float)frame);
        }
        const Camera3D ray_camera=build_raylib_camera(&camera);

        BeginDrawing();
        ClearBackground((Color){18,20,24,255});
        BeginMode3D(ray_camera);
        DrawGrid(80,1);
        draw_world(&world,&route);
        for(size_t i=0u;i<NF_MAX_ENTITIES;++i) {
            const NfActor *actor=&world.actors[i];
            if(!actor->active)continue;
            if(camera.mode==NF_CAMERA_FIRST_PERSON&&actor->id==player)continue;
            draw_actor(actor);
        }
        EndMode3D();

        char route_b_utility[32];
        if(route.open)snprintf(route_b_utility,sizeof(route_b_utility),"%.2f",teammate.trace.route_b_utility);
        else snprintf(route_b_utility,sizeof(route_b_utility),"BLOCKED");

        const float hfov=nf_camera_horizontal_fov_deg(&camera,(float)width/(float)height);
        const float gate_distance=player_body?nf_route_distance_to_gate(&route,player_body->transform.position):0.0f;
        const bool e_held=camera.mode==NF_CAMERA_FIRST_PERSON&&IsKeyDown(KEY_E);
        const bool manipulator_ready=player_body!=NULL&&nf_contamination_manipulator_scale(player_body)>=0.35f;
        const bool affordance_ready=!route.open&&gate_distance<=NF_V12_ROUTE_INTERACT_RADIUS&&manipulator_ready;
        const float teammate_speed=teammate_body?sqrtf(teammate_body->transform.velocity.x*teammate_body->transform.velocity.x+teammate_body->transform.velocity.z*teammate_body->transform.velocity.z):0.0f;
        const float teammate_displacement=teammate_body?distance_xz(teammate_origin,teammate_body->transform.position):0.0f;
        const float teammate_control=fabsf(last_teammate_control.move.forward)+fabsf(last_teammate_control.move.strafe);

        DrawText("nightfall!punk v1.2c — INTEGRATED POPULATION / EMBODIED CAUSALITY",20,16,21,RAYWHITE);
        DrawText("F1 PLAYER | F2 OBSERVER/LIVE | F3 TOP/LIVE | F5 reset | WASD Shift Ctrl Space | hold E at purple gate",20,43,15,GRAY);
        DrawText(TextFormat("POP PLAYER %zu | TEAMMATE %zu | RIVAL %zu (%zu moving) | CATTLER %zu (%zu moving)",
            faction_count(&world,NF_FACTION_PLAYER),faction_count(&world,NF_FACTION_TEAMMATE),
            faction_count(&world,NF_FACTION_RIVAL),moving_faction_count(&world,NF_FACTION_RIVAL),
            faction_count(&world,NF_FACTION_RANCHER),moving_faction_count(&world,NF_FACTION_RANCHER)),20,66,16,RAYWHITE);
        DrawText(TextFormat("CAMERA %s | %s | anchor %.3fm | FOV %.1fV / %.1fH",
            nf_camera_mode_name(camera.mode),nf_camera_correction_name(camera.correction_class),camera.anchor_error,
            camera.current_vertical_fov_deg,hfov),20,89,16,camera.mode==NF_CAMERA_FIRST_PERSON?SKYBLUE:GOLD);
        DrawText(TextFormat("ALTER_ROUTE %s | gate %.1fm | E %s | AFFORDANCE %s",
            nf_route_state_name(&route),gate_distance,e_held?"HELD":"UP",route.open?"CONSUMED":(affordance_ready?"READY":"NOT_READY")),
            20,112,17,affordance_ready?GREEN:(route.open?SKYBLUE:(Color){200,170,210,255}));
        DrawText(TextFormat("TEAMMATE CONTROL %.2f | SPEED %.2fm/s | DISPLACEMENT %.2fm | OCCUPANCY %s",
            teammate_control,teammate_speed,teammate_displacement,teammate.trace.yielded_for_occupancy?"YIELD":"CLEAR"),20,136,16,
            teammate_displacement>1.0f?GREEN:YELLOW);
        DrawText(TextFormat("EVIDENCE %s | BELIEF %s | GRADIENT %.2f | COMMIT %s",
            nf_teammate_evidence_name(teammate.trace.evidence),teammate.memory.known_open?"OPEN":"UNCHANGED",
            teammate.trace.hotspot_gradient,nf_teammate_commitment_name(teammate.commitment)),20,159,17,(Color){46,168,255,255});
        DrawText(TextFormat("ENERGY A %.1f B %.1f | UTILITY A %.2f B %s",
            teammate.trace.route_a_energy,teammate.trace.route_b_energy,teammate.trace.route_a_utility,route_b_utility),20,182,16,RAYWHITE);
        DrawText("CAUSAL CHAIN: EVIDENCE -> BELIEF -> UTILITY -> COMMITMENT -> CONTROL -> BODY DISPLACEMENT -> WORLD RELATION",20,205,15,GOLD);
        DrawText("white Player | chrome-blue teammate | red Human Rivals | orange/yellow Dream Cattlers",20,228,15,GRAY);
        DrawText("v1.2c proof: camera retained; living population restored; AI action is not accepted until bodies actually move",20,688,14,GRAY);
        EndDrawing();
    }

    EnableCursor();
    CloseWindow();
    return 0;
}
