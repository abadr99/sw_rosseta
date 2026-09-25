#!/usr/bin/env bash
set -euo pipefail

echo "--- Running Sanitizer Google Tests (Linux) ---"

SCRIPT_PATH="$(cd "$(dirname -- "$0")" && pwd -P)"
PROJECT_ROOT="$(cd "$SCRIPT_PATH/../../.." && pwd -P)"

export ASAN_OPTIONS=detect_leaks=1:halt_on_error=1
export UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1

"$PROJECT_ROOT/build-linux-asan/tests/unit_tests/unit_tests" --gtest_shuffle
