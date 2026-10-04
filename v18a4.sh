#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")"
mkdir -p build/v18a4
CC="${CC:-cc}"
CFLAGS=(-O2 -std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc/shared)
SRC=(src/shared/nf_contact18a.c src/shared/nf_contact18a2.c src/shared/nf_contact18a3.c src/shared/nf_contact18a4.c)
case "${1:-all}" in build|test|samples|regression|all) : ;; *) echo 'usage: ./v18a4.sh [build|test|samples|regression|all]' >&2; exit 2;; esac
"$CC" "${CFLAGS[@]}" "${SRC[@]}" src/tests/test_v18a4_contact.c -lm -o build/v18a4/test_v18a4_contact
"$CC" "${CFLAGS[@]}" "${SRC[@]}" src/tests/sample_v18a4_contact.c -lm -o build/v18a4/sample_v18a4_contact
if [[ "${1:-all}" == test || "${1:-all}" == all || "${1:-all}" == regression ]]; then
  ./build/v18a4/test_v18a4_contact | tee build/v18a4/fixtures.csv
fi
if [[ "${1:-all}" == regression ]]; then
  bash ./v18a1.sh test > build/v18a4/inherited-a1.log
  bash ./v18a2.sh test > build/v18a4/inherited-a2.log
  bash ./v18a3.sh test > build/v18a4/inherited-a3.log
  echo 'inherited contact regression: PASS (A1/A2/A3/A4)'
fi
if [[ "${1:-all}" == samples || "${1:-all}" == all ]]; then
  ./build/v18a4/sample_v18a4_contact build/v18a4/samples.csv
  python3 tools/analyze_v18a4_contact.py build/v18a4/samples.csv build/v18a4/PAR_RESULTS.md
fi
