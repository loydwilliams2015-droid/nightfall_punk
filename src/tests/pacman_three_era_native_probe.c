/* nightfall!punk Pac-Man-inspired physics PROBE, not Pac-Man ghost AI.
 * Exactly the same C source is compiled against pinned 1.8A.6 and 1.8B.6
 * engine builds. Pac-Man position is a 3x3 goal tile; only one real actor
 * is spawned because historical A6 deliberately forbids a second owner.
 * This cannot measure attack/chase/guard AI decision quality.
 */
#include "nf_embody18a6.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t next(uint32_t *v){
 *v^=*v<<13;*v^=*v>>17;*v^=*v<<5;return *v;
}
static int scenario(unsigned family,unsigned index){
 NfWorld *w=malloc(sizeof *w);
 Nf18a6Runtime *r=malloc(sizeof *r);
 if(!w||!r){free(w);free(r);return 2;}
 nf_world_init(w,42u);
 if(nf_world_add_collider(w,NF_COLLIDER_SOLID,
    (NfVec3){-1,-.5f,-1},(NfVec3){5,0,5})<0)return 3;
 if(nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,
    (NfVec3){1,0,1})!=10)return 4;
 NfActor *a=nf_world_find_actor(w,10);
 a->movement.grounded=true;a->movement.mode=NF_MOVE_GROUND;
 uint32_t seed=0x20260813u+(family+1u)*1009u+index*23u;
 const uint32_t jitter=next(&seed);
 Nf18a4Body crate={0};
 crate.id=2010;crate.center=(NfVec3){2.0f+.01f*(jitter%9u),.9f,1};
 crate.inverse_mass=1.f/24.f;
 crate.inverse_inertia=(NfVec3){.3f,.3f,.3f};
 crate.box_half=(NfVec3){.2f,.5f,.35f};
 if(!nf18a6_init(r,w,10,crate,3,NF18A6_WORLD_BRIDGE))return 5;
 uint8_t zero[NF18A5_CANON_VOXELS]={0};
 if(!nf18a5_load_canonical(&r->physical.world.grid,0,0,0,zero,1))return 6;
 if(!nf18a6_bind_world_tick(w,r,NULL,NULL,NULL))return 7;
 /* Historical 10-tick v1.5 player positions B,E,H,G,G,H,I,I,H,E
    repeated 3 times to expose obstruction/pending, without pretending
    the physical actor has an omniscient AI. */
 const unsigned target[]={1,4,7,6,6,7,8,8,7,4};
 unsigned commits=0,pending=0,rejected=0,contacts=0;
 unsigned long first_pending=0;
 for(unsigned tick=0;tick<30;++tick){
  const unsigned goal=target[tick%10];
  NfMoveInput in={0};
  const NfActor *now=nf_world_find_actor_const(w,10);
  const float gx=1.f+(float)(goal%3);
  const float gz=1.f+(float)(goal/3);
  const float dx=gx-now->transform.position.x;
  const float dz=gz-now->transform.position.z;
  if(fabsf(dx)>.15f)in.strafe=dx>0?1.f:-1.f;
  if(fabsf(dz)>.15f)in.forward=dz>0?1.f:-1.f;
  /* Strata alter only the action schedule, not authoritative code.
     canonical, randomized, pathological, boundary, and held-out. */
  if(family==1&&(next(&seed)&3u)==0u)in.strafe=0;
  if(family==2){in.strafe=1;in.forward=0;}
  if(family==3&&(tick%3u)==0u)in=(NfMoveInput){0};
  if(family==4&&(next(&seed)&1u)==0u)in.forward=0;
  nf_world_set_input(w,10,in);
  const uint32_t old_contacts=r->physical.committed_receipts;
  const uint64_t old_tick=w->tick;
  const uint32_t old_hist=r->physical.world.history.raw_samples;
  NfWorldTickStatus status=nf_world_step_checked(w,1.f/60.f);
  if(status==NF_WORLD_TICK_COMMITTED){
   if(w->tick!=old_tick+1||r->physical.world.tick!=w->tick)return 8;
   ++commits;
   if(r->physical.committed_receipts>old_contacts){
    if(r->physical.world.history.raw_samples<=old_hist)return 9;
    ++contacts;
   }
  }else{
   if(w->tick!=old_tick||r->physical.world.tick!=old_tick||
      r->physical.committed_receipts!=old_contacts||
      r->physical.world.history.raw_samples!=old_hist)return 10;
   if(status==NF_WORLD_TICK_PENDING)++pending;else ++rejected;
   if(!first_pending)first_pending=tick+1;
   break; /* Retain fail-closed behavior; no forced tick. */
  }
 }
 const NfActor *now=nf_world_find_actor_const(w,10);
 printf("RESULT family=%u seed=%u commit=%u pending=%u reject=%u contact=%u"
        " first_stop=%lu hash=%u history=%u x=%.6f z=%.6f\n",
        family,index,commits,pending,rejected,contacts,first_pending,
        r->last_hash,r->physical.world.history.raw_samples,
        now->transform.position.x,now->transform.position.z);
 if(!nf18a6_unbind_world_tick(w,r))return 11;
 free(r);free(w);
 return 0;
}
int main(void){
 puts("PACMAN_NATIVE_H1 3x3 goals B,E,H,G,G,H,I,I,H,E repeated, one real actor");
 for(unsigned f=0;f<5;++f)
  for(unsigned seed=0;seed<32;++seed){
   const int rc=scenario(f,seed);
   if(rc){fprintf(stderr,"FAIL f=%u seed=%u rc=%d\n",f,seed,rc);return rc;}
  }
 puts("NATIVE_H1_SCENARIOS=160 PASS");
 return 0;
}
