#!/usr/bin/env bash
set -euo pipefail

cmake . -B build-linux \
  -DCMAKE_C_COMPILER_LAUNCHER=ccache \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
  