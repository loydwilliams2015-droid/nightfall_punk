#ifndef NF_MULTI_NEXT2_H
#define NF_MULTI_NEXT2_H
#include "nf_multi_c1_next.h"
#include "nf_eligibility18b2.h"
#define NF_MA01C2_MAX_EVENTS 8u
typedef struct {
 NfNarrowStatus status;
 Nf18a4Body bodies[NF18A4_MAX_BODIES];
 size_t count;
 uint32_t impacts;
 float advanced_seconds;
} NfMa01c2Result;
/* Serial single-owner reference; horizontal upright equal-center capsule
 * pairs only. No live A6 publication, no A5/B3/B5 durability. */
NfMa01c2Result nf_ma01c2_integrate(const Nf18a4Body *bodies,size_t n,float dt,
 uint32_t tick,uint32_t revision);
/* Scene is returned by SERVER provider, never by a client input. The B2
 * evaluator itself reads A5 canonical/fine witnesses. No authority inferred
 * merely because a pointer was supplied: caller must configure trusted source. */
typedef bool (*NfMa02ServerSnapshot)(void *ctx,Nf18b2Scene *scene,Nf18b2Use *request);
Nf18b2UseDecision nf_ma02_live_evaluate(NfMa02ServerSnapshot provider,void *ctx,
 uint32_t actor,uint32_t action,Nf18b2Model model);
#endif
