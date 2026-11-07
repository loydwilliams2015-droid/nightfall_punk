#!/usr/bin/env python3
"""Next-Mx entry gate tests; static proof of HOLD consistency, not engine integration."""
import copy
import json
import pathlib
import re
import unittest

ROOT=pathlib.Path(__file__).resolve().parents[2]
FILE=ROOT/"docs/retc_next_mx_entry.json"
SHA=re.compile(r"^[0-9a-f]{40}$")
IDS=tuple("MXN-%02d"%n for n in range(1,6))

def validate(x):
    if x.get("schema")!=1 or x.get("programme")!="RET-C next Mx (provisional M3) entry":
        raise ValueError("invalid programme")
    if not SHA.fullmatch(str(x.get("source_sha",""))):
        raise ValueError("unbound incumbent SHA")
    gates=x.get("gates")
    if not isinstance(gates,list) or len(gates)!=5 or tuple(g.get("id") for g in gates)!=IDS:
        raise ValueError("five ordered gates required")
    if x.get("entry")!="HOLD" or x.get("production_release") is not False:
        raise ValueError("premature M3 admission")
    for g in gates:
        if g.get("status")!="HOLD" or g.get("evidence")!=[]:
            raise ValueError("unsupported gate promotion")
        if not isinstance(g.get("prerequisites"),list) or len(g["prerequisites"])!=3:
            raise ValueError("missing gate prerequisites")
        if len(set(g["prerequisites"]))!=3:
            raise ValueError("repeated prerequisites")
    for key in ("m25_manifest","m2x_manifest"):
        p=(ROOT/x[key]).resolve()
        if not p.is_relative_to(ROOT) or not p.is_file():
            raise ValueError("invalid antecedent manifest path")
        prev=json.loads(p.read_text())
        if prev.get("production_release") is not False or prev.get("promotion")!="HOLD":
            raise ValueError("antecedent programme is not on hold")
    if "no automatic gate promotion" not in x.get("policy",""):
        raise ValueError("missing explicit authority policy")
    return True

class NextMxTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.x=json.loads(FILE.read_text())
    def test_01_positive_hold_contract(self):
        self.assertTrue(validate(copy.deepcopy(self.x)))
    def test_02_missing_or_reordered_gate_refuses(self):
        for mutate in (lambda x:x["gates"].pop(),lambda x:x["gates"].reverse()):
            y=copy.deepcopy(self.x);mutate(y)
            with self.assertRaises(ValueError):validate(y)
    def test_03_unwitnessed_promotion_refuses(self):
        y=copy.deepcopy(self.x);y["gates"][0]["status"]="PASS"
        with self.assertRaises(ValueError):validate(y)
    def test_04_release_or_entry_escalation_refuses(self):
        for field,val in (("entry","READY"),("production_release",True)):
            y=copy.deepcopy(self.x);y[field]=val
            with self.assertRaises(ValueError):validate(y)
    def test_05_antecedent_source_and_coverage(self):
        self.assertEqual(self.x["incumbent_pr"],207)
        m25=json.loads((ROOT/self.x["m25_manifest"]).read_text())
        m2x=json.loads((ROOT/self.x["m2x_manifest"]).read_text())
        self.assertEqual(len(m25["gates"]),5)
        self.assertEqual(len(m2x["stages"]),5)
        self.assertTrue(all(g["status"]=="HOLD" for g in m25["gates"]))
        self.assertTrue(validate(copy.deepcopy(self.x)))

if __name__=="__main__":unittest.main(verbosity=2)
