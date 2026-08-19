#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"
cmd="${1:-help}"

standard_check(){
  bash ./nightfall.sh standard-check
  bash -n ./v16.sh
  test -f src/shared/nf_weapon_solution.h
  test -f src/shared/nf_weapon_solution.c
  test -f src/tests/test_v16_weapon_solution.c
  test -f src/demo/v16_weapon_solution.c
  test -f docs/LEDGER_V1.6_WEAPON_SOLUTION_FOCUS.md
  test -f docs/HANDOFF_V1.6_WEAPON_SOLUTION_FOCUS.md
  grep -q 'project(nightfall_punk VERSION 1.6' CMakeLists.txt
  grep -q 'nf_weapon_solution.c' CMakeLists.txt
  grep -q 'nightfall_v16_demo' CMakeLists.txt
  grep -q 'weapon_solution_focus_v16' CMakeLists.txt
  grep -q 'NfWeaponSolutionRuntime' src/shared/nf_weapon_solution.h
  grep -q 'context_sensitive=PASS' src/tests/test_v16_weapon_solution.c
  echo "[ok] v1.6 Weapon Solution / Focus source tree present"
}

v16_smoke(){
  if [[ ! -x build/headless/nightfall_v16_weapon_solution_test ]]; then
    ./nightfall.sh build-headless
  fi
  ./build/headless/nightfall_v16_weapon_solution_test | tee build/v16-weapon-solution-smoke.log
  grep -q 'v1.6 weapon solution / Focus contracts: PASS' build/v16-weapon-solution-smoke.log
  grep -q 'BASELINE physical_cone=PASS state_dispersion=PASS deterministic=PASS' build/v16-weapon-solution-smoke.log
  grep -q 'FOCUS context_sensitive=PASS recovery_advantage=PASS sprint_transition=PASS' build/v16-weapon-solution-smoke.log
  grep -q 'RETICLE truthful_projection=PASS' build/v16-weapon-solution-smoke.log
  echo "[ok] v1.6 deterministic Weapon Solution / Focus proof"
}

demo(){
  if [[ ! -x build/full/nightfall_v16_demo ]]; then
    ./nightfall.sh build
  fi
  exec ./build/full/nightfall_v16_demo
}

case "$cmd" in
  standard-check) standard_check ;;
  build) ./nightfall.sh build ;;
  build-headless) ./nightfall.sh build-headless ;;
  test)
    ./nightfall.sh test
    bash ./v15.sh camera-smoke
    bash ./v15.sh integration-smoke
    bash ./v15.sh v14-smoke
    bash ./v15.sh v15-smoke
    v16_smoke
    ;;
  v16-smoke) v16_smoke ;;
  regression) bash ./v15.sh regression ;;
  demo) demo ;;
  help|--help|-h)
    cat <<'EOF'
nightfall!punk v1.6 Weapon Solution / Focus

  ./v16.sh standard-check
  ./v16.sh build
  ./v16.sh build-headless
  ./v16.sh test
  ./v16.sh v16-smoke
  ./v16.sh regression
  ./v16.sh demo

Demo controls:
  WASD / Shift / Ctrl / Space
  LMB fire | RMB Focus | R reload | 1/2 weapons
  F5 reset

Human proof sequence:
  1. stationary hip-fire at near/mid/far targets;
  2. move/strafe and fire without RMB;
  3. hold RMB while moving for ~5 seconds;
  4. keep RMB held and stop for ~3 seconds;
  5. sustain a short burst and watch truthful brackets recover;
  6. crouch-fire and jump-fire briefly;
  7. move near solid geometry and verify the physical muzzle cannot shoot through it;
  8. keep the SOLUTION diagnostics visible long enough to read cone components.
EOF
    ;;
  *) echo "unknown v1.6 command: $cmd" >&2; exit 2 ;;
esac
