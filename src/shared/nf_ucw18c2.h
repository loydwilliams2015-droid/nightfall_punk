#ifndef NF_UCW18C2_H
#define NF_UCW18C2_H
#include "nf_ucw18c1.h"
/* H0 full LOGICAL world-envelope laboratory: B3-style action identities,
 * contracts, and two conserved aggregates. Not an A4/A5 rigid-body snapshot.
 */
#define UCW2_RECORD_SIZE (64u+UCW_MAX_ACTIONS*36u)
typedef struct {int fd;bool quarantine;UcwWorld state;} Ucw2Journal;
UcwStatus ucw2_open(Ucw2Journal *j,const char *path,int64_t material,int64_t contracts);
UcwStatus ucw2_commit(Ucw2Journal *j,const UcwCandidate *candidate);
void ucw2_close(Ucw2Journal *j);
#endif
