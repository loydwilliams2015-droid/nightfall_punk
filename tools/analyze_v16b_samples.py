#!/usr/bin/env python3
"""nightfall!punk v1.6B sample-database analysis.

Uses only the Python standard library. It treats repeated same-seed scripts as
computational-trust controls and selects one representative "soil sample" from
each data stratum for close inspection.
"""
from __future__ import annotations

import csv
import math
import statistics
import sys
from collections import defaultdict
from pathlib import Path

METRICS = [
    "shots", "focus_tick_share", "focus_mean", "focus_max",
    "cone_mean_deg", "cone_max_deg", "redirect_max_deg",
    "support_max_deg", "recoil_max_deg", "speed_max_mps",
]

# OITOA Assessability asks whether the sample system can discriminate a named
# causal effect, not whether that effect has already reached a final gameplay
# tuning magnitude. 0.01 degrees is well above float/noise scale in this data
# and remains a separate gate from later PAR/feel calibration.
MIN_REDIRECT_DISCRIMINATION_DEG = 0.01


def f(row, key):
    return float(row[key])


def main() -> int:
    if len(sys.argv) < 2:
        print(
            "usage: analyze_v16b_samples.py samples.csv [report.md] [soil.csv]",
            file=sys.stderr,
        )
        return 2
    src = Path(sys.argv[1])
    report_path = (
        Path(sys.argv[2]) if len(sys.argv) > 2 else src.with_suffix(".report.md")
    )
    soil_path = (
        Path(sys.argv[3])
        if len(sys.argv) > 3
        else src.with_name(src.stem + "_soil.csv")
    )
    with src.open(newline="", encoding="utf-8") as fh:
        rows = list(csv.DictReader(fh))
    if not rows:
        print("empty sample database", file=sys.stderr)
        return 2

    required = {
        "sample_id",
        "scenario",
        "stratum",
        "seed",
        "repeat",
        "ticks",
        *METRICS,
    }
    missing_columns = sorted(required - set(rows[0]))
    missing_values = sum(
        1 for row in rows for key in required if row.get(key, "") == ""
    )

    groups = defaultdict(list)
    for row in rows:
        groups[(row["scenario"], row["seed"])].append(row)
    deterministic_pairs = 0
    deterministic_failures = []
    for key, pair in groups.items():
        if len(pair) < 2:
            continue
        deterministic_pairs += 1
        a, b = sorted(pair, key=lambda row: int(row["repeat"]))[:2]
        if any(a[metric] != b[metric] for metric in METRICS):
            deterministic_failures.append(key)

    scenarios = sorted({row["scenario"] for row in rows})
    strata = sorted({row["stratum"] for row in rows})
    by_scenario = defaultdict(list)
    for row in rows:
        by_scenario[row["scenario"]].append(row)

    pathological_redirect = max(
        (
            f(row, "redirect_max_deg")
            for row in by_scenario.get("pathological_redirect", [])
        ),
        default=0.0,
    )
    calm_redirect = max(
        (f(row, "redirect_max_deg") for row in by_scenario.get("calm", [])),
        default=0.0,
    )
    focus_peak = max(
        (f(row, "focus_max") for row in by_scenario.get("focus_hold", [])),
        default=0.0,
    )
    calm_focus = max(
        (f(row, "focus_max") for row in by_scenario.get("calm", [])),
        default=0.0,
    )
    finite = all(
        math.isfinite(f(row, metric)) for row in rows for metric in METRICS
    )

    observability = (
        not missing_columns and missing_values == 0 and len(rows) >= 20
    )
    inspectability = all(
        key in rows[0]
        for key in [
            "focus_mean",
            "cone_mean_deg",
            "redirect_max_deg",
            "support_max_deg",
            "recoil_max_deg",
        ]
    )
    testability = (
        len(scenarios) >= 7
        and len(strata) >= 5
        and "random_control" in scenarios
        and "pathological_redirect" in scenarios
    )
    organizationality = deterministic_pairs >= 7 and not deterministic_failures
    redirect_separation = pathological_redirect - calm_redirect
    assessability = (
        finite
        and redirect_separation >= MIN_REDIRECT_DISCRIMINATION_DEG
        and focus_peak > 0.80
        and calm_focus < 0.05
    )

    oitoa = {
        "Observability": observability,
        "Inspectability": inspectability,
        "Testability": testability,
        "Organizationality": organizationality,
        "Assessability": assessability,
    }
    credibility_score = 100.0 * sum(oitoa.values()) / len(oitoa)

    # Representative close-inspection slices: row nearest median cone_mean per stratum.
    soil_rows = []
    for stratum in strata:
        stratum_rows = [row for row in rows if row["stratum"] == stratum]
        median = statistics.median(
            f(row, "cone_mean_deg") for row in stratum_rows
        )
        soil_rows.append(
            min(
                stratum_rows,
                key=lambda row: abs(f(row, "cone_mean_deg") - median),
            )
        )
    with soil_path.open("w", newline="", encoding="utf-8") as fh:
        writer = csv.DictWriter(fh, fieldnames=rows[0].keys())
        writer.writeheader()
        writer.writerows(soil_rows)

    lines = [
        "# nightfall!punk v1.6B automated sample analysis\n\n",
        f"Samples: **{len(rows)}**  \nScenarios: **{len(scenarios)}**  \n"
        f"Strata: **{len(strata)}**  \n",
        f"Deterministic duplicate pairs: **{deterministic_pairs}**; "
        f"mismatches: **{len(deterministic_failures)}**.\n\n",
        "## OITOA\n\n",
        "| Lens | Result | Executable criterion |\n|---|---|---|\n",
    ]
    criteria = {
        "Observability": "no missing required values; >=20 samples",
        "Inspectability": "named Focus/cone/redirect/support/recoil columns",
        "Testability": ">=7 scenarios, >=5 strata, random + pathological controls",
        "Organizationality": "same seed/script repeats exactly",
        "Assessability": (
            "finite data; pathological redirect >=0.01 deg above calm; "
            "Focus positive/negative controls"
        ),
    }
    for name, ok in oitoa.items():
        lines.append(
            f"| {name} | {'PASS' if ok else 'FAIL'} | {criteria[name]} |\n"
        )
    lines += [
        "\n## Credibility\n\n",
        f"Executable OITOA credibility score: **{credibility_score:.1f}/100**.\n\n",
        f"Pathological redirect max: **{pathological_redirect:.4f}°** vs calm "
        f"**{calm_redirect:.4f}°**; separation **{redirect_separation:.4f}°**.\n\n",
        f"Focus-hold peak: **{focus_peak:.4f}**; calm negative-control peak: "
        f"**{calm_focus:.4f}**.\n\n",
        "The score is a build-diagnostic summary, not a substitute for native "
        "human feel/readability evidence.\n\n",
        "## Soil samples\n\n",
        "One median-cone representative from each stratum is written to `"
        + soil_path.name
        + "`. These are close-inspection slices, not claims that the "
        "distribution is exhausted.\n",
    ]
    if deterministic_failures:
        lines.append(
            "\nDeterminism failures: "
            + ", ".join(f"{a}/{b}" for a, b in deterministic_failures)
            + "\n"
        )
    report_path.write_text("".join(lines), encoding="utf-8")
    print("".join(lines))
    return 0 if all(oitoa.values()) else 1


if __name__ == "__main__":
    raise SystemExit(main())
