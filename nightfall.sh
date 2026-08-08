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
  "$HEADLESS_BUILD_DIR/nightfall_server" --duration 9 >"$BUILD_ROOT/combat-smoke-server.log" 2>&1 &
  server_pid=$!
  trap 'kill "$server_pid" 2>/dev/null || true' EXIT INT TERM
  sleep 0.35
  pids=()
  for pattern in 0 1 2 3; do
    "$HEADLESS_BUILD_DIR/nightfall_netbot" --duration 6 --pattern "$pattern" &
    pids+=("$!")
  done
  result=0
  for pid in "${pids[@]}"; do wait "$pid" || result=1; done
  wait "$server_pid" || true
  return "$result"
}

case "$cmd" in
  standard-check)
    command -v cmake >/dev/null
    command -v cc >/dev/null
    command -v git >/dev/null
    test -f "$ROOT_DIR/src/shared/nf_net.c"
    test -f "$ROOT_DIR/src/shared/nf_protocol.c"
    test -f "$ROOT_DIR/src/shared/nf_prediction.c"
    test -f "$ROOT_DIR/src/shared/nf_combat.c"
    echo "[ok] cmake: $(cmake --version | head -n1)"
    echo "[ok] cc: $(cc --version | head -n1)"
    if command -v pkg-config >/dev/null && pkg-config --exists libsodium 2>/dev/null; then
      echo "[ok] libsodium: $(pkg-config --modversion libsodium)"
    else
      echo "[warn] libsodium-dev not detected; localhost security scaffold will be used"
    fi
    echo "[ok] v0.4 authoritative combat source tree present"
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
    "$FULL_BUILD_DIR/nightfall_server" >"$BUILD_ROOT/server.log" 2>&1 &
    server_pid=$!
    trap 'kill "$server_pid" 2>/dev/null || true' EXIT INT TERM
    sleep 0.35
    "$FULL_BUILD_DIR/nightfall_client" "$@"
    ;;
  net-smoke|combat-smoke)
    run_smoke
    ;;
  clean)
    rm -rf "$BUILD_ROOT"
    ;;
  *)
    cat <<'HELP'
nightfall!punk v0.4 build helper

  ./nightfall.sh standard-check
  ./nightfall.sh build
  ./nightfall.sh build-headless
  ./nightfall.sh test
  ./nightfall.sh local
  ./nightfall.sh combat-smoke
  ./nightfall.sh net-smoke
  ./nightfall.sh server [--friendly-fire --sim-latency MS --sim-jitter MS --sim-loss PERCENT]
  ./nightfall.sh client [--host HOST --sim-latency MS --sim-jitter MS --sim-loss PERCENT]
  ./nightfall.sh clean

local         = dedicated v0.4 server + graphical combat client over real ENet localhost
combat-smoke  = dedicated server + four automated movement/combat clients
net-smoke     = alias retained for continuity
HELP
    ;;
esac
