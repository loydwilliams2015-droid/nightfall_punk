#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"
cmd="${1:-help}"

build_v17b(){
  mkdir -p build/v17b
  cc -std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc/shared \
    src/shared/nf_spatial_logic.c src/shared/nf_logic17b.c src/tests/test_v17b_logic.c -lm \
    -o build/v17b/nightfall_v17b_logic_test
  echo "[ok] v1.7B strict hybrid-contract compile"
}

standard_check(){
  bash ./v17a.sh standard-check
  bash -n ./v17b.sh
  test -f src/shared/nf_logic17b.h
  test -f src/shared/nf_logic17b.c
  test -f src/tests/test_v17b_logic.c
  grep -q 'Nf17bTransaction' src/shared/nf_logic17b.h
  grep -q 'Nf17bAgreementStage' src/shared/nf_logic17b.h
  grep -q 'Nf17bCrossNexusTransfer' src/shared/nf_logic17b.h
  grep -q 'Nf17bRouteHypothesis' src/shared/nf_logic17b.h
  grep -q 'Nf17bTrailField' src/shared/nf_logic17b.h
  echo "[ok] v1.7B route/transaction/cascade/refinement contracts present"
}

test_v17b(){
  test -x build/v17b/nightfall_v17b_logic_test || build_v17b
  ./build/v17b/nightfall_v17b_logic_test | tee build/v17b/contracts.log
  grep -q 'v1.7B hybrid contracts: PASS' build/v17b/contracts.log
}

case "$cmd" in
  standard-check) standard_check ;;
  build) build_v17b ;;
  test) build_v17b; test_v17b ;;
  regression)
    bash ./v17a.sh test
    standard_check
    build_v17b
    test_v17b
    ;;
  help|--help|-h)
    cat <<'HELP'
nightfall!punk v1.7B — Hybrid Contracts / Route Intelligence

  ./v17b.sh standard-check
  ./v17b.sh build
  ./v17b.sh test
  ./v17b.sh regression

Cumulative basis:
  v1.7A spatial logic + bounded route hypotheses + actor-scoped trail memory
  + authoritative snapshot transactions + agreement compensation horizon
  + material Causal Boundary Crossing + causal frontier + refinement metrics.
HELP
    ;;
  *) echo "unknown v1.7B command: $cmd" >&2; exit 2 ;;
esac
