#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"
mkdir -p build/v18a
CC="${CC:-cc}"
COMMON=(-std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc/shared)
case "${1:-test}" in
  build|test|samples|all)
    "$CC" "${COMMON[@]}" src/shared/nf_contact18a.c src/tests/test_v18a_contact.c -lm -o build/v18a/test_v18a_contact
    "$CC" -O2 "${COMMON[@]}" src/shared/nf_contact18a.c src/tests/sample_v18a_contact.c -lm -o build/v18a/sample_v18a_contact
    ;;
  *) echo "usage: ./v18a1.sh [build|test|samples|all]" >&2;exit 2;;
esac
case "${1:-test}" in
  test|all) ./build/v18a/test_v18a_contact | tee build/v18a/fixture_results.csv ;;
esac
case "${1:-test}" in
  samples|all)
    ./build/v18a/sample_v18a_contact build/v18a/contact_samples.csv
    python3 tools/analyze_v18a_contact.py build/v18a/contact_samples.csv build/v18a/PAR_RESULTS.md
    ;;
esac
