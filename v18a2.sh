#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"
mkdir -p build/v18a2
CC="${CC:-cc}"
COMMON=(-std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc/shared)
case "${1:-test}" in
  build|test|samples|all)
    "$CC" "${COMMON[@]}" src/shared/nf_contact18a.c src/shared/nf_contact18a2.c \
      src/tests/test_v18a2_contact.c -lm -o build/v18a2/test_v18a2_contact
    "$CC" -O2 "${COMMON[@]}" src/shared/nf_contact18a.c src/shared/nf_contact18a2.c \
      src/tests/sample_v18a2_contact.c -lm -o build/v18a2/sample_v18a2_contact
    ;;
  *) echo "usage: ./v18a2.sh [build|test|samples|all]" >&2;exit 2;;
esac
case "${1:-test}" in
  test|all) ./build/v18a2/test_v18a2_contact | tee build/v18a2/fixture_results.csv ;;
esac
case "${1:-test}" in
  samples|all)
    ./build/v18a2/sample_v18a2_contact build/v18a2/contact_samples.csv
    python3 tools/analyze_v18a2_contact.py build/v18a2/contact_samples.csv \
      build/v18a2/PAR_RESULTS.md
    ;;
esac
