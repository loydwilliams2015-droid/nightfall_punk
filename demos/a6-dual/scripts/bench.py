#!/usr/bin/env python3
"""Reproducible paired A6 arcade-policy study; descriptive not causal upstream benchmark."""
import argparse,csv,hashlib,json,math,random,statistics,subprocess,time
from pathlib import Path
R=Path(__file__).resolve().parents[1]
P=argparse.ArgumentParser();P.add_argument('--seeds',type=int,default=128);P.add_argument('--start',type=int,default=31001);P.add_argument('--steps',type=int,default=300);A=P.parse_args()
assert 1<=A.seeds<=20000 and 1<=A.steps<=300
(R/'results').mkdir(exist_ok=True)
allrows=[];elapsed=[]
for game in ('cinder','slipgate'):
 for player in ('goal','random'):
  for ai in ('classic','systemic'):
   args=[str(R/'bin/nightfall-cinder'),'--game',game,'--ai',ai,'--player',player,'--seed',str(A.start),'--batch',str(A.seeds),'--steps',str(A.steps)]
   start=time.perf_counter();out=subprocess.check_output(args,text=True);spent=time.perf_counter()-start
   rows=list(csv.DictReader(out.splitlines()))
   assert len(rows)==A.seeds,(game,player,ai,len(rows))
   assert all(row['history_fail']=='0' for row in rows),'history failure in paired samples'
   assert all(row['cache_loads']==row['cache_pending'] for row in rows),'cache miss not re-adjudicated'
   allrows+=rows;elapsed.append({'game':game,'player':player,'ai':ai,'wall_seconds':round(spent,4)})
   print(game,player,ai,'runs',len(rows),'seconds',round(spent,3),flush=True)
headers=list(allrows[0]);csvfile=R/'results/paired_1_8a6_arcade.csv'
with csvfile.open('w',newline='') as f:
 w=csv.DictWriter(f,headers);w.writeheader();w.writerows(allrows)
# Deterministic re-run of all rows, not just a subset; fail on any digest mismatch.
expected={(row['game'],row['ai'],row['player'],int(row['seed'])):row['digest'] for row in allrows}
for entry in elapsed:
 args=[str(R/'bin/nightfall-cinder'),'--game',entry['game'],'--ai',entry['ai'],'--player',entry['player'],'--seed',str(A.start),'--batch',str(A.seeds),'--steps',str(A.steps)]
 actual=list(csv.DictReader(subprocess.check_output(args,text=True).splitlines()))
 for row in actual:
  key=(row['game'],row['ai'],row['player'],int(row['seed']))
  if expected[key]!=row['digest']:raise SystemExit(f'REPLAY_MISMATCH,{key}')
print('REPLAY,all',len(allrows),'digest rows match',flush=True)
# Fixed-bootstrap RNG; cohort seeds, not independent game-world/maps.
rng=random.Random(0x18A6)
def ci_paired(x):
 boot=[];n=len(x)
 for _ in range(3000):
  boot.append(sum(x[rng.randrange(n)] for i in range(n))/n)
 boot.sort();return [round(statistics.mean(x),3),round(boot[int(.025*(len(boot)-1))],3),round(boot[int(.975*(len(boot)-1))],3)]
results=[]
for game in ('cinder','slipgate'):
 for player in ('goal','random'):
  groups={p:{int(r['seed']):r for r in allrows if r['game']==game and r['player']==player and r['ai']==p} for p in ('classic','systemic')}
  scores=[];survive=[];contact=[]
  for seed in sorted(groups['classic']):
   cl=groups['classic'][seed];sy=groups['systemic'][seed]
   scores.append(float(sy['score'])-float(cl['score']))
   survive.append(float(sy['health'])-float(cl['health']))
   contact.append(float(sy['history_events'])-float(cl['history_events']))
  summaries={}
  for p,rows in groups.items():
   summaries[p]={k:round(statistics.mean(float(r[k]) for r in rows.values()),3) for k in ('ticks','score','health','won','contacts','purple','cache_loads','cache_pending','energy_retreats','stale_chases','history_events','blocked_moves')}
  results.append({'game':game,'player':player,'n':A.seeds,'classical':summaries['classic'],'systemic':summaries['systemic'],
   'systemic_minus_classic_score':ci_paired(scores),'systemic_minus_classic_health':ci_paired(survive),'systemic_minus_classic_events':ci_paired(contact)})
metadata={'title':'nightfall!punk paired Pac-Man comparative test, NOT upstream GZDoom/LibreQuake benchmark',
 'seed_start':A.start,'per_cohort_seeds':A.seeds,'tick_cap':A.steps,
 'total_playthroughs':len(allrows),'key_limitations':['fixed two authored maps, not independent generated worlds','two simplified ghost policies are author-defined controls, not upstream engines','paired seeds share initial states, but random consumption differs after policy branches','the player AI uses a full maze search; only systemic ghosts have bounded local geometric memory','2D turn/tile projection of selected authoritative C contact modules, not full 3D FPS','no independent human play-feel evidence'],
 'runtime_seconds':elapsed,'csv_sha256':hashlib.sha256(csvfile.read_bytes()).hexdigest(),'replay_mismatches':0,'results':results}
(R/'results/PAR.json').write_text(json.dumps(metadata,indent=2)+'\n')
md=['# nightfall!punk 1.8A6 — Dual Arcade Scientific PAR','',
 f'- Cohorts: {len(allrows)} deterministic AI-player runs ({A.seeds} seeds × 2 authored arenas × 2 player policies × 2 ghost policies).',
 f'- Replay: 0 mismatches across all {len(allrows)} replayed output digests.',
 '- Hard gates: no observed history commit failure or unresolved cache miss in sampled runs; targeted scenarios independently test crate impulses, authorization and pending loading.',
 '- Evidence: H1 synthetic paired game comparison; NOT GZDoom or LibreQuake measured performance; NOT a 3D player physics benchmark.',
 '', '| Arena | Player | Classic mean score | Systemic mean score | Score delta [bootstrap 95% CI] | Classic health | Systemic health |',
 '|---|---|---:|---:|---|---:|---:|']
for row in results:
 c=row['classical'];s=row['systemic'];d=row['systemic_minus_classic_score']
 md.append(f"| {row['game']} | {row['player']} | {c['score']} | {s['score']} | {d[0]} [{d[1]}, {d[2]}] | {c['health']} | {s['health']} |")
md+=['','## Scope and falsification discipline','',
 'The baseline policy is a deliberately simple, omniscient BFS pursuer; the systemic policy observes local cells, retains last-seen actor position, spends/regenerates energy and retreats/recharges. The policies do not represent the actual GZDoom/LibreQuake AI.',
 'Cohort policy, player policy, initial RNG state and map are paired. Branch-specific random draws are not synchronized after divergence.',
 'Scores, health, contact count, history events and energy use are reported separately. Survival, lower contact or higher score is not by itself proof of a better FPS experience.',
 'The 1.8A authoritative tick scheduler, 3D physics, normal engine camera and server-client reconciliation are outside these small 2D executables; selected authoritative geometric, motor, impulse, and contact-history subroutines are used directly.',
 'No original Doom/Quake maps, proprietary art, GZDoom code, or LibreQuake assets are shipped in this experiment.',
 '', '## Per-cohort observations','']
for row in results:
 md.append(f"### {row['game']} / {row['player']}\n")
 for pol in ('classical','systemic'):
  ob=row[pol];md.append(f"- {pol}: " + '; '.join(f'{k}={v}' for k,v in ob.items()))
 md.append(f"- Paired systemic − classic mean surviving health [bootstrap 95% CI]: {row['systemic_minus_classic_health']}\n")
(R/'results/PAR.md').write_text('\n'.join(md)+'\n')
print('DATA',csvfile,'SHA256',metadata['csv_sha256'],flush=True)
for row in results:print('PAIR',row['game'],row['player'],row['systemic_minus_classic_score'],row['systemic_minus_classic_health'],flush=True)
