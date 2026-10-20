#ifndef NF_UCW18C3_H
#define NF_UCW18C3_H
#include "nf_ucw18c1.h"
#include "nf_contact18a5_close.h"
/* H0 single journal containing actual A5 world snapshot AND UCW contract state.
 * Physical payload is ABI-local, NOT portable/open-ended self-serialization.
 * Each section has independently typed schema; unknown optional sections may
 * be skipped by future decoders, unknown mandatory sections fail closed. */
#define UCW3_MAX_PAYLOAD (16u*1024u*1024u)
#define UCW3_SECTION_A5 1u
#define UCW3_SECTION_CONTRACTS 2u
typedef struct {int fd;bool quarantine;uint32_t generation;
 Nf18a5Integrated physical;UcwWorld contracts;} Ucw3Journal;
typedef struct {Nf18a5Integrated physical;UcwWorld contracts;} Ucw3Snapshot;
UcwStatus ucw3_open(Ucw3Journal *j,const char *path,const Ucw3Snapshot *genesis);
UcwStatus ucw3_commit(Ucw3Journal *j,const Ucw3Snapshot *candidate);
void ucw3_close(Ucw3Journal *j);
#endif
