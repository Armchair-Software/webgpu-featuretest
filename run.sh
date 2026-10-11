#!/bin/bash

# Based on webgpu-demo3/run.sh; emrun supplies COOP/COEP headers locally.
cd "$(dirname "$0")" || exit 1
emscripten_cache="${EM_CACHE:-}"
source /usr/src/emscripten/emsdk/emsdk_env.sh || exit 1
if [ -n "$emscripten_cache" ]; then
  export EM_CACHE="$emscripten_cache"
fi
case "${CMAKE_BUILD_TYPE,,}" in
  ""|debug) build_dir="build" ;;
  release) build_dir="build_rel" ;;
  *) echo "Use CMAKE_BUILD_TYPE=Debug or Release"; exit 1 ;;
esac
if [ ! -f "$build_dir/basic_wasm32.wasm" ] || [ ! -f "$build_dir/basic_wasm64.wasm" ] || [ ! -f "$build_dir/index.html" ]; then
  ./build.sh || exit 1
fi
port="${PORT:-6948}"
if [ "${1:-}" = "--serve" ]; then
  exec emrun --no_browser --port "$port" --serve_root "$build_dir" "$build_dir/index.html"
fi
browser="${BROWSER:-/usr/lib/chromium/chromium}"
if [ ! -x "$browser" ]; then
  browser=$(command -v chromium) || exit 1
fi
browser_args="--new-window --enable-unsafe-webgpu --enable-features=Vulkan --disable-background-networking --disable-component-update --disable-push-api-background-mode --no-sandbox --no-zygote"
if [ -n "$WAYLAND_DISPLAY" ]; then
  xwayland_display=$(ps -o args= -C Xwayland | awk 'NR == 1 {print $2}')
  if [ -n "$xwayland_display" ]; then
    export DISPLAY="$xwayland_display"
    browser_args="$browser_args --ozone-platform=x11"
  else
    echo "Warning: no XWayland display found; using the current display"
  fi
fi
exec emrun --port "$port" --serve_root "$build_dir" --browser "$browser" --browser_args="$browser_args ${EXTRA_BROWSER_ARGS:-}" "$build_dir/index.html"
