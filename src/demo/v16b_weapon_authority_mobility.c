#include "nf_combat.h"
#include "nf_weapon_geometry.h"
#include "nf_weapon_solution.h"
#include "nf_world.h"
#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct DemoState {
    NfWorld world;
    NfEntityId player_id;
    uint32_t input_sequence;
    float yaw;
    float pitch;
    float displayed_reticle;
    bool last_shot_valid;
    NfWeaponSolution last_shot;
    bool last_blocked;
    bool last_world_hit;
    NfVec3 last_origin;
    NfVec3 last_impact;
    unsigned shots;
    unsigned blocked_shots;
} DemoState;

static float clampf_local(float v,float lo,float hi){return v<lo?lo:(v>hi?hi:v);}
static Vector3 rv(NfVec3 v){return(Vector3){v.x,v.y,v.z};}

static void build_lab(DemoState *demo){
    nf_world_init(&demo->world,20260819u);
    nf_world_add_collider(&demo->world,NF_COLLIDER_SOLID,(NfVec3){-25,-0.5f,-25},(NfVec3){25,0,25});
    nf_world_add_collider(&demo->world,NF_COLLIDER_SOLID,(NfVec3){-3,0,7.2f},(NfVec3){3,3.2f,7.8f});
    nf_world_add_collider(&demo->world,NF_COLLIDER_SOLID,(NfVec3){4,0,-1},(NfVec3){7,0.9f,2});
    nf_world_add_collider(&demo->world,NF_COLLIDER_SOLID,(NfVec3){-7,0,-1},(NfVec3){-4,1.65f,2});
    nf_world_add_collider(&demo->world,NF_COLLIDER_LADDER,(NfVec3){-4.45f,0,-0.4f},(NfVec3){-3.95f,2.2f,0.8f});
    nf_world_add_moving_platform(&demo->world,(NfVec3){8,0.9f,8},(NfVec3){12,1.2f,11},(NfVec3){1,0,0},3.0f,5.0f);
    demo->player_id=nf_world_spawn_actor_with_id(&demo->world,1u,NF_FACTION_PLAYER,(NfVec3){0,0.05f,-8});
    demo->input_sequence=1u;demo->displayed_reticle=7.0f;demo->yaw=0.0f;demo->pitch=0.0f;
}

static void draw_collider(const NfCollider *c){
    Vector3 center={0.5f*(c->min.x+c->max.x),0.5f*(c->min.y+c->max.y),0.5f*(c->min.z+c->max.z)};
    const float sx=c->max.x-c->min.x,sy=c->max.y-c->min.y,sz=c->max.z-c->min.z;
    Color color=c->kind==NF_COLLIDER_LADDER?(Color){80,195,200,190}:(c->kind==NF_COLLIDER_MOVING_PLATFORM?(Color){130,115,170,255}:(Color){110,116,126,255});
    DrawCube(center,sx,sy,sz,color);DrawCubeWires(center,sx,sy,sz,RAYWHITE);
}

static void auto_inputs(const char *scenario,unsigned tick,NfMoveInput *move,NfCombatInput *combat,float *yaw,float *pitch){
    *move=(NfMoveInput){0};*combat=(NfCombatInput){0};*pitch=0.0f;
    if(strcmp(scenario,"focus_hold")==0){combat->focus_held=tick>=45u&&tick<210u;combat->fire_pressed=tick==120u||tick==180u;}
    else if(strcmp(scenario,"redirect_focus")==0){move->forward=0.80f;move->strafe=tick<90u?0.65f:-0.65f;*yaw=tick<90u?0.0f:1.25f;combat->focus_held=tick>=75u&&tick<160u;combat->fire_pressed=tick==105u||tick==150u;}
    else if(strcmp(scenario,"reload_focus")==0){combat->fire_pressed=tick==20u||tick==32u||tick==44u||tick==56u;combat->reload_pressed=tick==72u;combat->focus_held=tick>=76u&&tick<180u;}
    else if(strcmp(scenario,"blocked")==0){move->forward=tick<145u?0.82f:0.0f;combat->focus_held=tick>=100u&&tick<180u;combat->fire_pressed=tick==155u||tick==175u;}
    else {move->forward=tick<90u?0.45f:0.0f;combat->fire_pressed=tick==105u;}
    move->yaw_radians=*yaw;combat->aim_pitch_radians=*pitch;
}

static bool step_demo(DemoState *demo,NfMoveInput move,NfCombatInput combat,NfWeaponEnvelope *current_out){
    NfActor *actor=nf_world_find_actor(&demo->world,demo->player_id);if(actor==NULL)return false;
    /* Q76: movement integrates before the Weapon Authority sample/fire gate. */
    nf_world_set_input(&demo->world,demo->player_id,move);nf_world_step(&demo->world,1.0f/(float)NF_TICK_RATE);actor=nf_world_find_actor(&demo->world,demo->player_id);if(actor==NULL)return false;
    if((move.sprint_held||move.interact_held)&&actor->combat.state==NF_WEAPON_RELOADING)nf_combat_cancel_reload(actor);
    if(combat.weapon_slot>0u&&combat.weapon_slot<NF_WEAPON_COUNT)(void)nf_combat_select_weapon(actor,(NfWeaponId)combat.weapon_slot);
    if(combat.reload_pressed)(void)nf_combat_start_reload(actor);
    nf_weapon_authority_step_actor(actor,combat.focus_held,1.0f/(float)NF_TICK_RATE);
    if(current_out!=NULL)*current_out=nf_weapon_authority_envelope(actor,move.yaw_radians,combat.aim_pitch_radians);

    NfWeaponSolution solution={0};NfCombatEvent event={0};const uint32_t sequence=demo->input_sequence++;
    if(nf_weapon_authority_try_fire(actor,&combat,sequence,demo->world.tick,demo->world.seed,move.yaw_radians,&solution,&event)){
        ++demo->shots;demo->last_shot_valid=true;demo->last_shot=solution;demo->last_origin=nf_weapon_muzzle_origin(actor,move.yaw_radians);const NfWeaponSpec *spec=nf_weapon_spec(actor->combat.weapon);NfWorldRayHit hit={0};demo->last_world_hit=nf_weapon_world_first_hit(&demo->world,demo->last_origin,solution.solved_direction,spec->range,&hit);demo->last_blocked=demo->last_world_hit&&hit.distance<=1.10f;if(demo->last_blocked)++demo->blocked_shots;if(demo->last_world_hit)demo->last_impact=hit.point;else demo->last_impact=(NfVec3){demo->last_origin.x+solution.solved_direction.x*20.0f,demo->last_origin.y+solution.solved_direction.y*20.0f,demo->last_origin.z+solution.solved_direction.z*20.0f};
        return true;
    }
    return false;
}

int main(int argc,char **argv){
    const char *auto_scenario=NULL,*frames_dir=NULL;double auto_seconds=0.0;
    for(int i=1;i<argc;++i){if(strcmp(argv[i],"--auto")==0&&i+1<argc)auto_scenario=argv[++i];else if(strcmp(argv[i],"--seconds")==0&&i+1<argc)auto_seconds=atof(argv[++i]);else if(strcmp(argv[i],"--frames-dir")==0&&i+1<argc)frames_dir=argv[++i];}
    const int width=1280,height=720;InitWindow(width,height,"nightfall!punk v1.6B - Weapon Authority / Mobility");SetTargetFPS(60);DisableCursor();DemoState demo={0};build_lab(&demo);double accumulator=0.0;const double fixed=1.0/(double)NF_TICK_RATE;unsigned render_frame=0u,auto_tick=0u;bool fire_latched=false,reload_latched=false;uint8_t weapon_latched=0u;
    while(!WindowShouldClose()){
        const float frame_dt=clampf_local(GetFrameTime(),0.0f,0.10f);NfMoveInput move={0};NfCombatInput combat={0};
        if(auto_scenario!=NULL){auto_inputs(auto_scenario,auto_tick,&move,&combat,&demo.yaw,&demo.pitch);}else{Vector2 mouse=GetMouseDelta();demo.yaw-=mouse.x*0.0022f;demo.pitch=clampf_local(demo.pitch-mouse.y*0.0022f,-1.45f,1.45f);move.forward=(IsKeyDown(KEY_W)?1.0f:0.0f)-(IsKeyDown(KEY_S)?1.0f:0.0f);move.strafe=(IsKeyDown(KEY_D)?1.0f:0.0f)-(IsKeyDown(KEY_A)?1.0f:0.0f);move.sprint_held=IsKeyDown(KEY_LEFT_SHIFT);move.crouch_held=IsKeyDown(KEY_LEFT_CONTROL);move.jump_pressed=IsKeyPressed(KEY_SPACE);if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))fire_latched=true;if(IsKeyPressed(KEY_R))reload_latched=true;if(IsKeyPressed(KEY_ONE))weapon_latched=1u;if(IsKeyPressed(KEY_TWO))weapon_latched=2u;combat.fire_held=IsMouseButtonDown(MOUSE_BUTTON_LEFT);combat.fire_pressed=fire_latched;combat.reload_pressed=reload_latched;combat.weapon_slot=weapon_latched;combat.focus_held=IsMouseButtonDown(MOUSE_BUTTON_RIGHT)||IsKeyDown(KEY_F);}
        move.yaw_radians=demo.yaw;combat.aim_pitch_radians=demo.pitch;accumulator+=frame_dt;NfWeaponEnvelope current={0};bool fired=false;bool first_tick=true;
        while(accumulator>=fixed){NfMoveInput tick_move=move;NfCombatInput tick_combat=combat;if(!first_tick){tick_move.jump_pressed=false;tick_combat.fire_pressed=false;tick_combat.reload_pressed=false;tick_combat.weapon_slot=0u;}if(step_demo(&demo,tick_move,tick_combat,&current))fired=true;if(auto_scenario!=NULL)++auto_tick;if(first_tick){fire_latched=false;reload_latched=false;weapon_latched=0u;}accumulator-=fixed;first_tick=false;}
        NfActor *actor=nf_world_find_actor(&demo.world,demo.player_id);if(actor==NULL)break;if(current.weapon==NF_WEAPON_NONE)current=nf_weapon_authority_envelope(actor,demo.yaw,demo.pitch);const float fov=82.0f-actor->weapon_authority.focus_amount*7.0f;const float target_reticle=nf_weapon_reticle_radius_px_for_envelope(&current,fov,(float)height);const float alpha=1.0f-expf(-frame_dt/0.055f);demo.displayed_reticle+=(target_reticle-demo.displayed_reticle)*alpha;
        NfVec3 eye3={actor->transform.position.x,actor->transform.position.y+actor->movement.eye_height,actor->transform.position.z};Vector3 eye=rv(eye3);Vector3 look={cosf(demo.pitch)*sinf(demo.yaw),sinf(demo.pitch),cosf(demo.pitch)*cosf(demo.yaw)};Camera3D camera={.position=eye,.target={eye.x+look.x,eye.y+look.y,eye.z+look.z},.up={0,1,0},.fovy=fov,.projection=CAMERA_PERSPECTIVE};
        BeginDrawing();ClearBackground((Color){18,20,24,255});BeginMode3D(camera);DrawGrid(50,1.0f);for(size_t i=0;i<demo.world.collider_count;++i)draw_collider(&demo.world.colliders[i]);DrawCube((Vector3){0,0.9f,10.5f},0.8f,1.8f,0.8f,(Color){214,40,40,255});NfVec3 muzzle=nf_weapon_muzzle_origin(actor,demo.yaw);DrawSphere(rv(muzzle),0.06f,(Color){50,220,220,255});Vector3 intended_end={muzzle.x+current.intended_direction.x*3.0f,muzzle.y+current.intended_direction.y*3.0f,muzzle.z+current.intended_direction.z*3.0f};DrawLine3D(rv(muzzle),intended_end,(Color){50,220,220,150});if(demo.last_shot_valid){DrawLine3D(rv(demo.last_origin),rv(demo.last_impact),demo.last_blocked?ORANGE:(Color){255,220,80,220});DrawSphere(rv(demo.last_impact),0.08f,demo.last_blocked?ORANGE:YELLOW);}EndMode3D();
        const int cx=width/2,cy=height/2;const int r=(int)demo.displayed_reticle;DrawLine(cx-r,cy,cx-r/2,cy,RAYWHITE);DrawLine(cx+r/2,cy,cx+r,cy,RAYWHITE);DrawLine(cx,cy-r,cx,cy-r/2,RAYWHITE);DrawLine(cx,cy+r/2,cx,cy+r,RAYWHITE);DrawCircle(cx,cy,2,(Color){80,220,220,255});
        DrawText("nightfall!punk v1.6B — WEAPON AUTHORITY / MOBILITY",20,18,22,RAYWHITE);DrawText(TextFormat("INPUT RMB/Focus %s | LMB %s | MOVE %s %.2fm/s",combat.focus_held?"DOWN":"UP",(combat.fire_held||combat.fire_pressed)?"DOWN":"UP",nf_movement_mode_name(actor->movement.mode),sqrtf(actor->transform.velocity.x*actor->transform.velocity.x+actor->transform.velocity.z*actor->transform.velocity.z)),20,50,16,RAYWHITE);DrawText(TextFormat("NOW focus %.3f | cone %.3f deg | motion %.3f redirect %.3f support %.3f recoil %.3f",actor->weapon_authority.focus_amount,current.total_cone_deg,current.motion_cone_deg,current.redirect_cone_deg,current.support_cone_deg,current.recoil_cone_deg),20,72,16,(Color){110,220,220,255});DrawText(TextFormat("AUTH accepted-shot %u | weapon %s %u/%u | reload %s",actor->weapon_authority.accepted_shot_sequence,nf_weapon_name(actor->combat.weapon),actor->combat.ammo_mag[actor->combat.weapon],actor->combat.reserve_ammo[actor->combat.weapon],actor->combat.state==NF_WEAPON_RELOADING?"YES":"NO"),20,94,16,RAYWHITE);DrawText(TextFormat("LAST SHOT %s seq %u cone %.3f blocked %s | shots %u blocked %u",demo.last_shot_valid?"YES":"NO",demo.last_shot.shot_sequence,demo.last_shot.total_cone_deg,demo.last_blocked?"YES":"NO",demo.shots,demo.blocked_shots),20,116,16,demo.last_blocked?ORANGE:RAYWHITE);DrawText("cyan = intention/muzzle | yellow = realized ray/impact | orange = near-muzzle blocked",20,height-34,15,GRAY);if(auto_scenario!=NULL)DrawText(TextFormat("AUTO SAMPLE %s tick %u",auto_scenario,auto_tick),width-330,20,18,YELLOW);if(fired)DrawCircle(width-30,30,8,YELLOW);EndDrawing();
        if(frames_dir!=NULL&&(render_frame%2u)==0u){char path[512];snprintf(path,sizeof(path),"%s/frame_%05u.png",frames_dir,render_frame/2u);TakeScreenshot(path);}++render_frame;if(auto_scenario!=NULL&&auto_seconds>0.0&&GetTime()>=auto_seconds)break;
    }
    EnableCursor();CloseWindow();return 0;
}
