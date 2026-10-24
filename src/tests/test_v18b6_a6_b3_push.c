#define _POSIX_C_SOURCE 200809L
#include "nf_b6_push18b6.h"
#include "nf_v18c2_h9_auth.h"
#include "nf_c2_h21_reconcile.h"
#include "nf_c2_h22_envelope.h"
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
static unsigned long long checks;
#define CK(x) do{if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return false;}++checks;}while(0)
static NfMoveInput rightward(void){NfMoveInput a={0};a.strafe=1;return a;}
static Nf18a4Body crate(float x){
 Nf18a4Body b={0};b.id=2010u;b.center=(NfVec3){x,.9f,1};
 b.inverse_mass=1.f/24.f;b.inverse_inertia=(NfVec3){.3f,.3f,.3f};
 b.box_half=(NfVec3){.2f,.5f,.35f};return b;
}
static bool setup(NfWorld *w,Nf18a6Runtime *r,float crate_x){
 nf_world_init(w,42u);
 if(nf_world_add_collider(w,NF_COLLIDER_SOLID,
       (NfVec3){0,-.5f,0},(NfVec3){4,0,4})<0)return false;
 if(nf_world_spawn_actor_with_id(w,10u,NF_FACTION_PLAYER,(NfVec3){1,0,1})!=10)return false;
 NfActor *a=nf_world_find_actor(w,10);
 a->movement.grounded=true;a->movement.mode=NF_MOVE_GROUND;
 if(!nf18a6_init(r,w,10,crate(crate_x),3,NF18A6_WORLD_BRIDGE))return false;
 uint8_t zero[NF18A5_CANON_VOXELS]={0};
 if(!nf18a5_load_canonical(&r->physical.world.grid,0,0,0,zero,1))return false;
 nf_world_set_input(w,10,rightward());return true;
}
static bool test_contract_and_replay(void){
 NfWorld *w=malloc(sizeof *w);Nf18a6Runtime *r=malloc(sizeof *r);
 Nf18b6Push *ctx=calloc(1,sizeof *ctx);
 CK(w&&r&&ctx);
 CK(setup(w,r,2.2f));
 CK(nf18b6_push_attach(ctx,r,w,3));
 CK(!nf18b6_push_attach(ctx,r,w,3));
 CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 /* Source-exact A6 prepublication negative controls: idle or spoofed owner
    cannot publish a B3 receipt, even if the candidate is plausible. */
 CK(r->owner_hooks.prepublish != NULL);
 Nf18a5Integrated candidate=r->physical;
 candidate.world.tick=(uint32_t)w->tick+1u;
 CK(!r->owner_hooks.prepublish(w,&candidate,ctx));
 NfWorld *spoof=malloc(sizeof *spoof);
 CK(spoof);
 *spoof=*w;
 spoof->tick_in_progress=true;
 spoof->tick_owner_context=NULL;
 CK(!r->owner_hooks.prepublish(spoof,&candidate,ctx));
 free(spoof);
 CK(ctx->receipt_count==0&&r->physical.committed_receipts==0);
 CK(nf18b6_push_arm(ctx,w,1,true,true,true)==NF18B3_PENDING);
 CK(!ctx->armed&&ctx->receipt_count==0);
 CK(nf18b6_push_arm(ctx,w,1,true,false,false)==NF18B3_DENIED);
 CK(nf18b6_push_arm(ctx,w,1,false,true,false)==NF18B3_DENIED);
 CK(nf18b6_push_arm(ctx,w,1,true,true,false)==NF18B3_OK);
 CK(ctx->armed);
 const uint32_t tick_before=(uint32_t)w->tick;
 ctx->contract_epoch=4; /* contractual revision changed before tick */
 CK(nf_world_step_checked(w,1.f/60.f)!=NF_WORLD_TICK_COMMITTED);
 CK(w->tick==tick_before&&ctx->receipt_count==0);
 CK(!ctx->armed&&r->physical.world.tick==tick_before);
 ctx->contract_epoch=3;
 unsigned physical_contacts=0,applied=0,pending=0;
 uint32_t first_receipt_action=0;
 for(unsigned j=0;j<55;++j){
  const uint32_t action=100+j;
  nf_world_set_input(w,10,rightward());
  CK(nf18b6_push_arm(ctx,w,action,true,true,false)==NF18B3_OK);
  const uint32_t prior_history=r->physical.world.history.raw_samples;
  const uint32_t prior_receipts=r->physical.committed_receipts;
  const uint64_t before_tick=w->tick;
  const uint32_t before_b3=ctx->receipt_count;
  const NfWorldTickStatus status=nf_world_step_checked(w,1.f/60.f);
  if(status!=NF_WORLD_TICK_COMMITTED){
   /* Real A6/A5 may legitimately become PENDING at contact frontier.
      Never pressure the engine to commit, fabricate an impulse or claim B3
      success. Both material history and authoritative tick must be unchanged. */
   CK(status==NF_WORLD_TICK_PENDING||status==NF_WORLD_TICK_REJECTED);
   CK(w->tick==before_tick&&r->physical.world.tick==before_tick);
   CK(r->physical.world.history.raw_samples==prior_history);
   CK(r->physical.committed_receipts==prior_receipts);
   CK(ctx->receipt_count==before_b3&&!ctx->event.applied);
   CK(ctx->last_result==NF18B3_PENDING&&!ctx->armed);
   pending++;
   break;
  }
  CK(w->tick==before_tick+1u&&ctx->armed==false);
  if(r->physical.committed_receipts>prior_receipts){
   physical_contacts++;
   CK(r->physical.world.history.raw_samples>prior_history);
   CK(ctx->last_result==NF18B3_OK&&ctx->event.applied &&
      ctx->event.phase==NF18B3_APPLIED);
   CK(ctx->receipts[ctx->receipt_count-1].tick==w->tick);
   if(!first_receipt_action)first_receipt_action=action;
   applied++;
  }else{
   CK(ctx->last_result==NF18B3_PENDING&&!ctx->event.applied);
   pending++;
  }
 }
 CK(physical_contacts>0&&applied==physical_contacts&&pending>0);
 CK(ctx->receipt_count==applied);
 CK(nf18b6_push_arm(ctx,w,first_receipt_action,true,true,false)==NF18B3_DUPLICATE);
 CK(r->physical.object.velocity.x>0.f);
 CK(nf18a6_unbind_world_tick(w,r));
 CK(nf18b6_push_detach(ctx,r,w));
 free(ctx);free(r);free(w);
 printf("physical_contacts=%u b3_applied=%u no_contact_pending=%u\n",physical_contacts,applied,pending);
 return true;
}
static bool test_missing_material_and_remote_actor(void){
 NfWorld *w=malloc(sizeof *w);Nf18a6Runtime *r=malloc(sizeof *r);
 Nf18b6Push *ctx=calloc(1,sizeof *ctx);
 CK(w&&r&&ctx);
 CK(setup(w,r,2.2f));
 CK(nf18b6_push_attach(ctx,r,w,1));
 CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 CK(nf18b6_push_arm(ctx,w,9,true,true,false)==NF18B3_OK);
 /* A second distant live actor causes existing A6 owner to reject the whole
    tick rather than starving the other actor. B3 cannot bypass that gate. */
 CK(nf_world_spawn_actor_with_id(w,77,NF_FACTION_RIVAL,(NfVec3){99,0,99})==77);
 const uint32_t rev=r->physical.world.revision;
 CK(nf_world_step_checked(w,1.f/60.f)!=NF_WORLD_TICK_COMMITTED);
 CK(w->tick==0&&ctx->receipt_count==0&&!ctx->event.applied);
 CK(r->physical.world.revision==rev);
 CK(nf_world_despawn_actor(w,77));
 CK(nf_world_step_checked(w,1.f/60.f)==NF_WORLD_TICK_COMMITTED);
 CK(ctx->receipt_count==0&&ctx->last_result==NF18B3_PENDING);
 CK(nf18a6_unbind_world_tick(w,r));CK(nf18b6_push_detach(ctx,r,w));
 free(ctx);free(r);free(w);return true;
}
static bool test_no_behavior_regression(unsigned seeds){
 NfWorld *a=malloc(sizeof *a),*b=malloc(sizeof *b);
 Nf18a6Runtime *ra=malloc(sizeof *ra),*rb=malloc(sizeof *rb);
 Nf18b6Push *ctx=calloc(1,sizeof *ctx);CK(a&&b&&ra&&rb&&ctx);
 clock_t incumbent_cost=0,candidate_cost=0;
 for(unsigned k=0;k<seeds;++k){
  CK(setup(a,ra,2.0f+0.02f*(k%9u)));
  CK(setup(b,rb,2.0f+0.02f*(k%9u)));
  CK(nf18a6_bind_world_tick(a,ra,NULL,NULL,NULL));
  CK(nf18b6_push_attach(ctx,rb,b,1));
  CK(nf18a6_bind_world_tick(b,rb,NULL,NULL,NULL));
  for(unsigned j=0;j<50;++j){
   NfMoveInput in=rightward();
   /* Same deterministic drive under incumbent owner vs challenger hooks.
    * No B3 request means the hooks are strictly transparent.
    */
   if(j%17u==0){in.strafe=0;}
   nf_world_set_input(a,10,in);nf_world_set_input(b,10,in);
   clock_t t=clock();
   const NfWorldTickStatus sa=nf_world_step_checked(a,1.f/60.f);
   incumbent_cost+=clock()-t;
   t=clock();
   const NfWorldTickStatus sb=nf_world_step_checked(b,1.f/60.f);
   candidate_cost+=clock()-t;
   CK(sa==sb&&a->tick==b->tick);
   CK(nf18a5_history_hash(&ra->physical.world.history)==
      nf18a5_history_hash(&rb->physical.world.history));
   CK(ra->last_hash==rb->last_hash);
   CK(ra->physical.committed_receipts==rb->physical.committed_receipts);
  }
  CK(nf18a6_unbind_world_tick(a,ra)&&nf18a6_unbind_world_tick(b,rb));
  CK(nf18b6_push_detach(ctx,rb,b));
  memset(ctx,0,sizeof *ctx);
 }
 free(ctx);free(ra);free(rb);free(a);free(b);
 printf("paired_seeds=%u paired_ticks=%u incumbent_cpu_s=%.6f hook_cpu_s=%.6f (diagnostic only)\n",
        seeds,seeds*50,(double)incumbent_cost/CLOCKS_PER_SEC,
        (double)candidate_cost/CLOCKS_PER_SEC);
 return true;
}
static unsigned rng_next(unsigned *s){
 *s^=*s<<13;*s^=*s>>17;*s^=*s<<5;return *s;
}
static bool adversarial_corpus(void){
 NfWorld *w=malloc(sizeof *w);Nf18a6Runtime *r=malloc(sizeof *r);
 Nf18b6Push *ctx=calloc(1,sizeof *ctx);
 CK(w&&r&&ctx);
 CK(setup(w,r,2.2f));
 CK(nf18b6_push_attach(ctx,r,w,7));
 CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 unsigned seed=0xB618A6C1u;
 for(unsigned j=0;j<2048;++j){
  const unsigned bits=rng_next(&seed);
  const uint32_t action=(j%17u==0u)?0u:4000+j;
  const bool unknown=(bits&3u)==0u;
  const bool physical=false,contract=(bits&4u)!=0u;
  Nf18b3Result v=nf18b6_push_arm(ctx,w,action,physical,contract,unknown);
  CK(v==(!action?NF18B3_INVALID:
     unknown?NF18B3_PENDING:NF18B3_DENIED));
  CK(!ctx->armed&&w->tick==0&&ctx->receipt_count==0);
 }
 ctx->receipt_count=NF18B6_PUSH_RECEIPTS+1u;
 CK(nf18b6_push_arm(ctx,w,9000,true,true,false)==NF18B3_INVALID);
 CK(!ctx->armed&&w->tick==0);
 ctx->receipt_count=0;
 CK(nf18a6_unbind_world_tick(w,r));
 CK(nf18b6_push_detach(ctx,r,w));
 for(unsigned j=0;j<256;++j){
  CK(setup(w,r,2.2f));
  CK(nf18b6_push_attach(ctx,r,w,7));
  CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
  CK(nf18b6_push_arm(ctx,w,9000+j,true,true,false)==NF18B3_OK);
  switch(j%8u){
   case 0: ctx->event.actor++;break;
   case 1: ctx->event.object++;break;
   case 2: ctx->event.tick++;break;
   case 3: ctx->event.world_epoch++;break;
   case 4: ctx->event.material_epoch++;break;
   case 5: ctx->event.contract_epoch++;break;
   case 6: ctx->event.kind=NF18B3_CARRY;break;
   default: ctx->event.authorized=0;break;
  }
  const uint32_t rev=r->physical.world.revision;
  CK(nf_world_step_checked(w,1.f/60.f)!=NF_WORLD_TICK_COMMITTED);
  CK(w->tick==0&&r->physical.world.revision==rev);
  CK(!ctx->event.applied&&ctx->receipt_count==0);
  CK(!ctx->armed);
  CK(nf18a6_unbind_world_tick(w,r));
  CK(nf18b6_push_detach(ctx,r,w));
 }
 free(ctx);free(r);free(w);
 printf("adversarial_invalid_inputs=2048 stale_or_forged_frames=256 PASS\\n");
 return true;
}
static bool conservation_corpus(void){
 /* Independently check the two-body contact's linear impulse accounting.
    The motor is excluded so there is no hidden external momentum source.
    The resulting receipt is a physics observation, not a B3 publication. */
 unsigned seed=0xC06E5E17u;
 for(unsigned j=0;j<2048;++j){
  const unsigned s=rng_next(&seed);
  const float ma=10.f+(float)(s%79u), mb=10.f+(float)((s>>9)%79u);
  const float impact=0.25f+(float)((s>>17)%300u)*.01f;
  Nf18a4Body a={0},b={0};
  a.id=1;a.center=(NfVec3){0,0,0};
  a.velocity=(NfVec3){-impact,0,0};
  a.inverse_mass=1.f/ma;
  a.inverse_inertia=(NfVec3){1,1,1};
  a.radius=.3f;a.height=1.8f;
  b.id=2;b.center=(NfVec3){-1,0,0};
  b.inverse_mass=1.f/mb;
  b.inverse_inertia=(NfVec3){1,1,1};
  b.box_half=(NfVec3){.5f,.5f,.5f};
  const Nf18a4Constraint constraint={1,2,5,{1,0,0},{-.5f,0,0},0,0};
  const float p0=ma*a.velocity.x+mb*b.velocity.x;
  const float e0=.5f*ma*a.velocity.x*a.velocity.x;
  const Nf18a4Receipt receipt=nf18a4_apply_contact(
      &a,&b,constraint,NF18A4_RECIPROCAL_LINEAR,17,9);
  CK(receipt.status==NF18A4_ACCEPTED&&receipt.impulses_applied);
  const float p1=ma*a.velocity.x+mb*b.velocity.x;
  const float e1=.5f*ma*a.velocity.x*a.velocity.x+
                 .5f*mb*b.velocity.x*b.velocity.x;
  CK(fabsf(p1-p0)<=.002f);
  CK(e1<=e0+.002f);
  CK(a.velocity.x> -impact&&b.velocity.x<0);
 }
 printf("conservation_paired_impulses=2048 PASS\\n");
 return true;
}
typedef struct B2OwnerFixture {
 Nf18b2Actor actor;
 Nf18b2Object object;
 Nf18aCollider solids[2];
 unsigned provider_calls,deny_call;
 bool unavailable,forged,deny;
} B2OwnerFixture;
static bool b2_owned_snapshot(void *p,const NfWorld *w,
    const Nf18a6Runtime *r,uint32_t action,
    Nf18b2Scene *scene,Nf18b2Use *use){
 B2OwnerFixture *f=p;
 f->provider_calls++;
 if(f->unavailable)return false;
 const NfActor *a=nf_world_find_actor_const(w,r->actor_id);
 if(!a)return false;
 f->actor=(Nf18b2Actor){.id=a->id,.revision=1,
     .credentials=(f->deny||f->provider_calls==f->deny_call)?0u:1u,.inventory=0,
     .support_id=101,.feet=a->transform.position,.radius=r->physical.actor.radius,
     .height=r->physical.actor.height,.eye_height=a->movement.eye_height,
     .active=a->active?1u:0u,.grounded=a->movement.grounded?1u:0u};
 const Nf18a4Body *o=&r->physical.object;
 f->object=(Nf18b2Object){.id=o->id,.revision=1,.cell_id=1,
     .nexus_id=1,.slot_id=1,.target={o->center.x-o->box_half.x,
         o->center.y+o->box_half.y-.1f,o->center.z},
     .max_reach=2.5f,.required_credentials=1,.capacity=1,
     .preconditions=1,.requires_support=1};
 f->solids[0]=(Nf18aCollider){.body_id=101,
     .min=w->colliders[0].min,.max=w->colliders[0].max};
 if(f->forged)f->solids[0].max.x-=.25f;
 *scene=(Nf18b2Scene){.material=(Nf18a5World *)&r->physical.world,
     .geometry={.solids=f->solids,.solid_count=1,
                .world_version=r->physical.world.revision,
                .tick=(uint32_t)w->tick+1},
     .actors=&f->actor,.objects=&f->object,.actor_count=1,.object_count=1};
 *use=(Nf18b2Use){.actor_id=a->id,.object_id=o->id,
      .action_id=action,.tick=(uint32_t)w->tick+1,
      .world_revision=r->physical.world.revision,
      .material_epoch=r->physical.world.grid.global_revision,
      .actor_revision=1,.object_revision=1};
 return true;
}
static bool b2_real_server_gate(void){
 NfWorld *w=malloc(sizeof *w);
 Nf18a6Runtime *r=malloc(sizeof *r);
 Nf18b6Push *ctx=calloc(1,sizeof *ctx);
 B2OwnerFixture *f=calloc(1,sizeof *f);
 CK(w&&r&&ctx&&f);
 CK(setup(w,r,2.2f));
 CK(nf18b6_push_attach(ctx,r,w,7));
 CK(nf18b6_push_require_b2(ctx,b2_owned_snapshot,f,
                           NF18B2_M4_MATERIAL_FIRST));
 CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 CK(nf18b6_push_arm(ctx,w,5,true,true,false)==NF18B3_INVALID);
 f->unavailable=true;
 CK(nf18b6_push_arm_b2(ctx,w,5)==NF18B3_PENDING);
 CK(!ctx->armed&&ctx->receipt_count==0);
 f->unavailable=false;f->forged=true;
 CK(nf18b6_push_arm_b2(ctx,w,5)==NF18B3_DENIED);
 CK(ctx->last_b2_status==NF18B2_STALE);
 f->forged=false;f->deny=true;
 CK(nf18b6_push_arm_b2(ctx,w,5)==NF18B3_DENIED);
 CK(ctx->last_b2_status==NF18B2_BLOCKED);
 f->deny=false;
 /* Compare the distinct B2 jurisdiction orderings on identical live A6
    server snapshots, without conflating this with real-world H3 admission. */
 unsigned m3_shape_queries=0,m4_shape_queries=0;
 for(unsigned i=0;i<256;++i){
  f->deny=(i&1u)!=0u;
  Nf18b2Scene scene={0};Nf18b2Use use={0};
  CK(b2_owned_snapshot(f,w,r,500+i,&scene,&use));
  const Nf18b2UseDecision a=nf18b2_evaluate(&scene,use,NF18B2_M3_EXHAUSTIVE);
  const Nf18b2UseDecision b=nf18b2_evaluate(&scene,use,NF18B2_M4_MATERIAL_FIRST);
  CK(a.status==b.status&&a.contract==b.contract&&a.physical==b.physical);
  m3_shape_queries+=a.geometry_queries;
  m4_shape_queries+=b.geometry_queries;
 }
 CK(m3_shape_queries==m4_shape_queries);
 f->deny=false;
 /* H10 source-exact positive path: the existing A6-owned B2 fixture and
    actual A5 canonical FREE point issue an H9 one-use witness. */
 const NfVec3 exact_probe={1.f,.1f,1.f};
 CK(nf_c2_h9_issue(w,r,b2_owned_snapshot,f,40000,exact_probe)==NF_C2W_READY);
 CK(nf_c2_h9_valid(w,r,40000));
 CK(!nf_c2_h9_consume(w,r,40001)); /* wrong action revokes */
 CK(nf_c2_h9_issue(w,r,b2_owned_snapshot,f,40002,exact_probe)==NF_C2W_READY);
 CK(nf_c2_h9_consume(w,r,40002));
 CK(!nf_c2_h9_consume(w,r,40002)); /* one-shot */
 f->deny=true;
 CK(nf_c2_h9_issue(w,r,b2_owned_snapshot,f,40003,exact_probe)==NF_C2W_DENIED);
 CK(!nf_c2_h9_valid(w,r,40003));
 f->deny=false;
 printf("b2_m3_m4_same_frame=256 shape_queries_m3=%u m4=%u\\n",
        m3_shape_queries,m4_shape_queries);
 Nf18b3Result decision=nf18b6_push_arm_b2(ctx,w,5);
 fprintf(stderr,"B2 valid snapshot returned %d B2 status %d\\n",
         (int)decision,(int)ctx->last_b2_status);
 CK(decision==NF18B3_OK);
 CK(ctx->armed);
 const uint32_t prior_epoch=r->physical.world.grid.global_revision;
 f->deny=true; /* requirement changed after arming: fail closed */
 CK(nf_world_step_checked(w,1.f/60.f)!=NF_WORLD_TICK_COMMITTED);
 CK(!ctx->armed&&w->tick==0&&ctx->receipt_count==0);
 CK(r->physical.world.grid.global_revision==prior_epoch);
 f->deny=false;
 CK(nf18b6_push_arm_b2(ctx,w,6)==NF18B3_OK);
 f->deny_call=f->provider_calls+2u; /* begin still eligible; prepublish revoked */
 const uint32_t revision_before=r->physical.world.revision;
 CK(nf_world_step_checked(w,1.f/60.f)!=NF_WORLD_TICK_COMMITTED);
 CK(w->tick==0&&ctx->receipt_count==0&&!ctx->event.applied);
 CK(revision_before==r->physical.world.revision);
 CK(!ctx->armed);
 f->deny_call=0;
 CK(nf18b6_push_arm_b2(ctx,w,7)==NF18B3_OK);
 CK(nf_world_step_checked(w,1.f/60.f)==NF_WORLD_TICK_COMMITTED);
 CK(w->tick==1&&ctx->receipt_count==0&&!ctx->event.applied);
 unsigned b2_real_contact=0,verified_ticks=1;
 for(unsigned j=0;j<40;++j){
  nf_world_set_input(w,10,rightward());
  const uint32_t old_raw=r->physical.world.history.raw_samples;
  const uint32_t old_applied=ctx->receipt_count;
  const uint64_t old_tick=w->tick;
  const Nf18b3Result allowed=nf18b6_push_arm_b2(ctx,w,1000+j);
  if(allowed!=NF18B3_OK){
   CK(allowed==NF18B3_PENDING||allowed==NF18B3_DENIED);
   CK(w->tick==old_tick&&ctx->receipt_count==old_applied);
   break;
  }
  const NfWorldTickStatus status=nf_world_step_checked(w,1.f/60.f);
  if(status!=NF_WORLD_TICK_COMMITTED){
   CK(status==NF_WORLD_TICK_PENDING||status==NF_WORLD_TICK_REJECTED);
   CK(w->tick==old_tick&&ctx->receipt_count==old_applied);
   CK(r->physical.world.history.raw_samples==old_raw);
   break;
  }
  verified_ticks++;
  if(r->physical.world.history.raw_samples>old_raw){
   CK(ctx->last_result==NF18B3_OK);
   CK(ctx->receipt_count==old_applied+1u&&ctx->event.applied);
   b2_real_contact++;
  }else CK(ctx->receipt_count==old_applied&&!ctx->event.applied);
 }
 CK(b2_real_contact>0);
 printf("b2_verified_ticks=%u b2_real_contact_receipts=%u\\n",
        verified_ticks,b2_real_contact);
 CK(f->provider_calls>=7);
 CK(nf18a6_unbind_world_tick(w,r)&&nf18b6_push_detach(ctx,r,w));
 free(f);free(ctx);free(r);free(w);
 return true;
}

static bool test_h11_owner_capability_prepublication(void){
 NfWorld *w=malloc(sizeof *w);
 Nf18a6Runtime *r=malloc(sizeof *r);
 Nf18b6Push *ctx=calloc(1,sizeof *ctx);
 B2OwnerFixture *f=calloc(1,sizeof *f);
 CK(w&&r&&ctx&&f);
 CK(setup(w,r,2.2f));
 CK(nf18b6_push_attach(ctx,r,w,7));
 CK(nf18b6_push_require_b2(ctx,b2_owned_snapshot,f,
                            NF18B2_M4_MATERIAL_FIRST));
 CK(nf18b6_push_require_h9(ctx));
 char receipt_path[160];snprintf(receipt_path,sizeof receipt_path,
   "/tmp/nightfall_h20_receipts_%ld.bin",(long)getpid());
 remove(receipt_path);
 CK(nf18b6_push_h20_journal(ctx,receipt_path));
 char envelope_path[160];snprintf(envelope_path,sizeof envelope_path,
   "/tmp/nightfall_h24_check_%ld.bin",(long)getpid());
 CK(nf18b6_push_h24_envelope(ctx,envelope_path));
 CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 CK(nf_c2_lease_acquire(&r->world_lease));
 CK(!nf18a6_unbind_world_tick(w,r)); /* registered holder blocks teardown */
 nf_c2_lease_release(&r->world_lease);
 /* Unbind attempt closes lease; must rebind after fully draining. */
 CK(nf18a6_unbind_world_tick(w,r));
 CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 const NfVec3 probe={1.f,.1f,1.f};
 /* Missing capability: B2 alone never permits physical publication. */
 CK(nf18b6_push_arm_b2(ctx,w,600)==NF18B3_OK);
 const uint32_t before_physical=r->physical.committed_receipts;
 const uint32_t before_samples=r->physical.world.history.raw_samples;
 CK(nf_world_step_checked(w,1.f/60.f)!=NF_WORLD_TICK_COMMITTED);
 CK(w->tick==0&&ctx->receipt_count==0);
 CK(r->physical.committed_receipts==before_physical);
 CK(r->physical.world.history.raw_samples==before_samples);
 CK(!ctx->armed);
 /* Revoked capability: prepublication refuses it. */
 CK(nf_c2_h9_issue(w,r,b2_owned_snapshot,f,601,probe)==NF_C2W_READY);
 CK(nf18b6_push_arm_b2(ctx,w,601)==NF18B3_OK);
 nf_c2_h9_revoke(r);
 CK(nf_world_step_checked(w,1.f/60.f)!=NF_WORLD_TICK_COMMITTED);
 CK(w->tick==0&&ctx->receipt_count==0);
 CK(r->physical.committed_receipts==before_physical);
 CK(r->physical.world.history.raw_samples==before_samples);
 /* Stale material dependency must fail closed. */
 CK(nf_c2_h9_issue(w,r,b2_owned_snapshot,f,602,probe)==NF_C2W_READY);
 CK(nf18b6_push_arm_b2(ctx,w,602)==NF18B3_OK);
 nf_c2_h9_revoke(r); /* emulate revoked/stale authorization */
 CK(nf_world_step_checked(w,1.f/60.f)!=NF_WORLD_TICK_COMMITTED);
 CK(w->tick==0&&ctx->receipt_count==0);
 CK(r->physical.committed_receipts==before_physical);
 CK(r->physical.world.history.raw_samples==before_samples);
 /* Already-consumed capability may not be replayed. */
 CK(nf_c2_h9_issue(w,r,b2_owned_snapshot,f,603,probe)==NF_C2W_READY);
 CK(nf_c2_h9_consume(w,r,603));
 CK(nf18b6_push_arm_b2(ctx,w,603)==NF18B3_OK);
 CK(nf_world_step_checked(w,1.f/60.f)!=NF_WORLD_TICK_COMMITTED);
 CK(w->tick==0&&ctx->receipt_count==0);
 CK(nf_c2_h9_issue(w,r,b2_owned_snapshot,f,604,probe)==NF_C2W_READY);
 CK(nf18b6_push_arm_b2(ctx,w,604)==NF18B3_OK);
 CK(nf_world_step_checked(w,1.f/60.f)==NF_WORLD_TICK_COMMITTED);
 CK(w->tick==1);
 /* H18 positive source-bound publication: only count physical B3 successes
    when A4 reciprocal impulse and actual A5 contact history both advance. */
 /* An unproven prepublication fault cannot be asserted on an arbitrary
    movement tick: a no-contact tick need not invoke the push gate.
    H34 instead retains source-bound positive contact and separate cutpoints.
    Do not count missing gate invocation as passing atomicity evidence. */
 unsigned positive_contacts=0;
 for(unsigned j=0;j<40;++j){
  nf_world_set_input(w,10,rightward());
  const uint32_t tick_before=(uint32_t)w->tick;
  const uint32_t a4_before=r->physical.committed_receipts;
  const uint32_t a5_before=r->physical.world.history.raw_samples;
  const uint32_t b3_before=ctx->receipt_count;
  uint32_t action=700+j;
  NfC2WitnessStatus grant=nf_c2_h9_issue(w,r,b2_owned_snapshot,f,action,probe);
  if(grant!=NF_C2W_READY){
   CK(grant==NF_C2W_DENIED||grant==NF_C2W_PENDING);
   CK(w->tick==tick_before&&ctx->receipt_count==b3_before);
   break;
  }
  Nf18b3Result armed=nf18b6_push_arm_b2(ctx,w,action);
  if(armed!=NF18B3_OK){CK(armed==NF18B3_DENIED||armed==NF18B3_PENDING);break;}
  NfWorldTickStatus outcome=nf_world_step_checked(w,1.f/60.f);
  if(outcome!=NF_WORLD_TICK_COMMITTED){
   CK(w->tick==tick_before&&ctx->receipt_count==b3_before);
   CK(r->physical.committed_receipts==a4_before);
   CK(r->physical.world.history.raw_samples==a5_before);
   break;
  }
  if(ctx->receipt_count>b3_before){
   CK(r->physical.committed_receipts>a4_before);
   CK(r->physical.world.history.raw_samples>a5_before);
   CK(ctx->event.applied);
   CK(ctx->staged_commit_valid);
   CK(ctx->staged_commit.action==ctx->event.action);
   CK(ctx->staged_commit.tick==ctx->event.tick);
   Nf18a6Snapshot accepted_world={0};Nf18b3Event accepted_contract={0};
   CK(nf18b6_push_h37_committed_view(w,r,ctx,
      &accepted_world,&accepted_contract));
   CK(ctx->published_epoch==w->tick);
   CK(r->joint_commit_epoch==ctx->published_epoch);
   CK(accepted_world.tick==w->tick&&
      accepted_contract.action==ctx->event.action);
   ++positive_contacts;
  } else CK(!ctx->event.applied);
 }
 CK(positive_contacts>0);
 Nf18a5Integrated *h25_restored=calloc(1,sizeof *h25_restored);
 Nf18b3Event h25_event={0};
 CK(h25_restored!=NULL);
 CK(nf_c2_h23_restore_full(envelope_path,h25_restored,&h25_event));
 CK(h25_restored->world.tick==w->tick);
 CK(h25_restored->committed_receipts==r->physical.committed_receipts);
 CK(h25_restored->world.history.raw_samples==
    r->physical.world.history.raw_samples);
 CK(h25_event.phase==NF18B3_APPLIED&&h25_event.applied);
 free(h25_restored);
 CK(remove(envelope_path)==0);
 FILE *receipt_file=fopen(receipt_path,"rb");CK(receipt_file!=NULL);
 uint32_t receipt_fields[7];unsigned recorded=0;
 while(fread(receipt_fields,sizeof receipt_fields,1,receipt_file)==1){
  CK(receipt_fields[0]>0&&receipt_fields[1]==10&&receipt_fields[3]>0);
  CK(receipt_fields[4]>0&&receipt_fields[5]>0);
  CK(nf_c2_h21_reconcile(receipt_path,receipt_fields[0],receipt_fields[1],
       receipt_fields[2],receipt_fields[3],receipt_fields[4],receipt_fields[5])
       ==NF_C2_H21_MATCHED);
  CK(nf_c2_h21_reconcile(receipt_path,receipt_fields[0],receipt_fields[1],
       receipt_fields[2],receipt_fields[3],receipt_fields[4]+1,receipt_fields[5])
       ==NF_C2_H21_REJECT);
  CK(receipt_fields[6]==(receipt_fields[0]^receipt_fields[1]^receipt_fields[2]^
     receipt_fields[3]^receipt_fields[4]^receipt_fields[5]^0xA5B318C2u));
  ++recorded;
 }
 CK(fclose(receipt_file)==0);
 CK(recorded==positive_contacts);
 CK(remove(receipt_path)==0);
 CK(nf_c2_h21_reconcile(receipt_path,1,10,700,1,1,1)==NF_C2_H21_UNRESOLVED);
 printf("h20_persisted_a4_a5_b3_receipts=%u\\n",recorded);
 CK(nf18a6_unbind_world_tick(w,r));
 CK(nf18b6_push_detach(ctx,r,w));
 free(f);free(ctx);free(r);free(w);
 return true;
}
static bool test_h36_capacity_precommit(void){
 NfWorld *w=calloc(1,sizeof *w);
 Nf18a6Runtime *r=calloc(1,sizeof *r);
 Nf18b6Push *ctx=calloc(1,sizeof *ctx);
 CK(w&&r&&ctx);
 CK(setup(w,r,2.2f));
 CK(nf18b6_push_attach(ctx,r,w,7));
 CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 CK(nf18b6_push_arm(ctx,w,990,true,true,false)==NF18B3_OK);
 ctx->receipt_count=NF18B6_PUSH_RECEIPTS;
 const uint32_t old_tick=(uint32_t)w->tick;
 const uint32_t old_a4=r->physical.committed_receipts;
 const uint32_t old_a5=r->physical.world.history.raw_samples;
 nf_world_set_input(w,10,rightward());
 CK(nf_world_step_checked(w,1.f/60.f)!=NF_WORLD_TICK_COMMITTED);
 CK(w->tick==old_tick&&r->physical.committed_receipts==old_a4);
 CK(r->physical.world.history.raw_samples==old_a5);
 CK(ctx->receipt_count==NF18B6_PUSH_RECEIPTS);
 CK(nf18a6_unbind_world_tick(w,r));
 CK(nf18b6_push_detach(ctx,r,w));
 free(ctx);free(r);free(w);
 return true;
}
int main(void){
 clock_t begin=clock();
 if(!test_contract_and_replay()||!test_missing_material_and_remote_actor()||
    !test_no_behavior_regression(32)||!adversarial_corpus()||
    !conservation_corpus()||!b2_real_server_gate()||
    !test_h11_owner_capability_prepublication()||
    !test_h36_capacity_precommit())return 1;
 printf("assertions=%llu cpu_seconds=%.6f PASS\n",checks,
       (double)(clock()-begin)/CLOCKS_PER_SEC);
 return 0;
}
