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
  test -x "$HEADLESS_BUILD_DIR/nightfall_netbot" || "$0" build-headless
  "$HEADLESS_BUILD_DIR/nightfall_server" --duration 13 --ai-count 0 --pressure-slots 0 --cattler-count 3 --cattler-profile pack >"$BUILD_ROOT/cattler-smoke-server.log" 2>&1 &
  server_pid=$!
  trap 'kill "$server_pid" 2>/dev/null || true' EXIT INT TERM
  sleep 0.35
  result=0
  "$HEADLESS_BUILD_DIR/nightfall_netbot" --duration 9 --pattern 1 --passive || result=1
  wait "$server_pid" || true
  cat "$BUILD_ROOT/cattler-smoke-server.log" || true
  grep -q "\[cattler\] habitat continuity" "$BUILD_ROOT/cattler-smoke-server.log" || result=1
  grep -q "social=PACK" "$BUILD_ROOT/cattler-smoke-server.log" || result=1
  grep -q "infest=" "$BUILD_ROOT/cattler-smoke-server.log" || result=1
  grep -q "\[ecology\]" "$BUILD_ROOT/cattler-smoke-server.log" || result=1
  return "$result"
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
    test -f "$ROOT_DIR/src/shared/nf_relations.c"
    test -f "$ROOT_DIR/src/shared/nf_semantics.c"
    test -f "$ROOT_DIR/src/shared/nf_region.c"
    test -f "$ROOT_DIR/src/server/ai/nf_ai.c"
    test -f "$ROOT_DIR/src/server/ai/nf_encounter.c"
    test -f "$ROOT_DIR/src/server/ai/nf_spatial.c"
    test -f "$ROOT_DIR/src/server/ai/nf_spatial_filter.c"
    test -f "$ROOT_DIR/src/server/ai/nf_cattler.c"
    echo "[ok] cmake: $(cmake --version | head -n1)"
    echo "[ok] cc: $(cc --version | head -n1)"
    echo "[ok] nightfall.sh syntax"
    if command -v pkg-config >/dev/null && pkg-config --exists libsodium 2>/dev/null; then
      echo "[ok] libsodium: $(pkg-config --modversion libsodium)"
    else
      echo "[warn] libsodium-dev not detected; localhost security scaffold will be used"
    fi
    echo "[ok] v0.8 Dream Cattler habitat ecology source tree present"
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
      echo "nightfall: local v0.8 server failed to start on five isolated ports" >&2
      echo "nightfall: server log follows" >&2
      cat "$BUILD_ROOT/server.log" >&2 || true
      exit 1
    fi
    trap 'kill "$server_pid" 2>/dev/null || true; wait "$server_pid" 2>/dev/null || true' EXIT INT TERM
    sleep 0.20
    if ! kill -0 "$server_pid" 2>/dev/null; then
      echo "nightfall: local v0.8 server exited before client launch" >&2
      cat "$BUILD_ROOT/server.log" >&2 || true
      exit 1
    fi
    echo "[local] v0.8 dedicated server pid=$server_pid isolated_port=$local_port rivals=$ai_count pressure_slots=$pressure_slots cattlers=$cattler_count profile=$cattler_profile truce=${NF_RIVAL_TRUCE:-0}"
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
  clean)
    rm -rf "$BUILD_ROOT"
    ;;
  *)
    cat <<'HELP'
nightfall!punk v0.8 build helper

  ./nightfall.sh standard-check
  ./nightfall.sh build
  ./nightfall.sh build-headless
  ./nightfall.sh test
  ./nightfall.sh local
  ./nightfall.sh combat-smoke
  ./nightfall.sh encounter-smoke
  ./nightfall.sh spatial-smoke
  ./nightfall.sh cattler-smoke
  ./nightfall.sh server [--ai-count 0..4 --pressure-slots 0..2 --cattler-count 0..5 --cattler-profile normal|pack|loner|horde --rival-truce --friendly-fire --sim-latency MS --sim-jitter MS --sim-loss PERCENT]
  ./nightfall.sh client [--host HOST --sim-latency MS --sim-jitter MS --sim-loss PERCENT]
  ./nightfall.sh clean

local            = isolated-port v0.8 server + graphical client; 4 Human Rivals + 3 Dream Cattlers by default
combat-smoke     = v0.4-v0.7 combat/network regression proof with Cattlers disabled
encounter-smoke  = passive player versus v0.6 bounded-pressure Human Rivals with Cattlers disabled
spatial-smoke    = v0.7 situated Rival roaming proof with Cattlers disabled
cattler-smoke    = 3-Cattler pack ecology proof: habitat/pack/infestation/ledger diagnostics
net-smoke        = alias retained for continuity

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
