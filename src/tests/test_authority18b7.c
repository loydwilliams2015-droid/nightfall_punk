#include "nf_authority18b7.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static unsigned checks=0;
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return false;}++checks;}while(0)
static bool success(void *ctx,const Nf7World *w){*(Nf7World*)ctx=*w;return true;}
static bool failure(void *ctx,const Nf7World *w){(void)ctx;(void)w;return false;}
static Nf7World setup(void){Nf7World w;nf7_init(&w);Nf7Object o={10,1,1,27,0,100};(void)nf7_add_object(&w,o);o.id=20;(void)nf7_add_object(&w,o);return w;}
static Nf7Request request(unsigned actor,unsigned action,unsigned object,int debit){
 Nf7Request r={actor,action,object,1,1,1,debit,NF7_ELIGIBLE,NF7_ELIGIBLE};return r;
}
static uint32_t next(uint32_t *s){*s^=*s<<13;*s^=*s>>17;*s^=*s<<5;return *s;}
static bool test_basic(void){
 Nf7World w=setup(),disk=w;Nf7Status st[4];Nf7Request r[]={request(4,1,10,12),request(2,2,10,19),request(9,3,20,5)};
 CHECK(nf7_batch(&w,r,3,NF7_LOW_ACTOR,success,&disk,st));
 CHECK(st[0]==NF7_CONTESTED&&st[1]==NF7_COMMITTED&&st[2]==NF7_COMMITTED);
 CHECK(memcmp(&w,&disk,sizeof w)==0);CHECK(w.receipt_count==2);CHECK(w.epoch==2&&w.tick==2);
 CHECK(w.objects[0].quantity==81&&w.objects[0].owner==2);
 CHECK(w.objects[1].quantity==95&&w.objects[1].owner==9);
 CHECK(w.objects[0].material_identity_epoch==27);CHECK(w.receipts[0].request.object==10);
 CHECK(w.receipts[1].request.object==20);
 Nf7World before=w;Nf7Request repeated=r[1];
 CHECK(nf7_batch(&w,&repeated,1,NF7_LOW_ACTOR,success,&disk,st));CHECK(st[0]==NF7_DUPLICATE);
 CHECK(memcmp(&before,&w,sizeof w)==0);
 repeated.debit=11;CHECK(nf7_batch(&w,&repeated,1,NF7_LOW_ACTOR,success,&disk,st));CHECK(st[0]==NF7_INVALID);
 CHECK(memcmp(&before,&w,sizeof w)==0);
 return true;
}
static bool test_failures(void){
 Nf7World w=setup(),disk=w,before=w;Nf7Status st[4];Nf7Request r=request(1,1,10,5);
 CHECK(nf7_batch(&w,&r,1,NF7_LOW_ACTOR,failure,NULL,st));CHECK(st[0]==NF7_PERSIST_FAIL);
 CHECK(memcmp(&w,&before,sizeof w)==0);
 r.physical=NF7_UNKNOWN;CHECK(nf7_batch(&w,&r,1,NF7_LOW_ACTOR,success,&disk,st));
 CHECK(st[0]==NF7_PENDING);CHECK(memcmp(&w,&before,sizeof w)==0);
 r.physical=NF7_ELIGIBLE;r.epoch=2;
 CHECK(nf7_batch(&w,&r,1,NF7_LOW_ACTOR,success,&disk,st));CHECK(st[0]==NF7_STALE);
 r.epoch=1;r.generation=2;
 CHECK(nf7_batch(&w,&r,1,NF7_LOW_ACTOR,success,&disk,st));CHECK(st[0]==NF7_STALE);
 r.generation=1;r.physical=NF7_DENIED;
 CHECK(nf7_batch(&w,&r,1,NF7_LOW_ACTOR,success,&disk,st));CHECK(st[0]==NF7_REFUSED);
 r.physical=NF7_ELIGIBLE;
 Nf7Request req[2]={r,r};req[1].debit=8;
 CHECK(nf7_batch(&w,req,2,NF7_LOW_ACTOR,success,&disk,st));
 CHECK(st[0]==NF7_INVALID&&st[1]==NF7_INVALID);CHECK(memcmp(&w,&before,sizeof w)==0);
 w.epoch=UINT32_MAX;r.epoch=UINT32_MAX;before=w;
 CHECK(nf7_batch(&w,&r,1,NF7_LOW_ACTOR,success,&disk,st));CHECK(st[0]==NF7_FULL);
 CHECK(memcmp(&w,&before,sizeof w)==0);
 w=setup();w.objects[0].revision=UINT32_MAX;r.epoch=1;r.revision=UINT32_MAX;before=w;
 CHECK(nf7_batch(&w,&r,1,NF7_LOW_ACTOR,success,&disk,st));CHECK(st[0]==NF7_FULL);
 CHECK(memcmp(&w,&before,sizeof w)==0);
 return true;
}
static bool test_permutations(void){
 Nf7World a=setup(),b=a,da=a,db=b;Nf7Status sa[3],sb[3];
 Nf7Request r[]={request(8,1,10,4),request(2,2,10,7),request(3,3,20,9)};
 Nf7Request p[]={r[2],r[0],r[1]};
 CHECK(nf7_batch(&a,r,3,NF7_ROTATING_ACTOR,success,&da,sa));
 CHECK(nf7_batch(&b,p,3,NF7_ROTATING_ACTOR,success,&db,sb));
 CHECK(memcmp(&a,&b,sizeof a)==0);CHECK(memcmp(&a,&da,sizeof a)==0);
 CHECK(sa[0]==sb[1]&&sa[1]==sb[2]&&sa[2]==sb[0]);
 CHECK(a.objects[0].quantity+a.objects[1].quantity==200-7-9);
 return true;
}
static bool test_corpus(uint32_t seed,unsigned count,unsigned *positive,unsigned *rejected){
 for(unsigned i=0;i<count;++i){
  uint32_t x=next(&seed);Nf7World w=setup(),disk=w,before=w;
  Nf7Request r=request(1+(x&7),1+(i%1000),10,(int)((x>>4)%110));
  if(x&0x100)r.physical=NF7_UNKNOWN;
  if(x&0x200)r.contractual=NF7_DENIED;
  if(x&0x400)r.epoch=2;
  if(x&0x800)r.generation=2;
  if(x&0x1000)r.revision=2;
  Nf7Status st;
  Nf7Status oracle=NF7_COMMITTED;
  if(r.epoch!=1||r.generation!=1||r.revision!=1)oracle=NF7_STALE;
  else if(r.physical==NF7_UNKNOWN)oracle=NF7_PENDING;
  else if(r.contractual==NF7_DENIED||r.debit>100)oracle=NF7_REFUSED;
  CHECK(nf7_batch(&w,&r,1,NF7_LOW_ACTOR,success,&disk,&st));
  CHECK(st==oracle);
  if(oracle==NF7_COMMITTED){++*positive;
   CHECK(w.objects[0].quantity==100-r.debit);
   CHECK(w.receipt_count==1&&w.epoch==2&&w.objects[0].material_identity_epoch==27);
   CHECK(memcmp(&w,&disk,sizeof w)==0);
  }else{++*rejected;CHECK(memcmp(&w,&before,sizeof w)==0);}
 }
 return true;
}

static bool test_competing_policies(void){
 Nf7World low=setup(),rot=low,disk1=low,disk2=low;low.cursor=5;rot.cursor=5;
 Nf7Request a[2]={request(2,1,10,7),request(8,2,10,9)};
 Nf7Status l[2],r[2];
 CHECK(nf7_batch(&low,a,2,NF7_LOW_ACTOR,success,&disk1,l));
 CHECK(nf7_batch(&rot,a,2,NF7_ROTATING_ACTOR,success,&disk2,r));
 CHECK(l[0]==NF7_COMMITTED&&l[1]==NF7_CONTESTED);
 CHECK(r[0]==NF7_CONTESTED&&r[1]==NF7_COMMITTED);
 CHECK(low.objects[0].owner==2&&low.objects[0].quantity==93);
 CHECK(rot.objects[0].owner==8&&rot.objects[0].quantity==91);
 CHECK(low.receipt_count==1&&rot.receipt_count==1);
 return true;
}
static bool test_pathological(uint32_t seed,unsigned cases){
 for(unsigned i=0;i<cases;++i){
  uint32_t x=next(&seed);Nf7World w=setup(),disk=w,before=w;
  Nf7Request r[3]={request(1+(x%16u),10+i,10,1),
     request(1+((x>>8)%16u),20+i,10,3),request(1+((x>>16)%16u),30+i,20,4)};
  /* Test physically unknown, same action collisions and determinism under permutation. */
  if(i%4==0)r[0].physical=NF7_UNKNOWN;
  if(i%7==0){r[1].actor=r[0].actor;r[1].action=r[0].action;}
  Nf7Status st[3];CHECK(nf7_batch(&w,r,3,NF7_ROTATING_ACTOR,success,&disk,st));
  CHECK(memcmp(&w,&disk,sizeof w)==0);
  unsigned committed=0;int64_t debit=0;
  for(unsigned j=0;j<3;++j)if(st[j]==NF7_COMMITTED){committed++;debit+=r[j].debit;}
  CHECK(committed<=2);CHECK(w.receipt_count==committed);
  CHECK(w.objects[0].quantity+w.objects[1].quantity==200-debit);
  CHECK(w.objects[0].material_identity_epoch==27&&w.objects[1].material_identity_epoch==27);
  if(!committed)CHECK(memcmp(&before,&w,sizeof w)==0);
 }
 return true;
}
static bool test_boundary(uint32_t seed,unsigned cases){
 for(unsigned i=0;i<cases;++i){
  uint32_t x=next(&seed);Nf7World w=setup(),disk=w,before;
  Nf7Request r=request(1+(x&31u),10+i,10,1);Nf7Status st;
  switch(i%8){
   case 0:w.epoch=UINT32_MAX;r.epoch=UINT32_MAX;break;
   case 1:w.tick=UINT32_MAX;break;
   case 2:w.objects[0].revision=UINT32_MAX;r.revision=UINT32_MAX;break;
   case 3:w.receipt_count=NF7_RECEIPTS;for(unsigned j=0;j<NF7_RECEIPTS;++j){
       w.receipts[j].request=request(200+j,200+j,20,0);
      }break;
   case 4:w.objects[0].quantity=0;r.debit=1;break;
   case 5:r.generation=UINT32_MAX;break;
   case 6:r.contractual=NF7_UNKNOWN;break;
   default:r.debit=0;break;
  }
  before=w;disk=w;
  CHECK(nf7_batch(&w,&r,1,NF7_LOW_ACTOR,success,&disk,&st));
  if(i%8==7){CHECK(st==NF7_COMMITTED);CHECK(w.receipt_count==1);}
  else {Nf7Status expected=i%8==5?NF7_STALE:
     i%8==6?NF7_PENDING:i%8==4?NF7_REFUSED:NF7_FULL;
     CHECK(st==expected);CHECK(memcmp(&before,&w,sizeof w)==0);}
 }
 return true;
}

int main(void){unsigned pos=0,rej=0;clock_t start=clock();
 if(!test_basic()||!test_failures()||!test_permutations()||!test_competing_policies())return 1;
 struct {const char *name;uint32_t seed;unsigned n;} strata[]={
  {"canonical",0x7001u,128},{"randomized",0x18B70002u,2048},
  {"pathological",0x18B70003u,512},{"boundary",0x18B70004u,512},
  {"heldout",0xCE107B77u,2048}};
 for(size_t k=0;k<sizeof strata/sizeof strata[0];++k){unsigned p=0,r=0;
  bool ok=k==2?test_pathological(strata[k].seed,strata[k].n):
      k==3?test_boundary(strata[k].seed,strata[k].n):
      test_corpus(strata[k].seed,strata[k].n,&p,&r);
  if(!ok)return 1;
  pos+=p;rej+=r;
  if(k==2||k==3)printf("%s cases=%u specialized=pass\n",strata[k].name,strata[k].n);
  else printf("%s cases=%u positive=%u blocked_or_pending=%u\n",strata[k].name,strata[k].n,p,r);
 }
 printf("total_cases=%u sampled_positive=%u sampled_other=%u assertions=%u cpu_seconds=%.6f\n",5248u,pos,rej,checks,(double)(clock()-start)/CLOCKS_PER_SEC);
 return 0;
}
