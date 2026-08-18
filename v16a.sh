#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"
cmd="${1:-help}"

standard_check(){
  bash ./v16.sh standard-check
  bash -n ./v16a.sh
  test -f src/shared/nf_weapon_solution.h
  test -f src/shared/nf_weapon_solution.c
  test -f src/tests/test_v16a_weapon_correspondence.c
  test -f src/demo/v16a_weapon_correspondence.c
  test -f docs/LEDGER_V1.6A_WEAPON_CORRESPONDENCE_CALIBRATION.md
  test -f docs/HANDOFF_V1.6A_WEAPON_CORRESPONDENCE_CALIBRATION.md
  grep -q 'project(nightfall_punk VERSION 1.6.1' CMakeLists.txt
  grep -q 'nightfall_v16a_demo' CMakeLists.txt
  grep -q 'weapon_correspondence_calibration_v16a' CMakeLists.txt
  grep -q 'NfWeaponEnvelope' src/shared/nf_weapon_solution.h
  grep -q 'nf_weapon_evaluate_envelope' src/shared/nf_weapon_solution.c
  grep -q 'present_tense=PASS' src/tests/test_v16a_weapon_correspondence.c
  echo "[ok] v1.6A Weapon Correspondence / Calibration source tree present"
}

v16a_smoke(){
  if [[ ! -x build/headless/nightfall_v16a_weapon_correspondence_test ]]; then
    ./nightfall.sh build-headless
  fi
  ./build/headless/nightfall_v16a_weapon_correspondence_test | tee build/v16a-weapon-correspondence-smoke.log
  grep -q 'v1.6A weapon correspondence / calibration contracts: PASS' build/v16a-weapon-correspondence-smoke.log
  grep -q 'TEMPORAL current_envelope=PASS accepted_shot_history=PASS legacy_coherence=PASS' build/v16a-weapon-correspondence-smoke.log
  grep -q 'FOCUS timing_200_250ms=PASS release_fast=PASS contextual_reduction=PASS' build/v16a-weapon-correspondence-smoke.log
  grep -q 'RETICLE present_tense=PASS mild_nonlinear=PASS fov_aware=PASS' build/v16a-weapon-correspondence-smoke.log
  echo "[ok] v1.6A deterministic correspondence / calibration proof"
}

demo(){
  if [[ ! -x build/full/nightfall_v16a_demo ]]; then
    ./nightfall.sh build
  fi
  exec ./build/full/nightfall_v16a_demo
}

case "$cmd" in
  standard-check) standard_check ;;
  build) ./nightfall.sh build ;;
  build-headless) ./nightfall.sh build-headless ;;
  test)
    bash ./v16.sh test
    v16a_smoke
    ;;
  v16a-smoke) v16a_smoke ;;
  regression) bash ./v16.sh regression ;;
  demo) demo ;;
  help|--help|-h)
    cat <<'EOF'
nightfall!punk v1.6A Weapon Correspondence / Calibration

  ./v16a.sh standard-check
  ./v16a.sh build
  ./v16a.sh build-headless
  ./v16a.sh test
  ./v16a.sh v16a-smoke
  ./v16a.sh regression
  ./v16a.sh demo

Demo controls:
  WASD / Shift / Ctrl / Space
  LMB fire | RMB Focus | F developer Focus control
  R reload | 1/2 weapons | F5 reset

Human proof sequence (one continuous 90–150 second recording is enough):
  1. Stationary, no fire: keep NOW diagnostics readable.
  2. Move then stop without firing: brackets must expand then recover in present tense.
  3. Hold RMB at sky/background for ~3 seconds: RMB=DOWN and Focus should approach 1.0.
  4. Repeat with F developer control as the negative-control input path.
  5. Use foreground geometry while holding Focus so ~7 degree FOV narrowing is visible.
  6. Move while focused, stop while still focused, then release; compare recapture with hip behavior.
  7. Fire at least one shot while Focus is materially nonzero; LAST SHOT must record Focus/reduction.
  8. Fire pistol, switch to carbine without firing: NOW must update while LAST SHOT stays pistol history.
  9. Fire a short burst and release input: live brackets/recoil must recover without persistent camera aim theft.
 10. Approach solid geometry until blocked=YES, then fire: round is accepted, recoil/sound/impact occur nearby, target is not hit through the obstruction.

Report back:
  Upload the video. No written report is required. Mention a timestamp only if you see RMB dropping during a hold,
  stale NOW brackets, Focus that feels mandatory/useless, unexplained miss, camera possession, false blocked cue,
  target damage through obstruction, or a crash/regression.
EOF
    ;;
  *) echo "unknown v1.6A command: $cmd" >&2; exit 2 ;;
esac
