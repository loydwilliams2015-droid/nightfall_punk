# LEGAL-01 — rights and licensing audit

**Audit date:** 2026-10-10  
**Scope:** current stewardship ancestry and tracked repository tree.  
**Purpose:** document what was actually checked before applying GPL-3.0-or-later to project-authored software.

## Completed checks

### Visible Git authorship
- 537 commits enumerated across the current stewardship ancestry.
- All 537 expose the same GitHub author/committer account: `loydwilliams2015-droid`.
- No second visible code contributor was found in that ancestry.

This supports a simple current rights picture but does not establish the legal identity behind the account or prove originality of every historical line.

### Tree/content inventory
Current audited tracked tree:
- 351 blobs total;
- 127 Markdown;
- 111 C source files;
- 47 headers;
- 28 shell scripts;
- 14 Python files;
- 11 YAML files;
- small text/CSV/JSON remainder.

No tracked PNG/JPEG/WebP/GIF/SVG, audio, font, PAK, WAD, BSP, GLTF/GLB, FBX, or Blender assets were found.

No obvious `vendor/`, `external/`, bundled GZDoom, Quake/LibreQuake, Tomb Raider, or Unreal source tree was found.

Repository code search found no existing embedded third-party copyright notices, SPDX headers, MIT/GPL/BSD/Apache license texts, or similar imported license markers in the reviewed project tree.

### Direct dependency review
The build configuration references:
- **ENet 1.3.18** — upstream MIT license;
- **raylib 5.5** — upstream zlib/libpng license;
- **libsodium** (optional system dependency) — upstream ISC license.

These dependencies are resolved externally (system package or configured fetch), not vendored into the audited project tree.

### Licensing action
- `GPL-3.0-or-later` selected and applied to project-authored software paths identified in `REUSE.toml`.
- Standard GPLv3 text added at `LICENSE` and `LICENSES/GPL-3.0-or-later.txt`.
- Documentation/evidence/art/trademarks are not silently included in the software grant unless separately marked.
- Release notice and third-party notice updated.

### Contribution mechanism decision
- **Narrow CLA direction selected** rather than DCO-alone if future commercial relicensing is to remain possible.
- Contributors should retain copyright.
- Public GPL availability of accepted contributions should remain permanent.
- Binding CLA language is not yet adopted and requires qualified legal review.
- External code contributions remain gated until that review is complete.

## Residual risks / limits of this audit

This is a repository and provenance audit, not a forensic code-similarity opinion. It cannot prove that every historical AI-assisted or manually written line is free of accidental similarity to third-party code. Credible provenance concerns should trigger targeted review.

The current public/default branch situation is also not fully reconciled: the licensing work exists on the stewardship/research line while `main` remains historically diverged.

## Closure requirements for Issue #36

- [x] Choose intended software license: GPL-3.0-or-later.
- [x] Enumerate visible contributors/authorship in current ancestry.
- [x] Audit current tree for vendored source and distributable media assets.
- [x] Verify direct dependency licenses.
- [x] Add GPL license text.
- [x] Add SPDX/REUSE machine-readable licensing scope.
- [x] Update license status, contributing guidance, release notice, README, dependency notices, and stewardship ledger.
- [x] Decide DCO vs CLA direction: narrow CLA selected for future dual-license optionality.
- [ ] Have the actual narrow CLA text reviewed by qualified open-source counsel before external code contributions are merged.
- [ ] Decide explicit licenses for documentation/evidence/data and future art/audio packages before encouraging redistribution of those materials.
- [ ] Reconcile/merge the licensing commits into the accepted canonical/default branch history so public repository license metadata and user expectations match the adopted policy.
- [ ] Perform a targeted source-provenance review if any credible historical copying/AI-source-similarity concern is identified.

Issue #36 should remain open until the first three unchecked items are intentionally resolved or explicitly moved to separate tracked issues.
