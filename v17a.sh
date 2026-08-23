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
    src/shared/nf_spatial_logic.c src/tools/v17a_spatial_lab.c -lm \
    -o build/v17a/nightfall_v17a_spatial_lab
  echo "[ok] v1.7A strict spatial laboratory compile"
}

standard_check(){
  ./v16b.sh standard-check
  bash -n ./v17a.sh
  test -f src/shared/nf_spatial_logic.h
  test -f src/shared/nf_spatial_logic.c
  test -f src/tools/v17a_spatial_lab.c
  test -f src/tests/test_v17a_spatial_logic.c
  test -f tools/analyze_v17a_spatial_samples.py
  test -f docs/LEDGER_V1.7A_SPATIAL_LOGIC.md
  test -f docs/HANDOFF_V1.7A_SPATIAL_LOGIC.md
  grep -q 'NfContractExchangeEvent' src/shared/nf_spatial_logic.h
  grep -q 'contract_memory' src/shared/nf_spatial_logic.h
  grep -q 'NF_SPATIAL_POLICY_GREEDY' src/shared/nf_spatial_logic.h
  echo "[ok] v1.7A spatial ontology / ownership contract present"
}

v17a_test(){
  test -x build/v17a/nightfall_v17a_spatial_test || build_lab
  ./build/v17a/nightfall_v17a_spatial_test | tee build/v17a/contracts.log
  grep -q 'v1.7A spatial logic contracts: PASS' build/v17a/contracts.log
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

case "$cmd" in
  standard-check) standard_check ;;
  build) build_lab ;;
  test) build_lab; v17a_test ;;
  sample-db) build_lab; sample_db "$@" ;;
  regression)
    standard_check
    ./v16b.sh test
    build_lab
    v17a_test
    sample_db sample-db 512
    ;;
  help|--help|-h)
    cat <<'HELP'
nightfall!punk v1.7A — Spatial Logic Laboratory

  ./v17a.sh standard-check
  ./v17a.sh build
  ./v17a.sh test
  ./v17a.sh sample-db [variants-per-lab]
  ./v17a.sh regression

The default sample database runs 8 spatial laboratories x 512 procedural
variants x 3 policies (informed / greedy / random) x 2 deterministic
replicates = 24,576 AI-player rounds.
HELP
    ;;
  *) echo "unknown v1.7A command: $cmd" >&2; exit 2 ;;
esac
