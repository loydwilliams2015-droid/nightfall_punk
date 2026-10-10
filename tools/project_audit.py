#!/usr/bin/env python3
"""Lightweight repository-governance audit. No network access required."""
from __future__ import annotations
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
REQUIRED = [
    "PROJECT_STATE_INDEX.md",
    "LICENSE",
    "LICENSES/GPL-3.0-or-later.txt",
    "LICENSE_STATUS.md",
    "REUSE.toml",
    "RELEASE_LICENSING_NOTICE.md",
    "THIRD_PARTY_NOTICES.md",
    "CONTRIBUTING.md",
    "CODE_OF_CONDUCT.md",
    "SECURITY.md",
    "docs/SCIENTIFIC_ACCOUNTABILITY_PROTOCOL_V2.md",
    "docs/LEGAL_PROVENANCE_AND_FAIR_USE_POLICY.md",
    "docs/DATA_AND_EVIDENCE_POLICY.md",
    "docs/BRANCH_AND_RELEASE_POLICY.md",
    "docs/CONTRIBUTOR_LICENSING_POLICY.md",
]
errors: list[str] = []
warnings: list[str] = []

for rel in REQUIRED:
    if not (ROOT / rel).is_file():
        errors.append(f"missing required governance file: {rel}")

readme = (ROOT / "README.md").read_text(encoding="utf-8") if (ROOT / "README.md").exists() else ""
license_status = (ROOT / "LICENSE_STATUS.md").read_text(encoding="utf-8") if (ROOT / "LICENSE_STATUS.md").exists() else ""
reuse = (ROOT / "REUSE.toml").read_text(encoding="utf-8") if (ROOT / "REUSE.toml").exists() else ""

has_license = (ROOT / "LICENSE").is_file() and (ROOT / "LICENSES/GPL-3.0-or-later.txt").is_file()
if not has_license:
    errors.append("GPL license files are missing")
if has_license and "GPL-3.0-or-later" not in license_status:
    errors.append("LICENSE_STATUS.md does not identify GPL-3.0-or-later")
if "SPDX-License-Identifier = \"GPL-3.0-or-later\"" not in reuse:
    errors.append("REUSE.toml does not contain the GPL-3.0-or-later SPDX expression")
if "GPL-3.0-or-later" not in readme:
    errors.append("README does not state the active software license")

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
