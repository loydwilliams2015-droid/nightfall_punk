#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

cmd="${1:-help}"

standard_check() {
    bash ./v12c.sh standard-check
    bash -n ./v14.sh
    test -f src/shared/nf_report.h
    test -f src/shared/nf_report.c
    test -f src/shared/nf_claim.h
    test -f src/shared/nf_claim.c
    test -f src/tests/test_v14_relations.c
    test -f src/demo/v14_greybox.c
    test -f docs/LEDGER_V1.4_COMBAT_COMPARE10_COMMS.md
    test -f docs/LEDGER_V1.4_BUILD_READINESS.md
    test -f docs/HANDOFF_V1.4_HUMAN_ACCEPTANCE.md
    grep -q 'project(nightfall_punk VERSION 1.4' CMakeLists.txt
    grep -q 'NF_REPORT_POSITION_QUANTUM' src/shared/nf_report.h
    grep -q 'NF_CLAIM_INTERACTION' src/shared/nf_claim.h
    grep -q 'relational_comms_claims_v14' CMakeLists.txt
    grep -q 'F4 PROOF SETUP' src/demo/v14_greybox.c
    grep -q 'v1.3 is intentionally unused' docs/LEDGER_V1.4_BUILD_READINESS.md
    echo "[ok] v1.4 relational combat support / bounded communications source tree present"
}

v14_smoke() {
    if [[ ! -x build/headless/nightfall_v14_relations_test ]]; then
        ./nightfall.sh build-headless
    fi
    ./build/headless/nightfall_v14_relations_test | tee build/v14-relations-smoke.log
    grep -q 'bounded communications / claims / revisions tests: PASS' build/v14-relations-smoke.log
    grep -q 'REPORT DELAY=PASS PROVENANCE=PASS COARSE_POSITION=PASS SCOPE=PASS' build/v14-relations-smoke.log
    grep -q 'CLAIM SOFT_SHARE=PASS HARD_BLOCK=PASS EXPIRY=PASS' build/v14-relations-smoke.log
    grep -q 'ROUTE STABLE_ID=PASS REVISION=PASS CREW_REPORT=PASS RIVAL_REPORT=PASS' build/v14-relations-smoke.log
    echo "[ok] v1.4 report + claim + revision causal proof"
}

demo() {
    if [[ ! -x build/full/nightfall_v14_demo ]]; then
        ./nightfall.sh build
    fi
    exec ./build/full/nightfall_v14_demo
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
        bash ./v12c.sh camera-smoke
        bash ./v12c.sh integration-smoke
        v14_smoke
        ;;
    v14-smoke)
        v14_smoke
        ;;
    integration-smoke)
        bash ./v12c.sh integration-smoke
        ;;
    camera-smoke)
        bash ./v12c.sh camera-smoke
        ;;
    regression)
        bash ./v12c.sh regression
        ;;
    demo)
        demo
        ;;
    help|--help|-h)
        cat <<'EOF'
nightfall!punk v1.4 relational combat support / bounded communications wrapper

  ./v14.sh standard-check    inherited v1.2c checks + v1.4 report/claim/revision contracts
  ./v14.sh build             strict graphical Linux compile
  ./v14.sh build-headless    strict headless compile
  ./v14.sh test              all CTests + camera + population + v1.4 causal proofs
  ./v14.sh v14-smoke         bounded communication / claim / object-revision proof
  ./v14.sh integration-smoke inherited v1.2c population/embodied-action proof
  ./v14.sh camera-smoke      inherited relational-camera proof
  ./v14.sh regression        inherited network/combat/ecology/energy/Lifeworld smokes
  ./v14.sh demo              run v1.4 bounded-communications human proof

Human demo controls:
  F1 PLAYER VIEW | F2 OBSERVER/LIVE | F3 DEBUG TOP/LIVE
  F4 PROOF SETUP | F5 full reset
  Mouse look | WASD | Shift sprint | Ctrl crouch | Space jump | hold E at purple gate

Recommended human proof:
  F4 -> confirm GATE rev1 CLOSED + TEAM EVIDENCE NONE -> hold E -> INTERACTION CLAIM GRANTED
  -> GATE rev2 OPEN -> CREW REPORT PENDING -> LIVE -> TEAM EVIDENCE CREW_REPORT
  -> BELIEF OPEN -> gradient/utility/commitment change -> embodied teammate trajectory change.

Player-visible weapons/viewmodel and full reciprocal playable conflict remain the intended v1.5 boundary.
EOF
        ;;
    *)
        echo "unknown v1.4 command: $cmd" >&2
        exit 2
        ;;
esac
