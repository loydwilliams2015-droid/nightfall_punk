#!/usr/bin/env python3
import csv
import statistics
import sys
from collections import Counter
from pathlib import Path

source=Path(sys.argv[1]);out=Path(sys.argv[2]);rows=list(csv.DictReader(source.open()))
cohorts=Counter(int(r['cohort']) for r in rows)
max_error=max(float(r['momentum_error']) for r in rows)
shape_diff=sum(r['box_overlap']!=r['capsule_overlap'] for r in rows)
retry=Counter(int(r['adaptive_attempts']) for r in rows)
summary=Counter(int(r['history_summaries']) for r in rows)
events={k:[int(r['history_events']) for r in rows if int(r['cohort'])==k] for k in sorted(cohorts)}
checks=[
    ('1000 fixed-seed rows with five equal cohorts',len(rows)==1000 and all(cohorts[i]==200 for i in range(5))),
    ('two-body linear momentum residual <=0.003 kg m/s',max_error<=0.003),
    ('rounded capsule vs box distinguishable in edge/step sample',shape_diff>0),
    ('adaptive solver remains within three bounded attempts',all(1<=i<=3 for i in retry)),
    ('periodic contact summaries emitted',all(int(r['history_summaries'])>0 for r in rows)),
    ('some severe-impact cohort results exceed mild cohort',statistics.median(events[2])>statistics.median(events[0])),
]
text=[
    '# nightfall!punk 1.8A.2 — Collision policy evidence',
    '',
    '**Evidence grade: H1 laboratory only; not integrated multiplayer/player H3.**',
    '',
    f'- Independent seeded input configurations: **{len(rows)}** (five cohorts × 200).',
    f'- Shape classification contrasts (swept-box occupancy vs static capsule): **{shape_diff}/{len(rows)}**.',
    f'- Maximum linear momentum residual in impulse fixture: **{max_error:.8f} kg m/s**.',
    f'- Adaptive attempts distribution: **{dict(sorted(retry.items()))}**. Current corpus does not force reserve passes.',
    f'- Periodic summary count distribution: **{dict(sorted(summary.items()))}**.',
    '- Median persistent event counts by cohort: '+str({k:statistics.median(v) for k,v in events.items()}),
    '',
    '| Gate | Result |','|---|---|'
]
text.extend(f'| {label} | {"PASS" if passed else "FAIL"} |' for label,passed in checks)
text += [
    '',
    '## Limits that remain unresolved',
    '- True swept capsule/rounded-box hybrid narrowphase is **not** integrated; static capsule/AABB distance and gated profile policy only.',
    '- The equal-and-opposite impulse fixture resolves one normal between two point-mass linear bodies, not rotating rigid bodies or full islands.',
    '- Motor integration is a testable component; no player-world authoritative collision integration yet.',
    '- Chunk cache accepts authoritative 4×4 canonical and 8×8/16×16 fine data, but game-world streaming/physical broadphase wiring remains open.',
    '- The in-memory event ring **is not durable long-term storage**. Persist promoted events outside this ring before production use.',
    '- 15-tick summaries (250ms at 60Hz), 60-tick sampled-duration, and impulse thresholds are starting priors, not proven optima.',
    '- Long-term threshold history must not fabricate causally necessary events when compression loses supporting low-level evidence.',
    '- This corpus has no human feel, network-prediction, stress-of-contact island, or cross-machine reproducibility evidence.',
]
out.parent.mkdir(parents=True,exist_ok=True);out.write_text('\n'.join(text)+'\n')
print('\n'.join(text))
if not all(ok for _,ok in checks):raise SystemExit(1)
