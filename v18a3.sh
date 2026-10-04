#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"
mkdir -p build/v18a3
CC="${CC:-cc}"
COMMON=(-std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc/shared)
SRC=(src/shared/nf_contact18a.c src/shared/nf_contact18a2.c src/shared/nf_contact18a3.c)
case "${1:-all}" in
 build|test|samples|all)
   "$CC" -O2 -DNF18A3_TEST_CONTROLS "${COMMON[@]}" "${SRC[@]}" \
     src/tests/test_v18a3_contact.c -lm -o build/v18a3/test_v18a3_contact
   "$CC" -O2 "${COMMON[@]}" "${SRC[@]}" \
     src/tests/test_v18a3_production.c -lm -o build/v18a3/test_v18a3_production
   "$CC" -O2 -DNF18A3_TEST_CONTROLS "${COMMON[@]}" "${SRC[@]}" \
     src/tests/sample_v18a3_contact.c -lm -o build/v18a3/sample_v18a3_contact
   ;;
 *) echo 'usage: bash ./v18a3.sh [build|test|samples|all]' >&2; exit 2;;
esac
case "${1:-all}" in
 test|all)
   ./build/v18a3/test_v18a3_contact | tee build/v18a3/fixture_results.csv
   ./build/v18a3/test_v18a3_production | tee build/v18a3/production_gate.log
   ;;
esac
case "${1:-all}" in
 samples|all)
   ./build/v18a3/sample_v18a3_contact \
     build/v18a3/shape_samples.csv \
     build/v18a3/sweep_oracle.csv \
     build/v18a3/heldout_support.csv
   python3 tools/analyze_v18a3_contact.py build/v18a3 build/v18a3/PAR_RESULTS.md
   ;;
esac
