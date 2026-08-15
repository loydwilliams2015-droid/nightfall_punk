#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

cmd="${1:-help}"

standard_check() {
    ./nightfall.sh standard-check
    bash -n ./v12.sh
    test -f src/shared/nf_route.c
    test -f src/server/ai/nf_teammate.c
    test -f src/tests/test_teammate.c
    test -f src/demo/v12_greybox.c
    test -f docs/LEDGER_V1.2_BUILD_READINESS.md
    grep -Eq 'project\(nightfall_punk VERSION (1\.2|1\.4)' CMakeLists.txt
    echo "[ok] v1.2 teammate causality / kinetic stigmergy source tree present (native or compatible descendant)"
}

teammate_smoke() {
    if [[ ! -x build/headless/nightfall_teammate_test ]]; then
        ./nightfall.sh build-headless
    fi
    ./build/headless/nightfall_teammate_test | tee build/v12-teammate-smoke.log
    grep -Eq 'teammate causality / kinetic stigmergy tests: PASS|teammate causality / integrated embodied action tests: PASS' build/v12-teammate-smoke.log
    grep -q 'EVIDENCE=NONE' build/v12-teammate-smoke.log
    grep -q 'ACTION=REROUTE' build/v12-teammate-smoke.log
    echo "[ok] v1.2 teammate causal positive + negative proof"
}

demo() {
    if [[ ! -x build/full/nightfall_v12_demo ]]; then
        ./nightfall.sh build
    fi
    exec ./build/full/nightfall_v12_demo
}

case "$cmd" in
    standard-check)
        standard_check
        ;;
    build)
        ./nightfall.sh build
        ;;
    build-headless)
        ./nightfall.sh build-headless
        ;;
    test)
        ./nightfall.sh test
        teammate_smoke
        ;;
    teammate-smoke)
        teammate_smoke
        ;;
    demo)
        demo
        ;;
    regression)
        ./nightfall.sh combat-smoke
        ./nightfall.sh encounter-smoke
        ./nightfall.sh spatial-smoke
        ./nightfall.sh cattler-smoke
        ./nightfall.sh energy-smoke
        ./nightfall.sh lifeworld-smoke
        ./nightfall.sh contamination-smoke
        ;;
    help|--help|-h)
        cat <<'EOF'
nightfall!punk v1.2 wrapper

  ./v12.sh standard-check   inherited checks + v1.2 causal-slice contracts
  ./v12.sh build            strict graphical Linux compile
  ./v12.sh build-headless   strict headless compile
  ./v12.sh test             all inherited CTests + named teammate causal proof
  ./v12.sh teammate-smoke   direct positive/negative causal proof
  ./v12.sh regression       inherited network/combat/ecology/energy/Lifeworld smokes
  ./v12.sh demo             run the 80x80 m graphical causal greybox

Human demo controls:
  WASD move | Shift sprint | E ALTER_ROUTE | F5 reset | Esc exit
EOF
        ;;
    *)
        echo "unknown v1.2 command: $cmd" >&2
        exit 2
        ;;
esac
