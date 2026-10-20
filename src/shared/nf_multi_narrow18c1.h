#ifndef NF_MULTI_NARROW18C1_H
#define NF_MULTI_NARROW18C1_H
#include "nf_multi_broad18c1.h"
/* H1: capsule-vs-translating AABB only; no capsule-capsule or box-box CCD. */
typedef enum {NF_NARROW_OK=0,NF_NARROW_PENDING,NF_NARROW_INVALID} NfNarrowStatus;
typedef struct {NfNarrowStatus status;size_t count;Nf18a4Constraint constraints[NF18A4_MAX_CONSTRAINTS];uint32_t considered,unsupported;} NfNarrowResult;
NfNarrowResult nf_multi_narrow18c1(const NfMulti18c1World *world,const NfMultiBroad *candidates,float dt);
#endif
