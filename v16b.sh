#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"
cmd="${1:-help}"

standard_check(){
  ./nightfall.sh standard-check
  bash -n ./v16b.sh
  test -f src/shared/nf_weapon_solution.h
  test -f src/shared/nf_weapon_geometry.c
  test -f src/shared/nf_protocol.c
  test -f src/shared/nf_prediction.c
  test -f src/server/v16b_main.c
  test -f src/tools/v16b_netbot.c
  test -f src/tools/v16b_sample_db.c
  test -f tools/analyze_v16b_samples.py
  test -f src/tests/test_v16b_weapon_authority.c
  test -f src/demo/v16b_weapon_authority_mobility.c
  test -f docs/LEDGER_V1.6B_WEAPON_AUTHORITY_MOBILITY.md
  test -f docs/HANDOFF_V1.6B_WEAPON_AUTHORITY_MOBILITY.md
  grep -q 'project(nightfall_punk VERSION 1.6.2' CMakeLists.txt
  grep -q 'NF_PROTOCOL_VERSION 6u' src/shared/nf_protocol.h
  grep -q 'NF_NET_STALE_HELD_INPUT_MS 350u' src/shared/nf_protocol.h
  grep -q 'focus_held' src/shared/nf_world.h
  grep -q 'nightfall_v16b_server' CMakeLists.txt
  grep -q 'nightfall_v16b_demo' CMakeLists.txt
  grep -q 'weapon_authority_mobility_v16b' CMakeLists.txt
  echo "[ok] v1.6B Weapon Authority / Mobility source contract present"
}

v16b_smoke(){
  test -x build/headless/nightfall_v16b_weapon_authority_test || ./nightfall.sh build-headless
  ./build/headless/nightfall_v16b_weapon_authority_test | tee build/v16b-weapon-authority-smoke.log
  grep -q 'v1.6B Weapon Authority / Mobility contracts: PASS' build/v16b-weapon-authority-smoke.log
  grep -q 'packet_1186=PASS' build/v16b-weapon-authority-smoke.log
  echo "[ok] v1.6B deterministic authority/mobility proof"
}

sample_db(){
  test -x build/headless/nightfall_v16b_sample_db || ./nightfall.sh build-headless
  mkdir -p build/v16b-samples
  ./build/headless/nightfall_v16b_sample_db build/v16b-samples/samples.csv 360
  python3 tools/analyze_v16b_samples.py \
    build/v16b-samples/samples.csv \
    build/v16b-samples/report.md \
    build/v16b-samples/soil.csv
  echo "[ok] sample database: build/v16b-samples/samples.csv"
  echo "[ok] OITOA report: build/v16b-samples/report.md"
  echo "[ok] soil samples: build/v16b-samples/soil.csv"
}

net_smoke(){
  test -x build/headless/nightfall_v16b_server || ./nightfall.sh build-headless
  mkdir -p build/v16b-samples
  ./build/headless/nightfall_v16b_server --duration 12 --sim-latency 100 --sim-jitter 25 --sim-loss 0.02 --ai-count 2 --pressure-slots 1 --cattler-count 0 > build/v16b-samples/net-server.log 2>&1 &
  local server_pid=$!
  trap 'kill "$server_pid" 2>/dev/null || true' EXIT INT TERM
  sleep 0.5
  for scenario in focus_hold redirect_focus reload_focus sustained_hold; do
    ./build/headless/nightfall_v16b_netbot --duration 6 --scenario "$scenario" | tee "build/v16b-samples/net-${scenario}.csv"
  done
  wait "$server_pid" || true
  trap - EXIT INT TERM
  echo "[ok] v1.6B 100ms +/-25ms 2% loss scenario smoke"
}

demo(){
  test -x build/full/nightfall_v16b_demo || ./nightfall.sh build
  exec ./build/full/nightfall_v16b_demo
}

case "$cmd" in
  standard-check) standard_check ;;
  build) ./nightfall.sh build ;;
  build-headless) ./nightfall.sh build-headless ;;
  test)
    ./nightfall.sh test
    v16b_smoke
    ;;
  v16b-smoke) v16b_smoke ;;
  sample-db) sample_db ;;
  net-smoke) net_smoke ;;
  demo) demo ;;
  regression)
    ./nightfall.sh test
    sample_db
    net_smoke
    ;;
  help|--help|-h)
    cat <<'EOF'
nightfall!punk v1.6B — Weapon Authority / Mobility

  ./v16b.sh standard-check
  ./v16b.sh build
  ./v16b.sh build-headless
  ./v16b.sh test
  ./v16b.sh v16b-smoke
  ./v16b.sh sample-db
  ./v16b.sh net-smoke
  ./v16b.sh regression
  ./v16b.sh demo

Demo controls:
  WASD / Shift / Ctrl / Space
  Mouse look
  LMB fire | RMB Focus | F developer Focus control
  R reload | 1/2 weapon

Automation examples:
  ./build/full/nightfall_v16b_demo --auto focus_hold --seconds 5
  ./build/full/nightfall_v16b_demo --auto redirect_focus --seconds 5
  ./build/full/nightfall_v16b_demo --auto blocked --seconds 5

The sample database is deterministic scripted play. Native human video remains the acceptance layer for feel/readability/mastery.
EOF
    ;;
  *) echo "unknown v1.6B command: $cmd" >&2; exit 2 ;;
esac
