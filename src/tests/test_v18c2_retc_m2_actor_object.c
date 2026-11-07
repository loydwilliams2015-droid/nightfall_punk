#include "nf_retc_m2_pair_tick.h"
#include "nf_retc_m2_support_tick.h"
#include "nf_retc_m2_push_tick.h"
#include "nf_retc_m2_carry_tick.h"
#include "nf_retc_m2_transition_tick.h"
#include "nf_retc_m2_contention_tick.h"
#include "nf_retc_m2_fairness_tick.h"
#include "nf_retc_m2_three_entity_tick.h"
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
