#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"
cmd="${1:-help}"

build_lab(){
  mkdir -p build/v17a
  cc -std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc/shared \
    src/shared/nf_spatial_logic.c src/tests/test_v17a_spatial_logic.c -lm \
    -o build/v17a/nightfall_v17a_spatial_test
  cc -std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc/shared \
    src/shared/nf_spatial_logic.c src/shared/nf_spatial_review.c src/tests/test_v17a_spatial_review.c -lm \
    -o build/v17a/nightfall_v17a_review_test
  cc -std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc/shared \
    src/shared/nf_spatial_logic.c src/tools/v17a_spatial_lab.c -lm \
    -o build/v17a/nightfall_v17a_spatial_lab
  cc -std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc/shared \
    src/shared/nf_spatial_logic.c src/shared/nf_spatial_review.c src/tools/v17a_graphical_review.c -lm \
    -o build/v17a/nightfall_v17a_graphical_review
  echo "[ok] v1.7A strict spatial and graphical-review compile"
}

standard_check(){
  ./v16b.sh standard-check
  bash -n ./v17a.sh
  test -f src/shared/nf_spatial_logic.h
  test -f src/shared/nf_spatial_logic.c
  test -f src/shared/nf_spatial_review.h
  test -f src/shared/nf_spatial_review.c
  test -f src/tools/v17a_spatial_lab.c
  test -f src/tools/v17a_graphical_review.c
  test -f src/tests/test_v17a_spatial_logic.c
  test -f src/tests/test_v17a_spatial_review.c
  test -f src/demo/v17a_spatial_viewer.c
  test -f tools/select_v17a_graphical_seeds.py
  test -f tools/analyze_v17a_spatial_samples.py
  test -f tools/analyze_v17a_graphical_review.py
  test -f tools/v17a_viewer/CMakeLists.txt
  test -f docs/LEDGER_V1.7A_SPATIAL_LOGIC.md
  test -f docs/HANDOFF_V1.7A_SPATIAL_LOGIC.md
  grep -q 'NfContractExchangeEvent' src/shared/nf_spatial_logic.h
  grep -q 'contract_memory' src/shared/nf_spatial_logic.h
  grep -q 'NF_SPATIAL_POLICY_GREEDY' src/shared/nf_spatial_logic.h
  grep -q 'NF_SPATIAL_REVIEW_PERCEPTUAL' src/shared/nf_spatial_review.h
  echo "[ok] v1.7A spatial ontology / ownership / player-channel review contract present"
}

v17a_test(){
  test -x build/v17a/nightfall_v17a_spatial_test || build_lab
  ./build/v17a/nightfall_v17a_spatial_test | tee build/v17a/contracts.log
  grep -q 'v1.7A spatial logic contracts: PASS' build/v17a/contracts.log
  ./build/v17a/nightfall_v17a_review_test | tee build/v17a/review-contracts.log
  grep -q 'v1.7A graphical review contracts: PASS' build/v17a/review-contracts.log
}

sample_db(){
  local variants="${2:-512}"
  test -x build/v17a/nightfall_v17a_spatial_lab || build_lab
  mkdir -p build/v17a-samples
  ./build/v17a/nightfall_v17a_spatial_lab build/v17a-samples/samples.csv "$variants"
  python3 tools/analyze_v17a_spatial_samples.py \
    build/v17a-samples/samples.csv \
    build/v17a-samples/report.md \
    build/v17a-samples/soil.csv \
    build/v17a-samples/par.md
  echo "[ok] v1.7A AI-player sample database and OITOA/PAR analysis"
}

graphical_review(){
  test -f build/v17a-samples/samples.csv || sample_db sample-db 512
  test -x build/v17a/nightfall_v17a_graphical_review || build_lab
  mkdir -p build/v17a-review
  python3 tools/select_v17a_graphical_seeds.py \
    build/v17a-samples/samples.csv \
    build/v17a-review/selections.csv
  ./build/v17a/nightfall_v17a_graphical_review \
    build/v17a-review/selections.csv \
    build/v17a-review/review.csv
  echo "[ok] v1.7A 40-seed x 4-mode x 2-replicate graphical-review data"
}

viewer_build(){
  cmake -S tools/v17a_viewer -B build/v17a-viewer -DCMAKE_BUILD_TYPE=Release
  cmake --build build/v17a-viewer --parallel 2
  echo "[ok] v1.7A graphical referee viewer build"
}

case "$cmd" in
  standard-check) standard_check ;;
  build) build_lab ;;
  test) build_lab; v17a_test ;;
  sample-db) build_lab; sample_db "$@" ;;
  graphical-review) build_lab; graphical_review ;;
  viewer-build) viewer_build ;;
  regression)
    standard_check
    ./v16b.sh test
    build_lab
    v17a_test
    sample_db sample-db 512
    graphical_review
    ;;
  help|--help|-h)
    cat <<'HELP'
nightfall!punk v1.7A — Spatial Logic Laboratory

  ./v17a.sh standard-check
  ./v17a.sh build
  ./v17a.sh test
  ./v17a.sh sample-db [variants-per-lab]
  ./v17a.sh graphical-review
  ./v17a.sh viewer-build
  ./v17a.sh regression

Baseline sample database:
  8 labs x 512 procedural variants x 3 policies x 2 deterministic replicates
  = 24,576 AI-player rows.

Graphical correspondence experiment:
  8 labs x 5 canonical seed classes (median/best/worst/pathological/disagreement)
  x 4 modes (instrumented/perceptual/greedy/random) x 2 deterministic replicates
  = 320 review rows, plus 40 instrumented-vs-perceptual referee frames.
HELP
    ;;
  *) echo "unknown v1.7A command: $cmd" >&2; exit 2 ;;
esac
