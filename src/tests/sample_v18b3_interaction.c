#define _POSIX_C_SOURCE 200809L
#include "v18b3_fixtures.h"
#include <math.h>
#include <stdio.h>
#include <time.h>
static uint32_t random32(uint32_t *s){*s^=*s<<13;*s^=*s>>17;*s^=*s<<5;return *s;}
static uint64_t now_ns(void){struct timespec t;if(clock_gettime(CLOCK_MONOTONIC,&t))abort();return (uint64_t)t.tv_sec*1000000000u+(uint64_t)t.tv_nsec;}
static bool deny(const NfWorld *w,const Nf18b3State *s,void *ctx){(void)w;(void)s;(void)ctx;return false;}
static void immediate_control(B3Fixture *f,Nf18b3Kind kind){
    /* Laboratory ablation: actual immediate state mutations without physical
       screening/motor realization. This callback is absent from production. */
    Nf18b3Object *o=&f->r->state.objects[0];f->r->state.action_count=1;f->r->state.active=UINT32_MAX;
    Nf18b3Action *a=&f->r->state.actions[0];*a=(Nf18b3Action){.status=NF18B3_COMPLETED,.kind=kind,.published=1,.material_effects=1,.action_id=1};
    if(kind==NF18B3_PICKUP){f->r->state.inventory_units+=o->units;o->units=0;o->available=0;}
    else if(kind==NF18B3_REPAIR){uint32_t units=o->max_health-o->health;o->health=o->max_health;f->r->state.salvage_units-=units;f->a->registry.accounts[0].balance-=units;}
    else if(kind==NF18B3_SWITCH){o->on=!o->on;f->r->state.power_enabled=o->on;}
    else o->body.center.x+=kind==NF18B3_DOOR_CLOSE?-o->travel:o->travel;
}
int main(int argc,char **argv){
    if(argc!=3)return 2;
    unsigned repeat=(unsigned)atoi(argv[1]);if(repeat<1||repeat>3)return 2;
    FILE *fp=fopen(argv[2],"w");if(!fp)return 2;B3Fixture f;if(!b3_allocate(&f))return 2;
    fputs("corpus,case,seed,variant,kind,model,expected_state,state,motor_steps,contact_receipts,material_effects,events,inventory_delta,health,salvage,tokens,grants,slots,frames,semantic_hash,correct,unsafe,conservation,proposal_ns,total_owner_ns,max_frame_ns,frame_overruns,geometry_queries,material_queries\n",fp);
    const char *names[]={"canonical","randomized","pathological","boundary","held-out"};
    const unsigned counts[]={48,256,96,96,256};unsigned errors=0,negative_errors=0,rows=0;
    uint32_t rng=UINT32_C(0xb318cafe);
    for(unsigned c=0;c<5;++c){if(c==4)rng=UINT32_C(0xb3eec019);
        for(unsigned i=0;i<counts[c];++i){
            const uint32_t seed=rng;
            unsigned variant=c==0?i/7:c==2?i%15:c==3?(i%2?14:0):i%2?1+random32(&rng)%14:0;
            Nf18b3Kind kind=(Nf18b3Kind)(c==1||c==4?random32(&rng)%7:i%7);
            if(c==3||variant==12||variant==14)kind=NF18B3_PICKUP;
            if(variant==9||variant==11)kind=NF18B3_DOOR_OPEN;
            if(variant==10)kind=NF18B3_CARRY;
            if(variant==13)kind=NF18B3_REPAIR;
            float x=1,z=1;unsigned far=32;
            if(c==1||c==4){x+=.04f*(float)(random32(&rng)%1000)/1000;z+=.04f*(float)(random32(&rng)%1000)/1000;far=random32(&rng)%65;}
            Nf18b3Status truth=NF18B3_COMPLETED;
            switch(variant){case 1:case 9:case 12:truth=NF18B3_BLOCKED;break;case 2:case 8:truth=NF18B3_PENDING;break;
            case 3:truth=NF18B3_EXPIRED;break;case 4:truth=NF18B3_CANCELLED;break;case 5:truth=NF18B3_PUBLISH_FAILED;break;
            case 6:case 10:case 14:truth=NF18B3_JAMMED;break;case 7:truth=NF18B3_UNSUPPORTED;break;
            case 11:truth=NF18B3_INVALID;break;case 13:truth=NF18B3_COMPENSATION_REQUIRED;break;default:break;}
            for(unsigned k=0;k<3;++k){const unsigned model=k==2?0:3+(k+i+repeat)%2;
                if(!b3_fixture(&f,model?model:4,kind,x,z,far))return 2;
                Nf18b3Object *o=&f.r->state.objects[0];const NfVec3 before=o->body.center;
                if(c==3||variant==14){
                    float hand=x+.8f;float center=hand+.25f;if(variant==14)center=nextafterf(center,INFINITY);
                    o->body.center.x=center;o->origin=o->body.center;
                    NfCollider *collider=&f.w->colliders[o->collider_index];
                    collider->min.x=center-o->body.box_half.x;collider->max.x=center+o->body.box_half.x;
                    f.a->registry.objects[0].object.target.x=center-o->body.box_half.x-.001f;
                }
                if(variant==1)nf18b2_set_contract(f.a,10,0,1);
                if(variant==2)f.p->physical.world.grid.chunks[0].loaded=0;
                if(variant==6)o->jammed=1;
                if(variant==7)o->rotated=1;
                if(variant==9)f.r->state.power_joules=0;
                if(variant==10){f.p->physical.object.inverse_mass=.01f;o->body.inverse_mass=.01f;}
                if(variant==11)o->max_force=NAN;
                if(variant==12)f.r->state.inventory_units=UINT32_MAX;
                const uint32_t inv_before=f.r->state.inventory_units;
                const double power_before=f.r->state.power_joules;
                const uint64_t start=now_ns();
                if(model)(void)nf18b3_request(f.r,10,f.target,1,kind);else immediate_control(&f,kind);
                const uint64_t proposal=now_ns()-start;
                if(model&&variant==3)nf18b2_set_contract(f.a,10,0,1);
                if(model&&variant==4)nf18b3_cancel(f.r,10,1,false);
                if(model&&variant==5)f.r->publication_validator=deny;
                if(model&&variant==8)f.r->state.event_count=NF18B3_MAX_EVENTS;
                const size_t events_before=f.r->state.event_count;
                uint64_t total=0,max_frame=0;unsigned frames=0,overruns=0;
                if(model)for(unsigned frame=0;frame<160&&f.r->state.active!=UINT32_MAX;++frame){
                    if(kind==NF18B3_CARRY&&!variant&&frame==24)nf18b3_cancel(f.r,10,1,true);
                    if(variant==13&&o->health)nf18b3_cancel(f.r,10,1,false);
                    const uint64_t begin=now_ns();(void)nf_world_step_checked(f.w,1.f/60);
                    const uint64_t ns=now_ns()-begin;total+=ns;if(ns>max_frame)max_frame=ns;overruns+=ns>16666667;++frames;
                }
                if(f.r->state.action_count!=1)return 2;
                const Nf18b3Action *a=&f.r->state.actions[0];
                const int64_t inv_delta=(int64_t)f.r->state.inventory_units-inv_before;
                const unsigned expected_health=variant==13?1:!variant&&kind==NF18B3_REPAIR?3:0;
                const unsigned expected_inventory=!variant&&kind==NF18B3_PICKUP?1:0;
                const bool conserve=fabs(f.r->state.power_joules+f.r->state.spent_joules-power_before)<1e-7&&
                    f.r->state.power_joules>=0&&f.r->state.salvage_units+o->health==8&&
                    f.a->registry.accounts[0].balance+o->health==10&&
                    (kind!=NF18B3_PICKUP||(uint64_t)o->units+f.r->state.inventory_units==(uint64_t)1+inv_before);
                bool realized=true;
                if(!variant){
                    if(kind==NF18B3_PICKUP)realized=!o->available&&o->collider_index==UINT32_MAX;
                    else if(kind==NF18B3_PUSH)realized=a->contact_receipts>0&&f.p->physical.world.history.raw_samples>0;
                    else if(kind==NF18B3_CARRY)realized=a->motor_receipts>0&&a->contact_receipts>0&&o->body.center.y>before.y&&!o->carried_by;
                    else realized=a->motor_receipts>0&&f.r->state.event_count>0;
                }
                const bool expected_no_effect=variant&&variant!=13;
                const bool effect=a->motor_receipts||a->contact_receipts||a->material_effects||inv_delta||o->health||
                    (kind!=NF18B3_PICKUP&&(o->body.center.x!=before.x||o->body.center.y!=before.y));
                const bool unsafe=expected_no_effect&&effect;
                const bool correct=a->status==truth&&inv_delta==expected_inventory&&o->health==expected_health&&
                    conserve&&realized&&!unsafe&&f.a->registry.objects[0].object.occupant_count==0&&f.r->state.active==UINT32_MAX;
                fprintf(fp,"%s,%u,%u,%u,%u,M%u,%s,%s,%u,%u,%u,%zu,%lld,%u,%u,%u,%zu,%u,%u,%u,%u,%u,%u,%llu,%llu,%llu,%u,%llu,%llu\n",
                    names[c],i,seed,variant,(unsigned)kind,model,nf18b3_status_name(truth),nf18b3_status_name(a->status),a->motor_receipts,
                    a->contact_receipts,a->material_effects,f.r->state.event_count-events_before,(long long)inv_delta,o->health,f.r->state.salvage_units,
                    f.a->registry.accounts[0].balance,f.a->grant_count,f.a->registry.objects[0].object.occupant_count,frames,nf18b3_semantic_hash(f.r),
                    correct,unsafe,conserve,(unsigned long long)proposal,(unsigned long long)total,(unsigned long long)max_frame,overruns,
                    (unsigned long long)f.r->geometry_queries,(unsigned long long)f.r->material_queries);
                ++rows;if(model)errors+=!correct;else negative_errors+=!correct;
            }
        }
    }
    int closed=fclose(fp);b3_free(&f);
    printf("rows=%u admissible_errors=%u negative_control_errors=%u repeat=%u\n",rows,errors,negative_errors,repeat);
    return closed?2:errors?1:0;
}
