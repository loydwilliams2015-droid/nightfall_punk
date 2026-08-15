#ifndef NF_REPORT_H
#define NF_REPORT_H

#include "nf_world.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NF_REPORT_CAPACITY 64u
#define NF_REPORT_MIN_PRECISION_M 0.25f
#define NF_REPORT_DEFAULT_LIFETIME_TICKS (NF_TICK_RATE * 3u)

typedef enum NfReportKind {
    NF_REPORT_NONE = 0,
    NF_REPORT_ACTOR_POSITION,
    NF_REPORT_ROUTE_STATE,
    NF_REPORT_RESOURCE_STATE,
    NF_REPORT_STASIS_STATE
} NfReportKind;

typedef enum NfReportScope {
    NF_REPORT_SCOPE_PRIVATE = 0,
    NF_REPORT_SCOPE_CREW,
    NF_REPORT_SCOPE_RIVAL_CREW,
    NF_REPORT_SCOPE_PUBLIC
} NfReportScope;

typedef enum NfInformationChannel {
    NF_INFO_CHANNEL_NONE = 0,
    NF_INFO_CHANNEL_DIRECT_VISUAL,
    NF_INFO_CHANNEL_AUDIBLE,
    NF_INFO_CHANNEL_TRACE,
    NF_INFO_CHANNEL_REPORT
} NfInformationChannel;

typedef struct NfReport {
    uint32_t id;
    uint32_t ancestry_id;
    uint32_t subject_key;
    NfReportKind kind;
    NfReportScope scope;
    NfInformationChannel origin_channel;
    NfEntityId origin;
    NfEntityId reporter;
    NfVec3 position;
    float value;
    float confidence;
    float origin_precision_m;
    float precision_m;
    uint8_t hops;
    uint64_t origin_tick;
    uint64_t issued_tick;
    uint64_t deliver_tick;
    uint64_t expiry_tick;
} NfReport;

typedef struct NfReportBus {
    NfReport reports[NF_REPORT_CAPACITY];
    size_t count;
    size_t next;
    uint32_t next_id;
} NfReportBus;

void nf_report_bus_init(NfReportBus *bus);
bool nf_report_publish(NfReportBus *bus, NfReport report, uint32_t *id_out);
bool nf_report_relay(
    NfReportBus *bus,
    const NfReport *parent,
    NfEntityId reporter,
    float confidence_factor,
    float requested_precision_m,
    uint64_t issued_tick,
    uint64_t deliver_tick,
    uint64_t expiry_tick,
    uint32_t *id_out);
bool nf_report_is_live(const NfReport *report, uint64_t now_tick);
bool nf_report_scope_allows(
    const NfReport *report,
    NfEntityId recipient,
    NfFaction faction);
float nf_report_weight(const NfReport *report, uint64_t now_tick);
bool nf_report_best(
    const NfReportBus *bus,
    NfEntityId recipient,
    NfFaction faction,
    uint64_t now_tick,
    NfReportKind kind,
    uint32_t subject_key,
    NfReport *out);
size_t nf_report_live_count(const NfReportBus *bus, uint64_t now_tick);
const char *nf_report_kind_name(NfReportKind kind);
const char *nf_report_scope_name(NfReportScope scope);
const char *nf_information_channel_name(NfInformationChannel channel);

#endif
