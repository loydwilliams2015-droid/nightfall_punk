#include "nf_report.h"

#include <float.h>
#include <math.h>
#include <string.h>

static float clamp01(float value) { return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value); }
static float maxf_local(float a, float b) { return a > b ? a : b; }
static float sanitize_precision(float precision_m) {
    if (!isfinite(precision_m) || precision_m < NF_REPORT_MIN_PRECISION_M) return NF_REPORT_MIN_PRECISION_M;
    return precision_m;
}
static float quantize(float value, float precision_m) { return roundf(value / precision_m) * precision_m; }
static NfVec3 quantize_position(NfVec3 p, float precision_m) {
    p.x = quantize(p.x, precision_m); p.y = quantize(p.y, precision_m); p.z = quantize(p.z, precision_m); return p;
}

bool nf_report_is_live(const NfReport *report, uint64_t now_tick) {
    if (report == NULL || report->kind == NF_REPORT_NONE || report->id == 0u) return false;
    return now_tick >= report->deliver_tick && now_tick <= report->expiry_tick;
}

float nf_report_weight(const NfReport *report, uint64_t now_tick) {
    if (!nf_report_is_live(report, now_tick)) return 0.0f;
    const uint64_t lifetime = report->expiry_tick > report->origin_tick ? report->expiry_tick - report->origin_tick : 1u;
    const uint64_t age = now_tick > report->origin_tick ? now_tick - report->origin_tick : 0u;
    const float freshness = clamp01(1.0f - (float)age / (float)lifetime);
    const float hop_penalty = 1.0f / (1.0f + 0.18f*(float)report->hops);
    const float precision_penalty = 1.0f / (1.0f + 0.08f*maxf_local(0.0f, report->precision_m - NF_REPORT_MIN_PRECISION_M));
    return clamp01(report->confidence) * (0.35f + 0.65f*freshness) * hop_penalty * precision_penalty;
}

static bool redundant_with(const NfReport *a, const NfReport *b) {
    if (a == NULL || b == NULL) return false;
    return a->kind == b->kind && a->subject_key == b->subject_key && a->scope == b->scope &&
        a->reporter == b->reporter && a->ancestry_id != 0u && a->ancestry_id == b->ancestry_id;
}
static size_t choose_slot(const NfReportBus *bus, const NfReport *incoming) {
    if (bus->count < NF_REPORT_CAPACITY) return bus->count;
    const uint64_t now = incoming->issued_tick;
    size_t candidate = 0u; float candidate_weight = FLT_MAX;
    for (size_t i = 0u; i < NF_REPORT_CAPACITY; ++i) {
        const NfReport *r = &bus->reports[i];
        if (r->expiry_tick < now) return i;
        if (redundant_with(r, incoming)) return i;
        const float w = nf_report_weight(r, now);
        if (w < candidate_weight) { candidate_weight = w; candidate = i; }
    }
    return candidate;
}

void nf_report_bus_init(NfReportBus *bus) {
    if (bus == NULL) return;
    memset(bus, 0, sizeof(*bus)); bus->next_id = 1u;
}

bool nf_report_publish(NfReportBus *bus, NfReport report, uint32_t *id_out) {
    if (bus == NULL || report.kind == NF_REPORT_NONE || report.reporter == 0u) return false;
    if (bus->next_id == 0u) bus->next_id = 1u;
    report.id = bus->next_id++;
    if (report.ancestry_id == 0u) report.ancestry_id = report.id;
    if (report.origin == 0u) report.origin = report.reporter;
    if (report.origin_tick == 0u) report.origin_tick = report.issued_tick;
    if (report.deliver_tick < report.issued_tick) report.deliver_tick = report.issued_tick;
    if (report.expiry_tick <= report.deliver_tick) report.expiry_tick = report.deliver_tick + NF_REPORT_DEFAULT_LIFETIME_TICKS;
    report.confidence = clamp01(report.confidence);
    report.origin_precision_m = sanitize_precision(report.origin_precision_m);
    report.precision_m = maxf_local(sanitize_precision(report.precision_m), report.origin_precision_m);
    report.position = quantize_position(report.position, report.precision_m);
    const size_t index = choose_slot(bus, &report);
    bus->reports[index] = report;
    if (bus->count < NF_REPORT_CAPACITY) ++bus->count;
    bus->next = (index + 1u) % NF_REPORT_CAPACITY;
    if (id_out != NULL) *id_out = report.id;
    return true;
}

bool nf_report_relay(
    NfReportBus *bus, const NfReport *parent, NfEntityId reporter,
    float confidence_factor, float requested_precision_m,
    uint64_t issued_tick, uint64_t deliver_tick, uint64_t expiry_tick,
    uint32_t *id_out) {
    if (bus == NULL || parent == NULL || reporter == 0u || parent->kind == NF_REPORT_NONE) return false;
    NfReport relay = *parent;
    relay.id = 0u;
    relay.ancestry_id = parent->ancestry_id != 0u ? parent->ancestry_id : parent->id;
    relay.reporter = reporter;
    relay.confidence = clamp01(parent->confidence * clamp01(confidence_factor) * 0.85f);
    relay.precision_m = maxf_local(sanitize_precision(requested_precision_m), sanitize_precision(parent->precision_m));
    relay.hops = parent->hops < UINT8_MAX ? (uint8_t)(parent->hops + 1u) : UINT8_MAX;
    relay.issued_tick = issued_tick; relay.deliver_tick = deliver_tick; relay.expiry_tick = expiry_tick;
    return nf_report_publish(bus, relay, id_out);
}

bool nf_report_scope_allows(const NfReport *report, NfEntityId recipient, NfFaction faction) {
    if (report == NULL) return false;
    switch (report->scope) {
        case NF_REPORT_SCOPE_PRIVATE: return recipient == report->reporter;
        case NF_REPORT_SCOPE_CREW: return faction == NF_FACTION_PLAYER || faction == NF_FACTION_TEAMMATE;
        case NF_REPORT_SCOPE_RIVAL_CREW: return faction == NF_FACTION_RIVAL;
        case NF_REPORT_SCOPE_PUBLIC: return faction != NF_FACTION_NONE;
        default: return false;
    }
}

bool nf_report_best(
    const NfReportBus *bus, NfEntityId recipient, NfFaction faction,
    uint64_t now_tick, NfReportKind kind, uint32_t subject_key, NfReport *out) {
    if (bus == NULL || out == NULL || kind == NF_REPORT_NONE) return false;
    float best_weight = 0.0f; const NfReport *best = NULL;
    for (size_t i = 0u; i < bus->count; ++i) {
        const NfReport *r = &bus->reports[i];
        if (r->kind != kind) continue;
        if (subject_key != 0u && r->subject_key != subject_key) continue;
        if (!nf_report_scope_allows(r, recipient, faction)) continue;
        const float w = nf_report_weight(r, now_tick);
        if (w > best_weight) { best_weight = w; best = r; }
    }
    if (best == NULL) return false;
    *out = *best; return true;
}

size_t nf_report_live_count(const NfReportBus *bus, uint64_t now_tick) {
    if (bus == NULL) return 0u;
    size_t count = 0u;
    for (size_t i = 0u; i < bus->count; ++i) if (nf_report_is_live(&bus->reports[i], now_tick)) ++count;
    return count;
}

const char *nf_report_kind_name(NfReportKind kind) {
    switch (kind) { case NF_REPORT_ACTOR_POSITION: return "ACTOR_POSITION"; case NF_REPORT_ROUTE_STATE: return "ROUTE_STATE"; case NF_REPORT_RESOURCE_STATE: return "RESOURCE_STATE"; case NF_REPORT_STASIS_STATE: return "STASIS_STATE"; default: return "NONE"; }
}
const char *nf_report_scope_name(NfReportScope scope) {
    switch (scope) { case NF_REPORT_SCOPE_PRIVATE: return "PRIVATE"; case NF_REPORT_SCOPE_CREW: return "CREW"; case NF_REPORT_SCOPE_RIVAL_CREW: return "RIVAL_CREW"; case NF_REPORT_SCOPE_PUBLIC: return "PUBLIC"; default: return "UNKNOWN"; }
}
const char *nf_information_channel_name(NfInformationChannel channel) {
    switch (channel) {
        case NF_INFO_CHANNEL_DIRECT_VISUAL: return "DIRECT_VISUAL";
        case NF_INFO_CHANNEL_AUDIBLE_GUNFIRE: return "AUDIBLE_GUNFIRE";
        case NF_INFO_CHANNEL_IMPACT: return "IMPACT";
        case NF_INFO_CHANNEL_DAMAGE_RECEIVED: return "DAMAGE_RECEIVED";
        case NF_INFO_CHANNEL_TRACE: return "TRACE";
        case NF_INFO_CHANNEL_REPORT: return "REPORT";
        case NF_INFO_CHANNEL_AUDIBLE_SPEECH: return "AUDIBLE_SPEECH";
        default: return "NONE";
    }
}
