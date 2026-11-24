"""Diagnostic schema contract tests; all inputs synthetic and NOT acceptance."""
import copy
import unittest
from tools.retc_h319_evidence_gate import valid, checksum

def fixture():
    return {
        "source_sha": "a" * 40, "frame_hash": "synthetic-not-real",
        "world_tick": 100, "source_version": 4, "material_epoch": 7,
        "contacts": [
            {"actor_id": a, "object_id": 2010, "contact_id": a,
             "accepted": True, "impulse": 1.0, "toi": .5, "tick": 100,
             "source_version": 4, "oracle_agrees": True} for a in (10, 20)],
        "clearances": [
            {"actor_id": a, "status": "FREE", "material_epoch": 7,
             "body_clear": True, "visible": True} for a in (10, 20)]
    }

class TestEvidence(unittest.TestCase):
    def test_schema_complete_only(self):
        self.assertEqual(valid(fixture()), [])
    def test_duplicate_actor(self):
        d = fixture(); d["contacts"][1]["actor_id"] = 10
        self.assertTrue(valid(d))
    def test_wrong_target(self):
        d = fixture(); d["contacts"][1]["object_id"] = 10
        self.assertTrue(valid(d))
    def test_a5_pending(self):
        d = fixture(); d["clearances"][1]["status"] = "PENDING"
        self.assertTrue(valid(d))
    def test_stale_tick(self):
        d = fixture(); d["contacts"][1]["tick"] += 1
        self.assertTrue(valid(d))
    def test_zero_impulse(self):
        d = fixture(); d["contacts"][0]["impulse"] = 0
        self.assertTrue(valid(d))
    def test_oracle_disagreement(self):
        d = fixture(); d["contacts"][0]["oracle_agrees"] = False
        self.assertTrue(valid(d))
    def test_epoch_split(self):
        d = fixture(); d["clearances"][0]["material_epoch"] += 1
        self.assertTrue(valid(d))
    def test_wrong_actor(self):
        d = fixture(); d["contacts"][0]["actor_id"] = 30
        self.assertTrue(valid(d))
    def test_nan(self):
        d = fixture(); d["contacts"][0]["toi"] = float("nan")
        self.assertTrue(valid(d))
    def test_missing_field(self):
        d = fixture(); del d["contacts"][0]["contact_id"]
        self.assertTrue(valid(d))
    def test_duplicate_contact_receipt(self):
        d = fixture(); d["contacts"][1]["contact_id"] = d["contacts"][0]["contact_id"]
        self.assertTrue(valid(d))
    def test_boolean_contact_epoch(self):
        d = fixture(); d["contacts"][0]["tick"] = True
        self.assertTrue(valid(d))
    def test_boolean_material_epoch(self):
        d = fixture(); d["clearances"][0]["material_epoch"] = True
        self.assertTrue(valid(d))
    def test_boolean_object_id(self):
        d = fixture(); d["contacts"][0]["object_id"] = True
        self.assertTrue(valid(d))
    def test_zero_source_epoch(self):
        d = fixture(); d["material_epoch"] = 0
        self.assertTrue(valid(d))
    def test_zero_world_tick(self):
        d = fixture(); d["world_tick"] = 0
        self.assertTrue(valid(d))
    def test_digest_order_sensitive(self):
        d = fixture(); e = copy.deepcopy(d)
        e["contacts"].reverse()
        self.assertNotEqual(checksum(d), checksum(e))

if __name__ == "__main__":
    unittest.main()
