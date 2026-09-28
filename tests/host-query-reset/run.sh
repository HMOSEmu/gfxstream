#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd -- "$(dirname -- "$0")/../.." && pwd)
OUT=${OUT:-/tmp/gfxstream-host-query-reset-test}
"${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror -DTEST_FIXED \
 -I "$ROOT/third_party/vulkan/include" -I "$ROOT/host/vulkan" \
 "$ROOT/tests/host-query-reset/dispatch_test.cpp" -o "$OUT"
"$OUT"
