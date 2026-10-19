# Group 5 S01–S14 — executed evidence for 72-item provenance validator
Dated 2026-10-19. Repo `loydwilliams2015-droid/nightfall_punk`; draft PR #57. **Exact tested source commit** `6b599c5123f41b26bf84d25b634d69ef17dbe72e`, workflow **39331098000**, job **118329179673**, GitHub conclusion **SUCCESS**.

## Raw executed result
```json
{"accepted_full_engine": 0, "by_group": {"G1": 12, "G2": 9, "G3": 11, "G4": 26, "G5": 14}, "determinations": 72, "errors": [], "negative_controls": 7, "result": "PASS", "schema": "1.8B.7A-G5-S01-v1", "unresolved_gate_families": 18}
```

## Exact SHA-256 inputs, from GitHub job
- `docs/evidence/v18b7a_g5_master_ledger.json`: `4afb77e135112566c4f1d9c99ba5268d2f3c4950511cc54f56d303370038e35e`
- `tools/verify_v18b7a_ledger.py`: `2c661af1c37c6ffe7184c020ab36e03eca710dd77e6fefc96633948cb569ee0a`

## What this **demonstrates**
- All 72 declared design determination IDs are present with correct group ownership and unique membership; expected source contract documents exist in the checked-out repository.
- Four historical test evidence groups have pinned workflow IDs and valid source commit SHA shapes. Their test file bytes match pinned Git blob hashes, including the supplementary G4 background governor.
- Every S01–S14 entry is a **design specification** only; no separately executed S-group physics, human, or security test.
- All 72 full-engine acceptance fields remain OPEN; all 18 consolidated remaining gate families OPEN; release status HOLD.
- Seven deliberate negative mutations are rejected: illegal full-engine acceptance, missing cited file, duplicate decision ID, release promotion, removing a remaining gate, zeroed workflow ID, false source hash.

## What this **does not demonstrate**
- The verifier checks structural integrity and object hashes but **does not query GitHub live CI to establish that recorded historical job IDs actually passed**; their reported scope/results were independently read via the GitHub connector.
- Not a recovered B1 H1 original, independently implemented M3-versus-M4 oracle, full A3/A4/A5/A6/B3/B4/B5 integration, H4 human acceptance, file-system power-loss certification, networking security or sustained 60Hz latency test.
- A metadata schema cannot prove the absence of every missing project requirement; the 18 gate families consolidate *known* issues, not an exhaustive formal proof that nothing else could be wrong.

## Release decision and handoff
**S01–S14 design specification LOCKED and exact-source validator CI PASS within declared scope. All 72 full-engine acceptances remain OPEN; all 18 consolidated integration gate families remain unresolved. Release and merge HOLD; upstream PR #53–#56 draft/unmerged.** A maintainer may use the manifest to prioritize 7B correctness-admissible performance and 7C complete acceptance without implying a release decision.
