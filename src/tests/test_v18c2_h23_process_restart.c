#define _POSIX_C_SOURCE 200809L
#include "nf_c2_h22_envelope.h"
#include "nf_embody18a6.h"
#include "nf_b6_push18b6.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"H23 failure line %d\n",__LINE__);return 1;}}while(0)
int main(void){
 Nf18a5Integrated *physical=calloc(1,sizeof *physical);
 Nf18a5Integrated *restored=calloc(1,sizeof *restored);
 CK(physical&&restored);
 NfWorld *world=calloc(1,sizeof *world);
 Nf18a6Runtime *runtime=calloc(1,sizeof *runtime);
 CK(world&&runtime);
 nf_world_init(world,42);
 CK(nf_world_spawn_actor_with_id(world,10,NF_FACTION_PLAYER,(NfVec3){1,0,1})==10);
 Nf18a4Body crate={0};crate.id=2010;crate.center=(NfVec3){2,1,1};
 crate.inverse_mass=1.f/24.f;crate.box_half=(NfVec3){.2f,.5f,.35f};
 CK(nf18a6_init(runtime,world,10,crate,3,NF18A6_WORLD_BRIDGE));
 *physical=runtime->physical;
 physical->world.tick=1;physical->committed_receipts=1;
 physical->world.history.raw_samples=1;
 NfC2H22Receipt r={.tick=1,.actor=10,.action=91,
  .a4_receipts=1,.a5_samples=1,.b3_receipts=1};
 r.b3_event=(Nf18b3Event){.actor=10,.object=2010,.action=91,
  .tick=1,.world_epoch=0,.material_epoch=0,.contract_epoch=7,
  .kind=NF18B3_PUSH,.phase=NF18B3_APPLIED,.authorized=1,.applied=1};
 char path[160];snprintf(path,sizeof path,"/tmp/nightfall_h23_%ld.bin",(long)getpid());
 remove(path);Nf18b3Event event={0};
 CK(!nf_c2_h23_restore_full(path,restored,&event));
 /* Restart from a separate child process: only persisted bytes survive. */
 pid_t child=fork();CK(child>=0);
 if(child==0)_exit(nf_c2_h22_save(path,physical,r)?0:9);
 int status=0;CK(waitpid(child,&status,0)==child&&WIFEXITED(status)&&WEXITSTATUS(status)==0);
 CK(nf_c2_h23_restore_full(path,restored,&event));
 CK(event.action==91&&event.contract_epoch==7&&restored->committed_receipts==1);
 Nf18b6Push *push=calloc(1,sizeof *push);CK(push!=NULL);
 /* Recovery begins from the preceding uncommitted physical frame. */
 runtime->physical.committed_receipts=0;
 runtime->physical.world.history.raw_samples=0;
 CK(nf18b6_push_attach(push,runtime,world,7));
 CK(nf18b6_push_h26_recover(world,runtime,push,path));
 CK(world->tick==1&&runtime->last_commit_tick==1);
 CK(runtime->last_hash==0&&runtime->tick_trace.tick==0);
 CK(push->receipt_count==1&&push->event.action==91&&
    push->event.phase==NF18B3_APPLIED&&push->last_result==NF18B3_OK);
 CK(!nf18b6_push_h26_recover(world,runtime,push,path)); /* replay */
 CK(nf18a6_bind_world_tick(world,runtime,NULL,NULL,NULL));
 CK(!nf18b6_push_h26_recover(world,runtime,push,path)); /* active owner */
 CK(nf18a6_unbind_world_tick(world,runtime));
 free(push);
 /* A crash before publication is denied, not fabricated. */
 CK(remove(path)==0);
 child=fork();CK(child>=0);
 if(child==0)_exit(0);
 CK(waitpid(child,&status,0)==child&&WIFEXITED(status));
 CK(!nf_c2_h23_restore_full(path,restored,&event));
 free(runtime);free(world);free(restored);free(physical);
 puts("H23 child restart B3/A5 envelope complete and missing refusal PASS");return 0;
}
