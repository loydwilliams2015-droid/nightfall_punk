#!/usr/bin/env python3
import csv
import json
from pathlib import Path
import sys

root = Path(sys.argv[1] if len(sys.argv) > 1 else 'build/b3-evidence')
rows = []
semantic = ('state', 'motor_steps', 'contact_receipts', 'material_effects', 'events',
            'inventory_delta', 'health', 'salvage', 'tokens', 'grants', 'slots',
            'frames', 'semantic_hash', 'correct', 'unsafe', 'conservation')
replays, pairs = {}, {}
for repeat in range(1, 4):
    batch = list(csv.DictReader((root / f'run{repeat}.csv').open()))
    assert len(batch) == 2256
    ids = set()
    for row in batch:
        key = (row['corpus'], row['case'], row['seed'], row['variant'], row['kind'], row['model'])
        assert key not in ids, key
        ids.add(key)
        values = tuple(row[s] for s in semantic)
        assert replays.setdefault(key, values) == values, ('replay mismatch', repeat, key)
        if row['model'] != 'M0':
            paired = (repeat, *key[:-1])
            assert pairs.setdefault(paired, values) == values, ('paired mismatch', paired)
    rows.extend(batch)

def percentile(values, q):
    values = sorted(values)
    return values[min(len(values)-1, int((len(values)-1)*q))]

models = {}
for model in ('M0', 'M3', 'M4'):
    selected = [r for r in rows if r['model'] == model]
    report = {'dependent_rows': len(selected), 'authored_errors': sum(r['correct'] != '1' for r in selected),
              'unsafe_effects': sum(int(r['unsafe']) for r in selected),
              'conservation_violations': sum(r['conservation'] != '1' for r in selected),
              'frame_overruns': sum(int(r['frame_overruns']) for r in selected)}
    tp = fp = fn = 0
    for row in selected:
        expected = row['variant'] in ('0', '13')
        observed = any(int(row[field]) != 0 for field in
            ('motor_steps', 'contact_receipts', 'material_effects', 'inventory_delta', 'health'))
        tp += expected and observed
        fp += not expected and observed
        fn += expected and not observed
    report['case_material_effect_detection'] = {'true_positive': tp, 'false_positive': fp, 'false_negative': fn,
        'precision': tp / (tp + fp) if tp + fp else None, 'recall': tp / (tp + fn) if tp + fn else None,
        'scope': 'Authored case-level effect presence, not per-contact or general physics accuracy; forces separately checked by fixtures'}
    for corpus in ('all', 'held-out', 'positive'):
        subset = selected if corpus == 'all' else [r for r in selected if
            (r['variant'] == '0' if corpus == 'positive' else r['corpus'] == corpus)]
        report[corpus] = {field: {f'p{int(q*100)}': percentile([int(r[field]) for r in subset], q)
            for q in (.5, .95, .99)} for field in ('proposal_ns', 'total_owner_ns', 'max_frame_ns', 'frames')}
    models[model] = report
assert models['M0']['authored_errors'] > 0 and models['M0']['unsafe_effects'] > 0
for model in ('M3', 'M4'):
    assert models[model]['authored_errors'] == models[model]['unsafe_effects'] == models[model]['conservation_violations'] == 0
out = {'indexed_fixture_ids': 752, 'model_case_ids_per_repeat': 2256,
       'timing_repeats_dependent': 3, 'paired_semantics_and_replay': 'PASS',
       'models': models, 'selection': 'M4 provisional; assess whole-owner tails and separate release gates',
       'scope': 'Headless single-actor real owner CLOCK_MONOTONIC elapsed time, including scheduling; no H4, general rigid-body or B4/B5 inference'}
(root / 'comparison.json').write_text(json.dumps(out, indent=2) + '\n')
print(json.dumps(out, indent=2))
