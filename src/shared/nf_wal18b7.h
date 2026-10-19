#ifndef NF_WAL18B7_H
#define NF_WAL18B7_H
/* POSIX single-process H0 WAL, not A6/world physical integration. */
#include <stdint.h>
#include <stdbool.h>
#define NF4_MAX_RECORDS 64u
#define NF4_RECORD_BYTES 64u
typedef enum {
 NF4_APPLIED=0,NF4_DUPLICATE,NF4_STALE,NF4_INVALID,NF4_FULL,
 NF4_UNCERTAIN,NF4_QUARANTINED,NF4_IO_ERROR
} Nf4Status;
typedef struct {uint32_t actor,action,epoch;int32_t debit;} Nf4Event;
typedef struct {
 uint32_t epoch, count;
 int64_t balance;
 Nf4Event receipts[NF4_MAX_RECORDS];
} Nf4Ledger;
typedef struct {
 int fd;
 Nf4Ledger ledger;
 bool quarantine;
#ifdef NF4_TESTING
 bool fault_after_write;
#endif
} Nf4Wal;
/* Empty file starts with epoch 1 and supplied initial balance; all later opens
   reconstruct from validated on-disk records and must use the same initial balance.
   Returns QUARANTINED for any torn, unknown or contradictory trailing record;
   the valid prefix is inspectable but not editable until reviewed. */
Nf4Status nf4_open(Nf4Wal *wal,const char *path,int64_t initial_balance);
void nf4_close(Nf4Wal *wal);
/* fdatasync happens before local state changes and before APPLIED is returned.
   If fdatasync/write outcome is uncertain, quarantine until recovery.
   A failed call is not evidence that an action failed to persist. */
Nf4Status nf4_apply(Nf4Wal *wal,Nf4Event event);
#endif
