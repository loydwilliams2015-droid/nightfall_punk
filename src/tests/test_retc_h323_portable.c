/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "nf_retc_h323_portable.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <threads.h>
#define CHECK(x) do { ++checks; if(!(x)){++failures; if(failures<5) fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x); } }while(0)
static uint32_t checks=0,failures=0;
static NfH323Record fixture(uint32_t n) {
 NfH323Record r={0};
 r.type=NF_H323_TYPE_COMMIT;r.txn_low=n;r.txn_high=42;
 r.prior_tick=n;r.next_tick=n+1;r.owner_epoch=7;
 r.actor[0]=10;r.actor[1]=20;r.object=2010;
 r.action[0]=91;r.action[1]=92;
 r.contact[0]=101;r.contact[1]=102;
 r.material_epoch=3;r.grant[0]=301;r.grant[1]=302;
 r.receipt[0]=401;r.receipt[1]=402;
 r.prior_digest=0x1234;r.new_digest=0x5678;
 return r;
}
static uint32_t checksum(const uint8_t *b) {
 uint32_t h=2166136261u;for(size_t i=0;i<92;i++){h^=b[i];h*=16777619u;}return h;
}
static void put32(uint8_t *b,size_t idx,uint32_t v){
 size_t i=idx*4;b[i]=(uint8_t)v;b[i+1]=(uint8_t)(v>>8);
 b[i+2]=(uint8_t)(v>>16);b[i+3]=(uint8_t)(v>>24);
}
typedef struct {uint32_t first,last,fail;} Worker;
static int worker(void *p){
 Worker *w=p;uint8_t b[NF_H323_BYTES];
 for(uint32_t i=w->first;i<w->last;i++){
  NfH323Record r=fixture(i+1),v={0};
  if(!nf_h323_encode(&r,b)||!nf_h323_decode(b,sizeof b,&v)||
      v.txn_low!=r.txn_low||v.next_tick!=r.next_tick)w->fail++;
 }return 0;
}
int main(void){
 uint8_t bytes[NF_H323_BYTES]={0},temp[NF_H323_BYTES],initial[NF_H323_BYTES];
 NfH323Record r=fixture(1),decoded={0};
 CHECK(sizeof bytes==96);
 CHECK(nf_h323_encode(&r,bytes));
 CHECK(bytes[0]==0x48&&bytes[1]==0x33&&bytes[2]==0x32&&bytes[3]==0x33);
 CHECK(bytes[4]==1&&bytes[5]==0&&bytes[6]==0&&bytes[7]==0);
 CHECK(nf_h323_decode(bytes,sizeof bytes,&decoded));
 CHECK(decoded.actor[0]==10&&decoded.actor[1]==20&&decoded.object==2010);
 CHECK(decoded.receipt[0]==401&&decoded.receipt[1]==402);
 CHECK(!nf_h323_decode(bytes,sizeof bytes-1,&decoded));
 CHECK(!nf_h323_decode(bytes,sizeof bytes+1,&decoded));
 CHECK(!nf_h323_encode(NULL,bytes));
 CHECK(!nf_h323_encode(&r,NULL));
 CHECK(!nf_h323_decode(NULL,96,&decoded));
 CHECK(!nf_h323_decode(bytes,96,NULL));
 memcpy(initial,bytes,sizeof bytes);
 for(size_t i=0;i<sizeof bytes;i++){
   memcpy(temp,initial,sizeof temp);temp[i]^=0x80;
   CHECK(!nf_h323_decode(temp,sizeof temp,&decoded));
 }
 /* Checksum recomputed adversarial variants: schema and semantic gates must reject. */
 const size_t badfields[]={1,2,3,4,5,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
 const uint32_t badvalues[]={2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 for(size_t i=0;i<sizeof badfields/sizeof badfields[0];i++){
  memcpy(temp,initial,sizeof temp);put32(temp,badfields[i],badvalues[i]);
  put32(temp,23,checksum(temp));
  CHECK(!nf_h323_decode(temp,sizeof temp,&decoded));
 }
 memcpy(temp,initial,sizeof temp);put32(temp,7,99);put32(temp,23,checksum(temp));
 CHECK(!nf_h323_decode(temp,sizeof temp,&decoded));
 memcpy(temp,initial,sizeof temp);put32(temp,10,10);put32(temp,23,checksum(temp));
 CHECK(!nf_h323_decode(temp,sizeof temp,&decoded));
 memcpy(temp,initial,sizeof temp);put32(temp,18,301);put32(temp,23,checksum(temp));
 CHECK(!nf_h323_decode(temp,sizeof temp,&decoded));
 memcpy(temp,initial,sizeof temp);put32(temp,20,401);put32(temp,23,checksum(temp));
 CHECK(!nf_h323_decode(temp,sizeof temp,&decoded));
 /* No output mutation on denial. */
 decoded=fixture(99);memset(temp,0,sizeof temp);
 CHECK(!nf_h323_decode(temp,sizeof temp,&decoded)&&decoded.txn_low==99);
 /* Serial and 4-worker concurrent isolated codec equality; A6 is NOT exercised. */
 uint8_t old[NF_H323_BYTES],again[NF_H323_BYTES];
 for(uint32_t i=1;i<=8192;i++){
  r=fixture(i);CHECK(nf_h323_encode(&r,old));
  CHECK(nf_h323_decode(old,sizeof old,&decoded));
  CHECK(nf_h323_encode(&decoded,again)&&memcmp(old,again,sizeof old)==0);
 }
 thrd_t threads[4];Worker w[4];int started=0;
 for(int i=0;i<4;i++){
  w[i]=(Worker){(uint32_t)(i*2048),(uint32_t)((i+1)*2048),0};
  if(thrd_create(&threads[i],worker,&w[i])!=thrd_success)break;
  started++;
 }
 CHECK(started==4);
 for(int i=0;i<started;i++){int status=-1;
  CHECK(thrd_join(threads[i],&status)==thrd_success&&status==0&&w[i].fail==0);
 }
 printf("H323 portable codec checks=%u failures=%u serial=8192 parallel=8192 workers=4\n",checks,failures);
 return failures?1:0;
}
