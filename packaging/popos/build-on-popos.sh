#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC="$ROOT/source"
if [[ ! -d "$SRC" ]]; then
  echo "source directory not present in this package" >&2
  exit 2
fi
sudo apt-get update
sudo apt-get install -y build-essential cmake git pkg-config \
  libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev \
  libglu1-mesa-dev libxcursor-dev libxinerama-dev
cmake -S "$SRC" -B "$SRC/build/popos-v17e" -DCMAKE_BUILD_TYPE=Release \
  -DNF_BUILD_CLIENT=ON -DNF_BUILD_SERVER=OFF -DNF_BUILD_NETBOT=OFF -DNF_BUILD_TESTS=ON
cmake --build "$SRC/build/popos-v17e" --target nightfall_v17e_observer --parallel
mkdir -p "$ROOT/bin"
cp "$SRC/build/popos-v17e/nightfall_v17e_observer" "$ROOT/bin/"
echo "built: $ROOT/bin/nightfall_v17e_observer"
