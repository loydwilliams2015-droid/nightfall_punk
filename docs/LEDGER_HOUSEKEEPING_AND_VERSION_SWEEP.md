# nightfall!punk — Housekeeping + Longitudinal Version Sweep

Status: **STANDING PROJECT-ORGANIZING CONTRACT**
Branch context: `build/v1.5-combat-ai-communications`

This ledger extends `LEDGER_META_DESIGN_CONSOLIDATION_ROUTINE.md` with the accepted housekeeping and version-sweep recommendations.

## H1 — KEEP: One compact authoritative dashboard

Maintain one canonical `PROJECT_STATE_INDEX` as the concise current-state dashboard. It should link outward to richer subsystem, philosophical, build, test, and archaeological ledgers rather than duplicating their full contents.

The index should privilege current observability:

- current build / branch / exact candidate where relevant;
- current causal spine;
- EXISTING / ADVANCING / PAR / OPEN / DEBT / RETIRED;
- what is specified versus implemented versus integrated versus observable versus verified versus accepted;
- top current risks / contradictions;
- next proof / build frontier.

Rule:

> **One concise current count-as-one; many provenance-preserving traces.**

## H2 — KEEP: Sweep canonical + important experimental history

The eventual longitudinal version sweep must examine both:

1. canonical build lineage; and
2. important failed, divergent, experimental, or superseded branches.

They must remain explicitly classified so experimental ancestry is never silently promoted to canonical authority.

For each important version/branch record:

`Purpose -> ancestry -> intended advance -> actual implementation -> automated proof -> human proof -> failure/regression -> later salvage/supersession -> current authority`

Canonical rule:

`Historical existence != Current authority`

Experimental and failed work is retained because it can contain decisive negative evidence, reusable machinery, or explanations for why later architecture took its current form.

## H3 — KEEP + AMEND: Explicit longitudinal-sweep checkpoint

The large longitudinal version sweep is **not automatically triggered by a calendar or by merely reaching 3–5 build generations**.

Instead, at every relevant full consolidation, build boundary, milestone close, or state where 3–5 substantive build generations have accumulated, explicitly prompt:

> **H3: Is this the right time for a longitudinal version sweep?**

Evaluate the answer using evidence rather than cadence alone.

### Evidence favoring YES

- 3–5 substantive build generations have accumulated;
- a major public milestone / alpha / architectural transition is approaching;
- project-state summaries are becoming difficult to state accurately;
- nomenclature or authority drift is accumulating;
- repeated regressions suggest lost inheritance;
- experimental branches contain potentially valuable unsalvaged work;
- current systems have unclear historical provenance;
- duplicate machinery or semantic drift is suspected;
- contradictions have accumulated across ledgers/builds;
- a major build would benefit from reconstructing what earlier versions actually proved.

### Evidence favoring NOT YET

- only one narrow build frontier has advanced since the last sweep;
- current ancestry and system maturity remain clear;
- no meaningful lost-capability or semantic-drift risk is visible;
- the sweep would interrupt a short, well-bounded proof cycle without changing decisions;
- relevant experimental material has already been reconciled.

### H3 disposition

Use:

- `SWEEP NOW` — evidence supports immediate longitudinal archaeology;
- `DEFER WITH TRIGGER` — not yet; name the next build/milestone/condition that should re-prompt H3;
- `NO SWEEP NEEDED` — only when a candidate sweep would add essentially no assessability or organizational value.

The default is **prompt, assess, then decide** — never silently auto-run the large sweep.

## Standing housekeeping cadence

- after ~10–25 substantive decisions or dense automatist work: micro-consolidation;
- before substantial builds: full OIAOT + dependency/count-as-one pass;
- after automated tests: machine reconciliation;
- after human video: observability/causal reconciliation;
- at milestone acceptance: concise FROM -> TO closeout;
- at relevant boundaries: ask **H3**;
- if H3 = SWEEP NOW: perform longitudinal build/version archaeology.

## Longitudinal sweep structure when H3 authorizes it

1. **Lineage reconstruction** — exact ancestry, purpose, implementation, proof, regressions, supersession.
2. **System-by-system comparison** — movement, camera, combat, AI, Cattlers, contamination, energy, Information Topography, communications, routes, claims, networking, rendering, diagnostics, etc.
3. **Inheritance audit** — good inheritance / dead inheritance / semantic drift / duplicates / lost capability / unintegrated advance.
4. **Conceptual archaeology** — distinguish influence, current principle, implemented mechanism, metaphor, testable contract, retired interpretation.
5. **Statistical maturity recount** — architecture / implementation / integration / observability / verification / acceptance with bounded confidence.
6. **Outputs** — full archaeological ledger + engineering state index + concise executive dashboard.

## Governing maxim

> **Housekeeping keeps the present intelligible; longitudinal archaeology keeps the project's development intelligible. Neither should replace experimentation.**
