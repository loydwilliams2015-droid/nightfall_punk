#include "nf_v18c2_h9_auth.h"
NfC2WitnessStatus nf_c2_h9_issue(NfWorld *w,Nf18a6Runtime *r,
 NfMa03Provider provider,void *ctx,uint32_t action,NfVec3 point){
 if(!r)return NF_C2W_STALE;
 r->h9_issued=0;
 if(!w||!provider||!action)return NF_C2W_DENIED;
 const uint32_t epoch=r->physical.world.grid.global_revision;
 NfC2WitnessStatus s=nf_c2_witness(w,r,provider,ctx,action,point,epoch);
 if(s!=NF_C2W_READY)return s;
 /* Recheck after externally supplied server snapshot callback. A6 retains
    exclusive slot; no credential accepted from the action request itself. */
 if(w->tick_owner_context!=r||w->tick_in_progress||
    w->tick!=r->last_commit_tick||
    r->physical.world.tick!=w->tick||
    epoch!=r->physical.world.grid.global_revision)return NF_C2W_STALE;
 r->h9_action=action;r->h9_world_tick=(uint32_t)w->tick;
 r->h9_revision=r->physical.world.revision;
 r->h9_material_epoch=epoch;r->h9_issued=1;
 return NF_C2W_READY;
}
bool nf_c2_h9_consume(NfWorld *w,Nf18a6Runtime *r,uint32_t action){
 if(!r)return false;
 const bool good=w && r->h9_issued && action==r->h9_action &&
    w->tick_owner_context==r && w->tick_owner && !w->tick_in_progress &&
    w->tick==r->last_commit_tick && w->tick==r->h9_world_tick &&
    r->physical.world.tick==w->tick &&
    r->physical.world.revision==r->h9_revision &&
    r->physical.world.grid.global_revision==r->h9_material_epoch;
 r->h9_issued=0;return good;
}
