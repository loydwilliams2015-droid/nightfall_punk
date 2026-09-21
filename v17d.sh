#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"
cmd="${1:-help}"

build_v17d(){
  mkdir -p build/v17d
  cc -std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc/shared \
    src/shared/nf_spatial_logic.c \
    src/shared/nf_logic17b.c \
    src/shared/nf_logic17c.c \
    src/shared/nf_cleave17d.c \
    src/tests/test_v17d_cleave.c -lm \
    -o build/v17d/nightfall_v17d_cleave_test
  echo "[ok] v1.7D strict Cleave compile"
}

standard_check(){
  bash ./v17c.sh standard-check
  bash -n ./v17d.sh
  test -f src/shared/nf_cleave17d.h
  test -f src/shared/nf_cleave17d.c
  test -f src/tests/test_v17d_cleave.c
  grep -q 'Nf17dCleavePolicy' src/shared/nf_cleave17d.h
  grep -q 'Nf17dConflictSet' src/shared/nf_cleave17d.h
  grep -q 'Nf17dCacheStamp' src/shared/nf_cleave17d.h
  grep -q 'Nf17dBudgetPool' src/shared/nf_cleave17d.h
  grep -q 'Nf17dObservabilityFrame' src/shared/nf_cleave17d.h
  echo "[ok] v1.7D cleave / cache / observability contracts present"
}

test_v17d(){
  test -x build/v17d/nightfall_v17d_cleave_test || build_v17d
  ./build/v17d/nightfall_v17d_cleave_test | tee build/v17d/contracts.log
  grep -q 'v1.7D cleave contracts: PASS' build/v17d/contracts.log
}

case "$cmd" in
  standard-check) standard_check ;;
  build) build_v17d ;;
  test) build_v17d; test_v17d ;;
  regression)
    bash ./v17c.sh regression
    standard_check
    build_v17d
    test_v17d
    ;;
  help|--help|-h)
    cat <<'HELP'
nightfall!punk v1.7D — Cleave Build

  ./v17d.sh standard-check
  ./v17d.sh build
  ./v17d.sh test
  ./v17d.sh regression

Cumulative lineage:
  v1.7A spatial logic
  -> v1.7B hybrid contracts / route intelligence
  -> v1.7C calibration / coupling / attribution
  -> v1.7D cleave: promote supported paths, retain experimental interfaces,
     disable/prune unsupported defaults, version derived-state dependencies,
     group conflict sets, account deterministic budget borrowing, and expose
     read-only observability presets for v1.7E.
HELP
    ;;
  *) echo "unknown v1.7D command: $cmd" >&2; exit 2 ;;
esac
