#ifndef NF_UCW18C1_H
#define NF_UCW18C1_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
/* A bounded C11/POSIX H0 transaction laboratory. No production A6 hook yet. */
#define UCW_MAX_ACTIONS 32u
#define UCW_MAX_FRONTIER 32u
#define UCW_DISK_BYTES 64u
typedef enum {UCW_RESOLVED,UCW_PROVISIONAL,UCW_PENDING,UCW_QUARANTINED} UcwResolution;
typedef enum {UCW_OK,UCW_DUPLICATE,UCW_STALE,UCW_CONFLICT,UCW_FULL,UCW_INVALID,UCW_IO,UCW_UNCERTAIN} UcwStatus;
typedef struct {
 uint32_t actor,action,object,contract,authority_epoch;
 int32_t material_delta,contract_delta;
 uint32_t read_revision,priority;
} UcwProposal;
typedef struct {
 uint32_t tick,revision,count;
 int64_t material,contracts;
 uint64_t history_hash;
 uint32_t pending_frontier;
 UcwResolution resolution;
 UcwProposal receipts[UCW_MAX_ACTIONS];
} UcwWorld;
typedef struct {
 UcwWorld staged;
 bool ready;
} UcwCandidate;
typedef struct {
 int fd;
 bool quarantine;
 UcwWorld state;
} UcwJournal;
void ucw_init(UcwWorld *world,int64_t material,int64_t contracts);
UcwStatus ucw_stage(const UcwWorld *current,const UcwProposal *actions,
                    size_t count,UcwCandidate *out);
UcwStatus ucw_frontier_step(UcwWorld *world,unsigned budget);
UcwStatus ucw_open(UcwJournal *journal,const char *path,int64_t material,int64_t contracts);
UcwStatus ucw_commit(UcwJournal *journal,const UcwCandidate *candidate);
void ucw_close(UcwJournal *journal);
#endif
