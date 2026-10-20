#ifndef NF_MULTI_C1_NEXT_H
#define NF_MULTI_C1_NEXT_H
#include "nf_multi_narrow18c1.h"
/* Research-only conservative sphere-equivalent actor capsule contact:
 * ONLY upright capsules with equal vertical centers and horizontal motion.
 * Unsupported geometries stay PENDING, never guessed. */
typedef struct {NfNarrowStatus status;bool hit;float toi;Nf18a4Constraint contact;} NfMultiCcd;
NfMultiCcd nf_multi_capsule_ccd18c1(const Nf18a4Body *a,const Nf18a4Body *b,float dt);
typedef enum {NF_MA02_ALLOWED,NF_MA02_PENDING,NF_MA02_DENIED} NfMa02Result;
typedef struct {uint32_t actor_id,revision,contract_id;bool authenticated,material_known,material_free,contract_grants;} NfMa02Witness;
NfMa02Result nf_ma02_eligibility(uint32_t actor_id,uint32_t revision,const NfMa02Witness *w);
#endif
