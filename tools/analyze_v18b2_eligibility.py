#!/usr/bin/env python3
"""Validate complete B2 runs and report native measurements without H3/H4 claims."""
import csv
import json
import math
from collections import defaultdict
from pathlib import Path
import sys

COUNTS = {"canonical": 2048, "randomized": 8192, "pathological": 1024,
          "boundary": 2048, "held-out": 8192}


def percentile(values, q):
    values = sorted(values)
    return values[max(0, math.ceil(len(values) * q) - 1)]


def main(root):
    buckets = defaultdict(list)
    semantic = {}
    runs = sorted(root.glob("run*-*.csv"))
    if len(runs) != 6:
        raise SystemExit("Expected three complete calibration/held-out pairs")
    for path in runs:
        seen = set()
        with path.open(newline="") as stream:
            for row in csv.DictReader(stream):
                corpus, model = row["corpus"], row["model"]
                key = (corpus, int(row["case"]), model)
                if key in seen:
                    raise SystemExit(f"Duplicate result: {path}: {key}")
                seen.add(key)
                if not math.isfinite(float(row["ns"])) or float(row["ns"]) < 0:
                    raise SystemExit("Invalid native timing")
                state = tuple(row[k] for k in ("seed", "flags", "expected_status", "status",
                                               "expected_reason", "reason", "unsafe", "correct", "replay"))
                if key in semantic and semantic[key] != state:
                    raise SystemExit(f"Cross-run semantic divergence: {key}")
                semantic[key] = state
                buckets[corpus, model].append(row)
        expected = ["held-out"] if "held-out" in path.name else list(COUNTS)[:-1]
        required = {(c, i, f"M{m}") for c in expected for i in range(COUNTS[c]) for m in range(5)}
        if seen != required:
            raise SystemExit(f"Incomplete selected corpus: {path}")
    result = []
    for (corpus, model), rows in sorted(buckets.items()):
        timings = [float(r["ns"]) for r in rows]
        result.append({"corpus": corpus, "model": model, "evaluations": len(rows),
                       "case_ids": COUNTS[corpus],
                       "unsafe": sum(int(r["unsafe"]) for r in rows),
                       "mismatches": sum(not int(r["correct"]) for r in rows),
                       "replay_discrepancies": sum(not int(r["replay"]) for r in rows),
                       "p50_ns": percentile(timings, .5), "p95_ns": percentile(timings, .95),
                       "p99_ns": percentile(timings, .99),
                       "mean_geometry_queries": sum(int(r["geometry_queries"]) for r in rows) / len(rows),
                       "mean_material_queries": sum(int(r["material_queries"]) for r in rows) / len(rows)})
    with (root / "comparison.csv").open("w", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=list(result[0]), lineterminator="\n")
        writer.writeheader()
        writer.writerows(result)
    admissible = [m for m in ("M3", "M4") if all(
        r["unsafe"] == r["mismatches"] == r["replay_discrepancies"] == 0
        for r in result if r["model"] == m)]
    hold = {r["model"]: r for r in result if r["corpus"] == "held-out"}
    chosen = "M4" if "M4" in admissible and (
        "M3" not in admissible or hold["M4"]["p50_ns"] <= hold["M3"]["p50_ns"]) else (
        "M3" if "M3" in admissible else None)
    report = {"scope": "new-source authored-domain native laboratory; not historical B1 reproduction",
              "indexed_cases": sum(COUNTS.values()), "models": 5, "timing_repeats": 3,
              "model_case_evaluations_per_repeat": sum(COUNTS.values()) * 5,
              "admissible": admissible, "exit_candidate": chosen, "release_exit": "BLOCKED",
              "unresolved": ["historical B1 archive", "live object/credential registry and tick-owner integration",
                             "durable multi-object reservation", "network recovery and cross-tick fairness",
                             "integrated frame timing and human responsiveness"], "rows": result}
    (root / "comparison.json").write_text(json.dumps(report, indent=2) + "\n")
    lines = ["# 1.8B.2 executed comparative results", "",
             f"{sum(COUNTS.values()):,} indexed authored cases × 5 models = "
             f"{sum(COUNTS.values()) * 5:,} model-case IDs; 3 dependent timing repeats.",
             "Case IDs do not imply distinct stimuli: boundary repeats two one-ULP states 1,024 times each.",
             "No independent game-world failure probability or formal proof is inferred.", "",
             "| Corpus | Model | Mismatches (3 repeats) | Unsafe grants | p50 ns | p95 ns | p99 ns |",
             "|---|---|---:|---:|---:|---:|---:|"]
    for r in result:
        lines.append(f"| {r['corpus']} | {r['model']} | {r['mismatches']} | {r['unsafe']} | "
                     f"{r['p50_ns']:.1f} | {r['p95_ns']:.1f} | {r['p99_ns']:.1f} |")
    lines += ["", f"Selected scoped laboratory candidate: **{chosen or 'NONE'}**.",
              "Truth gates precede cost. M3 remains the admissible exhaustive counterpoint.",
              "Timings include clock overhead amortized over eight calls; fixture construction and I/O excluded.",
              "These are distributions of batch-average CPU times, not individual-action tail latency.",
              "Three repeats assess local run stability, not independent replication or confidence intervals.",
              "A finite-radius conservative visibility query can deny marginal surface interactions.",
              "Movement support is stationary only; moving support is honestly blocked.",
              "Full release exit **BLOCKED** on the individually listed integration/provenance/H4 gates."]
    (root / "RESULTS.md").write_text("\n".join(lines) + "\n")
    print(json.dumps({k: report[k] for k in ("indexed_cases", "admissible", "exit_candidate", "release_exit")}))
    if not chosen:
        raise SystemExit(1)


if __name__ == "__main__":
    main(Path(sys.argv[1]))
