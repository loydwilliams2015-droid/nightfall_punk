#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_ROOT="$ROOT_DIR/build"
FULL_BUILD_DIR="$BUILD_ROOT/full"
HEADLESS_BUILD_DIR="$BUILD_ROOT/headless"
cmd="${1:-help}"
if [[ $# -gt 0 ]]; then shift; fi

run_smoke() {
  test -x "$HEADLESS_BUILD_DIR/nightfall_netbot" || "$0" build-headless
  "$HEADLESS_BUILD_DIR/nightfall_server" --duration 11 --ai-count 4 --pressure-slots 2 --cattler-count 0 >"$BUILD_ROOT/combat-smoke-server.log" 2>&1 &
  server_pid=$!
  trap 'kill "$server_pid" 2>/dev/null || true' EXIT INT TERM
  sleep 0.35
  pids=()
  for pattern in 0 1 2 3; do
    "$HEADLESS_BUILD_DIR/nightfall_netbot" --duration 7 --pattern "$pattern" &
    pids+=("$!")
  done
  result=0
  for pid in "${pids[@]}"; do wait "$pid" || result=1; done
  wait "$server_pid" || true
  return "$result"
}

run_ai_smoke() {
  test -x "$HEADLESS_BUILD_DIR/nightfall_netbot" || "$0" build-headless
  "$HEADLESS_BUILD_DIR/nightfall_server" --duration 15 --ai-count 4 --pressure-slots 2 --cattler-count 0 >"$BUILD_ROOT/ai-smoke-server.log" 2>&1 &
  server_pid=$!
  trap 'kill "$server_pid" 2>/dev/null || true' EXIT INT TERM
  sleep 0.35
  result=0
  "$HEADLESS_BUILD_DIR/nightfall_netbot" --duration 12 --pattern 0 --passive --require-incoming-death || result=1
  wait "$server_pid" || true
  cat "$BUILD_ROOT/ai-smoke-server.log" || true
  return "$result"
}

run_spatial_smoke() {
  test -x "$HEADLESS_BUILD_DIR/nightfall_server" || "$0" build-headless
  "$HEADLESS_BUILD_DIR/nightfall_server" --duration 8 --ai-count 4 --pressure-slots 2 --cattler-count 0 --rival-truce >"$BUILD_ROOT/spatial-smoke-server.log" 2>&1
  cat "$BUILD_ROOT/spatial-smoke-server.log" || true
  grep -q "\[spatial\] 0.40 km\^2 graybox" "$BUILD_ROOT/spatial-smoke-server.log"
  grep -q "task=ROAM" "$BUILD_ROOT/spatial-smoke-server.log"
  grep -q "fields\[c=" "$BUILD_ROOT/spatial-smoke-server.log"
}

run_cattler_smoke() {
  test -x "$HEADLESS_BUILD_DIR/nightfall_server" || "$0" build-headless
  "$HEADLESS_BUILD_DIR/nightfall_server" --duration 8 --ai-count 0 --pressure-slots 0 --cattler-count 3 --cattler-profile pack >"$BUILD_ROOT/cattler-smoke-server.log" 2>&1
  cat "$BUILD_ROOT/cattler-smoke-server.log" || true
  grep -q "\[cattler\] habitat return is ecological" "$BUILD_ROOT/cattler-smoke-server.log"
  grep -q "social=PACK" "$BUILD_ROOT/cattler-smoke-server.log"
  grep -q "mode=INFEST" "$BUILD_ROOT/cattler-smoke-server.log"
  grep -q "infest=" "$BUILD_ROOT/cattler-smoke-server.log"
  grep -q "\[ecology\] cattler living=3/3" "$BUILD_ROOT/cattler-smoke-server.log"
  grep -Eq "expansion=[1-9]" "$BUILD_ROOT/cattler-smoke-server.log"
}

run_energy_smoke() {
  test -x "$HEADLESS_BUILD_DIR/nightfall_energy_test" || "$0" build-headless
  mkdir -p "$BUILD_ROOT"
  "$HEADLESS_BUILD_DIR/nightfall_energy_test" | tee "$BUILD_ROOT/energy-smoke.log"
  grep -q "topographical energy accounting tests: PASS" "$BUILD_ROOT/energy-smoke.log"
  grep -q "\[energy\] South Works" "$BUILD_ROOT/energy-smoke.log"
  grep -q "\[belief\]" "$BUILD_ROOT/energy-smoke.log"
  grep -q "\[helicopter\] rank=1" "$BUILD_ROOT/energy-smoke.log"
}

run_lifeworld_smoke() {
  test -x "$HEADLESS_BUILD_DIR/nightfall_lifeworld_test" || "$0" build-headless
  test -x "$HEADLESS_BUILD_DIR/nightfall_lifeworld_runtime_test" || "$0" build-headless
  mkdir -p "$BUILD_ROOT"
  {
    "$HEADLESS_BUILD_DIR/nightfall_lifeworld_test"
    "$HEADLESS_BUILD_DIR/nightfall_lifeworld_runtime_test"
  } | tee "$BUILD_ROOT/lifeworld-smoke.log"
  grep -q "nightfall v1.0 topographic lifeworld tests: PASS" "$BUILD_ROOT/lifeworld-smoke.log"
  grep -q "nightfall v1.0 lifeworld runtime tests: PASS" "$BUILD_ROOT/lifeworld-smoke.log"
  grep -q "\[lifeworld\] contested infestation=" "$BUILD_ROOT/lifeworld-smoke.log"
  grep -q "\[primary\]" "$BUILD_ROOT/lifeworld-smoke.log"
  ! grep -q '"darkness"' "$BUILD_ROOT/lifeworld-smoke.log"
}

run_contamination_smoke() {
  test -x "$HEADLESS_BUILD_DIR/nightfall_contamination_test" || "$0" build-headless
  mkdir -p "$BUILD_ROOT"
  "$HEADLESS_BUILD_DIR/nightfall_contamination_test" | tee "$BUILD_ROOT/contamination-smoke.log"
  grep -q "nightfall v1.1 contamination / stasis / transport tests: PASS" "$BUILD_ROOT/contamination-smoke.log"
  grep -q "inventory=SITE_PERSIST/BASE_RESTOCK/ECO_ABSORB" "$BUILD_ROOT/contamination-smoke.log"
  grep -q "infestation=SEPARATE" "$BUILD_ROOT/contamination-smoke.log"
}

case "$cmd" in
  standard-check)
    command -v cmake >/dev/null
    command -v cc >/dev/null
    command -v git >/dev/null
    bash -n "$0"
    test -f "$ROOT_DIR/src/shared/nf_net.c"
    test -f "$ROOT_DIR/src/shared/nf_protocol.c"
    test -f "$ROOT_DIR/src/shared/nf_prediction.c"
    test -f "$ROOT_DIR/src/shared/nf_combat.c"
    test -f "$ROOT_DIR/src/shared/nf_contamination.c"
    test -f "$ROOT_DIR/src/shared/nf_contamination.h"
    test -f "$ROOT_DIR/src/shared/nf_relations.c"
    test -f "$ROOT_DIR/src/shared/nf_semantics.c"
    test -f "$ROOT_DIR/src/shared/nf_region.c"
    test -f "$ROOT_DIR/src/shared/nf_energy.c"
    test -f "$ROOT_DIR/src/shared/nf_energy_actor.c"
    test -f "$ROOT_DIR/src/shared/nf_hitbox.c"
    test -f "$ROOT_DIR/src/shared/nf_lifeworld.c"
    test -f "$ROOT_DIR/src/server/ai/nf_ai.c"
    test -f "$ROOT_DIR/src/server/ai/nf_encounter.c"
    test -f "$ROOT_DIR/src/server/ai/nf_spatial.c"
    test -f "$ROOT_DIR/src/server/ai/nf_spatial_filter.c"
    test -f "$ROOT_DIR/src/server/ai/nf_spatial_attention.c"
    test -f "$ROOT_DIR/src/server/ai/nf_cattler.c"
    test -f "$ROOT_DIR/src/server/ai/nf_cattler_recurrence.c"
    test -f "$ROOT_DIR/src/server/ai/nf_lifeworld_spatial.c"
    test -f "$ROOT_DIR/src/tests/test_lifeworld_runtime.c"
    test -f "$ROOT_DIR/src/tests/test_contamination.c"
    test -f "$ROOT_DIR/docs/COMPARE5_LEDGER.md"
    test -f "$ROOT_DIR/docs/LEDGER_V1.1.md"
    echo "[ok] cmake: $(cmake --version | head -n1)"
    echo "[ok] cc: $(cc --version | head -n1)"
    echo "[ok] nightfall.sh syntax"
    if command -v pkg-config >/dev/null && pkg-config --exists libsodium 2>/dev/null; then
      echo "[ok] libsodium: $(pkg-config --modversion libsodium)"
    else
      echo "[warn] libsodium-dev not detected; localhost security scaffold will be used"
    fi
    echo "[ok] v1.1 contamination / relational-physics source tree present"
    ;;
  build)
    cmake -S "$ROOT_DIR" -B "$FULL_BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DNF_BUILD_CLIENT=ON
    cmake --build "$FULL_BUILD_DIR" -j
    ;;
  build-headless)
    cmake -S "$ROOT_DIR" -B "$HEADLESS_BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DNF_BUILD_CLIENT=OFF
    cmake --build "$HEADLESS_BUILD_DIR" -j
    ;;
  test)
    cmake -S "$ROOT_DIR" -B "$HEADLESS_BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DNF_BUILD_CLIENT=OFF
    cmake --build "$HEADLESS_BUILD_DIR" -j
    ctest --test-dir "$HEADLESS_BUILD_DIR" --output-on-failure
    ;;
  server)
    if [[ -x "$FULL_BUILD_DIR/nightfall_server" ]]; then exec "$FULL_BUILD_DIR/nightfall_server" "$@"; fi
    exec "$HEADLESS_BUILD_DIR/nightfall_server" "$@"
    ;;
  client)
    exec "$FULL_BUILD_DIR/nightfall_client" "$@"
    ;;
  local)
    test -x "$FULL_BUILD_DIR/nightfall_server" || "$0" build
    mkdir -p "$BUILD_ROOT"
    server_pid=""
    local_port=""
    ai_count="${NF_AI_COUNT:-4}"
    pressure_slots="${NF_AI_PRESSURE_SLOTS:-2}"
    cattler_count="${NF_CATTLER_COUNT:-3}"
    cattler_profile="${NF_CATTLER_PROFILE:-normal}"
    server_extra=()
    if [[ "${NF_RIVAL_TRUCE:-0}" == "1" ]]; then server_extra+=(--rival-truce); fi
    for attempt in 1 2 3 4 5; do
      local_port=$((20000 + RANDOM % 20000))
      "$FULL_BUILD_DIR/nightfall_server" --port "$local_port" --ai-count "$ai_count" --pressure-slots "$pressure_slots" --cattler-count "$cattler_count" --cattler-profile "$cattler_profile" "${server_extra[@]}" >"$BUILD_ROOT/server.log" 2>&1 &
      candidate_pid=$!
      sleep 0.20
      if kill -0 "$candidate_pid" 2>/dev/null; then
        server_pid="$candidate_pid"
        break
      fi
      wait "$candidate_pid" 2>/dev/null || true
    done
    if [[ -z "$server_pid" ]]; then
      echo "nightfall: local v1.1 server failed to start on five isolated ports" >&2
      echo "nightfall: server log follows" >&2
      cat "$BUILD_ROOT/server.log" >&2 || true
      exit 1
    fi
    trap 'kill "$server_pid" 2>/dev/null || true; wait "$server_pid" 2>/dev/null || true' EXIT INT TERM
    sleep 0.20
    if ! kill -0 "$server_pid" 2>/dev/null; then
      echo "nightfall: local v1.1 server exited before client launch" >&2
      cat "$BUILD_ROOT/server.log" >&2 || true
      exit 1
    fi
    echo "[local] v1.1 dedicated server pid=$server_pid isolated_port=$local_port rivals=$ai_count pressure_slots=$pressure_slots cattlers=$cattler_count profile=$cattler_profile truce=${NF_RIVAL_TRUCE:-0}"
    "$FULL_BUILD_DIR/nightfall_client" "$@" --port "$local_port"
    ;;
  net-smoke|combat-smoke)
    run_smoke
    ;;
  ai-smoke|encounter-smoke)
    run_ai_smoke
    ;;
  spatial-smoke)
    run_spatial_smoke
    ;;
  cattler-smoke)
    run_cattler_smoke
    ;;
  energy-smoke)
    run_energy_smoke
    ;;
  lifeworld-smoke)
    run_lifeworld_smoke
    ;;
  contamination-smoke)
    run_contamination_smoke
    ;;
  clean)
    rm -rf "$BUILD_ROOT"
    ;;
  *)
    cat <<'HELP'
nightfall!punk v1.1 build helper

  ./nightfall.sh standard-check
  ./nightfall.sh build
  ./nightfall.sh build-headless
  ./nightfall.sh test
  ./nightfall.sh local
  ./nightfall.sh combat-smoke
  ./nightfall.sh encounter-smoke
  ./nightfall.sh spatial-smoke
  ./nightfall.sh cattler-smoke
  ./nightfall.sh energy-smoke
  ./nightfall.sh lifeworld-smoke
  ./nightfall.sh contamination-smoke
  ./nightfall.sh server [--ai-count 0..4 --pressure-slots 0..2 --cattler-count 0..5 --cattler-profile normal|pack|loner|horde --rival-truce --friendly-fire --sim-latency MS --sim-jitter MS --sim-loss PERCENT]
  ./nightfall.sh client [--host HOST --sim-latency MS --sim-jitter MS --sim-loss PERCENT]
  ./nightfall.sh clean

local                = isolated-port v1.1 server + graphical client; 4 Human Rivals + 3 Dream Cattlers by default
combat-smoke         = combat/network regression proof with Cattlers disabled
encounter-smoke      = passive player versus bounded-pressure Human Rivals with Cattlers disabled
spatial-smoke        = situated Rival roaming proof with Cattlers disabled
cattler-smoke        = server-only 3-Cattler pack habitat proof; validates ecological-return contract banner and live ecology
energy-smoke         = inherited v0.9 C20-C22 universal-ledger/topography/history proof
lifeworld-smoke      = inherited v1.0 primary-memory + recurrence/reseed + embodied-attention + recovery proof
contamination-smoke  = v1.1 damage=contamination + stasis + inventory fate + persistent trace + water transport proof
net-smoke            = alias retained for continuity

Local debug environment:
  NF_AI_COUNT=1 ./nightfall.sh local
  NF_AI_PRESSURE_SLOTS=1 ./nightfall.sh local
  NF_RIVAL_TRUCE=1 ./nightfall.sh local
  NF_AI_COUNT=0 NF_CATTLER_COUNT=3 NF_CATTLER_PROFILE=pack ./nightfall.sh local
  NF_AI_COUNT=0 NF_CATTLER_COUNT=1 NF_CATTLER_PROFILE=loner ./nightfall.sh local
  NF_AI_COUNT=0 NF_CATTLER_COUNT=5 NF_CATTLER_PROFILE=horde ./nightfall.sh local
HELP
    ;;
esac
