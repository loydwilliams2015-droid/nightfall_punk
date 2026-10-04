#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
[[ -x "$ROOT/bin/nightfall-slipgate" ]] || bash "$ROOT/build.sh"
exec "$ROOT/bin/nightfall-slipgate" --game slipgate "$@"
