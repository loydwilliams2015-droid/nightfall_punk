# nightfall!punk — v1.4 Human Acceptance Handoff

Date: 2026-08-14
Branch: `build/v1.4-relational-combat-communications`
Status: **DRAFT HUMAN-VIDEO GATE**

## FROM

v1.4 begins from the exact green v1.2c corrective candidate:

`27af894fb9148be11447b8e26320bf18307645d6`

v1.3 is intentionally unused.

The new v1.2c human recording verified the integrated proof surface: Player 1, teammate 1, Human Rivals 2, Dream Cattlers 2, preserved first-person camera and measurable teammate displacement. The positive ALTER_ROUTE/reroute sequence was not cleanly closed by that recording and is intentionally made easier to prove in v1.4 through an explicit debug proof setup.

## v1.4 thesis

> **Information spreads; cognition does not copy itself. Coordination claims possibilities; it does not own them forever.**

v1.4 protects existing combat/movement truth while advancing incomplete-information communication and temporal coordination.

## Final candidate identity

Fill only after exact-head CI is green:

- exact head: `PENDING_FINAL_GREEN_HEAD`
- workflow: `nightfall v1.4 CI`
- run: `PENDING_FINAL_GREEN_RUN`
- PR: `PENDING_DRAFT_PR`

Do not merge before the Pop!_OS human-video pass is reviewed.

## What changed

- explicit fixed-capacity `NfReportBus`;
- report scopes: private / Crew / Rival Crew / public;
- report provenance: origin / reporter / ancestry / hops;
- report time: origin / issue / delivery / expiry;
- coarse unseen position reports rather than exact transforms;
- explicit Player Crew ALTER_ROUTE report;
- delayed report acquisition with negative pre-delivery control;
- Human Rival exact instant squad blackboard removed;
- Rival evidence classified DIRECT / AUDIBLE / REPORT;
- fixed-capacity `NfClaimTable`;
- soft versus hard temporal claims;
- cover claim migration;
- graded three-height cover exposure;
- stable ALTER_ROUTE gate semantic key/revision;
- dedicated v1.4 relational tests;
- dedicated v1.4 human proof executable.

## Deliberate non-goals

No v1.4 acceptance depends on:

- player-visible weapon/viewmodel;
- player reload/fire/switch UI;
- full reciprocal playable conflict;
- new weapon families;
- deliberate deception;
- generalized GOAP;
- radio/jamming infrastructure;
- universal mutable-object replication;
- large map expansion.

The player weapon/viewmodel/full playable conflict frontier remains v1.5.

## Run locally

```bash
cd ~/Downloads/nightfall_punk_v12 && \
git fetch origin && \
git switch build/v1.4-relational-combat-communications && \
git pull --ff-only && \
chmod +x v14.sh && \
./v14.sh standard-check && \
./v14.sh build && \
./v14.sh test && \
./v14.sh demo
```

If the branch is not known in the local clone, repair fetch scope once:

```bash
cd ~/Downloads/nightfall_punk_v12 && \
git config remote.origin.fetch '+refs/heads/*:refs/remotes/origin/*' && \
git fetch origin && \
git switch --track origin/build/v1.4-relational-combat-communications && \
chmod +x v14.sh && \
./v14.sh standard-check && \
./v14.sh build && \
./v14.sh test && \
./v14.sh demo
```

## Human proof controls

- F1 — Player View
- F2 — Observer / Live
- F3 — Debug Top / Live
- F4 — **PROOF SETUP**
- F5 — full reset
- mouse — look
- WASD — move
- Shift — sprint
- Ctrl — crouch
- Space — jump
- E — interact with ALTER_ROUTE gate

F4 is an explicit debug/laboratory operation. It relocates the Player and teammate into a controlled proof arrangement and clears route-report memory so the communication sequence can be recorded. It is not diegetic teleport gameplay.

## Recommended recording sequence

### A. Identity + integrated population

1. Start recording before/at launch.
2. Confirm the title reads `nightfall!punk v1.4`.
3. Keep HUD visible.
4. Confirm Player / TEAM / RIVAL / CATTLER counts.
5. Give a short first-person 360-degree survey.

### B. Bounded Crew-report proof

1. Press **F4**.
2. Confirm:
   - `GATE ... rev=1 ... CLOSED`;
   - teammate `EVIDENCE NONE`;
   - teammate `BELIEF UNCHANGED`;
   - Crew report `NONE`.
3. Hold **E** at the purple gate.
4. Capture:
   - interaction claim `GRANTED`;
   - gate revision changes from `1` to `2`;
   - gate becomes `OPEN`;
   - Crew report first reads `PENDING`;
   - before delivery, teammate still has no artificial report knowledge.
5. Keep recording through the bounded delay.
6. Capture:
   - Crew report becomes `LIVE`;
   - teammate evidence becomes `CREW_REPORT`;
   - belief becomes `OPEN`;
   - gradient/utility/commitment respond;
   - physical trajectory/displacement changes.

### C. Rival information / claim diagnostics

During ordinary movement, try to keep both red Rivals in or near view for several seconds. The HUD may show:

- `DIRECT` when firsthand sight exists;
- `AUDIBLE` after relevant semantic sound evidence;
- `REPORT` when one Rival receives a delayed coarse report from another;
- live Rival report count;
- report ID / ancestry / hops;
- live cover-claim count;
- selected cover exposure.

You do not need to force every Rival state in one take. Deterministic tests are authoritative for exact delay/scope/coarsening/provenance; the video is principally a legibility/integration proof.

### D. Optional camera regression

Use `F2 -> F3 -> F1` once if convenient. Camera regression is inherited and automated, so do not sacrifice the bounded-report proof to make a long camera video.

## What to look for

### PASS indicators

- title says v1.4;
- integrated population remains present;
- first-person movement/camera remains coherent;
- F4 cleanly creates the proof arrangement;
- gate revision changes only after legitimate interaction;
- report visibly passes through PENDING before LIVE;
- teammate does not learn from an undelivered report;
- delivered report changes evidence/belief without becoming world truth;
- teammate subsequently produces embodied action;
- Rivals remain behaviorally alive after blackboard removal;
- no catastrophic claim deadlock/clumping.

### Report as errors if seen

- teammate learns route state while report is PENDING and without direct/encounter evidence;
- report exposes exact unseen transforms as if firsthand;
- report never delivers;
- gate revision changes without gate state change;
- temporal claims never expire/release;
- Rivals stop moving/acting entirely;
- Rival reports create obvious instant squad telepathy;
- camera regression;
- population regression;
- crash/hang.

## After video

Send the recording as-is even if something fails. A visible failure is useful causal evidence. The next disposition should be KEEP / AMEND / DEFER, not silent patching.
