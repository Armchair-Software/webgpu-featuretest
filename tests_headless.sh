#!/bin/bash

# Adapted from webgpu-demo3/tests_headless.sh. The checklist reports its
# aggregate result through emrun's stdio protocol instead of a single C++ main.
cd "$(dirname "$0")" || exit 1
./build.sh || exit 1
source /usr/src/emscripten/emsdk/emsdk_env.sh || exit 1
build_dir="build"
if [ "${CMAKE_BUILD_TYPE,,}" = "release" ]; then
  build_dir="build_rel"
fi
browser="${BROWSER:-/usr/lib/chromium/chromium}"
if [ ! -x "$browser" ]; then
  browser=$(command -v chromium) || exit 1
fi
# EXTRA_BROWSER_ARGS can enable autoplay or select a software GPU for diagnostics.
# Defaults preserve browser policy; blocked tests do not count as passes.
browser_profile=$(mktemp -d /tmp/webgpu-featuretest-headless.XXXXXX) || exit 1
trap 'rm -rf "$browser_profile"' EXIT

emrun \
  --port "${PORT:-7948}" \
  --serve_root "$build_dir" \
  --kill_exit \
  --timeout 30 \
  --timeout-returncode 1 \
  --browser "$browser" \
  --browser_args="--headless --no-sandbox --no-zygote --disable-background-networking --user-data-dir=$browser_profile ${EXTRA_BROWSER_ARGS:-}" \
  "$build_dir/index.html#autoclose"
result=$?
echo "Browser smoke test exited with code $result"
exit "$result"
