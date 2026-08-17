# nightfall!punk v1.5 — Build Handoff

Date: 2026-08-15
Branch: `build/v1.5-combat-ai-communications`
Basis: v1.4 green `e16ab0e7a6d12e0382403e007b37755ae56099f8`
Verified code candidate: `74fd07a2bb6d7ffcfcdf06fd6691bca2a1cffb36`
CI: run `31908280120` / #479 — SUCCESS
Status: **READY FOR HUMAN GRAPHICAL PROOF — DO NOT MERGE YET**

## Pull / build / run

```bash
git switch build/v1.5-combat-ai-communications
git pull
./v15.sh standard-check
./v15.sh demo
```

GitHub CI has already compiled the strict graphical target and passed the complete inherited + v1.5 automated suite. Local `./v15.sh test` is optional unless you want an additional machine-local verification.

## Demo controls

- F1 player view; F2 observer; F3 top view; F5 reset.
- WASD, Shift, Ctrl, Space.
- LMB fire; RMB Focus; R reload; 1/2 weapon selection.
- Q creates a WATCH communication marker plus Crew `SEEN` receipt presentation. `SEEN` means received, **not** committed or obeyed.
- F6 creates bounded source -> sink extraction/outflow.
- F7 creates bounded return/restitution.

## Recording checklist

Prefer one continuous run if practical.

- [ ] Show the provisional eco-tech-guard carbine in first person.
- [ ] Fire enough to show both hit and miss consequences if practical.
- [ ] Capture the restrained confirmed-hit cue and, if it happens naturally, the slight weakpoint/head distinction.
- [ ] Hold RMB while moving/stopping so Snap-Guided Focus visibly bobbles/jostles but rapidly recenters without stealing long-horizon aim.
- [ ] Keep Rival 1 / Rival 2 diagnostics visible long enough to capture channel, confidence, precision, hypothesis/action differences.
- [ ] Break/reacquire line of sight if practical; check that stale information does not remain exact through occlusion.
- [ ] Capture visible Rival physical movement/action, not only diagnostic text.
- [ ] Press Q once and capture WATCH + Crew `SEEN`; do not interpret it as autonomous-compliance proof.
- [ ] Press F6 and hold the metabolic diagnostic long enough to show return deficit/restitution gap and source-capacity pressure.
- [ ] Press F7 and show return/restitution improving cycle continuity.
- [ ] Switch briefly to F2 or F3 to show player/teammate/two Human Rivals/two Dream Cattlers in the same world.
- [ ] Capture Cattlers as distinct ecological actors rather than members of the human report/communication ontology.

## What to upload back

Upload the video here. No written report is required from you; I can evaluate the recording against the ledger. If you notice a crash, impossible shot, exact wall tracking after occlusion, frozen AI, excessive Focus oscillation, unreadable communication, or restitution values moving in the wrong direction, mention the approximate timestamp.

## Acceptance cautions

- A report, gunshot or damage cue must never become exact unseen target identity/position without lawful evidence.
- Communication presentation may simplify information but cannot increase its precision.
- `SEEN` is receipt only; full teammate request -> independent appraisal -> commit/reject -> embodied action remains ADVANCING/PAR for this candidate.
- F6/F7 are explicit accounting proof controls, not final diegetic ecology UX.
- Finished art, voice and fine Focus tuning are not architecture gates.
- Do not merge PR #23 before human-video review and explicit authorization.

## H3

The full longitudinal sweep remains **DEFER WITH TRIGGER: after integrated v1.5 human proof**. Earlier targeted ancestry checks are justified only by concrete regression, semantic drift, duplicate authority or lost capability.
