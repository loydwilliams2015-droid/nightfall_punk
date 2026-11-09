#include "nf_retc_m2_pair_tick.h"
#include "nf_retc_m2_support_tick.h"
#include "nf_retc_m2_push_tick.h"
#include "nf_retc_m2_carry_tick.h"
#include "nf_retc_m2_transition_tick.h"
#include "nf_retc_m2_contention_tick.h"
#include "nf_retc_m2_fairness_tick.h"
#include "nf_retc_m2_three_entity_tick.h"
#include "nf_retc_m2_three_contact_tick.h"
#include "nf_retc_m2_event_scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"M2 H198-H202 failure line %d: %s\n",__LINE__,#x);return 1;} }while(0)
typedef struct M2Probe {NfRetcM2PairEvidence *out;unsigned calls,success;} M2Probe;
static bool probe(const NfWorld *w,const Nf18a6Runtime *owner,float dt,void *ctx){
 M2Probe *p=(M2Probe *)ctx;
 ++p->calls;
 const bool result=nf_retc_m2_pair_stage_in_tick(w,owner,dt,p->out);
 if(result)++p->success;
 return result;
}
typedef struct M2SupportProbe {
 NfRetcM2SupportEvidence *out;
 unsigned calls,success;
} M2SupportProbe;
static bool support_probe(const NfWorld *w,const Nf18a6Runtime *owner,
                          float dt,void *context){
 M2SupportProbe *p=(M2SupportProbe *)context;
 ++p->calls;
 const bool ok=nf_retc_m2_support_stage_in_tick(w,owner,dt,p->out);
 if(ok)++p->success;
 return ok;
}
typedef struct M2PushProbe {NfRetcM2PushEvidence *out;unsigned calls,accepted;} M2PushProbe;
static bool push_probe(const NfWorld *w,const Nf18a6Runtime *owner,float dt,void *context){
 M2PushProbe *p=(M2PushProbe *)context;
 ++p->calls;
 const bool accepted=nf_retc_m2_push_stage_in_tick(w,owner,dt,p->out);
 if(accepted)++p->accepted;
 return accepted;
}
typedef struct M2CarryProbe {NfRetcM2CarryEvidence *out;unsigned calls,accepted;} M2CarryProbe;
static bool carry_probe(const NfWorld *w,const Nf18a6Runtime *owner,float dt,void *context){
 M2CarryProbe *p=(M2CarryProbe *)context;
 ++p->calls;
 const bool accepted=nf_retc_m2_carry_stage_in_tick(w,owner,dt,p->out);
 if(accepted)++p->accepted;
 return accepted;
}
typedef struct M2TransitionProbe {
 NfRetcM2Transition *state;
 NfRetcM2TransitionMode mode;
 unsigned calls,passed;
} M2TransitionProbe;
static bool transition_probe(const NfWorld *w,const Nf18a6Runtime *owner,float dt,void *context){
 M2TransitionProbe *p=(M2TransitionProbe *)context;
 ++p->calls;
 const bool ok=nf_retc_m2_transition_in_tick(w,owner,dt,p->mode,p->state);
 if(ok)++p->passed;
 return ok;
}
typedef struct M2ContentionProbe {
 NfRetcM2Contention *out;
 unsigned calls,passed;
} M2ContentionProbe;
static bool contention_probe(const NfWorld *w,const Nf18a6Runtime *owner,float dt,void *context){
 M2ContentionProbe *p=(M2ContentionProbe *)context;
 ++p->calls;
 const bool ok=nf_retc_m2_contend_in_tick(w,owner,dt,p->out);
 if(ok)++p->passed;
 return ok;
}
typedef struct M2FairProbe {
 NfRetcM2Fairness *state;
 unsigned calls,staged;
} M2FairProbe;
static bool fair_probe(const NfWorld *w,const Nf18a6Runtime *owner,
                       float dt,void *context){
 M2FairProbe *p=(M2FairProbe *)context;
 ++p->calls;
 const bool ok=nf_retc_m2_fair_in_tick(w,owner,dt,p->state);
 if(ok)++p->staged;
 return ok;
}
typedef struct M2ThreeProbe {NfRetcM2ThreeEntity *out;unsigned calls,passed;} M2ThreeProbe;
static bool three_probe(const NfWorld *w,const Nf18a6Runtime *owner,float dt,void *context){
 M2ThreeProbe *p=(M2ThreeProbe *)context;
 ++p->calls;
 const bool ok=nf_retc_m2_three_entity_in_tick(w,owner,dt,p->out);
 if(ok)++p->passed;
 return ok;
}
typedef struct M2ChainProbe {NfRetcM2ThreeContact *out;unsigned calls,passed;bool reverse;} M2ChainProbe;
static bool chain_probe(const NfWorld *w,const Nf18a6Runtime *owner,float dt,void *context){
 M2ChainProbe *p=(M2ChainProbe *)context;
 ++p->calls;
 const bool ok=nf_retc_m2_three_contact_ordered_in_tick(w,owner,dt,p->reverse,p->out);
 if(ok)++p->passed;
 return ok;
}
typedef struct M2EventProbe {
 NfRetcM2EventChain *out;
 unsigned calls,passed;
} M2EventProbe;
static bool event_probe(const NfWorld *w,const Nf18a6Runtime *owner,
                        float dt,void *context){
 M2EventProbe *p=(M2EventProbe *)context;
 ++p->calls;
 const bool ok=nf_retc_m2_event_chain_in_tick(w,owner,dt,p->out);
 if(ok)++p->passed;
 return ok;
}
typedef struct M2ReverseProbe {
 NfRetcM2ReverseFirst *out;
 unsigned calls,passed;
} M2ReverseProbe;
static bool reverse_probe(const NfWorld *w,const Nf18a6Runtime *owner,
                          float dt,void *context){
 M2ReverseProbe *p=(M2ReverseProbe *)context;
 ++p->calls;
 const bool ok=nf_retc_m2_reverse_first_in_tick(w,owner,dt,p->out);
 if(ok)++p->passed;
 return ok;
}
typedef struct M2ReverseSecondProbe {
 NfRetcM2ReverseSecond *out;unsigned calls,passed;
} M2ReverseSecondProbe;
static bool reverse_second_probe(const NfWorld *w,const Nf18a6Runtime *owner,
 float dt,void *ctx){
 M2ReverseSecondProbe *p=(M2ReverseSecondProbe *)ctx;
 ++p->calls;
 const bool ok=nf_retc_m2_reverse_second_in_tick(w,owner,dt,p->out);
 if(ok)++p->passed;
 return ok;
}
typedef struct M2ReverseMaterialProbe {
 NfRetcM2ReverseMaterial *out;unsigned calls,passed;
} M2ReverseMaterialProbe;
static bool reverse_material_probe(const NfWorld *w,const Nf18a6Runtime *owner,
 float dt,void *ctx){
 M2ReverseMaterialProbe *p=(M2ReverseMaterialProbe *)ctx;
 ++p->calls;
 const bool ok=nf_retc_m2_reverse_material_in_tick(w,owner,dt,p->out);
 if(ok)++p->passed;
 return ok;
}
typedef struct M2ReverseClearanceProbe {
 NfRetcM2ReverseClearance *out;unsigned calls,passed;
} M2ReverseClearanceProbe;
static bool reverse_clearance_probe(const NfWorld *w,const Nf18a6Runtime *owner,
 float dt,void *ctx){
 M2ReverseClearanceProbe *p=(M2ReverseClearanceProbe *)ctx;
 ++p->calls;
 const bool ok=nf_retc_m2_reverse_clearance_in_tick(w,owner,dt,p->out);
 if(ok)++p->passed;
 return ok;
}
typedef struct M2SimultaneousProbe {
 NfRetcM2Simultaneous *out;
 bool reverse;
 unsigned calls,passed;
} M2SimultaneousProbe;
static bool simultaneous_probe(const NfWorld *w,const Nf18a6Runtime *owner,
 float dt,void *ctx){
 M2SimultaneousProbe *p=(M2SimultaneousProbe *)ctx;
 ++p->calls;
 const bool ok=nf_retc_m2_simultaneous_in_tick(
    w,owner,dt,p->reverse,p->out);
 if(ok)++p->passed;
 return ok;
}
typedef struct M2SimultaneousMaterialProbe {
 NfRetcM2SimultaneousMaterial *out;
 bool reverse;unsigned calls,passed;
} M2SimultaneousMaterialProbe;
static bool simultaneous_material_probe(const NfWorld *w,
 const Nf18a6Runtime *owner,float dt,void *ctx){
 M2SimultaneousMaterialProbe *p=(M2SimultaneousMaterialProbe *)ctx;
 ++p->calls;
 const bool ok=nf_retc_m2_simultaneous_material_in_tick(
    w,owner,dt,p->reverse,p->out);
 if(ok)++p->passed;
 return ok;
}
typedef struct M2OffaxisProbe {
 NfRetcM2OffaxisCorner *out;
 unsigned calls,passed;
} M2OffaxisProbe;
static bool offaxis_probe(const NfWorld *w,const Nf18a6Runtime *owner,
 float dt,void *ctx){
 (void)dt;
 M2OffaxisProbe *p=(M2OffaxisProbe *)ctx;
 ++p->calls;
 const bool ok=nf_retc_m2_offaxis_corner_in_tick(w,owner,p->out);
 if(ok)++p->passed;
 return ok;
}
typedef struct M2SweptCornerProbe {
 NfRetcM2SweptCorner *out;
 unsigned calls,passed;
} M2SweptCornerProbe;
static bool swept_corner_probe(const NfWorld *w,
 const Nf18a6Runtime *owner,float dt,void *ctx){
 M2SweptCornerProbe *p=(M2SweptCornerProbe *)ctx;
 ++p->calls;
 const bool ok=nf_retc_m2_swept_corner_in_tick(w,owner,dt,p->out);
 if(ok)++p->passed;
 return ok;
}
typedef struct M2SweptFaceProbe {
 NfRetcM2SweptFace *out;
 unsigned calls,passed;
} M2SweptFaceProbe;
static bool swept_face_probe(const NfWorld *w,
 const Nf18a6Runtime *owner,float dt,void *ctx){
 M2SweptFaceProbe *p=(M2SweptFaceProbe *)ctx;
 ++p->calls;
 const bool ok=nf_retc_m2_swept_face_in_tick(w,owner,dt,p->out);
 if(ok)++p->passed;
 return ok;
}
int main(void){
 NfWorld *world=calloc(1,sizeof *world);
 Nf18a6Runtime *owner=calloc(1,sizeof *owner);
 NfRetcM2PairEvidence *e=calloc(1,sizeof *e);
 CHECK(world&&owner&&e);
 nf_world_init(world,100u);
 CHECK(nf_world_spawn_actor_with_id(world,30u,NF_FACTION_PLAYER,
       (NfVec3){0.f,0.f,0.f})==30u);
 NfActor *a=nf_world_find_actor(world,30u);
 CHECK(a);
 a->transform.velocity.x=4.f;
 Nf18a4Body crate={0};
 crate.id=3050u;crate.center=(NfVec3){.95f,.9f,0.f};
 crate.inverse_mass=1.f/24.f;
 crate.box_half=(NfVec3){.2f,.5f,.35f};
 CHECK(nf18a6_init(owner,world,30u,crate,8u,NF18A6_WORLD_BRIDGE));
 CHECK(nf18a6_bind_world_tick(world,owner,NULL,NULL,NULL));
 const uint8_t free_cells[NF18A5_CANON_VOXELS]={0};
 for(int cx=-1;cx<=0;++cx)
  for(int cz=-1;cz<=0;++cz)
   CHECK(nf18a5_load_canonical(&owner->physical.world.grid,
      cx,0,cz,free_cells,1u));
 M2Probe p={.out=e};
 owner->require_pair_research=true;
 owner->research_pair_stage=probe;
 owner->research_pair_context=&p;
 const NfVec3 before_actor=a->transform.position;
 const NfVec3 before_crate=owner->physical.object.center;
 const uint32_t revision=owner->physical.world.revision;
 const uint32_t samples=owner->physical.world.history.raw_samples;
 /* H198: real actor+dynamic crate CCD yields both body trajectories,
    authentic reciprocal A4 contact and one private A5 contact history. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(p.calls==1u&&p.success==1u&&e->staged);
 CHECK(e->result.dynamic_contact&&e->candidate.committed_receipts==1u);
 CHECK(e->candidate.actor.velocity.x<4.f&&e->candidate.object.velocity.x>0.f);
 CHECK(e->candidate.world.history.raw_samples==samples+1u);
 /* H199: repeat source-exact owner tick is deterministic. */
 const uint32_t hash=nf18a5_history_hash(&e->candidate.world.history);
 const float impulse=e->result.applied.normal_impulse.x;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(p.calls==2u&&p.success==2u&&e->staged);
 CHECK(hash==nf18a5_history_hash(&e->candidate.world.history));
 CHECK(impulse==e->result.applied.normal_impulse.x);
 /* H200: real actor/source mismatch refuses stage. */
 a->transform.velocity.x+=1.f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(p.calls==3u&&p.success==2u&&!e->staged);
 a->transform.velocity.x-=1.f;
 /* H201: source canonical solid obstruction refuses contact history. */
 Nf18a5Chunk *chunk=NULL;
 for(size_t i=0;i<owner->physical.world.grid.count;++i){
  Nf18a5Chunk *c=&owner->physical.world.grid.chunks[i];
  if(c->x==0&&c->y==0&&c->z==0)chunk=c;
 }
 CHECK(chunk);
 const uint8_t cell=chunk->canonical[0];
 chunk->canonical[0]=NF18A5_SOLID;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(p.calls==4u&&p.success==2u&&!e->staged);
 chunk->canonical[0]=cell;
 /* H202: restored source stages one contact privately but cannot commit
    actor, object, A5 world history, A6 tick or B3 consequences. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(p.calls==5u&&p.success==3u&&e->staged);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision);
 CHECK(owner->physical.world.history.raw_samples==samples);
 CHECK(a->transform.position.x==before_actor.x&&
       a->transform.position.y==before_actor.y&&
       a->transform.position.z==before_actor.z);
 CHECK(owner->physical.object.center.x==before_crate.x&&
       owner->physical.object.center.y==before_crate.y&&
       owner->physical.object.center.z==before_crate.z);
 /* H208-H212: source-owned PUSH action, never a B2 grant. */
 NfRetcM2PushEvidence *push_effect=calloc(1,sizeof *push_effect);
 CHECK(push_effect);
 M2PushProbe pp={.out=push_effect};
 a->input.interact_held=true;
 a->input.forward=1.f;
 a->input.strafe=0.f;
 a->input.yaw_radians=0.f;
 owner->research_pair_stage=push_probe;
 owner->research_pair_context=&pp;
 /* H208 source-owned physical dynamic push and private A5 history. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(pp.calls==1u&&pp.accepted==1u&&push_effect->staged);
 CHECK(push_effect->pair.staged&&push_effect->source_object==3050u);
 CHECK(push_effect->pair.candidate.object.velocity.x>0.f&&
       push_effect->pair.candidate.world.history.raw_samples==samples+1u);
 /* H209 input release interrupts research PUSH without publication. */
 a->input.interact_held=false;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(pp.calls==2u&&pp.accepted==1u&&!push_effect->staged);
 a->input.interact_held=true;
 /* H210 absent forward intention refuses. */
 a->input.forward=0.f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(pp.calls==3u&&pp.accepted==1u&&!push_effect->staged);
 a->input.forward=1.f;
 /* H211 changed live collider mirror refuses stale physical crate. */
 NfCollider *mirror=&world->colliders[owner->object_collider_index];
 mirror->min.x+=.125f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(pp.calls==4u&&pp.accepted==1u&&!push_effect->staged);
 mirror->min.x-=.125f;
 /* H212 restored source stages exactly one private PUSH contact. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(pp.calls==5u&&pp.accepted==2u&&push_effect->staged);
 CHECK(push_effect->pair.candidate.object.velocity.x>0.f);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples);
 CHECK(a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 free(push_effect);
 /* H213-H217: no object ownership is minted; private kinematic carry
    must use the real actor input and both source-bounded swept bodies. */
 NfRetcM2CarryEvidence *carry=calloc(1,sizeof *carry);
 CHECK(carry);
 M2CarryProbe cp={.out=carry};
 owner->research_pair_stage=carry_probe;
 owner->research_pair_context=&cp;
 /* H213: conservative dual swept clearance admits bounded carry motion,
    preserving the source-relative actor/object offset. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(cp.calls==1u&&cp.accepted==1u&&carry->staged);
 CHECK(fabsf(carry->candidate.actor.center.x-
             owner->physical.actor.center.x-.5f)<.0001f);
 CHECK(fabsf(carry->candidate.object.center.x-
             owner->physical.object.center.x-.5f)<.0001f);
 CHECK(fabsf(carry->source_offset.x-.95f)<.0001f);
 CHECK(carry->candidate.world.history.raw_samples==samples&&
       carry->candidate.committed_receipts==owner->physical.committed_receipts);
 /* H214: released interaction interrupts carry with no history effect. */
 a->input.interact_held=false;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(cp.calls==2u&&cp.accepted==1u&&!carry->staged);
 a->input.interact_held=true;
 /* H215: object teleport beyond source bounded reach cannot claim carry. */
 const NfVec3 carried_from=owner->physical.object.center;
 owner->physical.object.center.x+=1.f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(cp.calls==3u&&cp.accepted==1u&&!carry->staged);
 owner->physical.object.center=carried_from;
 /* H216: a SOLID canonical source voxel in either swept path blocks
    carrying, even when the actor still holds the interaction input. */
 chunk->canonical[0]=NF18A5_SOLID;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(cp.calls==4u&&cp.accepted==1u&&!carry->staged);
 chunk->canonical[0]=cell;
 /* H217: restore and deterministically carry in PRIVATE only.
    There is no fabricated A4 contact receipt, B2/B3 consequence,
    A6 tick advancement, actor/object publication, or A5 sample. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(cp.calls==5u&&cp.accepted==2u&&carry->staged);
 CHECK(carry->candidate.world.revision==revision+1u&&
       carry->candidate.world.history.raw_samples==samples&&
       carry->candidate.committed_receipts==owner->physical.committed_receipts);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 free(carry);
 /* H218: source-exact private PUSH arms a transitional witness. */
 NfRetcM2Transition *tr=calloc(1,sizeof *tr);
 CHECK(tr);
 M2TransitionProbe tp={.state=tr,.mode=NF_RETC_M2_PUSH};
 owner->research_pair_stage=transition_probe;
 owner->research_pair_context=&tp;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(tp.calls==1u&&tp.passed==1u&&tr->armed&&tr->staged);
 CHECK(tr->mode==NF_RETC_M2_PUSH&&tr->push.staged&&
       tr->object_id==owner->physical.object.id);
 /* H219: an unchanged source may privately hand off from PUSH to CARRY,
    revalidating both swept volumes and object source. */
 tp.mode=NF_RETC_M2_CARRY;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(tp.calls==2u&&tp.passed==2u&&tr->carry.staged&&
       tr->mode==NF_RETC_M2_CARRY);
 CHECK(tr->carry.candidate.world.history.raw_samples==samples);
 /* H220: actual input release disarms the transition and invalidates
    previous PUSH/CARRY staging despite A6 rejecting publication. */
 a->input.interact_held=false;
 tp.mode=NF_RETC_M2_RELEASE;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(tp.calls==3u&&tp.passed==2u&&!tr->armed&&
       !tr->push.staged&&!tr->carry.staged);
 /* H221: reholding the input cannot resume a cancelled carry without
    a new source-exact PUSH stage. */
 a->input.interact_held=true;
 tp.mode=NF_RETC_M2_CARRY;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(tp.calls==4u&&tp.passed==2u&&!tr->armed&&!tr->staged);
 /* H222: source-restored rearm and carry transition are deterministic,
    with zero authoritative A6 tick, A5 history, or B3 receipts. */
 tp.mode=NF_RETC_M2_PUSH;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(tp.calls==5u&&tp.passed==3u&&tr->armed&&tr->push.staged);
 tp.mode=NF_RETC_M2_CARRY;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(tp.calls==6u&&tp.passed==4u&&tr->carry.staged);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u);
 CHECK(a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 free(tr);
 /* H223: two REAL world actors claim the same A6-owned crate. The bounded
    research tie-break chooses source actor id 30 before id 40. */
 CHECK(nf_world_spawn_actor_with_id(world,40u,NF_FACTION_RIVAL,
       (NfVec3){0.f,0.f,0.f})==40u);
 NfActor *rival=nf_world_find_actor(world,40u);
 CHECK(rival);
 rival->input.interact_held=true;
 rival->input.forward=1.f;
 rival->input.strafe=0.f;
 rival->input.yaw_radians=0.f;
 NfRetcM2Contention *cont=calloc(1,sizeof *cont);
 CHECK(cont);
 M2ContentionProbe mp={.out=cont};
 owner->research_pair_stage=contention_probe;
 owner->research_pair_context=&mp;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(mp.calls==1u&&mp.passed==1u&&cont->staged&&
       cont->claim_count==2u&&cont->winning_actor==30u);
 CHECK(cont->winner_effect.staged&&
       cont->winner_effect.pair.candidate.world.history.raw_samples==samples+1u);
 /* H224: permutation of the captured source roster does not change the
    deterministic contender winner or the source claim count. */
 NfMa03Roster permuted=cont->roster;
 NfMa03Participant temp=permuted.actors[0];
 permuted.actors[0]=permuted.actors[1];
 permuted.actors[1]=temp;
 uint32_t permuted_claims=0u;
 CHECK(nf_retc_m2_select_claim(&permuted,
       owner->physical.object.center,&permuted_claims)==30u);
 CHECK(permuted_claims==2u);
 /* H225: lower-id REAL contender wins; A6 owner 30 MUST refuse to stage
    any rival-owned physical consequence or A5 sample. */
 rival->id=20u;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(mp.calls==2u&&mp.passed==1u&&!cont->staged&&
       cont->claim_count==2u&&cont->winning_actor==20u);
 rival->id=40u;
 /* H226: owner releases and source rival remains held: competitor wins
    the proposal, but this A6 owner may not act on its behalf. */
 a->input.interact_held=false;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(mp.calls==3u&&mp.passed==1u&&!cont->staged&&
       cont->claim_count==1u&&cont->winning_actor==40u);
 /* H227: rival release and source owner re-entry deterministically
    restores private staging. Still no live world/body/A5/B3 effects. */
 a->input.interact_held=true;
 rival->input.interact_held=false;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(mp.calls==4u&&mp.passed==2u&&cont->staged&&
       cont->winning_actor==30u&&cont->claim_count==1u);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 free(cont);
 /* H228: real competing actors 30 and 40, same source, alternating
    diagnostic claimant selections. Only owner 30 may stage A4/A5. */
 rival->input.interact_held=true;
 NfRetcM2Fairness *fair=calloc(1,sizeof *fair);
 CHECK(fair);
 M2FairProbe fp={.state=fair};
 owner->research_pair_stage=fair_probe;
 owner->research_pair_context=&fp;
 for(unsigned i=0u;i<6u;++i){
  CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
  CHECK(fp.calls==i+1u&&fair->rounds==i+1u&&fair->claim_count==2u);
  CHECK(fair->winner==(i%2u==0u?30u:40u));
  CHECK(fair->staged==(i%2u==0u));
 }
 CHECK(fair->owner_stages==3u&&fair->rival_decisions==3u&&fp.staged==3u);
 /* H229: fair selector's result is independent of roster order. */
 NfMa03Roster reversed=permuted;
 reversed.actors[0].input.interact_held=true;
 reversed.actors[1].input.interact_held=true;
 uint32_t n=0u;
 CHECK(nf_retc_m2_fair_select(&reversed,
       owner->physical.object.center,30u,&n)==40u&&n==2u);
 CHECK(nf_retc_m2_fair_select(&reversed,
       owner->physical.object.center,40u,&n)==30u&&n==2u);
 /* H230: owner stops claiming; sole rival wins proposals but cannot
    borrow A6's physical owner or publish its own PUSH. */
 a->input.interact_held=false;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(fp.calls==7u&&fair->winner==40u&&fair->claim_count==1u&&
       !fair->staged&&fair->rival_decisions==4u);
 a->input.interact_held=true;
 /* H231: stale material epoch invalidates the diagnostic binding and
    requires a fresh source claim; no implicit source rebinding. */
 ++owner->physical.world.revision;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(fp.calls==8u&&!fair->staged&&!fair->bound);
 --owner->physical.world.revision;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(fp.calls==9u&&fair->staged&&fair->winner==30u&&fair->bound);
 /* H232: continued contention never starves a claimant in this
    REJECTED-tick probe, and never publishes any material outcome. */
 const uint32_t first_rounds=fair->rounds;
 for(unsigned i=0u;i<8u;++i){
  CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
  CHECK(fair->winner==(i%2u==0u?40u:30u));
 }
 CHECK(fair->rounds==first_rounds+8u);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 free(fair);
 /* H233: one real A6 owner actor pushes the real crate while the second
    world actor follows a separated, source-bound passive trajectory. */
 NfRetcM2ThreeEntity *three=calloc(1,sizeof *three);
 CHECK(three);
 M2ThreeProbe tp3={.out=three};
 rival->transform.position.x=-3.f;
 rival->transform.velocity.x=0.f;
 owner->research_pair_stage=three_probe;
 owner->research_pair_context=&tp3;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(tp3.calls==1u&&tp3.passed==1u&&three->staged);
 CHECK(three->roster.count==2u&&three->rival_id==40u&&
       three->rival_before.id==40u&&three->rival_after.id==40u);
 CHECK(three->owner_push.staged&&
       three->owner_push.pair.candidate.object.velocity.x>0.f&&
       three->owner_push.pair.candidate.world.history.raw_samples==samples+1u);
 /* H234: stage is deterministic for the same exact world source. */
 const uint32_t three_hash=nf18a5_history_hash(
       &three->owner_push.pair.candidate.world.history);
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(tp3.calls==2u&&tp3.passed==2u&&three->staged);
 CHECK(nf18a5_history_hash(&three->owner_push.pair.candidate.world.history)==three_hash);
 CHECK(three->rival_after.center.x==three->rival_before.center.x);
 /* H235: actual rival enters the owner actor's contact reach. We MUST
    refuse the unresolved shared three-body physics instead of hiding it. */
 rival->transform.position.x=-.2f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(tp3.calls==3u&&tp3.passed==2u&&!three->staged);
 rival->transform.position.x=-3.f;
 /* H236: an otherwise remote rival's swept trajectory crosses the
    interaction; conservative rejection is the only supported result. */
 rival->transform.velocity.x=24.f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(tp3.calls==4u&&tp3.passed==2u&&!three->staged);
 rival->transform.velocity.x=0.f;
 /* H237: restored three-entity source passes but NO rival body, actor,
    crate, A5 history, B3 receipt or live A6 tick is published. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(tp3.calls==5u&&tp3.passed==3u&&three->staged);
 CHECK(three->rival_after.velocity.x==0.f&&
       three->rival_after.center.x==-3.f);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x&&
       rival->transform.position.x==-3.f);
 free(three);
 /* H238: source-exact owner capsule -> A6 crate -> touching rival actor.
    Both A4 contacts are SOLVED together with momentum reference and
    produce TWO authentic private A5 contact-history samples. */
 NfRetcM2ThreeContact *chain=calloc(1,sizeof *chain);
 CHECK(chain);
 M2ChainProbe chain_probe_state={.out=chain};
 rival->transform.position.x=owner->physical.object.center.x+
      owner->physical.object.box_half.x+owner->physical.actor.radius;
 owner->research_pair_stage=chain_probe;
 owner->research_pair_context=&chain_probe_state;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 if(!chain->staged)fprintf(stderr,"H238 three-contact stage reject line %u\n",chain->reject_line);
 CHECK(chain_probe_state.calls==1u&&chain_probe_state.passed==1u&&chain->staged);
 CHECK(chain->passes<=32u&&chain->passes>=2u);
 CHECK(chain->bodies[0].id==30u&&chain->bodies[1].id==3050u&&
       chain->bodies[2].id==40u);
 CHECK(chain->contacts[0].impulses_applied&&
       chain->contacts[1].impulses_applied);
 CHECK(chain->material.history.raw_samples==samples+2u&&
       chain->samples[0].body_a==30u&&chain->samples[0].body_b==3050u&&
       chain->samples[1].body_a==3050u&&chain->samples[1].body_b==40u);
 for(unsigned k=0u;k<3u;++k)
  CHECK(fabsf(chain->bodies[k].velocity.x-chain->expected_velocity)<.004f);
 /* H239: exact same in-tick source reproduces both causal A4/A5 edges. */
 const uint32_t chain_history=nf18a5_history_hash(&chain->material.history);
 const float common_speed=chain->expected_velocity;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==2u&&chain_probe_state.passed==2u&&chain->staged);
 CHECK(nf18a5_history_hash(&chain->material.history)==chain_history&&
       chain->expected_velocity==common_speed);
 /* H240: rival not touching the dynamic crate is outside this narrow
    simultaneous chain domain; must refuse invented contact ancestry. */
 rival->transform.position.x+=.1f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==3u&&chain_probe_state.passed==2u&&!chain->staged);
 rival->transform.position.x-=.1f;
 /* H241: source crate collider diverges from the A6 physical crate.
    A4 may not stage using detached, caller-supplied geometry. */
 NfCollider *chain_mirror=&world->colliders[owner->object_collider_index];
 chain_mirror->min.x+=.1f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==4u&&chain_probe_state.passed==2u&&!chain->staged);
 chain_mirror->min.x-=.1f;
 /* H242: after restoration TWO private A5 contacts stage from the
    authentic A4 network, but no A6, A5, actor, crate or B3 publication. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==5u&&chain_probe_state.passed==3u&&chain->staged);
 CHECK(chain->material.history.raw_samples==samples+2u&&
       nf18a5_history_hash(&chain->material.history)==chain_history);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x&&
       fabsf(rival->transform.position.x-(before_crate.x+owner->physical.object.box_half.x+owner->physical.actor.radius))<.0001f);
 /* H243-H247: material transaction integrity on the true two-edge
    A4 contact graph. All calls use PRIVATE copied A5 world state. */
 const uint32_t chain_ids[3]={30u,3050u,40u};
 Nf18a5World private_history=owner->physical.world;
 const uint32_t original_history_digest=
     nf18a5_history_hash(&private_history.history);
 /* H243: genuine pair of distinct source contacts commits together
    under one A6 actor owner; preserve crate->rival material ancestry. */
 CHECK(nf18a5_world_commit_three(&private_history,revision,1u,30u,
                                 chain_ids,chain->samples));
 CHECK(private_history.revision==revision+1u&&
       private_history.tick==1u&&
       private_history.history.raw_samples==samples+2u);
 unsigned active_edges=0u;
 bool edge_owner_box=false,edge_box_rival=false;
 for(size_t si=0u;si<NF18A5_MAX_STREAMS;++si){
  const Nf18a5Stream *st=&private_history.history.streams[si];
  if(!st->active)continue;
  ++active_edges;
  if(st->body_a==30u&&st->body_b==3050u)edge_owner_box=true;
  if(st->body_a==3050u&&st->body_b==40u)edge_box_rival=true;
 }
 CHECK(active_edges==2u&&edge_owner_box&&edge_box_rival);
 /* H244: forge the second source edge as owner->rival rather than
    crate->rival. Must fail atomically and preserve the input world. */
 Nf18a5Sample forged_edges[2]={chain->samples[0],chain->samples[1]};
 forged_edges[1].body_a=30u;
 Nf18a5World rejected=owner->physical.world;
 CHECK(!nf18a5_world_commit_three(&rejected,revision,1u,30u,
                                  chain_ids,forged_edges));
 CHECK(rejected.revision==revision&&rejected.tick==0u&&
       rejected.history.raw_samples==samples&&
       nf18a5_history_hash(&rejected.history)==original_history_digest);
 /* H245: two same-origin contact IDs are not two distinct material
    interactions, even when the physical solver returned two receipts. */
 forged_edges[1]=chain->samples[1];
 forged_edges[1].contact_id=forged_edges[0].contact_id;
 CHECK(!nf18a5_world_commit_three(&rejected,revision,1u,30u,
                                  chain_ids,forged_edges));
 CHECK(rejected.revision==revision&&
       rejected.history.raw_samples==samples);
 /* H246: both material epoch and expected world revision must be exact. */
 forged_edges[1]=chain->samples[1];
 forged_edges[1].material_epoch+=1u;
 CHECK(!nf18a5_world_commit_three(&rejected,revision,1u,30u,
                                  chain_ids,forged_edges));
 CHECK(!nf18a5_world_commit_three(&rejected,revision+1u,1u,30u,
                                  chain_ids,chain->samples));
 CHECK(rejected.revision==revision&&
       rejected.history.raw_samples==samples);
 /* H247: force stream exhaustion and demonstrate no half-commit of
    first edge even when second stream cannot be allocated. */
 Nf18a5World saturated=owner->physical.world;
 for(size_t si=0u;si<NF18A5_MAX_STREAMS;++si){
  Nf18a5Stream *st=&saturated.history.streams[si];
  *st=(Nf18a5Stream){0};
  st->active=1u;
  st->contact_id=100000u+(uint32_t)si;
  st->body_a=30u;st->body_b=3050u;
 }
 const uint32_t saturated_hash=nf18a5_history_hash(&saturated.history);
 CHECK(!nf18a5_world_commit_three(&saturated,revision,1u,30u,
                                  chain_ids,chain->samples));
 CHECK(saturated.revision==revision&&saturated.tick==0u&&
       saturated.history.raw_samples==samples&&
       nf18a5_history_hash(&saturated.history)==saturated_hash);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u);
 /* H248-H252: independent mass-weighted momentum oracle for genuine
    A4 3-contact chain; preserve exact A5 ancestry and failed rollback. */
 const float source_crate_inv_mass=owner->physical.object.inverse_mass;
 const float source_actor_inv_mass=owner->physical.actor.inverse_mass;
 const float reference_speed=chain->expected_velocity;
 const uint32_t reference_history=nf18a5_history_hash(&chain->material.history);
 /* H248: heavier crate (48kg) must share source total momentum across
    real actor/crate/rival body masses and produce TWO history samples. */
 owner->physical.object.inverse_mass=1.f/48.f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==6u&&chain_probe_state.passed==4u&&chain->staged);
 const float mass48_expected=(80.f*4.f)/(80.f+48.f+80.f);
 CHECK(fabsf(chain->expected_velocity-mass48_expected)<.001f);
 for(unsigned k=0u;k<3u;++k)
  CHECK(fabsf(chain->bodies[k].velocity.x-mass48_expected)<.004f);
 CHECK(chain->material.history.raw_samples==samples+2u);
 /* H249: still heavier crate (96kg) must not retain the prior
    common velocity or reuse the previous contact history. */
 owner->physical.object.inverse_mass=1.f/96.f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==7u&&chain_probe_state.passed==5u&&chain->staged);
 const float mass96_expected=(80.f*4.f)/(80.f+96.f+80.f);
 CHECK(fabsf(chain->expected_velocity-mass96_expected)<.001f);
 CHECK(fabsf(chain->expected_velocity-mass48_expected)>.02f);
 CHECK(chain->material.history.raw_samples==samples+2u);
 /* H250: independently change owner actor mass, not the crate mass.
    Total source momentum and every solved body must re-evaluate. */
 owner->physical.object.inverse_mass=source_crate_inv_mass;
 owner->physical.actor.inverse_mass=1.f/120.f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==8u&&chain_probe_state.passed==6u&&chain->staged);
 const float actor120_expected=(120.f*4.f)/
     (120.f+1.f/source_crate_inv_mass+80.f);
 CHECK(fabsf(chain->expected_velocity-actor120_expected)<.001f);
 for(unsigned k=0u;k<3u;++k)
  CHECK(fabsf(chain->bodies[k].velocity.x-actor120_expected)<.004f);
 /* H251: invalid zero inverse-mass for the pushed dynamic crate
    is refused. A bad mass cannot produce a phantom A5 contact. */
 owner->physical.actor.inverse_mass=source_actor_inv_mass;
 owner->physical.object.inverse_mass=0.f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==9u&&chain_probe_state.passed==6u&&!chain->staged);
 /* H252: restoring original mass reproduces the identical
    two-contact digest; all changes stayed private and fail-closed. */
 owner->physical.object.inverse_mass=source_crate_inv_mass;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==10u&&chain_probe_state.passed==7u&&chain->staged);
 CHECK(chain->expected_velocity==reference_speed&&
       nf18a5_history_hash(&chain->material.history)==reference_history);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 /* H253-H257: source-exact A4 first-impact TOI/timing boundary.
    This is a collinear 1D scheduled-contact test, NOT proof of
    reversing a real collision graph or generalized 3D contacts. */
 const float baseline_contact_time=chain->first_impact_time;
 const uint32_t baseline_contact_history=
     nf18a5_history_hash(&chain->material.history);
 CHECK(baseline_contact_time>0.f&&baseline_contact_time<.125f);
 /* H253: a shorter VALID dt which still contains the impact stages the
    same A4 contact time and both genuine A5 history edges. */
 CHECK(nf_world_step_checked(world,.11f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==11u&&chain_probe_state.passed==8u&&chain->staged);
 CHECK(chain->first_impact_fraction>0.f&&chain->first_impact_fraction<1.f);
 CHECK(fabsf(chain->first_impact_time-baseline_contact_time)<.003f);
 CHECK(chain->material.history.raw_samples==samples+2u);
 /* H254: a longer valid dt preserves TOI as physical time, although
    its normalized fraction necessarily changes. */
 CHECK(nf_world_step_checked(world,.15f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==12u&&chain_probe_state.passed==9u&&chain->staged);
 CHECK(fabsf(chain->first_impact_time-baseline_contact_time)<.003f);
 CHECK(chain->material.history.raw_samples==samples+2u);
 /* H255: dt stops BEFORE the first actor-crate impact; never invent
    both contact receipts or material history for future collisions. */
 CHECK(nf_world_step_checked(world,.075f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==13u&&chain_probe_state.passed==9u&&!chain->staged);
 /* H256: dt outside the supported 250ms tick boundary is rejected
    before any private three-body physical or A5 stage. */
 CHECK(nf_world_step_checked(world,.3f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==14u&&chain_probe_state.passed==9u&&!chain->staged);
 /* H257: source baseline replay exactly reestablishes the original
    first impact time and private two-edge history hash; nothing live. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==15u&&chain_probe_state.passed==10u&&chain->staged);
 CHECK(chain->first_impact_time==baseline_contact_time&&
       nf18a5_history_hash(&chain->material.history)==baseline_contact_history);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 /* H258-H262: SAME real contact graph solved in the inverse A4
    Gauss-Seidel constraint order. This tests iterative-order
    convergence, NOT reversal of physical impact arrival times. */
 const float order_baseline=chain->expected_velocity;
 const uint32_t order_baseline_hash=nf18a5_history_hash(&chain->material.history);
 chain_probe_state.reverse=true;
 /* H258: reverse the two actual A4 contact constraints, preserve
    source identities and recover the independently calculated momentum. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==16u&&chain_probe_state.passed==11u&&chain->staged);
 CHECK(chain->bodies[0].id==30u&&chain->bodies[1].id==3050u&&
       chain->bodies[2].id==40u);
 CHECK(chain->material.history.raw_samples==samples+2u);
 for(unsigned i=0u;i<3u;++i)
  CHECK(fabsf(chain->bodies[i].velocity.x-order_baseline)<.004f);
 CHECK(chain->contacts[0].impulses_applied&&chain->contacts[1].impulses_applied);
 /* H259: identical reordered A4 constraints replay deterministically,
    including A5 history; no invented extra contacts. */
 const uint32_t reverse_history=nf18a5_history_hash(&chain->material.history);
 const unsigned reverse_passes=chain->passes;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==17u&&chain_probe_state.passed==12u&&chain->staged);
 CHECK(nf18a5_history_hash(&chain->material.history)==reverse_history&&
       chain->passes==reverse_passes);
 /* H260: unequal source crate mass must satisfy SAME independent
    conservation law under reversed A4 visitation. */
 const float order_crate_mass=owner->physical.object.inverse_mass;
 owner->physical.object.inverse_mass=1.f/48.f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==18u&&chain_probe_state.passed==13u&&chain->staged);
 const float order_mass_oracle=(80.f*4.f)/(80.f+48.f+80.f);
 CHECK(fabsf(chain->expected_velocity-order_mass_oracle)<.001f);
 for(unsigned i=0u;i<3u;++i)
  CHECK(fabsf(chain->bodies[i].velocity.x-order_mass_oracle)<.004f);
 CHECK(chain->material.history.raw_samples==samples+2u);
 owner->physical.object.inverse_mass=order_crate_mass;
 /* H261: separated rival is an unsupported distinct arrival event.
    Neither permutation may claim the resulting two-contact solution. */
 rival->transform.position.x+=.10f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==19u&&chain_probe_state.passed==13u&&!chain->staged);
 rival->transform.position.x-=.10f;
 /* H262: original incumbent remains available and its exact A5
    contact history is restored with zero authoritative side effects. */
 chain_probe_state.reverse=false;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(chain_probe_state.calls==20u&&chain_probe_state.passed==14u&&chain->staged);
 CHECK(chain->expected_velocity==order_baseline&&
       nf18a5_history_hash(&chain->material.history)==order_baseline_hash);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 free(chain);
 /* H263-H267: a true SECOND event later than owner->crate impact.
    The incumbent simultaneous-contact solver refuses the .04m gap;
    the new event-driven challenger may privately stage both edges. */
 NfRetcM2EventChain *scheduled=calloc(1,sizeof *scheduled);
 CHECK(scheduled);
 M2EventProbe ev={.out=scheduled};
 owner->research_pair_stage=event_probe;
 owner->research_pair_context=&ev;
 const float touching_x=owner->physical.object.center.x+
    owner->physical.object.box_half.x+owner->physical.actor.radius;
 rival->transform.position.x=touching_x+.04f;
 /* H263: two separate physical events, real A4 receipts and A5 edges;
    final velocities agree with an independent inelastic 1D oracle. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(ev.calls==1u&&ev.passed==1u&&scheduled->staged);
 CHECK(scheduled->first_time>0.f&&
       scheduled->second_time>scheduled->first_time&&
       scheduled->second_time<.125f);
 CHECK(scheduled->contacts[0].impulses_applied&&
       scheduled->contacts[1].impulses_applied);
 CHECK(scheduled->material.history.raw_samples==samples+2u&&
       scheduled->samples[0].body_a==30u&&
       scheduled->samples[0].body_b==3050u&&
       scheduled->samples[1].body_a==3050u&&
       scheduled->samples[1].body_b==40u);
 for(unsigned i=0u;i<3u;++i)
  CHECK(fabsf(scheduled->bodies[i].velocity.x-
              scheduled->expected_velocity)<.004f);
 /* H264: repeat exact authoritative source, same event times and
    A5 material ancestry; diagnostic replay does not mutate source. */
 const float first_time=scheduled->first_time;
 const float second_time=scheduled->second_time;
 const uint32_t event_hash=nf18a5_history_hash(&scheduled->material.history);
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(ev.calls==2u&&ev.passed==2u&&scheduled->staged);
 CHECK(scheduled->first_time==first_time&&
       scheduled->second_time==second_time&&
       nf18a5_history_hash(&scheduled->material.history)==event_hash);
 /* H265: physically distinct second event beyond tick horizon cannot
    fabricate a crate-to-rival receipt or history publication. */
 rival->transform.position.x=touching_x+.20f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(ev.calls==3u&&ev.passed==2u&&!scheduled->staged);
 rival->transform.position.x=touching_x+.04f;
 /* H266: owner source collider altered; both purported A4 events
    must refuse without trusting the detached crate geometry. */
 NfCollider *event_mirror=&world->colliders[owner->object_collider_index];
 event_mirror->min.x+=.1f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(ev.calls==4u&&ev.passed==2u&&!scheduled->staged);
 event_mirror->min.x-=.1f;
 /* H267: tick stops after event one but before event two. Deny
    two-contact candidate; baseline restoration recreates both edges. */
 const float midpoint=.5f*(first_time+second_time);
 CHECK(nf_world_step_checked(world,midpoint)==NF_WORLD_TICK_REJECTED);
 CHECK(ev.calls==5u&&ev.passed==2u&&!scheduled->staged);
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(ev.calls==6u&&ev.passed==3u&&scheduled->staged);
 CHECK(nf18a5_history_hash(&scheduled->material.history)==event_hash);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 /* H268-H272: the second contact must be computed from BOTH moving
    source bodies at the real first event, never from a stationary rival
    assumption. The incumbent stationary case remains the control. */
 const float moving_source_crate=owner->physical.object.velocity.x;
 const float moving_source_rival=rival->transform.velocity.x;
 const float moving_baseline_first=scheduled->first_time;
 const float moving_baseline_second=scheduled->second_time;
 const uint32_t moving_baseline_hash=
     nf18a5_history_hash(&scheduled->material.history);
 /* H268: moving rival translates before event one and changes the
    second event time. Conservation checks all THREE source masses. */
 rival->transform.velocity.x=.25f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(ev.calls==7u&&ev.passed==4u&&scheduled->staged);
 CHECK(scheduled->second_time>moving_baseline_second);
 CHECK(fabsf(scheduled->expected_velocity-
       (80.f*4.f+80.f*.25f)/(80.f+24.f+80.f))<.002f);
 for(unsigned j=0u;j<3u;++j)
  CHECK(fabsf(scheduled->bodies[j].velocity.x-
       scheduled->expected_velocity)<.004f);
 CHECK(scheduled->material.history.raw_samples==samples+2u);
 /* H269: moving source crate changes FIRST impact time and gap
    to rival at first impact; the second event is not stationary CCD. */
 rival->transform.velocity.x=0.f;
 owner->physical.object.velocity.x=.25f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(ev.calls==8u&&ev.passed==5u&&scheduled->staged);
 CHECK(scheduled->first_time>moving_baseline_first);
 CHECK(fabsf(scheduled->expected_velocity-
       (80.f*4.f+24.f*.25f)/(80.f+24.f+80.f))<.002f);
 CHECK(scheduled->first_time<scheduled->second_time&&
       scheduled->second_time<.125f);
 /* H270: both source bodies advance at .25m/s. Both impacts
    now occur inside the same .125s horizon (unlike the .5m/s
    rival, which correctly yielded an out-of-tick event). */
 rival->transform.velocity.x=.25f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(ev.calls==9u&&ev.passed==6u&&scheduled->staged);
 CHECK(scheduled->first_time<scheduled->second_time&&
       scheduled->second_time<.125f);
 CHECK(fabsf(scheduled->expected_velocity-
       (80.f*4.f+24.f*.25f+80.f*.25f)/(80.f+24.f+80.f))<.002f);
 CHECK(scheduled->samples[0].body_a==30u&&
       scheduled->samples[0].body_b==3050u&&
       scheduled->samples[1].body_a==3050u&&
       scheduled->samples[1].body_b==40u);
 /* H271: rival outruns the post-first-event crate; a second impact
    cannot be forged inside this tick, so the joint stage refuses. */
 owner->physical.object.velocity.x=0.f;
 rival->transform.velocity.x=3.5f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(ev.calls==10u&&ev.passed==6u&&!scheduled->staged);
 /* H272: restore source velocities. Both event times and exact
    A5 digest reproduce, no world tick, actor, crate or B3 publication. */
 owner->physical.object.velocity.x=moving_source_crate;
 rival->transform.velocity.x=moving_source_rival;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(ev.calls==11u&&ev.passed==7u&&scheduled->staged);
 CHECK(scheduled->first_time==moving_baseline_first&&
       scheduled->second_time==moving_baseline_second&&
       nf18a5_history_hash(&scheduled->material.history)==moving_baseline_hash);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 /* H273A (preflight only): real source crate-rival contact would
    occur BEFORE actor-crate arrival. The actor-first scheduler MUST
    reject, never misidentify event ordering or publish any A5 edge.
    Full reversed-order scheduling remains an unimplemented challenger. */
 const float reversal_gap=.02f;
 rival->transform.position.x=touching_x+reversal_gap;
 owner->physical.object.velocity.x=.75f;
 rival->transform.velocity.x=0.f;
 /* Independent constant-speed arrival estimate places crate-rival
    first; actor-to-crate source closing speed is smaller than 4m/s. */
 const float reversal_crate_rival_t=reversal_gap/.75f;
 CHECK(reversal_crate_rival_t<scheduled->first_time);
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(ev.calls==12u&&ev.passed==7u&&!scheduled->staged);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u);
 /* Replaying the same prohibited order cannot fabricate a receipt. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(ev.calls==13u&&ev.passed==7u&&!scheduled->staged);
 /* Restore baseline and demonstrate source-exact recovery of the
    original two-edge PRIVATE event candidate and history digest. */
 rival->transform.position.x=touching_x+.04f;
 owner->physical.object.velocity.x=moving_source_crate;
 rival->transform.velocity.x=moving_source_rival;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(ev.calls==14u&&ev.passed==8u&&scheduled->staged);
 CHECK(scheduled->first_time==moving_baseline_first&&
       scheduled->second_time==moving_baseline_second&&
       nf18a5_history_hash(&scheduled->material.history)==moving_baseline_hash);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u);
 /* M2.x final H273B: five independently checkable source-arrival
    classifications. Tied events are deliberately NOT falsely accepted
    by the actor-first solver; the full tie/reversal solver is open. */
 CHECK(nf_retc_m2_arrival_order(.02f,.75f,0.f,.08f,.125f)==2);
 CHECK(nf_retc_m2_arrival_order(.06f,.75f,0.f,.08f,.125f)==2);
 CHECK(nf_retc_m2_arrival_order(.09f,.75f,0.f,.08f,.125f)==1);
 CHECK(nf_retc_m2_arrival_order(.02f,.25f,.5f,.08f,.125f)==1);
 CHECK(nf_retc_m2_arrival_order(.02f,.75f,0.f,NAN,.125f)==0);
 /* H273C: explicit dual-clock source evidence for the future positive
    reversed-event solver, still fail-closed in the incumbent. */
 NfRetcM2ArrivalEvidence clock={0};
 /* C1: crate/rival impact precedes authentic actor/crate event time. */
 CHECK(nf_retc_m2_arrival_evidence(.02f,.75f,0.f,.08f,.125f,&clock));
 CHECK(clock.order==2&&clock.rival_closing&&
       fabsf(clock.rival_time-(.02f/.75f))<.00001f&&
       clock.actor_time==.08f);
 /* C2: equal arrival times are a conservative unsupported tie. */
 CHECK(nf_retc_m2_arrival_evidence(.06f,.75f,0.f,.08f,.125f,&clock));
 CHECK(clock.order==2&&fabsf(clock.rival_time-clock.actor_time)<.00001f);
 /* C3: actor-first ordering preserves exact independent event clock. */
 CHECK(nf_retc_m2_arrival_evidence(.09f,.75f,0.f,.08f,.125f,&clock));
 CHECK(clock.order==1&&fabsf(clock.rival_time-.12f)<.00001f);
 /* C4: no rival closing cannot invent an arrival in the tick. */
 CHECK(nf_retc_m2_arrival_evidence(.02f,.25f,.5f,.08f,.125f,&clock));
 CHECK(clock.order==3&&!clock.rival_closing&&isinf(clock.rival_time));
 /* C5: invalid A4 event time fails with no usable order. */
 CHECK(!nf_retc_m2_arrival_evidence(.02f,.75f,0.f,NAN,.125f,&clock));
 CHECK(clock.order==0&&!clock.rival_closing);
 /* H274A: five independent source-event arbitration categories; a tie
    is NOT a permit to solve or publish a simultaneous A4 impulse. */
 NfRetcM2EventArbitration arbitration={0};
 /* A1: exact source actor-first must select the incumbent order. */
 CHECK(nf_retc_m2_arbitrate_contact_events(
       .09f,.75f,0.f,.08f,.125f,&arbitration));
 CHECK(arbitration.order==NF_RETC_M2_ORDER_ACTOR_FIRST&&
       arbitration.can_stage_sequential&&arbitration.signed_arrival_delta>0.f);
 /* A2: genuinely rival-first order remains a separate A4 challenger. */
 CHECK(nf_retc_m2_arbitrate_contact_events(
       .02f,.75f,0.f,.08f,.125f,&arbitration));
 CHECK(arbitration.order==NF_RETC_M2_ORDER_RIVAL_FIRST&&
       arbitration.can_stage_sequential&&arbitration.signed_arrival_delta<0.f);
 /* A3: true simultaneous contact has NO deterministic preference. */
 CHECK(nf_retc_m2_arbitrate_contact_events(
       .06f,.75f,0.f,.08f,.125f,&arbitration));
 CHECK(arbitration.order==NF_RETC_M2_ORDER_SIMULTANEOUS&&
       !arbitration.can_stage_sequential);
 /* A4: a nearly simultaneous but source-nonidentical event is also
    rejected; exact margin is 10 microseconds in the event clock. */
 CHECK(nf_retc_m2_arbitrate_contact_events(
       .060004f,.75f,0.f,.08f,.125f,&arbitration));
 CHECK(arbitration.order==NF_RETC_M2_ORDER_SIMULTANEOUS&&
       !arbitration.can_stage_sequential&&
       fabsf(arbitration.signed_arrival_delta)<=arbitration.source.tie_margin);
 /* A5: nonclosing and nonfinite clocks never acquire authority. */
 CHECK(nf_retc_m2_arbitrate_contact_events(
       .02f,.25f,.5f,.08f,.125f,&arbitration));
 CHECK(arbitration.order==NF_RETC_M2_ORDER_NOT_CLOSING&&
       !arbitration.can_stage_sequential);
 CHECK(!nf_retc_m2_arbitrate_contact_events(
       .02f,.75f,0.f,NAN,.125f,&arbitration));
 CHECK(arbitration.order==NF_RETC_M2_ORDER_INVALID&&
       !arbitration.can_stage_sequential);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u);
 /* H273D: independent mass-weighted rival-first oracle; deliberately
    predictive and non-authoritative, never fabricates A4/A5 receipts. */
 NfRetcM2ReverseOracle rev={0};
 /* D1: actual source order is rival first; then actor reaches merged
    crate/rival pair, preserving full three-body 1D momentum. */
 CHECK(nf_retc_m2_reverse_oracle(.02f,4.f,.75f,0.f,
        80.f,24.f,80.f,.08f,.125f,&rev));
 const float rival_first_ref=.02f/.75f;
 const float crate_post_ref=24.f*.75f/(24.f+80.f);
 const float actor_next_ref=rival_first_ref+
     (4.f-.75f)*(.08f-rival_first_ref)/(4.f-crate_post_ref);
 CHECK(rev.predicted&&
       fabsf(rev.rival_time-rival_first_ref)<.00001f&&
       fabsf(rev.crate_after_rival-crate_post_ref)<.00001f&&
       fabsf(rev.actor_time-actor_next_ref)<.00001f&&
       fabsf(rev.final_common_speed-
             (80.f*4.f+24.f*.75f)/(80.f+24.f+80.f))<.00001f);
 /* D2: event-time tie cannot be silently elevated into order. */
 CHECK(!nf_retc_m2_reverse_oracle(.06f,4.f,.75f,0.f,
       80.f,24.f,80.f,.08f,.125f,&rev)&&!rev.predicted);
 /* D3: actor-first incumbent stays separate from reversed oracle. */
 CHECK(!nf_retc_m2_reverse_oracle(.09f,4.f,.75f,0.f,
       80.f,24.f,80.f,.08f,.125f,&rev)&&!rev.predicted);
 /* D4: nonclosing crate-rival pair cannot create a first event. */
 CHECK(!nf_retc_m2_reverse_oracle(.02f,4.f,.25f,.5f,
       80.f,24.f,80.f,.08f,.125f,&rev)&&!rev.predicted);
 /* D5: invalid body mass and stale/nonfinite event inputs fail. */
 CHECK(!nf_retc_m2_reverse_oracle(.02f,4.f,.75f,0.f,
       80.f,0.f,80.f,.08f,.125f,&rev)&&!rev.predicted);
 CHECK(!nf_retc_m2_reverse_oracle(.02f,4.f,.75f,0.f,
       80.f,24.f,80.f,NAN,.125f,&rev)&&!rev.predicted);

 /* Tested private staging remains bytewise source-derived and fails
    closed on rival-first geometry (H273A above); restoring source
    still returns the exact two-contact A5 history hash. */
 /* H273E: real SOURCE-backed A4 first collision, private owner tick.
    This is a genuine crate->rival impulse, not yet a second A4
    actor->crate event, two-edge A5 commit, or live publication. */
 NfRetcM2ReverseFirst *reverse_stage=calloc(1,sizeof *reverse_stage);
 CHECK(reverse_stage);
 M2ReverseProbe reverse_state={.out=reverse_stage};
 owner->research_pair_stage=reverse_probe;
 owner->research_pair_context=&reverse_state;
 rival->transform.position.x=touching_x+.02f;
 owner->physical.object.velocity.x=.75f;
 rival->transform.velocity.x=0.f;
 /* E1: genuine first A4 receipt and reference agreement. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(reverse_state.calls==1u&&reverse_state.passed==1u&&reverse_stage->staged);
 CHECK(reverse_stage->receipt.status==NF18A4_ACCEPTED&&
       reverse_stage->receipt.impulses_applied);
 CHECK(fabsf(reverse_stage->crate_after.velocity.x-
       reverse_stage->oracle.crate_after_rival)<.004f);
 CHECK(fabsf(reverse_stage->rival_after.velocity.x-
       reverse_stage->oracle.crate_after_rival)<.004f);
 CHECK(reverse_stage->oracle.rival_time<reverse_stage->oracle.actor_time);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u);
 /* E2: repeat same original A6 source gives deterministic first impulse. */
 const float first_rival_impulse=reverse_stage->receipt.normal_impulse.x;
 const float first_rival_time=reverse_stage->oracle.rival_time;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(reverse_state.calls==2u&&reverse_state.passed==2u&&reverse_stage->staged);
 CHECK(reverse_stage->receipt.normal_impulse.x==first_rival_impulse&&
       reverse_stage->oracle.rival_time==first_rival_time);
 /* E3: compute the genuine A4 actor-crate TOI for this moving
    source; arrange crate-rival impact at EXACTLY that time. A static
    hard-coded .06m gap would not be simultaneous for a moving box. */
 const Nf18a2ShapePolicy tie_shape={
    owner->physical.actor.radius,owner->physical.actor.height,
    .05f,.4f,.65f};
 const Nf18a4Motor tie_motor={owner->physical.actor.velocity,0.f,0.f};
 const Nf18a4PairResult tie_pair=nf18a4_pair_step(
    owner->physical.actor,owner->physical.object,tie_shape,tie_motor,
    NF18A4_RECIPROCAL_LINEAR,.125f,1u,revision,0.f,0.f);
 CHECK(tie_pair.status==NF18A4_ACCEPTED&&tie_pair.swept_hit);
 const float tie_gap=owner->physical.object.velocity.x*tie_pair.toi*.125f;
 rival->transform.position.x=touching_x+tie_gap;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(reverse_state.calls==3u&&reverse_state.passed==2u&&!reverse_stage->staged);
 /* E4: detached owner collider mirror must refuse A4 staging. */
 rival->transform.position.x=touching_x+.02f;
 NfCollider *reverse_mirror=&world->colliders[owner->object_collider_index];
 reverse_mirror->max.x+=.1f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(reverse_state.calls==4u&&reverse_state.passed==2u&&!reverse_stage->staged);
 reverse_mirror->max.x-=.1f;
 /* E5: source restoration replays the identical first contact;
    live physical and contractual consequences remain untouched. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(reverse_state.calls==5u&&reverse_state.passed==3u&&reverse_stage->staged);
 CHECK(reverse_stage->receipt.normal_impulse.x==first_rival_impulse&&
       reverse_stage->oracle.rival_time==first_rival_time);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 /* H273F: source-backed SECOND genuine A4 impulse after the
    rival-first contact; both receipts live only in copied state. */
 NfRetcM2ReverseSecond *second=calloc(1,sizeof *second);
 CHECK(second);
 M2ReverseSecondProbe second_state={.out=second};
 owner->research_pair_stage=reverse_second_probe;
 owner->research_pair_context=&second_state;
 /* F1: both actual A4 contact receipts, conservation along actor/crate
    at event two, and no invented rival re-impulse. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(second_state.calls==1u&&second_state.passed==1u&&second->staged);
 CHECK(second->first.receipt.status==NF18A4_ACCEPTED&&
       second->first.receipt.impulses_applied);
 CHECK(second->second_receipt.status==NF18A4_ACCEPTED&&
       second->second_receipt.impulses_applied);
 CHECK(fabsf(second->actor_after.velocity.x-
              second->expected_pair_velocity)<.004f&&
       fabsf(second->crate_after.velocity.x-
              second->expected_pair_velocity)<.004f);
 CHECK(second->first.oracle.rival_time<second->first.oracle.actor_time);
 /* F2: deterministic replay of both real A4 impulses. */
 const float second_impulse=second->second_receipt.normal_impulse.x;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(second_state.calls==2u&&second_state.passed==2u&&second->staged);
 CHECK(second->second_receipt.normal_impulse.x==second_impulse);
 /* F3: tie in source event order must refuse BOTH staged events. */
 rival->transform.position.x=touching_x+tie_gap;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(second_state.calls==3u&&second_state.passed==2u&&!second->staged);
 /* F4: world collider diverges from A6 crate; refuse. */
 rival->transform.position.x=touching_x+.02f;
 reverse_mirror->max.x+=.1f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(second_state.calls==4u&&second_state.passed==2u&&!second->staged);
 reverse_mirror->max.x-=.1f;
 /* F5: restored source replays private physics, no world/A5/B3 change. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(second_state.calls==5u&&second_state.passed==3u&&second->staged);
 CHECK(second->second_receipt.normal_impulse.x==second_impulse);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 /* H273G: one PRIVATE A5 material revision from the two actual
    reversed-order A4 impulses; no live tick or B3 publication. */
 NfRetcM2ReverseMaterial *dual=calloc(1,sizeof *dual);
 CHECK(dual);
 M2ReverseMaterialProbe g_material_probe={.out=dual};
 owner->research_pair_stage=reverse_material_probe;
 owner->research_pair_context=&g_material_probe;
 /* G1: source-exact positive two-edge private ancestry; physical event
    order is rival-first, graph storage is actor/crate then crate/rival. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(g_material_probe.calls==1u&&g_material_probe.passed==1u&&dual->staged);
 CHECK(dual->material.revision==revision+1u&&dual->material.tick==1u&&
       dual->material.history.raw_samples==samples+2u);
 CHECK(dual->samples[0].body_a==30u&&dual->samples[0].body_b==3050u&&
       dual->samples[1].body_a==3050u&&dual->samples[1].body_b==40u);
 CHECK(dual->first_contact_id==dual->samples[1].contact_id&&
       dual->second_contact_id==dual->samples[0].contact_id);
 const uint32_t reverse_a5_digest=nf18a5_history_hash(&dual->material.history);
 /* G2: repeated source proves deterministic physical + A5 results. */
 const float g_impulse=dual->physical.second_receipt.normal_impulse.x;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(g_material_probe.calls==2u&&g_material_probe.passed==2u&&dual->staged);
 CHECK(nf18a5_history_hash(&dual->material.history)==reverse_a5_digest&&
       dual->physical.second_receipt.normal_impulse.x==g_impulse);
 /* G3: source-exact tie refuses a partial private material transaction. */
 rival->transform.position.x=touching_x+tie_gap;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(g_material_probe.calls==3u&&g_material_probe.passed==2u&&!dual->staged);
 /* G4: stale source material epoch fails without committing an edge. */
 rival->transform.position.x=touching_x+.02f;
 const uint32_t g_epoch=owner->physical.world.grid.global_revision;
 owner->physical.world.grid.global_revision=0u;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(g_material_probe.calls==4u&&g_material_probe.passed==2u&&!dual->staged);
 owner->physical.world.grid.global_revision=g_epoch;
 /* G5: source restores original digest, preserves ALL live state. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(g_material_probe.calls==5u&&g_material_probe.passed==3u&&dual->staged);
 CHECK(nf18a5_history_hash(&dual->material.history)==reverse_a5_digest);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 /* H273H: full source->final static A5 sweep across ALL reversed
    contact participants. A blocked source cell must fail without
    exposing privately staged G history or any live consequences. */
 NfRetcM2ReverseClearance *clearance=calloc(1,sizeof *clearance);
 CHECK(clearance);
 M2ReverseClearanceProbe hp={.out=clearance};
 owner->research_pair_stage=reverse_clearance_probe;
 owner->research_pair_context=&hp;
 /* H1: three monotone source-final sweeps and two-edge A5 witness. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(hp.calls==1u&&hp.passed==1u&&clearance->staged);
 CHECK(clearance->candidate.material.history.raw_samples==samples+2u&&
       nf18a5_history_hash(&clearance->candidate.material.history)==
         reverse_a5_digest);
 CHECK(clearance->end_x[0]>owner->physical.actor.center.x&&
       clearance->end_x[1]>owner->physical.object.center.x&&
       clearance->end_x[2]>rival->transform.position.x);
 /* H2: source-identical re-entry preserves exact swept endpoints. */
 const float h_end[3]={
    clearance->end_x[0],clearance->end_x[1],clearance->end_x[2]};
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(hp.calls==2u&&hp.passed==2u&&clearance->staged);
 CHECK(clearance->end_x[0]==h_end[0]&&
       clearance->end_x[1]==h_end[1]&&
       clearance->end_x[2]==h_end[2]);
 /* H3: a source-backed static blocker invalidates a swept route. */
 Nf18a5Grid *h_original_grid=malloc(sizeof *h_original_grid);
 CHECK(h_original_grid);
 *h_original_grid=owner->physical.world.grid;
 uint8_t h_blocked[NF18A5_CANON_VOXELS];
 for(size_t h=0u;h<NF18A5_CANON_VOXELS;++h)h_blocked[h]=1u;
 CHECK(nf18a5_load_canonical(&owner->physical.world.grid,
     0,0,0,h_blocked,1u));
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(hp.calls==3u&&hp.passed==2u&&!clearance->staged);
 owner->physical.world.grid=*h_original_grid;
 /* H4: missing material revision is fail-closed even with free cells. */
 owner->physical.world.grid.global_revision=0u;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(hp.calls==4u&&hp.passed==2u&&!clearance->staged);
 owner->physical.world.grid=*h_original_grid;
 free(h_original_grid);
 /* H5: restored unblocked source replays exact A5 and swept witness,
    and all world/material/contractual live projections remain unchanged. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(hp.calls==5u&&hp.passed==3u&&clearance->staged);
 CHECK(clearance->end_x[0]==h_end[0]&&
       clearance->end_x[1]==h_end[1]&&
       clearance->end_x[2]==h_end[2]&&
       nf18a5_history_hash(&clearance->candidate.material.history)==
         reverse_a5_digest);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 /* H274B: exact source-timed simultaneous A4 dual-contact
    challenger. TRUE coupled private A4 impulses, no A5/B2/B3 commit. */
 NfRetcM2Simultaneous *sim=calloc(1,sizeof *sim);
 CHECK(sim);
 M2SimultaneousProbe sim_probe={.out=sim};
 owner->research_pair_stage=simultaneous_probe;
 owner->research_pair_context=&sim_probe;
 rival->transform.position.x=touching_x+tie_gap;
 /* B1: both true A4 impulses and independent whole-system momentum. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sim_probe.calls==1u&&sim_probe.passed==1u&&sim->staged);
 CHECK(sim->contacts[0].status==NF18A4_ACCEPTED&&
       sim->contacts[0].impulses_applied&&
       sim->contacts[1].status==NF18A4_ACCEPTED&&
       sim->contacts[1].impulses_applied);
 const float sim_expected=(80.f*4.f+24.f*.75f)/(80.f+24.f+80.f);
 CHECK(fabsf(sim->oracle_velocity-sim_expected)<.004f);
 for(unsigned k=0u;k<3u;++k)
   CHECK(fabsf(sim->bodies[k].velocity.x-sim_expected)<.004f);
 /* B2: reversing visit order retains both authentic receipts and
    independent conservation under source-identical simultaneous impact. */
 sim_probe.reverse=true;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sim_probe.calls==2u&&sim_probe.passed==2u&&sim->staged);
 for(unsigned k=0u;k<3u;++k)
   CHECK(fabsf(sim->bodies[k].velocity.x-sim_expected)<.004f);
 /* B3: near-tie outside the solver's stricter exact-contact tolerance
    stays a diagnostic tie but may not fabricate A4 simultaneous impulses. */
 sim_probe.reverse=false;
 rival->transform.position.x=touching_x+tie_gap+.000003f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sim_probe.calls==3u&&sim_probe.passed==2u&&!sim->staged);
 /* B4: source collider mismatch cannot stage either A4 edge. */
 rival->transform.position.x=touching_x+tie_gap;
 reverse_mirror->max.x+=.1f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sim_probe.calls==4u&&sim_probe.passed==2u&&!sim->staged);
 reverse_mirror->max.x-=.1f;
 /* B5: restored source replays two private authentic A4 contacts,
    no A6 accepted world tick or A5/B3 side effect. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sim_probe.calls==5u&&sim_probe.passed==3u&&sim->staged);
 CHECK(fabsf(sim->oracle_velocity-sim_expected)<.004f&&
       world->tick==0u&&
       owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 /* H274C: two simultaneous true A4 impulses must become ONE
    source-identity-bound private A5 material revision after three
    source-to-final static clearance checks. Never publish A6/B3. */
 NfRetcM2SimultaneousMaterial *sm=calloc(1,sizeof *sm);
 CHECK(sm);
 M2SimultaneousMaterialProbe sm_probe={.out=sm};
 owner->research_pair_stage=simultaneous_material_probe;
 owner->research_pair_context=&sm_probe;
 /* C1: both material edges atomically accepted in copied A5 history. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sm_probe.calls==1u&&sm_probe.passed==1u&&sm->staged);
 CHECK(sm->material.revision==revision+1u&&sm->material.tick==1u&&
       sm->material.history.raw_samples==samples+2u);
 CHECK(sm->samples[0].body_a==30u&&sm->samples[0].body_b==3050u&&
       sm->samples[1].body_a==3050u&&sm->samples[1].body_b==40u);
 const uint32_t sm_hash=nf18a5_history_hash(&sm->material.history);
 /* C2: opposite constraint order gives same physical source momentum,
    graph identities and source-to-final clearance. */
 sm_probe.reverse=true;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sm_probe.calls==2u&&sm_probe.passed==2u&&sm->staged);
 CHECK(sm->material.history.raw_samples==samples+2u&&
       sm->samples[0].body_a==30u&&sm->samples[1].body_b==40u);
 for(unsigned k=0u;k<3u;++k)
   CHECK(fabsf(sm->physical.bodies[k].velocity.x-sim_expected)<.004f);
 /* C3: a diagnostic near-tie, not an exact coupled event, cannot
    privately publish even one material edge. */
 sm_probe.reverse=false;
 rival->transform.position.x=touching_x+tie_gap+.000003f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sm_probe.calls==3u&&sm_probe.passed==2u&&!sm->staged);
 /* C4: real source static obstruction refuses all staged A5 edges. */
 rival->transform.position.x=touching_x+tie_gap;
 Nf18a5Grid *sm_grid=malloc(sizeof *sm_grid);
 CHECK(sm_grid);
 *sm_grid=owner->physical.world.grid;
 uint8_t sm_blocked[NF18A5_CANON_VOXELS];
 for(size_t k=0u;k<NF18A5_CANON_VOXELS;++k)sm_blocked[k]=1u;
 CHECK(nf18a5_load_canonical(&owner->physical.world.grid,
      0,0,0,sm_blocked,1u));
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sm_probe.calls==4u&&sm_probe.passed==2u&&!sm->staged);
 owner->physical.world.grid=*sm_grid;
 free(sm_grid);
 /* C5: restored exact source reproduces private A5 digest while the
    accepted world and B3 remain completely unchanged. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sm_probe.calls==5u&&sm_probe.passed==3u&&sm->staged);
 CHECK(nf18a5_history_hash(&sm->material.history)==sm_hash);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u&&
       a->transform.position.x==before_actor.x&&
       owner->physical.object.center.x==before_crate.x);
 /* H275A: real A4 source-bound off-axis CORNER contact, with
    X and Z momentum conservation; no generic 3D or A5 claim. */
 NfRetcM2OffaxisCorner *off=calloc(1,sizeof *off);
 CHECK(off);
 M2OffaxisProbe off_probe_state={.out=off};
 owner->research_pair_stage=offaxis_probe;
 owner->research_pair_context=&off_probe_state;
 const NfVec3 original_off_center=owner->physical.actor.center;
 const NfVec3 original_off_velocity=owner->physical.actor.velocity;
 const NfVec3 original_off_position=a->transform.position;
 const NfVec3 original_off_actor_velocity=a->transform.velocity;
 owner->physical.actor.center.x=owner->physical.object.center.x-
    owner->physical.object.box_half.x-.2f;
 owner->physical.actor.center.z=owner->physical.object.center.z+
    owner->physical.object.box_half.z+.2f;
 owner->physical.actor.velocity.x=4.f;
 owner->physical.actor.velocity.z=-2.f;
 a->transform.position.x=owner->physical.actor.center.x;
 a->transform.position.z=owner->physical.actor.center.z;
 a->transform.velocity.x=4.f;
 a->transform.velocity.z=-2.f;
 /* H275A-1: genuine source A4 corner normal has X and Z components,
    crate receives a real off-axis impulse and momentum is conserved. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(off_probe_state.calls==1u&&off_probe_state.passed==1u&&off->staged);
 CHECK(off->receipt.status==NF18A4_ACCEPTED&&
       off->receipt.impulses_applied&&off->normal.x<0.f&&off->normal.z>0.f);
 const float off_ma=1.f/owner->physical.actor.inverse_mass;
 const float off_mb=1.f/owner->physical.object.inverse_mass;
 CHECK(fabsf(off_ma*off->actor_after.velocity.x+
       off_mb*off->crate_after.velocity.x-off->source_momentum_x)<.004f);
 CHECK(fabsf(off_ma*off->actor_after.velocity.z+
       off_mb*off->crate_after.velocity.z-off->source_momentum_z)<.004f);
 /* H275A-2: exact source repetition reproduces both velocity axes. */
 const float off_result_x=off->crate_after.velocity.x;
 const float off_result_z=off->crate_after.velocity.z;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(off_probe_state.calls==2u&&off_probe_state.passed==2u&&off->staged);
 CHECK(off->crate_after.velocity.x==off_result_x&&
       off->crate_after.velocity.z==off_result_z);
 /* H275A-3: separating actor motion cannot manufacture a contact. */
 owner->physical.actor.velocity.x=-4.f;
 owner->physical.actor.velocity.z=4.f;
 a->transform.velocity.x=-4.f;
 a->transform.velocity.z=4.f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(off_probe_state.calls==3u&&off_probe_state.passed==2u&&!off->staged);
 owner->physical.actor.velocity.x=4.f;
 owner->physical.actor.velocity.z=-2.f;
 a->transform.velocity.x=4.f;
 a->transform.velocity.z=-2.f;
 /* H275A-4: world collider mirror mismatch fails closed. */
 reverse_mirror->max.z+=.1f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(off_probe_state.calls==4u&&off_probe_state.passed==2u&&!off->staged);
 reverse_mirror->max.z-=.1f;
 /* H275A-5: restored source reproduces genuine off-axis A4; no live
    actor, crate, A5, A6, B2 or B3 publication is admitted. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(off_probe_state.calls==5u&&off_probe_state.passed==3u&&off->staged);
 CHECK(off->crate_after.velocity.x==off_result_x&&
       off->crate_after.velocity.z==off_result_z);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u);
 owner->physical.actor.center=original_off_center;
 owner->physical.actor.velocity=original_off_velocity;
 a->transform.position=original_off_position;
 a->transform.velocity=original_off_actor_velocity;
 /* H275B: source-owned moving off-axis XZ corner CCD. The
    starting actor is separated from the corner and must hit it at a
    strictly positive swept time, rather than staging an overlap. */
 NfRetcM2SweptCorner *swept=calloc(1,sizeof *swept);
 CHECK(swept);
 M2SweptCornerProbe sweep_probe={.out=swept};
 owner->research_pair_stage=swept_corner_probe;
 owner->research_pair_context=&sweep_probe;
 owner->physical.actor.center.x=owner->physical.object.center.x-
    owner->physical.object.box_half.x-.45f;
 owner->physical.actor.center.z=owner->physical.object.center.z+
    owner->physical.object.box_half.z+.45f;
 owner->physical.actor.velocity.x=4.f;
 owner->physical.actor.velocity.z=-4.f;
 a->transform.position.x=owner->physical.actor.center.x;
 a->transform.position.z=owner->physical.actor.center.z;
 a->transform.velocity.x=4.f;
 a->transform.velocity.z=-4.f;
 /* B1: real positive TOI, genuine off-axis A4, and independent 2-axis
    momentum conservation on the resulting pair. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sweep_probe.calls==1u&&sweep_probe.passed==1u&&swept->staged);
 CHECK(swept->toi>0.f&&swept->toi<1.f&&
       swept->impact_time>0.f&&swept->impact_time<.125f&&
       swept->normal.x<0.f&&swept->normal.z>0.f&&
       swept->receipt.status==NF18A4_ACCEPTED&&
       swept->receipt.impulses_applied);
 CHECK(fabsf(off_ma*swept->actor_after.velocity.x+
       off_mb*swept->crate_after.velocity.x-swept->source_momentum_x)<.004f);
 CHECK(fabsf(off_ma*swept->actor_after.velocity.z+
       off_mb*swept->crate_after.velocity.z-swept->source_momentum_z)<.004f);
 /* B2: exact source replay preserves TOI and both post-contact axes. */
 const float sweep_t=swept->impact_time;
 const float sweep_x=swept->crate_after.velocity.x;
 const float sweep_z=swept->crate_after.velocity.z;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sweep_probe.calls==2u&&sweep_probe.passed==2u&&swept->staged);
 CHECK(swept->impact_time==sweep_t&&
       swept->crate_after.velocity.x==sweep_x&&
       swept->crate_after.velocity.z==sweep_z);
 /* B3: separating velocities never create a positive TOI. */
 owner->physical.actor.velocity.x=-4.f;
 owner->physical.actor.velocity.z=4.f;
 a->transform.velocity.x=-4.f;a->transform.velocity.z=4.f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sweep_probe.calls==3u&&sweep_probe.passed==2u&&!swept->staged);
 /* B4: a grazing near-miss (Z remains outside the corner radius) is
    rejected rather than manufacturing contact. */
 owner->physical.actor.velocity.x=4.f;
 owner->physical.actor.velocity.z=0.f;
 a->transform.velocity.x=4.f;a->transform.velocity.z=0.f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sweep_probe.calls==4u&&sweep_probe.passed==2u&&!swept->staged);
 /* B5: source collider detachment refuses, then restoration reproduces
    the same exact TOI and preserves all live authority and history. */
 owner->physical.actor.velocity.z=-4.f;a->transform.velocity.z=-4.f;
 reverse_mirror->max.x+=.1f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sweep_probe.calls==5u&&sweep_probe.passed==2u&&!swept->staged);
 reverse_mirror->max.x-=.1f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(sweep_probe.calls==6u&&sweep_probe.passed==3u&&swept->staged);
 CHECK(swept->impact_time==sweep_t&&
       swept->crate_after.velocity.x==sweep_x&&
       swept->crate_after.velocity.z==sweep_z);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u);
 owner->physical.actor.center=original_off_center;
 owner->physical.actor.velocity=original_off_velocity;
 a->transform.position=original_off_position;
 a->transform.velocity=original_off_actor_velocity;
 /* H275C: distinct moving crate -X FACE TOI with tangential Z
    motion. The contact is interior to the face, not a corner. */
 NfRetcM2SweptFace *face=calloc(1,sizeof *face);
 CHECK(face);
 M2SweptFaceProbe face_probe={.out=face};
 owner->research_pair_stage=swept_face_probe;
 owner->research_pair_context=&face_probe;
 owner->physical.actor.center.x=owner->physical.object.center.x-
    owner->physical.object.box_half.x-.45f;
 owner->physical.actor.center.z=owner->physical.object.center.z+.05f;
 owner->physical.actor.velocity.x=4.f;
 owner->physical.actor.velocity.z=.5f;
 a->transform.position.x=owner->physical.actor.center.x;
 a->transform.position.z=owner->physical.actor.center.z;
 a->transform.velocity.x=4.f;
 a->transform.velocity.z=.5f;
 /* C1: real first face impact, X normal and unchanged tangential Z
    velocity after authentic A4 impulse with momentum conservation. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(face_probe.calls==1u&&face_probe.passed==1u&&face->staged);
 CHECK(face->impact_time>0.f&&face->impact_time<.125f&&
       face->toi>0.f&&face->toi<1.f&&
       face->normal.x==-1.f&&face->normal.z==0.f&&
       face->receipt.status==NF18A4_ACCEPTED&&
       face->receipt.impulses_applied);
 CHECK(fabsf(off_ma*face->actor_after.velocity.x+
       off_mb*face->crate_after.velocity.x-face->source_momentum_x)<.004f);
 CHECK(fabsf(off_ma*face->actor_after.velocity.z+
       off_mb*face->crate_after.velocity.z-face->source_momentum_z)<.004f);
 CHECK(face->actor_after.velocity.z==.5f);
 const float face_t=face->impact_time;
 const float face_x=face->crate_after.velocity.x;
 /* C2: replay source exact TOI and postimpact normal impulse. */
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(face_probe.calls==2u&&face_probe.passed==2u&&face->staged);
 CHECK(face->impact_time==face_t&&face->crate_after.velocity.x==face_x);
 /* C3: actor moving away from the -X face cannot stage contact. */
 owner->physical.actor.velocity.x=-4.f;
 a->transform.velocity.x=-4.f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(face_probe.calls==3u&&face_probe.passed==2u&&!face->staged);
 owner->physical.actor.velocity.x=4.f;a->transform.velocity.x=4.f;
 /* C4: source near the face/corner boundary is excluded explicitly. */
 owner->physical.actor.center.z=owner->physical.object.center.z+
    owner->physical.object.box_half.z-.005f;
 owner->physical.actor.velocity.z=0.f;
 a->transform.position.z=owner->physical.actor.center.z;
 a->transform.velocity.z=0.f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(face_probe.calls==4u&&face_probe.passed==2u&&!face->staged);
 owner->physical.actor.center.z=owner->physical.object.center.z+.05f;
 owner->physical.actor.velocity.z=.5f;
 a->transform.position.z=owner->physical.actor.center.z;
 a->transform.velocity.z=.5f;
 /* C5: mirror mismatch denied; restored source replays positive
    face TOI without any A5/B2/B3/live A6 consequences. */
 reverse_mirror->max.z+=.1f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(face_probe.calls==5u&&face_probe.passed==2u&&!face->staged);
 reverse_mirror->max.z-=.1f;
 CHECK(nf_world_step_checked(world,.125f)==NF_WORLD_TICK_REJECTED);
 CHECK(face_probe.calls==6u&&face_probe.passed==3u&&face->staged);
 CHECK(face->impact_time==face_t&&face->crate_after.velocity.x==face_x);
 CHECK(world->tick==0u&&owner->physical.world.revision==revision&&
       owner->physical.world.history.raw_samples==samples&&
       owner->physical.committed_receipts==0u);
 owner->physical.actor.center=original_off_center;
 owner->physical.actor.velocity=original_off_velocity;
 a->transform.position=original_off_position;
 a->transform.velocity=original_off_actor_velocity;
 free(face);
 free(swept);
 free(off);
 free(sm);
 free(sim);
 free(clearance);
 free(dual);
 free(second);
 owner->physical.object.velocity.x=moving_source_crate;
 rival->transform.velocity.x=moving_source_rival;
 rival->transform.position.x=touching_x+.04f;
 free(reverse_stage);
 free(scheduled);
 owner->require_pair_research=false;
 owner->research_pair_stage=NULL;
 owner->research_pair_context=NULL;
 CHECK(nf18a6_unbind_world_tick(world,owner));
 free(e);free(owner);free(world);
 /* H203-H207: independent real owner tick, grounded actor on the A6
    source static box; actual island support impulse and A5 force history. */
 NfWorld *sw=calloc(1,sizeof *sw);
 Nf18a6Runtime *sr=calloc(1,sizeof *sr);
 NfRetcM2SupportEvidence *se=calloc(1,sizeof *se);
 CHECK(sw&&sr&&se);
 nf_world_init(sw,101u);
 CHECK(nf_world_spawn_actor_with_id(sw,50u,NF_FACTION_PLAYER,
        (NfVec3){0.f,0.f,0.f})==50u);
 NfActor *supported=nf_world_find_actor(sw,50u);
 CHECK(supported);
 Nf18a4Body box={0};
 box.id=5050u;box.center=(NfVec3){0.f,-.5f,0.f};
 box.box_half=(NfVec3){.7f,.5f,.7f};
 CHECK(nf18a6_init(sr,sw,50u,box,4u,NF18A6_WORLD_BRIDGE));
 CHECK(nf18a6_bind_world_tick(sw,sr,NULL,NULL,NULL));
 supported->movement.grounded=true;
 supported->movement.ground_collider=(int)sr->object_collider_index;
 sr->require_pair_research=true;
 M2SupportProbe sp={.out=se};
 sr->research_pair_stage=support_probe;
 sr->research_pair_context=&sp;
 const NfVec3 supported_start=supported->transform.position;
 const NfVec3 support_object_start=sr->physical.object.center;
 const uint32_t support_revision=sr->physical.world.revision;
 const uint32_t support_samples=sr->physical.world.history.raw_samples;
 /* H203: real island support compensates gravity and records FORCE, not
    an alleged impact; both bodies in a private A5 candidate. */
 CHECK(nf_world_step_checked(sw,1.f/60.f)==NF_WORLD_TICK_REJECTED);
 CHECK(sp.calls==1u&&sp.success==1u&&se->staged);
 CHECK(se->result.dynamic_contact&&se->result.applied.impulses_applied);
 CHECK(se->result.external_impulse.y<0.f);
 CHECK(se->candidate.world.history.raw_samples==support_samples+1u);
 CHECK(se->candidate.world.history.streams[0].window.impulse_integral==0.0);
 /* H204: deterministic repeated owner-tick staging. */
 const uint32_t shash=nf18a5_history_hash(&se->candidate.world.history);
 const float sforce=se->result.applied.normal_impulse.y;
 CHECK(nf_world_step_checked(sw,1.f/60.f)==NF_WORLD_TICK_REJECTED);
 CHECK(sp.calls==2u&&sp.success==2u&&se->staged);
 CHECK(nf18a5_history_hash(&se->candidate.world.history)==shash);
 CHECK(se->result.applied.normal_impulse.y==sforce);
 /* H205: source actor losing actual grounding refuses the support. */
 supported->movement.grounded=false;
 CHECK(nf_world_step_checked(sw,1.f/60.f)==NF_WORLD_TICK_REJECTED);
 CHECK(sp.calls==3u&&sp.success==2u&&!se->staged);
 supported->movement.grounded=true;
 /* H206: actual A6 box is no longer touching the actor feet. */
 sr->physical.object.center.y-=.2f;
 CHECK(nf_world_step_checked(sw,1.f/60.f)==NF_WORLD_TICK_REJECTED);
 CHECK(sp.calls==4u&&sp.success==2u&&!se->staged);
 sr->physical.object.center=support_object_start;
 /* H207: restored support stages but does not advance world, alter the
    live actor or crate, or write any live material history. */
 CHECK(nf_world_step_checked(sw,1.f/60.f)==NF_WORLD_TICK_REJECTED);
 CHECK(sp.calls==5u&&sp.success==3u&&se->staged);
 CHECK(sw->tick==0u&&sr->physical.world.revision==support_revision&&
       sr->physical.world.history.raw_samples==support_samples);
 CHECK(supported->transform.position.x==supported_start.x&&
       supported->transform.position.y==supported_start.y&&
       supported->transform.position.z==supported_start.z);
 CHECK(sr->physical.object.center.y==support_object_start.y);
 sr->require_pair_research=false;
 sr->research_pair_stage=NULL;
 sr->research_pair_context=NULL;
 CHECK(nf18a6_unbind_world_tick(sw,sr));
 free(se);free(sr);free(sw);
 puts("M2 H198-H207 actor-object and support private owner-tick acceptance PASS");
 return 0;
}
