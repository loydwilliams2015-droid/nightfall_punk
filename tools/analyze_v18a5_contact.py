#!/usr/bin/env python3
"""Unweighted, stratified H1 comparisons. This is not a game-play survey."""
import csv
from collections import defaultdict
from pathlib import Path
import sys

src = Path(sys.argv[1] if len(sys.argv)>1 else 'build/v18a5/model_samples.csv')
out = Path(sys.argv[2] if len(sys.argv)>2 else 'build/v18a5/PAR_RESULTS.md')
rows=list(csv.DictReader(src.open(newline='',encoding='utf-8')))
assert len(rows)==10000, f'expected 10000 model evaluations, got {len(rows)}'
models=defaultdict(list)
for r in rows:
    models[(r['family'],r['model'])].append(r)
assert len(models)==10 and all(len(r)==1000 for r in models.values())

def n(rr,key):return sum(int(x[key]) for x in rr)
def val(rr,key):return sum(float(x[key]) for x in rr)/len(rr)

lines=[
 '# nightfall!punk 1.8A.5 — Canonical Grid and Condensed History: H1 PAR',
 '',
 '## Experimental design',
 '',
 '* 1,000 indexed grid fixture parameters × 5 models × 12 spatial queries = 60,000 grid query observations.',
 '* 1,000 indexed contact episodes × 5 history models = 5,000 contact-history model runs.',
 '* The two families together provide 10,000 model evaluations. Reused seeds/episodes are **paired**, not independent observations.',
 '* Cohorts 0–3 (800 parameter sets) are development strata and cohort 4 (200) is a held-out seed stratum. These are **not** independently ranked best/worst/pathological samples.',
 '* Grid scenarios use one planar mixed region, one canonical solid region, and ordinary canonical free space in an actual 3D chunk. Fine geometry is 0.25 m and is independently checked against the coarse witness.',
 '* History episodes deliberately distinguish peak impacts, distributed small impulses, prolonged resting load, duration, and harmless contacts. Ground truth is computed from fixture parameters, not the tested implementation.',
 '',
 '## Grid policy comparison',
 '',
 '| Policy | Accurate / 12,000 | False FREE | False SOLID | Explicit PENDING | Fine loads / 1,000 | Mean resident B | Held-out false FREE |',
 '|---|---:|---:|---:|---:|---:|---:|---:|',
]
for name in ('canonical_only','eager_fine','selective_fine','hysteretic_fine','unsafe_missing_fine'):
    rr=models['grid',name]
    hold=[r for r in rr if int(r['cohort'])==4]
    lines.append(f'| {name} | {n(rr,"correct_or_accurate"):,} | {n(rr,"false_free_or_detected"):,} | '
                 f'{n(rr,"false_block_or_truth"):,} | {n(rr,"pending_or_loss"):,} | '
                 f'{n(rr,"loaded_or_retained"):,} | {val(rr,"resident_bytes"):.0f} | {n(hold,"false_free_or_detected"):,} |')
lines += [
 '',
 '### Spatial hard gate',
 '',
 '**Hard zero:** false FREE from unavailable/inconsistent fine data, canonical material contradiction, and silent capacity overflow. Explicit PENDING is safe but may be costly for gameplay responsiveness.',
 'In this intentionally adversarial corpus the unsafe cache-miss control violates the hard gate. Eager fine and the hysteretic policy obey it; eager fine has higher resident bytes while hysteretic loading causes a nonzero unresolved-query rate. Neither outcome alone is sufficient to declare a universal optimal policy.',
 '',
 '## History policy comparison',
 '',
 '| Policy | Correct episodes / 1,000 | Relevant episodes detected / 800 | False events | Raw retained / total | Promoted events | Simulated ACKs |',
 '|---|---:|---:|---:|---:|---:|---:|',
]
for name in ('raw_all','peak_only','windows_only','cumulative_ack','overwrite_control'):
    rr=models['history',name]
    tp=sum(1 for r in rr if int(r['false_free_or_detected']) and int(r['false_block_or_truth']))
    false=sum(1 for r in rr if int(r['false_free_or_detected']) and not int(r['false_block_or_truth']))
    lines.append(f'| {name} | {n(rr,"correct_or_accurate"):,} | {tp:,} | {false:,} | '
                 f'{n(rr,"loaded_or_retained"):,} | {n(rr,"events_generated"):,} | {n(rr,"durable_acked"):,} |')
lines += [
 '',
 '### History hard gate',
 '',
 '**Hard zero:** lost consequential committed events, synthetic continuity through unsampled gaps, replay/owner/version duplicates, promotion of uncommitted impulses, and acknowledgment before durable append.',
 'Peak-only and isolated-window policies miss the cumulative-force and duration episodes by construction. Full raw retention is a scientifically useful control but does not satisfy bounded long-term memory indefinitely. Overwrite control can match recall below queue capacity but violates the hard gate once the outbox fills.',
 '',
 'The sample CSV column `durable_acked` counts **simulated acknowledgments only**. Separate native POSIX sink fixtures test fsync + ACK, crash-between-append-and-ACK idempotence, and fail-closed handling of torn records; this is not evidence of a replicated durable datastore.',
 '',
 '## Conditional a priori deductions',
 '',
 '| Proposition | Conditional truth | Testable implementation consequence |',
 '|---|---|---|',
 '| A missing detail cache proves a mixed material voxel is free | FALSE | Missing/stale fine result is PENDING or conservative SOLID |',
 '| A known canonical solid may be contradicted by unsigned fine data | FALSE | Refuse contradictory fine payload |',
 '| Loading fine cache changes material-world authority | FALSE | Separate material epoch from cache revision |',
 '| A solver proposal is equivalent to committed contact history | FALSE | Require applied dynamic receipt and monotone world commit |',
 '| A resting normal force is identical to a repeated impact | FALSE | Maintain `force × time` independently of discrete impulse sum |',
 '| An ACK alone implies persistence | FALSE | Sink must validate, append, flush, sync and only then acknowledge |',
 '| Digests permit perfect recovery of discarded raw samples | FALSE | Event captures consequential aggregate, not complete forensic trace |',
 '',
 '## Exit-candidate decision',
 '',
 '**Grid: hysteretic selective refinement with strict canonical witness, explicit PENDING, pinned hot chunks, and separate cache/material epochs** is the H1 candidate. Retain eager-fine as high-fidelity control and conservative coarse-only as low-cost control. Its pending rate requires an H3 responsiveness budget and a synchronous/on-demand escalation rule for consequential queries.',
 '',
 '**History: 15-tick periodic summaries → distinct peak / cumulative impulse / duration / resting-load thresholds → bounded outbox → fsync/ACK** is the H1 candidate. Retain complete raw-record and window-only policies as controls. Do not promote this to fully durable world history without authoritative-world WAL, transactional coupling and storage-recovery tests.',
 '',
 '## Evidence limitations',
 '',
 '* H1 strict native component tests and synthetic comparisons only. Repeated experiment types do not establish 1,000 independent topologies or a generalized failure probability.',
 '* Physical geometry is a 3D occupancy/material voxel model. A voxel occupancy query is not a completed CCD collider generator, support graph, or material process solver.',
 '* `nf18a5_world_commit` is a local copy-on-write history/world-version gate, not the full production transaction manager or a persisted authoritative world.',
 '* The sink is a POSIX single-process append demonstration; it does not handle all torn-write recovery, multiwriter exclusivity, process-crash atomic world-state commit or replicated storage.',
 '* Fine loading requires supplied authoritative data. Memory measurements count resident voxel bytes only, not metadata, allocator, or OS cache.',
 '* Cross-platform bit-identical fixed-point physics and multiplayer network reconciliation remain unproven.',
 '* Hardware frame latency, world-scale indexed broadphase, 1.8A.4 motor-body integration and H4 human gameplay need separate experiments.',
 '',
 '## Scientific rationale',
 '',
 '**Quine:** occupancy and history conclusions rely on shared coordinate, tick, threshold and source-validation assumptions. **Kuhn:** cache-driven false clearance and suppressed cumulative load are anomalies in competing designs. **Lakatos:** retain hard material truth and causal provenance while revising the fine-residency and summary protective belt. **Feyerabend:** keep eager-fine, coarse and raw-all controls instead of prematurely eliminating alternatives. **Popper:** challenge the favorite with missing-data, stale epochs, backpressure, gaps, pinned-cache pressure, and torn-write negative controls.',
]
# Abort rather than promote if the preferred models violate hard gates.
for name in ('hysteretic_fine','eager_fine','selective_fine','canonical_only'):
    assert n(models['grid',name],'false_free_or_detected')==0, f'unsafe grid candidate {name}'
assert n(models['grid','unsafe_missing_fine'],'false_free_or_detected')>0
assert n(models['history','cumulative_ack'],'correct_or_accurate')==1000
assert n(models['history','peak_only'],'correct_or_accurate')<1000
out.parent.mkdir(parents=True,exist_ok=True)
out.write_text('\n'.join(lines)+'\n',encoding='utf-8')
print('\n'.join(lines[:23]))
print('PAR analyzer: PASS')
