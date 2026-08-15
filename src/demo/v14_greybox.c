#include "nf_ai.h"
#include "nf_camera.h"
#include "nf_cattler.h"
#include "nf_claim.h"
#include "nf_contamination.h"
#include "nf_route.h"
#include "nf_semantics.h"
#include "nf_tactical.h"
#include "nf_teammate.h"
#include "nf_world.h"
#include "raylib.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define NF_V14_RIVALS 2u
#define NF_V14_CATTLERS 2u

static float distance_xz(NfVec3 a,NfVec3 b){const float x=a.x-b.x,z=a.z-b.z;return sqrtf(x*x+z*z);}

static Color faction_color(NfFaction faction){
    switch(faction){
        case NF_FACTION_PLAYER:return(Color){242,244,247,255};
        case NF_FACTION_TEAMMATE:return(Color){46,168,255,255};
        case NF_FACTION_RIVAL:return(Color){214,40,40,255};
        case NF_FACTION_RANCHER:return(Color){242,140,40,255};
        default:return GRAY;
    }
}

static void draw_actor(const NfActor *actor){
    if(actor==NULL||!actor->active)return;
    const bool cattler=actor->faction==NF_FACTION_RANCHER;
    const float h=cattler?2.30f:1.80f,w=cattler?1.05f:0.80f;
    const Color color=faction_color(actor->faction);
    const Vector3 p={actor->transform.position.x,actor->transform.position.y+h*0.5f,actor->transform.position.z};
    DrawCube(p,w,h,w,color); DrawCubeWires(p,w,h,w,RAYWHITE);
    if(actor->faction==NF_FACTION_TEAMMATE)DrawSphere((Vector3){p.x,p.y+h*0.62f,p.z},0.16f,color);
    if(cattler){
        DrawCube((Vector3){p.x,actor->transform.position.y+0.62f,p.z},0.86f,0.26f,0.86f,(Color){255,212,59,240});
        DrawCube((Vector3){p.x,actor->transform.position.y+0.18f,p.z},0.90f,0.18f,0.90f,(Color){255,212,59,240});
    }
}

static void draw_world(const NfWorld *world,const NfRouteSystem *route){
    for(size_t i=0u;i<world->collider_count;++i){
        const NfCollider *c=&world->colliders[i];
        const float cx=0.5f*(c->min.x+c->max.x),cz=0.5f*(c->min.z+c->max.z);
        if(fabsf(cx)>42.0f||fabsf(cz)>42.0f)continue;
        const Vector3 p={cx,0.5f*(c->min.y+c->max.y),cz};
        const Color col=(int)i==route->gate_collider?(Color){170,95,185,255}:(Color){105,111,120,255};
        DrawCube(p,c->max.x-c->min.x,c->max.y-c->min.y,c->max.z-c->min.z,col);
        DrawCubeWires(p,c->max.x-c->min.x,c->max.y-c->min.y,c->max.z-c->min.z,GRAY);
    }
    DrawLine3D((Vector3){route->route_a_entry.x,.15f,route->route_a_entry.z},(Vector3){route->route_a_reconnect.x,.15f,route->route_a_reconnect.z},LIGHTGRAY);
    DrawLine3D((Vector3){route->route_a_reconnect.x,.15f,route->route_a_reconnect.z},(Vector3){route->goal.x,.15f,route->goal.z},LIGHTGRAY);
    DrawLine3D((Vector3){route->route_b_entry.x,.18f,route->route_b_entry.z},(Vector3){route->route_b_reconnect.x,.18f,route->route_b_reconnect.z},SKYBLUE);
    DrawLine3D((Vector3){route->route_b_reconnect.x,.18f,route->route_b_reconnect.z},(Vector3){route->goal.x,.18f,route->goal.z},SKYBLUE);
    DrawSphere((Vector3){route->goal.x,.3f,route->goal.z},.35f,GOLD);
}

static void place_cattlers(NfCattlerSystem *cattlers,NfWorld *world){
    static const NfVec3 positions[NF_V14_CATTLERS]={{24.0f,0.05f,8.0f},{-24.0f,0.05f,14.0f}};
    for(size_t i=0u;i<cattlers->count&&i<NF_V14_CATTLERS;++i){
        NfCattlerAgent *agent=&cattlers->agents[i];
        NfActor *body=nf_world_find_actor(world,agent->actor_id);
        if(body==NULL)continue;
        body->transform.position=positions[i]; body->transform.velocity=(NfVec3){0}; agent->spawn=positions[i];
        const uint8_t region=nf_region_nearest(&cattlers->graph,positions[i]);
        if(region!=NF_REGION_INVALID){agent->current_region=region;agent->target_region=region;agent->next_region=region;agent->home_region=region;}
    }
}

static void reset_all(NfWorld *world,NfRouteSystem *route,NfTeammateSystem *teammate,NfAiSystem *rivals,NfCattlerSystem *cattlers,NfSemanticBus *semantics,NfClaimTable *claims,NfEntityId *player,NfVec3 *team_origin){
    nf_world_init(world,20260814u); nf_world_build_movement_lab(world); nf_route_init_v12_slice(route,world); nf_semantic_bus_init(semantics); nf_claim_table_init(claims);
    *player=nf_world_spawn_actor(world,NF_FACTION_PLAYER,(NfVec3){0,.05f,-18});
    nf_teammate_init(teammate,world,true,world->seed^0x14u);
    nf_ai_init(rivals,world,NF_V14_RIVALS,world->seed^0xA114u); nf_ai_set_rival_relationship(rivals,NF_RELATION_HOSTILE);
    nf_cattler_init(cattlers,world,NF_V14_CATTLERS,NF_CATTLER_PROFILE_NORMAL,world->seed^0xCA771Eu); place_cattlers(cattlers,world);
    const NfActor *team=nf_world_find_actor_const(world,teammate->actor_id); *team_origin=team?team->transform.position:(NfVec3){0};
    for(unsigned i=0u;i<3u;++i)nf_world_step(world,1.0f/(float)NF_TICK_RATE);
}

static void proof_setup(NfWorld *world,NfRouteSystem *route,NfTeammateSystem *teammate,NfAiSystem *rivals,NfCattlerSystem *cattlers,NfSemanticBus *semantics,NfClaimTable *claims,NfEntityId *player,NfVec3 *team_origin,NfCameraState *camera){
    reset_all(world,route,teammate,rivals,cattlers,semantics,claims,player,team_origin);
    NfActor *p=nf_world_find_actor(world,*player); NfActor *t=nf_world_find_actor(world,teammate->actor_id);
    if(p&&t){p->transform.position=(NfVec3){0.0f,0.05f,1.4f};p->transform.velocity=(NfVec3){0};t->transform.position=(NfVec3){0.0f,0.05f,-12.0f};t->transform.velocity=(NfVec3){0};*team_origin=t->transform.position;}
    camera->initialized=false; nf_camera_set_mode(camera,NF_CAMERA_FIRST_PERSON); DisableCursor();
}

static Camera3D ray_camera(const NfCameraState *camera){
    if(camera->mode==NF_CAMERA_DEBUG_OBSERVER)return(Camera3D){.position={48,58,-58},.target={0,0,6},.up={0,1,0},.fovy=56,.projection=CAMERA_PERSPECTIVE};
    if(camera->mode==NF_CAMERA_DEBUG_TOP)return(Camera3D){.position={0,78,0.01f},.target={0,0,0},.up={0,0,1},.fovy=50,.projection=CAMERA_PERSPECTIVE};
    const NfVec3 a=nf_camera_render_anchor(camera),f=nf_camera_forward(camera);
    return(Camera3D){.position={a.x,a.y,a.z},.target={a.x+f.x,a.y+f.y,a.z+f.z},.up={0,1,0},.fovy=camera->current_vertical_fov_deg,.projection=CAMERA_PERSPECTIVE};
}

static size_t faction_count(const NfWorld *world,NfFaction faction){size_t n=0u;for(size_t i=0u;i<NF_MAX_ENTITIES;++i)if(world->actors[i].active&&world->actors[i].faction==faction)++n;return n;}
static size_t moving_count(const NfWorld *world,NfFaction faction){size_t n=0u;for(size_t i=0u;i<NF_MAX_ENTITIES;++i){const NfActor *a=&world->actors[i];if(!a->active||a->faction!=faction)continue;const float s=sqrtf(a->transform.velocity.x*a->transform.velocity.x+a->transform.velocity.z*a->transform.velocity.z);if(s>.15f)++n;}return n;}

int main(void){
    const int width=1280,height=720; InitWindow(width,height,"nightfall!punk v1.4 high-medium systems integration"); SetTargetFPS(120); DisableCursor();
    NfWorld world; NfRouteSystem route; NfTeammateSystem teammate; NfAiSystem rivals; NfCattlerSystem cattlers; NfSemanticBus semantics; NfClaimTable claims; NfEntityId player=0u; NfVec3 team_origin={0}; NfCameraState camera; NfControlFrame last_team={0}; NfClaimResult claim_result=NF_CLAIM_RESULT_INVALID;
    reset_all(&world,&route,&teammate,&rivals,&cattlers,&semantics,&claims,&player,&team_origin); nf_camera_init(&camera,82.0f);
    double acc=0.0; const double dt=1.0/(double)NF_TICK_RATE;
    while(!WindowShouldClose()){
        if(IsKeyPressed(KEY_F1)){nf_camera_set_mode(&camera,NF_CAMERA_FIRST_PERSON);DisableCursor();}
        if(IsKeyPressed(KEY_F2)){nf_camera_set_mode(&camera,NF_CAMERA_DEBUG_OBSERVER);EnableCursor();}
        if(IsKeyPressed(KEY_F3)){nf_camera_set_mode(&camera,NF_CAMERA_DEBUG_TOP);EnableCursor();}
        if(IsKeyPressed(KEY_F4)){proof_setup(&world,&route,&teammate,&rivals,&cattlers,&semantics,&claims,&player,&team_origin,&camera);last_team=(NfControlFrame){0};claim_result=NF_CLAIM_RESULT_INVALID;}
        if(IsKeyPressed(KEY_F5)){reset_all(&world,&route,&teammate,&rivals,&cattlers,&semantics,&claims,&player,&team_origin);camera.initialized=false;last_team=(NfControlFrame){0};claim_result=NF_CLAIM_RESULT_INVALID;}
        const double raw=GetFrameTime(),frame=raw>.1?.1:raw;
        if(camera.mode==NF_CAMERA_FIRST_PERSON){const Vector2 m=GetMouseDelta();nf_camera_apply_look_delta(&camera,m.x,m.y,.0022f);}
        NfMoveInput input={0};
        if(camera.mode==NF_CAMERA_FIRST_PERSON){input.forward=(IsKeyDown(KEY_W)?1.0f:0.0f)-(IsKeyDown(KEY_S)?1.0f:0.0f);input.strafe=(IsKeyDown(KEY_A)?1.0f:0.0f)-(IsKeyDown(KEY_D)?1.0f:0.0f);input.sprint_held=IsKeyDown(KEY_LEFT_SHIFT);input.interact_held=IsKeyDown(KEY_E);input.jump_pressed=IsKeyPressed(KEY_SPACE);input.crouch_held=IsKeyDown(KEY_LEFT_CONTROL);input.yaw_radians=camera.yaw;}
        acc+=frame;
        while(acc>=dt){
            nf_claim_expire(&claims,world.tick); nf_world_set_input(&world,player,input);
            const NfActor *p=nf_world_find_actor_const(&world,player);
            const bool ready=p&&!route.open&&input.interact_held&&nf_route_distance_to_gate(&route,p->transform.position)<=NF_V12_ROUTE_INTERACT_RADIUS&&nf_contamination_manipulator_scale(p)>=.35f;
            if(ready)claim_result=nf_claim_try_acquire(&claims,NF_CLAIM_INTERACTION,NF_CLAIM_HARD,player,route.gate_key,world.tick,NF_TICK_RATE/3u,NULL);
            const bool changed=nf_route_try_alter(&route,&world,player);
            if(changed){(void)nf_claim_release(&claims,NF_CLAIM_INTERACTION,player,route.gate_key);nf_semantic_bus_publish(&semantics,(NfSemanticAlert){.type=NF_SEMANTIC_ROUTE_CHANGED,.source=player,.subject=player,.position=route.gate_center,.intensity=1.0f,.radius=36.0f,.tick=world.tick,.lifetime_ticks=NF_TICK_RATE*2u});}
            NfControlFrame team_frame={0};if(nf_teammate_tick(&teammate,&world,&route,&team_frame,1u)>0u){last_team=team_frame;nf_world_set_input(&world,teammate.actor_id,team_frame.move);}
            NfControlFrame rc[NF_AI_MAX_AGENTS]={{0}};const size_t rn=nf_ai_tick(&rivals,&world,&semantics,rc,NF_AI_MAX_AGENTS);for(size_t i=0u;i<rn;++i)nf_world_set_input(&world,rc[i].actor,rc[i].move);
            NfControlFrame cc[NF_CATTLER_MAX_AGENTS]={{0}};const size_t cn=nf_cattler_tick(&cattlers,&world,&semantics,cc,NF_CATTLER_MAX_AGENTS);for(size_t i=0u;i<cn;++i)nf_world_set_input(&world,cc[i].actor,cc[i].move);
            nf_world_step(&world,(float)dt);input.jump_pressed=false;acc-=dt;
        }
        const NfActor *p=nf_world_find_actor_const(&world,player),*t=nf_world_find_actor_const(&world,teammate.actor_id);
        if(p){nf_camera_follow_actor(&camera,p,&world,(float)frame);nf_camera_step_presentation(&camera,(float)frame);} const Camera3D cam=ray_camera(&camera);
        BeginDrawing();ClearBackground((Color){18,20,24,255});BeginMode3D(cam);DrawGrid(80,1);draw_world(&world,&route);for(size_t i=0u;i<NF_MAX_ENTITIES;++i){const NfActor *a=&world.actors[i];if(!a->active)continue;if(camera.mode==NF_CAMERA_FIRST_PERSON&&a->id==player)continue;draw_actor(a);}EndMode3D();
        const float gate_distance=p?nf_route_distance_to_gate(&route,p->transform.position):0.0f;const bool e=camera.mode==NF_CAMERA_FIRST_PERSON&&IsKeyDown(KEY_E);const bool affordance=p&&!route.open&&gate_distance<=NF_V12_ROUTE_INTERACT_RADIUS&&nf_contamination_manipulator_scale(p)>=.35f;
        const float team_speed=t?sqrtf(t->transform.velocity.x*t->transform.velocity.x+t->transform.velocity.z*t->transform.velocity.z):0.0f;const float team_disp=t?distance_xz(team_origin,t->transform.position):0.0f;const float team_control=fabsf(last_team.move.forward)+fabsf(last_team.move.strafe);
        const NfVec3 threat=p?p->transform.position:(NfVec3){0};const float exp_a=nf_tactical_exposure(&world,threat,route.route_a_entry),exp_b=nf_tactical_exposure(&world,threat,route.route_b_reconnect);const float util_a=nf_tactical_cover_utility(exp_a,8.0f,12.0f),util_b=nf_tactical_cover_utility(exp_b,12.0f,18.0f);
        char b_util[24];if(route.open)snprintf(b_util,sizeof(b_util),"%.2f",teammate.trace.route_b_utility);else snprintf(b_util,sizeof(b_util),"BLOCKED");
        DrawText("nightfall!punk v1.4 — HIGH/MEDIUM SYSTEMS INTEGRATION",20,14,21,RAYWHITE);
        DrawText("F1 PLAYER | F2 OBSERVER | F3 TOP | F4 PROOF SETUP | F5 RESET | WASD Shift Ctrl Space | hold E at gate",20,40,14,GRAY);
        DrawText(TextFormat("POP P %zu | TEAM %zu | RIVAL %zu (%zu moving) | CATTLER %zu (%zu moving)",faction_count(&world,NF_FACTION_PLAYER),faction_count(&world,NF_FACTION_TEAMMATE),faction_count(&world,NF_FACTION_RIVAL),moving_count(&world,NF_FACTION_RIVAL),faction_count(&world,NF_FACTION_RANCHER),moving_count(&world,NF_FACTION_RANCHER)),20,63,15,RAYWHITE);
        DrawText(TextFormat("OBJECT gate key=%u rev=%u | %s | %.1fm | E %s | AFFORDANCE %s",route.gate_key,route.revision,nf_route_state_name(&route),gate_distance,e?"HELD":"UP",route.open?"CONSUMED":(affordance?"READY":"NOT_READY")),20,86,16,affordance?GREEN:(route.open?SKYBLUE:(Color){200,170,210,255}));
        DrawText(TextFormat("TEMPORAL CLAIM interaction=%s live=%zu | CAMERA %s anchor=%.3fm",nf_claim_result_name(claim_result),nf_claim_live_count(&claims,world.tick),nf_camera_mode_name(camera.mode),camera.anchor_error),20,109,15,GOLD);
        DrawText(TextFormat("TEAM control %.2f speed %.2fm/s displacement %.2fm | EVIDENCE %s | BELIEF %s | COMMIT %s",team_control,team_speed,team_disp,nf_teammate_evidence_name(teammate.trace.evidence),teammate.memory.known_open?"OPEN":"UNCHANGED",nf_teammate_commitment_name(teammate.commitment)),20,132,15,team_disp>1.0f?GREEN:YELLOW);
        DrawText(TextFormat("TEAM gradient %.2f | utility A %.2f B %s | energy A %.1f B %.1f",teammate.trace.hotspot_gradient,teammate.trace.route_a_utility,b_util,teammate.trace.route_a_energy,teammate.trace.route_b_energy),20,155,15,RAYWHITE);
        DrawText(TextFormat("TACTICAL GRADIENT exposure A %.2f B %.2f | option utility A %.2f B %.2f",exp_a,exp_b,util_a,util_b),20,178,15,(Color){255,180,120,255});
        DrawText("CAUSAL SPINE: OBJECT REVISION -> AFFORDANCE -> LEGITIMATE EVIDENCE -> UTILITY -> CLAIM -> CONTROL -> BODY -> NEW RELATION",20,204,14,GOLD);
        DrawText("v1.4 keeps communication/combat expansion for v1.5; this proof integrates high/medium relations without a new authority",20,688,14,GRAY);
        EndDrawing();
    }
    EnableCursor();CloseWindow();return 0;
}
