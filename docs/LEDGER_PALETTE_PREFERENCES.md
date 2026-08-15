# nightfall!punk — Palette Preferences Ledger

Timestamp: **2026-08-14 23:25 CDT (America/Chicago)**
Chat provenance (public/redacted): **[REDACTED CHAT — Visual Palette Preference]**
Roles: **PRAXIS / SIGNAL / STEWARD**
Branch at record creation: `build/v1.4-relational-combat-communications`
Status: **CURRENT VISUAL-DESIGN PREFERENCE / NO COMPILE IMPLIED**

## Record specificity

- temporal scope: **project-current, beginning with post-v1.4 visual planning**;
- systemic scope: **rendering / art direction / environment / UI-accent palette**;
- specificity level: **SUBSYSTEM -> VISUAL PALETTE PREFERENCE**;
- epistemic status: **explicit user preference + source-verified color values**;
- spatial scope: **Las Flores / Southern-California twilight and night presentation, extensible where compositionally appropriate**;
- exclusion boundary: **does not override existing gameplay-semantic debug/diagnostic colors, accessibility contrast requirements, material truth, lighting contracts, or faction/readability rules**.

> **Specific palette preference does not imply universal recoloring.**

## Added reference palette — LA Sunset

Source: `https://www.color-hex.com/color-palette/13305`
Source title: **LA Sunset Color Palette**

Verified source colors:

```text
#d0d6d6  RGB(208,214,214)
#1e7698  RGB(30,118,152)
#ff8e01  RGB(255,142,1)
#f45f94  RGB(244,95,148)
#081260  RGB(8,18,96)
```

This palette is now part of nightfall!punk's **existing palette preferences**.

It is an additive reference family rather than a command to flatten the project into five literal colors.

## SIGNAL — aesthetic interpretation

The palette is especially compatible with the project's alternate-Southern-California / Las Flores twilight-night identity because it contains a useful cool-to-warm spectrum:

- `#081260` — deep indigo / midnight structural field;
- `#1e7698` — cool blue-teal infrastructural and atmospheric middle value;
- `#d0d6d6` — pale cool silver / chrome / fogged-light neutral;
- `#f45f94` — saturated pink-magenta sunset/neon accent;
- `#ff8e01` — high-energy amber-orange warm accent.

The important relation is not only hue but **value and temperature organization**:

```text
DEEP INDIGO NIGHT
    -> COOL BLUE-TEAL INFRASTRUCTURE
    -> PALE SILVER READABILITY
    <-> PINK-MAGENTA CULTURAL/SUNSET ACCENT
    <-> AMBER-ORANGE ENERGY/WARMTH ACCENT
```

This supports the existing preference for a dark neo-noir world that is **not monochromatically dark**: cool structural depth can coexist with vivid cultural, ecological, transit, signage, sunset, wet-surface and evental warmth.

## PRAXIS — application guidance

Treat the five colors as **palette anchors**, then derive material- and lighting-dependent variations rather than hard-coding the exact swatches everywhere.

Recommended roles:

### `#081260` — midnight indigo anchor

Use as a preferred reference for:

- deep sky / late twilight;
- distant architectural depth;
- shadow-adjacent environmental masses where physically justified;
- cool night fields and large-scale composition.

Do **not** crush all shadow values to this hue; PBR/material/lighting response still governs local truth.

### `#1e7698` — infrastructural blue-teal

Use as a preferred reference for:

- transit/infrastructure accents;
- wet/cool reflected light;
- selected glass, signage, service equipment and atmospheric layers;
- intermediate separation between midnight indigo and pale silver.

### `#d0d6d6` — cool silver neutral

Use as a preferred reference for:

- chrome/silver accents;
- pale concrete/metal under cool light;
- fog/haze highlights;
- readable neutral separation against saturated night colors.

This color is especially useful for retaining value readability without defaulting to pure white.

### `#f45f94` — sunset/neon magenta

Use selectively for:

- cultural nightlife accents;
- sunset residues;
- signage, fashion, art, event light and environmental punctuation;
- warm/cool transitional compositions.

Avoid making magenta a universal cyberpunk shorthand; it should remain situated and compositionally motivated.

### `#ff8e01` — amber-orange energy accent

Use selectively for:

- sunset/warm artificial illumination;
- industrial activity and process-object warmth;
- evental focal points;
- signage and physical-state emphasis where it does not conflict with semantic HUD/hazard conventions.

Do not automatically equate amber with danger. Gameplay semantics take precedence when a color already has a defined functional meaning.

## Compatibility with existing visual doctrine

This palette should reinforce rather than replace the established visual direction:

- PBR/material response remains physically legible;
- cel/value organization may stylize value grouping without erasing material identity;
- neo-noir darkness uses selective vivid color and reflected/wet-surface contrast rather than uniform blackness;
- selective planar/SSR-style reflection language remains compatible where already accepted;
- default heavy outlines remain unnecessary;
- Las Flores should retain plural districts/material cultures rather than becoming one globally homogeneous color grade.

## Palette pluralism rule

nightfall!punk uses **organized palette pluralism**, not indiscriminate eclecticism and not one universal LUT.

```text
GLOBAL VISUAL IDENTITY
    = recurring anchor relations
    + district/material variation
    + time/weather/light variation
    + actor/cultural variation
    + gameplay readability constraints
```

The LA Sunset palette therefore contributes a recurring relational family while allowing neighboring palettes, material-local colors and physically motivated light to coexist.

## Accessibility / gameplay-semantic boundary

Before any production implementation:

- test sufficient luminance contrast for UI/text/critical indicators;
- preserve colorblind-readable redundancy for gameplay-critical information;
- do not encode important combat/relationship/interaction state solely by one palette hue;
- diagnostic colors used to expose engine functions may remain deliberately outside the art palette;
- faction, teammate, hazard and interaction semantics must be reconciled before assigning one of these swatches a permanent semantic role.

## STEWARD — status and precedence

Status: **ADOPTED PALETTE PREFERENCE**.

This record is more specific than project-wide rendering doctrine. It may guide art direction but does not supersede higher-authority contracts concerning:

- material/lighting truth;
- accessibility;
- semantic gameplay readability;
- district diversity;
- actor/faction identity;
- debug/proof visualization.

Precedence:

```text
GAMEPLAY / ACCESSIBILITY SEMANTICS
    > MATERIAL + LIGHTING CONTRACTS
    > CURRENT VISUAL DOCTRINE
    > PALETTE PREFERENCE MODULES
    > ONE-OFF OBSERVATIONS / MOCKUPS
```

Promotion from preferred reference palette to a stricter production color system requires explicit implementation/art-direction evidence rather than inference from this record alone.

## Maxim

> **Keep the night deep, the world legible, and the color culturally alive.**
