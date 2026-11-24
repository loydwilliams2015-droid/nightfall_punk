#!/usr/bin/env python3
"""H319 diagnostic-only evidence schema. NEVER an A4/A5/B2/A6 authority."""
import argparse
import hashlib
import json
import math

ACTORS = {10, 20}
TARGET = 2010

def valid(d):
    errors = []
    if not isinstance(d, dict):
        return ["root is not an object"]
    required = ("source_sha", "frame_hash", "world_tick", "source_version",
                "material_epoch", "contacts", "clearances")
    if any(k not in d for k in required):
        return ["missing required top-level field"]
    sha = d["source_sha"]
    if not isinstance(sha, str) or len(sha) != 40 or any(c not in "0123456789abcdef" for c in sha):
        errors.append("invalid source_sha")
    if not isinstance(d["frame_hash"], str) or not d["frame_hash"]:
        errors.append("missing frame_hash")
    for k in ("world_tick", "source_version", "material_epoch"):
        if type(d[k]) is not int or d[k] <= 0:
            errors.append("invalid " + k)
    for label, keys in (("contacts", ("actor_id", "object_id", "contact_id",
                                      "accepted", "impulse", "toi", "tick",
                                      "source_version", "oracle_agrees")),
                        ("clearances", ("actor_id", "status", "material_epoch",
                                        "body_clear", "visible"))):
        rows = d[label]
        if not isinstance(rows, list) or len(rows) != 2:
            errors.append(label + " must have exactly two rows")
            continue
        seen = set()
        contact_ids = set()
        for i, row in enumerate(rows):
            if not isinstance(row, dict) or any(k not in row for k in keys):
                errors.append(f"{label}[{i}]: malformed row")
                continue
            actor = row["actor_id"]
            if type(actor) is not int or actor not in ACTORS or actor in seen:
                errors.append(f"{label}[{i}]: invalid/duplicate actor")
            else:
                seen.add(actor)
            if label == "contacts":
                if type(row["object_id"]) is not int or row["object_id"] != TARGET or type(row["contact_id"]) is not int or row["contact_id"] <= 0:
                    errors.append(f"contact[{i}]: invalid object/contact id")
                elif row["contact_id"] in contact_ids:
                    errors.append(f"contact[{i}]: duplicate contact id")
                else:
                    contact_ids.add(row["contact_id"])
                if row["accepted"] is not True or row["oracle_agrees"] is not True:
                    errors.append(f"contact[{i}]: not accepted or oracle disagrees")
                impulse, toi = row["impulse"], row["toi"]
                if type(impulse) not in (int, float) or not math.isfinite(impulse) or impulse <= 0:
                    errors.append(f"contact[{i}]: invalid impulse")
                if type(toi) not in (int, float) or not math.isfinite(toi) or not 0 <= toi <= 1:
                    errors.append(f"contact[{i}]: invalid time-of-impact")
                if type(row["tick"]) is not int or type(row["source_version"]) is not int or row["tick"] != d["world_tick"] or row["source_version"] != d["source_version"]:
                    errors.append(f"contact[{i}]: stale epoch")
            else:
                if row["status"] != "FREE" or row["body_clear"] is not True or row["visible"] is not True:
                    errors.append(f"clearance[{i}]: blocked/pending/unknown")
                if type(row["material_epoch"]) is not int or row["material_epoch"] != d["material_epoch"]:
                    errors.append(f"clearance[{i}]: material epoch mismatch")
        if seen != ACTORS:
            errors.append(label + ": required actors 10/20 absent")
    return errors

def checksum(d):
    return hashlib.sha256(json.dumps(d, sort_keys=True, separators=(",", ":"), allow_nan=False).encode()).hexdigest()

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("evidence")
    a = p.parse_args()
    try:
        def unique_pairs(pairs):
            out = {}
            for key, value in pairs:
                if key in out:
                    raise ValueError("duplicate JSON key: " + key)
                out[key] = value
            return out
        with open(a.evidence, encoding="utf-8") as f:
            d = json.load(f, object_pairs_hook=unique_pairs,
                          parse_constant=lambda x: (_ for _ in ()).throw(ValueError(x)))
        errors = valid(d)
    except (OSError, ValueError) as e:
        errors = [str(e)]
    if errors:
        print("H319 EVIDENCE HOLD: " + "; ".join(errors))
        raise SystemExit(1)
    print("H319 SCHEMA PASS (NOT physics/owner acceptance):", checksum(d))

if __name__ == "__main__":
    main()
