#include "nf_physical18b7.h"
#include <limits.h>
#include <string.h>

void nf7f_init(Nf7fWorld *w){
 if(!w)return;
 memset(w,0,sizeof(*w));
 w->world_epoch=w->material_epoch=w->contract_epoch=w->tick=1;
 w->door_latched=1;w->power_known=1;w->powered=1;
 w->floor_supported=1;w->health=50;w->supplies=20;
}
static bool same(Nf7fRequest a,Nf7fRequest b){
 return a.actor==b.actor&&a.action==b.action&&a.kind==b.kind&&
 a.argument==b.argument&&a.world_epoch==b.world_epoch&&
 a.material_epoch==b.material_epoch&&a.contract_epoch==b.contract_epoch&&
 a.physical_ok==b.physical_ok&&a.contract_ok==b.contract_ok&&a.uncertain==b.uncertain;
}
static bool path_blocked(const Nf7fWorld *w,int32_t end){
 if(!w->door_obstructed)return false;
 const int32_t low=w->door_position<end?w->door_position:end;
 const int32_t high=w->door_position>end?w->door_position:end;
 return low<=w->door_obstacle_high&&high>=w->door_obstacle_low;
}
Nf7fStatus nf7f_apply(Nf7fWorld *w,Nf7fRequest r,Nf7fPublish pub,void *ctx){
 if(!w||!pub||!r.actor||!r.action||r.kind>=NF18B3_KIND_COUNT||
    r.physical_ok>1||r.contract_ok>1||r.uncertain>1||
    w->receipt_count>NF7F_RECEIPTS||!w->world_epoch||!w->material_epoch||
    !w->contract_epoch||!w->tick)return NF7F_INVALID;
 for(uint32_t i=0;i<w->receipt_count;++i){
  Nf7fRequest old=w->receipts[i].request;
  if(old.actor==r.actor&&old.action==r.action)return same(old,r)?NF7F_DUPLICATE:NF7F_INVALID;
 }
 if(r.world_epoch!=w->world_epoch||r.material_epoch!=w->material_epoch||
    r.contract_epoch!=w->contract_epoch)return NF7F_STALE;
 if(r.uncertain)return NF7F_PENDING;
 if(!r.physical_ok||!r.contract_ok)return NF7F_DENIED;
 if(w->receipt_count==NF7F_RECEIPTS||w->world_epoch==UINT32_MAX||
    w->material_epoch==UINT32_MAX||w->tick==UINT32_MAX)return NF7F_FULL;
 Nf7fWorld n=*w;
 switch(r.kind){
 case NF18B3_DOOR:{
  /* Positive or negative bounded displacement, not a teleport target. */
  if(r.argument==0||r.argument>10||r.argument< -10)return NF7F_INVALID;
  if((r.argument>0&&n.door_position>100-r.argument)||
     (r.argument<0&&n.door_position< -r.argument))return NF7F_DENIED;
  const int32_t end=n.door_position+r.argument;
  if(path_blocked(&n,end))return NF7F_PENDING;
  n.door_position=end;
  n.door_latched=(uint8_t)(end==0||end==100);
  break;
 }
 case NF18B3_SWITCH:
  if(!n.power_known)return NF7F_PENDING;
  if(!n.powered||r.argument!=0)return NF7F_DENIED;
  n.switch_on=(uint8_t)!n.switch_on;
  break;
 case NF18B3_PICKUP:
  if(r.argument!=0||n.held_by||!n.floor_supported)return NF7F_DENIED;
  n.held_by=r.actor;n.floor_supported=0;
  break;
 case NF18B3_PUSH:
  if(n.held_by||r.argument==0||r.argument>1000||r.argument< -1000)return NF7F_DENIED;
  if((r.argument>0&&(n.actor_momentum<INT64_MIN+r.argument||n.object_momentum>INT64_MAX-r.argument))||
     (r.argument<0&&(n.actor_momentum>INT64_MAX+r.argument||n.object_momentum<INT64_MIN-r.argument)))return NF7F_FULL;
  n.actor_momentum-=r.argument;n.object_momentum+=r.argument; /* Opposite impulses in an isolated 1D model. */
  break;
 case NF18B3_CARRY:
  if(r.argument==1){
   if(n.held_by||!n.floor_supported)return NF7F_DENIED;
   n.held_by=r.actor;n.floor_supported=0;
  }else if(r.argument==0){
   if(n.held_by!=r.actor)return NF7F_DENIED;
   n.held_by=0;n.floor_supported=1;
  }else return NF7F_INVALID;
  break;
 case NF18B3_REPAIR:
  if(r.argument<=0||r.argument>10||n.health>100u-(uint32_t)r.argument||n.supplies<(uint32_t)r.argument)return NF7F_DENIED;
  n.supplies-=(uint32_t)r.argument;n.health+=(uint32_t)r.argument;
  break;
 case NF18B3_INTERRUPT:
  /* Explicit new compensation event; never erase existing receipt/history. */
  if(r.argument!=0||n.held_by!=r.actor||n.interrupted_actions==UINT32_MAX)return NF7F_DENIED;
  n.held_by=0;n.floor_supported=1;n.interrupted_actions++;
  break;
 default:return NF7F_INVALID;
 }
 n.world_epoch++;n.material_epoch++;n.tick++;
 n.receipts[n.receipt_count++]=(Nf7fReceipt){r,n.world_epoch};
 if(!pub(ctx,&n))return NF7F_PUBLISH_FAILED;
 *w=n;
 return NF7F_APPLIED;
}
