#include "nf_multi_broad18c1.h"
#include <math.h>
#include <string.h>
typedef struct {double low[3],high[3];uint32_t id;} Sweep;
static bool bound(const Nf18a4Body *b,float dt,Sweep *out){
 if(!nf18a4_body_valid(b)||!isfinite(dt)||dt<=0)return false;
 const double center[3]={b->center.x,b->center.y,b->center.z};
 const double velocity[3]={b->velocity.x,b->velocity.y,b->velocity.z};
 const bool capsule=b->radius>0&&b->height>0;
 if(capsule&&b->height<2.f*b->radius)return false;
 const double extent[3]={
  capsule?b->radius:b->box_half.x,
  capsule?b->height*.5:b->box_half.y,
  capsule?b->radius:b->box_half.z};
 for(int k=0;k<3;++k){
  double future=center[k]+velocity[k]*(double)dt;
  if(!isfinite(future)||!isfinite(extent[k])||extent[k]<=0)return false;
  out->low[k]=fmin(center[k],future)-extent[k];
  out->high[k]=fmax(center[k],future)+extent[k];
 }
 out->id=b->id;return true;
}
bool nf_multi_broad18c1(const NfMulti18c1World *w,float dt,NfMultiBroad *out){
 if(!out)return false;
 memset(out,0,sizeof *out);
 if(!w||!w->count||w->count>NF18A4_MAX_BODIES)return false;
 Sweep a[NF18A4_MAX_BODIES];
 for(size_t i=0;i<w->count;++i){
  if(!bound(&w->bodies[i],dt,&a[i]))return false;
  for(size_t j=0;j<i;++j)if(a[i].id==a[j].id)return false;
 }
 for(size_t i=0;i<w->count;++i)for(size_t j=i+1;j<w->count;++j){
  out->tested_pairs++;
  bool overlapping=true;
  for(int k=0;k<3;++k)
   if(a[i].high[k]<a[j].low[k]||a[j].high[k]<a[i].low[k])
    overlapping=false;
  if(!overlapping)continue;
  NfMultiPair pair={a[i].id<a[j].id?a[i].id:a[j].id,
   a[i].id<a[j].id?a[j].id:a[i].id};
  size_t pos=out->count++;
  while(pos&&
   (out->pairs[pos-1].id_a>pair.id_a||
    (out->pairs[pos-1].id_a==pair.id_a&&out->pairs[pos-1].id_b>pair.id_b))){
   out->pairs[pos]=out->pairs[pos-1];pos--;
  }
  out->pairs[pos]=pair;
 }
 return true;
}
