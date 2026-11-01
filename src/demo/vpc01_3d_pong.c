/* SPDX-License-Identifier: GPL-3.0-or-later
 * H63 VPC-01 descendant: minimalist 3D Pong using actual A4 impulse game core.
 * Graphical research demo, not production multiplayer or wall-riding proof.
 */
#include "nf_retc_pong.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DT (1.f/60.f)
static float clampf(float x,float a,float b){return x<a?a:(x>b?b:x);}
static float bot(const NfPongWorld *w,unsigned i){
 const float x=w->ball.center.x,vx=w->ball.velocity.x;
 const float paddle_x=i==0u?-NF_PONG_PADDLE_X:NF_PONG_PADDLE_X;
 float target=0.f;
 if((i==0u&&vx<0.f)||(i==1u&&vx>0.f)){
  const float time=fabsf(vx)>1.e-5f?(paddle_x-x)/vx:0.f;
  target=w->ball.center.z+w->ball.velocity.z*clampf(time,0.f,1.5f);
  /* Fold reflected wall paths into the playable court range. */
  const float edge=NF_PONG_COURT_Z;
  for(unsigned k=0;k<8u&&fabsf(target)>edge;k++){
   if(target>edge)target=2.f*edge-target;
   else if(target< -edge)target=-2.f*edge-target;
  }
 }
 const float diff=target-w->paddle_z[i];
 return fabsf(diff)<.06f?0.f:clampf(diff*3.f,-1.f,1.f);
}
static void draw_arena(const NfPongWorld *w){
 const Color floor=(Color){30,50,60,255};
 const Color line=(Color){129,177,188,255};
 DrawCube((Vector3){0,-.14f,0},17.8f,.25f,9.3f,floor);
 DrawCubeWires((Vector3){0,-.14f,0},17.8f,.25f,9.3f,(Color){82,112,124,255});
 DrawCube((Vector3){0,.02f,0},.05f,.03f,8.45f,line);
 for(int z=-4;z<=4;z+=2){
  DrawCube((Vector3){0,.04f,(float)z},.13f,.035f,.3f,
           (Color){196,214,195,255});
 }
 for(int side=-1;side<=1;side+=2){
  float z=(float)side*(NF_PONG_COURT_Z+.29f);
  DrawCube((Vector3){0,.28f,z},17.2f,.5f,.16f,(Color){49,114,136,255});
  DrawCube((Vector3){0,.53f,z},17.2f,.05f,.17f,(Color){120,219,224,255});
 }
 for(int side=0;side<2;side++){
  float x=side==0?-NF_PONG_PADDLE_X:NF_PONG_PADDLE_X;
  DrawCube((Vector3){x,.67f,w->paddle_z[side]},.56f,1.3f,
           2.f*NF_PONG_PADDLE_HALF_Z,
           side==0?(Color){242,179,119,255}:(Color){91,218,214,255});
  DrawCubeWires((Vector3){x,.67f,w->paddle_z[side]},.56f,1.3f,
                2.f*NF_PONG_PADDLE_HALF_Z,RAYWHITE);
 }
 DrawSphere((Vector3){w->ball.center.x,w->ball.center.y,
                      w->ball.center.z},NF_PONG_BALL_RADIUS,
                      (Color){254,244,182,255});
 DrawSphereWires((Vector3){w->ball.center.x,w->ball.center.y,
                      w->ball.center.z},NF_PONG_BALL_RADIUS,8,9,
                      (Color){255,255,255,255});
}
int main(int argc,char **argv){
 bool automated=false,right_bot=true;double seconds=0.;
 const char *frames_dir=NULL;
 for(int i=1;i<argc;i++){
  if(strcmp(argv[i],"--auto")==0)automated=true;
  else if(strcmp(argv[i],"--seconds")==0&&i+1<argc)seconds=atof(argv[++i]);
  else if(strcmp(argv[i],"--frames-dir")==0&&i+1<argc)frames_dir=argv[++i];
 }
 InitWindow(1280,720,"nightfall!punk | VPC-01 3D PONG research");
 SetTargetFPS(60);
 Camera3D cam={0};cam.position=(Vector3){3.f,16.5f,15.f};
 cam.target=(Vector3){0,0,0};cam.up=(Vector3){0,1,0};
 cam.fovy=43.f;cam.projection=CAMERA_PERSPECTIVE;
 NfPongWorld world={0};nf_pong_init(&world);
 double acc=0.0;unsigned frames=0u,captures=0u;bool blocked=false;
 while(!WindowShouldClose()){
  if(IsKeyPressed(KEY_R)){nf_pong_init(&world);blocked=false;acc=0.0;}
  if(IsKeyPressed(KEY_TAB))right_bot=!right_bot;
  float frame=clampf(GetFrameTime(),0.f,.075f);
  acc+=(double)frame;
  NfPongInput in={0};
  in.paddle_axis[0]=automated?bot(&world,0u):
    (float)(IsKeyDown(KEY_S)-IsKeyDown(KEY_W));
  in.paddle_axis[1]=automated||right_bot?bot(&world,1u):
    (float)(IsKeyDown(KEY_DOWN)-IsKeyDown(KEY_UP));
  while(acc>=DT&&!blocked){
   NfPongCandidate candidate={0};
   NfPongStatus s=nf_pong_stage(&world,&in,DT,&candidate);
   if(s!=NF_PONG_OK||nf_pong_publish(&world,&candidate)!=NF_PONG_OK){
    blocked=true;fprintf(stderr,"PONG_H63_DENIED tick=%u status=%u\n",
                         world.tick,(unsigned)s);break;
   }
   acc-=(double)DT;
  }
  if(blocked)acc=0.0;
  BeginDrawing();ClearBackground((Color){177,200,204,255});
  BeginMode3D(cam);draw_arena(&world);EndMode3D();
  DrawRectangle(20,17,720,123,(Color){19,37,48,238});
  DrawText("NIGHTFALL!PUNK  /  PRIMORDIAL 3D PONG",33,29,23,RAYWHITE);
  DrawText(TextFormat("%u  :  %u    |    TICK %u",world.score[0],world.score[1],world.tick),
           33,62,24,(Color){237,228,184,255});
  DrawText(TextFormat("A4 IMPULSES: PADDLES %u  WALLS %u     %s",
           world.paddle_hits,world.wall_hits,blocked?"PENDING":"RUNNING"),
           33,101,16,blocked?ORANGE:(Color){160,221,209,255});
  DrawRectangle(20,650,860,47,(Color){19,37,48,224});
  DrawText("W/S: left paddle    TAB: toggle right bot    arrows: right paddle    R: reset",
           32,660,17,(Color){231,235,225,255});
  DrawText("Physics research demo only: A4 contacts, not full A6/B2/B3 authority",
           32,684,11,(Color){177,213,219,255});
  EndDrawing();
  if(automated&&frames_dir&&frames%12u==0u){
   char name[512];
   int n=snprintf(name,sizeof name,"%s/frame_%04u.png",frames_dir,captures++);
   if(n>0&&n<(int)sizeof name)TakeScreenshot(name);
  }
  frames++;
  if(automated&&seconds>0.&&GetTime()>=seconds)break;
 }
 printf("PONG_H63_VIEW tick=%u scores=%u:%u paddle=%u wall=%u denied=%d frames=%u\n",
        world.tick,world.score[0],world.score[1],world.paddle_hits,
        world.wall_hits,blocked?1:0,frames);
 CloseWindow();
 return blocked?2:0;
}
