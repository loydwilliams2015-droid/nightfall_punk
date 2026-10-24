#ifndef NF_V18C2_H9_AUTH_H
#define NF_V18C2_H9_AUTH_H
#include "nf_v18c2_witness.h"
/* Allocated on A6 bind, invalidated and destroyed on unbind. */
bool nf_c2_h9_owner_init(Nf18a6Runtime *runtime);
void nf_c2_h9_owner_destroy(Nf18a6Runtime *runtime);
/* Owner-held research authorization; cannot be minted by H3 Boolean grant.
 * A trusted server provider is still required for credential provenance. */
NfC2WitnessStatus nf_c2_h9_issue(NfWorld *world,Nf18a6Runtime *runtime,
 NfMa03Provider server_provider,void *server_context,
 uint32_t action,NfVec3 material_point);
/* Consumes exactly once; does not advance world tick or publish B3. */
bool nf_c2_h9_valid(const NfWorld *,const Nf18a6Runtime *,uint32_t action);
bool nf_c2_h9_take_prepublication(const NfWorld *,const Nf18a6Runtime *,uint32_t action);
void nf_c2_h9_revoke(const Nf18a6Runtime *);
bool nf_c2_h9_consume(NfWorld *world,Nf18a6Runtime *runtime,uint32_t action);
#endif
