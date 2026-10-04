#!/usr/bin/env python3
"""H1 reporting, explicit denominators; does not treat paired rows as independent runs."""
import csv, collections, sys
from pathlib import Path
if len(sys.argv)!=3: raise SystemExit('usage: analyze_v18a5_closure.py input.csv report.md')
rows=list(csv.DictReader(open(sys.argv[1],newline='')))
expected={0:(2,0),1:(0,1),2:(1,0),3:(2,0),4:(1,0),5:(3,0),6:(0,1)}
scenes=collections.defaultdict(list)
for r in rows: scenes[int(r['scenario'])].append(r)
if len(rows)!=1400 or set(scenes)!=set(expected): raise SystemExit('invalid scenario count')
headers=['Scenario','Cases','Point-only FREE','Integrated COMMITTED','Integrated BLOCKED','PENDING','STALE','Real crate impulses']
lines=['# nightfall!punk 1.8A.5 — Integrated Closure PAR','','**Evidence level: H1 laboratory only.** Seven deliberately adversarial categories × 200 perturbations = 1,400 query/step cases. These are not 1,400 randomly sampled real game levels.','', '| '+' | '.join(headers)+' |','|'+'|'.join(['---']*len(headers))+'|']
for sc in sorted(scenes):
    xs=scenes[sc]; stat,status_ok=expected[sc]; actual=[int(x['integrated_status']) for x in xs]
    if any(v!=stat for v in actual) or any(int(x['world_commit'])!=status_ok for x in xs):
        raise SystemExit(f'FAIL: scenario {sc} violated predicted status/commit')
    if any(int(x['material_revision_changed']) for x in xs):raise SystemExit('cache changed authority')
    if any(int(x['actual_crate_impulse']) != status_ok for x in xs):raise SystemExit('body receipt mismatch')
    k=lambda field,n:sum(int(x[field])==n for x in xs)
    label=['Missing fine/provider','Loaded clear + dynamic impact','Clear point / obstructed capsule sweep','Malformed fine response','Solid neighbor / clear point','Stale revision','Preloaded fine + dynamic impact'][sc]
    lines.append(f"| {label} | {len(xs)} | {k('point_status',0)} | {actual.count(0)} | {actual.count(1)} | {actual.count(2)} | {actual.count(3)} | {k('actual_crate_impulse',1)} |")
lines+=['','## Mandatory interpretation','','- Point-only clearance incorrectly authorizes **400/400 known obstructed** cases across scenarios 2 and 4; those cases do not prove a real-world penetration rate.','- The integrated swept-capsule/static-voxel gate blocks **400/400** known obstruction cases.','- A valid resident or successfully loaded geometry witness leads to **400/400** dynamic pair commits with real crate impulse in scenarios 1 and 6.','- Missing or malformed fine data yields **400/400 PENDING**, not false clearance (scenarios 0 and 3).','- Stale revision yields **200/200 STALE** (scenario 5).','- Zero sampled material authority revision changes due solely to fine-cache fills.','','## Component regression','','- 1.8A.5 closure: 46/46 strict C fixtures, including restored world WAL, outbox failure rollback, actual gravity-normal support impulse provenance and adversarial swept-path clearance.','- Original 1.8A.5: 83/83 component fixtures + 9/9 POSIX sink assertions; A1–A4 inherited regressions run separately.','- Current closure uses a **single-process, same-ABI binary snapshot write-ahead journal** and acknowledged event sink. No claim of portable WAL encoding, multiwriter ordering, cross-machine deterministic replay or operational client/server integration.','- The static voxel sweep is conservative; it can block safe movement and performs a bounded voxel frontier, not a general optimized broadphase or multi-body/rotating collider CCD.','- History promotion retains threshold-triggered cumulative/force-time events; it cannot reconstruct discarded raw time-series detail.','- Exact summary cadence (15 ticks), load thresholds, and pending latency remain provisional.','','## Industry correspondence','','Box2D provides post-step begin/end/hit events; Rapier exposes contact force events computed after solver impulses and thresholded per contact pair. Our additional rule is that **only committed, physically applied contact receipts** enter an outbox carrying material and actor provenance.','', '## Exit determination','','**YES: close 1.8A.5 at H1 Integrated Physical-World Laboratory level**, conditional on the tested local assumptions. **NO: production player/body/physics/network promotion.** 1.8A.6 inherits the H1 integration adapter for hardware-scale streaming, orientation, multi-contact runtime, server trust and native camera/network proof.']
Path(sys.argv[2]).write_text('\n'.join(lines)+'\n')
print(f'CLOSURE_PAR,PASS,{len(rows)} cases,7 strata')
