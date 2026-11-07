#!/usr/bin/env python3
"""M2.x source/coverage dependency checks. Static preflight, NOT engine proof."""
import copy
import json
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[2]
MATRIX = ROOT / "docs/retc_m2x_implementation_matrix.json"
EXPECTED = tuple(f"M2X-{i:02d}" for i in range(1, 6))

def validate(data):
    if data.get("schema") != 1 or data.get("programme") != "RET-C M2.x implementation framework":
        raise ValueError("invalid programme")
    stages = data.get("stages")
    if not isinstance(stages, list) or tuple(s.get("id") for s in stages) != EXPECTED:
        raise ValueError("missing, repeated or unordered milestone")
    if data.get("promotion") != "HOLD" or data.get("production_release") is not False:
        raise ValueError("unverified milestone promotion")
    if any(s.get("status") not in ("RESEARCH_ONLY", "HOLD") for s in stages):
        raise ValueError("unverified promotion in source inventory")
    if stages[-1]["status"] != "HOLD":
        raise ValueError("publication/recovery promotion forbidden")
    if len(data.get("cross_gates", [])) != 5:
        raise ValueError("missing cross-cutting gate")
    m25 = json.loads((ROOT / data["m25_manifest"]).read_text())
    if m25.get("production_release") is not False or m25.get("promotion") != "HOLD":
        raise ValueError("M2.5 ledger disagrees with M2.x release hold")
    if any(s.get("status") != "HOLD" for s in m25.get("gates", [])):
        raise ValueError("M2.5 unfinished gates were promoted")
    return True

class M2ProgrammeAcceptance(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data = json.loads(MATRIX.read_text())
    def test_01_stage_inventory_and_consistent_holds(self):
        self.assertTrue(validate(copy.deepcopy(self.data)))
    def test_02_all_implementation_sources_and_owner_symbols_exist(self):
        for stage in self.data["stages"]:
            src = ROOT / stage["source"]
            self.assertTrue(src.is_file(), stage["id"])
            self.assertIn(stage["symbol"], src.read_text(), stage["id"])
    def test_03_regression_contains_source_backed_stage_anchors(self):
        source = (ROOT / self.data["test_source"]).read_text()
        for stage in self.data["stages"]:
            self.assertIn(stage["test_anchor"], source, stage["id"])
        self.assertIn("NF_WORLD_TICK_REJECTED", source)
    def test_04_missing_dependency_or_early_promotion_refuses(self):
        for mutate in (lambda m: m["stages"].pop(1),
                       lambda m: m["stages"][-1].update(status="RESEARCH_ONLY"),
                       lambda m: m.update(promotion="READY_FOR_REVIEW")):
            x = copy.deepcopy(self.data)
            mutate(x)
            with self.assertRaises(ValueError):
                validate(x)
    def test_05_m25_evidence_promotion_is_not_implicit(self):
        self.assertEqual(self.data["stages"][-1]["status"], "HOLD")
        m25 = json.loads((ROOT / self.data["m25_manifest"]).read_text())
        self.assertEqual(len(m25["gates"]), 5)
        self.assertTrue(all(g["status"] == "HOLD" for g in m25["gates"]))
        self.assertFalse(m25["production_release"])

if __name__ == "__main__":
    unittest.main(verbosity=2)
