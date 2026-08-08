#!/usr/bin/env bash
set -euo pipefail

# Resolve the source repository from the caller's current working directory,
# not from this script's own path. This keeps the helper safe when copied to /tmp.
if git -C "$PWD" rev-parse --show-toplevel >/dev/null 2>&1; then
  SOURCE_DIR="$(git -C "$PWD" rev-parse --show-toplevel)"
elif [[ -d "$HOME/Projects/nightfall_punk/.git" ]]; then
  SOURCE_DIR="$HOME/Projects/nightfall_punk"
else
  SOURCE_DIR=""
fi

if [[ -n "$SOURCE_DIR" ]]; then
  REMOTE_URL="$(git -C "$SOURCE_DIR" remote get-url origin 2>/dev/null || true)"
  PARENT_DIR="$(dirname "$SOURCE_DIR")"
else
  REMOTE_URL=""
  PARENT_DIR="$HOME/Projects"
fi

if [[ -z "$REMOTE_URL" ]]; then
  REMOTE_URL="https://github.com/loydwilliams2015-droid/nightfall_punk.git"
fi

mkdir -p "$PARENT_DIR"
BASE_NAME="nightfall_punk_v0.2_clean"
TARGET_DIR="$PARENT_DIR/$BASE_NAME"

if [[ -e "$TARGET_DIR" ]]; then
  TARGET_DIR="${TARGET_DIR}_$(date +%Y%m%d_%H%M%S)"
fi

if [[ -n "$SOURCE_DIR" ]]; then
  echo "[nightfall] preserving current checkout: $SOURCE_DIR"
else
  echo "[nightfall] no source checkout detected; using clean clone only"
fi

echo "[nightfall] cloning current main into: $TARGET_DIR"
git clone --branch main --single-branch "$REMOTE_URL" "$TARGET_DIR"

cd "$TARGET_DIR"

echo "[nightfall] clean checkout commit: $(git rev-parse --short HEAD)"
grep -q "v0.2 movement lab" src/client/main.c

echo "[nightfall] confirmed v0.2 movement client source"
chmod +x nightfall.sh
./nightfall.sh standard-check
./nightfall.sh build
./nightfall.sh test

echo
echo "[nightfall] clean v0.2 checkout is ready."
echo "[nightfall] launch with:"
echo "  cd '$TARGET_DIR'"
echo "  ./nightfall.sh client"
