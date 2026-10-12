/* Native H2 fixture viewer: rendering reads real owner state, never commits it.
   Authored scene inputs are shared with tests; their expected outcomes are not. */
#include "../tests/v18b3_fixtures.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static Vector3 vec(NfVec3 v){return (Vector3){v.x,v.y,v.z};}
int main(int argc,char **argv){
    bool automatic=false;int kind=0,model=4;double seconds=1.5;const char *capture=NULL;
    for(int i=1;i<argc;++i){
        if(!strcmp(argv[i],"--auto"))automatic=true;
        else if(!strcmp(argv[i],"--kind")&&i+1<argc)kind=atoi(argv[++i]);
        else if(!strcmp(argv[i],"--model")&&i+1<argc)model=atoi(argv[++i]);
        else if(!strcmp(argv[i],"--seconds")&&i+1<argc)seconds=atof(argv[++i]);
        else if(!strcmp(argv[i],"--capture")&&i+1<argc)capture=argv[++i];
    }
    if(kind<0||kind>6||(model!=3&&model!=4)||!(seconds>0)||!isfinite(seconds))return 2;
    B3Fixture f;if(!b3_allocate(&f)||!b3_fixture(&f,(unsigned)model,(Nf18b3Kind)kind,1,1,0))return 2;
    InitWindow(960,640,"nightfall!punk 1.8B.3 physical interaction — H2 fixture viewer");
    SetTargetFPS(60);
    Camera3D camera={.position={5.8f,4.2f,5.4f},.target={1.8f,.7f,1.4f},.up={0,1,0},.fovy=48,.projection=CAMERA_PERSPECTIVE};
    const char *names[]={"Sliding door","Mechanical switch","Pickup","Push","Carry / release","Repair","Close door"};
    double started=GetTime(),accumulator=0;unsigned attempts=0;
    if(automatic)(void)nf18b3_request(f.r,10,f.target,1,(Nf18b3Kind)kind);
    while(!WindowShouldClose()&&(!automatic||GetTime()-started<seconds)){
        if(!automatic){
            bool reset=IsKeyPressed(KEY_R);
            for(int n=0;n<7;++n)if(IsKeyPressed(KEY_ONE+n)){kind=n;reset=true;}
            if(reset){if(!b3_fixture(&f,(unsigned)model,(Nf18b3Kind)kind,1,1,0))break;attempts=0;accumulator=0;}
            if(IsKeyPressed(KEY_E))(void)nf18b3_request(f.r,10,f.target,(uint32_t)f.r->state.action_count+1,(Nf18b3Kind)kind);
            if(IsKeyPressed(KEY_SPACE)&&f.r->state.active!=UINT32_MAX){
                uint32_t id=f.r->state.actions[f.r->state.active].action_id;(void)nf18b3_cancel(f.r,10,id,kind==NF18B3_CARRY);}
            if(IsKeyPressed(KEY_J))f.r->state.objects[0].jammed=!f.r->state.objects[0].jammed;
        }
        accumulator+=GetFrameTime();
        while(accumulator>=1.0/60){
            NfMoveInput movement={0};
            if(kind==NF18B3_PUSH)movement.strafe=automatic?(attempts<20?1:0):(IsKeyDown(KEY_RIGHT)?1:0);
            nf_world_set_input(f.w,10,movement);
            if(automatic&&kind==NF18B3_CARRY&&attempts==24)(void)nf18b3_cancel(f.r,10,1,true);
            (void)b3_step(&f);++attempts;accumulator-=1.0/60;
        }
        BeginDrawing();ClearBackground((Color){17,23,34,255});BeginMode3D(camera);
        NfCollider floor=f.w->colliders[0];
        DrawCubeV((Vector3){(floor.min.x+floor.max.x)*.5f,(floor.min.y+floor.max.y)*.5f,(floor.min.z+floor.max.z)*.5f},
            (Vector3){floor.max.x-floor.min.x,floor.max.y-floor.min.y,floor.max.z-floor.min.z},(Color){44,54,70,255});DrawGrid(8,1);
        NfActor *actor=nf_world_find_actor(f.w,10);
        Vector3 feet=vec(actor->transform.position);
        DrawCapsule((Vector3){feet.x,feet.y+f.w->movement.radius,feet.z},
            (Vector3){feet.x,feet.y+actor->movement.body_height-f.w->movement.radius,feet.z},f.w->movement.radius,12,8,(Color){245,211,126,255});
        for(size_t i=0;i<f.r->state.object_count;++i){const Nf18b3Object *o=&f.r->state.objects[i];
            if(o->available)DrawCubeV(vec(o->body.center),(Vector3){2*o->body.box_half.x,2*o->body.box_half.y,2*o->body.box_half.z},
                o->jammed?RED:(Color){98,174,231,255});}
        if(f.target!=f.p->physical.object.id){Nf18a4Body b=f.p->physical.object;
            DrawCubeV(vec(b.center),(Vector3){2*b.box_half.x,2*b.box_half.y,2*b.box_half.z},GRAY);}
        EndMode3D();
        const char *state=f.r->state.action_count?nf18b3_status_name(f.r->state.actions[f.r->state.action_count-1].status):"READY";
        DrawText(TextFormat("1.8B.3  |  %s  |  %s",names[kind],state),24,20,24,RAYWHITE);
        DrawText("1-7 scene   E use   Right arrow push   Space cancel/release   R reset   J inject jam",24,56,16,LIGHTGRAY);
        DrawText(TextFormat("Inventory %u   Repair %u / %u   Salvage %u   Power %.2f J",f.r->state.inventory_units,
            f.r->state.objects[0].health,f.r->state.objects[0].max_health,f.r->state.salvage_units,f.r->state.power_joules),24,596,18,RAYWHITE);
        DrawText("Local physical fixture viewer — release HOLD",24,620,14,GRAY);EndDrawing();
    }
    if(capture)TakeScreenshot(capture);
    bool passed=!automatic||(f.r->state.action_count&&f.r->state.actions[0].status==NF18B3_COMPLETED);
    printf("B3_native_viewer kind=%d model=M%d attempts=%u tick=%llu result=%s\n",kind,model,attempts,(unsigned long long)f.w->tick,passed?"PASS":"FAIL");
    CloseWindow();b3_free(&f);return passed?0:1;
}
