# P11LINE — Tail latency and responsiveness
**nightfall!punk • Group 4 • 2026-10-19 • PRAXIS / SIGNAL / STEWARD**

## Research design commitment — LOCK
Optimize 95th and 99th percentile input-to-visible response and frame time, never merely mean throughput.

## Source status / no invented evidence
No new end-to-end timing data.

## Remaining mandatory gate
Representative 60Hz, server/client timestamps, delayed display and crash restart behavior.

## RET classification
A priori: authorized consequence, apparent client effect and unacknowledged packet are distinct. Formally specified: bounded, evidence-first policy, not a demonstrated performance theorem. Empirical: only direct compiler/CI and profiled trials count. Highly predictable but uncertain: simplicity and budgeted fine checks may reduce cost under suitable workloads. Unresolved: integrated full-world measurement, real network and physical durability and review.

## Cross-cutting decisions
Graceful degradation: reduce optional work and become PENDING/read-only rather than falsely grant. Occluded crash resolution: ambiguous outcomes resolved only by checked journal/history, otherwise quarantine. Background stability: maintenance uses residual measured budget and must never silently rewrite authoritative history. Fidelity: simple default plus selective finer checks. Never conflate C03 M3/M4 with fixed/rotating arbitration modes.

**Disposition:** P11 architecture/policy LOCKED; end-to-end implementation, actual benchmark and release acceptance HOLD.
