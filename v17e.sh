#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"
cmd="${1:-help}"

build_v17e(){
  mkdir -p build/v17e
  cc -std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc/shared \
    src/shared/nf_spatial_logic.c \
    src/shared/nf_logic17b.c \
    src/shared/nf_logic17c.c \
    src/shared/nf_cleave17d.c \
    src/shared/nf_observe17e.c \
    src/tests/test_v17e_observe.c -lm \
    -o build/v17e/nightfall_v17e_observe_test
  echo "[ok] v1.7E strict observability compile"
}

standard_check(){
  bash ./v17d.sh standard-check
  bash -n ./v17e.sh
  test -f src/shared/nf_observe17e.h
  test -f src/shared/nf_observe17e.c
  test -f src/tests/test_v17e_observe.c
  grep -q 'Nf17eOverlayObject' src/shared/nf_observe17e.h
  grep -q 'NF17E_COMPOSITE_RAINBOW' src/shared/nf_observe17e.h
  grep -q 'nf17e_compose_overlays' src/shared/nf_observe17e.h
  echo "[ok] v1.7E multidimensional overlay contracts present"
}

test_v17e(){
  test -x build/v17e/nightfall_v17e_observe_test || build_v17e
  ./build/v17e/nightfall_v17e_observe_test | tee build/v17e/contracts.log
  grep -q 'v1.7E overlay compositor: PASS' build/v17e/contracts.log
}

case "$cmd" in
  standard-check) standard_check ;;
  build) build_v17e ;;
  test) build_v17e; test_v17e ;;
  regression)
    bash ./v17d.sh regression
    standard_check
    build_v17e
    test_v17e
    ;;
  help|--help|-h)
    cat <<'HELP'
nightfall!punk v1.7E — Graphical Observability

  ./v17e.sh standard-check
  ./v17e.sh build
  ./v17e.sh test
  ./v17e.sh regression

Current primitive:
  overlapping diagnostic dimensions compose into one multidimensional object.
  Higher-priority dimensions suppress lower-priority presentation without
  deleting metadata. Equal-priority dimensions remain compound/rainbow.
HELP
    ;;
  *) echo "unknown v1.7E command: $cmd" >&2; exit 2 ;;
esac
