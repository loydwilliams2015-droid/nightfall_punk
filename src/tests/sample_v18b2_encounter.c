#define _POSIX_C_SOURCE 200809L
/* Independent constructed encounter outcomes; whole production A6 owner time. */
#include "nf_encounter18b2.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static uint32_t random32(uint32_t *s){*s^=*s<<13;*s^=*s>>17;*s^=*s<<5;return *s;}
static uint64_t now_ns(void){struct timespec t;if(clock_gettime(CLOCK_MONOTONIC,&t))abort();
    return (uint64_t)t.tv_sec*UINT64_C(1000000000)+(uint64_t)t.tv_nsec;}
static bool deny(const Nf18a5Integrated *p,void *ctx){(void)p;(void)ctx;return false;}
typedef struct Fixture {NfWorld *w;Nf18a6Runtime *a;Nf18b2EncounterRuntime *r;} Fixture;
static bool setup(Fixture f,unsigned model,float x,float z,unsigned variant,bool boundary){
    nf_world_init(f.w,42);
    nf_world_add_collider(f.w,NF_COLLIDER_SOLID,(NfVec3){0,-.5f,0},(NfVec3){4,0,4});
    if(!nf_world_spawn_actor_with_id(f.w,10,NF_FACTION_PLAYER,(NfVec3){x,0,z}))return false;
    NfActor *actor=nf_world_find_actor(f.w,10);actor->movement.grounded=true;actor->movement.mode=NF_MOVE_GROUND;
    Nf18a4Body crate={.id=2010,.center={2.2f,.9f,z},.inverse_mass=1.f/24,
        .inverse_inertia={.3f,.3f,.3f},.box_half={.2f,.5f,.35f}};
    if(!nf18a6_init(f.a,f.w,10,crate,3,NF18A6_WORLD_CAMERA))return false;
    uint8_t cells[NF18A5_CANON_VOXELS]={0};
    if(!nf18a5_load_canonical(&f.a->physical.world.grid,0,0,0,cells,1)||
       !nf18a6_bind_world_tick(f.w,f.a,NULL,NULL,NULL)||
       !nf18b2_encounter_init(f.r,f.w,f.a,(Nf18b2Model)model)||
       !nf18b2_register_actor(f.r,10,variant==1?0:1,variant==9?0:1,10))return false;
    Nf18b2Object object={.id=2010,.revision=1,.cell_id=1,.nexus_id=2,.slot_id=3,
        .target={1.999f,actor->movement.eye_height,z},.max_reach=3,
        .required_credentials=1,.required_inventory=1,.capacity=1,
        .preconditions=variant==10?0:1,.requires_support=1};
    if(boundary)object.max_reach=variant?nextafterf(object.target.x-x,0):object.target.x-x;
    if(!nf18b2_register_object(f.r,object,f.a->physical.world.grid.global_revision,1,1)||!nf18b2_attach(f.r))return false;
    if(!boundary){
        if(variant==2)f.a->physical.world.grid.chunks[0].loaded=0;
        if(variant==8){f.w->colliders[0].kind=NF_COLLIDER_MOVING_PLATFORM;f.w->colliders[0].velocity.x=1;}
        if(variant==11)f.a->physical.world.grid.chunks[0].canonical[(1*4+1)*4+1]=NF18A5_SOLID;
        if(variant==12)actor->movement.body_height=NAN;
        if(variant==13)actor->transform.position.x=INFINITY;
    }
    return true;
}
int main(int argc,char **argv){
    if(argc!=3){fprintf(stderr,"usage: %s repeat output.csv\n",argv[0]);return 2;}
    const unsigned repeat=(unsigned)atoi(argv[1]);if(repeat<1||repeat>3)return 2;
    FILE *fp=fopen(argv[2],"w");if(!fp)return 2;
    Fixture f={malloc(sizeof(NfWorld)),malloc(sizeof(Nf18a6Runtime)),malloc(sizeof(Nf18b2EncounterRuntime))};
    if(!f.w||!f.a||!f.r)return 2;
    fputs("corpus,case,seed,variant,model,expected_state,state,expected_grant,grant,balance,inventory,slots,receipts,history,world_tick,correct,unsafe,proposal_ns,tick_ns,total_ns,frame_overrun\n",fp);
    const char *names[]={"canonical","randomized","pathological","boundary","held-out"};
    const unsigned counts[]={32,256,64,64,256};unsigned errors=0,tested=0;
    uint32_t rng=UINT32_C(0xa6b218b2);
    for(unsigned c=0;c<5;++c){
        if(c==4)rng=UINT32_C(0xeec018b2);
        for(unsigned i=0;i<counts[c];++i){
            const uint32_t seed=rng;const unsigned variant=c==3?i%2:c==1||c==4?random32(&rng)%12:i%(c==2?14:12);
            float x=1,z=1;
            if(c==1||c==4){x+=.04f*(float)(random32(&rng)%1000)/1000;z+=.04f*(float)(random32(&rng)%1000)/1000;}
            Nf18b2EncounterState truth=NF18B2_EXECUTING;unsigned grant=1;
            if(c==3){if(variant){truth=NF18B2_ENCOUNTER_BLOCKED;grant=0;}}
            else if(variant){grant=0;
                switch(variant){
                case 2:case 6:case 8:truth=NF18B2_ENCOUNTER_PENDING;break;
                case 3:case 7:truth=NF18B2_EXPIRED;break;
                case 4:truth=NF18B2_CANCELLED;break;
                case 5:truth=NF18B2_FAILED_NO_EFFECT;break;
                default:truth=NF18B2_ENCOUNTER_BLOCKED;break;
                }
            }
            for(unsigned k=0;k<2;++k){
                const unsigned model=3+(k+i+repeat)%2;
                if(!setup(f,model,x,z,variant,c==3))return 2;
                const uint64_t start=now_ns();
                (void)nf18b2_propose(f.r,(Nf18b2Intent){10,2010,1,NF18B2_PUSH});
                const uint64_t assessed=now_ns();
                if(c!=3)switch(variant){
                case 3:if(!nf18b2_set_contract(f.r,10,0,1))return 2;break;
                case 4:if(!nf18b2_withdraw(f.r,10,1))return 2;break;
                case 5:f.r->publication_validator=deny;break;
                case 6:f.r->grant_count=64;break;
                case 7:if(!nf18b2_touch_object(f.r,2010))return 2;break;
                default:break;
                }
                const uint64_t tick_start=now_ns();
                (void)nf_world_step_checked(f.w,1.f/60);
                const uint64_t end=now_ns();
                if(f.r->trace_count!=1)return 2;
                const Nf18b2EncounterTrace *t=&f.r->traces[0];
                const unsigned balance=f.r->registry.accounts[0].balance;
                const unsigned inventory=f.r->registry.accounts[0].inventory;
                const unsigned slots=f.r->registry.objects[0].object.occupant_count;
                const unsigned expected_inventory=c!=3&&variant==9?0:1-grant;
                const unsigned receipts=f.a->physical.committed_receipts,history=f.a->physical.world.history.raw_samples;
                const bool correct=t->state==truth&&t->published==grant&&balance==10-grant&&
                    inventory==expected_inventory&&slots==grant&&receipts==0&&history==0&&
                    f.w->tick==grant&&f.a->physical.world.tick==grant;
                const uint64_t total=assessed-start+end-tick_start;
                fprintf(fp,"%s,%u,%u,%u,M%u,%s,%s,%u,%u,%u,%u,%u,%u,%u,%llu,%u,%u,%llu,%llu,%llu,%u\n",
                    names[c],i,seed,variant,model,nf18b2_encounter_state_name(truth),nf18b2_encounter_state_name(t->state),
                    grant,t->published,balance,inventory,slots,receipts,history,(unsigned long long)f.w->tick,correct,t->published&&!grant,
                    (unsigned long long)(assessed-start),(unsigned long long)(end-tick_start),
                    (unsigned long long)total,total>UINT64_C(16666667));
                ++tested;errors+=!correct;
            }
        }
    }
    const int closed=fclose(fp);free(f.r);free(f.a);free(f.w);
    printf("indexed_evaluations=%u authored_errors=%u repeat=%u\n",tested,errors,repeat);
    return closed?2:errors?1:0;
}
