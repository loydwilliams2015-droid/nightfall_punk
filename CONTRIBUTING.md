# Contributing to nightfall!punk

Thank you for helping improve the project. Contributions should strengthen both the code and the evidence behind it.

## Software license

Project-authored software covered by `REUSE.toml` is licensed under **GPL-3.0-or-later**. See `LICENSE`, `LICENSE_STATUS.md`, and `RELEASE_LICENSING_NOTICE.md`.

## External code contribution gate

The project intends to preserve the option of future GPL + commercial dual licensing. For that reason, **external code contributions are not yet mergeable until the reviewed narrow CLA is in place**.

The chosen policy direction is:
- contributors retain copyright;
- contributions remain available under GPL-3.0-or-later;
- the eventual CLA grants only the additional rights needed for project distribution and optional separately negotiated commercial licenses;
- no blanket copyright assignment;
- employer-owned contributions require appropriate authority.

A DCO may later be added as provenance certification, but DCO-alone is not the selected rights mechanism for dual licensing. See `docs/CONTRIBUTOR_LICENSING_POLICY.md`.

Issues, design discussion, bug reports, reproducible test reports, documentation review, and scientific critique remain welcome while the CLA is being drafted/reviewed.

## Contribution standard

Every substantive PR should state:
- source parent and intended branch role;
- design claim and evidence grade (H0–H4);
- changed authority boundaries;
- tests actually run and platform/toolchain;
- new/changed datasets and hashes;
- third-party code/assets/references and their licenses;
- substantive AI assistance, if any;
- known limitations and falsification cases.

Do not copy code or assets from another project without compatible licensing/permission and clear attribution. Mechanism study should normally lead to an independent implementation, not unattributed transcription.

## Engineering

Use strict C11 where applicable, keep warnings enabled, preserve deterministic controls, avoid silent fallback from PENDING/unsupported into success, and keep rendering/diagnostics read-only with respect to authoritative state.

## Science

Keep negative controls, failed seeds, pathological cases, and challenger models. Do not delete inconvenient evidence. Do not call a synthetic fixture corpus representative of players or production worlds.

## Security/privacy

Never commit secrets, credentials, private keys, personal telemetry, or private server data. Report security-sensitive issues privately when GitHub private vulnerability reporting is available.

See `PROJECT_STATE_INDEX.md` and the repository policy documents before submitting a major build.
