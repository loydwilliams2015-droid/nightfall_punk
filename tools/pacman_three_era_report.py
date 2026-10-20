#!/usr/bin/env python3
"""Scientific Pac-Man-themed three-era comparison.

Original: archived nightfall v1.5 toy trace, NOT Namco's actual source.
Classic rule: newly reconstructed, NOT the original ghost controller.
A6 and B6: same C H1 physical probe compiled against pinned code.
Benchmark reports scoped observations; it cannot compare AI win rate or H4 feel.
"""
import argparse
import json
import math
import re
from collections import Counter
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
ARCHIVE=ROOT/"docs/archive/nightfall_punk_v15_pacman_metabolic_demo.txt"
TRACE=ROOT/"docs/ret/pacman_original_v15_trace_reconstruction.json"
RESULT=re.compile(
 r"RESULT family=(\d+) seed=(\d+) commit=(\d+) pending=(\d+)"
 r" reject=(\d+) contact=(\d+) first_stop=(\d+) hash=(\d+)"
 r" history=(\d+) x=(-?[0-9.]+) z=(-?[0-9.]+)"
)
def main():
 ap=argparse.ArgumentParser()
 ap.add_argument("--a6",required=True,type=Path)
 ap.add_argument("--b6",required=True,type=Path)
 ap.add_argument("--out",type=Path)
 args=ap.parse_args()
 trace=json.loads(TRACE.read_text())
 original=ARCHIVE.read_text()
 assert "NOT a reconstruction of historical Pac-Man ghost source code" in original
 rows=trace["records"]
 assert len(rows)==10
 assert [r["player"] for r in rows]==list("BEHGGHIIHE")
 hist=Counter(a for r in rows for a in r["ghost_actions"])
 assert hist=={"GUARD":24,"CHASE":6},hist
 classic=Counter("FLEE" if r["tick"] in trace["player_power_ticks"] else "CHASE"
                  for r in rows for _ in trace["ghosts"])
 assert classic=={"CHASE":21,"FLEE":9}
 def parse(path):
  result={}
  content=path.read_text()
  assert "NATIVE_H1_SCENARIOS=160 PASS" in content, path
  for line in content.splitlines():
   m=RESULT.search(line)
   if not m:continue
   family,seed,committed,pending,rejected,contact,stop,hist_hash,history,x,z=m.groups()
   key=(int(family),int(seed))
   if key in result:raise ValueError(f"duplicate scenario {key}")
   d=dict(zip(("commits","pending","rejected","contacts","first_stop","hash","history"),
              map(int,(committed,pending,rejected,contact,stop,hist_hash,history))))
   d["x"]=float(x);d["z"]=float(z)
   if not (math.isfinite(d["x"]) and math.isfinite(d["z"])):raise ValueError("nonfinite")
   assert d["commits"]+d["pending"]+d["rejected"]<=30
   assert d["contacts"]<=d["commits"]
   result[key]=d
  keys={(f,s) for f in range(5) for s in range(32)}
  assert set(result)==keys,(path,len(result),keys-set(result))
  return result
 a=parse(args.a6);b=parse(args.b6)
 same=sum(a[k]==b[k] for k in sorted(a))
 mismatches=[dict(family=f,seed=s,a6=a[f,s],b6=b[f,s])
             for f,s in sorted(a) if a[f,s]!=b[f,s]]
 def summarize(d):
  values=list(d.values())
  return {"scenarios":len(values),"committed_ticks":sum(x["commits"] for x in values),
   "pending_terminations":sum(x["pending"] for x in values),
   "rejected_terminations":sum(x["rejected"] for x in values),
   "scenario_contacts":sum(x["contacts"] for x in values),
   "first_stop_ticks":[x["first_stop"] for x in values if x["first_stop"]]}
 report={"scope":"Archived v1.5 decisions versus two real H1 A6 physics builds; NOT a common Pac-Man AI controller",
 "historic_v15":{"decisions":sum(hist.values()),"actions":dict(hist),
     "player_route":"B,E,H,G,G,H,I,I,H,E","energy_end":trace["energy_end"],
     "viability_equal_energy_divergent":trace["two_equal_viability_distinct_energy"]},
 "reconstructed_classic_like":{"decisions":sum(classic.values()),"actions":dict(classic),
    "not_original_game_source":True},
 "native_a6":summarize(a),"native_b6":summarize(b),
 "native_same_scenario_count":same,"native_unequal_scenario_count":len(mismatches),
 "native_mismatches":mismatches,
 "labels":{"native":"Same physical probe and target tile script. Original A6 commit and corrected B6 source built separately.","historical":"v1.5 text artifact, not executable source; actions cannot be compared to physics receipts.","performance":"No representative render/network latency, stable FPS, or statistical independent ghost AI comparison."}}
 print("ARCHIVE v1.5 decisions: GUARD=24 CHASE=6 / 30; classic-like reconstructed CHASE=21 FLEE=9 / 30")
 print("A6 "+json.dumps(report["native_a6"],sort_keys=True))
 print("B6 "+json.dumps(report["native_b6"],sort_keys=True))
 print(f"PAIRED_ENGINE_PARITY equal={same}/160 changed={len(mismatches)}/160")
 print("CONCLUSION "+("same H1 physical response for this deterministic corpus" if not mismatches else "observed differences require causal adjudication, not automatic failure or speed claim"))
 if args.out:
  args.out.parent.mkdir(parents=True,exist_ok=True)
  args.out.write_text(json.dumps(report,indent=2,sort_keys=True)+"\n")
 return 0
if __name__=="__main__":
 raise SystemExit(main())
