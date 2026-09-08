#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd -- "$(dirname -- "$0")/../.." && pwd)
OUT=${OUT:-/tmp/gfxstream-renderpass2-dispatch-test}
HEADERS=${HEADERS:-$ROOT/third_party/vulkan/include}
"${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror \
    -I "$HEADERS" -I "$ROOT/host/vulkan" \
    "$ROOT/tests/host-renderpass2/dispatch_test.cpp" -o "$OUT"
"$OUT"
