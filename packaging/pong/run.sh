#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
set -euo pipefail
root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$root"
if [[ ! -x bin/nightfall_vpc01_3d_pong ]]; then
  echo 'Missing game executable in bin/' >&2
  exit 72
fi
if [[ -z "${DISPLAY:-}" && -z "${WAYLAND_DISPLAY:-}" ]]; then
  echo 'Graphical desktop session required' >&2
  exit 73
fi
echo 'nightfall!punk 1.8C.x VPC-01 Pong research demo — NOT the final authoritative engine'
echo 'W/S = left paddle | right paddle = AI | TAB = manual right paddle with arrows | R = restart | ESC = quit'
exec "$root/bin/nightfall_vpc01_3d_pong" "$@"
