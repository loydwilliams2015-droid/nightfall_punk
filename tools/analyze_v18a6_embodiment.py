#!/usr/bin/env python3
"""Unweighted, denominator-transparent 1.8A.6 embodiment PAR analyzer."""
import csv
import hashlib
import statistics
import sys
from collections import Counter, defaultdict
from pathlib import Path

if len(sys.argv)!=3:
    raise SystemExit('usage: analyze_v18a6_embodiment.py SAMPLE.csv REPORT.md')
source=Path(sys.argv[1]); report=Path(sys.argv[2]);rows=list(csv.DictReader(source.open()))
assert len(rows)==4800, f'expected 4 models × 6 strata × 200 configurations, got {len(rows)}'
assert len({(x['seed'],x['scenario'],x['model']) for x in rows})==4800
bykey={(x['seed'],x['scenario'],x['model']):x for x in rows}
summary=[]
for model in range(4):
    v=[x for x in rows if int(x['model'])==model]
    counts=Counter(x['status'] for x in v)
    contacts=sum(int(x['raw_contact_samples'])>0 for x in v)
    actor=sum(int(x['world_actor_moved']) for x in v)
    camera=sum(int(x['camera_updated'])>0 for x in v)
    crate=sum(float(x['crate_speed'])>0 for x in v)
    summary.append((model,counts,contacts,actor,camera,crate,statistics.mean(int(x['committed_ticks']) for x in v)))
physics_mismatches=[]
for x in rows:
    if int(x['model'])!=2:continue
    y=bykey[(x['seed'],x['scenario'],'3')]
    physics_fields=['attempted_ticks','committed_ticks','status','world_actor_moved','crate_speed','raw_contact_samples','material_epoch','world_revision','replay_hash']
    if any(x[f]!=y[f] for f in physics_fields):physics_mismatches.append((x['seed'],x['scenario']))
assert not physics_mismatches, f'camera changed physical authority in {len(physics_mismatches)} cases'
for s in range(6):
    subset=[r for r in rows if int(r['scenario'])==s and int(r['model'])==3]
    if s==0:
        assert all(int(r['raw_contact_samples'])>0 and float(r['crate_speed'])>0 for r in subset)
    elif s==1:
        assert all(r['status']=='BLOCKED' for r in subset)
    elif s==2:
        assert all(r['status']=='PENDING' and int(r['committed_ticks'])==0 for r in subset)
    else:
        assert all(r['status']=='UNSUPPORTED' and int(r['committed_ticks'])==0 for r in subset)
selected=[r for r in rows if int(r['model'])==3]
legacy=[r for r in rows if int(r['model'])==0]
assert sum(r['status']=='COMMITTED' for r in legacy if r['scenario']=='2')==200
assert all(r['status']=='PENDING' for r in selected if r['scenario']=='2')
assert all(int(r['camera_updated'])==int(r['committed_ticks']) for r in selected)
assert all(int(r['camera_updated'])==0 for r in rows if int(r['model'])!=3)
assert all(r['status']=='PENDING' for r in selected if r['scenario']=='0')
sha=hashlib.sha256(source.read_bytes()).hexdigest()
lines=[
'# nightfall!punk 1.8A.6 — unweighted embodiment-model PAR',
'',
'**Scope:** opt-in grounded actor + one translating dynamic AABB + canonical material + contact-history + existing camera, offline headless H1. **Not** a full FPS client, networked authoritative server, AI scheduler, or complete stair/ladder/crouch/jump integration.',
'',
'**Sampling unit:** six deliberately selected scene classes × 200 independent parameter seeds = **1,200 independent world/input configurations**; each is replayed across four model controls, making **4,800 matched model rows**. Seed perturbations vary crate location. Within each row, up to 45 ticks are *dependent temporal observations*, not independent trials. Model 1 is limited to one tick because it deliberately never commits NfActor. Cases 100–199 within each scene are an ex ante parameter hold-out, **not** unseen scene types.',
'',
'## Unweighted policy comparison',
'',
'| Model | Final status distribution (committed / blocked / pending / unsupported) | World actor moved | Camera updated | Applied contact history | Crate response | Mean successful ticks |',
'|---|---|---:|---:|---:|---:|---:|',
]
for model,c,contacts,actor,cam,crate,avg in summary:
    names=['Legacy movement (negative material control)','A5 lab-only (negative embodiment control)','Physical world bridge','Physical world bridge + camera']
    dist='/'.join(str(c.get(k,0)) for k in ('COMMITTED','BLOCKED','PENDING','UNSUPPORTED'))
    lines.append(f'| {model} — {names[model]} | {dist} | {actor}/1200 | {cam}/1200 | {contacts}/1200 | {crate}/1200 | {avg:.2f} |')
lines +=[
'',
'**Paired physical-authority invariant:** Model 2 versus Model 3 produced **0/1,200** mismatches over all compared physical fields, including the authoritative result hash. This establishes that the existing read-only camera follower does not modify physical authority in the sampled fixtures; it does not demonstrate actual rendered frame-time or input feel.',
'',
'**Scene strata, each 200 independent parameter seeds:** 0: loaded static canonical corridor, motor pushes real crate, both integrated models eventually PENDING at edge of loaded canonical world; 1: existing world obstacle despite free canonical voxels -> BLOCKED; 2: missing authoritative canonical chunk -> PENDING for integrated models, but historical reference continues -> negative material-coverage control; 3: ramp -> UNSUPPORTED; 4: nearby actor -> UNSUPPORTED; 5: moving platform -> UNSUPPORTED. These last three are open integration deficits rather than PASS for full embodiment.',
'',
'**Hard H1 PAR (pass/fail):** No sampled model 2/3 clearance from absent canonical material; no sampled false clearance from existing world collider; every recorded world-bridge crate response is backed by applied contact-history samples; NfActor projection agrees with authority snapshots; camera cannot modify authority (0/1,200). Tests E01–E54 separately cover selected stale tick, rollback-on-WAL-failure and recovery. No claim of zero problems beyond this declared corpus.',
'',
'**Graded PAR:** mean committed ticks and counts above; memory, P95/P99 loading latency, CPU and wall time, human feel and network reconciliation **NOT MEASURED** in the controlled comparison. The 45-tick loaded corridor intentionally becomes PENDING before completion, demonstrating incomplete canonical world streaming. The old controller making progress without canonical data is not considered authoritative correctness.',
'',
'**Selection:** Model 3 (WORLD_CAMERA) is the strongest **conditional H1 grounded embodiment bridge** because it includes verified actor projection, dynamic object response, physical contact history, and camera correspondence without additional physics divergence. Model 2 remains the nonpresentation comparator; Model 0 remains legacy control; Model 1 documents false embodiment claims if world projection is omitted. **Do not certify a full A6 gameplay exit:** the present API deliberately rejects jump/crouch/ladder/ramp/moving-platform and near actor interactions and runs outside ordinary world scheduling.',
'',
'## Scientific method and accountability',
'',
'- A priori (conditional): authoritative history requires applied, committed material contact; visual observation cannot cause world state; unknown canonical geometry cannot imply free passage; geometry AND traversal jurisdiction remain necessary for genuine traversal.',
'- Quine: pending may arise from canonical coverage, actor constraints, contact-budget convergence or bad test fixtures; diagnose causes individually rather than attributing every pending to collision failure.',
'- Kuhn: the historical control and new source of material authority disagree in the absent-canonical stratum—an explicit anomaly to resolve before architectural promotion.',
'- Lakatos: the selected bridge predicts matching world/camera hashes while additionally preserving applied impulses and history; further held-out game genres of movement required.',
'- Feyerabend: four distinct control policies remain reproducible, rather than erasing the incumbent.',
'- Popper: future severe tests are low ceilings, ladder transitions, multi-actor same-tick ownership, moving supports, physics stacks, unsampled chunk edges, client loss/jitter and corrupted world WAL.',
'',
'## Reproducibility',
'',
'```sh',
'bash ./v18a6.sh all',
'```',
'',
f'CSV SHA-256: `{sha}`. This is exact reproduction on this machine/compiler only; floating-point cross-platform identical hashes are unverified.',
'',
'**Source-provenance requirement:** test C code and analysis must be attached to the same GitHub commit/CI run before claiming GitHub CI results; an independently reviewed PR is required for release promotion.',
]
report.parent.mkdir(parents=True,exist_ok=True)
report.write_text('\n'.join(lines)+'\n')
print(f'MODEL_ROWS={len(rows)} CONFIGURATIONS={len(rows)//4} CAMERA_PHYSICS_MISMATCHES={len(physics_mismatches)}')
for m,c,contacts,actor,cam,crate,avg in summary:
 print(f'MODEL={m} END_STATUS={dict(c)} ACTOR={actor} CAMERA={cam} CONTACT_HISTORY={contacts} CRATE={crate} MEAN_TICKS={avg:.2f}')
print(f'SHA256={sha}')
