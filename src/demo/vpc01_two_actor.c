/* VPC-01 research viewer: two deliberate actors + a movable pod.
 * Draws the exact NfVpc01World staged by A4 impulse physics.
 * Not an A6/B2/B3 game, a production server, or a public playtest. */
#include "nf_vpc01_two_actor.h"
#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define VPC_DT (1.f/60.f)
static Vector3 as_ray(NfVec3 p){return (Vector3){p.x,p.y,p.z};}
static Color col(unsigned index){
 return index==0u?(Color){225,174,111,255}:(Color){91,196,205,255};
}
static void draw_actor(const Nf18a4Body *body,unsigned index){
 const Vector3 pos=as_ray(body->center);
 Color c=col(index);
 DrawCylinder(pos,body->radius,body->radius,body->height,16,c);
 DrawSphere((Vector3){pos.x,pos.y+body->height*.40f,pos.z},
            body->radius*.8f,(Color){235,232,210,255});
 DrawCylinderWires(pos,body->radius,body->radius,body->height,16,
                   (Color){32,59,66,255});
 DrawSphere((Vector3){pos.x,pos.y+body->height*.26f,pos.z+.17f},
            .09f,(Color){62,77,92,255});
}
static void draw_scene(const NfVpc01World *w){
 DrawPlane((Vector3){0,0,0},(Vector2){34,34},(Color){64,83,86,255});
 DrawGrid(34,1.f);
 /* Static greybox-biopunk environmental references, not collision geometry. */
 for(int i=-2;i<=2;++i){
  float x=(float)i*4.5f;
  DrawCube((Vector3){x,.5f,-7.5f},1.f,1.f,1.2f,
           (Color){56,111,118,255});
  DrawCube((Vector3){x,1.2f,-7.5f},1.25f,.23f,1.4f,
           (Color){176,208,169,255});
  DrawSphere((Vector3){x,1.45f,-7.5f},.2f,
             (Color){211,140,168,255});
 }
 for(int z=-2;z<=2;++z){
  DrawCube((Vector3){-7.5f,.55f,(float)z*4.f},1.f,1.1f,1.f,
           (Color){95,112,116,255});
  DrawCube((Vector3){-7.5f,1.2f,(float)z*4.f},1.05f,.15f,1.05f,
           (Color){117,175,142,255});
 }
 DrawCube((Vector3){0,.02f,0},5.f,.04f,5.f,(Color){92,115,124,255});
 DrawCubeWires((Vector3){0,.04f,0},5.f,.08f,5.f,
               (Color){191,193,147,255});
 const Nf18a4Body *pod=&w->body[2];
 DrawCylinder(as_ray(pod->center),.54f,.54f,1.1f,12,
              (Color){178,131,164,255});
 DrawCylinderWires(as_ray(pod->center),.54f,.54f,1.1f,12,
                   (Color){230,217,190,255});
 draw_actor(&w->body[0],0u);
 draw_actor(&w->body[1],1u);
}
int main(int argc,char **argv){
 bool automated=false,second_scripted=true;
 const char *frames_dir=NULL;double seconds=0.0;
 for(int i=1;i<argc;++i){
  if(strcmp(argv[i],"--auto")==0)automated=true;
  else if(strcmp(argv[i],"--seconds")==0&&i+1<argc)seconds=atof(argv[++i]);
  else if(strcmp(argv[i],"--frames-dir")==0&&i+1<argc)frames_dir=argv[++i];
 }
 InitWindow(1280,720,"nightfall!punk - VPC-01 primordial two-actor physics research");
 SetTargetFPS(60);
 NfVpc01World world={0};nf_vpc01_init(&world);
 Camera3D camera={0};camera.position=(Vector3){11,13,16};
 camera.target=(Vector3){0,0,0};camera.up=(Vector3){0,1,0};
 camera.fovy=48.f;camera.projection=CAMERA_PERSPECTIVE;
 double accumulator=0.0;unsigned captured=0u,frames=0u;
 bool blocked=false;
 while(!WindowShouldClose()){
  float frame_dt=GetFrameTime();
  if(frame_dt<0.f)frame_dt=0.f;if(frame_dt>.08f)frame_dt=.08f;
  accumulator+=(double)frame_dt;
  if(IsKeyPressed(KEY_R)){nf_vpc01_init(&world);blocked=false;}
  if(IsKeyPressed(KEY_TAB))second_scripted=!second_scripted;
  NfVpc01Input input={0};
  if(automated){
   if(world.tick<85u){
     input.requested[0].x=1.f;input.requested[1].x=-1.f;
   }else if(world.tick<165u){
     input.requested[0].z=1.f;input.requested[1].z=-1.f;
   }
  }else{
   input.requested[0].z=(float)(IsKeyDown(KEY_S)-IsKeyDown(KEY_W));
   input.requested[0].x=(float)(IsKeyDown(KEY_D)-IsKeyDown(KEY_A));
   if(second_scripted){
    if(world.tick%240u<100u)input.requested[1].x=-.75f;
    else if(world.tick%240u<200u)input.requested[1].x=.75f;
   }else{
    input.requested[1].z=(float)(IsKeyDown(KEY_DOWN)-IsKeyDown(KEY_UP));
    input.requested[1].x=(float)(IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT));
   }
  }
  while(accumulator>=VPC_DT&&!blocked){
   NfVpc01Candidate candidate={0};
   NfVpc01Status status=nf_vpc01_stage(&world,&input,VPC_DT,&candidate);
   if(status!=NF_VPC01_COMMITTED||
      nf_vpc01_publish(&world,&candidate)!=NF_VPC01_COMMITTED){
    blocked=true;
    fprintf(stderr,"VPC-01 H60 staged tick %u denied: status=%u\n",
            world.tick,(unsigned)status);
    break;
   }
   accumulator-=(double)VPC_DT;
  }
  if(blocked)accumulator=0.0;
  BeginDrawing();
  ClearBackground((Color){161,177,174,255});
  BeginMode3D(camera);draw_scene(&world);EndMode3D();
  DrawRectangle(14,14,770,122,(Color){32,50,55,227});
  DrawText("VPC-01  |  TWO-ACTOR PRIMORDIAL PHYSICS",27,25,22,
           (Color){236,236,215,255});
  DrawText("A4 reciprocal impulse laboratory / not A6-B2-B3 gameplay",27,56,17,
           (Color){200,219,208,255});
  DrawText(TextFormat("TICK %u   CONTACTS %u   STATE %s",
          world.tick,world.contact_events,blocked?"PENDING / REFUSED":"STAGED"),
          27,83,17,blocked?ORANGE:(Color){188,223,192,255});
  DrawText("A: WASD     B: ARROWS (TAB toggle auto)     R: reset",27,108,16,
           (Color){220,215,191,255});
  DrawText("Research observation only - no production or human acceptance",23,685,
           15,(Color){30,49,53,255});
  EndDrawing();
  if(automated&&frames_dir!=NULL&&(frames%10u==0u)){
    char output[512];
    int n=snprintf(output,sizeof output,"%s/frame_%05u.png",frames_dir,captured++);
    if(n>0&&n<(int)sizeof output)TakeScreenshot(output);
  }
  ++frames;
  if(automated&&seconds>0.0&&GetTime()>=seconds)break;
 }
 printf("VPC01_H60_VIEW tick=%u contacts=%u denied=%d frames=%u\n",
        world.tick,world.contact_events,(int)blocked,frames);
 CloseWindow();
 return blocked?2:0;
}
