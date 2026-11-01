# VPC-01 — Neutral human observation pilot (proposed)
**Scope:** one self-observation and optionally 2–3 independent voluntary testers.
**Purpose:** collect uncoached first-person observations; identify specific defects and disagreements without preloading the designer's hypotheses.
**Status:** protocol only. No collected human responses. No release-approved game build.

## Tester-facing prompt (verbatim, before ANY ratings or diagnostic questioning)

> Please explore the material in whatever way feels natural to you. When you're finished, describe your experience in your own words.
>
> What did you notice? What happened as you explored? What stands out to you afterward?
>
> You can discuss anything you consider relevant, in as much or as little detail as you wish. There are no expected answers, and you don't need to agree with anyone else's impressions.
>
> If you'd like to add anything else, please do.

Neutral optional follow-ups only: “Could you tell me more about that?”; “What happened next?”; “Is there anything else you'd like to add?” Do not affirm, debate, lead, or suggest an ideal answer.

## Preparation and informed participation
- Request voluntary informed consent; allow stopping, refusing recording and skipping questions. Collect no sensitive demographic data.
- Record a pseudonymous tester ID, date, hardware/display only as needed, exact source SHA/build and scene/seed.
- **Label the medium correctly:** concept art, captured gameplay video, or a verified playable build. Never represent generated reference art as gameplay output. The H60 two-actor research viewer is not an accepted multiplayer playtest.
- Give all participants the same material, scene and minimal actual operating instructions. Do not disclose physics/design targets or terms such as collision predictability, darkness, biopunk or the intended response until after independent observations have been recorded.
- Do not coach exploration. Capture the free narrative first without scores or structured prompts; include any spontaneous positives, negatives, confusions or unexpected observations as given.

## Optional diagnostic follow-up (strictly AFTER the unprimed narrative)
If a participant reports a specific event, ask them to demonstrate what they did and what they observed again, without suggesting a cause. For reproducible defects, record steps, initial conditions, expected/observed behavior *in the participant's words*, frequency, severity, corroboration and source-linked gate. Keep the participant's subjective reaction distinct from an engineer's causal diagnosis.

Do not add forced ratings or closed-choice questions to the first-contact protocol. If separately authorized later, a secondary task-specific survey can use an explicitly scoped scale with N/A and neutral anchors, and should follow free observation, not precede it.

## VPC-01 model and evidence boundaries
See `docs/VPC01_PRIMORDIAL_TWO_ACTOR.md`. The intentional new benchmark is the two-actor+pod physical situation; inherited greybox tests remain regression controls, not proof of this experience. Ask about playable control, interaction or real-time physics only when those features have actually been implemented and independently verified in the distributed executable.

## RET / RET-HK handling
- Distinguish (a) observed facts/reproductions, (b) testers' interpretations and preferences, (c) engineering hypotheses, and (d) tested mechanisms.
- Two or three volunteer accounts are exploratory qualitative evidence, not a representative sample or statistical certification. Never calculate an unjustified p-value, effect size or confidence interval.
- Preserve quotations, consent status, source SHA, issue ownership, and unresolved uncertainty. No fictional responses.
- Production, promotion to 1.8C.3 and human distribution remain HOLD until mandatory engineering and availability gates pass.
