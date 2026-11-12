#!/usr/bin/env python3
"""PEB-1 H277: source-exact A6/B6/current common-subset Pac-Man physics comparison.

This is a headless single-actor physical regression probe, NOT ghost AI, a
general multi-actor solver, B2/B3 accepted transaction test, or 1.7x native
comparison. Never convert differences into inferred improvements.
"""
import argparse
import json
import re
from pathlib import Path

PATTERN = re.compile(
    r"^RESULT family=(\d+) seed=(\d+) commit=(\d+) pending=(\d+)"
    r" reject=(\d+) contact=(\d+) first_stop=(\d+) hash=(\d+)"
    r" history=(\d+) x=(-?[\d.]+) z=(-?[\d.]+)$"
)
FIELDS = ("commit", "pending", "reject", "contact", "first_stop",
          "hash", "history", "x", "z")
EXPECTED = {(family, seed) for family in range(5) for seed in range(32)}

def parse(path):
    raw = Path(path).read_text(encoding="utf-8")
    if "NATIVE_H1_SCENARIOS=160 PASS" not in raw:
        raise ValueError(f"missing success marker: {path}")
    scenarios = {}
    for line in raw.splitlines():
        found = PATTERN.fullmatch(line.strip())
        if not found:
            continue
        values = list(found.groups())
        key = (int(values[0]), int(values[1]))
        if key in scenarios:
            raise ValueError(f"duplicate scenario {key} in {path}")
        record = {name: float(value) if name in ("x", "z") else int(value)
                  for name, value in zip(FIELDS, values[2:])}
        if record["commit"] + record["pending"] + record["reject"] > 30:
            raise ValueError(f"invalid tick count: {key}")
        if record["contact"] > record["commit"]:
            raise ValueError(f"contacts exceed commits: {key}")
        if record["pending"] + record["reject"] > 1:
            raise ValueError(f"multiple terminal outcomes: {key}")
        scenarios[key] = record
    if set(scenarios) != EXPECTED:
        raise ValueError(f"wrong coverage in {path}: {len(scenarios)}")
    return scenarios

def stats(rows):
    return {
        "scenarios": len(rows),
        "committed_ticks": sum(r["commit"] for r in rows.values()),
        "pending_terminations": sum(r["pending"] for r in rows.values()),
        "rejected_terminations": sum(r["reject"] for r in rows.values()),
        "contact_observations": sum(r["contact"] for r in rows.values()),
    }

def compare(a, b):
    mismatches = []
    for key in sorted(EXPECTED):
        if a[key] != b[key]:
            mismatches.append({"family": key[0], "seed": key[1],
                               "previous": a[key], "challenger": b[key]})
    return {"equal": 160 - len(mismatches), "different": len(mismatches),
            "mismatches": mismatches}

def main():
    ap = argparse.ArgumentParser()
    for name in ("a6", "b6", "current"):
        ap.add_argument("--" + name, required=True)
    ap.add_argument("--out", required=True)
    args = ap.parse_args()
    a, b, c = (parse(getattr(args, key)) for key in ("a6", "b6", "current"))
    old = compare(a, b)
    new = compare(b, c)
    report = {
        "method": "same authored C probe; 160 cases / version, 5 strata x 32",
        "scope": "single-actor, source-bound H1 movement/contact regression",
        "excluded": ["historical 1.7x executable", "three-ghost AI",
                     "concurrent multiple A6 owners", "B2/B3 joint acceptance",
                     "WAL crash recovery", "frame pacing"],
        "sources": {
            "a6": "e0d6aa1334df92c7f8e4425cac96a9f1d4bb4243",
            "b6": "1ebd10049b7f555d9ee037690f6cd090255c98ec",
            "current_parent": "4a28fdb1a04e6e36421c3fbb7c87d1ef97173d86"
        },
        "a6": stats(a), "b6": stats(b), "current": stats(c),
        "a6_to_b6": old, "b6_to_current": new,
        "mandatory_gates": {
            "PEB01_common_scenario_coverage": True,
            "PEB02_internal_tick_accounting": True,
            "PEB03_historical_A6_B6_parity": old["different"] == 0,
            "PEB04_current_divergence_reported": True,
            "PEB05_authority_concurrency_durability": "NOT TESTED"
        },
    }
    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    print("PEB1 A6", stats(a))
    print("PEB1 B6", stats(b))
    print("PEB1 CURRENT", stats(c))
    print("PEB1 A6_B6 equal", old["equal"], "changed", old["different"])
    print("PEB1 B6_CURRENT equal", new["equal"], "changed", new["different"])
    if old["different"]:
        raise SystemExit("PEB1 historical control drift: investigate before accepting")
    print("PEB1 COMMON-SUBSET GATES PASS; authority / durability gates NOT TESTED")

if __name__ == "__main__":
    main()
