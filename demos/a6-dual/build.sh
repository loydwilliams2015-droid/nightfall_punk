#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
if [[ -f "$ROOT/engine/CMakeLists.txt" ]]; then ENGINE="$ROOT/engine";
elif [[ -f "$ROOT/../../CMakeLists.txt" ]]; then ENGINE="$(cd "$ROOT/../.." && pwd)";
else echo 'Cannot find the nightfall!punk 1.8A6 engine source. Supply engine/ or place under demos/a6-dual/ in the GitHub repository.' >&2;exit 2;fi
cd "$ROOT"
cmake -S "$ENGINE" -B "$ENGINE/build/dual" -DNF_CONTACT_LAB_ONLY=ON -DCMAKE_BUILD_TYPE=Release > /dev/null
cmake --build "$ENGINE/build/dual" --target nightfall_embody_prod --parallel "${JOBS:-4}" > /dev/null
mkdir -p bin
cc="${CC:-cc}"
flags=(-std=c11 -O2 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -Werror -I"$ENGINE/src/shared" -I"$ENGINE/src/client")
if "${cc}" "${flags[@]}" -DNF_WITH_X11 src/arcade.c src/main.c "$ENGINE/build/dual/libnightfall_embody_prod.a" -lX11 -lm -o bin/nightfall-cinder 2>/dev/null; then
  cp bin/nightfall-cinder bin/nightfall-slipgate
  echo "X11 visual builds ready (both games; use --game cinder|slipgate)"
else
  "${cc}" "${flags[@]}" src/arcade.c src/main.c "$ENGINE/build/dual/libnightfall_embody_prod.a" -lm -o bin/nightfall-cinder
  cp bin/nightfall-cinder bin/nightfall-slipgate
  echo "Headless builds ready (--text/--batch; install libx11-dev for window mode)"
fi
