# nightfall!punk — Data and evidence organization

## Canonical layout for new experiments

```
evidence/<milestone>/
  README.md              # purpose, ancestry, evidence grade, limitations
  manifest.json          # source SHA, toolchain, platform, commands, seed sets
  PAR.md                 # dimension-specific interpretation
  results.csv            # raw or minimally processed rows
  summary.json           # derived summary, never a replacement for raw rows
  SHA256SUMS.txt          # immutable identities
  logs/                  # selected reproducibility logs, if useful
```

Large binaries/build trees belong in release artifacts or external archival storage, not Git history, unless their inclusion is itself required evidence.

## Data classes

- **Raw:** direct test output. Preserve unchanged after publication.
- **Derived:** summaries, confidence intervals, charts, rankings. Must identify raw inputs and analysis code.
- **Fixture:** authored cases, seeds, or world definitions. Distinguish these from naturalistic samples.
- **Human:** gameplay/telemetry/interviews. Require consent, minimization, and an explicit retention policy.

## Statistical accountability

Report independent experimental units, repeated observations, held-out definitions, exclusions, tolerances, seeds, and denominators. Do not convert authored-case frequency into claims about real gameplay prevalence.

## Corrections

Never overwrite published raw evidence to make a repaired test look historically clean. Add a new result set and an erratum linking old and new source/data hashes.

## Privacy and security

Do not commit credentials, IP addresses tied to individuals, private chat logs, private server dumps, raw voice/video without consent, or unnecessary identifiers. Prefer anonymized/aggregate human-study data.

## Retention

Keep compact raw scientific evidence and manifests when they materially support a decision. Retire redundant build artifacts after hashes and reproducible build instructions exist. Preserve failed/pathological cases when they informed architecture.
