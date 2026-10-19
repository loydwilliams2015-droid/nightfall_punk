#include "nf_b6_push18b6.h"
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
  CK(nf_world_step_checked(w,1.f/60.f)==NF_WORLD_TICK_COMMITTED);
  CK(w->tick==j+1&&ctx->armed==false);
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
   const NfWorldTickStatus sa=nf_world_step_checked(a,1.f/60.f);
   const NfWorldTickStatus sb=nf_world_step_checked(b,1.f/60.f);
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
 printf("paired_seeds=%u paired_ticks=%u\n",seeds,seeds*50);
 return true;
}
int main(void){
 clock_t begin=clock();
 if(!test_contract_and_replay()||!test_missing_material_and_remote_actor()||
    !test_no_behavior_regression(32))return 1;
 printf("assertions=%llu cpu_seconds=%.6f PASS\n",checks,
       (double)(clock()-begin)/CLOCKS_PER_SEC);
 return 0;
}
