#!/usr/bin/env bash
set -euo pipefail

SCRIPT_PATH="$(cd "$(dirname -- "$0")" && pwd -P)"
PROJECT_ROOT="$(cd "$SCRIPT_PATH/../../.." && pwd -P)"

echo " ----- Installing Lit and GCC ----- "
sudo apt-get update
sudo apt-get install -y gcc python3-pip
python3 -m pip install --user lit

echo " ----- Setting up FileCheck ----- "
chmod +x $PROJECT_ROOT/third_party/llvm/FileCheck

