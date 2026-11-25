/* MAP-01: incumbent geometry regression, not production A6 acceptance. */
#include "nf_world.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"MAP01 failure line %d: %s\n",__LINE__,#x); return 1; } } while(0)
static int near(float a,float b){return fabsf(a-b)<0.001f;}
static int box(NfVec3 lo,NfVec3 hi){
    return isfinite(lo.x)&&isfinite(lo.y)&&isfinite(lo.z)&&
           isfinite(hi.x)&&isfinite(hi.y)&&isfinite(hi.z)&&
           lo.x<hi.x&&lo.y<hi.y&&lo.z<hi.z;
}
static int match(const NfCollider *c,NfVec3 lo,NfVec3 hi){
    return c->kind==NF_COLLIDER_SOLID &&
        near(c->min.x,lo.x)&&near(c->min.y,lo.y)&&near(c->min.z,lo.z)&&
        near(c->max.x,hi.x)&&near(c->max.y,hi.y)&&near(c->max.z,hi.z);
}
int main(void){
    static NfWorld w;
    nf_world_init(&w,20260815u);
    nf_world_build_movement_lab(&w);
    CHECK(w.collider_count==54u);
    CHECK(w.ramp_count==3u);
    unsigned solid=0,ladder=0,moving=0;
    unsigned boundary[5]={0};
    const NfVec3 lo[5]={{-250,-.5f,-400},{-250,0,-400},{249.2f,0,-400},{-250,0,-400},{-250,0,399.2f}};
    const NfVec3 hi[5]={{250,0,400},{-249.2f,6,400},{250,6,400},{250,6,-399.2f},{250,6,400}};
    const NfVec3 spawn={0,.05f,-18};
    const float radius=w.movement.radius,height=w.movement.stand_height;
    CHECK(isfinite(radius)&&radius>0&&isfinite(height)&&height>0);
    for(size_t i=0;i<w.collider_count;i++){
        const NfCollider *c=&w.colliders[i];
        CHECK(box(c->min,c->max));
        CHECK(c->min.x>=-250&&c->max.x<=250&&c->min.z>=-400&&c->max.z<=400);
        switch(c->kind){
            case NF_COLLIDER_SOLID: solid++;break;
            case NF_COLLIDER_LADDER: ladder++;break;
            case NF_COLLIDER_MOVING_PLATFORM: moving++;break;
            default: CHECK(0);
        }
        for(size_t j=0;j<5;j++) if(match(c,lo[j],hi[j])) boundary[j]++;
        /* Conservative standing AABB at initial V15 player spawn. */
        CHECK(!(spawn.x+radius>c->min.x&&spawn.x-radius<c->max.x&&
                spawn.z+radius>c->min.z&&spawn.z-radius<c->max.z&&
                spawn.y+height>c->min.y&&spawn.y<c->max.y));
    }
    CHECK(solid==51&&ladder==2&&moving==1);
    for(size_t j=0;j<5;j++) CHECK(boundary[j]==1);
    for(size_t i=0;i<w.ramp_count;i++){
        const NfRamp *r=&w.ramps[i];
        CHECK(box(r->min,r->max));
        CHECK(r->min.x>=-250&&r->max.x<=250&&r->min.z>=-400&&r->max.z<=400);
        CHECK(r->axis==NF_RAMP_POS_X||r->axis==NF_RAMP_NEG_X||
              r->axis==NF_RAMP_POS_Z||r->axis==NF_RAMP_NEG_Z);
        const float run=(r->axis==NF_RAMP_POS_X||r->axis==NF_RAMP_NEG_X)?
            r->max.x-r->min.x:r->max.z-r->min.z;
        CHECK((r->max.y-r->min.y)/run<=.2f);
    }
    CHECK(w.tick==0u);
    puts("MAP01 PASS incumbent 54 colliders, 3 ramps, 5 boundary surfaces, standing spawn; static geometry only");
    return 0;
}
