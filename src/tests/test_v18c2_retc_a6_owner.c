#include "nf_embody18a6.h"
#include "nf_c2_retc_frame.h"
#include <stdio.h>
#include <stdlib.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"RET-C A6 gate failed line %d\n",__LINE__);return 1;}}while(0)
int main(void){
 NfWorld *w=calloc(1,sizeof *w);
 Nf18a6Runtime *r=calloc(1,sizeof *r);
 NfRetcPublisher *pub=calloc(1,sizeof *pub);
 CK(w&&r&&pub);
 nf_world_init(w,42);
 nf_world_add_collider(w,NF_COLLIDER_SOLID,(NfVec3){0,-.5f,0},
  (NfVec3){4,0,4});
 CK(nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,
  (NfVec3){1,0,1})==10);
 Nf18a4Body obj={0};obj.id=2001;obj.center=(NfVec3){2,.9f,1};
 obj.inverse_mass=1.f/24.f;obj.box_half=(NfVec3){.2f,.5f,.35f};
 CK(nf18a6_init(r,w,10,obj,3,NF18A6_WORLD_BRIDGE));
 const uint32_t initial=(uint32_t)w->tick;
 CK(nf18a6_retc_bind(r,pub,"/tmp/nightfall_retc_no_commit_test"));
 CK(!nf18a6_retc_bind(r,pub,"/tmp/nightfall_retc_no_commit_test"));
 NfMoveInput move={0};move.strafe=1;
 Nf18a6Outcome result=nf18a6_step(r,w,move,NULL,NULL,1.f/60.f,NULL);
 CK(result.status==NF18A6_UNSUPPORTED);
 CK(w->tick==initial&&r->last_commit_tick==initial);
 CK(nf_retc_read(pub)==NULL);
 CK(!r->joint_commit_epoch);
 free(pub);free(r);free(w);
 puts("RET-C live A6 refuses fallback physical publication without joint commit PASS");
 return 0;
}
