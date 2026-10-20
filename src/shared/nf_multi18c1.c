#include "nf_multi18c1.h"
#include <math.h>
#include <string.h>
#include <stdint.h>
static size_t by_id(const NfMulti18c1World *w,uint32_t id){
 for(size_t i=0;i<w->count;++i)if(w->bodies[i].id==id)return i;
 return w->count;
}
static int before(const NfMulti18c1Proposal *a,const NfMulti18c1Proposal *b){
 return a->actor<b->actor || (a->actor==b->actor && a->action<b->action);
}
NfMulti18c1Status nf_multi18c1_stage(const NfMulti18c1World *w,
 const NfMulti18c1Proposal *p,size_t n,const Nf18a4Constraint *contacts,
 size_t nc,float dt,NfMulti18c1Candidate *out){
 if(!out)return NF_MULTI_INVALID;
 memset(out,0,sizeof *out);
 if(!w||!w->count||w->count>NF18A4_MAX_BODIES||n>w->count||
    (n&&!p)||(nc&&!contacts)||nc>NF18A4_MAX_CONSTRAINTS||
    !isfinite(dt)||dt<=0||w->tick==UINT32_MAX||w->revision==UINT32_MAX)
    return NF_MULTI_INVALID;
 for(size_t i=0;i<w->count;++i){
  if(!nf18a4_body_valid(&w->bodies[i]))return NF_MULTI_INVALID;
  for(size_t j=0;j<i;++j)if(w->bodies[i].id==w->bodies[j].id)
   return NF_MULTI_INVALID;
 }
 NfMulti18c1Proposal order[NF18A4_MAX_BODIES];
 if(n)memcpy(order,p,n*sizeof *p);
 for(size_t i=1;i<n;++i){
  NfMulti18c1Proposal v=order[i];size_t j=i;
  while(j&&before(&v,&order[j-1])){order[j]=order[j-1];--j;}
  order[j]=v;
 }
 NfMulti18c1World next=*w;
 for(size_t i=0;i<n;++i){
  if(!order[i].actor||!order[i].action||order[i].revision!=w->revision)
   return NF_MULTI_STALE;
  if(i&&order[i].actor==order[i-1].actor)return NF_MULTI_CONFLICT;
  const size_t k=by_id(&next,order[i].actor);
  if(k==next.count||next.bodies[k].inverse_mass<=0)return NF_MULTI_INVALID;
  Nf18a4MotorReceipt r={0};
  if(!nf18a4_motor_drive(&next.bodies[k],order[i].motor,dt,&r))
   return NF_MULTI_INVALID;
 }
 Nf18a4IslandResult result={0};
 if(nc){
  result=nf18a4_solve_island(next.bodies,next.count,contacts,nc,
      NF18A4_FRICTION_ADAPTIVE,w->tick+1,w->revision);
  if(result.status==NF18A4_PENDING_CONTACT)return NF_MULTI_PENDING;
  if(result.status!=NF18A4_ACCEPTED)return NF_MULTI_INVALID;
 }
 for(size_t i=0;i<next.count;++i){
  next.bodies[i].center.x+=next.bodies[i].velocity.x*dt;
  next.bodies[i].center.y+=next.bodies[i].velocity.y*dt;
  next.bodies[i].center.z+=next.bodies[i].velocity.z*dt;
  if(!nf18a4_body_valid(&next.bodies[i]))return NF_MULTI_INVALID;
 }
 next.tick++;next.revision++;
 out->successor=next;out->contact=result;
 out->applied_motors=(uint32_t)n;
 return NF_MULTI_COMMITTED;
}
NfMulti18c1Status nf_multi18c1_publish(NfMulti18c1World *w,
 const NfMulti18c1Candidate *c){
 if(!w||!c||c->successor.count!=w->count||
    c->successor.tick!=w->tick+1||c->successor.revision!=w->revision+1)
   return NF_MULTI_STALE;
 /* Publishing is one struct assignment: no parallel world mutation.
  * Persistence, B2 and live A6 integration remain explicit hold gates. */
 *w=c->successor;return NF_MULTI_COMMITTED;
}
