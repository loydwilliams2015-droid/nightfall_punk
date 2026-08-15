#ifndef NF_COMM_PRESENT_H
#define NF_COMM_PRESENT_H

#include "nf_report.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_COMM_PRESENT_CAP 32u

typedef enum NfCommKind {
    NF_COMM_NONE = 0,
    NF_COMM_MARK_LOCATION,
    NF_COMM_MARK_DANGER,
    NF_COMM_MARK_ATTACK,
    NF_COMM_MARK_GUARD,
    NF_COMM_MARK_WATCH,
    NF_COMM_MARK_REGROUP,
    NF_COMM_MARK_RESOURCE,
    NF_COMM_MARK_WITHDRAW,
    NF_COMM_MARK_ASSIST,
    NF_COMM_ACK_SEEN,
    NF_COMM_ACK_COMMITTED,
    NF_COMM_ACK_REJECTED,
    NF_COMM_SPEECH
} NfCommKind;

typedef struct NfCommPresentEvent {
    bool active;
    uint32_t id;
    NfCommKind kind;
    NfEntityId sender;
    uint32_t subject_key;
    NfVec3 position;
    float precision_m;
    float urgency;
    uint64_t issued_tick;
    uint64_t expiry_tick;
} NfCommPresentEvent;

typedef struct NfCommPresentBus {
    NfCommPresentEvent events[NF_COMM_PRESENT_CAP];
    uint32_t next_id;
} NfCommPresentBus;

void nf_comm_present_init(NfCommPresentBus *bus);
bool nf_comm_present_publish(
    NfCommPresentBus *bus, NfCommPresentEvent event, uint32_t *id_out);
size_t nf_comm_present_collect(
    const NfCommPresentBus *bus, uint64_t now_tick,
    NfCommPresentEvent *out, size_t cap);
bool nf_comm_present_from_report(
    NfCommPresentBus *bus, const NfReport *report,
    NfCommKind kind, uint64_t now_tick, uint32_t *id_out);
const char *nf_comm_kind_name(NfCommKind kind);

#endif
