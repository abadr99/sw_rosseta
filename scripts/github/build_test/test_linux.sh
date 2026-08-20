#!/usr/bin/env bash
set -euo pipefail

echo "--- Running Raw Google Tests (Linux) ---"

SCRIPT_PATH="$(cd "$(dirname -- "$0")" && pwd -P)"
PROJECT_ROOT="$(cd "$SCRIPT_PATH/../../.." && pwd -P)"

"$PROJECT_ROOT/build-linux/tests/unit_tests" --gtest_shuffle
