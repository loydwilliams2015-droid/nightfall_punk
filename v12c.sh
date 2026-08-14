#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

cmd="${1:-help}"

standard_check() {
    ./v12b.sh standard-check
    bash -n ./v12c.sh
    test -f src/tests/test_teammate.c
    test -f src/demo/v12_greybox.c
    grep -q 'v1.2c' src/demo/v12_greybox.c
    grep -q 'NF_V12C_RIVALS' src/demo/v12_greybox.c
    grep -q 'NF_V12C_CATTLERS' src/demo/v12_greybox.c
    grep -q 'DISPLACEMENT' src/tests/test_teammate.c
    grep -q 'AFFORDANCE' src/demo/v12_greybox.c
    echo "[ok] v1.2c integrated population / embodied causality source tree present"
}

integration_smoke() {
    if [[ ! -x build/headless/nightfall_teammate_test ]]; then
        ./nightfall.sh build-headless
    fi
    ./build/headless/nightfall_teammate_test | tee build/v12c-integration-smoke.log
    grep -q 'v1.2c teammate causality / integrated embodied action tests: PASS' build/v12c-integration-smoke.log
    grep -q 'POPULATION PLAYER=1 TEAMMATE=1 RIVAL=2 CATTLER=2' build/v12c-integration-smoke.log
    grep -q 'TEAMMATE CONTROL=' build/v12c-integration-smoke.log
    grep -q 'EVIDENCE=NONE' build/v12c-integration-smoke.log
    grep -q 'ACTION=REROUTE' build/v12c-integration-smoke.log
    echo "[ok] v1.2c population + embodied teammate causal proof"
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
        ./v12b.sh camera-smoke
        integration_smoke
        ;;
    integration-smoke)
        integration_smoke
        ;;
    camera-smoke)
        ./v12b.sh camera-smoke
        ;;
    regression)
        ./v12b.sh regression
        ;;
    demo)
        demo
        ;;
    help|--help|-h)
        cat <<'EOF'
nightfall!punk v1.2c integrated population / embodied causality wrapper

  ./v12c.sh standard-check      inherited camera checks + v1.2c integration contracts
  ./v12c.sh build               strict graphical Linux compile
  ./v12c.sh build-headless      strict headless compile
  ./v12c.sh test                inherited CTests + camera + embodied population proof
  ./v12c.sh integration-smoke   population + teammate physical displacement proof
  ./v12c.sh camera-smoke        inherited relational camera proof
  ./v12c.sh regression          inherited network/combat/ecology/energy/Lifeworld smokes
  ./v12c.sh demo                run integrated first-person causal greybox

Human demo controls:
  F1 PLAYER VIEW | F2 OBSERVER/LIVE | F3 DEBUG TOP/LIVE | F5 reset
  Mouse look | WASD | Shift sprint | Ctrl crouch | Space jump | hold E at purple ALTER_ROUTE gate

Human proof should visibly show:
  chrome-blue teammate moving; red Human Rivals moving; orange/yellow Dream Cattlers moving;
  ALTER_ROUTE AFFORDANCE READY -> E HELD -> OPEN; teammate evidence/commitment then changed trajectory.
EOF
        ;;
    *)
        echo "unknown v1.2c command: $cmd" >&2
        exit 2
        ;;
esac
