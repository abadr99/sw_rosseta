$ErrorActionPreference = "Stop"
$scriptPath = Split-Path -Parent $PSCommandPath
$projectRoot = (Resolve-Path (Join-Path $scriptPath "../../..")).Path
$buildDir = Join-Path $projectRoot "build"

Write-Host "--- Generating Build System (MSVC, NMake Makefiles) ---"

# Single-config, same pattern as Linux: CMAKE_BUILD_TYPE + cmake --build.
# NMake is required (not GNU make / MinGW) so we can link the MSVC LIEF SDK.
cmake $projectRoot -B $buildDir -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
