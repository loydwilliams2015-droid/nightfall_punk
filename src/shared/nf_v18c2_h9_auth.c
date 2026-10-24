#include "nf_v18c2_h9_auth.h"
#include <stdlib.h>
#include <stdatomic.h>
/* Opaque per-A6 allocation, no global slot registry. Lock protects grant
 * records, not arbitrary concurrent mutation of the parent world. */
struct Nf18c2AuthState {
 atomic_flag lock;
 uint32_t action,tick,revision,material;
 bool issued;
};
static void enter(struct Nf18c2AuthState *s){
 while(atomic_flag_test_and_set_explicit(&s->lock,memory_order_acquire)){}
}
static void leave(struct Nf18c2AuthState *s){
 atomic_flag_clear_explicit(&s->lock,memory_order_release);
}
bool nf_c2_h9_owner_init(Nf18a6Runtime *r){
 if(!r||r->auth_state)return false;
 struct Nf18c2AuthState *s=calloc(1,sizeof *s);
 if(!s)return false;
 atomic_flag_clear(&s->lock);
 r->auth_state=s;
 return true;
}
void nf_c2_h9_owner_destroy(Nf18a6Runtime *r){
 if(!r||!r->auth_state)return;
 struct Nf18c2AuthState *s=r->auth_state;
 enter(s);
 s->issued=false;
 leave(s);
 r->auth_state=NULL;
 free(s);
}
void nf_c2_h9_revoke(const Nf18a6Runtime *r){
 if(!r||!r->auth_state)return;
 struct Nf18c2AuthState *s=r->auth_state;
 enter(s);s->issued=false;leave(s);
}
static bool matches(const NfWorld *w,const Nf18a6Runtime *r,
 uint32_t action,bool allow_tick){
 if(!w||!r||!r->auth_state)return false;
 struct Nf18c2AuthState *s=r->auth_state;
 enter(s);
 const bool valid=s->issued&&s->action==action&&
  w->tick_owner_context==r&&w->tick_owner&&
  (!w->tick_in_progress||allow_tick)&&
  w->tick==r->last_commit_tick&&w->tick==s->tick&&
  r->physical.world.tick==w->tick&&
  r->physical.world.revision==s->revision&&
  r->physical.world.grid.global_revision==s->material;
 leave(s);return valid;
}
bool nf_c2_h9_valid(const NfWorld *w,const Nf18a6Runtime *r,uint32_t action){
 return matches(w,r,action,true);
}
bool nf_c2_h9_take_prepublication(const NfWorld *w,
 const Nf18a6Runtime *r,uint32_t action){
 if(!r||!r->auth_state||!w)return false;
 struct Nf18c2AuthState *s=r->auth_state;
 enter(s);
 const bool valid=w->tick_in_progress&&s->issued&&s->action==action&&
  w->tick_owner_context==r&&w->tick_owner&&
  w->tick==r->last_commit_tick&&w->tick==s->tick&&
  r->physical.world.tick==w->tick&&
  r->physical.world.revision==s->revision&&
  r->physical.world.grid.global_revision==s->material;
 s->issued=false;leave(s);return valid;
}
NfC2WitnessStatus nf_c2_h9_issue(NfWorld *w,Nf18a6Runtime *r,
 NfMa03Provider provider,void *ctx,uint32_t action,NfVec3 point){
 if(!r)return NF_C2W_STALE;
 if(!r->auth_state && !nf_c2_h9_owner_init(r))return NF_C2W_PENDING;
 nf_c2_h9_revoke(r);
 if(!w||!provider||!action)return NF_C2W_DENIED;
 const uint32_t epoch=r->physical.world.grid.global_revision;
 NfC2WitnessStatus result=nf_c2_witness(w,r,provider,ctx,action,point,epoch);
 if(result!=NF_C2W_READY)return result;
 if(w->tick_owner_context!=r||w->tick_in_progress||!w->tick_owner||
    w->tick!=r->last_commit_tick||r->physical.world.tick!=w->tick||
    epoch!=r->physical.world.grid.global_revision)return NF_C2W_STALE;
 struct Nf18c2AuthState *s=r->auth_state;
 enter(s);
 s->action=action;s->tick=(uint32_t)w->tick;
 s->revision=r->physical.world.revision;
 s->material=epoch;s->issued=true;
 leave(s);return NF_C2W_READY;
}
bool nf_c2_h9_consume(NfWorld *w,Nf18a6Runtime *r,uint32_t action){
 if(!w||!r||!r->auth_state)return false;
 struct Nf18c2AuthState *s=r->auth_state;
 enter(s);
 const bool valid=!w->tick_in_progress&&s->issued&&
  s->action==action&&w->tick_owner_context==r&&w->tick_owner&&
  w->tick==r->last_commit_tick&&w->tick==s->tick&&
  r->physical.world.tick==w->tick&&
  r->physical.world.revision==s->revision&&
  r->physical.world.grid.global_revision==s->material;
 s->issued=false;leave(s);return valid;
}
