#!/usr/bin/env bash
set -euo pipefail

cmake -S . -B build-linux \
  -DCMAKE_C_COMPILER_LAUNCHER=ccache \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
  