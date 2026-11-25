#!/usr/bin/env bash
# Provisional constitutional administration: compile once per configuration,
# verify delegated scopes in five bounded rounds; never publish a game tick.
set -Eeuo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."
round="${1:-all}"
case "$round" in all|1|2|3|4|5) ;; *) echo "usage: bash tools/physics_closeout.sh [all|1|2|3|4|5]" >&2; exit 2;; esac
cc="${CC:-cc}"
command -v "$cc" >/dev/null
command -v cmake >/dev/null
command -v ctest >/dev/null
command -v python3 >/dev/null
build="build/physics-closeout/$(basename "$cc")"
mkdir -p "$build"
exec > >(tee "$build/closeout-$round.log") 2>&1
trap 'rc=$?; if (( rc != 0 )); then echo "PHYSICS_CLOSEOUT status=FAIL_OR_INTERRUPTED exit=$rc map_handoff=HOLD production=HOLD"; fi' EXIT
echo "PHYSICS_CLOSEOUT source=$(git rev-parse HEAD) round=$round policy=provisional-c2-demo-v1"
dirty="$(git status --porcelain --untracked-files=no)"
if [[ -n "$dirty" ]]; then
  printf "Tracked source changes (not release evidence):\n%s\n" "$dirty"
  echo "PHYSICS_CLOSEOUT provenance=DIRTY_WORKTREE"
else
  echo "PHYSICS_CLOSEOUT provenance=CLEAN_TRACKED_SOURCE"
fi
"$cc" --version | sed -n '1p'
cmake -S . -B "$build" -DNF_CONTACT_LAB_ONLY=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER="$cc"
# Shared build graph is incremental: no clean rebuild for each round.
run(){ local n="$1"; shift; if [[ "$round" == all || "$round" == "$n" ]]; then
  echo "ROUND $n BEGIN"
  "$@"
  echo "ROUND $n PASS"
fi; }
check(){ local pattern="$1"; shift
  cmake --build "$build" --parallel "${JOBS:-2}" --target "$@"
  local expected="$#" actual
  actual="$(ctest --test-dir "$build" -N -R "$pattern" | sed -n 's/^Total Tests: //p')"
  [[ "$actual" == "$expected" ]] || { echo "CHECKLIST mismatch expected=$expected actual=$actual"; return 1; }
  ctest --test-dir "$build" --output-on-failure --no-tests=error -R "$pattern"
}
r1(){ check '^(canonical_history_v18a5|physical_world_closure_v18a5|world_camera_embodiment_v18a6|embodiment_production_controls_v18a6)$' nightfall_v18a5_contact_test nightfall_v18a5_close_test nightfall_v18a6_embodiment_test nightfall_v18a6_production_test; }
r2(){
  cmake --build "$build" --parallel "${JOBS:-2}" --target nightfall_contact_lab
  for test in three_body_island shared_contact a5_frontier material_ledger material_transport; do
    "$cc" -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror -Isrc/shared "src/tests/test_retc_h319_$test.c" "$build/libnightfall_contact_lab.a" -lm -o "$build/$test"
    "$build/$test"
  done
}
r3(){ check '^(contradictory_tick_owner_v18a6|h319_owner_source_frontier_research|eligibility_v18b2|physical_contract_eligibility_v18b2|real_a6_a4_a5_b3_push_v18b6)$' nightfall_v18a6_tick_owner_test nightfall_h319_owner_frontier_test nightfall_v18b2_eligibility_test nightfall_v18b2_contract_test nightfall_v18b6_physics_push_test; }
r4(){ check '^(retc_a6_owner_fails_closed_v18c2|retc_immutable_joint_frame_v18c2|retc_reader_lease_retirement_v18c2|retc_real_dispatcher_two_joint_ticks_v18c2)$' nightfall_v18c2_retc_a6_owner_test nightfall_v18c2_retc_frame_test nightfall_v18c2_retc_reader_lease_test nightfall_v18c2_retc_real_twotick_test; }
r5(){
  cmake --build "$build" --parallel "${JOBS:-2}" --target nightfall_h319_a6_tick_cost
  "$build/nightfall_h319_a6_tick_cost"
  python3 -m unittest discover -s tests -p test_retc_h319_evidence_gate.py
}
run 1 r1
run 2 r2
run 3 r3
run 4 r4
run 5 r5
if [[ "$round" == all && -z "$dirty" && -z "$(git status --porcelain --untracked-files=no)" ]]; then
  echo "PHYSICS_CLOSEOUT status=PASS round=all map_handoff=REVIEW_REQUIRED production=HOLD"
else
  echo "PHYSICS_CLOSEOUT status=PASS round=$round map_handoff=HOLD production=HOLD"
fi
