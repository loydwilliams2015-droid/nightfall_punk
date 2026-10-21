#ifndef NF_MA03E_H
#define NF_MA03E_H
#include "nf_multi_ma03d.h"
/* Supported off-axis horizontal capsule pair; simultaneous groups PENDING. */
typedef struct {NfNarrowStatus status;Nf18a4Body a,b;float toi;unsigned impulses;} NfMa03ePair;
NfMa03ePair nf_ma03e_offaxis(Nf18a4Body a,Nf18a4Body b,float dt,uint32_t tick,uint32_t version);
/* Hook adapter: owner calls this during actual A6 prepublish; no second owner. */
typedef struct {Nf18a6Runtime *runtime;NfMa03Provider provider;void *provider_ctx;
 NfMa024Receipt expected;Nf18b2Model model;uint32_t action;} NfMa026Gate;
bool nf_ma026_prepublish(const NfWorld *world,const Nf18a5Integrated *candidate,void *context);
#endif
