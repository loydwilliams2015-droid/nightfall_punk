#include "nf_multi18b7.h"
#include "nf_hf18b4.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static uint64_t checks;
#define CK(x) do{if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return false;}++checks;}while(0)
static bool publish(void *c,const NfM7World *w){*(NfM7World*)c=*w;return true;}
static bool reject(void *c,const NfM7World *w){(void)c;(void)w;return false;}
static NfM7World setup(uint8_t capacity){
 NfM7World w;nf_m7_init(&w);
 for(uint32_t a=1;a<=4;++a){NfM7Actor actor={a,100+a,1};assert(nf_m7_add_actor(&w,actor));}
 NfM7Slot one={.id=10,.revision=1,.capacity=capacity};
 NfM7Slot two={.id=20,.revision=1,.capacity=1};
 assert(nf_m7_add_slot(&w,one));assert(nf_m7_add_slot(&w,two));
 return w;
}
static NfM7Request req(const NfM7World *w,uint32_t actor,uint32_t action,
                       uint32_t slot,NfM7Operation op){
 NfM7Request r={0};r.actor=actor;r.session=100+actor;r.action=action;r.slot=slot;
 r.epoch=w->epoch;r.slot_revision=slot==10?w->slots[0].revision:w->slots[1].revision;
 r.operation=op;r.physical=NF_M7_YES;r.contract=NF_M7_YES;r.consent=1;return r;
}
static bool check_main_cases(void){
 NfM7World w=setup(1),disk=w;NfM7Status st[4];
 NfM7Request q[3]={req(&w,3,3,10,NF_M7_CLAIM),
                   req(&w,1,1,10,NF_M7_CLAIM),
                   req(&w,2,2,10,NF_M7_CLAIM)};
 CK(nf_m7_step(&w,q,3,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_CONTESTED&&st[1]==NF_M7_GRANTED&&st[2]==NF_M7_CONTESTED);
 CK(w.slots[0].occupant_count==1&&w.slots[0].occupant[0]==1);
 CK(w.receipt_count==1&&w.epoch==2&&w.slots[0].revision==2);
 CK(memcmp(&w,&disk,sizeof w)==0);
 NfM7World before=w;
 CK(nf_m7_step(&w,&q[1],1,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_DUPLICATE&&memcmp(&w,&before,sizeof w)==0);
 q[1].slot=20;
 CK(nf_m7_step(&w,&q[1],1,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_INVALID&&memcmp(&w,&before,sizeof w)==0);
 w=setup(2);disk=w;
 q[0]=req(&w,3,3,10,NF_M7_CLAIM);q[1]=req(&w,1,1,10,NF_M7_CLAIM);
 q[2]=req(&w,2,2,10,NF_M7_CLAIM);
 CK(nf_m7_step(&w,q,3,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_CONTESTED&&st[1]==NF_M7_GRANTED&&st[2]==NF_M7_GRANTED);
 CK(w.slots[0].occupant_count==2&&w.slots[0].occupant[0]==1&&w.slots[0].occupant[1]==2);
 CK(w.slots[0].revision==2&&w.receipt_count==2);
 before=w;
 NfM7Request release=req(&w,1,4,10,NF_M7_RELEASE);
 NfM7Request claim=req(&w,3,5,10,NF_M7_CLAIM);
 NfM7Request two[2]={release,claim};
 CK(nf_m7_step(&w,two,2,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_GRANTED&&st[1]==NF_M7_CONTESTED);
 CK(w.slots[0].occupant_count==1&&w.slots[0].occupant[0]==2);
 CK(w.receipt_count==3&&w.epoch==before.epoch+1);
 CK(w.slots[0].revision==before.slots[0].revision+1);
 return true;
}
static bool check_atomic_identity(void){
 NfM7World w=setup(2),disk=w,before=w;NfM7Status st[3];
 NfM7Request r=req(&w,1,1,10,NF_M7_CLAIM);
 CK(nf_m7_step(&w,&r,1,NF_M7_FIXED_ID,reject,NULL,st));
 CK(st[0]==NF_M7_PUBLISH_FAILED&&memcmp(&w,&before,sizeof w)==0);
 r.physical=NF_M7_UNKNOWN;CK(nf_m7_step(&w,&r,1,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_PENDING&&memcmp(&w,&before,sizeof w)==0);
 r.physical=NF_M7_YES;r.contract=NF_M7_NO;
 CK(nf_m7_step(&w,&r,1,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_REFUSED&&memcmp(&w,&before,sizeof w)==0);
 r.contract=NF_M7_YES;r.consent=0;
 CK(nf_m7_step(&w,&r,1,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_REFUSED&&memcmp(&w,&before,sizeof w)==0);
 r.consent=1;r.session++;
 CK(nf_m7_step(&w,&r,1,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_STALE&&memcmp(&w,&before,sizeof w)==0);
 r.session--;r.epoch++;
 CK(nf_m7_step(&w,&r,1,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_STALE);
 r.epoch--;r.slot_revision++;
 CK(nf_m7_step(&w,&r,1,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_STALE);
 r.slot_revision--;
 NfM7Request same[2]={r,r};same[1].action++;
 CK(nf_m7_step(&w,same,2,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_INVALID&&st[1]==NF_M7_INVALID);
 CK(memcmp(&w,&before,sizeof w)==0);
 w.epoch=UINT32_MAX;r=req(&w,1,7,10,NF_M7_CLAIM);before=w;
 CK(nf_m7_step(&w,&r,1,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_FULL&&memcmp(&w,&before,sizeof w)==0);
 w=setup(2);w.slots[0].revision=UINT32_MAX;r=req(&w,1,7,10,NF_M7_CLAIM);before=w;
 CK(nf_m7_step(&w,&r,1,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_FULL&&memcmp(&w,&before,sizeof w)==0);
 w=setup(2);w.receipt_count=NF_M7_RECEIPTS;
 for(uint32_t j=0;j<NF_M7_RECEIPTS;++j){
  NfM7Request old=req(&w,3,200+j,20,NF_M7_CLAIM);
  w.receipts[j]=(NfM7Receipt){old,1};
 }
 CK(nf_m7_valid(&w));r=req(&w,1,7,10,NF_M7_CLAIM);before=w;
 CK(nf_m7_step(&w,&r,1,NF_M7_FIXED_ID,publish,&disk,st));
 CK(st[0]==NF_M7_FULL&&memcmp(&w,&before,sizeof w)==0);
 return true;
}
static bool check_permutation_and_client(void){
 NfM7World a=setup(2),b=a,da=a,db=b;
 NfM7Request q[3]={req(&a,3,3,10,NF_M7_CLAIM),
  req(&a,1,1,10,NF_M7_CLAIM),req(&a,2,2,10,NF_M7_CLAIM)};
 NfM7Request p[3]={q[2],q[0],q[1]};NfM7Status x[3],y[3];
 CK(nf_m7_step(&a,q,3,NF_M7_ROTATING,publish,&da,x));
 CK(nf_m7_step(&b,p,3,NF_M7_ROTATING,publish,&db,y));
 CK(memcmp(&a,&b,sizeof a)==0);
 CK(x[0]==y[1]&&x[1]==y[2]&&x[2]==y[0]);
 /* HF client state cannot write the authoritative world. */
 NfHfClient hf={.actor=1,.action=1};
 NfM7World unchanged=a;
 CK(nf_hf_input(&hf,NF_HF_INPUT_PRESS,5));
 CK(nf_hf_server(&hf,NF_HF_SERVER_PENDING,5));
 CK(nf_hf_server(&hf,NF_HF_SERVER_APPLIED,5));
 CK(!nf_hf_server(&hf,NF_HF_SERVER_CONTESTED,5));
 CK(!nf_hf_server(&hf,NF_HF_SERVER_REFUSED,4));
 CK(hf.state==NF_HF_APPLIED&&memcmp(&a,&unchanged,sizeof a)==0);
 CK(nf_hf_server(&hf,NF_HF_SERVER_INTERRUPTED,5));
 CK(hf.state==NF_HF_INTERRUPTED&&memcmp(&a,&unchanged,sizeof a)==0);
 return true;
}
static bool test_fairness(void){
 NfM7World w=setup(1),disk=w;NfM7Status st[2];
 for(unsigned round=0;round<8;++round){
  NfM7Request q[2]={req(&w,1,10+round*4,10,NF_M7_CLAIM),
                    req(&w,2,11+round*4,10,NF_M7_CLAIM)};
  CK(nf_m7_step(&w,q,2,NF_M7_ROTATING,publish,&disk,st));
  const uint32_t winner=1+(round%2);
  CK(w.slots[0].occupant_count==1&&w.slots[0].occupant[0]==winner);
  CK(st[winner-1]==NF_M7_GRANTED&&st[2-winner]==NF_M7_CONTESTED);
  NfM7Request release=req(&w,winner,12+round*4,10,NF_M7_RELEASE);
  CK(nf_m7_step(&w,&release,1,NF_M7_ROTATING,publish,&disk,st));
  CK(st[0]==NF_M7_GRANTED&&w.slots[0].occupant_count==0);
 }
 CK(w.receipt_count==16);
 return true;
}
static uint32_t random32(uint32_t *s){
 *s^=*s<<13;*s^=*s>>17;*s^=*s<<5;return *s;
}
/* A separately written small reference evaluator for initial empty scopes.
   It does not invoke the implementation's classification or priority code. */
static void oracle(const NfM7World *w,const NfM7Request q[3],NfM7Status out[3]){
 unsigned seats=w->slots[0].capacity;
 for(unsigned i=0;i<3;++i){
  out[i]=NF_M7_GRANTED;
  if(q[i].session!=100+q[i].actor||q[i].epoch!=w->epoch||
     q[i].slot_revision!=w->slots[0].revision)out[i]=NF_M7_STALE;
  else if(q[i].physical==NF_M7_UNKNOWN||q[i].contract==NF_M7_UNKNOWN)
   out[i]=NF_M7_PENDING;
  else if(q[i].physical==NF_M7_NO||q[i].contract==NF_M7_NO||!q[i].consent)
   out[i]=NF_M7_REFUSED;
 }
 for(unsigned i=0;i<3;++i)if(out[i]==NF_M7_GRANTED){
  if(seats)seats--;else out[i]=NF_M7_CONTESTED;
 }
}
static bool corpus(uint32_t seed,unsigned cases,unsigned mode,
                   unsigned *approved,unsigned *denied){
 for(unsigned i=0;i<cases;++i){
  uint32_t n=random32(&seed);
  NfM7World w=setup((uint8_t)(1+(n&1u))),disk=w,before=w;
  NfM7Request q[3];
  for(unsigned a=0;a<3;++a){
   q[a]=req(&w,a+1,100+i*3+a,10,NF_M7_CLAIM);
   uint32_t bits=n>>(a*5);
   if(bits&1u)q[a].physical=NF_M7_UNKNOWN;
   if(bits&2u)q[a].contract=NF_M7_NO;
   if(bits&4u)q[a].consent=0;
   if(bits&8u)q[a].epoch++;
   if(bits&16u)q[a].slot_revision++;
  }
  if(mode==1){
   /* Pathological overlapping action identities must fail closed. */
   q[1].actor=q[0].actor;q[1].session=q[0].session;q[1].action=q[0].action;
  }
  if(mode==2){ /* Overflow/old-session boundaries on one proposal. */
   if(i%2==0){w.epoch=UINT32_MAX;for(unsigned a=0;a<3;++a)q[a].epoch=UINT32_MAX;}
   else q[2].session++;
  }
  before=w;disk=w;
  NfM7Status got[3],want[3];CK(nf_m7_step(&w,q,3,NF_M7_FIXED_ID,publish,&disk,got));
  if(mode==0){
   oracle(&before,q,want);
   for(unsigned a=0;a<3;++a)CK(got[a]==want[a]);
  }else if(mode==1){
   CK(got[0]==NF_M7_INVALID&&got[1]==NF_M7_INVALID);
   CK(got[2]!=NF_M7_DUPLICATE&&got[2]!=NF_M7_INVALID);
  }else{
   if(i%2==0)for(unsigned a=0;a<3;++a)
    CK(got[a]!=NF_M7_GRANTED);
   else CK(got[2]==NF_M7_STALE);
  }
  unsigned n_grants=0;
  for(unsigned a=0;a<3;++a)if(got[a]==NF_M7_GRANTED)n_grants++;
  CK(n_grants<=before.slots[0].capacity);
  CK(w.slots[0].occupant_count==n_grants);
  CK(w.receipt_count==n_grants);
  CK(n_grants?memcmp(&w,&disk,sizeof w)==0:memcmp(&w,&before,sizeof w)==0);
  CK(nf_m7_valid(&w));
  *approved+=n_grants;*denied+=3-n_grants;
 }
 return true;
}
int main(void){
 clock_t start=clock();unsigned approved=0,denied=0;
 if(!check_main_cases()||!check_atomic_identity()||
    !check_permutation_and_client()||!test_fairness())return 1;
 struct {const char *name;uint32_t seed;unsigned count,mode;} stratum[]={
  {"canonical",0x3A010001u,128,0},
  {"randomized",0x3A010002u,2048,0},
  {"pathological",0x3A010003u,512,1},
  {"boundary",0x3A010004u,512,2},
  {"heldout",0xB4187A11u,2048,0}};
 for(unsigned j=0;j<5;++j){
  if(!corpus(stratum[j].seed,stratum[j].count,stratum[j].mode,&approved,&denied))return 1;
  printf("%s=%u cases PASS\n",stratum[j].name,stratum[j].count);
 }
 printf("total=%u grants=%u others=%u assertions=%llu cpu_seconds=%.6f\n",
  5248,approved,denied,(unsigned long long)checks,(double)(clock()-start)/CLOCKS_PER_SEC);
 return 0;
}
