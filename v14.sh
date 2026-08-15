#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"; cd "$ROOT_DIR"; cmd="${1:-help}"
standard_check(){
  bash ./v12c.sh standard-check
  bash -n ./v14.sh
  test -f src/shared/nf_claim.h; test -f src/shared/nf_claim.c
  test -f src/shared/nf_tactical.h; test -f src/shared/nf_tactical.c
  test -f src/tests/test_v14_integration.c; test -f src/demo/v14_greybox.c
  test -f docs/LEDGER_V1.4_BUILD_READINESS.md; test -f docs/HANDOFF_V1.4_HUMAN_ACCEPTANCE.md
  test -f docs/PROJECT_STATE_INDEX.md
  grep -q 'project(nightfall_punk VERSION 1.4' CMakeLists.txt
  grep -q 'NF_ROUTE_GATE_KEY' src/shared/nf_route.h
  grep -q 'high_medium_integration_v14' CMakeLists.txt
  grep -q 'F4 PROOF SETUP' src/demo/v14_greybox.c
  echo "[ok] v1.4 high/medium systems integration source tree present"
}
v14_smoke(){
  if [[ ! -x build/headless/nightfall_v14_integration_test ]]; then ./nightfall.sh build-headless; fi
  ./build/headless/nightfall_v14_integration_test | tee build/v14-integration-smoke.log
  grep -q 'high-medium integration tests: PASS' build/v14-integration-smoke.log
  grep -q 'CLAIM SOFT_SHARE=PASS HARD_BLOCK=PASS EXPIRY=PASS' build/v14-integration-smoke.log
  grep -q 'OBJECT STABLE_ID=PASS REVISION=PASS' build/v14-integration-smoke.log
  grep -q 'EXPOSURE GRADIENT=PASS UTILITY=PASS' build/v14-integration-smoke.log
  grep -q 'WORLD_CHANGE DIRECT_EVIDENCE=PASS BELIEF=PASS GRADIENT=PASS' build/v14-integration-smoke.log
  echo "[ok] v1.4 object/claim/exposure/integration proof"
}
demo(){ if [[ ! -x build/full/nightfall_v14_demo ]]; then ./nightfall.sh build; fi; exec ./build/full/nightfall_v14_demo; }
case "$cmd" in
 standard-check) standard_check;;
 build) ./nightfall.sh build;;
 build-headless) ./nightfall.sh build-headless;;
 test) ./nightfall.sh test; bash ./v12c.sh camera-smoke; bash ./v12c.sh integration-smoke; v14_smoke;;
 v14-smoke) v14_smoke;;
 camera-smoke) bash ./v12c.sh camera-smoke;;
 integration-smoke) bash ./v12c.sh integration-smoke;;
 regression) bash ./v12c.sh regression;;
 demo) demo;;
 help|--help|-h) cat <<'EOF'
nightfall!punk v1.4 high/medium systems integration

  ./v14.sh standard-check
  ./v14.sh build
  ./v14.sh build-headless
  ./v14.sh test
  ./v14.sh v14-smoke
  ./v14.sh regression
  ./v14.sh demo

Demo: F1 player | F2 observer | F3 top | F4 proof setup | F5 reset
Mouse/WASD/Shift/Ctrl/Space | hold E at purple gate

Proof: F4 -> gate rev1 CLOSED -> EVIDENCE NONE -> hold E -> interaction claim
-> gate rev2 OPEN -> DIRECT evidence -> BELIEF OPEN -> gradient/utility/commitment -> embodied action.
Communications + full playable conflict remain v1.5.
EOF
 ;;
 *) echo "unknown v1.4 command: $cmd" >&2; exit 2;;
esac
