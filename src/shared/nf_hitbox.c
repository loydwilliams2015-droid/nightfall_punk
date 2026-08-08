#include "nf_hitbox.h"

#include <float.h>
#include <math.h>

static NfVec3 vsub(NfVec3 a,NfVec3 b){return(NfVec3){a.x-b.x,a.y-b.y,a.z-b.z};}
static float vdot(NfVec3 a,NfVec3 b){return a.x*b.x+a.y*b.y+a.z*b.z;}

static bool ray_aabb(
    NfVec3 origin,NfVec3 direction,NfVec3 minimum,NfVec3 maximum,float *distance){
    float tmin=0.0f,tmax=FLT_MAX;
    const float ov[3]={origin.x,origin.y,origin.z};
    const float dv[3]={direction.x,direction.y,direction.z};
    const float mn[3]={minimum.x,minimum.y,minimum.z};
    const float mx[3]={maximum.x,maximum.y,maximum.z};
    for(int i=0;i<3;++i){
        if(fabsf(dv[i])<1e-6f){if(ov[i]<mn[i]||ov[i]>mx[i])return false;continue;}
        float t1=(mn[i]-ov[i])/dv[i],t2=(mx[i]-ov[i])/dv[i];
        if(t1>t2){float tmp=t1;t1=t2;t2=tmp;}
        if(t1>tmin)tmin=t1;
        if(t2<tmax)tmax=t2;
        if(tmin>tmax)return false;
    }
    if(tmax<0.0f)return false;
    if(distance!=NULL)*distance=tmin>=0.0f?tmin:tmax;
    return true;
}

static bool ray_sphere(
    NfVec3 origin,NfVec3 direction,NfVec3 center,float radius,float *distance){
    NfVec3 oc=vsub(origin,center);
    float b=vdot(oc,direction),c=vdot(oc,oc)-radius*radius,disc=b*b-c;
    if(disc<0.0f)return false;
    float root=sqrtf(disc),t=-b-root;
    if(t<0.0f)t=-b+root;
    if(t<0.0f)return false;
    if(distance!=NULL)*distance=t;
    return true;
}

static void consider(
    bool hit,float distance,NfHitZone zone,float *nearest,NfHitZone *best){
    if(hit&&distance>=0.0f&&distance<*nearest){*nearest=distance;*best=zone;}
}

NfHitZone nf_cattler_hit_zone_for_ray(
    NfVec3 origin,NfVec3 direction,NfVec3 actor_base,
    float max_distance,float *distance_out){
    float nearest=max_distance;
    NfHitZone best=NF_HIT_NONE;
    float t=FLT_MAX;
    bool hit=false;

    /* Weak zones are evaluated before adjacent body volumes so an exact shared
       boundary resolves toward the deliberate David-counterplay hit zone. */
    NfVec3 foot_min={actor_base.x-NF_CATTLER_WEAK_HALF_WIDTH,actor_base.y+NF_CATTLER_FOOT_MIN_Y,actor_base.z-NF_CATTLER_WEAK_HALF_WIDTH};
    NfVec3 foot_max={actor_base.x+NF_CATTLER_WEAK_HALF_WIDTH,actor_base.y+NF_CATTLER_FOOT_MAX_Y,actor_base.z+NF_CATTLER_WEAK_HALF_WIDTH};
    hit=ray_aabb(origin,direction,foot_min,foot_max,&t);
    consider(hit,t,NF_HIT_FOOT,&nearest,&best);

    NfVec3 knee_min={actor_base.x-NF_CATTLER_WEAK_HALF_WIDTH,actor_base.y+NF_CATTLER_KNEE_MIN_Y,actor_base.z-NF_CATTLER_WEAK_HALF_WIDTH};
    NfVec3 knee_max={actor_base.x+NF_CATTLER_WEAK_HALF_WIDTH,actor_base.y+NF_CATTLER_KNEE_MAX_Y,actor_base.z+NF_CATTLER_WEAK_HALF_WIDTH};
    t=FLT_MAX;
    hit=ray_aabb(origin,direction,knee_min,knee_max,&t);
    consider(hit,t,NF_HIT_KNEE,&nearest,&best);

    NfVec3 body_min={actor_base.x-NF_CATTLER_BODY_HALF_WIDTH,actor_base.y+NF_CATTLER_BODY_MIN_Y,actor_base.z-NF_CATTLER_BODY_HALF_WIDTH};
    NfVec3 body_max={actor_base.x+NF_CATTLER_BODY_HALF_WIDTH,actor_base.y+NF_CATTLER_BODY_MAX_Y,actor_base.z+NF_CATTLER_BODY_HALF_WIDTH};
    t=FLT_MAX;
    hit=ray_aabb(origin,direction,body_min,body_max,&t);
    consider(hit,t,NF_HIT_BODY,&nearest,&best);

    NfVec3 head={actor_base.x,actor_base.y+NF_CATTLER_HEAD_CENTER_Y,actor_base.z};
    t=FLT_MAX;
    hit=ray_sphere(origin,direction,head,NF_CATTLER_HEAD_RADIUS,&t);
    consider(hit,t,NF_HIT_HEAD,&nearest,&best);

    if(best!=NF_HIT_NONE&&distance_out!=NULL)*distance_out=nearest;
    return best;
}
