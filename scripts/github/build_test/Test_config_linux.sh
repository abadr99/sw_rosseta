#!/usr/bin/env bash
set -euo pipefail

SCRIPT_PATH="$(cd "$(dirname -- "$0")" && pwd -P)"
PROJECT_ROOT="$(cd "$SCRIPT_PATH/../../.." && pwd -P)"

chmod +x "$PROJECT_ROOT/build-linux/tests/unit_tests"