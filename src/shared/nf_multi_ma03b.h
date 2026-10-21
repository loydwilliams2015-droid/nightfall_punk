#ifndef NF_MULTI_MA03B_H
#define NF_MULTI_MA03B_H
#include "nf_multi_ma03.h"
/* Independent 1D unequal-mass, zero-restitution impact reference. */
typedef struct {double x[3],v[3];unsigned impacts;bool complete;} NfMa03bChain;
NfMa03bChain nf_ma03b_oracle(const double x[3],const double v[3],
 const double mass[3],double radius,double dt);
/* Check provider twice, validate owner on both snapshots, and require
 * matching B2 decision identity/status. No durable publish is implied. */
Nf18b2UseDecision nf_ma023_recheck(const NfWorld *w,const Nf18a6Runtime *r,
 NfMa03Provider provider,void *ctx,uint32_t action,Nf18b2Model model);
#endif
