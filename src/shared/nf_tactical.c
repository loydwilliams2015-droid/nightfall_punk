#include "nf_tactical.h"

#include <float.h>
#include <math.h>

static float clamp01(float v){return v<0.0f?0.0f:(v>1.0f?1.0f:v);}

static bool ray_aabb(NfVec3 o,NfVec3 d,NfVec3 mn,NfVec3 mx,float *distance){
    float tmin=0.0f,tmax=FLT_MAX;
    const float ov[3]={o.x,o.y,o.z},dv[3]={d.x,d.y,d.z},lo[3]={mn.x,mn.y,mn.z},hi[3]={mx.x,mx.y,mx.z};
    for(int i=0;i<3;++i){
        if(fabsf(dv[i])<1e-6f){if(ov[i]<lo[i]||ov[i]>hi[i])return false;continue;}
        float t1=(lo[i]-ov[i])/dv[i],t2=(hi[i]-ov[i])/dv[i];
        if(t1>t2){float t=t1;t1=t2;t2=t;}
        if(t1>tmin)tmin=t1;if(t2<tmax)tmax=t2;if(tmin>tmax)return false;
    }
    if(tmax<0.0f)return false;if(distance)*distance=tmin>=0.0f?tmin:tmax;return true;
}

static bool blocked(const NfWorld *world,NfVec3 from,NfVec3 to){
    const NfVec3 delta={to.x-from.x,to.y-from.y,to.z-from.z};
    const float len=sqrtf(delta.x*delta.x+delta.y*delta.y+delta.z*delta.z);
    if(len<0.001f)return false;
    const NfVec3 dir={delta.x/len,delta.y/len,delta.z/len};
    for(size_t i=0u;i<world->collider_count;++i){
        const NfCollider *c=&world->colliders[i];
        if(c->kind==NF_COLLIDER_LADDER)continue;
        float hit=0.0f;
        if(ray_aabb(from,dir,c->min,c->max,&hit)&&hit>0.05f&&hit<len-0.10f)return true;
    }
    return false;
}

float nf_tactical_exposure(const NfWorld *world,NfVec3 threat,NfVec3 candidate){
    if(world==NULL)return 1.0f;
    const NfVec3 threat_eye={threat.x,threat.y+1.4f,threat.z};
    static const float heights[3]={0.45f,1.00f,1.55f};
    unsigned exposed=0u;
    for(size_t i=0u;i<3u;++i){
        const NfVec3 sample={candidate.x,candidate.y+heights[i],candidate.z};
        if(!blocked(world,threat_eye,sample))++exposed;
    }
    return (float)exposed/3.0f;
}

float nf_tactical_cover_utility(float exposure,float travel_m,float option_distance_m){
    const float travel_fit=1.0f-clamp01(travel_m/18.0f);
    const float option_value=clamp01(option_distance_m/20.0f);
    return clamp01((1.0f-clamp01(exposure))*0.62f+travel_fit*0.28f+option_value*0.10f);
}
