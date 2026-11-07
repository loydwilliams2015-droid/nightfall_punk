#!/usr/bin/env python3
"""Fail-closed M2.5 release-evidence validator; never substitutes for engineering tests."""
import json
import pathlib
import re
import sys
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
MANIFEST = ROOT / "docs/retc_m25_final_gate_manifest.json"
EXPECTED = tuple(f"M25-{i:02d}" for i in range(1, 6))
SHA = re.compile(r"^[a-f0-9]{40}$")
VALID_STATES = {"HOLD", "PASS"}

def validate(data):
    if not isinstance(data, dict) or data.get("schema") != 1:
        raise ValueError("invalid schema")
    if data.get("programme") != "RET-C M2.5 final closeout":
        raise ValueError("unexpected programme")
    if not SHA.fullmatch(str(data.get("source_base_sha", ""))):
        raise ValueError("missing source SHA")
    gates = data.get("gates")
    if not isinstance(gates, list) or tuple(g.get("id") for g in gates if isinstance(g, dict)) != EXPECTED:
        raise ValueError("must contain five ordered, unique M2.5 gates")
    all_passed = True
    for gate in gates:
        if gate.get("status") not in VALID_STATES:
            raise ValueError("invalid gate status")
        if not isinstance(gate.get("requirements"), list) or len(gate["requirements"]) < 3:
            raise ValueError("incomplete gate requirements")
        evidence = gate.get("evidence")
        if not isinstance(evidence, list):
            raise ValueError("invalid evidence list")
        if gate["status"] == "PASS":
            if not evidence:
                raise ValueError("unsupported PASS")
            for item in evidence:
                if not isinstance(item, dict) or not SHA.fullmatch(str(item.get("sha", ""))):
                    raise ValueError("missing exact evidence SHA")
                if not isinstance(item.get("run_url"), str) or not item["run_url"].startswith("https://github.com/"):
                    raise ValueError("missing CI run URL")
                if item.get("conclusion") != "success":
                    raise ValueError("non-successful CI evidence")
        else:
            all_passed = False
    if data.get("promotion") not in ("HOLD", "READY_FOR_REVIEW"):
        raise ValueError("invalid promotion state")
    if data.get("promotion") == "READY_FOR_REVIEW" and not all_passed:
        raise ValueError("promotion with open gates")
    # This manifest cannot authorize production autonomously, even with five PASS entries.
    if data.get("production_release") is not False:
        raise ValueError("production release requires independent human authorization")
    return all_passed

class FinalGateTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data = json.loads(MANIFEST.read_text(encoding="utf-8"))

    def clone(self):
        return json.loads(json.dumps(self.data))

    def test_01_baseline_fails_closed(self):
        self.assertFalse(validate(self.clone()))
        self.assertEqual(self.data["promotion"], "HOLD")

    def test_02_missing_gate_refuses(self):
        x = self.clone()
        x["gates"].pop()
        with self.assertRaises(ValueError):
            validate(x)

    def test_03_unproven_pass_refuses(self):
        x = self.clone()
        x["gates"][0]["status"] = "PASS"
        with self.assertRaises(ValueError):
            validate(x)

    def test_04_premature_promotion_refuses(self):
        x = self.clone()
        x["promotion"] = "READY_FOR_REVIEW"
        with self.assertRaises(ValueError):
            validate(x)

    def test_05_autonomous_release_refuses(self):
        x = self.clone()
        x["production_release"] = True
        with self.assertRaises(ValueError):
            validate(x)

if __name__ == "__main__":
    suite = unittest.defaultTestLoader.loadTestsFromTestCase(FinalGateTests)
    outcome = unittest.TextTestRunner(verbosity=2).run(suite)
    sys.exit(0 if outcome.wasSuccessful() else 1)
