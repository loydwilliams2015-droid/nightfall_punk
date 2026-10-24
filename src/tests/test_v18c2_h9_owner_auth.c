#include "nf_v18c2_h9_auth.h"
#include <stdlib.h>
#include <stdio.h>
#define CK(q) do{if(!(q)){fprintf(stderr,"H9 FAIL line %d\n",__LINE__);return 1;}}while(0)
int main(void){
 NfWorld *w=calloc(1,sizeof *w);Nf18a6Runtime *r=calloc(1,sizeof *r);
 CK(w&&r);nf_world_init(w,42);
 CK(nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,(NfVec3){1,0,1})==10);
 Nf18a4Body object={0};object.id=2010;object.center=(NfVec3){2,1,1};
 object.box_half=(NfVec3){.2f,.5f,.35f};object.inverse_mass=1.f/24.f;
 CK(nf18a6_init(r,w,10,object,3,NF18A6_WORLD_BRIDGE));
 CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 CK(r->auth_state==NULL);
 CK(nf_c2_h9_owner_init(r));
 CK(!nf_c2_h9_owner_init(r));
 CK(nf_c2_h9_issue(w,r,NULL,NULL,17,(NfVec3){.1f,.1f,.1f})==NF_C2W_DENIED);
 CK(!nf_c2_h9_valid(w,r,17)&&!nf_c2_h9_consume(w,r,17));
 CK(!nf_c2_h9_valid(w,r,17));
 nf_c2_h9_revoke(r);
 CK(!nf_c2_h9_consume(w,r,17));
 CK(w->tick==0&&r->physical.world.tick==0);
 CK(nf18a6_unbind_world_tick(w,r));
 CK(r->auth_state==NULL);
 CK(!nf_c2_h9_consume(w,r,17));
 CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 CK(r->auth_state==NULL);
 CK(!nf_c2_h9_valid(w,r,17));
 CK(nf18a6_unbind_world_tick(w,r));
 CK(r->auth_state==NULL);
 puts("H9 negative issue, one-shot, replay and stale guards PASS (synthetic slot controls)");
 free(r);free(w);return 0;
}
