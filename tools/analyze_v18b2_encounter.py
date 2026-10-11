#!/usr/bin/env python3
"""Paired independent truth and semantic replay gates; actual owner CPU time."""
import csv
import json
from pathlib import Path
import sys

root = Path(sys.argv[1] if len(sys.argv) > 1 else 'build/b2-resume-evidence')
all_rows = []
semantic = ('expected_state', 'state', 'expected_grant', 'grant', 'balance',
            'inventory', 'slots', 'receipts', 'history', 'world_tick', 'correct', 'unsafe')
replay = {}
pair = {}
for repeat in range(1, 4):
    rows = list(csv.DictReader((root / f'encounter-run{repeat}.csv').open()))
    assert len(rows) == 1344, (repeat, len(rows))
    ids = set()
    for row in rows:
        key = (row['corpus'], row['case'], row['seed'], row['variant'], row['model'])
        assert key not in ids, ('duplicate case', key)
        ids.add(key)
        values = tuple(row[s] for s in semantic)
        assert replay.setdefault(key, values) == values, ('semantic replay', repeat, key)
        paired = (repeat, *key[:-1])
        assert pair.setdefault(paired, values) == values, ('M3/M4 mismatch', paired)
    all_rows.extend(rows)
assert len(replay) == 1344
def percentile(values, fraction):
    values = sorted(values)
    return values[min(len(values)-1, int((len(values)-1)*fraction))]
models = {}
for model in ('M3', 'M4'):
    rows = [r for r in all_rows if r['model'] == model]
    models[model] = {'rows_dependent_repeats': len(rows),
        'authored_errors': sum(r['correct'] != '1' for r in rows),
        'unsafe_publications': sum(int(r['unsafe']) for r in rows),
        'grants': sum(int(r['grant']) for r in rows),
        'frame_overruns': sum(int(r['frame_overrun']) for r in rows)}
    for corpus in ('all', 'held-out', 'published'):
        selected = rows if corpus == 'all' else [r for r in rows if
            (r['grant'] == '1' if corpus == 'published' else r['corpus'] == corpus)]
        models[model][corpus] = {field: {f'p{int(fraction*100)}': percentile(
            [int(r[field]) for r in selected], fraction) for fraction in (.5, .95, .99)}
            for field in ('proposal_ns', 'tick_ns', 'total_ns')}
report = {'indexed_fixture_ids': 672, 'model_case_ids_per_repeat': 1344,
          'dependent_timing_repeats': 3, 'semantic_replay_and_pairs': 'PASS',
          'models': models, 'causal_positive_metrics': 'See independent EEC05 fixtures; no positive contacts in this one-frame corpus',
          'selection': 'M4 retained provisionally, subject to separate acceptance gates'}
(root / 'encounter-comparison.json').write_text(json.dumps(report, indent=2) + '\n')
print(json.dumps(report, indent=2))
assert all(m['authored_errors'] == 0 and m['unsafe_publications'] == 0 for m in models.values())
