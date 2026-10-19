#!/usr/bin/env python3
"""Check the 1.8B.7A research ledger using only Python's standard library.

Verifies internal traceability, complete decision IDs, file existence and
explicit acceptance holds. Does NOT establish compiler correctness, physical
truth, GitHub job completion or legal ownership.
"""
from __future__ import annotations

import copy
import hashlib
import json
import re
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
LEDGER = ROOT / "docs/evidence/v18b7a_g5_master_ledger.json"
COUNTS = {"C": 12, "F": 9, "M": 11, "N": 12, "P": 14, "S": 14}
SHA = re.compile(r"^[0-9a-f]{40}$")
CHECKED_GROUPS = {"G1_AUTHORITY", "G2_PHYSICAL", "G3_MULTI", "G4_WAL_GOVERNOR"}


def check_tree_path(value: object, errors: list[str], context: str) -> bool:
    if not isinstance(value, str) or not value:
        errors.append(f"{context}: missing path")
        return False
    p = Path(value)
    if p.is_absolute() or ".." in p.parts:
        errors.append(f"{context}: unsafe relative path {value!r}")
        return False
    if not (ROOT / p).is_file():
        errors.append(f"{context}: source file absent {value}")
        return False
    return True


def check_blob(path: str, expected: object, errors: list[str], context: str) -> None:
    if not isinstance(expected, str) or not SHA.fullmatch(expected):
        errors.append(f"{context}: missing pinned Git blob SHA-1")
        return
    p = ROOT / path
    if not p.is_file():
        return
    data = p.read_bytes()
    actual = hashlib.sha1(b"blob " + str(len(data)).encode("ascii") + b"\x00" + data).hexdigest()
    if actual != expected:
        errors.append(f"{context}: source drift {actual} != {expected}")


def validate(doc: dict) -> list[str]:
    problems: list[str] = []
    if doc.get("schema_version") != "1.8B.7A-G5-S01-v1":
        problems.append("bad manifest schema")
    if doc.get("release_status") != "HOLD":
        problems.append("unauthorized release-promotion status")
    if doc.get("repository") != "loydwilliams2015-droid/nightfall_punk":
        problems.append("unrecognized repository")
    if not SHA.fullmatch(str(doc.get("lineage_parent_commit", ""))):
        problems.append("lineage parent is not a pinned commit hash")
    items = doc.get("determinations")
    if not isinstance(items, list):
        return problems + ["missing determination array"]
    actual = [r.get("id", "") for r in items if isinstance(r, dict)]
    expected = [p + f"{i:02d}" for p, count in COUNTS.items()
                for i in range(1, count + 1)]
    counts = Counter(actual)
    if len(items) != len(expected):
        problems.append(f"expected {len(expected)} determinations; got {len(items)}")
    for identifier in expected:
        if counts[identifier] != 1:
            problems.append(f"{identifier}: count {counts[identifier]} instead of one")
    for extra in set(actual) - set(expected):
        problems.append(f"unexpected determination {extra}")
    allowed_owners = {"C": "G1", "F": "G2", "M": "G3",
                      "N": "G4", "P": "G4", "S": "G5"}
    groups = doc.get("evidence_groups", {})
    if not isinstance(groups, dict) or set(groups) != CHECKED_GROUPS:
        problems.append("incomplete or altered scope evidence groups")
        groups = {}
    for i, row in enumerate(items):
        if not isinstance(row, dict):
            problems.append(f"row {i}: not an object")
            continue
        identity = row.get("id", "")
        owner = row.get("owner")
        if owner != allowed_owners.get(str(identity)[:1]):
            problems.append(f"{identity}: incorrect determination owner")
        if row.get("design_status") != "locked":
            problems.append(f"{identity}: unapproved specification state")
        if row.get("full_engine_acceptance") != "open":
            problems.append(f"{identity}: no evidence permits closed integration gate")
        if row.get("retest_on_integration") is not True:
            problems.append(f"{identity}: must retain integration retest")
        check_tree_path(row.get("source_document"), problems, identity)
        proof = row.get("scope_evidence_group")
        if owner == "G5" and proof is not None:
            problems.append(f"{identity}: Group 5 has no executable result yet")
        if owner != "G5" and proof not in groups:
            problems.append(f"{identity}: missing inherited evidence reference")
    for key, entry in groups.items():
        if not isinstance(entry, dict):
            problems.append(f"{key}: invalid evidence record")
            continue
        if not isinstance(entry.get("workflow_run_id"), int) or entry["workflow_run_id"] < 1:
            problems.append(f"{key}: missing actual workflow run ID")
        if not SHA.fullmatch(str(entry.get("tested_source_commit", ""))):
            problems.append(f"{key}: invalid tested source commit")
        if not isinstance(entry.get("cases"), int) or entry["cases"] < 1:
            problems.append(f"{key}: invalid test case count")
        if not isinstance(entry.get("assertions_per_configuration"), int) or entry["assertions_per_configuration"] < 1:
            problems.append(f"{key}: invalid assertion count")
        if check_tree_path(entry.get("test_file"), problems, key):
            check_blob(entry["test_file"], entry.get("test_blob_sha1"), problems, key)
        if entry.get("additional_test_file"):
            if check_tree_path(entry["additional_test_file"], problems, key):
                check_blob(entry["additional_test_file"],
                           entry.get("additional_test_blob_sha1"), problems, key)
        scope = entry.get("scope", "")
        if not isinstance(scope, str) or "not " not in scope:
            problems.append(f"{key}: missing evidence scope exclusion")
    gates = doc.get("open_gate_families", [])
    if not isinstance(gates, list) or len(gates) != 18:
        problems.append("unresolved integration-gate inventory must have 18 families")
    else:
        if len(set(g.get("id") for g in gates if isinstance(g, dict))) != 18:
            problems.append("duplicate gate IDs")
        for gate in gates:
            if not isinstance(gate, dict) or gate.get("status") != "open":
                problems.append("unsupported closed/invalid acceptance gate")
            elif gate.get("owner") not in {"G1", "G2", "G3", "G4", "G5"}:
                problems.append("unowned unresolved gate")
    return problems


def main() -> int:
    with LEDGER.open(encoding="utf-8") as handle:
        ledger = json.load(handle)
    problems = validate(ledger)
    # Intentionally test that these regressions cannot be silently accepted.
    for label, mutation in [
        ("false acceptance", lambda d: d["determinations"][0].update(
            full_engine_acceptance="passed")),
        ("missing source", lambda d: d["determinations"][1].update(
            source_document="docs/missing_obviously_fake.md")),
        ("duplicate ID", lambda d: d["determinations"][1].update(id="C01")),
        ("unapproved release", lambda d: d.update(release_status="RELEASED")),
        ("deleted gate", lambda d: d["open_gate_families"].pop()),
        ("invented workflow proof", lambda d: d["evidence_groups"]["G1_AUTHORITY"].update(
            workflow_run_id=0)),
        ("source drift", lambda d: d["evidence_groups"]["G2_PHYSICAL"].update(
            test_blob_sha1="0" * 40)),
    ]:
        clone = copy.deepcopy(ledger)
        mutation(clone)
        if not validate(clone):
            problems.append(f"negative control unexpectedly passed: {label}")
    output = {
        "schema": ledger.get("schema_version"),
        "determinations": len(ledger.get("determinations", [])),
        "by_group": dict(Counter(d["owner"] for d in ledger["determinations"])),
        "accepted_full_engine": sum(
            d["full_engine_acceptance"] == "passed"
            for d in ledger["determinations"]),
        "unresolved_gate_families": len(ledger.get("open_gate_families", [])),
        "negative_controls": 7,
        "errors": problems,
        "result": "PASS" if not problems else "FAIL",
    }
    print(json.dumps(output, sort_keys=True))
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
