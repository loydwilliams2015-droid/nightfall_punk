#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT_DIR/build"

cmd="${1:-help}"

case "$cmd" in
  standard-check)
    command -v cmake >/dev/null
    command -v cc >/dev/null
    echo "[ok] cmake: $(cmake --version | head -n1)"
    echo "[ok] cc: $(cc --version | head -n1)"
    echo "[ok] source tree present"
    ;;
  build)
    cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug
    cmake --build "$BUILD_DIR" -j
    ;;
  build-headless)
    cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DNF_BUILD_CLIENT=OFF
    cmake --build "$BUILD_DIR" -j
    ;;
  test)
    cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DNF_BUILD_CLIENT=OFF
    cmake --build "$BUILD_DIR" -j
    ctest --test-dir "$BUILD_DIR" --output-on-failure
    ;;
  server)
    exec "$BUILD_DIR/nightfall_server"
    ;;
  client)
    exec "$BUILD_DIR/nightfall_client"
    ;;
  clean)
    rm -rf "$BUILD_DIR"
    ;;
  *)
    cat <<'EOF'
nightfall!punk v0.1 build helper

  ./nightfall.sh standard-check
  ./nightfall.sh build
  ./nightfall.sh build-headless
  ./nightfall.sh test
  ./nightfall.sh server
  ./nightfall.sh client
  ./nightfall.sh clean
EOF
    ;;
esac
