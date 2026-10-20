#ifndef NF_MULTI_BROAD18C1_H
#define NF_MULTI_BROAD18C1_H
#include "nf_multi18c1.h"
/* Exact conservative swept AABB candidate filter for bounded 8-body island.
 * This is BROADPHASE ONLY: candidate pairs are not collision confirmation.
 * Caller must run shape-specific CCD, A5 material checks, then A4 solve.
 */
typedef struct {uint32_t id_a,id_b;} NfMultiPair;
typedef struct {
 NfMultiPair pairs[NF18A4_MAX_BODIES*(NF18A4_MAX_BODIES-1u)/2u];
 size_t count;
 uint32_t tested_pairs;
} NfMultiBroad;
bool nf_multi_broad18c1(const NfMulti18c1World *world,float dt,NfMultiBroad *out);
#endif
