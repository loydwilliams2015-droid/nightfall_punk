#include "nf_v18c2_h9_auth.h"
#include <stddef.h>
/* Research-only owner-private slots; bounded, single process. Not thread-safe
 * or crash durable. No requester can manufacture an issued authorization. */
#define H12_SLOTS 64u
typedef struct {const Nf18a6Runtime *owner;uint32_t action,tick,revision,material;bool issued;} Slot;
static Slot slots[H12_SLOTS];
static Slot *lookup(const Nf18a6Runtime *r){
 for(size_t i=0;i<H12_SLOTS;i++)if(slots[i].owner==r)return &slots[i];
 return NULL;
}
static Slot *allocate(const Nf18a6Runtime *r){
 Slot *s=lookup(r);if(s)return s;
 for(size_t i=0;i<H12_SLOTS;i++)if(!slots[i].issued){
  slots[i]=(Slot){.owner=r};return &slots[i];
 }
 return NULL;
}
void nf_c2_h9_revoke(const Nf18a6Runtime *r){
 Slot *s=lookup(r);if(s)s->issued=false;
}
static bool matches(const NfWorld *w,const Nf18a6Runtime *r,uint32_t action,bool active){
 const Slot *s=lookup(r);
 return w&&r&&s&&s->issued&&s->action==action&&
  w->tick_owner_context==r&&w->tick_owner&&
  (!w->tick_in_progress||active)&&
  w->tick==r->last_commit_tick&&w->tick==s->tick&&
  r->physical.world.tick==w->tick&&
  r->physical.world.revision==s->revision&&
  r->physical.world.grid.global_revision==s->material;
}
bool nf_c2_h9_valid(const NfWorld *w,const Nf18a6Runtime *r,uint32_t action){
 return matches(w,r,action,true);
}
bool nf_c2_h9_take_prepublication(const NfWorld *w,const Nf18a6Runtime *r,uint32_t action){
 bool result=w&&w->tick_in_progress&&matches(w,r,action,true);
 nf_c2_h9_revoke(r);return result;
}
NfC2WitnessStatus nf_c2_h9_issue(NfWorld *w,Nf18a6Runtime *r,
 NfMa03Provider provider,void *ctx,uint32_t action,NfVec3 point){
 if(!r)return NF_C2W_STALE;
 nf_c2_h9_revoke(r);
 if(!w||!provider||!action)return NF_C2W_DENIED;
 const uint32_t epoch=r->physical.world.grid.global_revision;
 NfC2WitnessStatus result=nf_c2_witness(w,r,provider,ctx,action,point,epoch);
 if(result!=NF_C2W_READY)return result;
 if(w->tick_owner_context!=r||w->tick_in_progress||!w->tick_owner||
    w->tick!=r->last_commit_tick||r->physical.world.tick!=w->tick||
    epoch!=r->physical.world.grid.global_revision)return NF_C2W_STALE;
 Slot *s=allocate(r);if(!s)return NF_C2W_PENDING;
 *s=(Slot){.owner=r,.action=action,.tick=(uint32_t)w->tick,
  .revision=r->physical.world.revision,.material=epoch,.issued=true};
 return NF_C2W_READY;
}
bool nf_c2_h9_consume(NfWorld *w,Nf18a6Runtime *r,uint32_t action){
 bool ok=w&&!w->tick_in_progress&&matches(w,r,action,false);
 nf_c2_h9_revoke(r);return ok;
}
