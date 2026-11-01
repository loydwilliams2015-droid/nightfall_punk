#include "nf_retc_external.h"
#include <math.h>
#include <string.h>
bool nf_retc_external_valid(const NfRetcExternalRecord *r){
 if(!r||r->prior_epoch==UINT32_MAX||r->epoch!=r->prior_epoch+1u||
    !r->transaction_id||!r->actor_id||!r->object_id||!r->action_id||
    !r->contract_epoch||!r->committed_receipts||
    !r->account_count||r->account_count>NF18B2_MAX_REQUESTS||
    r->first_account_actor!=r->actor_id)return false;
 return isfinite(r->actor_center.x)&&isfinite(r->actor_center.y)&&
        isfinite(r->actor_center.z)&&isfinite(r->object_center.x)&&
        isfinite(r->object_center.y)&&isfinite(r->object_center.z);
}

bool nf_retc_external_accept(NfRetcExternalCursor *cursor,const NfRetcExternalRecord *record){
 if(!cursor||!nf_retc_external_valid(record))return false;
 if(cursor->epoch==UINT32_MAX||record->epoch!=cursor->epoch+1u||
    record->prior_epoch!=cursor->epoch||
    record->transaction_id<=cursor->transaction_id)return false;
 cursor->epoch=record->epoch;
 cursor->transaction_id=record->transaction_id;
 return true;
}
bool nf_retc_external_capture(const NfRetcPublisher *publisher,NfRetcExternalRecord *out){
 if(!out)return false;
 *out=(NfRetcExternalRecord){0};
 NfRetcReadLease lease={0};
 if(!nf_retc_read_lease_acquire(publisher,&lease))return false;
 NfRetcReadView v={0};
 if(!nf_retc_read_leased_view(&lease,&v)||!v.accepted||
    v.contract->account_count==0||v.contract->account_count>NF18B2_MAX_REQUESTS||
    v.contract->accounts[0].actor_id!=v.accepted->owner_actor){
     nf_retc_read_lease_release(&lease);return false;
 }
 const NfRetcFrame *f=v.accepted;
 NfRetcExternalRecord candidate={0};
 candidate.epoch=v.epoch;candidate.prior_epoch=f->prior_epoch;
 candidate.transaction_id=v.transaction_id;
 candidate.actor_id=f->owner_actor;candidate.object_id=f->object_id;
 candidate.action_id=v.event->action;candidate.contract_epoch=v.event->contract_epoch;
 candidate.world_revision=v.physical->world.revision;
 candidate.material_epoch=v.physical->world.grid.global_revision;
 candidate.committed_receipts=v.physical->committed_receipts;
 candidate.contract_revision=v.contract->revision;
 candidate.account_count=(uint32_t)v.contract->account_count;
 candidate.first_account_actor=v.contract->accounts[0].actor_id;
 candidate.first_account_balance=v.contract->accounts[0].balance;
 candidate.actor_center=v.physical->actor.center;
 candidate.object_center=v.physical->object.center;
 const bool valid=nf_retc_external_valid(&candidate);
 nf_retc_read_lease_release(&lease);
 if(!valid)return false;
 *out=candidate;
 return true;
}

/* H46: explicit width/byte-order; no native struct padding on wire. */
static void ext_put32(uint8_t *p,uint32_t v){
 p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);p[2]=(uint8_t)(v>>16);p[3]=(uint8_t)(v>>24);
}
static void ext_putfloat(uint8_t *p,float f){
 uint32_t bits=0;memcpy(&bits,&f,sizeof bits);ext_put32(p,bits);
}
static uint32_t ext_checksum(const uint8_t *p,size_t n){
 uint32_t h=2166136261u;
 for(size_t i=0;i<n;++i)h=(h^p[i])*16777619u;
 return h;
}
size_t nf_retc_external_encode(uint8_t *out,size_t cap,const NfRetcExternalRecord *r){
 if(!out||cap<NF_RETC_EXTERNAL_WIRE_BYTES||!nf_retc_external_valid(r))return 0;
 ext_put32(out,0x5845464eu); /* ASCII NFEX */
 ext_put32(out+4,1u);
 const uint32_t values[14]={
 r->epoch,r->prior_epoch,r->transaction_id,r->actor_id,r->object_id,
 r->action_id,r->contract_epoch,r->world_revision,r->material_epoch,
 r->committed_receipts,r->contract_revision,r->account_count,
 r->first_account_actor,r->first_account_balance
 };
 for(size_t i=0;i<14u;++i)ext_put32(out+8u+4u*i,values[i]);
 const float xyz[6]={r->actor_center.x,r->actor_center.y,r->actor_center.z,
                     r->object_center.x,r->object_center.y,r->object_center.z};
 for(size_t i=0;i<6u;++i)ext_putfloat(out+64u+4u*i,xyz[i]);
 ext_put32(out+88u,ext_checksum(out,88u));
 return NF_RETC_EXTERNAL_WIRE_BYTES;
}

/* H47: decoder never interprets raw bytes as a native ABI struct. */
static uint32_t ext_get32(const uint8_t *p){
 return (uint32_t)p[0]|((uint32_t)p[1]<<8)|
        ((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
static float ext_getfloat(const uint8_t *p){
 uint32_t bits=ext_get32(p);float value=0;
 memcpy(&value,&bits,sizeof value);return value;
}
bool nf_retc_external_decode(const uint8_t *bytes,size_t size,NfRetcExternalRecord *out){
 if(!out)return false;
 *out=(NfRetcExternalRecord){0};
 if(!bytes||size!=NF_RETC_EXTERNAL_WIRE_BYTES||
    ext_get32(bytes)!=0x5845464eu||ext_get32(bytes+4)!=1u||
    ext_get32(bytes+88)!=ext_checksum(bytes,88u))return false;
 uint32_t x[14];
 for(size_t i=0;i<14u;++i)x[i]=ext_get32(bytes+8u+4u*i);
 NfRetcExternalRecord candidate={0};
 candidate.epoch=x[0];candidate.prior_epoch=x[1];candidate.transaction_id=x[2];
 candidate.actor_id=x[3];candidate.object_id=x[4];candidate.action_id=x[5];
 candidate.contract_epoch=x[6];candidate.world_revision=x[7];
 candidate.material_epoch=x[8];candidate.committed_receipts=x[9];
 candidate.contract_revision=x[10];candidate.account_count=x[11];
 candidate.first_account_actor=x[12];candidate.first_account_balance=x[13];
 candidate.actor_center=(NfVec3){ext_getfloat(bytes+64),ext_getfloat(bytes+68),ext_getfloat(bytes+72)};
 candidate.object_center=(NfVec3){ext_getfloat(bytes+76),ext_getfloat(bytes+80),ext_getfloat(bytes+84)};
 if(!nf_retc_external_valid(&candidate))return false;
 *out=candidate;return true;
}

/* H50: invalid, duplicate or stale records clear out without moving cursor. */
bool nf_retc_external_consume(NfRetcExternalCursor *cursor,
 const uint8_t *bytes,size_t size,NfRetcExternalRecord *out){
 if(!out)return false;
 *out=(NfRetcExternalRecord){0};
 if(!cursor)return false;
 NfRetcExternalRecord record={0};
 if(!nf_retc_external_decode(bytes,size,&record)||
    !nf_retc_external_accept(cursor,&record))return false;
 *out=record;return true;
}
