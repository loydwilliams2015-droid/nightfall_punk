#include "nf_v18c2_a6_stage.h"
#include <string.h>
Nf18c2StageStatus nf18c2_a6_stage(const NfWorld *w,const Nf18a6Runtime *r,
 const NfMulti18c1World *island,const NfMulti18c1Proposal *p,size_t n,
 const Nf18a4Constraint *contacts,size_t nc,float dt,
 uint32_t action,Nf18b3Kind kind,bool b2_granted,Nf18c2A6Stage *out){
 if(out)memset(out,0,sizeof *out);
 if(!w||!r||!island||!out||!action||kind>=NF18B3_KIND_COUNT||
    w->tick_owner_context!=r||!w->tick_owner||w->tick_in_progress||
    w->tick!=r->last_commit_tick||island->tick!=w->tick||
    island->revision!=r->physical.world.revision||
    r->physical.world.tick!=w->tick)return NF18C2_STAGE_STALE;
 if(!b2_granted)return NF18C2_STAGE_DENIED;
 /* Real A5 source witness: absence/uncertainty cannot authorize material edit.
  * This H3 gate admits NO material-edit operations pending an A5 verifier. */
 if(kind!=NF18B3_PUSH)return NF18C2_STAGE_PENDING;
 if(island->count<2||n==0||!p||p[0].actor!=r->actor_id)return NF18C2_STAGE_DENIED;
 NfMulti18c1Candidate staged={0};
 NfMulti18c1Status status=nf_multi18c1_stage(island,p,n,contacts,nc,dt,&staged);
 if(status==NF_MULTI_PENDING)return NF18C2_STAGE_PENDING;
 if(status!=NF_MULTI_COMMITTED)return NF18C2_STAGE_DENIED;
 out->source_tick=(uint32_t)w->tick;out->source_revision=island->revision;
 out->material_epoch=r->physical.world.grid.global_revision;
 out->actor=r->actor_id;out->action=action;out->kind=kind;out->physical=staged;
 return NF18C2_STAGE_READY;
}
