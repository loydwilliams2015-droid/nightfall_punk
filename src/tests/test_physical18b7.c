#include "nf_physical18b7.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static unsigned checks=0;
#define T(x) do{if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return false;}checks++;}while(0)
static bool yes(void *p,const Nf7fWorld *w){*(Nf7fWorld*)p=*w;return true;}
static bool no(void *p,const Nf7fWorld *w){(void)p;(void)w;return false;}
static Nf7fRequest request(Nf7fWorld *w,uint32_t id,Nf18b3Kind kind,int32_t arg){
 Nf7fRequest r={.actor=7,.action=id,.kind=kind,.argument=arg,.world_epoch=w->world_epoch,
 .material_epoch=w->material_epoch,.contract_epoch=w->contract_epoch,
 .physical_ok=1,.contract_ok=1};return r;
}
static bool fixtures(void){
 Nf7fWorld w,disk,old;Nf7fRequest r;Nf7fStatus s;nf7f_init(&w);disk=w;
 /* Door: actual progressive movement and end-position latch. */
 r=request(&w,1,NF18B3_DOOR,10);T(nf7f_apply(&w,r,yes,&disk)==NF7F_APPLIED);
 T(w.door_position==10&&!w.door_latched&&w.receipt_count==1);
 T(memcmp(&w,&disk,sizeof w)==0);T(nf7f_apply(&w,r,yes,&disk)==NF7F_DUPLICATE);
 old=w;r.argument=5;T(nf7f_apply(&w,r,yes,&disk)==NF7F_INVALID);T(memcmp(&w,&old,sizeof w)==0);
 /* Actual obstruction leaves state untouched; remove and resume. */
 w.door_obstructed=1;w.door_obstacle_low=15;w.door_obstacle_high=17;
 r=request(&w,2,NF18B3_DOOR,10);old=w;
 T(nf7f_apply(&w,r,yes,&disk)==NF7F_PENDING);T(memcmp(&w,&old,sizeof w)==0);
 w.door_obstructed=0;disk=w;r=request(&w,2,NF18B3_DOOR,10);
 T(nf7f_apply(&w,r,yes,&disk)==NF7F_APPLIED);T(w.door_position==20);
 /* Switch with unknown power pending, known power applied. */
 w.power_known=0;r=request(&w,3,NF18B3_SWITCH,0);old=w;
 T(nf7f_apply(&w,r,yes,&disk)==NF7F_PENDING);T(memcmp(&w,&old,sizeof w)==0);
 w.power_known=1;disk=w;r=request(&w,3,NF18B3_SWITCH,0);
 T(nf7f_apply(&w,r,yes,&disk)==NF7F_APPLIED);T(w.switch_on==1);
 /* Pickup and interruption conserve possession history. */
 r=request(&w,4,NF18B3_PICKUP,0);T(nf7f_apply(&w,r,yes,&disk)==NF7F_APPLIED);
 T(w.held_by==7&&!w.floor_supported);T(w.receipt_count==4);
 r=request(&w,5,NF18B3_INTERRUPT,0);T(nf7f_apply(&w,r,yes,&disk)==NF7F_APPLIED);
 T(!w.held_by&&w.floor_supported&&w.interrupted_actions==1&&w.receipt_count==5);
 /* Push: equal and opposite momentum in test model. */
 r=request(&w,6,NF18B3_PUSH,25);int64_t before=w.actor_momentum+w.object_momentum;
 T(nf7f_apply(&w,r,yes,&disk)==NF7F_APPLIED);
 T(w.actor_momentum==-25&&w.object_momentum==25);T(w.actor_momentum+w.object_momentum==before);
 /* Carry attach and release and repair with real limited supplies. */
 r=request(&w,7,NF18B3_CARRY,1);T(nf7f_apply(&w,r,yes,&disk)==NF7F_APPLIED);
 T(w.held_by==7&&!w.floor_supported);
 r=request(&w,8,NF18B3_CARRY,0);T(nf7f_apply(&w,r,yes,&disk)==NF7F_APPLIED);
 T(!w.held_by&&w.floor_supported);
 r=request(&w,9,NF18B3_REPAIR,8);uint32_t total=w.health+w.supplies;
 T(nf7f_apply(&w,r,yes,&disk)==NF7F_APPLIED);
 T(w.health==58&&w.supplies==12);T(w.health+w.supplies==total);
 /* Stale, contract/physics denied, unknown, failed publication and exhaustion. */
 r=request(&w,10,NF18B3_DOOR,10);r.material_epoch--;old=w;
 T(nf7f_apply(&w,r,yes,&disk)==NF7F_STALE);T(memcmp(&w,&old,sizeof w)==0);
 r=request(&w,11,NF18B3_DOOR,10);r.contract_ok=0;
 T(nf7f_apply(&w,r,yes,&disk)==NF7F_DENIED);
 r=request(&w,12,NF18B3_DOOR,10);r.uncertain=1;
 T(nf7f_apply(&w,r,yes,&disk)==NF7F_PENDING);
 r=request(&w,13,NF18B3_DOOR,10);T(nf7f_apply(&w,r,no,NULL)==NF7F_PUBLISH_FAILED);
 T(memcmp(&w,&old,sizeof w)==0);
 w.world_epoch=UINT32_MAX;r=request(&w,14,NF18B3_DOOR,10);old=w;
 T(nf7f_apply(&w,r,yes,&disk)==NF7F_FULL);T(memcmp(&w,&old,sizeof w)==0);
 (void)s;return true;
}
static uint32_t rng(uint32_t *v){*v^=*v<<13;*v^=*v>>17;*v^=*v<<5;return *v;}
static bool batch(uint32_t seed,unsigned count,unsigned mode,unsigned *applied){
 for(unsigned i=0;i<count;i++){
  uint32_t u=rng(&seed);Nf7fWorld w,disk,old;nf7f_init(&w);disk=w;
  Nf18b3Kind kind=(Nf18b3Kind)((u>>4)%7u);
  int32_t arg=kind==NF18B3_DOOR?10:kind==NF18B3_PUSH?10:
       kind==NF18B3_REPAIR?3:kind==NF18B3_CARRY?1:0;
  Nf7fRequest r=request(&w,i+1,kind,arg);
  /* Interpret a simple independent specification for the initial state. */
  Nf7fStatus oracle=NF7F_APPLIED;
  if(kind==NF18B3_INTERRUPT)oracle=NF7F_DENIED;
  if(mode==1){r.uncertain=(uint8_t)((u>>11)&1u);r.contract_ok=(uint8_t)((u>>12)&1u);}
  if(mode==2){w.door_obstructed=1;w.door_obstacle_low=5;w.door_obstacle_high=6;}
  if(mode==3){if(i%7==0){w.world_epoch=UINT32_MAX;}else if(i%7==1){r.material_epoch++;}
   else if(i%7==2){w.receipt_count=NF7F_RECEIPTS;}
   else if(i%7==3){w.supplies=0;}
   else if(i%7==4){w.power_known=0;}
   else if(i%7==5){r.uncertain=1;}}
  /* Fixed precedence of fail closed stage ordering. */
  if(r.world_epoch!=w.world_epoch||r.material_epoch!=w.material_epoch||r.contract_epoch!=w.contract_epoch)oracle=NF7F_STALE;
  else if(r.uncertain)oracle=NF7F_PENDING;
  else if(!r.physical_ok||!r.contract_ok)oracle=NF7F_DENIED;
  else if(w.receipt_count==NF7F_RECEIPTS||w.world_epoch==UINT32_MAX||w.material_epoch==UINT32_MAX||w.tick==UINT32_MAX)oracle=NF7F_FULL;
  else if(kind==NF18B3_DOOR&&w.door_obstructed)oracle=NF7F_PENDING;
  else if(kind==NF18B3_SWITCH&&!w.power_known)oracle=NF7F_PENDING;
  else if(kind==NF18B3_REPAIR&&w.supplies<3)oracle=NF7F_DENIED;
  else if(kind==NF18B3_INTERRUPT)oracle=NF7F_DENIED;
  old=w;disk=w;
  Nf7fStatus got=nf7f_apply(&w,r,yes,&disk);
  T(got==oracle);
  if(got==NF7F_APPLIED){(*applied)++;
   T(w.receipt_count==1);T(w.world_epoch==old.world_epoch+1);
   T(memcmp(&w,&disk,sizeof w)==0);
   T(w.actor_momentum+w.object_momentum==old.actor_momentum+old.object_momentum);
   T(w.health+w.supplies==old.health+old.supplies);
   T(w.material_epoch==old.material_epoch+1);
  }else T(memcmp(&w,&old,sizeof w)==0);
 }
 return true;
}
int main(void){clock_t start=clock();unsigned applied=0;
 if(!fixtures())return 1;
 struct {const char *name;uint32_t seed;unsigned n;unsigned mode;} classes[]={
  {"canonical",0xF0100001u,128,0},
  {"randomized",0xF0100002u,2048,1},
  {"pathological",0xF0100003u,512,2},
  {"boundary",0xF0100004u,512,3},
  {"heldout",0xF010700Au,2048,1}};
 unsigned total=0;
 for(size_t j=0;j<sizeof(classes)/sizeof(classes[0]);++j){
  if(!batch(classes[j].seed,classes[j].n,classes[j].mode,&applied))return 1;
  printf("%s cases=%u OK\n",classes[j].name,classes[j].n);total+=classes[j].n;
 }
 printf("cases=%u positive=%u checks=%u cpu_seconds=%.6f\n",total,applied,checks,(double)(clock()-start)/CLOCKS_PER_SEC);
 return 0;
}
