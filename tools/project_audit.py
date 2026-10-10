#!/usr/bin/env python3
"""Lightweight repository-governance audit. No network access required."""
from __future__ import annotations
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
REQUIRED = [
    "PROJECT_STATE_INDEX.md",
    "LICENSE_STATUS.md",
    "THIRD_PARTY_NOTICES.md",
    "CONTRIBUTING.md",
    "CODE_OF_CONDUCT.md",
    "SECURITY.md",
    "docs/SCIENTIFIC_ACCOUNTABILITY_PROTOCOL_V2.md",
    "docs/LEGAL_PROVENANCE_AND_FAIR_USE_POLICY.md",
    "docs/DATA_AND_EVIDENCE_POLICY.md",
    "docs/BRANCH_AND_RELEASE_POLICY.md",
]
errors: list[str] = []
warnings: list[str] = []

for rel in REQUIRED:
    if not (ROOT / rel).is_file():
        errors.append(f"missing required governance file: {rel}")

readme = (ROOT / "README.md").read_text(encoding="utf-8") if (ROOT / "README.md").exists() else ""
has_license = any((ROOT / name).is_file() for name in ("LICENSE", "LICENSE.txt", "LICENSE.md", "COPYING"))
if not has_license and ("open-source" in readme.lower() or "copyleft licensed" in readme.lower()):
    errors.append("README claims active open-source/copyleft licensing but no project LICENSE is present")
if not has_license:
    warnings.append("no project-wide LICENSE present; LICENSE_STATUS.md governs current wording")

try:
    tracked = subprocess.check_output(["git", "ls-files"], cwd=ROOT, text=True).splitlines()
except Exception as exc:
    tracked = []
    warnings.append(f"git ls-files unavailable: {exc}")

bad_suffixes = (".o", ".a", ".so", ".dll", ".exe", ".pyc")
for path in tracked:
    p = Path(path)
    if path.startswith("build/") or p.suffix.lower() in bad_suffixes:
        errors.append(f"tracked build artifact: {path}")

if errors:
    print("PROJECT AUDIT: FAIL")
    for item in errors:
        print(f"ERROR: {item}")
else:
    print("PROJECT AUDIT: PASS")
for item in warnings:
    print(f"WARN: {item}")
sys.exit(1 if errors else 0)
