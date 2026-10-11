#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
if [[ -f /workspace/nightfall-setup/env.sh ]]; then source /workspace/nightfall-setup/env.sh; fi
jobs=$(nproc)
if (( jobs > 5 )); then jobs=5; fi
mode=${1:-all}
evidence=build/b2-evidence
mkdir -p "$evidence"
if [[ "$mode" == all || "$mode" == check ]]; then
  clang_bin=$(command -v clang-19 || command -v clang)
  for compiler in gcc "$clang_bin"; do
    name=$(basename "$compiler")
    cmake -S . -B "build/b2-$name" -DNF_CONTACT_LAB_ONLY=ON \
      -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS_RELEASE=-O2 -DCMAKE_C_COMPILER="$compiler"
    cmake --build "build/b2-$name" --parallel "$jobs"
    ctest --test-dir "build/b2-$name" --output-on-failure | tee "$evidence/$name-tests.log"
  done
  cmake -S . -B build/b2-asan -DNF_CONTACT_LAB_ONLY=ON -DCMAKE_BUILD_TYPE=Debug \
    '-DCMAKE_C_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer'
  cmake --build build/b2-asan --parallel "$jobs"
  ctest --test-dir build/b2-asan --output-on-failure | tee "$evidence/asan-tests.log"
  build/b2-gcc/nightfall_v18b2_contract_test > "$evidence/targeted.csv"
fi
if [[ "$mode" == all || "$mode" == benchmark ]]; then
  # No concurrent compilation while measuring. These seeds and strata were
  # registered before tuning. Repeats are NOT independent held-out worlds.
  for run in 1 2 3; do
    build/b2-gcc/nightfall_v18b2_sample_db calibration "$evidence/run$run-calibration.csv"
    build/b2-gcc/nightfall_v18b2_sample_db held-out "$evidence/run$run-held-out.csv"
  done
  python3 tools/analyze_v18b2_eligibility.py "$evidence"
fi
if [[ "$mode" == engine ]]; then
  cmake -S . -B build/full -DCMAKE_BUILD_TYPE=Debug -DNF_BUILD_CLIENT=ON
  cmake --build build/full --parallel "$jobs"
  ctest --test-dir build/full --output-on-failure | tee "$evidence/engine-tests.log"
fi
case "$mode" in all|check|benchmark|engine) ;; *) echo "usage: bash v18b2.sh all|check|benchmark|engine" >&2; exit 2;; esac
