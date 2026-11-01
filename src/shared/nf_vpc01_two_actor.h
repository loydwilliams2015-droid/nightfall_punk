#ifndef NF_VPC01_TWO_ACTOR_H
#define NF_VPC01_TWO_ACTOR_H
/* VPC-01 primordial two-actor contact reference, a deliberately bounded
 * RESEARCH model. It uses the real A4 impulse solver; it is NOT an A6 world
 * tick, B2 eligibility, B3 receipt, durable commit or production networking. */
#include "nf_contact18a4.h"
#include <stdbool.h>
#include <stdint.h>
#define NF_VPC01_BODY_COUNT 3u
#define NF_VPC01_ACTOR_COUNT 2u
typedef struct {
 Nf18a4Body body[NF_VPC01_BODY_COUNT]; /* actors 0/1, movable pod 2 */
 uint32_t tick,revision,contact_events;
} NfVpc01World;
typedef struct {
 NfVec3 requested[NF_VPC01_ACTOR_COUNT]; /* horizontal input -1..1 */
} NfVpc01Input;
typedef struct {
 NfVpc01World next;
 Nf18a4Receipt contact;
 uint32_t from_tick,from_revision;
 bool contact_applied;
} NfVpc01Candidate;
typedef enum {
 NF_VPC01_COMMITTED=0, NF_VPC01_PENDING=1,
 NF_VPC01_STALE=2, NF_VPC01_INVALID=3
} NfVpc01Status;
void nf_vpc01_init(NfVpc01World *world);
/* Stage ALL bodies from one immutable predecessor; if geometry is ambiguous
 * or multiple simultaneous events occur, fail closed without publication. */
NfVpc01Status nf_vpc01_stage(const NfVpc01World *world,
 const NfVpc01Input *input,float dt,NfVpc01Candidate *candidate);
/* Bounded single-thread laboratory publication (one world assignment). */
NfVpc01Status nf_vpc01_publish(NfVpc01World *world,
 const NfVpc01Candidate *candidate);
#endif
