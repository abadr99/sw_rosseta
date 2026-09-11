$ErrorActionPreference = "Stop"
$scriptPath = Split-Path -Parent $PSCommandPath
$projectRoot = (Resolve-Path (Join-Path $scriptPath "../../..")).Path
$buildDir = Join-Path $projectRoot "build"

Write-Host "--- Generating Build System (MSVC, Ninja) ---"

# Ninja uses the active MSVC environment (cl.exe) and compiles on all cores.
# Passing compiler launchers allows ccache to cache object files between runs.
cmake $projectRoot -B $buildDir -G "Ninja" `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_C_COMPILER_LAUNCHER=ccache `
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache