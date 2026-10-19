#include "nf_authority18b7.h"
#include <string.h>
#include <limits.h>

void nf7_init(Nf7World *w){if(w){memset(w,0,sizeof(*w));w->epoch=1;w->tick=1;w->cursor=1;}}
static int object_index(const Nf7World *w,uint32_t id){
 for(size_t i=0;i<w->object_count;++i)if(w->objects[i].id==id)return (int)i;
 return -1;
}
bool nf7_world_valid(const Nf7World *w){
 if(!w||!w->epoch||!w->tick||!w->cursor||w->object_count>NF7_OBJECTS||w->receipt_count>NF7_RECEIPTS)return false;
 for(size_t i=0;i<w->object_count;++i){
  const Nf7Object *o=&w->objects[i];
  if(!o->id||!o->generation||!o->revision||!o->material_identity_epoch||o->quantity<0)return false;
  for(size_t j=0;j<i;++j)if(o->id==w->objects[j].id)return false;
 }
 for(size_t i=0;i<w->receipt_count;++i){
  const Nf7Request *a=&w->receipts[i].request;
  if(!a->actor||!a->action)return false;
  for(size_t j=0;j<i;++j){const Nf7Request *b=&w->receipts[j].request;
   if(a->actor==b->actor&&a->action==b->action)return false;}
 }
 return true;
}
bool nf7_add_object(Nf7World *w,Nf7Object object){
 if(!nf7_world_valid(w)||w->object_count==NF7_OBJECTS||!object.id||!object.generation||
    !object.revision||!object.material_identity_epoch||object.quantity<0||
    object_index(w,object.id)>=0)return false;
 w->objects[w->object_count++]=object;return true;
}
static bool same_request(Nf7Request a,Nf7Request b){
 return a.actor==b.actor&&a.action==b.action&&a.object==b.object&&
 a.generation==b.generation&&a.epoch==b.epoch&&a.revision==b.revision&&
 a.debit==b.debit&&a.physical==b.physical&&a.contractual==b.contractual;
}
static bool preferred(Nf7Request a,Nf7Request b,Nf7Policy p,uint32_t cursor){
 if(p==NF7_ROTATING_ACTOR){
  const bool aa=a.actor>=cursor,bb=b.actor>=cursor;
  if(aa!=bb)return aa;
 }
 return a.actor!=b.actor?a.actor<b.actor:a.action<b.action;
}
static void classify(const Nf7World *w,const Nf7Request *req,size_t count,Nf7Status *out){
 for(size_t i=0;i<count;++i){Nf7Request r=req[i];out[i]=NF7_INVALID;
  if(!r.actor||!r.action||!r.object||!r.generation||r.debit<0||
     r.physical>NF7_UNKNOWN||r.contractual>NF7_UNKNOWN)continue;
  bool repeated=false;
  for(size_t j=0;j<count;++j)if(i!=j&&req[j].actor==r.actor&&req[j].action==r.action)repeated=true;
  if(repeated)continue; /* Both aliases fail closed, independent of input order. */
  bool prior=false;
  for(size_t j=0;j<w->receipt_count;++j){Nf7Request existing=w->receipts[j].request;
   if(existing.actor==r.actor&&existing.action==r.action){
    out[i]=same_request(existing,r)?NF7_DUPLICATE:NF7_INVALID;prior=true;break;}}
  if(prior)continue;
  if(r.epoch!=w->epoch){out[i]=NF7_STALE;continue;}
  int k=object_index(w,r.object);
  if(k<0){out[i]=NF7_INVALID;continue;}
  const Nf7Object *o=&w->objects[k];
  if(r.generation!=o->generation||r.revision!=o->revision){out[i]=NF7_STALE;continue;}
  if(r.physical==NF7_UNKNOWN||r.contractual==NF7_UNKNOWN){out[i]=NF7_PENDING;continue;}
  if(r.physical!=NF7_ELIGIBLE||r.contractual!=NF7_ELIGIBLE||
    (o->owner&&o->owner!=r.actor)||o->quantity<r.debit){out[i]=NF7_REFUSED;continue;}
  out[i]=NF7_COMMITTED; /* Provisional. Actual publish happens afterwards. */
 }
}
bool nf7_batch(Nf7World *w,const Nf7Request *req,size_t count,Nf7Policy policy,
 Nf7Persist persist,void *ctx,Nf7Status *out){
 if(!nf7_world_valid(w)||!persist||!out||count>NF7_REQUESTS||
  (count&&!req)||(policy!=NF7_LOW_ACTOR&&policy!=NF7_ROTATING_ACTOR))return false;
 classify(w,req,count,out);
 int winners[NF7_OBJECTS];for(size_t j=0;j<NF7_OBJECTS;++j)winners[j]=-1;
 for(size_t i=0;i<count;++i)if(out[i]==NF7_COMMITTED){
  const int k=object_index(w,req[i].object);int previous=winners[k];
  if(previous<0||preferred(req[i],req[previous],policy,w->cursor))winners[k]=(int)i;
 }
 size_t n=0;
 for(size_t i=0;i<count;++i)if(out[i]==NF7_COMMITTED){
  int k=object_index(w,req[i].object);
  if(winners[k]!=(int)i)out[i]=NF7_CONTESTED;else ++n;
 }
 if(!n)return true;
 if(w->epoch==UINT32_MAX||w->tick==UINT32_MAX||w->receipt_count+n>NF7_RECEIPTS){
  for(size_t i=0;i<count;++i)if(out[i]==NF7_COMMITTED)out[i]=NF7_FULL;
  return true;
 }
 Nf7World staged=*w;
 /* Iterate in increasing object ID, so permutation of request input does not
    affect receipt order, snapshot state, or the policy's selection. */
 for(size_t step=0;step<n;++step){
  int best=-1;
  for(size_t j=0;j<staged.object_count;++j)if(winners[j]>=0){
   if(best<0||staged.objects[j].id<staged.objects[best].id)best=(int)j;
  }
  if(best<0)return false; /* Invariant violation, no publication. */
  size_t idx=(size_t)winners[best];winners[best]=-1;
  Nf7Object *o=&staged.objects[best];Nf7Request r=req[idx];
  if(o->revision==UINT32_MAX){
   for(size_t i=0;i<count;++i)if(out[i]==NF7_COMMITTED)out[i]=NF7_FULL;
   return true;
  }
  o->owner=r.actor;o->quantity-=r.debit;o->revision++;
  staged.receipts[staged.receipt_count++]=(Nf7Receipt){r,w->epoch+1};
  if(policy==NF7_ROTATING_ACTOR)staged.cursor=r.actor==UINT32_MAX?1:r.actor+1;
 }
 staged.epoch++;staged.tick++;
 if(!persist(ctx,&staged)){
  for(size_t i=0;i<count;++i)if(out[i]==NF7_COMMITTED)out[i]=NF7_PERSIST_FAIL;
  return true;
 }
 *w=staged;return true;
}
