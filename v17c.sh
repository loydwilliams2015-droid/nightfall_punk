#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"
cmd="${1:-help}"

build_v17c(){
  mkdir -p build/v17c
  cc -std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc/shared \
    src/shared/nf_spatial_logic.c src/shared/nf_logic17b.c src/shared/nf_logic17c.c \
    src/tests/test_v17c_logic.c -lm \
    -o build/v17c/nightfall_v17c_logic_test
  echo "[ok] v1.7C strict calibration/coupling/attribution compile"
}

standard_check(){
  bash ./v17b.sh standard-check
  bash -n ./v17c.sh
  test -f src/shared/nf_logic17c.h
  test -f src/shared/nf_logic17c.c
  test -f src/tests/test_v17c_logic.c
  grep -q 'Nf17cCalibrator' src/shared/nf_logic17c.h
  grep -q 'Nf17cUtilityCandidate' src/shared/nf_logic17c.h
  grep -q 'Nf17cPurpleEnvelope' src/shared/nf_logic17c.h
  grep -q 'Nf17cAttributionRow' src/shared/nf_logic17c.h
  grep -q 'Nf17cReasonTrace' src/shared/nf_logic17c.h
  echo "[ok] v1.7C calibration / Purple / attribution contracts present"
}

test_v17c(){
  test -x build/v17c/nightfall_v17c_logic_test || build_v17c
  ./build/v17c/nightfall_v17c_logic_test | tee build/v17c/contracts.log
  grep -q 'v1.7C calibration/coupling/attribution: PASS' build/v17c/contracts.log
}

case "$cmd" in
  standard-check) standard_check ;;
  build) build_v17c ;;
  test) build_v17c; test_v17c ;;
  regression)
    bash ./v17b.sh regression
    standard_check
    build_v17c
    test_v17c
    ;;
  help|--help|-h)
    cat <<'HELP'
nightfall!punk v1.7C — Calibration, Coupling, and Causal Attribution

  ./v17c.sh standard-check
  ./v17c.sh build
  ./v17c.sh test
  ./v17c.sh regression

Cumulative basis:
  v1.7A spatial logic
  -> v1.7B route intelligence + snapshot transactions + CBC/frontier/refinement
  -> v1.7C separate confidence calibration + lexicographic utility
     + dependency-component Purple + fractional-factorial causal attribution
     + deterministic reason traces.
HELP
    ;;
  *) echo "unknown v1.7C command: $cmd" >&2; exit 2 ;;
esac
