#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
bash build.sh
bin/nightfall-cinder --selftest
for scenario in crate ladder pending door; do
  bin/nightfall-slipgate --scenario "$scenario"
done
python3 scripts/bench.py --seeds 128 --start 31001 --steps 300
if [[ -f engine/CMakeLists.txt ]]; then
  cmake --build engine/build/dual --parallel "${JOBS:-4}" >/dev/null
  ctest --test-dir engine/build/dual --output-on-failure
fi
