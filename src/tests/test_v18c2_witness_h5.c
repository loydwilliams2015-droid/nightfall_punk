#include "nf_v18c2_witness.h"
#include <stdio.h>
#include <stdlib.h>
static bool unavailable(void *ctx,const NfWorld *w,const Nf18a6Runtime *r,
 uint32_t action,Nf18b2Scene *s,Nf18b2Use *u){
 (void)ctx;(void)w;(void)r;(void)action;(void)s;(void)u;return false;
}
#define CK(x) do{if(!(x)){fprintf(stderr,"H5 FAIL %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
 NfWorld *w=calloc(1,sizeof *w);Nf18a6Runtime *r=calloc(1,sizeof *r);
 CK(w&&r);nf_world_init(w,42);
 CK(nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,(NfVec3){1,0,1})==10);
 Nf18a4Body crate={0};crate.id=2010;crate.center=(NfVec3){2,1,1};
 crate.inverse_mass=1.f/24.f;crate.box_half=(NfVec3){.2f,.5f,.35f};
 CK(nf18a6_init(r,w,10,crate,3,NF18A6_WORLD_BRIDGE));
 CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 uint8_t free_cells[NF18A5_CANON_VOXELS]={0};
 CK(nf18a5_load_canonical(&r->physical.world.grid,0,0,0,free_cells,1));
 NfVec3 point={.1f,.1f,.1f};
 uint32_t epoch=r->physical.world.grid.global_revision;
 /* Existing owner-bound MA03 returns INVALID when the provider fails.\n    Fail closed as DENIED, never reinterpret absent credentials as pending. */\n CK(nf_c2_witness(w,r,unavailable,NULL,5,point,epoch)==NF_C2W_DENIED);
 CK(nf_c2_witness(w,r,unavailable,NULL,5,point,epoch+1)==NF_C2W_STALE);
 free_cells[0]=NF18A5_MIXED;
 CK(nf18a5_load_canonical(&r->physical.world.grid,0,0,0,free_cells,1));
 epoch=r->physical.world.grid.global_revision;
 CK(nf_c2_witness(w,r,unavailable,NULL,5,point,epoch)==NF_C2W_PENDING);
 free_cells[0]=NF18A5_SOLID;
 CK(nf18a5_load_canonical(&r->physical.world.grid,0,0,0,free_cells,1));
 epoch=r->physical.world.grid.global_revision;
 CK(nf_c2_witness(w,r,unavailable,NULL,5,point,epoch)==NF_C2W_DENIED);
 CK(w->tick==0&&r->physical.world.tick==0);
 CK(nf18a6_unbind_world_tick(w,r));
 puts("H5 PASS stale, B2-provider pending, missing A5 fine pending, A5 solid denied");
 free(r);free(w);return 0;
}
