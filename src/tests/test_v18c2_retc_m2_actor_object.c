#include "nf_retc_m2_pair_tick.h"
#include "nf_retc_m2_support_tick.h"
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
