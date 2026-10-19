#ifndef NF_GOVERNOR18B7_H
#define NF_GOVERNOR18B7_H
/* H0 deterministic admission policy. No timers, threads, async I/O, or
   authority to alter world state. Actual scheduling remains A6-owned. */
#include <stdbool.h>
#include <stdint.h>
typedef enum { NF4G_NORMAL=0, NF4G_THROTTLED=1, NF4G_READ_ONLY=2 } Nf4gMode;
typedef struct {
 uint32_t budget_us, used_us;
 uint32_t critical_backlog, optional_backlog;
 bool telemetry_valid, journal_verified, uncertain_disk;
} Nf4gMetrics;
typedef struct {
 Nf4gMode mode;
 uint8_t healthy_ticks;
 uint32_t granted_optional_us;
} Nf4gState;
void nf4g_init(Nf4gState *);
Nf4gMode nf4g_step(Nf4gState *,Nf4gMetrics);
/* Caller must independently verify all work remains non-authoritative.
   Zero means skip background work, not discard the pending job. */
#endif
