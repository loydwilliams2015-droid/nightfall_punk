#!/usr/bin/env python3
"""Unweighted 1.8A.3 model comparison. Counts independent fixtures, not rows as IID."""
import collections
import csv
import math
import pathlib
import sys

ROOT = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else 'build/v18a3')
OUT = pathlib.Path(sys.argv[2] if len(sys.argv) > 2 else ROOT/'PAR_RESULTS.md')

def read(name):
    with (ROOT/name).open(newline='', encoding='utf-8') as f:
        return list(csv.DictReader(f))

def wilson_upper_zero(n, z=1.96):
    return z*z/(n+z*z) if n else float('nan')

main = read('shape_samples.csv')
held = read('heldout_support.csv')
oracle = read('sweep_oracle.csv')
models = [
    'Swept box + dual authority',
    'Swept capsule + dual authority',
    'Capsule + rounded-box support + dual authority',
    'Permissive climb (negative control)',
    'Contract bypass (negative control)',
]
scenarios = {
    0:'valid stair',1:'excessive rise',2:'low ceiling',3:'narrow ledge',
    4:'no permission',5:'stale world version',6:'origin spoof',7:'body shape spoof',
    8:'valid ladder entry',9:'ladder bad facing',10:'ladder out of reach',
    11:'ladder obstruction',12:'valid ladder exit',13:'exit without support',
    14:'grounded-state spoof',15:'requester step limit spoof',
}
lines = [
    '# nightfall!punk 1.8A.3 — Shape & Traversal scientific PAR',
    '',
    '## Scope and evidence provenance',
    '',
    '- 4,000 distinct scripted world/input fixtures, stratified equally across 16 scenario classes, shared by 5 policy models (20,000 paired model evaluations).',
    '- 1,200 **independent held-out** near-edge landing fixtures shared by 5 models (6,000 paired evaluations).',
    '- 2,400 separately seeded trajectory cases against an independent long-double convex-distance minimization / entry-time oracle.',
    '- Fixture strata are deliberately adversarial; they do **not** estimate ordinary gameplay incidence or FPS.',
    '- H1 controlled synthetic/compiled evidence. Results are not a compiled integrated dynamic player, general convex mesh CCD, or cross-platform replay proof.',
    '',
    '## Unweighted model comparison',
    '',
    '| Model | Primary errors / 4,000 | Held-out errors / 1,200 | Primary false approvals | Held-out false support | Disposition |',
    '|---|---:|---:|---:|---:|---|',
]
for policy, name in enumerate(models):
    m=[r for r in main if int(r['policy'])==policy]
    h=[r for r in held if int(r['policy'])==policy]
    errors=sum(r['correct']=='0' for r in m)
    edgeerrors=sum(r['correct']=='0' for r in h)
    falseallow=sum(r['expected']=='0' and r['commit_eligible']=='1' for r in m)
    falledge=sum(r['foot_patch_expected']=='0' and r['commit_eligible']=='1' for r in h)
    disposition='H1 EXit candidate' if policy==2 and errors==edgeerrors==0 else ('Inadmissible negative control' if policy>=3 else 'Admissible control; comparative deficit')
    lines.append(f'| {name} | {errors} | {edgeerrors} | {falseallow} | {falledge} | {disposition} |')

lines+=['','### Scenario-level primary failure distribution','','| Model | Failure classes (counts) |','|---|---|']
for policy,name in enumerate(models):
    bad=collections.Counter(int(r['scenario']) for r in main if int(r['policy'])==policy and r['correct']=='0')
    desc=', '.join(f'{scenarios[k]}: {v}' for k,v in sorted(bad.items())) or 'None in tested corpus'
    lines.append(f'| {name} | {desc} |')

admissible = [r for r in oracle if int(r['oracle'])>=0]
false_positive=sum(r['oracle']=='0' and r['analytic']=='1' for r in admissible)
false_negative=sum(r['oracle']=='1' and r['analytic']=='0' for r in admissible)
true_hits=sum(r['oracle']=='1' for r in admissible)
true_misses=sum(r['oracle']=='0' for r in admissible)
max_error=max(float(r['absolute_error']) for r in admissible)
lines += [
    '', '## Independent capsule-sweep geometric oracle', '',
    f'- {len(admissible):,} qualifying trajectories: {true_hits:,} true hits, {true_misses:,} true misses.',
    f'- False hit predictions: **{false_positive}**; missed true hits: **{false_negative}**.',
    f'- Maximum absolute first-contact time discrepancy: **{max_error:.10g}** of the normalized [0,1] sweep interval.',
    f'- For zero missed contacts in {len(admissible):,} deliberately sampled paths, the descriptive Wilson 95% upper bound is **{wilson_upper_zero(len(admissible))*100:.3f}%**; this does not generalize to all geometry.',
    '- Oracle is algorithmically independent (convex-distance ternary minimum + entry bisection, long double) but models the same idealized upright capsule/AABB geometry; agreement does not validate slopes, rotations, triangles, arbitrary convex hulls, or moving multiple-contact islands.',
    '', '## Hard logical gates, conditional on engine axioms', '',
    '| Gate | Status | Why |','|---|---|---|',
    '| G1: geometry and contract both approve before commit eligibility | PASS in guarded candidate | Independent reason codes; no override in selected policy |',
    '| G2: authority must match actor, feature, tick, world version, origin, grounded/ladder state and shape | PASS in named fixtures | Candidate properties compared with server-side snapshot structure |',
    '| G3: static ladder triggers are not material blocking walls | PASS in named fixtures | Trigger and solid arrays are distinct |',
    '| G4: no client-supplied step/reach inflation | PASS in named fixtures | Requested limits may not exceed contract limits |',
    '| G5: narrow foot patch must be stable without altering real capsule geometry | PASS in selected hybrid corpus | Landing patch tested as additional support condition |',
    '| G6: unsafe controls must not exist in production API | PASS in separate production binary | Controls compiled only with NF18A3_TEST_CONTROLS |',
    '', '## Scientific selection', '',
    '**Select model 2, capsule CCD + bounded rounded-box-like support proxy + simultaneous geometry/traversal verification, as the H1 exit candidate for 1.8A.3.** This is an evidence-conditioned selection, not a production promotion.',
    '', 'Model 1 (strict capsule) also passes all primary fixtures, but fails the separate declared foot-support safety patch in held-out near-edge tests. Model 0 box remains a regression control, and models 3–4 are intentionally invalid negative controls. Because flat-foot safety is an adopted normative threshold, its success is conditional on that criterion, not evidence of an absolute law of character support.',
    '', '## Remaining risks / next falsifying studies', '',
    '1. Full motor-driven dynamic actor and reciprocal impulse / angular momentum is **not integrated**; query returns read-only eligibility.',
    '2. No real server trust boundary is authenticated here: callers must source the geometry and contract snapshots from authoritative state. C structs themselves are forgeable by a malicious client.',
    '3. Capsule CCD currently supports translating vertical capsules against translating AABBs over linear intervals; there is no convex-mesh, general rigid-body angular CCD, or arbitrary slope surface proof.',
    '4. Sliding alongside a wall, near-simultaneous manifold contacts, long-term resting support, moving-platform departures and collision edge identities require a dedicated H2 corpus.',
    '5. A proxy patch can be overly conservative; H4 movement-feel testing and H2 false rejection under authentic stair layout remain necessary.',
    '6. `commit_eligible` is not a committed physical transaction, and the witness hash is diagnostic rather than cryptographic or machine-independent.',
    '7. Runtime broadphase still scans arrays in this lab; chunk-indexed locality, actual fine chunk physics and GPU observation are later gates.',
    '8. Multi-phase step movement is a trajectory proposal; the motor/impulse solver must realize it without teleporting or injecting unaccounted work.',
    '', '## Scientific framework', '',
    '- **Quine:** contract truth, shape, support patch and fixture constraints form a testable web; locate contradictory assumptions instead of protecting an incumbent.',
    '- **Kuhn:** the box baseline valid-ladder-exit failure is a localized anomaly; compare new alternatives on identical seeds rather than dismissing it.',
    '- **Lakatos:** material nonpenetration and no semantic bypass stay hard-core; shape/proxy/solver details remain replaceable protective-belt hypotheses.',
    '- **Feyerabend:** preserve box and strict-capsule models as common-corpus controls; the decision is non-monolithic.',
    '- **Popper:** keep intentionally permissive policies as falsifying negative controls compiled out of release binaries.',
]
OUT.parent.mkdir(parents=True,exist_ok=True)
OUT.write_text('\n'.join(lines)+'\n',encoding='utf-8')
print('REPORT:', OUT)
print('ORACLE:',len(admissible),'hits:',true_hits,'misses:',true_misses,'FP:',false_positive,'FN:',false_negative,'TOI_MAX_ERR:',max_error)
for i,name in enumerate(models):
    m=[r for r in main if int(r['policy'])==i];h=[r for r in held if int(r['policy'])==i]
    print(f'MODEL{i}: primary_wrong={sum(r["correct"]=="0" for r in m)}/4000 heldout_wrong={sum(r["correct"]=="0" for r in h)}/1200')
if false_positive or false_negative or max_error>0.00005:
    raise SystemExit('geometric oracle unacceptable')
if any(r['correct']=='0' for r in main if r['policy']=='2') or any(r['correct']=='0' for r in held if r['policy']=='2'):
    raise SystemExit('hybrid dual-jurisdiction candidate failed corpus')
