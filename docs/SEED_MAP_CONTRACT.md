# nightfall!punk — Seed Map Geographic Contract

Status: LOCKED design handoff for future consolidation into the master ledger.

## Core rule

Every seed map is a variable realization laid over a fixed Southern California / Las Flores geographic substrate. Seed number varies the world within place; it does not regenerate the place itself.

## Fixed across seed numbers

- Southern California topography and canonical physical geography.
- Major thoroughfares and corridor geometry relevant to Las Flores.
- Baseline wildlife patterns and habitat corridors at an appropriate non-sensitive resolution.
- Public GIS layers relevant to the game only after privacy, provenance, licensing and sensitivity review.
- Over-arching important buildings, landmarks and major infrastructure chosen for Las Flores canon.
- Canonical 2038 transformations of those features once established.

## Privacy rule

Use public geographic reality; fictionalize private lives. Real-world person-level information is not seed content. Public availability alone is not sufficient for import.

## Three layers

1. Fixed geographic substrate.
2. Seeded realization / initial conditions.
3. Historical world state produced by simulation and play.

## Seed identity

A numeric RNG seed is not a place identifier.

```c
typedef struct NfSeedKey {
    uint64_t place_id;
    uint64_t seed_number;
    uint32_t realization_id;
    uint32_t content_version;
} NfSeedKey;
```

## Overlap

Seed footprints may overlap partially or completely. Every overlapping coordinate must agree on fixed topography, major corridor geometry, wildlife baseline and canonical landmark truth for the same atlas version. Seeded initial conditions and subsequent history may differ.

The same numeric seed may occur in different places. The same place may host different seed realizations. Numeric seed equality alone never means map identity.

## Maxims

- Seed determines variation within place; seed does not determine where place is.
- Public terrain; fictional lives.
- Overlap shares geography, not necessarily history.
- Place is canonical; realization is variable; history is earned.
