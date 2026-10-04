#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
[[ -x "$ROOT/bin/nightfall-cinder" ]] || bash "$ROOT/build.sh"
exec "$ROOT/bin/nightfall-cinder" --game cinder "$@"
