#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")"
mkdir -p build/v18a5/closure
CC="${CC:-cc}"
FLAGS=(-O2 -std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc/shared)
SOURCES=(src/shared/nf_contact18a.c src/shared/nf_contact18a2.c src/shared/nf_contact18a3.c src/shared/nf_contact18a4.c src/shared/nf_contact18a5.c src/shared/nf_contact18a5_sink.c src/shared/nf_contact18a5_close.c src/shared/nf_contact18a5_wal.c)
case "${1:-all}" in all|test|samples|regression) ;; *) echo 'usage: ./v18a5_close.sh [all|test|samples|regression]' >&2;exit 2;; esac
"$CC" "${FLAGS[@]}" "${SOURCES[@]}" src/tests/test_v18a5_close.c -lm -o build/v18a5/closure/test_v18a5_close
"$CC" "${FLAGS[@]}" "${SOURCES[@]}" src/tests/sample_v18a5_close.c -lm -o build/v18a5/closure/sample_v18a5_close
if [[ "${1:-all}" != samples ]];then
  ./build/v18a5/closure/test_v18a5_close | tee build/v18a5/closure/fixtures.csv
fi
if [[ "${1:-all}" == all || "${1:-all}" == regression ]];then
  bash ./v18a5.sh regression > build/v18a5/closure/inherited.log
  echo 'Inherited 1.8A.1–1.8A.5: PASS'
fi
if [[ "${1:-all}" == all || "${1:-all}" == samples ]];then
  ./build/v18a5/closure/sample_v18a5_close build/v18a5/closure/samples.csv
  python3 tools/analyze_v18a5_closure.py build/v18a5/closure/samples.csv build/v18a5/closure/PAR_RESULTS.md
fi
