#!/usr/bin/env bash
set -euo pipefail

SCRIPT_PATH="$(cd "$(dirname -- "$0")" && pwd -P)"
PROJECT_ROOT="$(cd "$SCRIPT_PATH/../../.." && pwd -P)"

CCACHE_ARGS=()
if command -v ccache >/dev/null 2>&1; then
  CCACHE_ARGS+=("-DCMAKE_C_COMPILER_LAUNCHER=ccache"
                "-DCMAKE_CXX_COMPILER_LAUNCHER=ccache")
fi

cmake "$PROJECT_ROOT" -B "$PROJECT_ROOT/build-linux" \
  -DCMAKE_BUILD_TYPE=Release \
  "${CCACHE_ARGS[@]}"
