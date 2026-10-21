#include "nf_multi_ma03b.h"
#include <math.h>
#include <string.h>
NfMa03bChain nf_ma03b_oracle(const double x[3],const double v[3],
 const double m[3],double radius,double dt){
 NfMa03bChain o={0};
 if(!x||!v||!m||!isfinite(radius)||radius<=0||!isfinite(dt)||dt<=0)return o;
 for(int i=0;i<3;i++){
  if(!isfinite(x[i])||!isfinite(v[i])||!isfinite(m[i])||m[i]<=0)return o;
  o.x[i]=x[i];o.v[i]=v[i];
 }
 if(o.x[1]-o.x[0]<=2*radius||o.x[2]-o.x[1]<=2*radius)return o;
 double left=dt;
 for(unsigned k=0;k<12;k++){
  double event=left;int pair=-1;
  for(int i=0;i<2;i++){
   const double closing=o.v[i]-o.v[i+1];
   if(closing<=0)continue;
   const double gap=o.x[i+1]-o.x[i]-2*radius;
   const double t=gap/closing;
   if(t>=0&&t<event){event=t;pair=i;}
  }
  for(int i=0;i<3;i++)o.x[i]+=o.v[i]*event;
  left-=event;
  if(pair<0){o.complete=true;return o;}
  /* Closed-form elastic unequal-mass collision, independent of A4. */
  const double left_v=o.v[pair],right_v=o.v[pair+1];
  const double total=m[pair]+m[pair+1];
  o.v[pair]=((m[pair]-m[pair+1])*left_v+2*m[pair+1]*right_v)/total;
  o.v[pair+1]=(2*m[pair]*left_v+(m[pair+1]-m[pair])*right_v)/total;
  o.impacts++;
  if(left<=0){o.complete=true;return o;}
 }
 return o;
}
Nf18b2UseDecision nf_ma023_recheck(const NfWorld *w,const Nf18a6Runtime *r,
 NfMa03Provider provider,void *ctx,uint32_t action,Nf18b2Model model){
 Nf18b2UseDecision first=nf_ma03_b2_owner_evaluate(w,r,provider,ctx,action,model);
 if(first.status!=NF18B2_ELIGIBLE)return first;
 const Nf18b2UseDecision second=nf_ma03_b2_owner_evaluate(w,r,provider,ctx,action,model);
 if(second.status!=NF18B2_ELIGIBLE)return second;
 if(first.status!=second.status||first.physical!=second.physical||
    first.contract!=second.contract||first.reason!=second.reason){
  first.status=NF18B2_STALE;return first;
 }
 /* Caller must STILL bind a stable server scene and durable publisher:
  * matching outcomes alone do not prove equal scene versions/content. */
 return second;
}
