$ErrorActionPreference = "Stop"
Write-Host "`n--- Running Raw Google Tests (Windows) ---"

$scriptPath = Split-Path -Parent $PSCommandPath
$projectRoot = (Resolve-Path (Join-Path $scriptPath "../../..")).Path
$buildDir = Join-Path $projectRoot "build"

# Ensure MSYS2/MinGW is in the path for the DLLs
$env:Path = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;" + $env:Path

# Execute the test binary securely using its absolute path
& (Join-Path $buildDir "tests\unit_tests.exe") --gtest_shuffle