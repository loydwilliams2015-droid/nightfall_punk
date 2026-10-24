#ifndef NF_C2_H19_LAB_H
#define NF_C2_H19_LAB_H
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
typedef struct {atomic_uint users;atomic_bool closing;} NfC2Lease;
void nf_c2_lease_init(NfC2Lease *);
bool nf_c2_lease_acquire(NfC2Lease *);
void nf_c2_lease_release(NfC2Lease *);
void nf_c2_lease_close(NfC2Lease *);
bool nf_c2_lease_drained(const NfC2Lease *);
typedef struct {uint32_t tick,owner,action,a4_receipt,a5_digest,b3_receipt,checksum;uint8_t committed;} NfC2Frame;
typedef struct {NfC2Frame durable;NfC2Frame proposed;} NfC2Journal;
bool nf_c2_propose(NfC2Journal *,NfC2Frame);
bool nf_c2_commit(NfC2Journal *);
bool nf_c2_recover(const NfC2Journal *,NfC2Frame *);
#endif
