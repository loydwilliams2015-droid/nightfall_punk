#!/usr/bin/env python3
"""H281 exact-source matrix runner: real A6 owner, B2 grant, B3 PUSH, 2 ticks.

Runs five cases of one family twice from fresh world initialization. Fails on
missing source measurements, divergent replay, invalid source consequences, or
the C test's own full authorization/lineage/reader acceptance failures.
"""
import argparse
import json
import math
import re
import subprocess
from pathlib import Path

PATTERN = re.compile(
    r"H281 SOURCE_MATRIX case=(\d+) tick=(\d+) forward=([-\d.]+)"
    r" yaw=([-\d.]+) actor_dx=([-\d.]+) actor_dz=([-\d.]+)"
    r" crate_dx=([-\d.]+) crate_dz=([-\d.]+) receipts=(\d+)"
)
SUCCESS = "RET-C real A6 dispatcher two consecutive committed joint ticks PASS"

def execute(executable, case, log_path):
    command = [str(executable), "--sweep-case", str(case)]
    proc = subprocess.run(command, capture_output=True, text=True, timeout=60)
    result = proc.stdout + proc.stderr
    log_path.write_text(result, encoding="utf-8")
    if proc.returncode or SUCCESS not in result:
        raise RuntimeError(
            f"genuine B2/B3 case {case} failed rc={proc.returncode}; "
            f"tail={result[-3000:]}"
        )
    rows = []
    for match in PATTERN.finditer(result):
        c, tick, forward, yaw, ax, az, cx, cz, receipts = match.groups()
        row = {
            "case": int(c),
            "tick": int(tick),
            "forward": float(forward),
            "yaw": float(yaw),
            "actor_dx": float(ax),
            "actor_dz": float(az),
            "crate_dx": float(cx),
            "crate_dz": float(cz),
            "receipts": int(receipts),
        }
        if not all(math.isfinite(row[field]) for field in (
            "forward", "yaw", "actor_dx", "actor_dz", "crate_dx", "crate_dz"
        )):
            raise ValueError(f"nonfinite source measurement case={case}")
        rows.append(row)
    if len(rows) != 2 or [r["tick"] for r in rows] != [1, 2]:
        raise ValueError(f"missing two authentic source ticks case={case}: {rows}")
    if any(r["case"] != case for r in rows):
        raise ValueError(f"source case identity mismatch {case}: {rows}")
    if [r["receipts"] for r in rows] != [1, 2]:
        raise ValueError(f"not two cumulative genuine receipts case={case}: {rows}")
    expected_yaw = -.1 - .005 * (case // 5)
    expected_forward = .2 + .002 * (case % 5)
    if (abs(rows[0]["yaw"]) > 1e-5 or
        abs(rows[1]["yaw"]-expected_yaw) > 1e-5 or
        abs(rows[0]["forward"]-.2) > 1e-5 or
        abs(rows[1]["forward"]-expected_forward) > 1e-5):
        raise ValueError(f"input schedule mismatch case={case}: {rows}")
    if not all(
        r["crate_dx"] > 1e-6 and
        r["actor_dz"] > 1e-6 and
        r["crate_dz"] > 0.0
        for r in rows
    ):
        raise ValueError(f"no real positive off-axis PUSH case={case}: {rows}")
    return rows

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--executable", required=True, type=Path)
    ap.add_argument("--family", type=int, choices=range(8), required=True)
    ap.add_argument("--out", type=Path, required=True)
    args = ap.parse_args()
    exe = args.executable.resolve()
    if not exe.is_file():
        raise FileNotFoundError(exe)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    cases = []
    for local in range(5):
        case = 5 * args.family + local
        runs = []
        for repeat in range(2):
            filename = args.out.parent / f"h281_case_{case:02d}_run{repeat+1}.log"
            runs.append(execute(exe, case, filename))
        if runs[0] != runs[1]:
            raise AssertionError(f"nondeterministic replay case={case}: {runs}")
        cases.append({"case": case, "family": args.family, "replays": 2,
                      "observations": runs[0], "exact_replay": True})
        print(f"H281 CASE={case:02d} SOURCE_PASS replay=2/2 "
              f"yaw={runs[0][1]['yaw']:.5f} "
              f"actor_dz={runs[0][1]['actor_dz']:.8f} "
              f"crate_dz={runs[0][1]['crate_dz']:.8f}")
    result = {
        "family": args.family,
        "unique_cases": 5,
        "source_owned_executions": 10,
        "genuine_accepted_ticks": 20,
        "cases": cases,
        "scope": "B2/B3 issued single A6 owner only; not multi-actor publication",
    }
    args.out.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n")
    print(f"H281 FAMILY={args.family} PASS cases=5 replays=10 ticks=20")

if __name__ == "__main__":
    main()
