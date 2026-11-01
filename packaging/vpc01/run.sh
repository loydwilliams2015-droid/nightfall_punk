#!/usr/bin/env bash
set -euo pipefail
root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$root"
if [[ ! -x bin/nightfall_vpc01_two_actor ]]; then
  printf 'Missing VPC-01 executable in %s/bin\n' "$root" >&2
  exit 72
fi
if [[ -z "${DISPLAY:-}" && -z "${WAYLAND_DISPLAY:-}" ]]; then
  echo 'No graphical display session detected. Run from a Linux desktop session.' >&2
  exit 73
fi
echo 'VPC-01 engineering research viewer (NOT 1.8C.2 authorized multiplayer playtest)'
echo 'WASD = actor A | Tab = toggle actor B controls | arrows = actor B | R = reset | Esc = exit'
exec "$root/bin/nightfall_vpc01_two_actor" "$@"
