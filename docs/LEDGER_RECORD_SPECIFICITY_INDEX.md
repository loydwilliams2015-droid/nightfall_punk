# nightfall!punk — Record Specificity Index

Timestamp: **2026-08-14 22:59 CDT (America/Chicago)**
Chat provenance (public/redacted): **[REDACTED CHAT — Project Recordkeeping Specificity]**
Roles: **PRAXIS / STEWARD / SIGNAL**
Record specificity: **PROJECT-WIDE / RECORDKEEPING INDEX**
Status: **CURRENT SPECIFICITY RECONCILIATION**

This index applies the specificity policy in `docs/LEDGER_PROVENANCE_POLICY.md` to current active records without rewriting every historical file.

## Current record map

| Record | Effective specificity | Evidence/status |
|---|---|---|
| `docs/LEDGER_PROVENANCE_POLICY.md` | **PROJECT-WIDE / RECORDKEEPING POLICY** | Current policy |
| `docs/LEDGER_RECORD_SPECIFICITY_INDEX.md` | **PROJECT-WIDE / RECORDKEEPING INDEX** | Current reconciliation |
| `docs/HANDOFF_V1.4_HUMAN_ACCEPTANCE.md` | **VERSION v1.4 / TEST + HUMAN-ACCEPTANCE HANDOFF** | Automated-green candidate; human-video gate distinct |
| `docs/LEDGER_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md` | **VERSION v1.5 / SUBSYSTEM Combat + AI Communications / DESIGN RESOLUTION** | 150/150 design disposition; not v1.5 implementation proof |
| `docs/HANDOFF_V1.4_TO_V1.5_COMBAT_COMMUNICATIONS.md` | **VERSION TRANSITION v1.4→v1.5 / SUBSYSTEM Combat + AI Communications / HANDOFF** | Carries implemented v1.4 constraints into v1.5 planning |
| `docs/HANDOFF_RECALL_QUESTIONS_V1.5_COMBAT_COMMUNICATIONS.md` | **VERSION v1.5 / SUBSYSTEM Combat + AI Communications / QUESTIONS-HANDOFF** | Closed design guardrails + future uncertainty procedure |

## Mixed-specificity records

A single ledger may contain statements of different scope. The record-level specificity is the default ceiling, not permission to universalize every sentence.

When a section or entry is narrower than its parent record, add an inline field when useful:

```text
Specificity: SUBSYSTEM AI Communications / FEATURE Report Coarsening
Specificity: VERSION v1.5 / TEST Integrated Las Flores Proof
Specificity: OBSERVATION / seed 20260724 / recording 01
```

The narrowest applicable tag wins for interpretation.

## Specificity dimensions

For complex records, STEWARD may describe specificity across several dimensions:

- **temporal:** project-wide / version / build / run / observation;
- **systemic:** whole project / subsystem / feature / object/mechanic;
- **epistemic:** design resolution / implemented / automated proof / human proof / observation;
- **population/context:** all actors / one faction / one species / one role / one seed / one scenario;
- **spatial:** world / seed-world / region / 8 m base element / encounter site;
- **performance:** ordinary / surge / stress tier.

Example:

```text
Record specificity:
  temporal = v1.5
  systemic = Cattler ecology / predator search
  epistemic = design resolution
  population = Dream Cattlers
  spatial = local encounter + habitat context
  performance = ordinary tier
```

## Promotion / invalidation rule

Use specificity when resolving apparent contradictions.

A narrow record may:

- refine a broader rule inside its legitimate scope;
- provide evidence toward promotion;
- invalidate a broader assumption only if the contradiction is explicitly reconciled.

It may not silently replace a broader invariant outside its scope.

Conversely, a project-wide rule may invalidate a narrower proposal when the proposal directly conflicts with it.

Canonical relation:

```text
PROJECT-WIDE INVARIANT
        constrains
VERSION / SUBSYSTEM POLICY
        constrains
FEATURE / TEST CONTRACT
        interpreted through
OBSERVATION / SAMPLE
```

Evidence may travel upward only through explicit reconciliation:

```text
OBSERVATION -> REPLICATION/COMPARISON -> TEST/PROOF -> SUBSYSTEM CONFIDENCE -> VERSION POLICY -> PROJECT-WIDE CANON
```

## PRAXIS

Before implementing a ledger decision, identify its specificity. A version-local or feature-local rule must not be coded as an unconditional project-wide invariant unless STEWARD has promoted it.

## SIGNAL

When comparing games, science, literary theory, telemetry, or play recordings, preserve the scope of the evidence. One game example, one seed, or one recording is evidence at its actual level, not automatic universal proof.

## STEWARD

Maintain both **precedence** and **specificity**. When two records disagree, ask:

1. Which is newer and explicitly superseding?
2. Which has stronger evidence status?
3. What scope did each record actually earn?
4. Are they truly contradictory, or simply statements at different specificity levels?

## Maxim

> **Specific evidence should remain specific until it earns generality.**
