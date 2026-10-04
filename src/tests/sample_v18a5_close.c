#include "nf_contact18a5_close.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static uint32_t rng;
static uint32_t next_u32(void){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
static float variation(void){return ((float)(next_u32()%101u)-50.0f)*0.0002f;}
static Nf18a4Body body(uint32_t id,float mass,NfVec3 center){
    Nf18a4Body b={0};b.id=id;b.center=center;
    b.inverse_mass=mass>0?1.0f/mass:0;
    if(mass>0)b.inverse_inertia=(NfVec3){1,1,1};
    b.radius=.3f;b.height=1.8f;b.box_half=(NfVec3){.5f,.9f,.8f};return b;
}
static bool load(void *context,int32_t x,int32_t y,int32_t z,uint32_t epoch,
                 uint8_t *scale,uint8_t *voxels,size_t *count){
    (void)epoch;
    int mode=*(int*)context;
    if(!mode||x||y||z)return false;
    *scale=2;*count=512;memset(voxels,0,512);
    voxels[mode==4?19:27]=1;
    if(mode==3)*count=1024; /* malformed payload; must remain pending */
    return true;
}
int main(int argc,char **argv){
    if(argc!=2)return 2;
    FILE *f=fopen(argv[1],"w");if(!f)return 3;
    fputs("case,scenario,point_status,integrated_status,cache_loads,world_commit,actual_crate_impulse,material_revision_changed\n",f);
    const Nf18a2ShapePolicy shape={.3f,1.8f,.1f,.4f,.7f};
    const Nf18a4Motor motor={{120,0,0},200000,5000};
    for(unsigned scenario=0;scenario<7;++scenario)for(unsigned seed=0;seed<200;++seed){
        rng=0xa5b30da5u ^ (seed*0x9e3779b9u) ^ (scenario*0x6d2b79f5u);
        float jitter=variation();
        Nf18a5Integrated w;
        nf18a5_integrated_init(&w,2,body(1,80,(NfVec3){1+jitter,.9f,1+jitter}),
                                  body(2,40,(NfVec3){2.2f+jitter,.9f,1+jitter}));
        uint8_t cells[NF18A5_CANON_VOXELS]={0};cells[5]=scenario==4?NF18A5_SOLID:NF18A5_MIXED;
        if(!nf18a5_load_canonical(&w.world.grid,0,0,0,cells,1))return 4;
        int mode=(scenario==0?0:scenario==2?4:scenario==3?3:1);
        NfVec3 probe=(scenario==4||scenario==5)?(NfVec3){2.1f,.1f,1.1f}:
                         (NfVec3){1.1f,.1f,1.1f};
        if(scenario==6){
            uint8_t voxels[NF18A5_FINE_MAX_VOXELS]={0},scale=0;size_t count=0;
            if(!load(&mode,0,0,0,1,&scale,voxels,&count)||
               !nf18a5_load_fine(&w.world.grid,0,0,0,scale,voxels,count,1,1))return 5;
        }
        Nf18a5Grid baseline=w.world.grid;
        uint32_t loads=0;
        Nf18a5Query point=nf18a5_resolve_exact(&baseline,probe,17,scenario==0?NULL:load,
                                               &mode,&loads);
        uint32_t material_rev=w.world.grid.global_revision;
        Nf18a5CloseResult result=nf18a5_integrated_pair_step(&w,
           scenario==5?2u:1u,17,probe,scenario==0?NULL:load,&mode,shape,
           motor,1.0f/60,0,0,NULL);
        fprintf(f,"%u,%u,%u,%u,%u,%u,%u,%u\n",seed,scenario,(unsigned)point,
                (unsigned)result.status,result.cache_loads,
                w.world.revision>1u?1u:0u,w.object.velocity.x>0?1u:0u,
                w.world.grid.global_revision!=material_rev?1u:0u);
    }
    fclose(f);return 0;
}
