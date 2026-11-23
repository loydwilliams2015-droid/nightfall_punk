/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "nf_retc_h323_portable.h"
#include <string.h>
#include <limits.h>
static void put(uint8_t *b,size_t i,uint32_t x) {
 size_t k=4u*i;
 b[k]=(uint8_t)x;b[k+1]=(uint8_t)(x>>8);b[k+2]=(uint8_t)(x>>16);b[k+3]=(uint8_t)(x>>24);
}
static uint32_t get(const uint8_t *b,size_t i) {
 size_t k=4u*i;return (uint32_t)b[k]|((uint32_t)b[k+1]<<8)|
    ((uint32_t)b[k+2]<<16)|((uint32_t)b[k+3]<<24);
}
static uint32_t digest(const uint8_t *p,size_t n) {
 uint32_t h=2166136261u;for(size_t i=0;i<n;i++){h^=p[i];h*=16777619u;}return h;
}
static int valid(const NfH323Record *r) {
 return r && r->type==NF_H323_TYPE_COMMIT && r->flags==0u &&
   (r->txn_low||r->txn_high) && r->prior_tick!=UINT32_MAX &&
   r->next_tick==r->prior_tick+1u && r->owner_epoch!=0u &&
   r->actor[0] && r->actor[1] && r->actor[0]!=r->actor[1] &&
   r->object && r->object!=r->actor[0] && r->object!=r->actor[1] &&
   r->action[0] && r->action[1] && r->action[0]!=r->action[1] &&
   r->contact[0] && r->contact[1] && r->contact[0]!=r->contact[1] &&
   r->material_epoch && r->grant[0] && r->grant[1] && r->grant[0]!=r->grant[1] &&
   r->receipt[0] && r->receipt[1] && r->receipt[0]!=r->receipt[1] &&
   r->prior_digest && r->new_digest;
}
int nf_h323_encode(const NfH323Record *r,uint8_t out[NF_H323_BYTES]) {
 if(!valid(r)||!out)return 0;
 uint8_t temp[NF_H323_BYTES]={0};
 const uint32_t v[23]={NF_H323_MAGIC,NF_H323_VERSION,r->type,r->flags,
 r->txn_low,r->txn_high,r->prior_tick,r->next_tick,r->owner_epoch,
 r->actor[0],r->actor[1],r->object,r->action[0],r->action[1],
 r->contact[0],r->contact[1],r->material_epoch,r->grant[0],r->grant[1],
 r->receipt[0],r->receipt[1],r->prior_digest,r->new_digest};
 for(size_t i=0;i<23;i++)put(temp,i,v[i]);
 put(temp,23,digest(temp,92));
 memcpy(out,temp,sizeof temp);return 1;
}
int nf_h323_decode(const uint8_t *data,size_t n,NfH323Record *out) {
 if(!data||!out||n!=NF_H323_BYTES)return 0;
 if(get(data,0)!=NF_H323_MAGIC||get(data,1)!=NF_H323_VERSION||
    get(data,23)!=digest(data,92))return 0;
 NfH323Record r={0};
 r.type=get(data,2);r.flags=get(data,3);
 r.txn_low=get(data,4);r.txn_high=get(data,5);
 r.prior_tick=get(data,6);r.next_tick=get(data,7);r.owner_epoch=get(data,8);
 r.actor[0]=get(data,9);r.actor[1]=get(data,10);r.object=get(data,11);
 r.action[0]=get(data,12);r.action[1]=get(data,13);
 r.contact[0]=get(data,14);r.contact[1]=get(data,15);
 r.material_epoch=get(data,16);r.grant[0]=get(data,17);r.grant[1]=get(data,18);
 r.receipt[0]=get(data,19);r.receipt[1]=get(data,20);
 r.prior_digest=get(data,21);r.new_digest=get(data,22);
 if(!valid(&r))return 0;
 *out=r;return 1;
}
