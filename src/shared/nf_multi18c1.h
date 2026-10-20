#ifndef NF_MULTI18C1_H
#define NF_MULTI18C1_H
#include "nf_contact18a4.h"
/* Experimental ONE-owner multi-body A4 island; not bound to live A6.
 * Input is immutable; all effects staged and published together. */
typedef struct {
 uint32_t tick,revision;
 size_t count;
 Nf18a4Body bodies[NF18A4_MAX_BODIES];
} NfMulti18c1World;
typedef struct {
 uint32_t actor,action,revision;
 Nf18a4Motor motor;
} NfMulti18c1Proposal;
typedef struct {
 NfMulti18c1World successor;
 Nf18a4IslandResult contact;
 uint32_t applied_motors;
} NfMulti18c1Candidate;
typedef enum {NF_MULTI_COMMITTED,NF_MULTI_PENDING,NF_MULTI_STALE,
 NF_MULTI_CONFLICT,NF_MULTI_INVALID} NfMulti18c1Status;
NfMulti18c1Status nf_multi18c1_stage(const NfMulti18c1World *world,
 const NfMulti18c1Proposal *proposals,size_t n,
 const Nf18a4Constraint *contacts,size_t nc,float dt,
 NfMulti18c1Candidate *candidate);
NfMulti18c1Status nf_multi18c1_publish(NfMulti18c1World *world,
 const NfMulti18c1Candidate *candidate);
#endif
