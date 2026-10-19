# N03LINE — Snapshots and deterministic replay
**nightfall!punk • Group 4 • 2026-10-19 • PRAXIS / SIGNAL / STEWARD**

## Research design commitment — LOCK
Periodically checkpoint a complete authoritative state with stable schema, then replay an append-only journal after verifying base revision.

## Source status / no invented evidence
N01 replays from a caller-supplied genesis balance and bounded journal, no checkpoint.

## Remaining mandatory gate
Portable versioned snapshots, atomic manifest, snapshot+WAL cross-domain consistency and compaction.

## Scope and RET
- **A priori:** a durable authoritative consequence, unacknowledged action, observed animation and physical realization are distinct events. No false grant from insufficient evidence.
- **Specified:** this N03 contract establishes a bounded, traceable engineering obligation and an incumbent/challenger comparison requirement where useful.
- **Empirically supported:** only tests actually executed against source-locked artifacts count; do not infer passing N/P acceptance from a policy decision or an unrelated H0 laboratory.
- **Predictable but uncertain:** a simple default plus selective precision and graceful degradation may preserve responsiveness, but cost, correctness and human experience require measurements.
- **Unresolved:** the mandatory gate above, plus integrated A6/A4/A5/B5 durable physics, verified provenance, appropriate rights/licensing and reviewed full release.

## User-selected cross-cutting rules
1. **Graceful degradation:** reject or queue uncertain authoritative work; reduce optional work first, never silently grant it.
2. **Occluded crash resolution:** if an outcome is hidden by interruption, reconcile with verified journal evidence or remain visibly uncertain/quarantined.
3. **Background stability:** bounded low-priority maintenance may not preempt authoritative commitments or rewrite historical truth.
4. **Simplicity and precision:** minimal deterministic mechanisms, finer checks only when material evidence or uncertainty warrants them.

**Decision:** N03 architecture/policy LOCKED for development; production implementation and empirical acceptance OPEN unless separate executed evidence explicitly covers the limited model. HOLD release and upstream PR #53/#54/#55.
