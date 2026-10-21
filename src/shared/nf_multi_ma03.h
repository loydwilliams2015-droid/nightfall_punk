#ifndef NF_MULTI_MA03_H
#define NF_MULTI_MA03_H
#include "nf_multi_next2.h"
#include "nf_embody18a6.h"
/* Independent 1-D equal-mass elastic chain oracle, three upright bodies.
 * This reference is NOT the A4 solver and has no live game authority. */
typedef struct {float x[3],v[3];unsigned impacts;bool complete;} NfMa03Chain;
NfMa03Chain nf_ma03_chain_oracle(const float x[3],const float v[3],
                                float radius,float dt);
/* Bound a B2 scene to a REAL A6-owned world and physical runtime; B2
 * credentials and inventory still originate with the named server provider.
 * No additional movement owner, no publication. */
typedef bool (*NfMa03Provider)(void *,const NfWorld *,const Nf18a6Runtime *,
                               uint32_t,Nf18b2Scene *,Nf18b2Use *);
Nf18b2UseDecision nf_ma03_b2_owner_evaluate(const NfWorld *,
 const Nf18a6Runtime *,NfMa03Provider,void *,uint32_t action,Nf18b2Model);
#endif
