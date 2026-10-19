#!/usr/bin/env python3
"""Verify that the corrected B6 programme preserves every historical B7 datum.

The B7 ledger is immutable historical evidence. B6 is an ownership/target
migration, not a rewrite or a new engineering acceptance claim.
"""
from __future__ import annotations
import copy
import hashlib
import json
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OLD = ROOT / "docs/evidence/v18b7a_g5_master_ledger.json"
NEW = ROOT / "docs/evidence/v18b6_master_ledger.json"
FROZEN_OLD_SHA256 = "4afb77e135112566c4f1d9c99ba5268d2f3c4950511cc54f56d303370038e35e"
COUNTS = {"G1":12, "G2":9, "G3":11, "G4":26, "G5":14}

def validate(old: dict, new: dict, *, check_frozen_bytes:bool=True) -> list[str]:
    bad=[]
    if check_frozen_bytes and hashlib.sha256(OLD.read_bytes()).hexdigest()!=FROZEN_OLD_SHA256:
        bad.append("original G5 historical ledger changed bytes")
    if new.get("schema_version")!="1.8B.6-migration-v1":
        bad.append("wrong B6 schema")
    if new.get("active_build")!="1.8B.6":
        bad.append("not assigned to B6")
    if new.get("authoritative_production_baseline_verified") is not False:
        bad.append("unverified baseline promoted")
    if new.get("claimed_production_release") is not False:
        bad.append("unverified release claimed")
    guards=new.get("migration_guards",{})
    if guards.get("release_status")!="HOLD":
        bad.append("unapproved release/merge")
    for g in ("no_historical_relabel","no_automatic_merge","no_replay_of_unverified_results",
              "no_assumed_canonical_ledger_write"):
        if guards.get(g) is not True:
            bad.append("missing migration safety rule: "+g)
    original=new.get("migration_source",{})
    if original.get("pinned_head_commit")!="01229af48a099302c778248454d988ee38161855":
        bad.append("migration provenance head changed")
    if original.get("old_master_ledger")!="docs/evidence/v18b7a_g5_master_ledger.json":
        bad.append("migration provenance path changed")
    if new.get("inherited_evidence")!=old.get("evidence_groups"):
        bad.append("historical laboratory evidence rewritten")
    if new.get("unresolved_gate_families")!=old.get("open_gate_families"):
        bad.append("unresolved gate history rewritten or discarded")
    orig=old.get("determinations",[])
    moved=new.get("design_decisions",[])
    if len(moved)!=72 or len(orig)!=72:
        bad.append("incomplete 72-item design transfer")
    ref={d.get("id"):d for d in orig if isinstance(d,dict)}
    cnt=Counter(d.get("id") for d in moved if isinstance(d,dict))
    for id,d in ref.items():
        if cnt[id]!=1:
            bad.append(f"{id}: migrated count {cnt[id]}")
            continue
        x=next(z for z in moved if z["id"]==id)
        if x.get("group")!=d.get("owner"):
            bad.append(id+": owner altered")
        if x.get("historical_origin_document")!=d.get("source_document"):
            bad.append(id+": provenance path altered")
        if x.get("target_build")!="1.8B.6":
            bad.append(id+": still active on wrong build")
        if x.get("specification_status")!="inherited_locked_review_candidate":
            bad.append(id+": unapproved design-promotion state")
        if x.get("implemented_on_verified_1_8B6_baseline") is not False:
            bad.append(id+": falsely validated on new baseline")
        if x.get("integrated_acceptance")!="open" or x.get("integration_retest_required") is not True:
            bad.append(id+": lost empirical hold")
        p=ROOT / d.get("source_document","")
        if not p.is_file():
            bad.append(id+": missing original contract source")
    if set(cnt)!=set(ref):
        bad.append("migrated ID set mismatch")
    for d in new.get("unresolved_gate_families",[]):
        if d.get("status")!="open":
            bad.append("unresolved engineering gate falsely closed: "+str(d.get("id")))
    by=Counter(d.get("group") for d in moved)
    if dict(by)!=COUNTS:
        bad.append("group counts differ from original")
    return bad

def main()->int:
    old=json.loads(OLD.read_text(encoding="utf-8"))
    new=json.loads(NEW.read_text(encoding="utf-8"))
    bad=validate(old,new)
    mutations=[
        ("missing C01",lambda d:d["design_decisions"].pop(0)),
        ("false B6 source pass",lambda d:d["design_decisions"][0].update(
            implemented_on_verified_1_8B6_baseline=True)),
        ("wrong destination",lambda d:d["design_decisions"][1].update(target_build="1.8B.7A")),
        ("misleading historical run",lambda d:d["inherited_evidence"].pop("G1_AUTHORITY")),
        ("hidden gate",lambda d:d["unresolved_gate_families"].pop()),
        ("invented promotion",lambda d:d["migration_guards"].update(release_status="RELEASED")),
        ("provenance rewrite",lambda d:d["design_decisions"][2].update(
            historical_origin_document="docs/forged.md")),
        ("duplicate ID",lambda d:d["design_decisions"][3].update(id="C01")),
    ]
    for title,fn in mutations:
        shadow=copy.deepcopy(new)
        fn(shadow)
        if not validate(old,shadow,check_frozen_bytes=False):
            bad.append("negative control erroneously passed: "+title)
    print(json.dumps({
        "active_build":new.get("active_build"),"transferred":len(new.get("design_decisions",[])),
        "groups":dict(sorted(Counter(x["group"] for x in new["design_decisions"]).items())),
        "inherited_evidence_groups":len(new["inherited_evidence"]),
        "open_gate_families":len(new["unresolved_gate_families"]),
        "negative_controls":len(mutations),"errors":bad,
        "status":"PASS" if not bad else "FAIL"},sort_keys=True))
    return 0 if not bad else 1

if __name__=="__main__":
    sys.exit(main())
