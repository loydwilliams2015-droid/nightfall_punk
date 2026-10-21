#ifndef NF_MA03C_024_H
#define NF_MA03C_024_H
#include "nf_multi_ma03b.h"
/* Compare actual MA01C2 scheduler with independent analytical trajectory.
 * Only two-body isolated horizontal elastic-free outcomes are admissible;
 * multi-body unresolved results remain PENDING, never promoted. */
typedef struct {unsigned matched,unresolved,disagreed;} NfMa03cEvidence;
NfMa03cEvidence nf_ma03c_compare(const Nf18a4Body *b,size_t n,float dt,uint32_t tick,uint32_t rev);
/* Snapshot binding fingerprint: versioned, noncryptographic diagnostic only.
 * An authentic credential signer and durable atomic B5 writer remain open. */
typedef struct {uint32_t actor,action,tick,world_revision,material_epoch;uint64_t fingerprint;} NfMa024Receipt;
bool nf_ma024_record(const NfWorld *w,const Nf18a6Runtime *r,NfMa03Provider source,void *ctx,
 uint32_t action,Nf18b2Model model,NfMa024Receipt *out);
bool nf_ma024_recheck(const NfWorld *w,const Nf18a6Runtime *r,NfMa03Provider source,void *ctx,
 uint32_t action,Nf18b2Model model,const NfMa024Receipt *expected);
#endif
