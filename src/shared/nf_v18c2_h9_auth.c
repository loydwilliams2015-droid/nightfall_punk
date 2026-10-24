#include "nf_v18c2_h9_auth.h"
#include <stdlib.h>
#include <stdatomic.h>
/* A6 runtime owns lifetime gate. No global registry, no pointer dereference
   outside gate. Shutdown takes the same gate and drains every active call. */
struct Nf18c2AuthState {uint32_t action,tick,revision,material;bool issued;};
static void lock(Nf18a6Runtime *r){
 while(atomic_flag_test_and_set_explicit(&r->auth_lifecycle_lock,memory_order_acquire)){}
}
static void unlock(Nf18a6Runtime *r){
 atomic_flag_clear_explicit(&r->auth_lifecycle_lock,memory_order_release);
}
static bool init_locked(Nf18a6Runtime *r){
 if(atomic_load(&r->auth_shutdown)||r->auth_state)return false;
 r->auth_state=calloc(1,sizeof *r->auth_state);return r->auth_state!=NULL;
}
bool nf_c2_h9_owner_init(Nf18a6Runtime *r){
 if(!r)return false;
 lock(r);bool ok=init_locked(r);unlock(r);return ok;
}
void nf_c2_h9_owner_destroy(Nf18a6Runtime *r){
 if(!r)return;
 lock(r);
 atomic_store(&r->auth_shutdown,true);
 free(r->auth_state);r->auth_state=NULL;
 unlock(r);
}
void nf_c2_h9_revoke(const Nf18a6Runtime *r){
 if(!r)return;
 Nf18a6Runtime *owner=(Nf18a6Runtime *)r;
 lock(owner);if(r->auth_state)r->auth_state->issued=false;unlock(owner);
}
static bool matches(const NfWorld *w,const Nf18a6Runtime *r,
 const struct Nf18c2AuthState *s,uint32_t action,bool active){
 return w&&r&&s&&s->issued&&s->action==action&&
  w->tick_owner_context==r&&w->tick_owner&&
  (!w->tick_in_progress||active)&&w->tick==r->last_commit_tick&&
  w->tick==s->tick&&r->physical.world.tick==w->tick&&
  r->physical.world.revision==s->revision&&
  r->physical.world.grid.global_revision==s->material;
}
bool nf_c2_h9_valid(const NfWorld *w,const Nf18a6Runtime *r,uint32_t action){
 if(!r)return false;
 Nf18a6Runtime *owner=(Nf18a6Runtime *)r;
 lock(owner);bool ok=!atomic_load(&r->auth_shutdown)&&matches(w,r,r->auth_state,action,true);
 unlock(owner);return ok;
}
bool nf_c2_h9_take_prepublication(const NfWorld *w,const Nf18a6Runtime *r,uint32_t action){
 if(!r)return false;
 Nf18a6Runtime *owner=(Nf18a6Runtime *)r;
 lock(owner);
 bool ok=!atomic_load(&r->auth_shutdown)&&w&&w->tick_in_progress&&
   matches(w,r,r->auth_state,action,true);
 if(r->auth_state)r->auth_state->issued=false;
 unlock(owner);return ok;
}
NfC2WitnessStatus nf_c2_h9_issue(NfWorld *w,Nf18a6Runtime *r,
 NfMa03Provider provider,void *ctx,uint32_t action,NfVec3 point){
 if(!r)return NF_C2W_STALE;
 lock(r);
 if(atomic_load(&r->auth_shutdown)||!w||!w->tick_owner||
    w->tick_owner_context!=r||w->tick_in_progress){
  unlock(r);return NF_C2W_STALE;
 }
 if(!r->auth_state&&!init_locked(r)){unlock(r);return NF_C2W_PENDING;}
 r->auth_state->issued=false;
 if(!provider||!action){unlock(r);return NF_C2W_DENIED;}
 uint32_t epoch=r->physical.world.grid.global_revision;
 /* Provider callback executes inside the owner lifecycle gate. It MUST
    NOT recursively invoke H9 functions. */
 NfC2WitnessStatus result=nf_c2_witness(w,r,provider,ctx,action,point,epoch);
 if(result==NF_C2W_READY){
  if(w->tick_owner_context!=r||w->tick_in_progress||!w->tick_owner||
     w->tick!=r->last_commit_tick||r->physical.world.tick!=w->tick||
     epoch!=r->physical.world.grid.global_revision)result=NF_C2W_STALE;
  else {
   *r->auth_state=(struct Nf18c2AuthState){.action=action,.tick=(uint32_t)w->tick,
    .revision=r->physical.world.revision,.material=epoch,.issued=true};
  }
 }
 unlock(r);return result;
}
bool nf_c2_h9_consume(NfWorld *w,Nf18a6Runtime *r,uint32_t action){
 if(!r)return false;
 lock(r);
 bool ok=!atomic_load(&r->auth_shutdown)&&w&&!w->tick_in_progress&&
   matches(w,r,r->auth_state,action,false);
 if(r->auth_state)r->auth_state->issued=false;
 unlock(r);return ok;
}
