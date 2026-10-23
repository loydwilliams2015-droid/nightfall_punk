/* v1.8C.2 research: real A4 impulse / A5 material / B3 typed gates.
 * B3 success is intentionally NOT claimed without an A6 durable publisher. */
#include "nf_contact18a5.h"
#include "nf_interact18b3.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
static uint32_t state=0x18c2d017u;
static uint32_t rng(void){state^=state<<13;state^=state>>17;state^=state<<5;return state;}
static unsigned total, impulse_count, pending_count;
#define CHECK(x) do { if(!(x)){fprintf(stderr,"FAIL:%d %s\n",__LINE__,#x);return 1;}++total;}while(0)
static Nf18a4Body body(unsigned id,float x,float v,float mass){
 Nf18a4Body b={0};b.id=id;b.center=(NfVec3){x,1,0};b.velocity=(NfVec3){v,0,0};
 b.inverse_mass=1.f/mass;b.inverse_inertia=(NfVec3){1,1,1};
 b.radius=.3f;b.height=1.8f;return b;
}
int main(void){
 const unsigned cases[5]={128,256,192,128,173};
 for(unsigned family=0;family<5;family++)for(unsigned i=0;i<cases[family];i++){
  float ma=10+(rng()%80u),mb=10+(rng()%80u);
  float speed=.1f+(rng()%400u)/80.f;
  Nf18a4Body a=body(1,0,-speed,ma),b=body(2,-.6f,0,mb);
  Nf18a4Constraint c={0};c.body_a=1;c.body_b=2;
  c.normal_b_to_a=(NfVec3){1,0,0};c.contact_point=(NfVec3){-.3f,1,0};
  const float before=ma*a.velocity.x+mb*b.velocity.x;
  Nf18a4Receipt r=nf18a4_apply_contact(&a,&b,c,NF18A4_RECIPROCAL_LINEAR,1,1);
  CHECK(r.status==NF18A4_ACCEPTED&&r.impulses_applied);
  CHECK(fabsf(before-(ma*a.velocity.x+mb*b.velocity.x))<.003f);
  ++impulse_count;
  Nf18a5Grid g;nf18a5_grid_init(&g,1,NF18A5_SELECTIVE_FINE);
  uint8_t voxels[NF18A5_CANON_VOXELS]={0};
  voxels[0]=NF18A5_MIXED;
  CHECK(nf18a5_load_canonical(&g,0,0,0,voxels,1));
  CHECK(nf18a5_query(&g,.1f,.1f,.1f,1,true)==NF18A5_Q_PENDING);
  ++pending_count;
  Nf18b3Event e={0};e.actor=1;e.object=2;e.action=i+1;
  e.tick=1;e.world_epoch=1;e.material_epoch=g.global_revision;e.contract_epoch=1;
  e.kind=(Nf18b3Kind)(i%NF18B3_KIND_COUNT);
  CHECK(nf18b3_authorize(&e,true,true,true)==NF18B3_PENDING);
  CHECK(e.applied==0);
  if(family==3){ /* bound: stale contract must deny */
   Nf18b3Event z={0};z.actor=1;z.object=2;z.action=1;z.kind=NF18B3_PUSH;
   CHECK(nf18b3_authorize(&z,true,false,false)==NF18B3_DENIED);
  }
 }
 printf("PASS real A4/A5/B3 lab cases=%u checks=%u impulses=%u pending=%u\n",
        877u,total,impulse_count,pending_count);
 return 0;
}
