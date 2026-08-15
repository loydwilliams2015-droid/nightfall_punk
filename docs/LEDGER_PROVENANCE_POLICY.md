# nightfall!punk — Ledger / Handoff Provenance Policy

Timestamp: **2026-08-14 22:59 CDT (America/Chicago)**
Chat provenance (public/redacted): **[REDACTED CHAT — Project Recordkeeping Policy]**
Roles: **PRAXIS / STEWARD / SIGNAL**
Record specificity: **PROJECT-WIDE / RECORDKEEPING POLICY**
Status: **CURRENT PUBLIC-RECORD POLICY**

## Rule

Every new public-facing nightfall!punk ledger, handoff, reconciliation, or comparable project record must include:

- exact local timestamp;
- timezone or explicit offset/context;
- chat-origin field;
- **redacted public chat label**;
- relevant role attribution when PRAXIS / STEWARD / SIGNAL materially participate;
- **record specificity** stating how narrowly or broadly the record applies;
- implementation/evidence status when relevant.

## Public redaction

Do not place private conversation titles, personal names, account names, direct identifiers, or unnecessary private context into public repository provenance.

Preferred public form:

```text
Timestamp: YYYY-MM-DD HH:MM TZ (America/Chicago)
Chat provenance (public/redacted): [REDACTED CHAT — Semantic Project Topic]
Roles: PRAXIS / STEWARD / SIGNAL
Record specificity: <scope>
Status: <design / implementation / automated proof / human proof / etc.>
```

The semantic topic may describe the work sufficiently for archival navigation without reproducing the private chat title.

## Record specificity — required

Specificity describes **the domain over which a statement is intended to hold**. It prevents a local test result, one build constant, or one subsystem decision from being silently promoted into a project-wide law.

Use the narrowest accurate scope. Preferred specificity classes:

1. **PROJECT-WIDE** — durable doctrine, terminology, architecture, public-record policy, or cross-version invariant.
2. **VERSION / BUILD** — applies to a named version, candidate, branch, or build line.
3. **SUBSYSTEM** — applies to a bounded system such as combat, camera, communications, networking, Cattler ecology, Snap Locomotion, or resource accounting.
4. **FEATURE / MECHANIC** — applies to one mechanic, relation, object family, weapon role, report type, claim type, etc.
5. **TEST / PROOF** — applies only to a deterministic test, CI gate, human acceptance scenario, benchmark, or proof fixture.
6. **OBSERVATION / SAMPLE** — one playthrough, recording, trace, performance sample, seed, encounter, or measured instance; never universal by itself.

Compound specificity is allowed and preferred when useful, for example:

```text
Record specificity: VERSION v1.5 / SUBSYSTEM Combat + AI Communications / DESIGN RESOLUTION
Record specificity: VERSION v1.4 / TEST Human Acceptance / OBSERVATION seed 20260724
Record specificity: PROJECT-WIDE / SUBSYSTEM Snap Locomotion / TERMINOLOGY
```

## Specificity promotion rule

A narrower record does not automatically generalize upward.

```text
OBSERVATION
    -> repeated evidence / comparison
    -> TEST / PROOF
    -> FEATURE / SUBSYSTEM confidence
    -> VERSION policy
    -> PROJECT-WIDE rule only when explicitly reconciled and justified
```

Promotion to a broader scope requires an explicit STEWARD reconciliation or a stronger accepted ledger entry. Conversely, a project-wide invariant constrains narrower records unless a later explicit superseding decision changes it.

## Evidence/status specificity

Keep **what the record says** separate from **how strongly it is evidenced**. When relevant, identify one or more of:

- `DESIGN RESOLUTION`;
- `IMPLEMENTED`;
- `AUTOMATED GREEN`;
- `HUMAN PROOF / HUMAN ACCEPTED`;
- `DEFERRED`;
- `HISTORICAL / SUPERSEDED`;
- `OBSERVATIONAL ONLY`.

Do not infer `IMPLEMENTED` from a design ledger, `HUMAN ACCEPTED` from CI, or `PROJECT-WIDE` from a successful local proof.

## Historical records

Do not rewrite every historical immutable/version-specific ledger solely to retrofit metadata.

When an older record is materially amended:

- add timestamp/redacted-chat provenance and specificity to the amendment or updated record; or
- create a timestamped reconciliation/addendum that points back to the historical source and states the old record's effective scope.

Historical absence of this metadata does not erase provenance; the current policy governs records from this point forward.

## Role duties

### PRAXIS

Ensure implementation-oriented handoffs identify when/where the decision was recorded, its specificity, and its evidence state before translating it into code. Do not implement a project-wide interpretation of a subsystem-local rule without reconciliation.

### SIGNAL

Keep comparison/research evidence distinguishable from implementation truth; preserve timestamped origin and sample scope; label observations and comparisons at their actual specificity.

### STEWARD

Enforce timestamp + redacted-chat provenance, specificity, precedence, implementation-status distinctions, and public privacy hygiene across ledger/handoff records. Prevent accidental promotion from observation -> proof -> subsystem -> project-wide canon.

## Maxims

> **Preserve the history of the decision without publishing the identity of the conversation.**

> **A record is authoritative only within the specificity it actually earned.**
