# VPC-01 — Human observation pilot (proposed)
**Scope:** 1 self-observation + 2–3 independent voluntary testers.
**Purpose:** find defects and disagreements; NOT statistical certification.
**Status:** survey plan only; no human responses collected or implied.

## Session hygiene
- Obtain informed, voluntary consent; avoid collecting sensitive demographic details.
- Record pseudonymous tester ID, session date, actual build/source SHA, platform,
  approximate FPS familiarity, hardware and display only as needed.
- Identify material accurately: **concept art** vs **recorded gameplay** vs
  **playable build**. Never present generated reference imagery as engine output.
- Start with open observation before suggesting expected camera/art/physics outcomes.
- Give all reviewers the same references/task script; retain the order.
- If collecting video or telemetry, request separate opt-in and allow declining.

## Reference-only visual review (available before the gameplay slice)
Show the three recent postcyberpunk/biopunk direction images and clearly label
them aspirational reference art. Do not ask about experienced controls, FPS or
simulation quality from a static image.

Open prompts:
1. What kind of world and play experience do these images suggest?
2. What feels readable, distinctive, confusing or derivative?
3. Does the lighting balance beauty and danger? Where is it too dark or grimy?
4. What might distinguish these creatures from generic cyborg zombies?
5. Which visual promises could be difficult for the gameplay to satisfy?

Optional image-specific 1–5 ratings (with N/A): atmosphere, world legibility,
character readability, biome variety, aesthetic coherence.
Ask for one concrete strength, one concern and one improvement, in their words.

## Playable build review (only for features **actually implemented**)
Give each tester an identical executable, documented SHA, seed and scene.
Use up to three currently functional encounters: traversal/support,
single-object physical PUSH, and multi-actor interaction if playable.
Do not ask them to evaluate third-person controls until third-person is built.
Observe 10–20 minutes without coaching, then ask:

1. Where was navigation or targeting unclear?
2. When did physical outcomes disagree with what you expected to see?
3. Which collisions, interactions or interruptions felt predictable/unpredictable?
4. What felt responsive or delayed, with specific examples?
5. What should change first, and why?
6. Could you identify enemy intent and teammate effects without reading debug UI?

Optional 1–5 ratings (N/A allowed): spatial readability, control response,
physical consistency, camera comfort, enemy silhouette/intent,
environmental legibility, cooperative clarity.

## Observation record
- Tester ID / material type / SHA / scene / seed / hardware
- Reproduction steps / expected / actual / severity / confidence
- Direct quote or concise observation (label subjective judgment)
- Whether another tester independently reported it
- Issue owner / source link / gate mapping / follow-up result

## RET / RET-HK acceptance
- Separate observed facts, hypotheses, aesthetic preferences, and bugs.
- Two or three testimonials are exploratory *qualitative pilot evidence*, not a
  representative sample, effect size estimate, confidence interval or p-value.
- Repeated serious observations flag hypotheses for engineering tests; do not
  automatically promote or reject a feature based on votes.
- No artificial ML p-values; uncertainties near 1 remain unresolved until a
  correctly designed statistical study.
- Keep production and 1.8C.3 HOLD until all mandatory source-exact gates pass.
