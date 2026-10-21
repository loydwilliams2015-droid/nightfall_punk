#include "nf_multi_ma03d.h"
#include <math.h>
#include <string.h>
#include <float.h>
NfMa01c2Result nf_ma03d_three_body(const Nf18a4Body *input,float dt,uint32_t tick,uint32_t rev){
 NfMa01c2Result out={0};out.status=NF_NARROW_PENDING;
 if(!input||!isfinite(dt)||dt<=0||!tick||!rev)return out;
 Nf18a4Body b[3];memcpy(b,input,sizeof b);
 for(int i=0;i<3;i++){
  if(!nf18a4_body_valid(&b[i])||b[i].inverse_mass<=0||
   b[i].radius<=0||b[i].height<2*b[i].radius||
   fabsf(b[i].velocity.y)>1e-6f||fabsf(b[i].velocity.z)>1e-6f||
   fabsf(b[i].center.z-b[0].center.z)>1e-6f||
   fabsf(b[i].center.y-b[0].center.y)>1e-6f)return out;
  if(i && (b[i].id==b[0].id||b[i].id==b[i-1].id ||
           b[i].center.x-b[i-1].center.x<=b[i].radius+b[i-1].radius))return out;
 }
 double remaining=dt;
 for(unsigned step=0;step<12;step++){
  double earliest=remaining;int pair=-1;
  for(int k=0;k<2;k++){
   const double closing=(double)b[k].velocity.x-b[k+1].velocity.x;
   if(closing<=1e-7)continue;
   const double gap=(double)b[k+1].center.x-b[k].center.x-b[k].radius-b[k+1].radius;
   const double t=gap/closing;
   if(t>=-1e-7&&t<earliest){earliest=fmax(0,t);pair=k;}
  }
  for(int k=0;k<3;k++)b[k].center.x+=(float)(earliest*b[k].velocity.x);
  remaining-=earliest;out.advanced_seconds+=(float)earliest;
  if(pair<0){memcpy(out.bodies,b,sizeof b);out.count=3;out.status=NF_NARROW_OK;return out;}
  Nf18a4Constraint c={0};c.body_a=b[pair].id;c.body_b=b[pair+1].id;
  c.feature_id=(uint32_t)(pair+1);c.normal_b_to_a=(NfVec3){-1,0,0};
  c.contact_point=(NfVec3){b[pair+1].center.x-b[pair+1].radius,b[pair].center.y,0};
  c.restitution=1.f;
  Nf18a4Receipt r=nf18a4_apply_contact(&b[pair],&b[pair+1],c,
                                       NF18A4_RECIPROCAL_LINEAR,tick,rev);
  if(r.status!=NF18A4_ACCEPTED||!r.impulses_applied)return out;
  out.impacts++;
  if(remaining<=0){memcpy(out.bodies,b,sizeof b);out.count=3;out.status=NF_NARROW_OK;return out;}
 }
 return out;
}
bool nf_ma025_prepublication(const NfWorld *w,const Nf18a6Runtime *r,
 NfMa03Provider source,void *ctx,uint32_t action,Nf18b2Model model,
 const NfMa024Receipt *expected){
 if(!w||!r||!expected||w->tick_in_progress||
    w->tick_owner_context!=r||expected->tick!=w->tick+1u||
    expected->world_revision!=r->physical.world.revision||
    expected->material_epoch!=r->physical.world.grid.global_revision)return false;
 return nf_ma024_recheck(w,r,source,ctx,action,model,expected);
}
