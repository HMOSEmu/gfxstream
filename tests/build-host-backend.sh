#!/usr/bin/env bash
# Native host build; produces a fresh review artifact and never installs it.
set -euo pipefail
ROOT=$(cd -- "$(dirname -- "$0")/.." && pwd)
BUILD=${BUILD:-$ROOT/host-build}
JOBS=${JOBS:-4}
cmake -S "$ROOT" -B "$BUILD" -G Ninja \
    -DDEPENDENCY_RESOLUTION=SYSTEM -DBUILD_STANDALONE=ON \
    -DCMAKE_BUILD_TYPE=Release '-DCMAKE_C_FLAGS_RELEASE=-O2 -DNDEBUG' \
    '-DCMAKE_CXX_FLAGS_RELEASE=-O2 -DNDEBUG' \
    -DENABLE_VKCEREAL_TESTS=OFF -DASTC_CPU_DECODING=OFF \
    -DGFXSTREAM_ENABLE_HOST_TRACING=OFF -DUSE_ANGLE_SHADER_PARSER=OFF \
    -DWITH_BENCHMARK=OFF -DVIRGL_RENDERER_UNSTABLE_APIS=ON
"$ROOT/tests/host-renderpass2/run.sh"
"$ROOT/tests/host-dynamic-state/run.sh"
ninja -C "$BUILD" -j"$JOBS" gfxstream_backend
ARTIFACT=$(mktemp -d "$BUILD/verified-backend.XXXXXX")
cp -p "$BUILD/libgfxstream_backend.so" "$ARTIFACT/"
(cd "$ARTIFACT" && sha256sum libgfxstream_backend.so > SHA256SUMS)
readelf -n "$ARTIFACT/libgfxstream_backend.so" > "$ARTIFACT/build-id.txt"
printf 'New host library (not deployed): %s/libgfxstream_backend.so\n' "$ARTIFACT"
