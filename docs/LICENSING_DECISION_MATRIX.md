# nightfall!punk — Licensing decision record

**Decision date:** 2026-10-10  
**Software license:** **GPL-3.0-or-later — LOCKED and applied to project-authored software identified by `REUSE.toml`.**

## Completed selection work

1. Visible current ancestry audited: 537 commits, all using the same GitHub author/committer account.
2. Current tree audited for vendored/imported source and media assets: no obvious third-party source tree or tracked media package found.
3. Direct dependency terms reviewed: ENet MIT; raylib zlib/libpng; libsodium ISC.
4. GPL-3.0-or-later selected rather than AGPL for the present project software.
5. Future GPL + separately negotiated commercial licensing retained as an option.
6. **Narrow CLA direction selected** over DCO-alone for future external code contributions if commercial relicensing is to remain possible.
7. GPL text, REUSE/SPDX scope, status notice, contribution gate, and release notice added.

## Why GPL-3.0-or-later

The project favors strong distributed-software reciprocity while allowing commercial use, paid distribution, hosting, support, integration, customization, stewardship, and other services.

The `-or-later` expression gives recipients the GPLv3 terms or a later GPL version at their option.

## Why not AGPL now

The project has not made network-service source reciprocity a constitutional requirement. AGPL remains a future alternative only if that requirement changes; it is not the current license.

## Why a narrow CLA rather than DCO-alone

DCO is a provenance certification. It does not itself provide the additional relicensing grant needed for a future GPL + commercial dual-license model.

The intended CLA should let contributors retain ownership while granting a narrowly bounded additional license to the project. Binding CLA language still needs qualified legal review before external code contributions are merged.

## Separate materials

The GPL software decision does not automatically decide licenses for:
- documentation/research prose;
- evidence datasets;
- original art/audio/lore;
- trademarks/branding;
- hosted services and service contracts.

Those require explicit treatment before distribution.

## Commercial sustainability

GPL software may be used commercially subject to its terms. Commons-oriented revenue may include hosting, support, custom development, stewardship, media, convenience, packaged distributions, institutional contracts, and separately negotiated commercial integration where contributor rights permit it.

See `LICENSE_STATUS.md`, `docs/CONTRIBUTOR_LICENSING_POLICY.md`, and `RELEASE_LICENSING_NOTICE.md`.
