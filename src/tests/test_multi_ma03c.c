#include "nf_multi_ma03c.h"
#include "nf_multi_ma03.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
static unsigned checks;
#define CK(x) do{if(!(x)){fprintf(stderr,"FAIL line %d %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static bool unavailable(void *ctx,const NfWorld *w,const Nf18a6Runtime *r,uint32_t action,Nf18b2Scene *s,Nf18b2Use *u){
 (void)ctx;(void)w;(void)r;(void)action;(void)s;(void)u;return false;
}
int main(void){
 /* H81 positive precursor: use real NfWorld actor snapshots to feed the
    EXISTING MA01C2/MA03C contact scheduler, not a fabricated callback.
    This is an independent read-only proposal, not an A6 joint commit. */
 NfWorld *live=calloc(1,sizeof *live);
 CK(live!=NULL);
 nf_world_init(live,80u);
 CK(nf_world_spawn_actor_with_id(live,20,NF_FACTION_PLAYER,(NfVec3){2.f,1.f,0.f})==20);
 CK(nf_world_spawn_actor_with_id(live,10,NF_FACTION_PLAYER,(NfVec3){0.f,1.f,0.f})==10);
 NfActor *fast=nf_world_find_actor(live,10);
 NfActor *slow=nf_world_find_actor(live,20);
 CK(fast&&slow);
 fast->transform.velocity.x=150.f;
 NfMa03Roster roster={0};
 CK(nf_ma03_roster_capture(live,&roster));
 CK(roster.count==2&&roster.actors[0].id==10&&roster.actors[1].id==20);
 Nf18a4Body bodies[2]={0};
 for(unsigned i=0;i<2;++i){
   bodies[i].id=roster.actors[i].id;
   bodies[i].center=roster.actors[i].position;
   bodies[i].velocity=roster.actors[i].velocity;
   bodies[i].inverse_mass=1.f;
   bodies[i].radius=.3f;
   bodies[i].height=1.8f;
 }
 NfMa03cEvidence positive=nf_ma03c_compare(bodies,2,1.f/60.f,1,1);
 CK(positive.matched==1&&positive.disagreed==0&&positive.unresolved==0);
 CK(live->tick==0u);
 NfMa03cCandidate staged={0};
 CK(nf_ma03c_stage_contact(live,10u,1.f/60.f,&staged));
 CK(staged.eligible&&staged.contact.matched==1u&&
    staged.roster.count==2u&&staged.source_tick==0u);
 const NfMa03cCandidate original=staged;
 CK(!nf_ma03c_stage_contact(live,99u,1.f/60.f,&staged));
 CK(memcmp(&original,&staged,sizeof staged)==0);
 slow->input.forward=2.f;
 CK(!nf_ma03c_stage_contact(live,10u,1.f/60.f,&staged));
 CK(memcmp(&original,&staged,sizeof staged)==0);
 slow->input.forward=0.f;
 CK(fast->transform.velocity.x==150.f&&slow->transform.velocity.x==0.f);
 free(live);
 for(unsigned family=0;family<5;family++)for(unsigned seed=0;seed<128;seed++){
  Nf18a4Body b[3]={0};
  for(unsigned i=0;i<3;i++){b[i].id=i+1;b[i].inverse_mass=1.f/(1.f+(float)(i+seed%3));
   b[i].radius=.3f;b[i].height=1.8f;b[i].center.y=1;}
  b[0].velocity.x=150;b[1].center.x=2;b[2].center.x=4;
  if(family==1)b[0].velocity.x=1;
  if(family==2)b[1].center.x=.1f;
  if(family==3)b[1].velocity.y=2;
  if(family==4)b[1].center.z=10;
  NfMa03cEvidence e=nf_ma03c_compare(b,family==4?3:2,1.f/60.f,1,1);
  if(family==0)CK(e.matched==1&&e.disagreed==0);
  else CK(e.unresolved==1&&e.disagreed==0);
 }
 NfWorld w={0};Nf18a6Runtime r={0};NfMa024Receipt receipt={0};
 CK(!nf_ma024_record(&w,&r,unavailable,NULL,7,NF18B2_M4_MATERIAL_FIRST,&receipt));
 CK(!nf_ma024_recheck(&w,&r,unavailable,NULL,7,NF18B2_M4_MATERIAL_FIRST,&receipt));
 printf("MA03C_MA024 scenarios=640 checks=%u PASS\n",checks);return 0;
}
