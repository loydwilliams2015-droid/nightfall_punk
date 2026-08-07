#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_ROOT="$ROOT_DIR/build"
FULL_BUILD_DIR="$BUILD_ROOT/full"
HEADLESS_BUILD_DIR="$BUILD_ROOT/headless"
cmd="${1:-help}"

case "$cmd" in
  standard-check)
    command -v cmake >/dev/null
    command -v cc >/dev/null
    test -f "$ROOT_DIR/CMakeLists.txt"
    test -f "$ROOT_DIR/src/shared/nf_world.c"
    test -f "$ROOT_DIR/src/shared/nf_movement.c"
    echo "[ok] cmake: $(cmake --version | head -n1)"
    echo "[ok] cc: $(cc --version | head -n1)"
    echo "[ok] v0.2 source tree present"
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
    if [[ -x "$FULL_BUILD_DIR/nightfall_server" ]]; then exec "$FULL_BUILD_DIR/nightfall_server"; fi
    exec "$HEADLESS_BUILD_DIR/nightfall_server"
    ;;
  client)
    exec "$FULL_BUILD_DIR/nightfall_client"
    ;;
  clean)
    rm -rf "$BUILD_ROOT"
    ;;
  *)
    cat <<'EOF'
nightfall!punk v0.2 build helper

  ./nightfall.sh standard-check
  ./nightfall.sh build
  ./nightfall.sh build-headless
  ./nightfall.sh test
  ./nightfall.sh server
  ./nightfall.sh client
  ./nightfall.sh clean

Headless and graphical builds use separate CMake directories so their
configuration options cannot contaminate one another.
EOF
    ;;
esac
