#include "nf_multi18b7.h"
#include <limits.h>
#include <string.h>

void nf_m7_init(NfM7World *w){
 if(w){memset(w,0,sizeof(*w));w->epoch=1;w->tick=1;w->cursor=1;}
}
static int find_actor(const NfM7World *w,uint32_t id){
 for(size_t i=0;i<w->actor_count;++i)if(w->actors[i].id==id)return (int)i;
 return -1;
}
static int find_slot(const NfM7World *w,uint32_t id){
 for(size_t i=0;i<w->slot_count;++i)if(w->slots[i].id==id)return (int)i;
 return -1;
}
static bool occupant(const NfM7Slot *s,uint32_t actor){
 for(uint8_t i=0;i<s->occupant_count;++i)if(s->occupant[i]==actor)return true;
 return false;
}
bool nf_m7_valid(const NfM7World *w){
 if(!w||!w->epoch||!w->tick||!w->cursor||
    w->actor_count>NF_M7_ACTORS||w->slot_count>NF_M7_SLOTS||
    w->receipt_count>NF_M7_RECEIPTS)return false;
 for(size_t i=0;i<w->actor_count;++i){
  const NfM7Actor *a=&w->actors[i];
  if(!a->id||!a->session||a->active>1)return false;
  for(size_t j=0;j<i;++j)if(w->actors[j].id==a->id)return false;
 }
 for(size_t i=0;i<w->slot_count;++i){
  const NfM7Slot *s=&w->slots[i];
  if(!s->id||!s->revision||s->capacity==0||s->capacity>NF_M7_CAPACITY||
     s->occupant_count>s->capacity)return false;
  for(size_t j=0;j<i;++j)if(w->slots[j].id==s->id)return false;
  for(uint8_t j=0;j<s->occupant_count;++j){
   if(!s->occupant[j]||find_actor(w,s->occupant[j])<0)return false;
   for(uint8_t k=0;k<j;++k)if(s->occupant[j]==s->occupant[k])return false;
  }
 }
 for(size_t i=0;i<w->receipt_count;++i){
  const NfM7Request *a=&w->receipts[i].request;
  if(!a->actor||!a->session||!a->action)return false;
  for(size_t j=0;j<i;++j){
   const NfM7Request *b=&w->receipts[j].request;
   if(a->actor==b->actor&&a->session==b->session&&a->action==b->action)return false;
  }
 }
 return true;
}
bool nf_m7_add_actor(NfM7World *w,NfM7Actor a){
 if(!nf_m7_valid(w)||w->actor_count==NF_M7_ACTORS||
    !a.id||!a.session||a.active>1||find_actor(w,a.id)>=0)return false;
 w->actors[w->actor_count++]=a;return true;
}
bool nf_m7_add_slot(NfM7World *w,NfM7Slot s){
 if(!nf_m7_valid(w)||w->slot_count==NF_M7_SLOTS||
    !s.id||!s.revision||s.capacity==0||s.capacity>NF_M7_CAPACITY||
    s.occupant_count!=0||find_slot(w,s.id)>=0)return false;
 w->slots[w->slot_count++]=s;return true;
}
static bool same(NfM7Request a,NfM7Request b){
 return a.actor==b.actor&&a.session==b.session&&a.action==b.action&&
  a.slot==b.slot&&a.epoch==b.epoch&&a.slot_revision==b.slot_revision&&
  a.operation==b.operation&&a.physical==b.physical&&
  a.contract==b.contract&&a.consent==b.consent;
}
static bool earlier(uint32_t a,uint32_t b,NfM7Policy p,uint32_t cursor){
 if(p==NF_M7_ROTATING){
  bool aa=a>=cursor,bb=b>=cursor;
  if(aa!=bb)return aa;
 }
 return a<b;
}
static bool publication_order(NfM7Request a,NfM7Request b){
 if(a.slot!=b.slot)return a.slot<b.slot;
 if(a.operation!=b.operation)return a.operation==NF_M7_RELEASE;
 if(a.actor!=b.actor)return a.actor<b.actor;
 return a.action<b.action;
}
bool nf_m7_step(NfM7World *w,const NfM7Request *r,size_t n,NfM7Policy policy,
                NfM7Publisher publish,void *ctx,NfM7Status *result){
 if(!nf_m7_valid(w)||!publish||!result||(n&&!r)||n>NF_M7_BATCH||
    (policy!=NF_M7_FIXED_ID&&policy!=NF_M7_ROTATING))return false;
 for(size_t i=0;i<n;++i){
  NfM7Request x=r[i];result[i]=NF_M7_INVALID;
  if(!x.actor||!x.session||!x.action||!x.slot||
     (x.operation!=NF_M7_CLAIM&&x.operation!=NF_M7_RELEASE)||
     x.physical>NF_M7_UNKNOWN||x.contract>NF_M7_UNKNOWN||x.consent>1)continue;
  /* Same action or competing same-actor same-slot intents within one tick:
     invalidate BOTH, regardless of arrival ordering. */
  bool alias=false;
  for(size_t j=0;j<n;++j)if(i!=j&&r[j].actor==x.actor&&r[j].session==x.session&&
   (r[j].action==x.action||r[j].slot==x.slot))alias=true;
  if(alias)continue;
  const int actor=find_actor(w,x.actor);
  if(actor<0){result[i]=NF_M7_REFUSED;continue;}
  if(w->actors[actor].session!=x.session){result[i]=NF_M7_STALE;continue;}
  if(!w->actors[actor].active){result[i]=NF_M7_REFUSED;continue;}
  bool historical=false;
  for(size_t j=0;j<w->receipt_count;++j){
   NfM7Request old=w->receipts[j].request;
   if(old.actor==x.actor&&old.session==x.session&&old.action==x.action){
    result[i]=same(old,x)?NF_M7_DUPLICATE:NF_M7_INVALID;historical=true;break;
   }
  }
  if(historical)continue;
  if(x.epoch!=w->epoch){result[i]=NF_M7_STALE;continue;}
  const int idx=find_slot(w,x.slot);
  if(idx<0){result[i]=NF_M7_INVALID;continue;}
  const NfM7Slot *s=&w->slots[idx];
  if(x.slot_revision!=s->revision){result[i]=NF_M7_STALE;continue;}
  if(x.physical==NF_M7_UNKNOWN||x.contract==NF_M7_UNKNOWN){
   result[i]=NF_M7_PENDING;continue;
  }
  if(x.physical!=NF_M7_YES||x.contract!=NF_M7_YES||!x.consent){
   result[i]=NF_M7_REFUSED;continue;
  }
  if(x.operation==NF_M7_CLAIM&&occupant(s,x.actor)){result[i]=NF_M7_REFUSED;continue;}
  if(x.operation==NF_M7_RELEASE&&!occupant(s,x.actor)){result[i]=NF_M7_REFUSED;continue;}
  result[i]=NF_M7_GRANTED; /* provisional: never physics or durable WAL */
 }
 /* Capacity is measured against S_t: a release cannot create same-tick
    speculative free capacity for a new claimant. */
 for(size_t k=0;k<w->slot_count;++k){
  const NfM7Slot *s=&w->slots[k];
  uint8_t free_seats=(uint8_t)(s->capacity-s->occupant_count);
  bool chosen[NF_M7_BATCH]={false};
  for(uint8_t seat=0;seat<free_seats;++seat){
   int best=-1;
   for(size_t j=0;j<n;++j)if(result[j]==NF_M7_GRANTED&&
      r[j].operation==NF_M7_CLAIM&&r[j].slot==s->id&&!chosen[j]){
    if(best<0||earlier(r[j].actor,r[best].actor,policy,w->cursor))best=(int)j;
   }
   if(best<0)break;
   chosen[best]=true;
  }
  for(size_t j=0;j<n;++j)if(result[j]==NF_M7_GRANTED&&
      r[j].operation==NF_M7_CLAIM&&r[j].slot==s->id&&!chosen[j])
   result[j]=NF_M7_CONTESTED;
 }
 size_t accepted=0;
 for(size_t j=0;j<n;++j)if(result[j]==NF_M7_GRANTED)accepted++;
 if(!accepted)return true;
 bool capacity_overflow= w->epoch==UINT32_MAX||w->tick==UINT32_MAX||
                         accepted>NF_M7_RECEIPTS-w->receipt_count;
 for(size_t k=0;k<w->slot_count;++k)if(w->slots[k].revision==UINT32_MAX){
  for(size_t j=0;j<n;++j)if(result[j]==NF_M7_GRANTED&&r[j].slot==w->slots[k].id)
   capacity_overflow=true;
 }
 if(capacity_overflow){
  for(size_t j=0;j<n;++j)if(result[j]==NF_M7_GRANTED)result[j]=NF_M7_FULL;
  return true;
 }
 NfM7World next=*w;
 bool written[NF_M7_BATCH]={false};
 uint32_t last_actor=0;
 for(size_t step=0;step<accepted;++step){
  int best=-1;
  for(size_t j=0;j<n;++j)if(result[j]==NF_M7_GRANTED&&!written[j]&&
    (best<0||publication_order(r[j],r[best])))best=(int)j;
  if(best<0)return false;
  written[best]=true;NfM7Request x=r[best];
  const int k=find_slot(&next,x.slot);
  if(k<0)return false;
  NfM7Slot *slot=&next.slots[k];
  if(x.operation==NF_M7_RELEASE){
   uint8_t j=0;while(j<slot->occupant_count&&slot->occupant[j]!=x.actor)j++;
   if(j>=slot->occupant_count)return false;
   for(uint8_t t=j;t+1<slot->occupant_count;++t)slot->occupant[t]=slot->occupant[t+1];
   slot->occupant[--slot->occupant_count]=0;
  }else{
   if(slot->occupant_count>=slot->capacity)return false;
   slot->occupant[slot->occupant_count++]=x.actor;
   /* Cursor progresses only after a real granted claim. */
   last_actor=x.actor;
  }
  next.receipts[next.receipt_count++]=(NfM7Receipt){x,w->epoch+1};
 }
 for(size_t k=0;k<next.slot_count;++k){
  bool touched=false;
  for(size_t j=0;j<n;++j)if(result[j]==NF_M7_GRANTED&&r[j].slot==next.slots[k].id)
   touched=true;
  if(touched)next.slots[k].revision++;
 }
 next.epoch++;next.tick++;
 if(policy==NF_M7_ROTATING&&last_actor)
  next.cursor=last_actor==UINT32_MAX?1:last_actor+1;
 if(!publish(ctx,&next)){
  for(size_t j=0;j<n;++j)if(result[j]==NF_M7_GRANTED)result[j]=NF_M7_PUBLISH_FAILED;
  return true;
 }
 *w=next;return true;
}
