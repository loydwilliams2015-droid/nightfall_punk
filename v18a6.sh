#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"
MODE="${1:-all}"
BUILD_DIR="build/v18a6/offline-$(basename "${CC:-cc}")"
case "$MODE" in all|build|test|samples|regression) ;; *) echo 'Usage: bash ./v18a6.sh [all|build|test|samples|regression]' >&2; exit 2;; esac
mkdir -p build/v18a6
if [[ "$MODE" == regression || "$MODE" == all ]]; then
    # Avoid recursive execution of every historical sample generator. Each
    # historical fixture is compiled and tested exactly once here.
    { bash ./v18a1.sh test
      bash ./v18a2.sh test
      bash ./v18a3.sh test
      bash ./v18a4.sh test
      bash ./v18a5_close.sh test; } > build/v18a6/inherited_v18a1-v18a5.log
fi
if [[ "$MODE" != regression ]]; then
    cmake -S . -B ${BUILD_DIR} -DNF_CONTACT_LAB_ONLY=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER="${CC:-cc}" >/dev/null
    cmake --build ${BUILD_DIR} --parallel >/dev/null
fi
if [[ "$MODE" == test || "$MODE" == all ]]; then
    ctest --test-dir ${BUILD_DIR} --output-on-failure | tee build/v18a6/ctest.log
    ./${BUILD_DIR}/nightfall_v18a6_embodiment_test > build/v18a6/fixture_results.csv
fi
if [[ "$MODE" == samples || "$MODE" == all ]]; then
    ./${BUILD_DIR}/nightfall_v18a6_sample_db build/v18a6/models.csv
    python3 tools/analyze_v18a6_embodiment.py build/v18a6/models.csv build/v18a6/PAR_RESULTS.md | tee build/v18a6/analysis.log
    ./${BUILD_DIR}/nightfall_v18a6_sample_db build/v18a6/models-repeat.csv
    cmp build/v18a6/models.csv build/v18a6/models-repeat.csv
    sha256sum build/v18a6/models.csv build/v18a6/models-repeat.csv | tee build/v18a6/hashes.txt
fi
