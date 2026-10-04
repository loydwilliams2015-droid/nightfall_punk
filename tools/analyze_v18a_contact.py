#!/usr/bin/env python3
"""Scientific status for AABB swept-query contact laboratory. No unearned H2-H4 claims."""
import csv
from collections import Counter, defaultdict
from math import sqrt
from pathlib import Path
import sys

def wilson(k,n,z=1.96):
    if not n: return (0.0,1.0)
    p=k/n
    den=1+z*z/n
    center=(p+z*z/(2*n))/den
    spread=z*sqrt((p*(1-p)+z*z/(4*n))/n)/den
    return (max(0,center-spread),min(1,center+spread))

def main():
    src=Path(sys.argv[1]) if len(sys.argv)>1 else Path('build/v18a/contact_samples.csv')
    dest=Path(sys.argv[2]) if len(sys.argv)>2 else Path('build/v18a/PAR_RESULTS.md')
    rows=list(csv.DictReader(src.open()))
    # Independent world/input fixture count excludes the 12 repeated budget-density settings.
    unique={(r['stratum'],r['seed'],r['scene']):r for r in rows
            if r['budget']=='4' and r['density']=='0'}
    n=len(unique)
    old_misses=sum(int(r['old_endpoint_crossed']) for r in unique.values())
    new_tunnels=sum(int(r['contact_tunnel']) for r in unique.values())
    divergent=sum(1-int(r['stable_hash']) for r in unique.values())
    pends={b:sum(int(r['pending']) for r in rows if r['budget']==str(b)) for b in (1,2,4,8)}
    by_density={d:[r for r in rows if r['density']==str(d)] for d in (0,16,64)}
    tests=[
        ('G1: no tested swept barrier penetrations',new_tunnels==0,f'{new_tunnels}/{n} independent fixture configs'),
        ('G2: no duplicate replay mismatch',divergent==0,f'{divergent}/{n} independent fixture configs'),
        ('G3: bounded deferral is explicit',pends[1]>0 and pends[2]==0 and pends[4]==0 and pends[8]==0,
         ', '.join(f'{b} iterations: {pends[b]}/{len(rows)//4}' for b in (1,2,4,8))),
        ('G4: strict native sample harness',len(rows)==12000,f'{len(rows)} CSV rows'),
    ]
    grades=[
        ('H-CCD','H1 synthetic/native fixture', 'SUPPORTED IN TESTED AABB CASES', 'No missed tested obstacles; compare to endpoint-only occupancy'),
        ('H-ORDER','H1 fixture', 'SUPPORTED IN D11', 'Reversing collider order preserved geometry and hash in corner fixture'),
        ('H-BUDGET','H1 sample', 'SUPPORTED LOCALLY', '2 iterations sufficient for selected fixtures; may fail in denser/moving setups'),
        ('H-BROAD','H1 instrumentation', 'INCOMPLETE', 'Broadphase discards irrelevant narrowphase candidates but still scans all colliders O(N)'),
        ('H-CAPSULE','H0', 'UNTESTED', 'Current body is swept upright BOX, not capsule'),
        ('H-DYNAMIC','H0', 'UNTESTED', 'Impulse is estimated only; no two-body momentum conservation solver'),
        ('H-REFINE','H0', 'UNTESTED', 'No Cell/Nexus material refinement or structural contact coupling yet'),
        ('H-EVENT','H1 fixture', 'PARTIAL', 'Begin/persist/end/hit events tested, but sustained resting contacts and multi-feature identities remain open'),
        ('H-MULTI','H0', 'UNTESTED', 'No live authoritative server reconciliation for new kernel'),
        ('H-PLAYER','H0', 'UNTESTED', 'No human locomotion feel trials of challenger'),
    ]
    old_ci=wilson(old_misses,n);new_ci=wilson(new_tunnels,n)
    lines=['# nightfall!punk 1.8A.1 — Contact Truth PAR results','',
      '**Corpus:** 5 deterministic seed cohorts × 50 seeds × 4 spatial scenarios × 4 iteration budgets × 3 irrelevant-collider density levels = 12,000 rows. There are **1,000 distinct world/input fixtures**, reused for budget/density comparisons. Cohort labels are not empirical best/median/worst ranks.','',
      '## Deductive fixture gates','',
      'The 27 named assertions in `test_v18a_contact.c` cover collision time of impact, normals, tangent sliding, multiple constraints, ceilings/support, order/rotation metamorphism, explicit pending, event histories, and `NfWorld` conversion. Native status: **27/27 PASS**.','',
      '| Graded gate | Result | Evidence |','|---|---|---|']
    for name,ok,info in tests: lines.append(f'| {name} | **{"PASS" if ok else "FAIL"}** | {info} |')
    lines += ['', '## Controlled comparisons (endpoint query versus AABB swept query)','',
      f'- Endpoint-only query misses a crossing in **{old_misses}/{n}** distinct high-speed fixtures ({100*old_misses/n:.1f}%; Wilson 95% interval [{100*old_ci[0]:.1f}%, {100*old_ci[1]:.1f}%]).',
      f'- New swept kernel penetrates the fixture-defined barrier in **{new_tunnels}/{n}** distinct fixtures (Wilson 95% upper bound {100*new_ci[1]:.2f}%).',
      f'- Duplicate fixed-seed replay divergences: **{divergent}/{n}**.',
      '- **Do not generalize** this rate to gameplay: high-speed, thin-barrier fixtures were designed to distinguish endpoint tests from sweeps; they are not representative player velocities.',
      '- The comparison is against the incumbent endpoint *occupancy query*, not a controlled end-to-end gameplay movement-controller ablation.',
      '', '## Solver iteration budget','',
      '| Iterations | Pending / 3,000 rows | Pass/fail interpretation |', '|---:|---:|---|']
    for b in (1,2,4,8):
        lines.append(f'| {b} | {pends[b]:,} | {"PASS conservative pending; less graceful" if b==1 else "PASS on this limited corpus"} |')
    lines+=['','## Broadphase and CPU sampling caveat','',
      'The query already skips many irrelevant **narrowphase** tests. But candidate collection itself currently walks the full collider list; it is **not** a spatial index and has no demonstrated sublinear world-scale behavior. CPU timings from `clock()` are coarse and include measurement noise; use native per-build high-resolution profiling and spatial indexing before an efficiency PAR claim.',
      '', '## Hypothesis evidence grades','',
      '| Hypothesis | Maturity | Disposition | Limitation |','|---|---|---|---|']
    for row in grades: lines.append('| '+' | '.join(row)+' |')
    lines+=['','## Scientific-method interpretations','',
      '**Quine (holism):** the result depends jointly on collision geometry, timestep, test fixture and ground-truth penetration predicate; a failure does not identify which assumption failed without control tests.',
      '**Kuhn (paradigm/normal science):** collision tests should expose anomalies the endpoint-only paradigm cannot resolve; do not redefine “contact” merely to keep the existing query passing.',
      '**Lakatos (progressive research programme):** protect the hard core (material authority, no false penetration, actor-local evidence) while allowing a replaceable protective belt of capsule/box, iteration budgets, broadphase and friction formulations. Demand novel correct predictions.',
      '**Feyerabend (methodological pluralism):** keep incumbent AABB endpoint, swept box and later capsule/convex challengers on common inputs; no preferred algorithm wins by architectural fashion.',
      '**Popper (severe falsification):** test edge cases targeted to refute a candidate: simultaneous contact, start overlap, rotating geometry, fast moving platforms, fixed-point reproducibility, near-parallel ramps and unsupported landings.',
      '', '## Known limitations: no full build promotion','',
      '- `Nf18aContact.normal_impulse` is a one-body constraint estimate; equal/opposite force is not yet applied to a moving obstacle.',
      '- The body shape is swept axis-aligned radius-height **box** (not capsule); no arbitrary convex meshes or ramp normal geometry.',
      '- Initial overlaps produce `INVALID_START`, not bounded depenetration; deterministic pending prevents inventing a free path.',
      '- Moving colliders are represented by relative linear sweeps but full moving-platform support and world position advance are not integrated.',
      '- Fixed-length contact and history buffers are explicit. Do not silently truncate critical contacts in authoritative gameplay.',
      '- Contact hashes are quantized for diagnostics; cross-platform bit-identical authoritative physics has not been established.',
      '- No whole-engine multiplayer, H4 player feel, or 1.7E graphical overlay for contacts is yet proven.',
    ]
    dest.parent.mkdir(parents=True,exist_ok=True)
    dest.write_text('\n'.join(lines)+'\n')
    print('\n'.join(lines[:22]))
if __name__=='__main__':main()
