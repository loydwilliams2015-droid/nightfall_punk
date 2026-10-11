#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
if [[ -f /workspace/nightfall-setup/env.sh ]]; then source /workspace/nightfall-setup/env.sh; fi
mkdir -p build/b2-evidence build/b1-pr41
clang_bin=$(command -v clang-19 || command -v clang)
for compiler in gcc "$clang_bin"; do
  name=$(basename "$compiler")
  "$compiler" -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 \
    src/reproduction/v18b1/nf_smart18b1.c src/reproduction/v18b1/test_smart18b1.c \
    -o "build/b1-pr41/$name-original"
  "build/b1-pr41/$name-original" | tee "build/b2-evidence/pr41-$name-original.log"
  "$compiler" -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 \
    src/reproduction/v18b1/nf_smart18b1.c src/tests/test_v18b1_pr41_coverage.c \
    -o "build/b1-pr41/$name-supplemental"
  "build/b1-pr41/$name-supplemental" | tee "build/b2-evidence/pr41-$name-supplemental.log"
done
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -fsanitize=address,undefined -fno-omit-frame-pointer \
  src/reproduction/v18b1/nf_smart18b1.c src/reproduction/v18b1/test_smart18b1.c \
  -o build/b1-pr41/asan-original
build/b1-pr41/asan-original | tee build/b2-evidence/pr41-asan-original.log
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -fsanitize=address,undefined -fno-omit-frame-pointer \
  src/reproduction/v18b1/nf_smart18b1.c src/tests/test_v18b1_pr41_coverage.c \
  -o build/b1-pr41/asan-supplemental
build/b1-pr41/asan-supplemental | tee build/b2-evidence/pr41-asan-supplemental.log
