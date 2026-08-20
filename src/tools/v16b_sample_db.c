#include "nf_prediction.h"
#include "nf_weapon_solution.h"
#include "nf_world.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SampleStats {
    unsigned shots;
    unsigned focus_ticks;
    float focus_sum;
    float focus_max;
    float cone_sum;
    float cone_max;
    float redirect_max;
    float support_max;
    float recoil_max;
    float speed_max;
} SampleStats;

static uint32_t lcg(uint32_t *state){*state=*state*1664525u+1013904223u;return *state;}
static float signed_unit(uint32_t *state){return((float)(lcg(state)&0xffffu)/32767.5f)-1.0f;}

static void scripted_input(
    const char *scenario,uint32_t tick,uint32_t *rng,NfMoveInput *move,NfCombatInput *combat){
    *move=(NfMoveInput){0};*combat=(NfCombatInput){0};
    if(strcmp(scenario,"calm")==0){combat->fire_pressed=(tick%60u)==30u;}
    else if(strcmp(scenario,"focus_hold")==0){combat->focus_held=tick>=45u&&tick<270u;combat->fire_pressed=(tick%48u)==24u;}
    else if(strcmp(scenario,"reload_focus")==0){combat->focus_held=tick>=75u&&tick<260u;combat->fire_pressed=tick<72u&&(tick%12u)==6u;combat->reload_pressed=tick==90u;}
    else if(strcmp(scenario,"sprint_focus")==0){move->forward=1.0f;move->sprint_held=tick<180u;combat->focus_held=tick>=72u&&tick<240u;combat->fire_pressed=(tick%42u)==18u;}
    else if(strcmp(scenario,"redirect_focus")==0){move->forward=0.75f;move->strafe=((tick/45u)%2u)==0u?0.75f:-0.75f;move->yaw_radians=((float)((tick/60u)%4u))*1.5707963f;combat->focus_held=(tick%120u)>=36u&&(tick%120u)<78u;combat->fire_pressed=(tick%40u)==20u;}
    else if(strcmp(scenario,"pathological_redirect")==0){move->forward=((tick/8u)%2u)==0u?1.0f:-1.0f;move->strafe=((tick/5u)%2u)==0u?1.0f:-1.0f;move->yaw_radians=((float)((tick/6u)%8u))*0.7853982f;combat->focus_held=(tick%24u)<8u;combat->fire_pressed=(tick%18u)==9u;}
    else if(strcmp(scenario,"random_control")==0){move->forward=signed_unit(rng);move->strafe=signed_unit(rng);move->yaw_radians=signed_unit(rng)*3.1415926f;move->sprint_held=(lcg(rng)&3u)==0u;combat->focus_held=(lcg(rng)&1u)!=0u;combat->fire_pressed=(lcg(rng)%17u)==0u;combat->reload_pressed=(lcg(rng)%211u)==0u;}
}

static const char *stratum_for(const char *scenario){
    if(strcmp(scenario,"calm")==0||strcmp(scenario,"focus_hold")==0)return"baseline";
    if(strcmp(scenario,"redirect_focus")==0||strcmp(scenario,"sprint_focus")==0)return"median";
    if(strcmp(scenario,"reload_focus")==0)return"transition";
    if(strcmp(scenario,"pathological_redirect")==0)return"pathological";
    return"random_control";
}

static SampleStats run_one(const char *scenario,uint32_t seed,unsigned ticks){
    NfWorld world;nf_world_init(&world,seed);nf_world_add_collider(&world,NF_COLLIDER_SOLID,(NfVec3){-40,-0.5f,-40},(NfVec3){40,0,40});
    const NfEntityId id=nf_world_spawn_actor_with_id(&world,7u,NF_FACTION_PLAYER,(NfVec3){0,0.05f,0});NfActor *actor=nf_world_find_actor(&world,id);actor->movement.grounded=true;actor->movement.mode=NF_MOVE_GROUND;
    NfPrediction prediction;nf_prediction_init(&prediction);SampleStats stats={0};uint32_t rng=seed^0xB16B2026u;
    for(unsigned t=0;t<ticks;++t){
        NfMoveInput move;NfCombatInput combat;scripted_input(scenario,t,&rng,&move,&combat);NfInputCommand cmd=nf_prediction_make_command(&prediction,world.tick,move,combat);const uint32_t before=actor->weapon_authority.accepted_shot_sequence;nf_prediction_simulate_command(&world,id,&cmd);actor=nf_world_find_actor(&world,id);nf_prediction_record(&prediction,&cmd,actor);const NfWeaponEnvelope e=nf_weapon_authority_envelope(actor,move.yaw_radians,combat.aim_pitch_radians);const float speed=sqrtf(actor->transform.velocity.x*actor->transform.velocity.x+actor->transform.velocity.z*actor->transform.velocity.z);if(actor->weapon_authority.accepted_shot_sequence>before)++stats.shots;if(actor->weapon_authority.focus_amount>0.05f)++stats.focus_ticks;stats.focus_sum+=actor->weapon_authority.focus_amount;if(actor->weapon_authority.focus_amount>stats.focus_max)stats.focus_max=actor->weapon_authority.focus_amount;stats.cone_sum+=e.total_cone_deg;if(e.total_cone_deg>stats.cone_max)stats.cone_max=e.total_cone_deg;if(e.redirect_cone_deg>stats.redirect_max)stats.redirect_max=e.redirect_cone_deg;if(e.support_cone_deg>stats.support_max)stats.support_max=e.support_cone_deg;if(e.recoil_cone_deg>stats.recoil_max)stats.recoil_max=e.recoil_cone_deg;if(speed>stats.speed_max)stats.speed_max=speed;
    }
    return stats;
}

int main(int argc,char **argv){
    const char *path=NULL;unsigned ticks=360u;if(argc>1)path=argv[1];if(argc>2){long v=strtol(argv[2],NULL,10);if(v>30)ticks=(unsigned)v;}
    FILE *out=path?fopen(path,"w"):stdout;if(out==NULL){perror("v16b sample database");return 2;}
    static const char *scenarios[]={"calm","focus_hold","reload_focus","sprint_focus","redirect_focus","pathological_redirect","random_control"};
    static const uint32_t seeds[]={2026081901u,2026081902u,2026081903u};
    fprintf(out,"sample_id,scenario,stratum,seed,repeat,ticks,shots,focus_tick_share,focus_mean,focus_max,cone_mean_deg,cone_max_deg,redirect_max_deg,support_max_deg,recoil_max_deg,speed_max_mps\n");
    unsigned sample_id=0;
    for(size_t s=0;s<sizeof(scenarios)/sizeof(scenarios[0]);++s){for(size_t k=0;k<sizeof(seeds)/sizeof(seeds[0]);++k){for(unsigned repeat=0;repeat<2u;++repeat){SampleStats st=run_one(scenarios[s],seeds[k],ticks);fprintf(out,"%u,%s,%s,%u,%u,%u,%u,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n",++sample_id,scenarios[s],stratum_for(scenarios[s]),seeds[k],repeat,ticks,st.shots,(float)st.focus_ticks/(float)ticks,st.focus_sum/(float)ticks,st.focus_max,st.cone_sum/(float)ticks,st.cone_max,st.redirect_max,st.support_max,st.recoil_max,st.speed_max);}}
    }
    if(path)fclose(out);return 0;
}
