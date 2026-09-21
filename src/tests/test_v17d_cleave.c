#include "nf_cleave17d.h"

#include <stdio.h>
#include <string.h>

static int failures = 0;

static void expect_true(bool value, const char *name) {
    if (!value) {
        fprintf(stderr, "FAIL: %s\n", name);
        ++failures;
    }
}

static void expect_int(int actual, int expected, const char *name) {
    if (actual != expected) {
        fprintf(stderr, "FAIL: %s actual=%d expected=%d\n", name, actual, expected);
        ++failures;
    }
}

static Nf17bTransaction make_txn(
    uint32_t tick,
    uint32_t target,
    Nf17bTxnKind kind,
    Nf17bComponentMask read_mask,
    Nf17bComponentMask write_mask) {

    Nf17bTransaction txn;
    memset(&txn, 0, sizeof(txn));
    txn.tick = tick;
    txn.target_id = target;
    txn.kind = (uint8_t)kind;
    txn.domain = (uint8_t)NF17B_DOMAIN_MATERIAL;
    txn.read_mask = read_mask;
    txn.write_mask = write_mask;
    txn.precondition_mask = read_mask;
    return txn;
}

int main(void) {
    Nf17dCleavePolicy policy;
    nf17d_policy_default(&policy);
    expect_true(nf17d_policy_is_safe(&policy), "default cleave policy preserves authority invariants");
    expect_int((int)policy.subsystem[NF17D_SUBSYS_CALIBRATION].disposition,
               (int)NF17D_CLEAVE_PROMOTE, "calibration promoted");
    expect_int((int)policy.subsystem[NF17D_SUBSYS_PURPLE].disposition,
               (int)NF17D_CLEAVE_RETAIN_EXPERIMENTAL, "Purple retained experimental");
    expect_int((int)policy.subsystem[NF17D_SUBSYS_DISTRIBUTED_BELIEF_CHALLENGER].disposition,
               (int)NF17D_CLEAVE_DISABLE, "failed belief challenger disabled");
    expect_int((int)policy.subsystem[NF17D_SUBSYS_GLOBAL_PRIORITY_ARBITRATION].disposition,
               (int)NF17D_CLEAVE_PRUNE, "global priority arbitration pruned");

    Nf17bTransaction txns[4];
    txns[0] = make_txn(7u, 4u, NF17B_TXN_OPEN,
        NF17B_COMPONENT_TOPOLOGY, NF17B_COMPONENT_TOPOLOGY);
    txns[1] = make_txn(7u, 4u, NF17B_TXN_CLOSE,
        NF17B_COMPONENT_TOPOLOGY, NF17B_COMPONENT_TOPOLOGY);
    txns[2] = make_txn(7u, 4u, NF17B_TXN_DESTROY,
        NF17B_COMPONENT_TOPOLOGY | NF17B_COMPONENT_MATERIAL,
        NF17B_COMPONENT_TOPOLOGY | NF17B_COMPONENT_MATERIAL);
    txns[3] = make_txn(7u, 99u, NF17B_TXN_DAMAGE,
        NF17B_COMPONENT_MATERIAL, NF17B_COMPONENT_MATERIAL);
    txns[0].scope_id = 44u;
    txns[1].scope_id = 44u;
    txns[2].scope_id = 44u;

    Nf17dConflictSet sets[NF17D_MAX_CONFLICT_SETS];
    const size_t set_count = nf17d_build_conflict_sets(txns, 4u, sets, NF17D_MAX_CONFLICT_SETS);
    expect_int((int)set_count, 1, "only interacting transactions form conflict set");
    expect_int((int)sets[0].count, 3, "transitive conflict component groups three transactions");
    expect_true((sets[0].combined_write_mask & NF17B_COMPONENT_TOPOLOGY) != 0u,
                "conflict set preserves topology write dependency");
    expect_true((sets[0].combined_write_mask & NF17B_COMPONENT_MATERIAL) != 0u,
                "conflict set preserves material write dependency");

    Nf17bTransaction scoped[2];
    scoped[0] = make_txn(8u, 100u, NF17B_TXN_OPEN,
        NF17B_COMPONENT_TOPOLOGY, NF17B_COMPONENT_TOPOLOGY);
    scoped[1] = make_txn(8u, 101u, NF17B_TXN_CLOSE,
        NF17B_COMPONENT_TOPOLOGY, NF17B_COMPONENT_TOPOLOGY);
    scoped[0].scope_id = 900u;
    scoped[1].scope_id = 900u;
    expect_int((int)nf17d_build_conflict_sets(scoped, 2u, sets, NF17D_MAX_CONFLICT_SETS),
               1, "same authoritative scope can conflict across different targets");

    Nf17cDomainDependency false_cycle[2] = {
        {(uint8_t)NF17B_DOMAIN_MATERIAL, (uint8_t)NF17B_DOMAIN_STRUCTURE, 0u, 100u, 1u},
        {(uint8_t)NF17B_DOMAIN_STRUCTURE, (uint8_t)NF17B_DOMAIN_MATERIAL, 0u, 200u, 2u}
    };
    Nf17dScopedPurpleEnvelope scoped_purple[NF17C_MAX_PURPLE_ENVELOPES];
    expect_int((int)nf17d_build_scoped_purple_envelopes(
                   false_cycle, 2u, 9u, scoped_purple, NF17C_MAX_PURPLE_ENVELOPES),
               0, "unrelated targets cannot synthesize a false Purple cycle");

    Nf17cDomainDependency true_cycle[2] = {
        {(uint8_t)NF17B_DOMAIN_MATERIAL, (uint8_t)NF17B_DOMAIN_STRUCTURE, 0u, 300u, 1u},
        {(uint8_t)NF17B_DOMAIN_STRUCTURE, (uint8_t)NF17B_DOMAIN_MATERIAL, 0u, 300u, 2u}
    };
    expect_int((int)nf17d_build_scoped_purple_envelopes(
                   true_cycle, 2u, 9u, scoped_purple, NF17C_MAX_PURPLE_ENVELOPES),
               1, "same target mutual dependence creates scoped Purple");
    expect_int((int)scoped_purple[0].target_id, 300, "Purple records causal target");

    Nf17dScopedTransaction hierarchical[2];
    memset(hierarchical, 0, sizeof(hierarchical));
    hierarchical[0].transaction = make_txn(8u, 501u, NF17B_TXN_OPEN,
        NF17B_COMPONENT_TOPOLOGY, NF17B_COMPONENT_TOPOLOGY);
    hierarchical[1].transaction = make_txn(8u, 502u, NF17B_TXN_CLOSE,
        NF17B_COMPONENT_TOPOLOGY, NF17B_COMPONENT_TOPOLOGY);
    hierarchical[0].scope.valid_mask = (1u << NF17D_SCOPE_CELL) | (1u << NF17D_SCOPE_NEXUS) | (1u << NF17D_SCOPE_WORLD);
    hierarchical[1].scope.valid_mask = hierarchical[0].scope.valid_mask;
    hierarchical[0].scope.id[NF17D_SCOPE_CELL] = 10u;
    hierarchical[1].scope.id[NF17D_SCOPE_CELL] = 11u;
    hierarchical[0].scope.id[NF17D_SCOPE_NEXUS] = 7u;
    hierarchical[1].scope.id[NF17D_SCOPE_NEXUS] = 7u;
    hierarchical[0].scope.id[NF17D_SCOPE_WORLD] = 1u;
    hierarchical[1].scope.id[NF17D_SCOPE_WORLD] = 1u;
    Nf17dScopeKind common_kind = NF17D_SCOPE_WORLD;
    uint32_t common_id = 0u;
    expect_true(nf17d_smallest_common_scope(
                    &hierarchical[0].scope, &hierarchical[1].scope, &common_kind, &common_id),
                "hierarchical transactions find common authority");
    expect_int((int)common_kind, (int)NF17D_SCOPE_NEXUS, "smallest common authority is nexus");
    expect_int((int)common_id, 7, "smallest common authority id retained");
    expect_int((int)nf17d_build_hierarchical_conflict_sets(
                   hierarchical, 2u, sets, NF17D_MAX_CONFLICT_SETS),
               1, "hierarchical shared nexus creates local conflict set");

    Nf17bAuthoritativeState state;
    nf17b_state_init(&state);
    Nf17dCacheStamp topo_cache = nf17d_cache_stamp(&state, NF17B_COMPONENT_TOPOLOGY);
    expect_true(nf17d_cache_valid(&topo_cache, &state), "fresh topology cache valid");

    nf17b_commit_write_mask(&state, NF17B_COMPONENT_MATERIAL);
    expect_true(nf17d_cache_valid(&topo_cache, &state), "unrelated material write does not invalidate topology cache");

    nf17b_commit_write_mask(&state, NF17B_COMPONENT_TOPOLOGY);
    expect_true(!nf17d_cache_valid(&topo_cache, &state), "topology write precisely invalidates topology cache");

    Nf17dLocalEpoch local = {1u, 7u, 2u, 21u, 3u};
    Nf17dHierCacheStamp hier_cache =
        nf17d_hier_cache_stamp(&state, NF17B_COMPONENT_TOPOLOGY, local);
    expect_true(nf17d_hier_cache_valid(&hier_cache, &state, local),
                "hierarchical local cache valid at matching epochs");
    Nf17dLocalEpoch unrelated_chunk = local;
    unrelated_chunk.chunk_id = 22u;
    unrelated_chunk.chunk_epoch = 1u;
    expect_true(!nf17d_hier_cache_valid(&hier_cache, &state, unrelated_chunk),
                "different chunk does not reuse local cache");
    Nf17dLocalEpoch global_flush = local;
    ++global_flush.global_epoch;
    expect_true(!nf17d_hier_cache_valid(&hier_cache, &state, global_flush),
                "global epoch provides coarse emergency invalidation");

    uint32_t floors[NF17B_DOMAIN_COUNT] = {5u, 5u, 5u, 5u, 5u, 5u, 5u};
    Nf17dBudgetPool budget;
    nf17d_budget_init(&budget, floors, 2u, 3u);
    expect_int((int)nf17d_budget_request(&budget, NF17B_DOMAIN_MATERIAL, 6u, false),
               5, "ordinary work cannot borrow before phase barrier");
    expect_int((int)budget.borrowed[NF17B_DOMAIN_MATERIAL], 0, "pre-barrier ordinary borrowing remains closed");
    expect_int((int)nf17d_budget_request(&budget, NF17B_DOMAIN_STRUCTURE, 7u, true),
               7, "critical work can use immediate emergency reserve");
    expect_int((int)budget.emergency_reserve, 1, "critical emergency borrowing is bounded");
    nf17d_budget_phase_barrier(&budget);
    expect_true(budget.borrow_open != 0u, "phase barrier opens shared borrowing");
    expect_true(budget.shared_surplus > 2u, "unused floors return to shared surplus at barrier");
    expect_int((int)nf17d_budget_request(&budget, NF17B_DOMAIN_MATERIAL, 4u, false),
               4, "post-barrier ordinary work may borrow shared surplus");
    expect_true(budget.borrowed[NF17B_DOMAIN_MATERIAL] >= 4u,
                "post-barrier borrowing is explicitly accounted");

    Nf17dReasonTraceRecord trace;
    memset(&trace, 0, sizeof(trace));
    trace.consequential = 1u;
    trace.trace.tick = state.tick;
    trace.trace.actor_id = 12u;
    trace.trace.evidence_hash = 11u;
    trace.trace.belief_hash = 13u;
    trace.trace.contract_hash = 17u;
    trace.trace.affordance_hash = 19u;
    trace.trace.transaction_hash = 23u;
    trace.trace.raw_confidence = 0.72f;
    trace.trace.calibrated_confidence = 0.66f;
    trace.present_mask =
        NF17D_TRACE_EVIDENCE |
        NF17D_TRACE_BELIEF |
        NF17D_TRACE_CONTRACT |
        NF17D_TRACE_AFFORDANCE |
        NF17D_TRACE_TRANSACTION |
        NF17D_TRACE_CONFIDENCE;
    expect_true(nf17d_reason_trace_complete(&trace), "consequential decision trace complete");
    trace.present_mask &= ~NF17D_TRACE_BELIEF;
    expect_true(!nf17d_reason_trace_complete(&trace), "missing belief provenance fails consequential trace");
    trace.present_mask |= NF17D_TRACE_BELIEF;

    Nf17dTraceHistory history;
    nf17d_trace_history_init(&history);
    for (uint32_t i = 0u; i < NF17D_TRACE_HOT_CAPACITY + 3u; ++i) {
        trace.trace.tick = i;
        trace.consequential = (i % 9u) == 0u ? 1u : 0u;
        nf17d_trace_history_push(&history, trace, false);
    }
    expect_int((int)history.hot_count, (int)NF17D_TRACE_HOT_CAPACITY,
               "hot trace history remains bounded");
    expect_true(history.promoted_count > 0u, "consequential events promote automatically");
    expect_true(history.cold_count > 0u && history.cold_hash != 0u,
                "aged trace detail decays to compact cold causal identity");

    expect_int((int)nf17d_overlay_mask(NF17D_VIEW_PLAY), 0, "play preset has no debug overlays");
    expect_true((nf17d_overlay_mask(NF17D_VIEW_WORLD) & NF17D_OVERLAY_MATERIAL) != 0u,
                "world preset exposes material state");
    expect_true((nf17d_overlay_mask(NF17D_VIEW_ACTOR) & NF17D_OVERLAY_BELIEF) != 0u,
                "actor preset exposes belief state");
    expect_true((nf17d_overlay_mask(NF17D_VIEW_CAUSAL) & NF17D_OVERLAY_PURPLE) != 0u,
                "causal preset exposes Purple");
    expect_true((nf17d_overlay_mask(NF17D_VIEW_FULL) & NF17D_OVERLAY_REASON_TRACE) != 0u,
                "full preset exposes reason trace");

    const Nf17bAuthoritativeState before = state;
    const uint32_t hash_before = nf17b_state_hash(&state);
    const Nf17dObservabilityFrame frame = nf17d_observe(
        &state, NF17D_VIEW_FULL, 4u, (uint16_t)set_count, 1u, 0u, &trace);
    const uint32_t hash_after = nf17b_state_hash(&state);
    expect_true(memcmp(&before, &state, sizeof(state)) == 0, "observer cannot mutate authority");
    expect_true(hash_before == hash_after && frame.authoritative_hash == hash_before,
                "viewer ON/OFF observes identical authoritative state");
    expect_int((int)frame.conflict_set_count, 1, "observability frame exposes conflict count");

    const Nf17dDebugBatchContract near_batch = nf17d_debug_batch_contract(&frame, 0u, 0u);
    expect_int((int)near_batch.detail, (int)NF17D_DEBUG_DETAIL_NEAR,
               "near low-pressure diagnostics retain fine detail");
    expect_true(near_batch.cpu_semantics != 0u && near_batch.gpu_presentation_only != 0u,
                "CPU owns debug meaning while GPU remains presentation-only");
    const Nf17dDebugBatchContract degraded_batch = nf17d_debug_batch_contract(&frame, 3u, 2u);
    expect_int((int)degraded_batch.detail, (int)NF17D_DEBUG_DETAIL_REGION,
               "distance and pressure gracefully aggregate diagnostic detail");
    expect_true(degraded_batch.authoritative_hash == frame.authoritative_hash,
                "debug detail degradation preserves authoritative identity");

    policy.observer_can_mutate_authority = 1u;
    expect_true(!nf17d_policy_is_safe(&policy), "unsafe observer mutation policy rejected");

    if (failures != 0) {
        fprintf(stderr, "nightfall v1.7D cleave contracts: FAIL (%d)\n", failures);
        return 1;
    }

    printf("nightfall v1.7D cleave contracts: PASS\n");
    printf("CLEAVE policy=PASS conflicts=PASS cache=PASS budget=PASS trace=PASS observer=PASS presets=PASS\n");
    return 0;
}