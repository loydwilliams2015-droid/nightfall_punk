#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

cmd="${1:-help}"

standard_check() {
    ./v12.sh standard-check
    bash -n ./v12b.sh
    test -f src/client/nf_camera.h
    test -f src/client/nf_camera.c
    test -f src/tests/test_camera.c
    test -f src/demo/v12_greybox.c
    test -f docs/LEDGER_V1.2B_CAMERA_REBUILD.md
    grep -q 'nightfall_camera' CMakeLists.txt
    grep -q 'relational_camera_v12b' CMakeLists.txt
    grep -q 'NF_CAMERA_FIRST_PERSON' src/client/nf_camera.h
    grep -q 'BLOCKED' src/demo/v12_greybox.c
    echo "[ok] v1.2b relational perceptual authority camera source tree present"
}

camera_smoke() {
    if [[ ! -x build/headless/nightfall_camera_test ]]; then
        ./nightfall.sh build-headless
    fi
    ./build/headless/nightfall_camera_test | tee build/v12b-camera-smoke.log
    grep -q 'camera relational perceptual authority tests: PASS' build/v12b-camera-smoke.log
    grep -q 'LOOK=IMMEDIATE BODY=TRUTH PRESENTATION=BOUNDED RELOCATION=DISCONTINUITY' build/v12b-camera-smoke.log
    echo "[ok] v1.2b camera causal contract proof"
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
        camera_smoke
        ./v12.sh teammate-smoke
        ;;
    camera-smoke)
        camera_smoke
        ;;
    teammate-smoke)
        ./v12.sh teammate-smoke
        ;;
    demo)
        demo
        ;;
    regression)
        ./v12.sh regression
        ;;
    help|--help|-h)
        cat <<'EOF'
nightfall!punk v1.2b camera rebuild wrapper

  ./v12b.sh standard-check  inherited v1.2 checks + camera contracts
  ./v12b.sh build           strict graphical Linux compile
  ./v12b.sh build-headless  strict headless compile
  ./v12b.sh test            inherited CTests + camera + teammate causal proofs
  ./v12b.sh camera-smoke    direct camera contract proof
  ./v12b.sh teammate-smoke  inherited v1.2 positive/negative teammate proof
  ./v12b.sh regression      inherited network/combat/ecology/energy/Lifeworld smokes
  ./v12b.sh demo            run first-person v1.2b causal greybox

Human demo controls:
  F1 PLAYER VIEW | F2 OBSERVER/LIVE | F3 DEBUG TOP/LIVE | F5 reset
  Player view: mouse look | WASD | Shift sprint | Ctrl crouch | Space jump | E ALTER_ROUTE
EOF
        ;;
    *)
        echo "unknown v1.2b command: $cmd" >&2
        exit 2
        ;;
esac
