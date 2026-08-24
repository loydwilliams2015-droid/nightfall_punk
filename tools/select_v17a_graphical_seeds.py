#!/usr/bin/env python3
import csv
import statistics
import sys
from collections import defaultdict

if len(sys.argv) != 3:
    raise SystemExit("usage: select_v17a_graphical_seeds.py samples.csv selections.csv")

samples_path, out_path = sys.argv[1:]
with open(samples_path, newline="", encoding="utf-8") as f:
    rows = [r for r in csv.DictReader(f) if int(r["replicate"]) == 0]

by_lab_seed = defaultdict(dict)
for row in rows:
    by_lab_seed[(row["lab"], int(row["seed"]))][row["policy"]] = row

labs = sorted({lab for lab, _ in by_lab_seed})
selected = []

def fv(row, key):
    return float(row[key])

def iv(row, key):
    return int(row[key])

def quality(row):
    return (
        120.0 * iv(row, "success")
        + fv(row, "health_remaining")
        - 1.40 * fv(row, "exposure_accum")
        - 0.80 * fv(row, "ecology_cost_accum")
        - 0.08 * fv(row, "steps")
        - 0.25 * fv(row, "revisits")
    )

def pathology(row):
    return (
        120.0 * (1 - iv(row, "success"))
        + 1.00 * fv(row, "revisits")
        + 2.00 * fv(row, "retreats")
        + 0.05 * fv(row, "topology_updates")
        + 0.45 * fv(row, "exposure_accum")
        + 0.25 * fv(row, "ecology_cost_accum")
        + 0.25 * max(0.0, 30.0 - fv(row, "health_remaining"))
    )

def disagreement(informed, greedy):
    return (
        80.0 * abs(iv(informed, "success") - iv(greedy, "success"))
        + 1.20 * abs(fv(informed, "exposure_accum") - fv(greedy, "exposure_accum"))
        + 0.70 * abs(fv(informed, "health_remaining") - fv(greedy, "health_remaining"))
        + 0.30 * abs(fv(informed, "ecology_cost_accum") - fv(greedy, "ecology_cost_accum"))
        + 0.15 * abs(fv(informed, "steps") - fv(greedy, "steps"))
    )

for lab in labs:
    candidates = []
    for (lab_name, seed), policies in by_lab_seed.items():
        if lab_name != lab or "informed" not in policies or "greedy" not in policies:
            continue
        candidates.append((seed, policies["informed"], policies["greedy"]))
    if len(candidates) < 5:
        raise SystemExit(f"not enough candidates for lab {lab}")

    used = set()
    exposures = [fv(informed, "exposure_accum") for _, informed, _ in candidates]
    median_exposure = statistics.median(exposures)

    ranked = {
        "median": sorted(candidates, key=lambda x: (abs(fv(x[1], "exposure_accum") - median_exposure), x[0])),
        "best": sorted(candidates, key=lambda x: (-quality(x[1]), x[0])),
        "worst": sorted(candidates, key=lambda x: (quality(x[1]), x[0])),
        "pathological": sorted(candidates, key=lambda x: (-pathology(x[1]), x[0])),
        "disagreement": sorted(candidates, key=lambda x: (-disagreement(x[1], x[2]), x[0])),
    }

    for category in ("median", "best", "worst", "pathological", "disagreement"):
        pick = next(item for item in ranked[category] if item[0] not in used)
        seed, informed, greedy = pick
        used.add(seed)
        if category == "median":
            score = abs(fv(informed, "exposure_accum") - median_exposure)
        elif category == "best":
            score = quality(informed)
        elif category == "worst":
            score = quality(informed)
        elif category == "pathological":
            score = pathology(informed)
        else:
            score = disagreement(informed, greedy)
        selected.append((lab, category, seed, score))

with open(out_path, "w", newline="", encoding="utf-8") as f:
    w = csv.writer(f)
    w.writerow(["lab", "category", "seed", "selection_score"])
    for lab, category, seed, score in selected:
        w.writerow([lab, category, seed, f"{score:.6f}"])

print(f"v1.7A graphical seed selector: {len(selected)} canonical seeds across {len(labs)} labs -> {out_path}")
if len(selected) != 40:
    raise SystemExit(f"expected 40 selected seeds, got {len(selected)}")
