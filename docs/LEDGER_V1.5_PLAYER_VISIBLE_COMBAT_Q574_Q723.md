# nightfall!punk — v1.5 Player-Visible Combat Sweep Q574–Q723

Status: **PRE-BUILD WEAK-POINT CONSOLIDATION — 145 AUTO-RESOLVED / 5 OPEN**
Branch: `build/v1.5-combat-ai-communications`

## Purpose

Thicken the player-visible combat seam without inventing a second combat system. The inherited authoritative substrate already owns weapon specs, magazine/reserve accounting, cadence, reload/late commit, switching, contamination damage, STASIS and revival. Player-visible combat is therefore a presentation/reconciliation layer over one authoritative combat cause.

## Governing causal loop

`INPUT -> PREDICTED LOCAL WEAPON RESPONSE -> AUTHORITATIVE WEAPON GATE / AMMO -> AUTHORITATIVE SHOT -> HIT OR MISS -> IMPACT / CONTAMINATION / SOUND -> PLAYER FEEDBACK + AI-LEGITIMATE EVIDENCE -> INFORMATION TOPOGRAPHY -> AI BELIEF / RESPONSE -> NEW WORLD RELATION`

## Locked implementation consequences

- Mouse1 expresses fire intent; server combat owns accepted round, ammo, hit/miss and contamination truth.
- Local muzzle/recoil/audio presentation may be predicted immediately, but speculative client events never generate authoritative AI evidence.
- One combat state drives world weapon and first-person viewmodel; animation never authors gameplay.
- Long-horizon aim belongs to player perceptual intent. Recoil is a bounded impulse/recovery, not aim theft.
- Carbine hip-fire remains competent and primary; optional focus/ADS is a sidegrade, not an accuracy gate.
- Reticle/spread presentation must correspond to real bounded accuracy/recovery state; no hidden arbitrary accuracy penalties.
- Conventional carbine/pistol use authoritative hitscan/raycast, but hitscan still creates material impact, sound, trace/evidence and contamination relations.
- Confirmed hits use restrained abstract feedback subordinate to diegetic impact; predicted/missed/rejected shots never generate confirmed hit cues.
- Relational contamination/functional consequences, not HP-as-substance, organize ordinary damage presentation.
- Directional damage feedback may be coarse but cannot reveal exact unseen attacker identity/location.
- Ammo stays conventionally readable while contributing to nested future-option/resource accounting.
- Reload/switch/viewmodel timing derives from authoritative combat state; no second animation gameplay state machine.
- Gunfire/impact semantics feed Information Topography only from authoritative events.
- Local spoken Rival communications may become coarse player evidence; subtitles/markers may not exceed channel precision.
- Shared/pure C combat contracts remain separate from raylib/client presentation; use stable shot/result sequences and fixed-capacity presentation/event pools.
- Human/debug proof must bind input -> weapon state -> shot -> hit/miss -> feedback -> evidence -> AI response as one inspectable causal chain.

## Dependency resolutions

Earlier decisions eliminate several apparent uncertainties:

- Relational Perceptual Authority invalidates recoil/camera systems that steal long-term aim ownership.
- Q423 invalidates ADS-required competent shooting.
- Q419 invalidates strong abstract hit-confirmation as the primary impact language.
- Relational health invalidates a conventional HP bar as the ontological center of damage feedback.
- Information Topography invalidates exact directional damage/source revelation and omniscient subtitle/world-marker precision.
- Single authoritative combat state invalidates gameplay-owning viewmodel/animation state machines.
- Minimum sufficient machinery defers universal IK, penetration, ricochet, tactical magazine retention, full acoustic expansion, and large-network soak from this weak-point pass.

## Literary / theoretical interpretation

- **Focalization:** combat feedback remains within the player's situated perceptual horizon.
- **Polyphony:** player, Crew and Rivals may hold different partial readings of the same firefight.
- **Diegesis:** weapon/body/world response carries primary meaning; HUD supplements rather than replaces it.
- **Phenomenological authority:** the player experiences owned intention meeting embodied/world resistance rather than arbitrary control denial.
- **Game theory:** ammo, exposure, information, position, damage and resupply enter opportunity-cost and future-option relations rather than serving as isolated scores.

## Thin executable seams after closure

1. client viewmodel/presentation module;
2. physical muzzle-obstruction / camera-aim bridge;
3. authoritative shot-resolution glue;
4. impact / hit-confirm presentation events;
5. contamination feedback projection;
6. predicted/confirmed event reconciliation;
7. combat diagnostics;
8. integrated shot -> AI evidence -> AI response human proof.

## Five remaining design questions

### Q719 — First-person reticle language

Open: minimal static mark + subtle spread brackets; Quake-like sparse crosshair; Halo-like role-readable reticle; or a nightfall synthesis.

### Q720 — Optional carbine focus/ADS form

Open: omit ADS from first proof; modest shoulder/focus shift with slight zoom; or more conventional sight picture. Hip-fire competence is already locked.

### Q721 — Moment-to-moment player contamination presentation

Open: exact blend of directional impulse, body-domain iconography, subtle material/screen distortion, audio, handling changes, and qualitative HUD tendency indicators.

### Q722 — Placeholder carbine silhouette/material language

Open: how much industrial/cyberpunk/nightfall identity the first greybox weapon should establish versus neutral test geometry.

### Q723 — Ordinary-play hit-confirmation strength

Open: nearly diegetic only; subtle marker + audio; or slightly stronger competitive-FPS confirmation. The invariant remains restrained and subordinate to diegetic impact.

## Build consequence

Do **not** run Vertical Integration Pass II yet. Resolve Q719–Q723 and any remaining weak-point sweeps first. Pass II remains the final reconciliation immediately before opening the next build.

No compile is authorized by this design sweep.