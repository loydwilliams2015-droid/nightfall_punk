# nightfall!punk — Seed Map Geographic Contract

Status: LOCKED design handoff for future consolidation into the master ledger.

## Core rule

Every seed map is a variable realization laid over a fixed Southern California / Las Flores geographic substrate. Seed number varies the world within place; it does not regenerate the place itself.

## Fixed across seed numbers

Every seed map inherits the same atlas truth for its footprint. The following do **not** reroll because the numeric seed changes:

- Southern California topography and canonical physical geography.
- Major thoroughfares and corridor geometry relevant to Las Flores.
- Baseline wildlife patterns, habitat corridors and broad ecological structure at an appropriate non-sensitive resolution.
- Public GIS layers relevant to the game only after privacy, provenance, licensing and sensitivity review.
- Over-arching important buildings, landmarks and major infrastructure chosen for Las Flores canon.
- Canonical 2038 transformations of those features once established.
- The alternate-history metropolitan morphology produced by Las Flores planning history: recognizable Los Angeles street memory, a much smaller urbanized footprint, Manhattanized coastal/core corridors, strongly preserved rural/wild eastern land, high-capacity transit, and sharp urban/wild interfaces.

## Las Flores fixed urban-history substrate

Las Flores remains deliberately close to Los Angeles/Southern California in geography while differing strongly in metropolitan development history.

- City Beautiful-era civic planning, CCC/New Deal greenbelt stewardship, and late-1960s onward environmentalism/growth-boundary politics are the principal long-run planning lineage.
- The urbanized metro footprint is approximately half the present-day Southern California sprawl footprint; much of the avoided growth is in eastern areas that remain rural, ecological reserve, foothill/wilderness, or other non-urban land.
- Coastal Los Angeles remains comparatively recognizable in street memory while undergoing strong vertical infill/Manhattanization.
- Hollybrook corresponds to the Hollywood geographic/cultural memory; Santa Magdalena corresponds to Santa Monica.
- Mulholland Drive remains Mulholland Drive as a deliberate easter egg and geographic continuity marker.
- Traditional Los Angeles economic strengths persist while dense urban habits borrow structurally from places such as Tokyo and Taipei: high transit use, vertical mixed use, street-level commercial intensity and smaller-footprint everyday urbanism.
- Roughly 10–15% of the otherwise preserved/emptied eastern land may be Cattler infestation at the 2038 setting condition; infestation is a historical/ecological condition layered over fixed geography, not a replacement for geographic truth.

## Privacy rule

Use public geographic reality; fictionalize private lives. Real-world person-level information is not seed content. Public availability alone is not sufficient for import.

Operational rule:

> **Generally use public GIS data about Southern California that is relevant to nightfall!punk only when it can be incorporated without reconstructing or exposing real-world persons.**

The seed-authoring/runtime dataset must not depend on real residents' biographies, household profiles, personal routines, personally identifying records, private access/security details or comparable person-level information. Public geographic structures may inform the world; private human lives are fictionalized.

Ecologically or culturally sensitive public data may require aggregation, abstraction, omission or transformation even when legally reusable.

## Three layers

1. **Fixed geographic substrate** — atlas truth and canonical alternate-history urban form.
2. **Seeded realization / initial conditions** — variable local circumstances laid over that place.
3. **Historical world state** — consequences produced by simulation, actors and play.

## Seed identity

A numeric RNG seed is not a place identifier.

```c
typedef struct NfSeedKey {
    uint64_t place_id;          /* WHERE */
    uint64_t seed_number;       /* INITIAL VARIATION */
    uint32_t realization_id;    /* WHICH WORLD/SCENARIO */
    uint32_t content_version;   /* WHICH AUTHORING VERSION */
} NfSeedKey;
```

## Overlap

Seed footprints may overlap partially or completely. Every overlapping coordinate must agree on fixed topography, major corridor geometry, wildlife baseline, canonical landmark truth and fixed Las Flores metropolitan morphology for the same atlas version. Seeded initial conditions and subsequent history may differ.

The same numeric seed may occur in different places. The same place may host different seed realizations. Numeric seed equality alone never means map identity.

## Fixed-versus-variable test

```c
bool nf_seed_feature_is_fixed(NfSeedFeatureKind k)
{
    switch (k) {
        case NF_SEED_TOPOGRAPHY:
        case NF_SEED_MAJOR_THOROUGHFARE:
        case NF_SEED_WILDLIFE_BASELINE:
        case NF_SEED_CANONICAL_LANDMARK:
        case NF_SEED_MAJOR_INFRASTRUCTURE:
        case NF_SEED_CANONICAL_URBAN_FORM:
            return true;
        default:
            return false;
    }
}
```

Variable examples may include minor fictional built fabric, local occupation, maintenance condition, resources, artifact distribution, Crew/Rival/Cattler positions, local ecological intensity and other realization/history-dependent state.

## Maxims

- **Seed determines variation within place; seed does not determine where place is.**
- **Public terrain; fictional lives.**
- **Overlap shares geography, not necessarily circumstance or history.**
- **Place is canonical; realization is variable; history is earned.**
- **Old Los Angeles street memory; different metropolitan history.**
- **You should be able to recognize Los Angeles from Mulholland Drive—and then look toward Hollybrook and realize that history went somewhere completely different.**
