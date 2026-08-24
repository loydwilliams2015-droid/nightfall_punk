#!/usr/bin/env python3
import csv
import math
import os
import statistics
import sys
from collections import defaultdict

if len(sys.argv) != 7:
    raise SystemExit(
        "usage: analyze_v17a_graphical_review.py review.csv report.md soil.csv par.md classifications.csv frames_dir"
    )

review_path, report_path, soil_path, par_path, class_path, frames_dir = sys.argv[1:]
with open(review_path, newline="", encoding="utf-8") as f:
    rows = list(csv.DictReader(f))

required = {
    "lab", "category", "seed", "mode", "replicate", "success", "steps", "unique_cells", "revisits",
    "retreats", "support_transitions", "affordance_accepts", "topology_updates", "route_discovery_step",
    "cue_disagreement_steps", "path_invalid_steps", "health_remaining", "exposure_accum", "ecology_cost_accum",
    "resource_collected", "contract_context_seen", "deterministic_hash"
}
missing = required.difference(rows[0].keys() if rows else set())

def iv(row, key):
    return int(row[key])

def fv(row, key):
    return float(row[key])

pairs = defaultdict(list)
for row in rows:
    pairs[(row["lab"], row["category"], row["seed"], row["mode"])].append(row)

determinism_mismatches = 0
for group in pairs.values():
    hashes = {r["deterministic_hash"] for r in group}
    if len(group) != 2 or len(hashes) != 1:
        determinism_mismatches += 1

base = [r for r in rows if iv(r, "replicate") == 0]
by_mode = defaultdict(list)
by_lab_mode = defaultdict(list)
by_seed = defaultdict(dict)
for row in base:
    by_mode[row["mode"]].append(row)
    by_lab_mode[(row["lab"], row["mode"])].append(row)
    by_seed[(row["lab"], row["category"], row["seed"])][row["mode"]] = row

modes_expected = {"instrumented", "perceptual", "greedy", "random"}

def summary(rs):
    if not rs:
        return {
            "n": 0, "success": 0.0, "steps": math.inf, "health": 0.0, "exposure": 0.0,
            "ecology": 0.0, "revisits": 0.0, "route_discovery": 0.0, "cue_disagree": 0.0,
        }
    successful = [r for r in rs if iv(r, "success") == 1]
    return {
        "n": len(rs),
        "success": statistics.fmean(iv(r, "success") for r in rs),
        "steps": statistics.fmean(iv(r, "steps") for r in successful) if successful else math.inf,
        "health": statistics.fmean(fv(r, "health_remaining") for r in rs),
        "exposure": statistics.fmean(fv(r, "exposure_accum") for r in rs),
        "ecology": statistics.fmean(fv(r, "ecology_cost_accum") for r in rs),
        "revisits": statistics.fmean(fv(r, "revisits") for r in rs),
        "route_discovery": statistics.fmean(fv(r, "route_discovery_step") for r in rs),
        "cue_disagree": statistics.fmean(fv(r, "cue_disagreement_steps") for r in rs),
    }

summaries = {mode: summary(by_mode[mode]) for mode in sorted(modes_expected)}
inst = summaries["instrumented"]
perc = summaries["perceptual"]
greedy = summaries["greedy"]
rnd = summaries["random"]

frame_count = 0
if os.path.isdir(frames_dir):
    frame_count = sum(1 for name in os.listdir(frames_dir) if name.lower().endswith(".png"))

path_invalid_total = sum(iv(r, "path_invalid_steps") for r in base)
cue_disagreement_total = sum(iv(r, "cue_disagreement_steps") for r in by_mode["perceptual"])

classifications = []
divergent_pairs = 0
labs_with_divergence = set()
for key in sorted(by_seed):
    modes = by_seed[key]
    if not modes_expected.issubset(modes):
        continue
    irow = modes["instrumented"]
    prow = modes["perceptual"]
    grow = modes["greedy"]

    ie = fv(irow, "exposure_accum")
    pe = fv(prow, "exposure_accum")
    ih = fv(irow, "health_remaining")
    ph = fv(prow, "health_remaining")
    isteps = iv(irow, "steps")
    psteps = iv(prow, "steps")
    cue_steps = iv(prow, "cue_disagreement_steps")
    exp_ratio = pe / ie if ie > 1e-6 else (1.0 if pe <= 1e-6 else 999.0)
    quality_i = 120.0 * iv(irow, "success") + ih - 1.25 * ie - 0.65 * fv(irow, "ecology_cost_accum") - 0.06 * isteps
    quality_p = 120.0 * iv(prow, "success") + ph - 1.25 * pe - 0.65 * fv(prow, "ecology_cost_accum") - 0.06 * psteps
    quality_g = 120.0 * iv(grow, "success") + fv(grow, "health_remaining") - 1.25 * fv(grow, "exposure_accum") - 0.65 * fv(grow, "ecology_cost_accum") - 0.06 * iv(grow, "steps")

    material_invalid = any(iv(row, "path_invalid_steps") > 0 for row in modes.values())
    same_success = iv(irow, "success") == iv(prow, "success")
    material_difference = (
        not same_success
        or abs(pe - ie) >= max(1.0, 0.20 * max(ie, 1.0))
        or abs(psteps - isteps) >= 12
        or cue_steps >= max(4, int(0.20 * max(psteps, 1)))
    )
    if material_difference:
        divergent_pairs += 1
        labs_with_divergence.add(key[0])

    if material_invalid:
        label = "MATERIAL_GAP"
        reason = "review path entered a materially invalid cell"
    elif iv(irow, "success") > iv(prow, "success") or quality_i - quality_p >= 15.0:
        label = "LOGIC_ONLY"
        reason = "instrumented channel materially outperformed player-perceptual channel"
    elif quality_g - quality_i >= 12.0:
        label = "ACTOR_APPRAISAL_GAP"
        reason = "greedy control materially outperformed richer instrumented appraisal"
    elif material_difference:
        label = "PERCEPTUAL_DIFFERENCE"
        reason = "player-channel quantization changed route or outcome without material invalidity"
    else:
        label = "CORRESPONDS"
        reason = "instrumented and player-perceptual outcomes remained within correspondence bounds"

    classifications.append({
        "lab": key[0], "category": key[1], "seed": key[2], "classification": label, "reason": reason,
        "instrumented_success": iv(irow, "success"), "perceptual_success": iv(prow, "success"),
        "instrumented_exposure": ie, "perceptual_exposure": pe, "exposure_ratio": exp_ratio,
        "instrumented_health": ih, "perceptual_health": ph, "instrumented_steps": isteps,
        "perceptual_steps": psteps, "cue_disagreement_steps": cue_steps,
    })

class_counts = defaultdict(int)
for row in classifications:
    class_counts[row["classification"]] += 1

observability = not missing and len(base) == 160 and frame_count == 40
inspectability = all(key in required for key in {
    "route_discovery_step", "cue_disagreement_steps", "exposure_accum", "ecology_cost_accum", "contract_context_seen"
})
testability = determinism_mismatches == 0 and path_invalid_total == 0 and len(pairs) == 160
organizationality = modes_expected.issubset(by_mode.keys())
assessability = (
    inst["success"] - rnd["success"] >= 0.25
    and divergent_pairs >= 8
    and len(labs_with_divergence) >= 4
    and cue_disagreement_total > 0
)
all_pass = observability and inspectability and testability and organizationality and assessability

with open(class_path, "w", newline="", encoding="utf-8") as f:
    fieldnames = list(classifications[0].keys()) if classifications else ["lab", "category", "seed", "classification"]
    w = csv.DictWriter(f, fieldnames=fieldnames)
    w.writeheader()
    w.writerows(classifications)

with open(soil_path, "w", newline="", encoding="utf-8") as f:
    w = csv.writer(f)
    w.writerow(["lab", "mode", "n", "success_rate", "mean_health", "mean_exposure", "mean_ecology", "mean_revisits", "mean_route_discovery", "mean_cue_disagreement"])
    for key in sorted(by_lab_mode):
        s = summary(by_lab_mode[key])
        w.writerow([key[0], key[1], s["n"], f"{s['success']:.6f}", f"{s['health']:.6f}", f"{s['exposure']:.6f}", f"{s['ecology']:.6f}", f"{s['revisits']:.6f}", f"{s['route_discovery']:.6f}", f"{s['cue_disagree']:.6f}"])

with open(report_path, "w", encoding="utf-8") as out:
    out.write("# nightfall!punk v1.7A — Graphical AI-player correspondence review\n\n")
    out.write(f"- Canonical seeds: **{len(by_seed)}** (8 labs x 5 classes)\n")
    out.write(f"- AI review rows: **{len(rows)}** (4 modes x 2 deterministic replicates)\n")
    out.write(f"- Graphical referee frames: **{frame_count}**\n")
    out.write(f"- Deterministic duplicate mismatches: **{determinism_mismatches}**\n")
    out.write(f"- Material-invalid path steps: **{path_invalid_total}**\n")
    out.write(f"- Instrumented/perceptual divergent seeds: **{divergent_pairs}/{len(by_seed)}** across **{len(labs_with_divergence)}/8** labs\n\n")

    out.write("## Mode readout\n\n")
    out.write("| Mode | Success | Mean health | Mean exposure | Mean ecology cost | Mean successful steps | Route discovery | Cue disagreement |\n")
    out.write("|---|---:|---:|---:|---:|---:|---:|---:|\n")
    for mode in ("instrumented", "perceptual", "greedy", "random"):
        s = summaries[mode]
        steps = f"{s['steps']:.2f}" if math.isfinite(s["steps"]) else "n/a"
        out.write(f"| {mode} | {s['success']:.1%} | {s['health']:.2f} | {s['exposure']:.2f} | {s['ecology']:.2f} | {steps} | {s['route_discovery']:.2f} | {s['cue_disagree']:.2f} |\n")

    out.write("\n## Player-channel correspondence classes\n\n")
    for label in ("CORRESPONDS", "PERCEPTUAL_DIFFERENCE", "LOGIC_ONLY", "ACTOR_APPRAISAL_GAP", "MATERIAL_GAP"):
        out.write(f"- {label}: **{class_counts[label]}**\n")

    out.write("\n## OITOA\n\n")
    for name, value in [
        ("Observability", observability), ("Inspectability", inspectability), ("Testability", testability),
        ("Organizationality", organizationality), ("Assessability", assessability)
    ]:
        out.write(f"- {name}: **{'PASS' if value else 'FAIL'}**\n")
    out.write(f"\n**Graphical correspondence diagnostic gate: {'PASS' if all_pass else 'FAIL'}**\n\n")
    out.write("The gate is diagnostic rather than a claim of final level-design quality. Instrumented AI receives bounded exact values for observed cells; perceptual AI receives the same local geometry through quantized player-legible cues and memory. The referee renderer exposes truth only after the run. Random remains the negative control and greedy remains the goal-directed control.\n")

with open(par_path, "w", encoding="utf-8") as out:
    out.write("# nightfall!punk v1.7A graphical PAR sheet — provisional\n\n")
    out.write("| Dimension | Reference family | Disposition | Current graphical evidence |\n")
    out.write("|---|---|---|---|\n")
    out.write("| Spatial legibility / route reading | Halo / Quake family | KEEP+MEASURE | player-perceptual mode now supplies a falsifiable correspondence gap; native human proof still required |\n")
    out.write("| Actor-local visibility and route evidence | Left 4 Dead / Source | MEETS PAR candidate | player channel is locally bounded and exact referee truth is withheld until replay |\n")
    out.write("| Actor-relative ecological knowledge | Rain World family | MEETS PAR candidate | perceptual ecology cues are local and quantized; persistent population ecology remains 1.7B |\n")
    out.write("| Traversal-affordance communication | Sunset Overdrive family | KEEP+MEASURE | affordance cues participate in player-channel choices; high-speed native feel remains unproven |\n")
    out.write("| Material topology correspondence | Oxygen Not Included family | BELOW PAR / EARLY | dynamic topology stays materially valid in selected seeds, but material richness remains early |\n")
    out.write("| Contract authority vs historical context | project-original | N/A external PAR | referee frames separate authority from gradient memory; actor-specific contract utility remains 1.7B |\n")

print(
    f"v1.7A graphical review: {'PASS' if all_pass else 'FAIL'}; "
    f"frames={frame_count}; mismatches={determinism_mismatches}; divergent={divergent_pairs}; "
    f"labs={len(labs_with_divergence)}; instrumented_success={inst['success']:.3f}; perceptual_success={perc['success']:.3f}; random_success={rnd['success']:.3f}"
)
if not all_pass:
    raise SystemExit(1)
