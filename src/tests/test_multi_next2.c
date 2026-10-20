#include "nf_multi_next2.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
static unsigned long long checks;
#define CK(x) do{if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);return 1;}checks++;}while(0)
typedef struct {Nf18a5World world;Nf18b2Actor actor;Nf18b2Object object;} Fixture;
static bool provider(void *v,Nf18b2Scene *scene,Nf18b2Use *use){
 Fixture *f=v;memset(scene,0,sizeof *scene);
 scene->material=&f->world;scene->actors=&f->actor;scene->actor_count=1;
 scene->objects=&f->object;scene->object_count=1;
 scene->geometry.tick=2;scene->geometry.world_version=f->world.revision;
 *use=(Nf18b2Use){1,10,30,2,f->world.revision,f->world.grid.global_revision,
 f->actor.revision,f->object.revision};
 return true;
}
int main(void){
 for(unsigned family=0;family<5;family++)for(unsigned seed=0;seed<128;seed++){
  Nf18a4Body bodies[2]={0};
  bodies[0].id=1;bodies[1].id=2;
  for(int i=0;i<2;i++){bodies[i].inverse_mass=1;bodies[i].radius=.35f;bodies[i].height=1.8f;bodies[i].center.y=1;}
  bodies[1].center.x=2;
  bodies[0].velocity.x=150;
  if(family==1)bodies[0].velocity.x=1;
  if(family==2)bodies[1].center.x=.1f;
  if(family==3)bodies[1].velocity.y=1;
  if(family==4)bodies[1].center.z=10;
  NfMa01c2Result result=nf_ma01c2_integrate(bodies,2,1.f/60.f,2,1);
  if(family==0){CK(result.status==NF_NARROW_OK);CK(result.impacts==1);
    CK(result.bodies[1].velocity.x>0);CK(fabsf(result.advanced_seconds-1.f/60.f)<1.e-5f);}
  if(family==1||family==4)CK(result.status==NF_NARROW_OK&&result.impacts==0);
  if(family==2||family==3)CK(result.status==NF_NARROW_PENDING);
  CK(bodies[0].center.x==0);
  Fixture f={0};nf18a5_world_init(&f.world,1);
  f.actor=(Nf18b2Actor){.id=1,.revision=1,.active=1,.grounded=1,.radius=.3f,.height=1.8f,.eye_height=1.6f};
  f.object=(Nf18b2Object){.id=10,.revision=1,.cell_id=1,.nexus_id=1,.slot_id=1,.capacity=1,.preconditions=1,.max_reach=3,
    .target={0,1.6f,0}};
  Nf18b2UseDecision decision=nf_ma02_live_evaluate(provider,&f,1,30,NF18B2_M4_MATERIAL_FIRST);
  CK(decision.status!=NF18B2_ELIGIBLE); /* no material chunks => no grant */
  CK(nf_ma02_live_evaluate(provider,&f,2,30,NF18B2_M4_MATERIAL_FIRST).status==NF18B2_INVALID);
 }
 printf("MA01C2_MA022 scenarios=640 checks=%llu PASS\n",checks);
 return 0;
}
