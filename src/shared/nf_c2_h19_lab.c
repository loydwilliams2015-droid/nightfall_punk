#include "nf_c2_h19_lab.h"
#include <string.h>
void nf_c2_lease_init(NfC2Lease *l){atomic_init(&l->users,0);atomic_init(&l->closing,false);}
bool nf_c2_lease_acquire(NfC2Lease *l){
 if(!l||atomic_load(&l->closing))return false;
 atomic_fetch_add(&l->users,1);
 if(atomic_load(&l->closing)){atomic_fetch_sub(&l->users,1);return false;}
 return true;
}
void nf_c2_lease_release(NfC2Lease *l){if(l)atomic_fetch_sub(&l->users,1);}
void nf_c2_lease_close(NfC2Lease *l){if(l)atomic_store(&l->closing,true);}
bool nf_c2_lease_drained(const NfC2Lease *l){return l&&atomic_load(&l->closing)&&!atomic_load(&l->users);}
static uint32_t sum(NfC2Frame f){return f.tick^f.owner^f.action^f.a4_receipt^f.a5_digest^f.b3_receipt^0xC219A5B3u;}
bool nf_c2_propose(NfC2Journal *j,NfC2Frame f){
 if(!j||!f.tick||!f.owner||!f.action||!f.a4_receipt||!f.a5_digest||!f.b3_receipt||
    f.a4_receipt!=f.b3_receipt)return false;
 f.committed=0;f.checksum=sum(f);j->proposed=f;return true;
}
bool nf_c2_commit(NfC2Journal *j){
 if(!j||!j->proposed.checksum||j->proposed.checksum!=sum(j->proposed))return false;
 NfC2Frame f=j->proposed;f.committed=1;j->durable=f;
 memset(&j->proposed,0,sizeof j->proposed);return true;
}
bool nf_c2_recover(const NfC2Journal *j,NfC2Frame *out){
 if(!j||!out)return false;
 if(!j->durable.committed||j->durable.checksum!=sum(j->durable))return false;
 *out=j->durable;return true;
}
