#include "nf_report.h"

#include <math.h>
#include <string.h>

static float clamp01(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

static float quantize(float value) {
    return roundf(value / NF_REPORT_POSITION_QUANTUM) * NF_REPORT_POSITION_QUANTUM;
}

static NfVec3 coarse_position(NfVec3 position) {
    position.x = quantize(position.x);
    position.z = quantize(position.z);
    return position;
}

void nf_report_bus_init(NfReportBus *bus) {
    if (bus == NULL) return;
    memset(bus, 0, sizeof(*bus));
    bus->next_id = 1u;
}

bool nf_report_publish(NfReportBus *bus, NfReport report, uint32_t *id_out) {
    if (bus == NULL || report.kind == NF_REPORT_NONE || report.reporter == 0u) return false;
    if (bus->next_id == 0u) bus->next_id = 1u;

    report.id = bus->next_id++;
    if (report.ancestry_id == 0u) report.ancestry_id = report.id;
    if (report.origin == 0u) report.origin = report.reporter;
    if (report.origin_tick == 0u) report.origin_tick = report.issued_tick;
    if (report.deliver_tick < report.issued_tick) report.deliver_tick = report.issued_tick;
    if (report.expiry_tick <= report.deliver_tick) report.expiry_tick = report.deliver_tick + NF_TICK_RATE * 3u;
    report.confidence = clamp01(report.confidence);
    report.coarse_position = coarse_position(report.coarse_position);

    const size_t index = bus->count < NF_REPORT_CAPACITY ? bus->count++ : bus->next;
    bus->reports[index] = report;
    bus->next = (index + 1u) % NF_REPORT_CAPACITY;
    if (id_out != NULL) *id_out = report.id;
    return true;
}

bool nf_report_relay(
    NfReportBus *bus, const NfReport *parent, NfEntityId reporter,
    float confidence_factor, uint64_t issued_tick, uint64_t deliver_tick,
    uint64_t expiry_tick, uint32_t *id_out) {
    if (bus == NULL || parent == NULL || reporter == 0u || parent->kind == NF_REPORT_NONE) return false;
    NfReport relay = *parent;
    relay.id = 0u;
    relay.ancestry_id = parent->ancestry_id != 0u ? parent->ancestry_id : parent->id;
    relay.reporter = reporter;
    relay.confidence = clamp01(parent->confidence * clamp01(confidence_factor) * 0.85f);
    relay.hops = parent->hops < UINT8_MAX ? (uint8_t)(parent->hops + 1u) : UINT8_MAX;
    relay.issued_tick = issued_tick;
    relay.deliver_tick = deliver_tick;
    relay.expiry_tick = expiry_tick;
    return nf_report_publish(bus, relay, id_out);
}

bool nf_report_is_live(const NfReport *report, uint64_t now_tick) {
    if (report == NULL || report->kind == NF_REPORT_NONE || report->id == 0u) return false;
    return now_tick >= report->deliver_tick && now_tick <= report->expiry_tick;
}

bool nf_report_scope_allows(
    const NfReport *report, NfEntityId recipient, NfFaction faction) {
    if (report == NULL) return false;
    switch (report->scope) {
        case NF_REPORT_SCOPE_PRIVATE:
            return recipient == report->reporter;
        case NF_REPORT_SCOPE_CREW:
            return faction == NF_FACTION_PLAYER || faction == NF_FACTION_TEAMMATE;
        case NF_REPORT_SCOPE_RIVAL_CREW:
            return faction == NF_FACTION_RIVAL;
        case NF_REPORT_SCOPE_PUBLIC:
            return faction != NF_FACTION_NONE;
        default:
            return false;
    }
}

float nf_report_weight(const NfReport *report, uint64_t now_tick) {
    if (!nf_report_is_live(report, now_tick)) return 0.0f;
    const uint64_t lifetime = report->expiry_tick > report->origin_tick
        ? report->expiry_tick - report->origin_tick
        : 1u;
    const uint64_t age = now_tick > report->origin_tick ? now_tick - report->origin_tick : 0u;
    const float freshness = clamp01(1.0f - (float)age / (float)lifetime);
    const float hop_penalty = 1.0f / (1.0f + 0.18f * (float)report->hops);
    return clamp01(report->confidence) * (0.35f + 0.65f * freshness) * hop_penalty;
}

bool nf_report_best(
    const NfReportBus *bus, NfEntityId recipient, NfFaction faction,
    uint64_t now_tick, NfReportKind kind, uint32_t subject_key,
    NfReport *out) {
    if (bus == NULL || out == NULL || kind == NF_REPORT_NONE) return false;
    float best_weight = 0.0f;
    const NfReport *best = NULL;
    for (size_t i = 0u; i < bus->count; ++i) {
        const NfReport *report = &bus->reports[i];
        if (report->kind != kind) continue;
        if (subject_key != 0u && report->subject_key != subject_key) continue;
        if (!nf_report_scope_allows(report, recipient, faction)) continue;
        const float weight = nf_report_weight(report, now_tick);
        if (weight > best_weight) {
            best_weight = weight;
            best = report;
        }
    }
    if (best == NULL) return false;
    *out = *best;
    return true;
}

size_t nf_report_live_count(const NfReportBus *bus, uint64_t now_tick) {
    if (bus == NULL) return 0u;
    size_t count = 0u;
    for (size_t i = 0u; i < bus->count; ++i) {
        if (nf_report_is_live(&bus->reports[i], now_tick)) ++count;
    }
    return count;
}

const char *nf_report_kind_name(NfReportKind kind) {
    switch (kind) {
        case NF_REPORT_ENEMY_POSITION: return "ENEMY_POSITION";
        case NF_REPORT_ROUTE_STATE: return "ROUTE_STATE";
        case NF_REPORT_RESOURCE_STATE: return "RESOURCE_STATE";
        default: return "NONE";
    }
}

const char *nf_report_scope_name(NfReportScope scope) {
    switch (scope) {
        case NF_REPORT_SCOPE_PRIVATE: return "PRIVATE";
        case NF_REPORT_SCOPE_CREW: return "CREW";
        case NF_REPORT_SCOPE_RIVAL_CREW: return "RIVAL_CREW";
        case NF_REPORT_SCOPE_PUBLIC: return "PUBLIC";
        default: return "UNKNOWN";
    }
}
