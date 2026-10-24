#include "nf_v18c2_a6_stage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CK(c) do {if(!(c)){fprintf(stderr,"H4 FAIL line %d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(void){
 NfWorld *w=calloc(1,sizeof *w);
 Nf18a6Runtime *r=calloc(1,sizeof *r);
 NfMulti18c1World *island=calloc(1,sizeof *island);
 Nf18c2A6Stage *out=calloc(1,sizeof *out);
 CK(w&&r&&island&&out);
 nf_world_init(w,42);
 CK(nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,(NfVec3){1,0,1})==10);
 Nf18a4Body crate={0};
 crate.id=2010;crate.center=(NfVec3){2,1,1};
 crate.inverse_mass=1.f/24.f;crate.box_half=(NfVec3){.2f,.5f,.35f};
 CK(nf18a6_init(r,w,10,crate,3,NF18A6_WORLD_BRIDGE));
 CK(nf18a6_bind_world_tick(w,r,NULL,NULL,NULL));
 island->count=2;island->tick=(uint32_t)w->tick;
 island->revision=r->physical.world.revision;
 island->bodies[0]=r->physical.actor;
 island->bodies[1]=r->physical.object;
 NfMulti18c1Proposal p={0};
 p.actor=10;p.action=17;p.revision=island->revision;
 p.motor.target_velocity=(NfVec3){0,0,0};
 p.motor.max_force=100;p.motor.max_accel=10;
 Nf18c2StageStatus s=nf18c2_a6_stage(w,r,island,&p,1,NULL,0,
    1.f/60.f,17,NF18B3_PUSH,true,out);
 CK(s==NF18C2_STAGE_READY);
 CK(out->actor==10&&out->source_tick==w->tick);
 CK(out->physical.successor.tick==island->tick+1);
 CK(w->tick==0&&r->physical.world.tick==0);
 CK(nf18c2_a6_stage(w,r,island,&p,1,NULL,0,1.f/60.f,
    18,NF18B3_PUSH,false,out)==NF18C2_STAGE_DENIED);
 CK(nf18c2_a6_stage(w,r,island,&p,1,NULL,0,1.f/60.f,
    18,NF18B3_CARRY,true,out)==NF18C2_STAGE_PENDING);
 ++island->revision;
 CK(nf18c2_a6_stage(w,r,island,&p,1,NULL,0,1.f/60.f,
    18,NF18B3_PUSH,true,out)==NF18C2_STAGE_STALE);
 CK(w->tick==0&&r->physical.world.tick==0);
 CK(nf18a6_unbind_world_tick(w,r));
 puts("H4 PASS owner-bound positive staging / denied / pending / stale; no world publish");
 free(out);free(island);free(r);free(w);return 0;
}
