#include "nf_weapon_geometry.h"

#include <float.h>
#include <math.h>
#include <stddef.h>

static bool ray_aabb(
    NfVec3 origin,NfVec3 direction,NfVec3 minimum,NfVec3 maximum,float *distance) {
    float tmin=0.0f,tmax=FLT_MAX;
    const float ov[3]={origin.x,origin.y,origin.z};
    const float dv[3]={direction.x,direction.y,direction.z};
    const float av[3]={minimum.x,minimum.y,minimum.z};
    const float bv[3]={maximum.x,maximum.y,maximum.z};
    for(int i=0;i<3;++i){
        if(fabsf(dv[i])<1e-6f){
            if(ov[i]<av[i]||ov[i]>bv[i])return false;
            continue;
        }
        float t1=(av[i]-ov[i])/dv[i],t2=(bv[i]-ov[i])/dv[i];
        if(t1>t2){float tmp=t1;t1=t2;t2=tmp;}
        if(t1>tmin)tmin=t1;
        if(t2<tmax)tmax=t2;
        if(tmin>tmax)return false;
    }
    if(tmax<0.0f)return false;
    if(distance!=NULL)*distance=tmin>=0.0f?tmin:tmax;
    return true;
}

NfVec3 nf_weapon_muzzle_origin(const NfActor *actor,float yaw_radians) {
    if(actor==NULL)return(NfVec3){0};
    const float sy=sinf(yaw_radians),cy=cosf(yaw_radians);
    const NfVec3 forward={sy,0.0f,cy};
    const NfVec3 right={cy,0.0f,-sy};
    const float vertical=actor->movement.eye_height-0.22f;
    return(NfVec3){
        actor->transform.position.x+forward.x*0.44f+right.x*0.12f,
        actor->transform.position.y+vertical,
        actor->transform.position.z+forward.z*0.44f+right.z*0.12f
    };
}

bool nf_weapon_world_first_hit(
    const NfWorld *world,NfVec3 origin,NfVec3 direction,float range,NfWorldRayHit *out) {
    if(out!=NULL)*out=(NfWorldRayHit){.collider_index=-1,.distance=range};
    if(world==NULL||range<=0.0f)return false;
    float nearest=range;
    int index=-1;
    for(size_t i=0;i<world->collider_count;++i){
        const NfCollider *collider=&world->colliders[i];
        if(collider->kind==NF_COLLIDER_LADDER)continue;
        float distance=0.0f;
        if(ray_aabb(origin,direction,collider->min,collider->max,&distance)&&
           distance>0.005f&&distance<nearest){
            nearest=distance;
            index=(int)i;
        }
    }
    if(index<0)return false;
    if(out!=NULL){
        out->hit=true;
        out->collider_index=index;
        out->distance=nearest;
        out->point=(NfVec3){
            origin.x+direction.x*nearest,
            origin.y+direction.y*nearest,
            origin.z+direction.z*nearest};
    }
    return true;
}
