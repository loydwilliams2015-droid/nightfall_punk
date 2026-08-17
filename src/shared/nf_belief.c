#include "nf_belief.h"

#include <float.h>
#include <math.h>
#include <string.h>

#define NF_BELIEF_MIN_PRECISION_M 0.25f
#define NF_BELIEF_FORGET_TICKS (NF_TICK_RATE * 10u)
#define NF_BELIEF_DIRECT_CURRENT_TICKS 8u

static float clamp01(float v) {
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}
static float maxf_local(float a, float b) { return a > b ? a : b; }
static float dist_xz(NfVec3 a, NfVec3 b) {
    const float dx = a.x - b.x, dz = a.z - b.z;
    return sqrtf(dx*dx + dz*dz);
}
static float sanitize_precision(float p) {
    return !isfinite(p) || p < NF_BELIEF_MIN_PRECISION_M
        ? NF_BELIEF_MIN_PRECISION_M : p;
}
static bool is_threat_kind(NfBeliefSubjectKind kind) {
    return kind == NF_BELIEF_SUBJECT_ACTOR ||
        kind == NF_BELIEF_SUBJECT_UNKNOWN_THREAT;
}
static bool fresh_direct_actor(
    const NfBeliefHypothesis *h, uint64_t now_tick) {
    return h != NULL && h->active &&
        h->kind == NF_BELIEF_SUBJECT_ACTOR &&
        h->channel == NF_INFO_CHANNEL_DIRECT_VISUAL &&
        now_tick >= h->updated_tick &&
        now_tick - h->updated_tick <= NF_BELIEF_DIRECT_CURRENT_TICKS;
}
static float hypothesis_weight(
    const NfBeliefHypothesis *h, uint64_t now_tick) {
    if (h == NULL || !h->active || now_tick < h->updated_tick) return 0.0f;
    const uint64_t age = now_tick - h->updated_tick;
    if (age > NF_BELIEF_FORGET_TICKS) return 0.0f;
    const float freshness = clamp01(
        1.0f - (float)age / (float)NF_BELIEF_FORGET_TICKS);
    const float precision = 1.0f / (1.0f + h->precision_m * 0.05f);
    return clamp01(h->confidence) * (0.30f + 0.70f*freshness) * precision;
}
static size_t competing_count(
    const NfBeliefStore *store, NfBeliefSubjectKind kind, uint32_t subject_key) {
    size_t n = 0u;
    for (size_t i = 0u; i < NF_BELIEF_CAPACITY; ++i) {
        const NfBeliefHypothesis *h = &store->hypotheses[i];
        if (h->active && h->kind == kind && h->subject_key == subject_key) ++n;
    }
    return n;
}
static NfBeliefHypothesis *weakest_for_subject(
    NfBeliefStore *store, NfBeliefSubjectKind kind, uint32_t subject_key) {
    NfBeliefHypothesis *weakest = NULL;
    for (size_t i = 0u; i < NF_BELIEF_CAPACITY; ++i) {
        NfBeliefHypothesis *h = &store->hypotheses[i];
        if (!h->active || h->kind != kind || h->subject_key != subject_key) continue;
        if (weakest == NULL || h->confidence < weakest->confidence ||
            (h->confidence == weakest->confidence && h->updated_tick < weakest->updated_tick)) {
            weakest = h;
        }
    }
    return weakest;
}
static NfBeliefHypothesis *slot_for_new(NfBeliefStore *store) {
    NfBeliefHypothesis *weakest = &store->hypotheses[0];
    for (size_t i = 0u; i < NF_BELIEF_CAPACITY; ++i) {
        NfBeliefHypothesis *h = &store->hypotheses[i];
        if (!h->active) return h;
        if (h->confidence < weakest->confidence ||
            (h->confidence == weakest->confidence && h->updated_tick < weakest->updated_tick)) {
            weakest = h;
        }
    }
    return weakest;
}
static void bind_nearby_anonymous(
    NfBeliefStore *store, const NfBeliefObservation *o) {
    if (o->kind != NF_BELIEF_SUBJECT_ACTOR || o->identified_actor == 0u) return;
    for (size_t i = 0u; i < NF_BELIEF_CAPACITY; ++i) {
        NfBeliefHypothesis *h = &store->hypotheses[i];
        if (!h->active || h->kind != NF_BELIEF_SUBJECT_UNKNOWN_THREAT) continue;
        const float envelope = maxf_local(2.0f, h->precision_m + o->precision_m);
        if (dist_xz(h->position, o->position) <= envelope) h->active = false;
    }
}

void nf_belief_init(NfBeliefStore *store) {
    if (store == NULL) return;
    memset(store, 0, sizeof(*store));
    store->next_hypothesis_id = 1u;
}

bool nf_belief_observe(
    NfBeliefStore *store, const NfBeliefObservation *observation) {
    if (store == NULL || observation == NULL ||
        observation->kind == NF_BELIEF_SUBJECT_NONE ||
        observation->subject_key == 0u) return false;

    NfBeliefObservation o = *observation;
    o.confidence = clamp01(o.confidence);
    o.precision_m = sanitize_precision(o.precision_m);
    bind_nearby_anonymous(store, &o);

    NfBeliefHypothesis *compatible = NULL;
    for (size_t i = 0u; i < NF_BELIEF_CAPACITY; ++i) {
        NfBeliefHypothesis *h = &store->hypotheses[i];
        if (!h->active || h->kind != o.kind || h->subject_key != o.subject_key) continue;

        if (o.ancestry_id != 0u && h->ancestry_id == o.ancestry_id) {
            if (o.tick >= h->updated_tick) {
                h->updated_tick = o.tick;
                h->last_evidence_id = o.evidence_id;
                if (o.confidence > h->confidence) h->confidence = o.confidence;
                if (o.precision_m > h->precision_m) h->precision_m = o.precision_m;
            }
            return true;
        }

        const float overlap = maxf_local(h->precision_m, o.precision_m) * 1.35f;
        if (dist_xz(h->position, o.position) <= overlap) {
            compatible = h;
            break;
        }
    }

    if (compatible != NULL) {
        const float old_w = clamp01(compatible->confidence);
        const float new_w = clamp01(o.confidence);
        const float denom = old_w + new_w;
        if (denom > 0.0001f) {
            compatible->position.x =
                (compatible->position.x*old_w + o.position.x*new_w) / denom;
            compatible->position.y =
                (compatible->position.y*old_w + o.position.y*new_w) / denom;
            compatible->position.z =
                (compatible->position.z*old_w + o.position.z*new_w) / denom;
        }
        compatible->confidence = clamp01(
            old_w + (1.0f-old_w)*new_w*0.55f);
        compatible->precision_m = fminf(
            compatible->precision_m, o.precision_m);
        compatible->channel = o.channel;
        compatible->identified_actor = o.identified_actor != 0u
            ? o.identified_actor : compatible->identified_actor;
        compatible->last_evidence_id = o.evidence_id;
        if (o.ancestry_id != 0u) compatible->ancestry_id = o.ancestry_id;
        compatible->updated_tick = o.tick;
        if (compatible->observed_tick == 0u) compatible->observed_tick = o.tick;
        return true;
    }

    NfBeliefHypothesis *slot = NULL;
    if (competing_count(store, o.kind, o.subject_key) >= NF_BELIEF_MAX_COMPETING) {
        slot = weakest_for_subject(store, o.kind, o.subject_key);
        if (slot != NULL && slot->confidence > o.confidence) return false;
    } else {
        slot = slot_for_new(store);
    }
    if (slot == NULL) return false;
    if (store->next_hypothesis_id == 0u) store->next_hypothesis_id = 1u;
    *slot = (NfBeliefHypothesis){
        .active = true,
        .hypothesis_id = store->next_hypothesis_id++,
        .kind = o.kind,
        .subject_key = o.subject_key,
        .identified_actor = o.identified_actor,
        .position = o.position,
        .confidence = o.confidence,
        .precision_m = o.precision_m,
        .channel = o.channel,
        .last_evidence_id = o.evidence_id,
        .ancestry_id = o.ancestry_id,
        .observed_tick = o.tick,
        .updated_tick = o.tick
    };
    return true;
}

bool nf_belief_observe_report(
    NfBeliefStore *store, const NfReport *report, uint64_t now_tick) {
    if (store == NULL || report == NULL || !nf_report_is_live(report, now_tick)) return false;

    NfBeliefSubjectKind kind = NF_BELIEF_SUBJECT_NONE;
    NfEntityId actor = 0u;
    switch (report->kind) {
        case NF_REPORT_ACTOR_POSITION:
            kind = NF_BELIEF_SUBJECT_ACTOR;
            actor = (NfEntityId)report->subject_key;
            break;
        case NF_REPORT_ROUTE_STATE: kind = NF_BELIEF_SUBJECT_ROUTE; break;
        case NF_REPORT_RESOURCE_STATE: kind = NF_BELIEF_SUBJECT_RESOURCE; break;
        case NF_REPORT_STASIS_STATE: kind = NF_BELIEF_SUBJECT_STASIS; break;
        default: return false;
    }

    return nf_belief_observe(store, &(NfBeliefObservation){
        .kind = kind,
        .subject_key = report->subject_key,
        .identified_actor = actor,
        .position = report->position,
        .confidence = nf_report_weight(report, now_tick),
        .precision_m = report->precision_m,
        .channel = NF_INFO_CHANNEL_REPORT,
        .evidence_id = report->id,
        .ancestry_id = report->ancestry_id,
        .tick = now_tick
    });
}

void nf_belief_decay(
    NfBeliefStore *store, uint64_t now_tick, float mobility_m_per_second) {
    if (store == NULL) return;
    const float mobility = mobility_m_per_second > 0.0f ? mobility_m_per_second : 0.0f;
    for (size_t i = 0u; i < NF_BELIEF_CAPACITY; ++i) {
        NfBeliefHypothesis *h = &store->hypotheses[i];
        if (!h->active) continue;
        if (now_tick < h->updated_tick) continue;
        const uint64_t age = now_tick - h->updated_tick;
        if (age > NF_BELIEF_FORGET_TICKS || h->confidence < 0.03f) {
            h->active = false;
            continue;
        }
        const float seconds = (float)age / (float)NF_TICK_RATE;
        const bool current_direct = fresh_direct_actor(h, now_tick);
        if (is_threat_kind(h->kind) && !current_direct) {
            h->precision_m = maxf_local(
                h->precision_m, NF_BELIEF_MIN_PRECISION_M + seconds*mobility*0.35f);
        }
    }
}

const NfBeliefHypothesis *nf_belief_best(
    const NfBeliefStore *store, NfBeliefSubjectKind kind,
    uint32_t subject_key, uint64_t now_tick) {
    if (store == NULL || kind == NF_BELIEF_SUBJECT_NONE) return NULL;
    const NfBeliefHypothesis *best = NULL;
    float best_weight = 0.0f;
    for (size_t i = 0u; i < NF_BELIEF_CAPACITY; ++i) {
        const NfBeliefHypothesis *h = &store->hypotheses[i];
        if (!h->active || h->kind != kind) continue;
        if (subject_key != 0u && h->subject_key != subject_key) continue;
        const float w = hypothesis_weight(h, now_tick);
        if (w > best_weight) { best = h; best_weight = w; }
    }
    return best;
}

const NfBeliefHypothesis *nf_belief_best_threat(
    const NfBeliefStore *store, uint64_t now_tick) {
    if (store == NULL) return NULL;

    const NfBeliefHypothesis *direct = NULL;
    for (size_t i = 0u; i < NF_BELIEF_CAPACITY; ++i) {
        const NfBeliefHypothesis *h = &store->hypotheses[i];
        if (!fresh_direct_actor(h, now_tick)) continue;
        if (direct == NULL || h->updated_tick > direct->updated_tick ||
            (h->updated_tick == direct->updated_tick &&
             h->confidence > direct->confidence)) {
            direct = h;
        }
    }
    if (direct != NULL) return direct;

    const NfBeliefHypothesis *best = NULL;
    float best_weight = 0.0f;
    for (size_t i = 0u; i < NF_BELIEF_CAPACITY; ++i) {
        const NfBeliefHypothesis *h = &store->hypotheses[i];
        if (!h->active || !is_threat_kind(h->kind)) continue;
        const float w = hypothesis_weight(h, now_tick);
        if (w > best_weight) { best = h; best_weight = w; }
    }
    return best;
}

size_t nf_belief_active_count(const NfBeliefStore *store) {
    if (store == NULL) return 0u;
    size_t count = 0u;
    for (size_t i = 0u; i < NF_BELIEF_CAPACITY; ++i)
        if (store->hypotheses[i].active) ++count;
    return count;
}

const char *nf_belief_subject_name(NfBeliefSubjectKind kind) {
    switch (kind) {
        case NF_BELIEF_SUBJECT_ACTOR: return "ACTOR";
        case NF_BELIEF_SUBJECT_UNKNOWN_THREAT: return "UNKNOWN_THREAT";
        case NF_BELIEF_SUBJECT_ROUTE: return "ROUTE";
        case NF_BELIEF_SUBJECT_RESOURCE: return "RESOURCE";
        case NF_BELIEF_SUBJECT_STASIS: return "STASIS";
        default: return "NONE";
    }
}
