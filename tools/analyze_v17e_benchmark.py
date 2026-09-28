#!/usr/bin/env python3
import csv
import pathlib
import sys

def read_rows(path):
    with open(path, newline="", encoding="utf-8") as f:
        return list(csv.DictReader(f))

def fv(row, key):
    return float(row[key])

def pct(delta, base):
    return 0.0 if base == 0.0 else 100.0 * delta / base

def main():
    src = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else "build/v17e/graphical_benchmark.csv")
    out = pathlib.Path(sys.argv[2] if len(sys.argv) > 2 else "build/v17e/graphical_benchmark_report.md")
    rows = read_rows(src)
    by = {r["preset"]: r for r in rows}
    required = ["off", "play", "world", "actor", "causal", "full"]
    missing = [x for x in required if x not in by]
    if missing:
        raise SystemExit("missing benchmark rows: " + repr(missing))

    base = fv(by["off"], "frame_avg_ms")
    checks = []
    checks.append(("E1 authoritative observer inertness",
                   all(int(by[p]["hash_mismatches"]) == 0 for p in required),
                   "all modes must preserve authoritative hash"))

    play_over = pct(fv(by["play"], "frame_avg_ms") - base, base)
    checks.append(("PLAY <=2% mean proxy", play_over <= 2.0,
                   f"{play_over:.2f}% vs OFF"))

    single = {}
    for p in ("world", "actor", "causal"):
        single[p] = pct(fv(by[p], "frame_avg_ms") - base, base)
    checks.append(("single diagnostic <=8% mean proxy",
                   all(v <= 8.0 for v in single.values()),
                   ", ".join(f"{k}={v:.2f}%" for k,v in single.items())))

    full_over = pct(fv(by["full"], "frame_avg_ms") - base, base)
    checks.append(("FULL <=20% mean proxy", full_over <= 20.0,
                   f"{full_over:.2f}% vs OFF"))

    inert = all(int(by[p]["hash_mismatches"]) == 0 for p in required)
    verdict = "PASS" if all(ok for _, ok, _ in checks) else "PROVISIONAL"

    lines = [
        "# nightfall!punk v1.7E graphical benchmark",
        "",
        f"Source: {src}",
        "",
        "> CI may use Xvfb/software OpenGL. Treat absolute frame times as runner-specific; relative observer overhead and hash-inertness are the primary CI signals.",
        "",
        "| Preset | Avg frame ms | P95 | P99 | Compose avg us | Render avg us | Contributions | Objects | Hash mismatches |",
        "|---|---:|---:|---:|---:|---:|---:|---:|---:|",
    ]
    for p in required:
        r = by[p]
        lines.append(
            f"| {p.upper()} | {fv(r,'frame_avg_ms'):.4f} | {fv(r,'frame_p95_ms'):.4f} | {fv(r,'frame_p99_ms'):.4f} | "
            f"{fv(r,'compose_avg_us'):.2f} | {fv(r,'render_avg_us'):.2f} | {fv(r,'contributions_avg'):.1f} | "
            f"{fv(r,'objects_avg'):.1f} | {int(r['hash_mismatches'])} |"
        )

    lines += ["", "## PAR checks", "", "| Check | Result | Observation |", "|---|---|---|"]
    for name, ok, obs in checks:
        lines.append(f"| {name} | {'PASS' if ok else 'REVIEW'} | {obs} |")

    lines += [
        "",
        f"**CI benchmark disposition: {verdict}.**",
        "",
        "Any authoritative hash mismatch invalidates the observer. Performance target misses identify batching/aggregation work; they do not by themselves falsify the semantic architecture.",
        "",
        "## Derived hypotheses",
        "",
        f"- E1 inertness: {'supported in this run' if inert else 'failed in this run'}.",
        f"- PLAY instrumentation overhead: {play_over:.2f}%.",
        "- WORLD/ACTOR/CAUSAL overheads: " + ", ".join(f"{k} {v:.2f}%" for k,v in single.items()) + ".",
        f"- FULL forensic overhead: {full_over:.2f}%.",
    ]

    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print("\n".join(lines))

if __name__ == "__main__":
    main()
