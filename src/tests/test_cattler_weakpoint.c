#include "nf_cattler.h"
#include "nf_combat.h"
#include "nf_hitbox.h"
#include "nf_world.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

static NfVec3 unit_toward(NfVec3 from,NfVec3 to){
    NfVec3 d={to.x-from.x,to.y-from.y,to.z-from.z};float len=sqrtf(d.x*d.x+d.y*d.y+d.z*d.z);assert(len>0.001f);return(NfVec3){d.x/len,d.y/len,d.z/len};
}

static void classify_band_contract(void){
    const NfVec3 base={0.0f,0.05f,0.0f},origin={0.0f,1.65f,-9.0f};float distance=0.0f;
    NfVec3 foot={base.x,base.y+NF_CATTLER_FOOT_BAND_CENTER_Y,base.z};NfHitZone zone=nf_cattler_hit_zone_for_ray(origin,unit_toward(origin,foot),base,100.0f,&distance);assert(zone==NF_HIT_FOOT&&distance>0.0f);
    NfVec3 knee={base.x,base.y+NF_CATTLER_KNEE_BAND_CENTER_Y,base.z};zone=nf_cattler_hit_zone_for_ray(origin,unit_toward(origin,knee),base,100.0f,&distance);assert(zone==NF_HIT_KNEE&&distance>0.0f);
    NfVec3 body={base.x,base.y+1.45f,base.z};zone=nf_cattler_hit_zone_for_ray(origin,unit_toward(origin,body),base,100.0f,&distance);assert(zone==NF_HIT_BODY&&distance>0.0f);
}

static void authoritative_locomotor_chain(void){
    NfWorld world;nf_world_init(&world,20260808u);nf_world_build_movement_lab(&world);
    NfCattlerSystem cattlers;nf_cattler_init(&cattlers,&world,1u,NF_CATTLER_PROFILE_PACK,0xCA771Eu);assert(cattlers.count==1u);
    NfCattlerAgent *agent=&cattlers.agents[0];NfActor *target=nf_world_find_actor(&world,agent->actor_id);assert(target!=NULL&&target->combat.alive);
    const NfVec3 base=target->transform.position,origin={base.x,base.y+1.65f,base.z-9.0f},aim={base.x,base.y+NF_CATTLER_FOOT_BAND_CENTER_Y,base.z};float distance=0.0f;
    const NfHitZone zone=nf_cattler_hit_zone_for_ray(origin,unit_toward(origin,aim),base,100.0f,&distance);assert(zone==NF_HIT_FOOT&&distance>0.0f);
    const float health_before=target->health,locomotor_before=agent->locomotor_integrity;NfCombatEvent event={0};
    assert(nf_combat_apply_damage(target,999u,NF_WEAPON_CARBINE,zone,10.0f,world.tick,&event));nf_cattler_on_combat_event(&cattlers,&world,&event);
    assert(target->health<health_before);assert(agent->locomotor_integrity<locomotor_before);assert(agent->locomotor_integrity<1.0f);
}

int main(void){classify_band_contract();authoritative_locomotor_chain();puts("nightfall v1.0 Dream Cattler weak-point truth: PASS");return 0;}
