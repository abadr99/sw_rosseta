#!/usr/bin/env bash
set -euo pipefail

echo "=== Locating Rosetta Binary (Linux) ==="

ROSETTA_BIN=$(find build -type f -name "rosetta" | head -n 1)

if [ -z "$ROSETTA_BIN" ]; then
  echo "ERROR: rosetta binary not found under build/"
  find build -type f | sort
  exit 1
fi

echo "Found rosetta at: $ROSETTA_BIN"
mkdir -p build/dev

TARGET="build/dev/rosetta"
if [ "$ROSETTA_BIN" != "$TARGET" ]; then
  cp "$ROSETTA_BIN" "$TARGET"
fi

chmod +x "$TARGET"
echo "Rosetta setup complete at $TARGET"
