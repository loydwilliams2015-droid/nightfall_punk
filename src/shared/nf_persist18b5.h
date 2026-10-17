#ifndef NF_PERSIST18B5_H
#define NF_PERSIST18B5_H
#include <stdint.h>
#include <stdbool.h>
#define NF_B5_RECEIPTS 64u
typedef struct { uint32_t actor,action,object,epoch; int32_t debit; } NfB5Event;
typedef struct { uint32_t epoch,count; int64_t balance; NfB5Event receipts[NF_B5_RECEIPTS]; } NfB5State;
typedef enum { NF_B5_APPLIED,NF_B5_DUPLICATE,NF_B5_STALE,NF_B5_FULL,NF_B5_INVALID,NF_B5_PERSIST_FAIL } NfB5Result;
typedef bool (*NfB5Persist)(void *,const NfB5State *);
NfB5Result nf_b5_apply(NfB5State *,NfB5Event,NfB5Persist,void *);
bool nf_b5_equal(const NfB5State *,const NfB5State *);
#endif
