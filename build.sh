#!/bin/bash

# Based on webgpu-demo3/build.sh; build all independent feature programs.
exec 1>&2
cd "$(dirname "$0")" || exit 1
emscripten_cache="${EM_CACHE:-}"
source /usr/src/emscripten/emsdk/emsdk_env.sh || exit 1
if [ -n "$emscripten_cache" ]; then
  export EM_CACHE="$emscripten_cache"
fi

case "${CMAKE_BUILD_TYPE,,}" in
  ""|debug) build_type="Debug"; build_dir="build" ;;
  release) build_type="Release"; build_dir="build_rel" ;;
  *) echo "Use CMAKE_BUILD_TYPE=Debug or Release"; exit 1 ;;
esac

target="${1:-client}"
if [ "$target" = "clean" ]; then
  rm -rf "$build_dir" || exit 1
  target="${2:-client}"
fi
if command -v ccache >/dev/null 2>&1; then
  export EM_COMPILER_WRAPPER=ccache
fi
emcmake cmake -S . -B "$build_dir" -DCMAKE_BUILD_TYPE="$build_type" || exit 1
emmake cmake --build "$build_dir" --parallel "$(nproc)" --target "$target" || exit 1
echo "Built $build_dir/index.html"
