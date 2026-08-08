#!/usr/bin/env bash
set -euo pipefail
if git rev-parse --show-toplevel >/dev/null 2>&1; then
  SOURCE_DIR="$(git rev-parse --show-toplevel)"
  REMOTE_URL="$(git -C "$SOURCE_DIR" remote get-url origin)"
  PARENT_DIR="$(dirname "$SOURCE_DIR")"
else
  SOURCE_DIR="$(pwd)"
  REMOTE_URL="https://github.com/loydwilliams2015-droid/nightfall_punk.git"
  PARENT_DIR="$HOME/Projects"
fi
BASE_NAME="nightfall_punk_v0.4_clean"
TARGET_DIR="$PARENT_DIR/$BASE_NAME"
if [[ -e "$TARGET_DIR" ]]; then TARGET_DIR="${TARGET_DIR}_$(date +%Y%m%d_%H%M%S)"; fi
echo "[nightfall] preserving current checkout: $SOURCE_DIR"
echo "[nightfall] cloning current main into: $TARGET_DIR"
git clone --branch main --single-branch "$REMOTE_URL" "$TARGET_DIR"
cd "$TARGET_DIR"
echo "[nightfall] clean checkout commit: $(git rev-parse --short HEAD)"
grep -q "v0.4 combat lab" src/client/main.c
chmod +x nightfall.sh
./nightfall.sh standard-check
./nightfall.sh build
./nightfall.sh test
./nightfall.sh combat-smoke
echo
echo "[nightfall] clean v0.4 checkout is ready"
echo "  cd '$TARGET_DIR'"
echo "  ./nightfall.sh local"
