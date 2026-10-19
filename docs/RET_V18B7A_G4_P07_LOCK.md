# P07LINE — Persistence sync latency
**nightfall!punk • Group 4 • 2026-10-19 • PRAXIS / SIGNAL / STEWARD**

## Research design commitment — LOCK
Measure durable fsync costs and compare safe append, snapshot and batching policies without weakening the commit boundary.

## Source status / no invented evidence
N01 uses fdatasync per accepted record; not benchmarked for tick deadline.

## Remaining mandatory gate
WAL latency p95/p99, SSD/HDD and injected latency, batching witness atomicity.

## Scope and RET
- **A priori:** a durable authoritative consequence, unacknowledged action, observed animation and physical realization are distinct events. No false grant from insufficient evidence.
- **Specified:** this P07 contract establishes a bounded, traceable engineering obligation and an incumbent/challenger comparison requirement where useful.
- **Empirically supported:** only tests actually executed against source-locked artifacts count; do not infer passing N/P acceptance from a policy decision or an unrelated H0 laboratory.
- **Predictable but uncertain:** a simple default plus selective precision and graceful degradation may preserve responsiveness, but cost, correctness and human experience require measurements.
- **Unresolved:** the mandatory gate above, plus integrated A6/A4/A5/B5 durable physics, verified provenance, appropriate rights/licensing and reviewed full release.

## User-selected cross-cutting rules
1. **Graceful degradation:** reject or queue uncertain authoritative work; reduce optional work first, never silently grant it.
2. **Occluded crash resolution:** if an outcome is hidden by interruption, reconcile with verified journal evidence or remain visibly uncertain/quarantined.
3. **Background stability:** bounded low-priority maintenance may not preempt authoritative commitments or rewrite historical truth.
4. **Simplicity and precision:** minimal deterministic mechanisms, finer checks only when material evidence or uncertainty warrants them.

**Decision:** P07 architecture/policy LOCKED for development; production implementation and empirical acceptance OPEN unless separate executed evidence explicitly covers the limited model. HOLD release and upstream PR #53/#54/#55.
