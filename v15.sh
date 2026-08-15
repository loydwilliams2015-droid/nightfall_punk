#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"
cmd="${1:-help}"

standard_check(){
  bash ./nightfall.sh standard-check
  bash -n ./v15.sh
  test -f src/shared/nf_belief.h
  test -f src/shared/nf_belief.c
  test -f src/shared/nf_metabolic.h
  test -f src/shared/nf_metabolic.c
  test -f src/shared/nf_comm_present.h
  test -f src/shared/nf_comm_present.c
  test -f src/tests/test_v15_integration.c
  test -f src/demo/v15_greybox.c
  test -f docs/LEDGER_V1.5_BUILD_INTEGRATION.md
  test -f docs/HANDOFF_V1.5_BUILD.md
  grep -q 'project(nightfall_punk VERSION 1.5' CMakeLists.txt
  grep -q 'nightfall_v15_demo' CMakeLists.txt
  grep -q 'vertical_integration_v15' CMakeLists.txt
  grep -q 'NF_BELIEF_SUBJECT_UNKNOWN_THREAT' src/shared/nf_belief.h
  grep -q 'NF_TACTICAL_CANDIDATE_CAP 12u' src/shared/nf_tactical.h
  grep -q 'NF_INFO_CHANNEL_DAMAGE_RECEIVED' src/shared/nf_report.h
  grep -q 'NfMetabolicLink' src/shared/nf_metabolic.h
  grep -q 'NfCommPresentEvent' src/shared/nf_comm_present.h
  ! grep -q 'NfAiBlackboard' src/server/ai/nf_ai.h
  echo "[ok] v1.5 vertical integration source tree present"
}

v15_smoke(){
  if [[ ! -x build/headless/nightfall_v15_integration_test ]]; then
    ./nightfall.sh build-headless
  fi
  ./build/headless/nightfall_v15_integration_test | tee build/v15-integration-smoke.log
  grep -q 'v1.5 vertical integration contracts: PASS' build/v15-integration-smoke.log
  grep -q 'BELIEF anonymous=PASS ancestry_no_gain=PASS' build/v15-integration-smoke.log
  grep -q 'TACTICAL nested_ordering=PASS viability_gate=PASS' build/v15-integration-smoke.log
  grep -q 'METABOLIC restitution_gap=PASS capacity_consequence=PASS' build/v15-integration-smoke.log
  grep -q 'COMM precision_boundary=PASS' build/v15-integration-smoke.log
  echo "[ok] v1.5 belief/nested/metabolic/communication integration proof"
}

demo(){
  if [[ ! -x build/full/nightfall_v15_demo ]]; then
    ./nightfall.sh build
  fi
  exec ./build/full/nightfall_v15_demo
}

case "$cmd" in
  standard-check) standard_check ;;
  build) ./nightfall.sh build ;;
  build-headless) ./nightfall.sh build-headless ;;
  test)
    ./nightfall.sh test
    bash ./v12c.sh camera-smoke
    bash ./v12c.sh integration-smoke
    bash ./v14.sh v14-smoke
    v15_smoke
    ;;
  v15-smoke) v15_smoke ;;
  camera-smoke) bash ./v12c.sh camera-smoke ;;
  integration-smoke) bash ./v12c.sh integration-smoke ;;
  v14-smoke) bash ./v14.sh v14-smoke ;;
  regression) bash ./v12c.sh regression ;;
  demo) demo ;;
  help|--help|-h)
    cat <<'EOF'
nightfall!punk v1.5 vertical integration

  ./v15.sh standard-check
  ./v15.sh build
  ./v15.sh build-headless
  ./v15.sh test
  ./v15.sh v15-smoke
  ./v15.sh regression
  ./v15.sh demo

Demo controls:
  F1 player | F2 observer | F3 top | F5 reset
  WASD / Shift / Ctrl / Space
  LMB fire | RMB Focus | R reload | 1/2 weapons
  Q WATCH ping
  F6 extract / F7 return (metabolic proof controls)

Human proof priorities:
  visible carbine + truthful hit/miss feedback;
  Rival 1 / Rival 2 belief-channel divergence;
  different tactical action selection and physical movement;
  WATCH ping + Crew receipt presentation without command authority;
  F6 extraction raises restitution gap / changes source capacity;
  F7 return improves cycle continuity.
EOF
    ;;
  *) echo "unknown v1.5 command: $cmd" >&2; exit 2 ;;
esac
