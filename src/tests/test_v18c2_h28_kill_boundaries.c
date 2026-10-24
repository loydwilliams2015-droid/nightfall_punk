#define _POSIX_C_SOURCE 200809L
#include "nf_c2_h22_envelope.h"
#include "nf_embody18a6.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"H28 FAIL line %d\n",__LINE__);return 1;}}while(0)
static int recovery_child(const char *path,int expected){
 pid_t id=fork();if(id<0)return 0;
 if(!id){
  Nf18a5Integrated *state=calloc(1,sizeof *state);Nf18b3Event event={0};
  if(!state)_exit(12);
  int ok=nf_c2_h23_restore_full(path,state,&event);
  int result=ok&&state->world.tick==1&&state->committed_receipts==1&&
       event.kind==NF18B3_PUSH&&event.action==91&&event.phase==NF18B3_APPLIED&&
       event.authorized&&event.applied;
  free(state);_exit((ok==expected&&(!ok||result))?0:13);
 }
 int status=0;return waitpid(id,&status,0)==id&&WIFEXITED(status)&&WEXITSTATUS(status)==0;
}
int main(void){
 NfWorld *w=calloc(1,sizeof *w);Nf18a6Runtime *r=calloc(1,sizeof *r);
 CK(w&&r);nf_world_init(w,42);
 CK(nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,(NfVec3){1,0,1})==10);
 Nf18a4Body box={0};box.id=2010;box.center=(NfVec3){2,1,1};
 box.inverse_mass=1.f/24.f;box.box_half=(NfVec3){.2f,.5f,.35f};
 CK(nf18a6_init(r,w,10,box,3,NF18A6_WORLD_BRIDGE));
 r->physical.world.tick=1;r->physical.committed_receipts=1;
 r->physical.world.history.raw_samples=1;
 NfC2H22Receipt rec={.tick=1,.actor=10,.action=91,
   .a4_receipts=1,.a5_samples=1,.b3_receipts=1};
 rec.b3_event=(Nf18b3Event){.actor=10,.object=2010,.action=91,
  .tick=1,.contract_epoch=7,.kind=NF18B3_PUSH,
  .phase=NF18B3_APPLIED,.authorized=1,.applied=1};
 const char *boundaries[]={"before_stage","after_physical_stage",
   "after_stage_write","after_file_sync","after_rename"};
 char path[180];snprintf(path,sizeof path,"/tmp/nightfall_h28_%ld.bin",(long)getpid());
 for(unsigned i=0;i<5;i++){
  remove(path);
  CK(recovery_child(path,0));
  pid_t id=fork();CK(id>=0);
  if(!id){
   setenv("NF_H28_KILL_BOUNDARY",boundaries[i],1);
   (void)nf_c2_h22_save(path,&r->physical,rec);
   _exit(14);
  }
  int status=0;CK(waitpid(id,&status,0)==id&&WIFEXITED(status)&&WEXITSTATUS(status)==93);
  CK(recovery_child(path,i==4));
  CK(remove(path)==0||i!=4);
  /* Killpoint before rename must not expose any partial destination. */
 }
 free(r);free(w);
 puts("H28 five process-kill cutpoints and fresh-process A5/B3 recovery PASS");
 return 0;
}
