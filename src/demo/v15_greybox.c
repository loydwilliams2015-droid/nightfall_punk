#include "nf_ai.h"
#include "nf_camera.h"
#include "nf_cattler.h"
#include "nf_claim.h"
#include "nf_combat.h"
#include "nf_comm_present.h"
#include "nf_contamination.h"
#include "nf_metabolic.h"
#include "nf_region.h"
#include "nf_route.h"
#include "nf_semantics.h"
#include "nf_teammate.h"
#include "nf_world.h"
#include "raylib.h"

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define NF_V15_RIVALS 2u
#define NF_V15_CATTLERS 2u

typedef struct V15Demo {
    NfWorld world;
    NfRouteSystem route;
    NfTeammateSystem teammate;
    NfAiSystem rivals;
    NfCattlerSystem cattlers;
    NfSemanticBus semantics;
    NfClaimTable claims;
    NfRegionGraph graph;
    NfMetabolicSystem metabolic;
    NfCommPresentBus comm;
    NfEntityId player;
    uint32_t metabolic_key;
    uint32_t player_fire_sequence;
    float hit_confirm;
    bool last_hit;
    bool last_head;
    float focus_amount;
    float weapon_x;
    float weapon_y;
    float weapon_vx;
    float weapon_vy;
} V15Demo;

static float clamp01f(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

static Color faction_color(NfFaction faction) {
    switch (faction) {
        case NF_FACTION_PLAYER: return (Color){242,244,247,255};
        case NF_FACTION_TEAMMATE: return (Color){46,168,255,255};
        case NF_FACTION_RIVAL: return (Color){224,66,74,255};
        case NF_FACTION_CATTLER: return (Color){242,140,40,255};
        default: return GRAY;
    }
}

static void draw_actor(const NfActor *actor) {
    if (actor == NULL || !actor->active) return;
    const bool cattler = actor->faction == NF_FACTION_CATTLER;
    const float height = cattler ? 2.30f : 1.80f;
    const float width = cattler ? 1.05f : 0.80f;
    const Color color = faction_color(actor->faction);
    const Vector3 p = {
        actor->transform.position.x,
        actor->transform.position.y+height*0.5f,
        actor->transform.position.z
    };
    DrawCube(p, width, height, width, color);
    DrawCubeWires(p, width, height, width, RAYWHITE);
    if (actor->faction == NF_FACTION_TEAMMATE) {
        DrawSphere((Vector3){p.x,p.y+height*0.62f,p.z}, 0.16f, color);
    }
    if (cattler) {
        DrawCube((Vector3){p.x,actor->transform.position.y+0.62f,p.z},
            0.86f,0.26f,0.86f,(Color){255,212,59,240});
    }
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

static NfVec3 direction_from_angles(float yaw, float pitch) {
    const float cp = cosf(pitch);
    return (NfVec3){sinf(yaw)*cp,sinf(pitch),cosf(yaw)*cp};
}

static NfEntityId hitscan(
    const NfWorld *world, NfEntityId shooter, NfFaction shooter_faction,
    NfVec3 origin, NfVec3 direction, float range,
    NfHitZone *zone_out, NfVec3 *hit_out) {
    float block_distance = range;
    for (size_t i = 0u; i < world->collider_count; ++i) {
        const NfCollider *collider = &world->colliders[i];
        if (collider->kind == NF_COLLIDER_LADDER) continue;
        float hit = 0.0f;
        if (ray_aabb(origin,direction,collider->min,collider->max,&hit) &&
            hit > 0.04f && hit < block_distance) block_distance = hit;
    }

    NfEntityId best = 0u;
    float best_distance = block_distance;
    NfHitZone best_zone = NF_HIT_NONE;
    for (size_t i = 0u; i < NF_MAX_ENTITIES; ++i) {
        const NfActor *actor = &world->actors[i];
        if (!actor->active || !actor->combat.alive || actor->id == shooter) continue;
        if (!nf_combat_can_damage(shooter_faction, actor->faction, false)) continue;
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
            best = actor->id;
            best_distance = t;
            best_zone = NF_HIT_HEAD;
        } else if (ray_sphere(origin,direction,body,0.55f,&t) && t < best_distance) {
            best = actor->id;
            best_distance = t;
            best_zone = NF_HIT_BODY;
        }
    }
    if (best != 0u) {
        if (zone_out != NULL) *zone_out = best_zone;
        if (hit_out != NULL) {
            *hit_out = (NfVec3){
                origin.x+direction.x*best_distance,
                origin.y+direction.y*best_distance,
                origin.z+direction.z*best_distance
            };
        }
    }
    return best;
}

static void publish_combat_semantic(V15Demo *demo, NfCombatEvent event) {
    NfSemanticAlert alert = nf_semantic_from_combat(&event);
    if (alert.type != NF_SEMANTIC_NONE) nf_semantic_bus_publish(&demo->semantics, alert);
}

static void fire_actor(
    V15Demo *demo, NfActor *shooter, const NfCombatInput *input,
    uint32_t sequence, float yaw, float pitch, bool player_shot) {
    if (shooter == NULL || input == NULL) return;
    NfCombatEvent fire = {0};
    if (!nf_combat_try_fire(shooter,input,sequence,demo->world.tick,&fire)) return;
    fire.position = (NfVec3){
        shooter->transform.position.x,
        shooter->transform.position.y+shooter->movement.eye_height,
        shooter->transform.position.z
    };
    publish_combat_semantic(demo, fire);
    const NfWeaponSpec *spec = nf_weapon_spec(shooter->combat.weapon);
    if (spec == NULL) return;
    NfHitZone zone = NF_HIT_NONE;
    NfVec3 point = {0};
    const NfVec3 direction = direction_from_angles(yaw,pitch);
    const NfEntityId hit = hitscan(
        &demo->world,shooter->id,shooter->faction,
        fire.position,direction,spec->range,&zone,&point);
    if (hit == 0u) {
        if (player_shot) {
            demo->hit_confirm = 0.0f;
            demo->last_hit = false;
            demo->last_head = false;
        }
        return;
    }
    NfActor *target = nf_world_find_actor(&demo->world, hit);
    if (target == NULL) return;
    const float amount = spec->damage*(zone == NF_HIT_HEAD ? spec->head_multiplier : 1.0f);
    NfCombatEvent damage = {0};
    if (nf_combat_apply_damage(
            target,shooter->id,shooter->combat.weapon,zone,amount,
            demo->world.tick,&damage)) {
        damage.position = point;
        publish_combat_semantic(demo, damage);
        if (player_shot) {
            demo->hit_confirm = 0.16f;
            demo->last_hit = true;
            demo->last_head = zone == NF_HIT_HEAD;
        }
    }
}

static void place_cattlers(V15Demo *demo) {
    static const NfVec3 positions[NF_V15_CATTLERS] = {
        {24.0f,0.05f,8.0f},{-24.0f,0.05f,14.0f}
    };
    for (size_t i = 0u; i < demo->cattlers.count && i < NF_V15_CATTLERS; ++i) {
        NfCattlerAgent *agent = &demo->cattlers.agents[i];
        NfActor *body = nf_world_find_actor(&demo->world, agent->actor_id);
        if (body == NULL) continue;
        body->transform.position = positions[i];
        body->transform.velocity = (NfVec3){0};
        agent->spawn = positions[i];
    }
}

static void reset_demo(V15Demo *demo) {
    nf_world_init(&demo->world, 20260815u);
    nf_world_build_movement_lab(&demo->world);
    nf_route_init_v12_slice(&demo->route, &demo->world);
    nf_semantic_bus_init(&demo->semantics);
    nf_claim_table_init(&demo->claims);
    nf_comm_present_init(&demo->comm);
    nf_region_graph_init_spatial_lab(&demo->graph);
    nf_energy_bind_spatial_lab(&demo->world.energy, &demo->graph);
    nf_metabolic_init(&demo->metabolic);
    demo->metabolic_key = nf_metabolic_bind(&demo->metabolic, 2u, 14u);
    demo->player = nf_world_spawn_actor(
        &demo->world,NF_FACTION_PLAYER,(NfVec3){0.0f,0.05f,-18.0f});
    nf_teammate_init(&demo->teammate,&demo->world,true,demo->world.seed^0x15u);
    nf_ai_init(&demo->rivals,&demo->world,NF_V15_RIVALS,demo->world.seed^0xA115u);
    nf_ai_bind_claims(&demo->rivals,&demo->claims);
    nf_ai_set_rival_relationship(&demo->rivals,NF_RELATION_HOSTILE);
    nf_cattler_init(&demo->cattlers,&demo->world,NF_V15_CATTLERS,
        NF_CATTLER_PROFILE_NORMAL,demo->world.seed^0xCA771Eu);
    place_cattlers(demo);
    demo->player_fire_sequence = 0u;
    demo->hit_confirm = 0.0f;
    demo->last_hit = false;
    demo->last_head = false;
    demo->focus_amount = 0.0f;
    demo->weapon_x = demo->weapon_y = 0.0f;
    demo->weapon_vx = demo->weapon_vy = 0.0f;
    for (unsigned i = 0u; i < 3u; ++i) {
        nf_world_step(&demo->world, 1.0f/(float)NF_TICK_RATE);
    }
}

static Camera3D ray_camera(const NfCameraState *camera, float focus) {
    if (camera->mode == NF_CAMERA_DEBUG_OBSERVER) {
        return (Camera3D){.position={48,58,-58},.target={0,0,6},
            .up={0,1,0},.fovy=56,.projection=CAMERA_PERSPECTIVE};
    }
    if (camera->mode == NF_CAMERA_DEBUG_TOP) {
        return (Camera3D){.position={0,78,0.01f},.target={0,0,0},
            .up={0,0,1},.fovy=50,.projection=CAMERA_PERSPECTIVE};
    }
    const NfVec3 anchor = nf_camera_render_anchor(camera);
    const NfVec3 forward = nf_camera_forward(camera);
    return (Camera3D){
        .position={anchor.x,anchor.y,anchor.z},
        .target={anchor.x+forward.x,anchor.y+forward.y,anchor.z+forward.z},
        .up={0,1,0},
        .fovy=camera->current_vertical_fov_deg-focus*7.0f,
        .projection=CAMERA_PERSPECTIVE
    };
}

static void draw_world(const V15Demo *demo) {
    for (size_t i = 0u; i < demo->world.collider_count; ++i) {
        const NfCollider *c = &demo->world.colliders[i];
        const float cx = 0.5f*(c->min.x+c->max.x);
        const float cz = 0.5f*(c->min.z+c->max.z);
        if (fabsf(cx) > 42.0f || fabsf(cz) > 42.0f) continue;
        const Vector3 p = {cx,0.5f*(c->min.y+c->max.y),cz};
        const Color color = (int)i == demo->route.gate_collider
            ? (Color){170,95,185,255} : (Color){90,99,108,255};
        DrawCube(p,c->max.x-c->min.x,c->max.y-c->min.y,c->max.z-c->min.z,color);
        DrawCubeWires(p,c->max.x-c->min.x,c->max.y-c->min.y,c->max.z-c->min.z,GRAY);
    }
    const NfEnergyRegion *source = nf_energy_region_const(&demo->world.energy,2u);
    const NfEnergyRegion *sink = nf_energy_region_const(&demo->world.energy,14u);
    if (source != NULL && sink != NULL) {
        DrawSphere((Vector3){(float)source->center_x,0.45f,(float)source->center_z},
            0.55f,(Color){65,210,150,255});
        DrawSphere((Vector3){(float)sink->center_x,0.45f,(float)sink->center_z},
            0.55f,(Color){90,150,245,255});
        DrawLine3D(
            (Vector3){(float)source->center_x,0.45f,(float)source->center_z},
            (Vector3){(float)sink->center_x,0.45f,(float)sink->center_z},
            (Color){100,200,170,180});
    }
}

static void update_focus_model(
    V15Demo *demo, const NfActor *player, float dt, bool focus) {
    const float target = focus ? 1.0f : 0.0f;
    demo->focus_amount += (target-demo->focus_amount)*clamp01f(dt*10.0f);
    if (player == NULL) return;
    const float desired_x = -player->transform.velocity.x*1.6f;
    const float desired_y = fabsf(player->transform.velocity.z)*0.35f;
    const float stiffness = focus ? 34.0f : 19.0f;
    const float damping = focus ? 10.5f : 7.0f;
    demo->weapon_vx += (desired_x-demo->weapon_x)*stiffness*dt-
        demo->weapon_vx*damping*dt;
    demo->weapon_vy += (desired_y-demo->weapon_y)*stiffness*dt-
        demo->weapon_vy*damping*dt;
    demo->weapon_x += demo->weapon_vx*dt;
    demo->weapon_y += demo->weapon_vy*dt;
    if (demo->weapon_x > 18.0f) demo->weapon_x = 18.0f;
    if (demo->weapon_x < -18.0f) demo->weapon_x = -18.0f;
    if (demo->weapon_y > 12.0f) demo->weapon_y = 12.0f;
    if (demo->weapon_y < -12.0f) demo->weapon_y = -12.0f;
}

static void draw_viewmodel(
    const V15Demo *demo, const NfActor *player, int width, int height) {
    if (player == NULL) return;
    const float center = (float)width*0.5f+demo->weapon_x;
    const float base = (float)height-74.0f+demo->weapon_y-demo->focus_amount*34.0f;
    DrawRectangle((int)(center-52),(int)(base-18),104,30,(Color){62,76,72,255});
    DrawRectangle((int)(center+28),(int)(base-25),48,12,(Color){116,142,126,255});
    DrawRectangle((int)(center-18),(int)(base+10),28,34,(Color){48,54,52,255});
    DrawRectangleLines((int)(center-52),(int)(base-18),104,30,(Color){176,205,188,255});
    DrawText("ECO-TECH GUARD",(int)(center-48),(int)(base-14),10,(Color){185,220,195,255});
    float spread = player->combat.state == NF_WEAPON_RECOVERING ? 12.0f : 7.0f;
    spread += sqrtf(player->transform.velocity.x*player->transform.velocity.x+
        player->transform.velocity.z*player->transform.velocity.z)*0.35f;
    spread *= 1.0f-demo->focus_amount*0.22f;
    const int x = width/2;
    const int y = height/2;
    const int s = (int)spread;
    DrawLine(x-s-8,y,x-s,y,(Color){220,235,228,210});
    DrawLine(x+s,y,x+s+8,y,(Color){220,235,228,210});
    DrawLine(x,y-s-8,x,y-s,(Color){220,235,228,210});
    DrawLine(x,y+s,x,y+s+8,(Color){220,235,228,210});
    DrawCircle(x,y,2,(Color){230,244,235,220});
}

static void draw_sensory_vignette(
    const NfActor *player, int width, int height) {
    if (player == NULL) return;
    const float sensory = clamp01f(
        player->contamination.sensory+player->contamination.systemic*0.35f);
    if (sensory < 0.08f) return;
    const unsigned char alpha = (unsigned char)(35.0f+sensory*55.0f);
    for (int y = 0; y < height; y += 6) {
        DrawRectangle(0,y,width,1,(Color){80,255,130,(unsigned char)(alpha/3u)});
    }
    const int edge = (int)(20.0f+sensory*70.0f);
    DrawRectangle(0,0,edge,height,(Color){30,230,90,alpha});
    DrawRectangle(width-edge,0,edge,height,(Color){30,230,90,alpha});
    DrawRectangle(0,0,width,edge/2,(Color){30,230,90,(unsigned char)(alpha/2u)});
}

static void publish_ping(V15Demo *demo, const NfCameraState *camera) {
    const NfVec3 anchor = nf_camera_render_anchor(camera);
    const NfVec3 forward = nf_camera_forward(camera);
    const NfVec3 position = {
        anchor.x+forward.x*14.0f,
        anchor.y+forward.y*14.0f,
        anchor.z+forward.z*14.0f
    };
    (void)nf_comm_present_publish(&demo->comm,(NfCommPresentEvent){
        .kind=NF_COMM_MARK_WATCH,.sender=demo->player,.position=position,
        .precision_m=1.0f,.urgency=0.55f,.issued_tick=demo->world.tick,
        .expiry_tick=demo->world.tick+NF_TICK_RATE*4u},NULL);
    if (demo->teammate.actor_id != 0u) {
        (void)nf_comm_present_publish(&demo->comm,(NfCommPresentEvent){
            .kind=NF_COMM_ACK_SEEN,.sender=demo->teammate.actor_id,.position=position,
            .precision_m=1.0f,.urgency=0.45f,.issued_tick=demo->world.tick,
            .expiry_tick=demo->world.tick+NF_TICK_RATE*2u},NULL);
    }
}

static void draw_comm(
    const V15Demo *demo, const Camera3D *camera, int width, int height) {
    NfCommPresentEvent events[8];
    const size_t count = nf_comm_present_collect(
        &demo->comm,demo->world.tick,events,8u);
    for (size_t i = 0u; i < count; ++i) {
        const NfCommPresentEvent *event = &events[i];
        if (event->kind == NF_COMM_ACK_SEEN ||
            event->kind == NF_COMM_ACK_COMMITTED ||
            event->kind == NF_COMM_ACK_REJECTED) {
            DrawText(TextFormat("CREW: %s",nf_comm_kind_name(event->kind)),
                20,height-112-(int)i*18,15,(Color){90,190,255,235});
            continue;
        }
        const Vector2 screen = GetWorldToScreen(
            (Vector3){event->position.x,event->position.y,event->position.z},*camera);
        if (screen.x >= 0 && screen.x < width && screen.y >= 0 && screen.y < height) {
            DrawCircleLines((int)screen.x,(int)screen.y,11,(Color){100,215,190,230});
            DrawText(nf_comm_kind_name(event->kind),(int)screen.x+14,
                (int)screen.y-7,13,(Color){175,235,215,235});
        }
    }
}

int main(void) {
    const int width = 1280;
    const int height = 720;
    InitWindow(width,height,
        "nightfall!punk v1.5 — combat / belief / communication integration");
    SetTargetFPS(120);
    DisableCursor();

    V15Demo demo = {0};
    reset_demo(&demo);
    NfCameraState camera;
    nf_camera_init(&camera,82.0f);
    double accumulator = 0.0;
    const double dt = 1.0/(double)NF_TICK_RATE;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F1)) { nf_camera_set_mode(&camera,NF_CAMERA_FIRST_PERSON); DisableCursor(); }
        if (IsKeyPressed(KEY_F2)) { nf_camera_set_mode(&camera,NF_CAMERA_DEBUG_OBSERVER); EnableCursor(); }
        if (IsKeyPressed(KEY_F3)) { nf_camera_set_mode(&camera,NF_CAMERA_DEBUG_TOP); EnableCursor(); }
        if (IsKeyPressed(KEY_F5)) { reset_demo(&demo); camera.initialized=false; }
        if (IsKeyPressed(KEY_F6)) {
            const double moved = nf_energy_transfer(&demo.world.energy,2u,14u,8.0);
            (void)nf_metabolic_record_outflow(
                &demo.metabolic,demo.metabolic_key,moved,0.5,demo.world.tick);
        }
        if (IsKeyPressed(KEY_F7)) {
            const double moved = nf_energy_transfer(&demo.world.energy,14u,2u,5.0);
            (void)nf_metabolic_record_return(
                &demo.metabolic,demo.metabolic_key,moved,demo.world.tick);
        }
        const bool select_one = IsKeyPressed(KEY_ONE);
        const bool select_two = IsKeyPressed(KEY_TWO);
        const double raw = GetFrameTime();
        const double frame = raw > 0.1 ? 0.1 : raw;
        if (camera.mode == NF_CAMERA_FIRST_PERSON) {
            const Vector2 mouse = GetMouseDelta();
            nf_camera_apply_look_delta(&camera,mouse.x,mouse.y,0.0022f);
            if (IsKeyPressed(KEY_Q)) publish_ping(&demo,&camera);
        }

        NfMoveInput move = {0};
        NfCombatInput combat = {0};
        if (camera.mode == NF_CAMERA_FIRST_PERSON) {
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
        }

        const bool focus = camera.mode == NF_CAMERA_FIRST_PERSON &&
            IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
        const NfActor *player_now = nf_world_find_actor_const(&demo.world,demo.player);
        update_focus_model(&demo,player_now,(float)frame,focus);

        accumulator += frame;
        while (accumulator >= dt) {
            nf_claim_expire(&demo.claims,demo.world.tick);
            nf_world_set_input(&demo.world,demo.player,move);
            NfActor *player = nf_world_find_actor(&demo.world,demo.player);
            if (player != NULL) {
                if (combat.reload_pressed) (void)nf_combat_start_reload(player);
                if (select_one) (void)nf_combat_select_weapon(player,NF_WEAPON_CARBINE);
                if (select_two) (void)nf_combat_select_weapon(player,NF_WEAPON_PISTOL);
                if (combat.fire_held || combat.fire_pressed) {
                    ++demo.player_fire_sequence;
                    fire_actor(&demo,player,&combat,demo.player_fire_sequence,
                        camera.yaw,camera.pitch,true);
                    demo.weapon_vy -= 4.5f;
                    demo.weapon_vx += sinf((float)demo.player_fire_sequence)*1.3f;
                }
            }

            NfControlFrame team_frame = {0};
            if (nf_teammate_tick(
                    &demo.teammate,&demo.world,&demo.route,&team_frame,1u) > 0u) {
                nf_world_set_input(&demo.world,demo.teammate.actor_id,team_frame.move);
            }

            NfControlFrame rival_frames[NF_AI_MAX_AGENTS] = {{0}};
            const size_t rival_count = nf_ai_tick(
                &demo.rivals,&demo.world,&demo.semantics,
                rival_frames,NF_AI_MAX_AGENTS);
            for (size_t i = 0u; i < rival_count; ++i) {
                nf_world_set_input(
                    &demo.world,rival_frames[i].actor,rival_frames[i].move);
                NfActor *rival = nf_world_find_actor(&demo.world,rival_frames[i].actor);
                if (rival == NULL) continue;
                if (rival_frames[i].combat.reload_pressed) {
                    (void)nf_combat_start_reload(rival);
                }
                if (rival_frames[i].combat.fire_held ||
                    rival_frames[i].combat.fire_pressed) {
                    fire_actor(&demo,rival,&rival_frames[i].combat,
                        rival_frames[i].sequence,rival_frames[i].move.yaw_radians,
                        rival_frames[i].combat.aim_pitch_radians,false);
                }
            }

            NfControlFrame cattler_frames[NF_CATTLER_MAX_AGENTS] = {{0}};
            const size_t cattler_count = nf_cattler_tick(
                &demo.cattlers,&demo.world,&demo.semantics,
                cattler_frames,NF_CATTLER_MAX_AGENTS);
            for (size_t i = 0u; i < cattler_count; ++i) {
                nf_world_set_input(
                    &demo.world,cattler_frames[i].actor,cattler_frames[i].move);
            }

            nf_metabolic_tick(
                &demo.metabolic,&demo.world.energy,(double)dt,demo.world.tick);
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
        const Camera3D raylib_camera = ray_camera(&camera,demo.focus_amount);

        BeginDrawing();
        ClearBackground((Color){14,18,20,255});
        BeginMode3D(raylib_camera);
        DrawGrid(80,1);
        draw_world(&demo);
        for (size_t i = 0u; i < NF_MAX_ENTITIES; ++i) {
            const NfActor *actor = &demo.world.actors[i];
            if (!actor->active) continue;
            if (camera.mode == NF_CAMERA_FIRST_PERSON && actor->id == demo.player) continue;
            draw_actor(actor);
        }
        EndMode3D();

        if (camera.mode == NF_CAMERA_FIRST_PERSON) {
            draw_viewmodel(&demo,player,width,height);
            draw_sensory_vignette(player,width,height);
        }
        draw_comm(&demo,&raylib_camera,width,height);

        const NfMetabolicLink *link = nf_metabolic_link_const(
            &demo.metabolic,demo.metabolic_key);
        const NfEnergyRegion *source = nf_energy_region_const(&demo.world.energy,2u);
        DrawText("nightfall!punk v1.5 — VERTICAL INTEGRATION BUILD",20,14,21,RAYWHITE);
        DrawText("WASD Shift Ctrl Space | LMB fire | RMB Focus | R reload | 1/2 weapon | Q WATCH ping | F1/F2/F3 cameras | F5 reset | F6 extract | F7 return",20,40,13,GRAY);
        if (player != NULL) {
            DrawText(TextFormat(
                "WEAPON %s | %u/%u reserve | %s | CONTAM sys %.2f sens %.2f | FOCUS %.2f",
                nf_weapon_name(player->combat.weapon),
                player->combat.ammo_mag[player->combat.weapon],
                player->combat.reserve_ammo[player->combat.weapon],
                nf_weapon_state_name(player->combat.state),
                player->contamination.systemic,player->contamination.sensory,
                demo.focus_amount),20,63,15,RAYWHITE);
        }
        for (size_t i = 0u; i < demo.rivals.count && i < 2u; ++i) {
            const NfAiAgent *agent = &demo.rivals.agents[i];
            const NfTacticalCandidate *top = agent->tactical_candidate_count > 0u
                ? &agent->tactical_candidates[0] : NULL;
            DrawText(TextFormat(
                "RIVAL%u belief=%s id=%u conf=%.2f prec=%.1fm hyp=%u | action=%s score=%.2f | beliefs=%zu",
                (unsigned)(i+1u),nf_information_channel_name(agent->knowledge.channel),
                agent->knowledge.target,agent->knowledge.confidence,
                agent->knowledge.precision_m,agent->knowledge.hypothesis_id,
                top != NULL ? nf_tactical_action_name(top->action) : "NONE",
                top != NULL ? top->score : 0.0f,nf_belief_active_count(&agent->beliefs)),
                20,88+(int)i*23,14,
                i == 0u ? (Color){255,150,150,255} : (Color){255,190,150,255});
        }
        if (link != NULL && source != NULL) {
            DrawText(TextFormat(
                "METABOLIC %s | gap %.2f ratio %.2f dependency %.2f | source productive %.3f | F6 outflow F7 restitution",
                nf_metabolic_state_name(link),link->restitution_gap,
                link->restitution_ratio,link->dependency,source->productive_capacity),
                20,140,14,(Color){120,225,170,255});
        }
        DrawText(TextFormat(
            "REPORT BUS live=%zu | CLAIMS live=%zu | communication: Q marks WATCH; Crew receipt is signal, not command",
            nf_report_live_count(nf_ai_report_bus_const(&demo.rivals),demo.world.tick),
            nf_claim_live_count(&demo.claims,demo.world.tick)),
            20,163,14,(Color){120,190,255,255});
        DrawText("CAUSAL SPINE: COMBAT -> EVIDENCE -> ACTOR-LOCAL BELIEF -> NESTED CANDIDATES -> CONTROL -> BODY -> NEW RELATION",20,188,14,GOLD);
        DrawText("Diagnostic content is rendered continuously for human proof; exact AI math remains debug-only.",20,211,13,GRAY);
        if (demo.hit_confirm > 0.0f && demo.last_hit) {
            const Color hit_color = demo.last_head
                ? (Color){255,225,120,220} : (Color){225,240,230,210};
            DrawCircleLines(width/2,height/2,16,hit_color);
        }
        DrawText("v1.5 proof target: divergent Rival knowledge + embodied tactical response + player-visible combat + bounded restitution",20,height-26,14,GRAY);
        EndDrawing();
    }

    EnableCursor();
    CloseWindow();
    return 0;
}
