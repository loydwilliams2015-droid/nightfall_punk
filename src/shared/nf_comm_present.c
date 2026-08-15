#include "nf_comm_present.h"

#include <float.h>
#include <math.h>
#include <string.h>

static float clamp01(float v) {
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}
static float sanitize_precision(float p) {
    return !isfinite(p) || p < NF_REPORT_MIN_PRECISION_M
        ? NF_REPORT_MIN_PRECISION_M : p;
}
static bool live(const NfCommPresentEvent *e, uint64_t now_tick) {
    return e != NULL && e->active &&
        now_tick >= e->issued_tick && now_tick <= e->expiry_tick;
}
static size_t choose_slot(NfCommPresentBus *bus, uint64_t now_tick) {
    size_t weakest = 0u;
    float weakest_priority = FLT_MAX;
    for (size_t i = 0u; i < NF_COMM_PRESENT_CAP; ++i) {
        NfCommPresentEvent *e = &bus->events[i];
        if (!e->active || e->expiry_tick < now_tick) return i;
        const float age_penalty = e->expiry_tick > now_tick
            ? 1.0f / (1.0f + (float)(e->expiry_tick-now_tick)) : 0.0f;
        const float priority = clamp01(e->urgency) + age_penalty*0.05f;
        if (priority < weakest_priority) {
            weakest_priority = priority;
            weakest = i;
        }
    }
    return weakest;
}

void nf_comm_present_init(NfCommPresentBus *bus) {
    if (bus == NULL) return;
    memset(bus, 0, sizeof(*bus));
    bus->next_id = 1u;
}

bool nf_comm_present_publish(
    NfCommPresentBus *bus, NfCommPresentEvent event, uint32_t *id_out) {
    if (bus == NULL || event.kind == NF_COMM_NONE || event.sender == 0u) return false;
    if (bus->next_id == 0u) bus->next_id = 1u;
    if (event.expiry_tick <= event.issued_tick)
        event.expiry_tick = event.issued_tick + NF_TICK_RATE*3u;
    event.active = true;
    event.id = bus->next_id++;
    event.precision_m = sanitize_precision(event.precision_m);
    event.urgency = clamp01(event.urgency);

    const size_t slot = choose_slot(bus, event.issued_tick);
    bus->events[slot] = event;
    if (id_out != NULL) *id_out = event.id;
    return true;
}

size_t nf_comm_present_collect(
    const NfCommPresentBus *bus, uint64_t now_tick,
    NfCommPresentEvent *out, size_t cap) {
    if (bus == NULL || out == NULL || cap == 0u) return 0u;
    size_t count = 0u;
    for (size_t i = 0u; i < NF_COMM_PRESENT_CAP && count < cap; ++i) {
        if (live(&bus->events[i], now_tick)) out[count++] = bus->events[i];
    }
    return count;
}

bool nf_comm_present_from_report(
    NfCommPresentBus *bus, const NfReport *report,
    NfCommKind kind, uint64_t now_tick, uint32_t *id_out) {
    if (bus == NULL || report == NULL || !nf_report_is_live(report, now_tick))
        return false;
    return nf_comm_present_publish(bus, (NfCommPresentEvent){
        .kind = kind,
        .sender = report->reporter,
        .subject_key = report->subject_key,
        .position = report->position,
        .precision_m = report->precision_m,
        .urgency = nf_report_weight(report, now_tick),
        .issued_tick = now_tick,
        .expiry_tick = report->expiry_tick
    }, id_out);
}

const char *nf_comm_kind_name(NfCommKind kind) {
    switch (kind) {
        case NF_COMM_MARK_LOCATION: return "LOCATION";
        case NF_COMM_MARK_DANGER: return "DANGER";
        case NF_COMM_MARK_ATTACK: return "ATTACK";
        case NF_COMM_MARK_GUARD: return "GUARD";
        case NF_COMM_MARK_WATCH: return "WATCH";
        case NF_COMM_MARK_REGROUP: return "REGROUP";
        case NF_COMM_MARK_RESOURCE: return "RESOURCE";
        case NF_COMM_MARK_WITHDRAW: return "WITHDRAW";
        case NF_COMM_MARK_ASSIST: return "ASSIST";
        case NF_COMM_ACK_SEEN: return "SEEN";
        case NF_COMM_ACK_COMMITTED: return "ON_IT";
        case NF_COMM_ACK_REJECTED: return "CANT_HOLDING";
        case NF_COMM_SPEECH: return "SPEECH";
        default: return "NONE";
    }
}
