# N01LINE — Durable authoritative transaction log
**nightfall!punk • Group 4 • 2026-10-19 • PRAXIS / SIGNAL / STEWARD**

## Research design commitment — LOCK
Versioned, checksummed append-only record; fdatasync before local publish or success acknowledgment; fail closed on ambiguous write.

## Source status / no invented evidence
POSIX C11 Nf4Wal file replay + bounded receipts + quarantine, but no A6 physical multi-domain commit.

## Remaining mandatory gate
Power-loss fault injection, file-system crash certification, true A6 binding, snapshot origin.

## Scope and RET
- **A priori:** a durable authoritative consequence, unacknowledged action, observed animation and physical realization are distinct events. No false grant from insufficient evidence.
- **Specified:** this N01 contract establishes a bounded, traceable engineering obligation and an incumbent/challenger comparison requirement where useful.
- **Empirically supported:** only tests actually executed against source-locked artifacts count; do not infer passing N/P acceptance from a policy decision or an unrelated H0 laboratory.
- **Predictable but uncertain:** a simple default plus selective precision and graceful degradation may preserve responsiveness, but cost, correctness and human experience require measurements.
- **Unresolved:** the mandatory gate above, plus integrated A6/A4/A5/B5 durable physics, verified provenance, appropriate rights/licensing and reviewed full release.

## User-selected cross-cutting rules
1. **Graceful degradation:** reject or queue uncertain authoritative work; reduce optional work first, never silently grant it.
2. **Occluded crash resolution:** if an outcome is hidden by interruption, reconcile with verified journal evidence or remain visibly uncertain/quarantined.
3. **Background stability:** bounded low-priority maintenance may not preempt authoritative commitments or rewrite historical truth.
4. **Simplicity and precision:** minimal deterministic mechanisms, finer checks only when material evidence or uncertainty warrants them.

**Decision:** N01 architecture/policy LOCKED for development; production implementation and empirical acceptance OPEN unless separate executed evidence explicitly covers the limited model. HOLD release and upstream PR #53/#54/#55.
