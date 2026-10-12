#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer src/shared/nf_encounter18b2.c src/tests/test_encounter18b2.c -Isrc/shared -o /tmp/nightfall_v18b2_encounter_test
/tmp/nightfall_v18b2_encounter_test
