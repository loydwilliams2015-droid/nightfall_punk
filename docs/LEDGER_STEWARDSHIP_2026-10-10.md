# STEWARD ledger — repository reconciliation, 2026-10-10

## Request

Tidy code and project relations; improve scientific transparency, provenance, attribution, copyright/fair-use discipline, business-model compatibility, housekeeping, and data organization.

## Audit findings

1. **License contradiction:** root README called the project open-source/copyleft while GitHub detected no LICENSE. Corrective action: preserve old README archaeologically; root wording must now state license pending.
2. **State drift:** root README described v0.9 while active research is 1.8A/1.8B. Corrective action: new Project State Index.
3. **Branch divergence:** current 1.8B research line remains diverged from `main`; issue #30 remains a promotion blocker.
4. **B.1 provenance gap:** handoff records an H1 M4 result, but the tested C package is not grafted into GitHub and the original complete archive was not recoverable in this session. Do not reconstruct it retroactively as original evidence.
5. **Third-party clarity:** ENet, raylib, and optional libsodium are build/runtime dependencies with distinct licenses; comparative references such as GZDoom/Quake/Tomb Raider/Unreal are not automatically dependencies.
6. **Governance gap:** no root contribution/security/conduct/data/branch/legal policies or PR provenance checklist.

## Actions in this stewardship branch

- preserve the historical root README;
- replace current project presentation with a state-aware README;
- add license-status and license-decision records;
- add project state index;
- add scientific-accountability v2;
- add legal/provenance/fair-use policy;
- add third-party notices;
- add data/evidence and branch/release policies;
- add contributing, conduct, security, and PR-review checklists;
- add a dependency-free governance audit script/workflow.

## Ethical/commercial stance

The project may pursue commons/nonprofit objectives and legitimate profit simultaneously. Do not encode a "noncommercial" restriction while calling the code open source. Prefer an OSI-compatible copyleft model if the rights audit supports it, and monetize services/distribution/stewardship transparently. Charity, solidarity, and community benefit supplement—not replace—fair wages, privacy, safety, taxes, provenance, and honest product claims.

## Scientific stance

Maintain challenger models, negative controls, failed runs, and evidence grades. Credibility attaches to the scope of the evidence, not to prestige. External games establish mechanism precedent, not project axioms. AI assistance is disclosed and reviewed; it is not independent experimental replication.

## Open blockers

- choose/adopt project license after rights audit;
- recover or explicitly reproduce 1.8B.1 source and rerun branch CI;
- reconcile six main-only commits before canonical promotion;
- review/close or archive older draft PRs through a dedicated historical pass rather than deleting them opportunistically.

**Disposition:** stewardship foundation prepared; no production evidence grade promoted by this documentation pass.
