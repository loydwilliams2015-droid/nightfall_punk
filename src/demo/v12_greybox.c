#include "nf_route.h"
#include "nf_teammate.h"
#include "nf_world.h"
#include "raylib.h"

#include <math.h>
#include <stdbool.h>

static void draw_actor(const NfActor *a,Color c){
    if(a==NULL)return;
    Vector3 p={a->transform.position.x,a->transform.position.y+0.9f,a->transform.position.z};
    DrawCube(p,0.8f,1.8f,0.8f,c);DrawCubeWires(p,0.8f,1.8f,0.8f,RAYWHITE);
    if(a->faction==NF_FACTION_TEAMMATE)DrawSphere((Vector3){p.x,p.y+1.15f,p.z},0.16f,c);
}
static void draw_world(const NfWorld *w,const NfRouteSystem *r){
    for(size_t i=0;i<w->collider_count;++i){
        const NfCollider *c=&w->colliders[i];float cx=0.5f*(c->min.x+c->max.x),cz=0.5f*(c->min.z+c->max.z);
        if(fabsf(cx)>42.0f||fabsf(cz)>42.0f)continue;
        Vector3 p={cx,0.5f*(c->min.y+c->max.y),cz};Color color=(int)i==r->gate_collider?(Color){170,95,185,255}:(Color){105,111,120,255};
        DrawCube(p,c->max.x-c->min.x,c->max.y-c->min.y,c->max.z-c->min.z,color);DrawCubeWires(p,c->max.x-c->min.x,c->max.y-c->min.y,c->max.z-c->min.z,GRAY);
    }
    DrawLine3D((Vector3){r->route_a_entry.x,.15f,r->route_a_entry.z},(Vector3){r->route_a_reconnect.x,.15f,r->route_a_reconnect.z},LIGHTGRAY);
    DrawLine3D((Vector3){r->route_a_reconnect.x,.15f,r->route_a_reconnect.z},(Vector3){r->goal.x,.15f,r->goal.z},LIGHTGRAY);
    DrawLine3D((Vector3){r->route_b_entry.x,.18f,r->route_b_entry.z},(Vector3){r->route_b_reconnect.x,.18f,r->route_b_reconnect.z},SKYBLUE);
    DrawLine3D((Vector3){r->route_b_reconnect.x,.18f,r->route_b_reconnect.z},(Vector3){r->goal.x,.18f,r->goal.z},SKYBLUE);
    DrawSphere((Vector3){r->goal.x,.3f,r->goal.z},.35f,GOLD);
}
static void reset(NfWorld *w,NfRouteSystem *r,NfTeammateSystem *t,NfEntityId *player){
    nf_world_init(w,20260813u);nf_world_build_movement_lab(w);nf_route_init_v12_slice(r,w);
    *player=nf_world_spawn_actor(w,NF_FACTION_PLAYER,(NfVec3){0,.05f,-18});nf_teammate_init(t,w,true,w->seed^0x12u);
    for(unsigned i=0;i<3u;++i)nf_world_step(w,1.0f/(float)NF_TICK_RATE);
}
int main(void){
    InitWindow(1280,720,"nightfall!punk v1.2 causal greybox");SetTargetFPS(120);
    NfWorld world;NfRouteSystem route;NfTeammateSystem teammate;NfEntityId player=0;reset(&world,&route,&teammate,&player);
    Camera3D cam={.position={48,58,-58},.target={0,0,6},.up={0,1,0},.fovy=56,.projection=CAMERA_PERSPECTIVE};
    double acc=0.0;const double dt=1.0/(double)NF_TICK_RATE;
    while(!WindowShouldClose()){
        if(IsKeyPressed(KEY_F5))reset(&world,&route,&teammate,&player);
        NfMoveInput in={0};in.forward=(IsKeyDown(KEY_W)?1.0f:0.0f)-(IsKeyDown(KEY_S)?1.0f:0.0f);in.strafe=(IsKeyDown(KEY_A)?1.0f:0.0f)-(IsKeyDown(KEY_D)?1.0f:0.0f);in.sprint_held=IsKeyDown(KEY_LEFT_SHIFT);in.interact_held=IsKeyDown(KEY_E);in.jump_pressed=IsKeyPressed(KEY_SPACE);
        double frame=GetFrameTime();if(frame>.1)frame=.1;acc+=frame;
        while(acc>=dt){
            nf_world_set_input(&world,player,in);(void)nf_route_try_alter(&route,&world,player);
            NfControlFrame ai={0};if(nf_teammate_tick(&teammate,&world,&route,&ai,1u)>0u)nf_world_set_input(&world,teammate.actor_id,ai.move);
            nf_world_step(&world,(float)dt);in.jump_pressed=false;acc-=dt;
        }
        const NfActor *p=nf_world_find_actor_const(&world,player),*a=nf_world_find_actor_const(&world,teammate.actor_id);
        BeginDrawing();ClearBackground((Color){18,20,24,255});BeginMode3D(cam);DrawGrid(80,1);draw_world(&world,&route);draw_actor(p,RAYWHITE);draw_actor(a,(Color){46,168,255,255});EndMode3D();
        DrawText("nightfall!punk v1.2 — TEAMMATE CAUSALITY / KINETIC STIGMERGY",20,18,22,RAYWHITE);
        DrawText("WASD move | Shift sprint | hold E near purple ALTER_ROUTE gate | F5 reset",20,48,16,GRAY);
        DrawText(TextFormat("ALTER_ROUTE %s | gate %.1fm",nf_route_state_name(&route),p?nf_route_distance_to_gate(&route,p->transform.position):0.0f),20,76,18,route.open?SKYBLUE:(Color){200,170,210,255});
        DrawText(TextFormat("EVIDENCE %s | BELIEF %s | GRADIENT %.2f | COMMIT %s",nf_teammate_evidence_name(teammate.trace.evidence),teammate.memory.known_open?"OPEN":"UNCHANGED",teammate.trace.hotspot_gradient,nf_teammate_commitment_name(teammate.commitment)),20,102,18,(Color){46,168,255,255});
        DrawText(TextFormat("ENERGY A %.1f B %.1f | UTILITY A %.2f B %.2f | OCCUPANCY %s",teammate.trace.route_a_energy,teammate.trace.route_b_energy,teammate.trace.route_a_utility,teammate.trace.route_b_utility,teammate.trace.yielded_for_occupancy?"YIELD":"CLEAR"),20,128,16,RAYWHITE);
        DrawText("EVIDENCE -> BELIEF -> GRADIENT -> HOTSPOT -> UTILITY -> COMMITMENT -> ACTION",20,154,16,GOLD);
        DrawText("Q136: bounded 80x80m causal slice inside existing 0.40 km^2 lab; existing network stack remains regression-protected",20,688,14,GRAY);EndDrawing();
    }
    CloseWindow();return 0;
}
