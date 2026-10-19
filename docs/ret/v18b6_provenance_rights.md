# nightfall!punk 1.8B.6 — Source Rights, Provenance and Commons Stewardship
PRAXIS / SIGNAL / STEWARD • 2026-10-19 • Engineering review branch.

## Recovered actual licensing source
GitHub issue [#36](https://github.com/loydwilliams2015-droid/nightfall_punk/issues/36) is OPEN but records the adopted **GPL-3.0-or-later software-license direction** (not a declaration that it has reached the accepted default branch).

The complete auditable files are present on **`stewardship/2026-10-10-project-reconciliation`**:
- `LICENSE`: Git blob SHA-1 `29b2470d77bd69a0bd86f17fff66ce9a7acafe30` — GPLv3 text.
- `REUSE.toml`: blob `5d7fb1986d9aadd0439660b6febf81a481b424aa` — per-file authored code/process annotations.
- `LICENSE_STATUS.md`: blob `3493bc8d431116bc4d8756b81005becf2b7ef236`.
- `docs/LEGAL_01_RIGHTS_AUDIT.md`: blob `6d712a12ef157049f5f00cb043c809f3bfbb68eb`.
- `RELEASE_LICENSING_NOTICE.md`: blob `7e82c8a6201101e722f778cd8aaf971aa72fda39`.
Other intended files include `LICENSES/GPL-3.0-or-later.txt`, `THIRD_PARTY_NOTICES.md`, contribution instructions and the draft narrow CLA. They require review in a separate synchronized graft; **none is silently copied into the B6 baseline in this maintenance PR**.

The stewardship audit examined 537 commits with one visible author/committer account, 351 tracked blobs and no obvious vendored third-party engine/source or binary asset collections in that ancestry. This supports but does **not prove** clear title or contributor authorization on all historical work. The corrected B6 tree is a different history; rerun the audit against exact proposed release source before public distribution.

### Direct dependencies and notices
`CMakeLists.txt` uses ENet 1.3.18 (issue #36 reports MIT), raylib 5.5 (zlib/libpng), optional libsodium (ISC). Verify exact upstream license texts at the selected package revisions and transitive notices in the release artifact. CMake's optional libsodium absence permits a **localhost noncryptographic scaffolding fallback**, which must not be deployed as authenticated public multiplayer networking. Internal code comments or OpenAI-generated ideas do not confer ownership of outside asset sets.

## Separate materials
The adopted software GPL direction does **not automatically apply** to research prose, evidence datasets, trademarks, future art/music/sound or player telemetry. Record explicit licensing or consent per material category; do not present fair use as a blanket game-asset redistribution right. Fair use is contextual and jurisdiction-specific; obtain appropriate legal review before reliance.

## Free software and cooperative commerce
GPL-licensed software can be sold and commercially supported. Preserve access to corresponding source and reciprocal licensing obligations when they apply; do not impose unapproved restrictions on GPL recipients. Under CommonStav's commons commitments, distinguish commercial services from proprietary ownership of others' contributions. Fair compensation, open contributor attribution, transparent governance, low-barrier participation and responsible business development are complementary, not substitutes for license compliance. CLA text and collective permissions require human review; no automated assertion that contributors signed away rights.

## Main-only six commit reconciliation — exact public ancestry
Compare corrected source `aaae30e7082f5ceaea198fe4a37f6c02692bab84` ... `main` (`bb377ee8569548eab180778d5bdc9d3c0b16296c`): GitHub returned **six main-only commits**, net **three added files**:
1. `0d911d140f9bb83af125cd1ec45ea7696c745e1c` — C34 non-exclusive semantic affordance contract.
2. `b2d9e79a3110de4c80083b573d238b8b5868ee81` — geographic seed-map contract.
3. `4cb248c79971cae7297e1abd09280ff8811f9650` — seed-map privacy and substrate refinement.
4. `72851c98911243665e4f420677eabc2d1c2402de` — urbanism, rendering shadows and street transition.
5. `32c836b3056d8c5b26befe38580963ebd01f8e46` — high-degree audit addition.
6. `bb377ee8569548eab180778d5bdc9d3c0b16296c` — revert of that audit addition.

Net surviving files: `docs/LEDGER_C34_SEMANTIC_AFFORDANCE_GAME.md`, `docs/SEED_MAP_CONTRACT.md`, `docs/LEDGER_ADDENDUM_C35_C40.md`. Source parent reconciliation requires a content-level audit: C34 shared-ownership may coexist with C06 exclusive resource relationships, and seed-map privacy constraints must be checked against A5 canonical material and server/client visibility. **No merge, cherry-pick, or contract supersession is authorized by this document**.

## Responsible AI and data stewardship
Agent-authored code and research notes are reviewable contributions, not independently certified scientific evidence. Retain source hashes, original human/agent reviewers, actionable tests, historical failed fixtures, and provenance links. Do not send private gameplay, personal participant records, proprietary datasets, credentials, or undisclosed contributor materials into public CI. Consent and minimization must precede H4 telemetry.

## Provenance disposition
The repository connection grants GitHub write access; it does **not** confer automatic cooperative-governance authority to change canonical records or finalize licensing. This engineering branch may add append-only RET documentation and scoped tests but leaves master historical ledgers and licensing source untouched pending reviewer signoff. Source rights, attribution and asset release all remain **HOLD** for production.
