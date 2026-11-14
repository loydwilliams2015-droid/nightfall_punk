#!/usr/bin/env python3
"""H281 cross-family acceptance aggregator: 8 x 5 real-source cases, 2 replays."""
import argparse
import json
import math
from pathlib import Path
from statistics import median

def main():
    p = argparse.ArgumentParser()
    p.add_argument("--input", type=Path, required=True)
    p.add_argument("--out", type=Path, required=True)
    args = p.parse_args()
    files = sorted(args.input.rglob("h281_family_*.json"))
    if len(files) != 8:
        raise ValueError(f"Expected 8 unique family reports, received {len(files)}")
    records = {}
    family_summary = []
    for report_path in files:
        report = json.loads(report_path.read_text(encoding="utf-8"))
        family = report["family"]
        if not isinstance(family, int) or not 0 <= family < 8:
            raise ValueError(f"Invalid family {report_path}: {family}")
        if (report["unique_cases"] != 5 or
            report["source_owned_executions"] != 10 or
            report["genuine_accepted_ticks"] != 20):
            raise ValueError(f"Invalid family counts {family}")
        cases = report["cases"]
        if len(cases) != 5:
            raise ValueError(f"Incomplete family {family}")
        for item in cases:
            case = item["case"]
            if (not isinstance(case, int) or
                case // 5 != family or
                item["family"] != family or
                item["replays"] != 2 or
                item["exact_replay"] is not True or
                case in records):
                raise ValueError(f"Duplicate or invalid case {case}")
            observations = item["observations"]
            if len(observations) != 2:
                raise ValueError(f"No source pair for case {case}")
            for tick, obs in enumerate(observations, 1):
                if obs["tick"] != tick or obs["case"] != case or obs["receipts"] != tick:
                    raise ValueError(f"Non-authoritative joint receipt: case {case} tick {tick}")
                for metric in ("actor_dx", "actor_dz", "crate_dx", "crate_dz"):
                    if not math.isfinite(obs[metric]):
                        raise ValueError(f"Invalid measurement {case}, {metric}")
                if obs["actor_dz"] <= 1e-6 or obs["crate_dx"] <= 1e-6 or obs["crate_dz"] <= 0:
                    raise ValueError(f"Non-positive source physical consequence case {case}")
            records[case] = item
        family_summary.append({"family": family, "cases": 5, "replays": 10,
            "yaw": cases[0]["observations"][1]["yaw"]})
    if set(records) != set(range(40)):
        raise ValueError(f"Missing H281 cases: {sorted(set(range(40)) - set(records))}")
    second_ticks = [records[i]["observations"][1] for i in range(40)]
    result = {
        "scope": "real single-owner A6/B2/B3 source-issued yaw/forward PUSH matrix; not H275F face/corner or multi-actor",
        "case_count": 40, "independent_replayed_executions": 80,
        "accepted_ticks": 160, "cumulative_b3_receipts_across_executions": 160,
        "family_count": 8, "cases_per_family": 5,
        "families": sorted(family_summary, key=lambda x: x["family"]),
        "gates": {
            "G01_all_40_real_cases": True,
            "G02_all_80_source_executions_success": True,
            "G03_deterministic_two_replays_each": True,
            "G04_positive_source_actor_and_crate_xz": True,
            "G05_genuine_two_tick_receipt_and_world_checks": True,
        },
        "second_tick": {
            "yaw_min": min(x["yaw"] for x in second_ticks),
            "yaw_max": max(x["yaw"] for x in second_ticks),
            "forward_min": min(x["forward"] for x in second_ticks),
            "forward_max": max(x["forward"] for x in second_ticks),
            "actor_dz_min": min(x["actor_dz"] for x in second_ticks),
            "actor_dz_median": median(x["actor_dz"] for x in second_ticks),
            "actor_dz_max": max(x["actor_dz"] for x in second_ticks),
            "crate_dx_min": min(x["crate_dx"] for x in second_ticks),
            "crate_dx_median": median(x["crate_dx"] for x in second_ticks),
            "crate_dx_max": max(x["crate_dx"] for x in second_ticks),
            "crate_dz_min": min(x["crate_dz"] for x in second_ticks),
            "crate_dz_median": median(x["crate_dz"] for x in second_ticks),
            "crate_dz_max": max(x["crate_dz"] for x in second_ticks),
        },
        "not_tested": ["H275F complete swept face/corner production promotion",
                       "shared multi-actor B2/B3 publication",
                       "power-loss recovery", "H15 TSAN runtime", "representative frame time"]
    }
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n")
    print("H281 MATRIX PASS real=40/40 replay=80/80 joint_ticks=160/160")
    print(json.dumps(result["second_tick"], sort_keys=True))

if __name__ == "__main__":
    main()
