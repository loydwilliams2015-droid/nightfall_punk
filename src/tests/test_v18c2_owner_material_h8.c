/* H8 source-exact regression: A6-owned grid material witness.
 * Does not assert positive B2 credentials or allow world publication. */
#include "nf_v18c2_witness.h"
#include <stdio.h>
#include <stdlib.h>
#define CK(c) do{if(!(c)){fprintf(stderr,"H8 FAIL line %d\n",__LINE__);return 1;}}while(0)
int main(void){
 NfWorld *w=calloc(1,sizeof *w);
 Nf18a6Runtime *r=calloc(1,sizeof *r);
 CK(w&&r);
 nf_world_init(w,42);
 CK(nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,(NfVec3){1,0,1})==10);
 Nf18a4Body crate={0};
 crate.id=2010;crate.center=(NfVec3){2,1,1};
 crate.inverse_mass=1.f/24.f;crate.box_half=(NfVec3){.2f,.5f,.35f};
 CK(nf18a6_init(r,w,10,crate,3,NF18A6_WORLD_BRIDGE));
 CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 uint8_t voxels[NF18A5_CANON_VOXELS]={0};
 CK(nf18a5_load_canonical(&r->physical.world.grid,0,0,0,voxels,1));
 uint32_t epoch=r->physical.world.grid.global_revision;
 NfVec3 pos={.1f,.1f,.1f};
 /* NULL server issuer must remain denied even with A5 FREE. */
 CK(nf_c2_witness(w,r,NULL,NULL,17,pos,epoch)==NF_C2W_STALE);
 /* Wrong epoch is always stale. */
 CK(nf_c2_witness(w,r,NULL,NULL,17,pos,epoch+1u)==NF_C2W_STALE);
 voxels[0]=NF18A5_MIXED;
 CK(nf18a5_load_canonical(&r->physical.world.grid,0,0,0,voxels,2));
 CK(nf18a5_query(&r->physical.world.grid,.1f,.1f,.1f,2,true)==NF18A5_Q_PENDING);
 CK(w->tick==0&&r->physical.world.tick==0);
 CK(nf18a6_unbind_world_tick(w,r));
 puts("H8 PASS A6 source frame, missing issuer, stale epoch, pending fine; no publication");
 free(r);free(w);return 0;
}
