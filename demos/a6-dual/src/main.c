#include "arcade.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#ifdef NF_WITH_X11
#include <X11/Xlib.h>
#include <X11/keysym.h>
#endif
static int parse_kind(const char *s){return strcmp(s,"quake")==0||strcmp(s,"slipgate")==0?GAME_SLIPGATE:GAME_CINDER;}
static int parse_ai(const char *s){return strcmp(s,"classic")==0||strcmp(s,"baseline")==0?AI_CLASSIC:AI_SYSTEMIC;}
static void csv_header(void){
 puts("game,ai,player,seed,ticks,score,pellets_remaining,health,won,lost,energy,pushes,contacts,cache_loads,cache_pending,purple,blue,ghost_steps,ghost_chases,energy_retreats,stale_chases,blocked_moves,gate_rejects,history_samples,history_events,history_fail,digest");
}
static void csv_row(const Arcade *g,const char *player){
 printf("%s,%s,%s,%u,%d,%d,%d,%d,%d,%d,%.3f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%u\n",
  g->kind==GAME_CINDER?"cinder":"slipgate",g->policy==AI_CLASSIC?"classic":"systemic",
  player,g->seed,g->tick,g->score,g->remaining,g->health,g->won,g->lost,g->player_energy,g->push_count,g->contact_count,g->cache_loads,g->pending_count,g->purple_count,g->blue_count,
  g->ghost_steps,g->ghost_chases,g->energy_retreats,g->stale_chases,g->blocked_moves,g->gate_rejects,g->history.raw_samples,g->history.generated_events,g->history_fail,g->hash);
}
static void run_bot(GameKind k,AiPolicy policy,uint32_t seed,int limit,int random_player,Arcade *out){
 arcade_init(out,k,policy,seed);
 for(int t=0;t<limit&&!out->lost&&!out->won;t++){
  int action=random_player?(int)((out->seed+out->tick*747796405u+out->tick*out->tick*2891336453u)%5u):arcade_bot_action(out);
  arcade_tick(out,action);
 }
}
#ifdef NF_WITH_X11
#define TS 29
#define GX 22
#define GY 57
#define W 930
#define H 598
static Display *disp;
static Window win;
static GC gc;
static unsigned long pixel(int r,int g,int b){return ((unsigned long)r<<16)|((unsigned long)g<<8)|(unsigned long)b;}
static void fg(int r,int g,int b){XSetForeground(disp,gc,pixel(r,g,b));}
static void rect(int x,int y,int w,int h,int r,int g,int b){fg(r,g,b);XFillRectangle(disp,win,gc,x,y,(unsigned int)w,(unsigned int)h);}
static void text(int x,int y,const char *str,int r,int g,int b){fg(r,g,b);XDrawString(disp,win,gc,x,y,str,(int)strlen(str));}
static void circ(int x,int y,int w,int h,int r,int g,int b){fg(r,g,b);XFillArc(disp,win,gc,x,y,(unsigned)w,(unsigned)h,0,360*64);}
static void paint(const Arcade *a){
 int quake=a->kind==GAME_SLIPGATE;
 rect(0,0,W,H,12,17,27);
 rect(0,0,W,42,quake?12:34,quake?30:16,quake?52:33);
 text(24,25,quake?"NIGHTFALL ! PUNK   /   SLIPGATE CIRCUIT":"NIGHTFALL ! PUNK   /   CINDER CIRCUIT",245,230,210);
 rect(GX-5,GY-5,AW*TS+10,AH*TS+10,38,49,66);
 for(int z=0;z<AH;z++)for(int x=0;x<AW;x++){
  int px=GX+x*TS,py=GY+z*TS;char c=a->cells[z][x];
  rect(px,py,TS-1,TS-1,(x+z)%2?24:21,(x+z)%2?32:29,quake?48:43);
  if(c=='#'){
   rect(px+1,py+1,TS-3,TS-3,quake?68:93,quake?73:52,quake?99:63);
   rect(px+3,py+3,TS-7,4,quake?102:133,quake?109:77,quake?133:83);
  }
  if(c=='.')circ(px+TS/2-3,py+TS/2-3,6,6,217,204,126);
  if(c=='o')circ(px+TS/2-7,py+TS/2-7,14,14,255,208,85);
  if(c=='K')rect(px+9,py+7,11,14,255,212,81);
  if(c=='D')rect(px+2,py+2,TS-5,TS-5,a->switches&1?41:160,a->switches&1?107:51,86);
  if(c=='S')rect(px+7,py+7,15,15,98,225,214);
  if(c=='H'){rect(px+11,py+2,7,TS-5,70,165,214);rect(px+4,py+7,TS-9,4,154,233,255);rect(px+4,py+19,TS-9,4,154,233,255);}
  if(c=='B'){rect(px+5,py+5,TS-11,TS-11,157,115,77);rect(px+7,py+9,TS-15,3,226,177,115);}
  if(c=='e')circ(px+8,py+8,13,13,242,102,151);
 }
 for(int i=0;i<GHOSTS;i++){
  int px=GX+a->ghosts[i].x*TS,py=GY+a->ghosts[i].z*TS;
  const int cc[3][3]={{245,82,83},{164,95,219},{55,184,218}};
  circ(px+3,py+3,TS-7,TS-7,a->ghosts[i].stun?110:cc[i][0],a->ghosts[i].stun?118:cc[i][1],a->ghosts[i].stun?145:cc[i][2]);
  circ(px+10,py+9,4,4,242,241,229);
 }
 circ(GX+a->x*TS+3,GY+a->z*TS+3,TS-7,TS-7,110,228,130);
 rect(GX+a->x*TS+13,GY+a->z*TS+4,4,9,25,67,41);
 rect(710,54,205,492,19,29,47);
 char s[125];
 text(728,78,"SIMULATION CHANNELS",185,219,242);
 text(728,104,"RED  : actor intention",241,118,122);
 text(728,124,"BLUE : material veto",105,185,240);
 text(728,144,"PURPLE: real reciprocity",199,134,231);
 text(728,178,"CONTROL PROFILE",234,207,170);
 text(728,200,a->policy==AI_CLASSIC?"Classic omniscient goal":"Bounded situated utility",211,221,227);
 snprintf(s,sizeof(s),"TICK             %d",a->tick);text(728,237,s,206,218,233);
 snprintf(s,sizeof(s),"SCORE            %d",a->score);text(728,257,s,206,218,233);
 snprintf(s,sizeof(s),"HEALTH           %d",a->health);text(728,277,s,206,218,233);
 snprintf(s,sizeof(s),"ENERGY           %.0f",a->player_energy);text(728,297,s,206,218,233);
 snprintf(s,sizeof(s),"PELLETS LEFT     %d",a->remaining);text(728,317,s,206,218,233);
 snprintf(s,sizeof(s),"IMPULSE CONTACTS %d",a->contact_count);text(728,350,s,206,218,233);
 snprintf(s,sizeof(s),"PURPLE           %d",a->purple_count);text(728,370,s,206,218,233);
 snprintf(s,sizeof(s),"BLUE REJECTIONS  %d",a->blue_count);text(728,390,s,206,218,233);
 snprintf(s,sizeof(s),"CACHE LOADS      %d",a->cache_loads);text(728,410,s,206,218,233);
 snprintf(s,sizeof(s),"PENDING->LOADED  %d",a->pending_count);text(728,430,s,206,218,233);
 snprintf(s,sizeof(s),"TRACE RECORDS    %d",a->history.raw_samples);text(728,450,s,206,218,233);
 snprintf(s,sizeof(s),"EVENTS           %d",a->history.generated_events);text(728,470,s,206,218,233);
 text(728,508,a->lost?"END: PLAYER DEFEATED":a->won?"END: ALL SIGNALS":"END: RUNNING",a->lost?245:138,218,144);
 text(23,574,"WASD / arrows move     SPACE pulse (-8 energy)      M switch AI      N restart      Q quit",208,211,225);
 XFlush(disp);
}
static int xgame(GameKind k,AiPolicy policy,uint32_t seed){
 disp=XOpenDisplay(NULL);if(!disp){fprintf(stderr,"No X11 display. Use --text or --batch; launch under an X11/Xwayland session.\n");return 2;}
 int scr=DefaultScreen(disp);win=XCreateSimpleWindow(disp,RootWindow(disp,scr),40,40,W,H,0,0,0);
 XStoreName(disp,win,"nightfall!punk - Dual Pacman Systems Lab");XSelectInput(disp,win,ExposureMask|KeyPressMask|StructureNotifyMask);
 gc=XCreateGC(disp,win,0,NULL);XMapWindow(disp,win);
 Arcade a;arcade_init(&a,k,policy,seed);int quit=0;
 while(!quit){int action=-1,redraw=0;
  while(XPending(disp)){
   XEvent e;XNextEvent(disp,&e);
   if(e.type==DestroyNotify){quit=1;break;}
   if(e.type==Expose)redraw=1;
   if(e.type==KeyPress){KeySym key=XLookupKeysym(&e.xkey,0);
    if(key==XK_q||key==XK_Escape){quit=1;break;}
    if(key==XK_w||key==XK_Up)action=0;
    if(key==XK_d||key==XK_Right)action=1;
    if(key==XK_s||key==XK_Down)action=2;
    if(key==XK_a||key==XK_Left)action=3;
    if(key==XK_space)action=4;
    if(key==XK_m){policy=policy==AI_CLASSIC?AI_SYSTEMIC:AI_CLASSIC;arcade_init(&a,k,policy,seed);redraw=1;}
    if(key==XK_n){seed++;arcade_init(&a,k,policy,seed);redraw=1;}
   }
  }
  if(!a.lost&&!a.won){arcade_tick(&a,action);redraw=1;}
  if(redraw)paint(&a);
  struct timespec pause={.tv_sec=0,.tv_nsec=140000000};nanosleep(&pause,NULL);
 }
 printf("FINAL,%s,%s,seed=%u,score=%d,hash=%u\n",arcade_kind_name(k),arcade_ai_name(policy),seed,a.score,a.hash);
 XFreeGC(disp,gc);XDestroyWindow(disp,win);XCloseDisplay(disp);return 0;
}
#endif
int main(int argc,char **argv){
 GameKind k=strstr(argv[0],"slipgate")?GAME_SLIPGATE:GAME_CINDER;AiPolicy policy=AI_SYSTEMIC;uint32_t seed=42u;int batch=0,textmode=0,steps=MAX_TICKS,selftest=0,random_player=0;const char *scenario=NULL;
 for(int i=1;i<argc;i++){
  if(strcmp(argv[i],"--game")==0&&i+1<argc)k=(GameKind)parse_kind(argv[++i]);
  else if(strcmp(argv[i],"--ai")==0&&i+1<argc)policy=(AiPolicy)parse_ai(argv[++i]);
  else if(strcmp(argv[i],"--seed")==0&&i+1<argc)seed=(uint32_t)strtoul(argv[++i],NULL,10);
  else if(strcmp(argv[i],"--batch")==0&&i+1<argc)batch=atoi(argv[++i]);
  else if(strcmp(argv[i],"--steps")==0&&i+1<argc)steps=atoi(argv[++i]);
  else if(strcmp(argv[i],"--text")==0)textmode=1;
  else if(strcmp(argv[i],"--player")==0&&i+1<argc)random_player=strcmp(argv[++i],"random")==0;
  else if(strcmp(argv[i],"--selftest")==0)selftest=1;
  else if(strcmp(argv[i],"--scenario")==0&&i+1<argc)scenario=argv[++i];
  else if(strcmp(argv[i],"--help")==0){puts("--game cinder|slipgate --ai classic|systemic --player goal|random --seed N --batch N --steps N --text --selftest --scenario crate|ladder|pending|door");return 0;}
  else {fprintf(stderr,"Invalid argument: %s\n",argv[i]);return 2;}
 }
 if(selftest)return arcade_selftest();
 if(scenario)return arcade_scenario(scenario);
 if(steps<0||steps>MAX_TICKS||batch<0||batch>100000){fprintf(stderr,"steps must be 0..300 and batch 0..100000\n");return 2;}
 if(batch){csv_header();for(int s=0;s<batch;s++){Arcade g;run_bot(k,policy,seed+(uint32_t)s,steps,random_player,&g);csv_row(&g,random_player?"random":"goal");}return 0;}
 if(textmode){Arcade g;run_bot(k,policy,seed,steps,random_player,&g);arcade_ascii(&g);csv_header();csv_row(&g,random_player?"random":"goal");return 0;}
#ifdef NF_WITH_X11
 return xgame(k,policy,seed);
#else
 fprintf(stderr,"X11 disabled. Use --text or --batch.\n");return 2;
#endif
}
