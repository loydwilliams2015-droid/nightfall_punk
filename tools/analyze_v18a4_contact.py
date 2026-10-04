#!/usr/bin/env python3
"""Audit controlled H1 contact model comparisons, never equate them with gameplay performance."""
import csv
import math
import statistics
import sys
from collections import defaultdict
from pathlib import Path

def main():
    if len(sys.argv) != 3:
        raise SystemExit('usage: analyzer samples.csv report.md')
    path, output = map(Path, sys.argv[1:])
    with path.open(newline='', encoding='utf-8') as inp:
        data = list(csv.DictReader(inp))
    required = {'cohort','seed','scenario','policy','status','applied','tangent_impulse',
                'angular_speed','crate_delta_speed','linear_momentum_residual',
                'energy_change','iterations','reserve','pending','replay_mismatch',
                'constraint_evaluations'}
    if not data or required - set(data[0]):
        raise SystemExit('missing fields: '+repr(required-set(data[0])))
    sets = defaultdict(list)
    for r in data:
        for key in ('status','applied','iterations','reserve','pending','replay_mismatch','constraint_evaluations'):
            r[key] = int(r[key])
        for key in ('tangent_impulse','angular_speed','crate_delta_speed',
                    'linear_momentum_residual','energy_change'):
            r[key] = float(r[key])
            if not math.isfinite(r[key]):
                raise SystemExit('nonfinite sample: '+repr(r))
        sets[r['scenario'],r['policy']].append(r)
    policies = ['estimate_only_control','reciprocal_linear','angular_normal',
                'friction_fixed6','friction_adaptive']
    assert len(data)==8000, 'sample count changed; re-evaluate corpus definition'
    assert all(len(sets['pair',p])==1000 for p in policies)
    assert all(len(sets['island_single',p])==1000 for p in policies[3:])
    assert all(len(sets['chain',p])==500 for p in policies[3:])
    def f(r,k):return r[k]
    lines = [
        '# nightfall!punk 1.8A.4 — Dynamic contact: scientific PAR',
        '',
        '## Provenance and denominator',
        '',
        '- Native strict-C source experiment: 1,000 shared random-parameter point-contact fixtures, 1,000 matching single-contact island fixtures, and 500 held-out three-body chain fixtures.',
        '- Five direct-contact policies; fixed-six and adaptive also evaluated in both island corpora. **8,000 model evaluations, but only 1,500 distinct parameter fixtures** across the two fixture families; single-island reuse is paired evaluation, not an independent replicate.',
        '- Five deterministic seed cohorts; two within-process runs per case for repeatability. Not a representative incidence estimate for production FPS play.',
        '- Angular inertia is axis-aligned at this instant, not evolved orientation; the swept adapter is an upright capsule versus translating axis-aligned box, not generalized angular-motion CCD.',
        '- Contact-island acceptable approach-velocity residual: <=0.005 m/s for this H1 test. Maximum six normal-contact passes.',
        '',
        '## Unweighted model comparison',
        '',
        '| Policy | Real crate change / 1000 | Angular response / 1000 | Friction response / 1000 | Max linear p residual (kg m/s) | Highest contact energy change (J) | Classification |',
        '|---|---:|---:|---:|---:|---:|---|'
    ]
    for p in policies:
        a=sets['pair',p]
        applied=sum(x['applied'] for x in a)
        angular=sum(x['angular_speed']>0.001 for x in a)
        friction=sum(x['tangent_impulse']>0.001 for x in a)
        maxp=max(x['linear_momentum_residual'] for x in a)
        maxe=max(x['energy_change'] for x in a)
        classification = ('INVALID negative control: impulse estimated without body response'
                          if p == policies[0] else
                          'Admissible point-normal control; omits angular and friction'
                          if p == policies[1] else
                          'Admissible angular control; omits friction'
                          if p == policies[2] else
                          'Admissible fixed-iteration comparator'
                          if p == policies[3] else
                          '**Selected H1 laboratory candidate**')
        lines.append(f'| {p} | {applied} | {angular} | {friction} | {maxp:.7f} | {maxe:.4f} | {classification} |')
    lines += ['', '## Iteration and constraint-work comparison', '',
        '| Scenario | Policy | Matched fixtures | Accepted | Pending | Mean passes | Mean constraint evaluations | Reserve invoked |',
        '|---|---|---:|---:|---:|---:|---:|---:|']
    for scen in ('island_single','chain'):
        for pol in policies[3:]:
            rr=sets[scen,pol]
            lines.append(f'| {scen} | {pol} | {len(rr)} | '
                         f'{sum(x["status"]==0 for x in rr)} | '
                         f'{sum(x["pending"] for x in rr)} | '
                         f'{statistics.mean(x["iterations"] for x in rr):.3f} | '
                         f'{statistics.mean(x["constraint_evaluations"] for x in rr):.3f} | '
                         f'{sum(x["reserve"] for x in rr)} |')
    weighted_by_work = {}
    for pol in policies[3:]:
        rr=sets['island_single',pol]+sets['chain',pol]
        weighted_by_work[pol]=sum(x['constraint_evaluations'] for x in rr)/len(rr)
    reduction = 100*(1-weighted_by_work['friction_adaptive']/weighted_by_work['friction_fixed6'])
    lines += ['',f'For the **predeclared 1,000 single-contact + 500 chain mixture**, average constraint evaluations are {weighted_by_work[policies[3]]:.3f} (fixed-six) versus {weighted_by_work[policies[4]]:.3f} (adaptive), a {reduction:.1f}% reduction **in evaluations, not measured milliseconds**. Fixed-six is defined to execute its six passes; adaptive may terminate at the first satisfied constraint set.',
              '', '## Deductive / hard PAR', '', '| Gate | Result |', '|---|---|']
    all_replay=sum(x['replay_mismatch'] for x in data)
    tests=[
        ('Zero duplicate same-fixture replay mismatches',all_replay==0),
        ('Estimate-only control does not receive physical impulse',all(x['applied']==0 for x in sets['pair',policies[0]])),
        ('All tested paired true-dynamics models apply both-body impulses', all(x['applied']==1 and x['crate_delta_speed']>0 for p in policies[1:] for x in sets['pair',p])),
        ('Angular models register off-center response',all(x['angular_speed']>0 for p in policies[2:] for x in sets['pair',p])),
        ('Friction models register tangential response',all(x['tangent_impulse']>0 for p in policies[3:] for x in sets['pair',p])),
        ('Pairwise linear momentum residual <= 0.001 kg*m/s',all(x['linear_momentum_residual']<=.001 for p in policies[1:] for x in sets['pair',p])),
        ('Contact kinetic energy never increases beyond 0.01 J in selected corpus',all(x['energy_change']<=.01 for p in policies[1:] for x in sets['pair',p])),
        ('Both bounded contact island policies accept all tested islands',all(x['status']==0 for sc in ('island_single','chain') for p in policies[3:] for x in sets[sc,p])),
        ('No sample exceeds six island iterations',all(x['iterations']<=6 for sc in ('island_single','chain') for p in policies[3:] for x in sets[sc,p])),
        ('Adaptive uses reserve on the harder chain corpus',all(x['reserve']==1 for x in sets['chain',policies[4]])),
    ]
    for label,good in tests:lines.append(f'| {label} | {"PASS" if good else "FAIL"} |')
    if not all(g for _,g in tests):
        raise SystemExit('scientific PAR hard gate FAIL: '+repr([n for n,g in tests if not g]))
    lines += ['', '## A priori (conditional design axioms) and falsification', '',
        '1. A proposed traversal may not become a committed movement without the simultaneous physical-geometry and traversal-contract jurisdictions (1.8A.3). The new trusted staging interface **re-adjudicates**, checks actor origin and tick/version, and authorizes bounded motor input without teleporting.',
        '2. For an isolated contact pair, every internally applied normal/tangential impulse must be reciprocal in linear momentum. The tested residual measures floating-point error, not a claim of universal exact arithmetic.',
        '3. A predicted numerical impulse without a material update is not sufficient to qualify as dynamic contact; estimate-only is retained as a negative control.',
        '4. Unresolved contact islands must not partially commit or silently become free space. A deliberately deep eight-body chain is tested for explicit PENDING with unchanged caller state.',
        '5. Motor work is separately recorded from contact energy change. External input energy is not a closed-system energy violation.',
        '', '## Five-model scientific adjudication', '',
        '- **Negative control:** estimate-only fails the requirement of actual crate momentum change despite a plausible numeric impulse.',
        '- **Linear:** physically reciprocal, but omits torque and friction for off-center/surface contacts.',
        '- **Angular-normal:** corrects a missing mechanical degree of freedom; tangential dynamics remain absent.',
        '- **Fixed-six:** supports linear/angular and Coulomb tangential impulses; succeeds but consumes all declared iterations by definition.',
        '- **Adaptive:** same tested physics family, with bounded early termination and explicit reserve. It is the H1 exit candidate, not a universally proven optimal solver.',
        '', '## Epistemic limitations and next falsifiers', '',
        '- **H1 demonstrated:** point-contact impulse response actually changes two bodies; normal/angular/friction decomposition; no mutation on invalid input; stable contact ordering; tested chain reserve; trusted dual-jurisdiction traversal staging; receipt-to-summary adapter.',
        '- **Not yet H2:** no independent complex-body held-out scenario showing material parity beyond the simple fixed/contact chains; current sample generator reuses deterministic parameter families and cannot estimate live incidence.',
        '- **Not yet H3:** not the production player/AI body pipeline; orientation is not integrated, angular CCD and rotated rigid-body meshes unsupported, resting friction and persistent multibody support unsolved, and no network reconciliation or hardware-time profiling.',
        '- **Not yet H4:** no human movement-feel tests. Explicit post-transaction authoritative commit and durable external event persistence remain separate stages.',
        '- **Critical next falsifiers:** very deep stacks, cradle/multi-contact geometries, stair and ladder motor realization under real obstructions, rotating crate contacts, fixed-point/cross-platform replay, long-duration friction, high-mass disparity, compression, and network client/server disagreement.',
        '', '## Scientific reasoning', '',
        '- **Quine:** an impulse failure may be caused by a collider normal, inertia approximation, solver order, fixture setup, or timestep; isolate each assumption.',
        '- **Kuhn:** the estimate-only controller cannot account for material crate motion; retaining it as a control makes the anomaly measurable.',
        '- **Lakatos:** protect material authority and reciprocal contact while revising motor gains, friction model, and solver budget as defeasible auxiliaries.',
        '- **Feyerabend:** keep linear, angular, fixed-six and adaptive challengers comparable; avoid a monopoly of one fashionable implementation.',
        '- **Popper:** target worst-case contact islands and other tests likely to break the selected model, not only easy two-body impacts.',
        '', '**Decision:** Select **motor-driven, reciprocal angular-plus-friction contact with 2→4→6 bounded adaptive solver** as the **1.8A.4 H1 laboratory exit candidate**. Do not promote to production player physics or claim full six-degree-of-freedom rigid-body completeness.', '']
    output.parent.mkdir(parents=True,exist_ok=True)
    output.write_text('\n'.join(lines),encoding='utf-8')
    print('PAR=PASS gates=',len(tests),' rows=',len(data),' distinct_world_parameter_fixtures=1500')
    for key in ('island_single','chain'):
        for p in policies[3:]:
            vals=sets[key,p]
            print(key,p,'pass=',sum(x['status']==0 for x in vals),'of',len(vals),
                  'evaluations_mean=',round(statistics.mean(x['constraint_evaluations'] for x in vals),3))
    print('work_reduction_percent=',round(reduction,3),' replay_mismatches=',all_replay)
if __name__=='__main__':main()
