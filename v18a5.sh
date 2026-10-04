#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")"
mkdir -p build/v18a5
CC="${CC:-cc}"
FLAGS=(-O2 -std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc/shared)
TEST_FLAGS=("${FLAGS[@]}" -DNF18A5_TEST_CONTROLS)
SRC=(src/shared/nf_contact18a.c src/shared/nf_contact18a2.c src/shared/nf_contact18a3.c src/shared/nf_contact18a4.c src/shared/nf_contact18a5.c src/shared/nf_contact18a5_sink.c)
case "${1:-all}" in all|build|test|samples|regression) ;; *) echo 'usage: bash ./v18a5.sh [all|build|test|samples|regression]' >&2; exit 2;; esac
"$CC" "${TEST_FLAGS[@]}" "${SRC[@]}" src/tests/test_v18a5_contact.c -lm -o build/v18a5/test_v18a5_contact
"$CC" "${TEST_FLAGS[@]}" "${SRC[@]}" src/tests/sample_v18a5_contact.c -lm -o build/v18a5/sample_v18a5_contact
"$CC" "${FLAGS[@]}" "${SRC[@]}" src/tests/test_v18a5_production.c -lm -o build/v18a5/test_v18a5_production
"$CC" "${FLAGS[@]}" "${SRC[@]}" src/tests/test_v18a5_sink.c -lm -o build/v18a5/test_v18a5_sink
if [[ "${1:-all}" == all || "${1:-all}" == test || "${1:-all}" == regression ]];then
  ./build/v18a5/test_v18a5_contact | tee build/v18a5/fixtures.csv
  ./build/v18a5/test_v18a5_production | tee build/v18a5/production_gate.log
  ./build/v18a5/test_v18a5_sink | tee build/v18a5/sink_gate.log
fi
if [[ "${1:-all}" == all || "${1:-all}" == regression ]];then
  bash ./v18a4.sh regression >build/v18a5/inherited-a4.log
  echo 'inherited 1.8A.1 through 1.8A.4: PASS'
fi
if [[ "${1:-all}" == all || "${1:-all}" == samples ]];then
  ./build/v18a5/sample_v18a5_contact build/v18a5/model_samples.csv
  python3 tools/analyze_v18a5_contact.py build/v18a5/model_samples.csv build/v18a5/PAR_RESULTS.md
fi
