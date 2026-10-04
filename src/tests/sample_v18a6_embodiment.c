/* Reproducible adversarial embodiment comparison. The four models share inputs,
   but LAB_ONLY is intentionally single-step: it cannot advance NfWorld truth. */
#include "nf_embody18a6.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t step_rand(uint32_t *s){uint32_t x=*s; x^=x<<13; x^=x>>17; x^=x<<5;return *s=x;}
static float jitter(uint32_t *s,float amp){return ((step_rand(s)%10001u)/10000.0f-.5f)*amp;}
static Nf18a4Body crate_at(float x){Nf18a4Body b={0};b.id=2001;
    b.center=(NfVec3){x,.9f,1.0f};b.inverse_mass=1.f/24.f;
    b.inverse_inertia=(NfVec3){.3f,.3f,.3f};
    b.box_half=(NfVec3){.2f,.5f,.35f};return b;}
static int initialize(NfWorld *w,Nf18a6Runtime *r,unsigned model,unsigned scene,uint32_t seed){
    nf_world_init(w,seed);uint32_t rng=seed;
    nf_world_add_collider(w,NF_COLLIDER_SOLID,(NfVec3){0,-.5f,0},(NfVec3){4,0,4});
    const NfEntityId id=nf_world_spawn_actor_with_id(w,10,NF_FACTION_PLAYER,(NfVec3){1,0,1});
    if(id!=10)return 0;
    NfActor *a=nf_world_find_actor(w,id);
    a->movement.grounded=true;a->movement.mode=NF_MOVE_GROUND;
    Nf18a4Body ob=crate_at(1.84f+jitter(&rng,.10f));
    if(!nf18a6_init(r,w,id,ob,3,(Nf18a6Model)model))return 0;
    if(scene!=2){uint8_t free_cells[NF18A5_CANON_VOXELS]={0};
        if(!nf18a5_load_canonical(&r->physical.world.grid,0,0,0,free_cells,1))return 0;}
    if(scene==1){nf_world_add_collider(w,NF_COLLIDER_SOLID,
         (NfVec3){1.25f,0,.60f},(NfVec3){1.35f,2,1.4f});}
    if(scene==3){nf_world_add_ramp(w,(NfVec3){1,0,1},(NfVec3){2,1,2},NF_RAMP_POS_X);}
    if(scene==4){nf_world_spawn_actor_with_id(w,77,NF_FACTION_RIVAL,(NfVec3){1.75f,0,1});}
    if(scene==5){nf_world_add_moving_platform(w,(NfVec3){2,1,1},
        (NfVec3){3,1.3f,2},(NfVec3){0,1,0},.5f,2);}
    return 1;
}
int main(int argc,char **argv){
    if(argc!=2){fputs("usage: sample_v18a6_embodiment PATH.csv\n",stderr);return 2;}
    FILE *out=fopen(argv[1],"wb");if(!out)return 2;
    fputs("seed,scenario,model,attempted_ticks,committed_ticks,status,world_actor_moved,camera_updated,crate_speed,raw_contact_samples,material_epoch,world_revision,replay_hash\n",out);
    NfWorld *w=malloc(sizeof(*w));Nf18a6Runtime *r=malloc(sizeof(*r));
    if(!w||!r)return 2;
    for(unsigned scene=0;scene<6;++scene){
      for(unsigned k=0;k<200;++k){
        uint32_t seed=0x9e3779b9u^(k*0x85ebca6bu)^(scene*0xc2b2ae35u);
        for(unsigned model=0;model<4;++model){
          if(!initialize(w,r,model,scene,seed)){fputs("sample init failure\n",stderr);return 3;}
          Nf18a6Outcome step={0};unsigned successes=0,attempted=0,camera=0;
          NfMoveInput input={0};input.strafe=1;
          /* Controls share the same first tick. Multi-tick continuation is
             defined only for world-committed models. */
          const unsigned max_ticks=(model==NF18A6_LAB_ONLY)?1u:45u;
          for(unsigned t=0;t<max_ticks;++t){
            ++attempted;
            step=nf18a6_step(r,w,input,NULL,NULL,1.f/60.f,NULL);
            if(step.status!=NF18A6_COMMITTED)break;
            ++successes;camera+=(unsigned)step.camera_updated;
          }
          const NfActor *a=nf_world_find_actor_const(w,10);
          const int moved=a->transform.position.x>1.000001f;
          const float crate_speed=r->physical.object.velocity.x;
          fprintf(out,"%" PRIu32 ",%u,%u,%u,%u,%s,%d,%u,%.8f,%u,%u,%u,%" PRIu32 "\n",
           seed,scene,model,attempted,successes,nf18a6_status_name(step.status),moved,camera,
           crate_speed,r->physical.world.history.raw_samples,r->physical.world.grid.global_revision,
           r->physical.world.revision,r->last_hash);
        }
      }
    }
    free(w);free(r);fclose(out);return 0;
}
