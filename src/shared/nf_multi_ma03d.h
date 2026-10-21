#ifndef NF_MULTI_MA03D_H
#define NF_MULTI_MA03D_H
#include "nf_multi_ma03c.h"
/* Bounded 3-body scheduler challenger. Only collinear, same-height upright
 * capsules with equal radius. All unresolved contact clusters fail closed. */
NfMa01c2Result nf_ma03d_three_body(const Nf18a4Body *b,float dt,uint32_t tick,uint32_t rev);
/* Owner-bound stronger prepublication diagnostic. Not a durable publisher. */
bool nf_ma025_prepublication(const NfWorld *w,const Nf18a6Runtime *r,
 NfMa03Provider source,void *ctx,uint32_t action,Nf18b2Model model,
 const NfMa024Receipt *expected);
#endif
