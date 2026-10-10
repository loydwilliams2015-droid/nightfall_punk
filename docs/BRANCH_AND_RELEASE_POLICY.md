# nightfall!punk — Branch, review, archive, and release policy

## Branch roles

- `main`: accepted integration line only; not automatically the newest experiment.
- `build/*`: executable milestone or build-specific work.
- `integration/*`: source grafts, cross-version reconciliation, runtime integration.
- `design/*`: specifications/PAR/architecture with no compile claim unless explicitly stated.
- `archive/*`: immutable accepted historical snapshots.
- `review/*`: review/reconciliation work, not new authority.
- `agent/*`: historical agent work unless separately promoted.

## Rules

1. Open PRs against the **true source parent**. Do not force every long-lived research branch onto `main`.
2. Do not force-push milestone/archive branches to erase failures.
3. State whether source is committed, local-only, reconstructed, or unavailable.
4. A design PR cannot promote H0 to H1; CI cannot promote an isolated lab to H3 without runtime integration.
5. Preserve old PRs/branches as archaeology until a dedicated closeout review marks them superseded and links the successor.
6. Do not merge across known divergence until conflicts and main-only commits are reviewed.
7. Release tags should include an evidence grade or maturity statement in release notes; avoid semantic-version implications not supported by the project state.

## Current reconciliation

As of 2026-10-10, the 1.8B PAR branch remains diverged from `main`. Issue #30 tracks the main-only reconciliation requirement. The 1.8A→1.8B PR chain remains open/draft and should be reviewed in ancestry order rather than flattened into one unreviewed merge.
