#include "nf_world.h"
#include "raylib.h"

#include <math.h>

static float clampf_local(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
static Color collider_color(const NfCollider *c) { if (c->kind == NF_COLLIDER_LADDER) return (Color){235,154,64,105}; if (c->kind == NF_COLLIDER_MOVING_PLATFORM) return (Color){143,104,220,255}; return (Color){95,101,112,255}; }
static void draw_collider(const NfCollider *c) { Vector3 center={0.5f*(c->min.x+c->max.x),0.5f*(c->min.y+c->max.y),0.5f*(c->min.z+c->max.z)}; float sx=c->max.x-c->min.x, sy=c->max.y-c->min.y, sz=c->max.z-c->min.z; Color color=collider_color(c); DrawCube(center,sx,sy,sz,color); DrawCubeWires(center,sx,sy,sz,(Color){185,191,202,180}); }
static void tri(Vector3 a, Vector3 b, Vector3 c, Color color) { DrawTriangle3D(a,b,c,color); }
static void draw_ramp(const NfRamp *r) {
    const Color color=(Color){82,119,132,255}; Vector3 low_a,low_b,high_a,high_b;
    if (r->axis==NF_RAMP_POS_Z || r->axis==NF_RAMP_NEG_Z) { bool pos=r->axis==NF_RAMP_POS_Z; float low_z=pos?r->min.z:r->max.z, high_z=pos?r->max.z:r->min.z; low_a=(Vector3){r->min.x,r->min.y,low_z}; low_b=(Vector3){r->max.x,r->min.y,low_z}; high_a=(Vector3){r->min.x,r->max.y,high_z}; high_b=(Vector3){r->max.x,r->max.y,high_z}; }
    else { bool pos=r->axis==NF_RAMP_POS_X; float low_x=pos?r->min.x:r->max.x, high_x=pos?r->max.x:r->min.x; low_a=(Vector3){low_x,r->min.y,r->min.z}; low_b=(Vector3){low_x,r->min.y,r->max.z}; high_a=(Vector3){high_x,r->max.y,r->min.z}; high_b=(Vector3){high_x,r->max.y,r->max.z}; }
    tri(low_a,low_b,high_b,color); tri(low_a,high_b,high_a,color); DrawLine3D(low_a,low_b,LIGHTGRAY); DrawLine3D(low_a,high_a,LIGHTGRAY); DrawLine3D(low_b,high_b,LIGHTGRAY); DrawLine3D(high_a,high_b,LIGHTGRAY);
}
static Color candidate_color(NfTraversalType type) { switch(type){case NF_TRAVERSAL_STEP:return GREEN;case NF_TRAVERSAL_VAULT:return YELLOW;case NF_TRAVERSAL_MANTLE:return SKYBLUE;case NF_TRAVERSAL_LADDER:return ORANGE;default:return RED;} }

int main(void) {
    const int width=1280,height=720; InitWindow(width,height,"nightfall!punk v0.2 movement lab"); SetTargetFPS(144); DisableCursor();
    NfWorld world; nf_world_init(&world,20260807u); nf_world_build_movement_lab(&world); const NfEntityId player_id=nf_world_spawn_actor(&world,NF_FACTION_PLAYER,(NfVec3){0.0f,0.05f,-18.0f});
    float yaw=0.0f,pitch=0.0f; double accumulator=0.0; const double fixed_dt=1.0/(double)NF_TICK_RATE; bool jump_latched=false;
    while(!WindowShouldClose()) {
        Vector2 mouse=GetMouseDelta(); yaw+=mouse.x*0.0022f; pitch=clampf_local(pitch-mouse.y*0.0022f,-1.45f,1.45f);
        NfMoveInput input={0}; input.forward=(IsKeyDown(KEY_W)?1.0f:0.0f)-(IsKeyDown(KEY_S)?1.0f:0.0f); input.strafe=(IsKeyDown(KEY_D)?1.0f:0.0f)-(IsKeyDown(KEY_A)?1.0f:0.0f); input.yaw_radians=yaw; if(IsKeyPressed(KEY_SPACE)) jump_latched=true; input.jump_pressed=jump_latched; input.crouch_held=IsKeyDown(KEY_LEFT_CONTROL); input.sprint_held=IsKeyDown(KEY_LEFT_SHIFT); input.interact_held=IsKeyDown(KEY_E);
        double frame_dt=GetFrameTime(); if(frame_dt>0.10) frame_dt=0.10; accumulator+=frame_dt; bool first_tick=true;
        while(accumulator>=fixed_dt){ if(!first_tick) input.jump_pressed=false; nf_world_set_input(&world,player_id,input); nf_world_step(&world,(float)fixed_dt); if(first_tick&&input.jump_pressed) jump_latched=false; accumulator-=fixed_dt; first_tick=false; }
        const NfActor *player=nf_world_find_actor_const(&world,player_id); if(player==NULL) break;
        Vector3 eye={player->transform.position.x,player->transform.position.y+player->movement.eye_height,player->transform.position.z}; Vector3 look={cosf(pitch)*sinf(yaw),sinf(pitch),cosf(pitch)*cosf(yaw)}; Camera3D camera={.position=eye,.target={eye.x+look.x,eye.y+look.y,eye.z+look.z},.up={0,1,0},.fovy=82.0f,.projection=CAMERA_PERSPECTIVE};
        BeginDrawing(); ClearBackground((Color){18,20,24,255}); BeginMode3D(camera); DrawGrid(60,1.0f); for(size_t i=0;i<world.collider_count;++i) draw_collider(&world.colliders[i]); for(size_t i=0;i<world.ramp_count;++i) draw_ramp(&world.ramps[i]);
        if(player->movement.candidate.active){ const NfTraversalCandidate *c=&player->movement.candidate; Vector3 point={c->point.x,c->point.y+0.15f,c->point.z}; DrawSphere(point,0.14f,candidate_color(c->type)); DrawLine3D((Vector3){player->transform.position.x,player->transform.position.y+0.6f,player->transform.position.z},point,candidate_color(c->type)); }
        EndMode3D(); DrawText("nightfall!punk v0.2 — MOVEMENT LAB",24,20,24,RAYWHITE); DrawText("WASD move | Shift sprint | Ctrl crouch | Space jump / crouch+Space second jump | E ladder",24,50,16,GRAY);
        float speed=sqrtf(player->transform.velocity.x*player->transform.velocity.x+player->transform.velocity.z*player->transform.velocity.z); DrawText(TextFormat("FPS %d | sim tick %llu",GetFPS(),(unsigned long long)world.tick),24,86,16,GREEN); DrawText(TextFormat("pos  %.2f %.2f %.2f",player->transform.position.x,player->transform.position.y,player->transform.position.z),24,108,16,RAYWHITE); DrawText(TextFormat("vel  %.2f %.2f %.2f | speed %.2f",player->transform.velocity.x,player->transform.velocity.y,player->transform.velocity.z,speed),24,130,16,RAYWHITE); DrawText(TextFormat("mode %s | grounded %s | crouched %s | jumps %d",nf_movement_mode_name(player->movement.mode),player->movement.grounded?"yes":"no",player->movement.crouched?"yes":"no",player->movement.jumps_used),24,152,16,RAYWHITE); DrawText(TextFormat("fuzzy rail: %s score %.2f feature %d",player->movement.candidate.active?nf_traversal_type_name(player->movement.candidate.type):"NONE",player->movement.candidate.score,player->movement.candidate.feature_index),24,174,16,player->movement.candidate.active?candidate_color(player->movement.candidate.type):GRAY); DrawCircle(width/2,height/2,2.0f,RAYWHITE); DrawText("v0.2: direct movement + local fuzzy-rail geometry; networking remains v0.3",24,height-34,15,(Color){160,166,178,255}); EndDrawing();
    }
    EnableCursor(); CloseWindow(); return 0;
}
