$ErrorActionPreference = "Stop"
Write-Host "`n--- Running Raw Google Tests (Windows) ---"

$scriptPath = Split-Path -Parent $PSCommandPath
$projectRoot = (Resolve-Path (Join-Path $scriptPath "../../..")).Path
$buildDir = Join-Path $projectRoot "build"

# Locate unit_tests.exe anywhere under build/
$testExe = Get-ChildItem -Path $buildDir -Recurse -Filter "unit_tests.exe" | Select-Object -ExpandProperty FullName -First 1

if (-not $testExe -or -not (Test-Path $testExe)) {
    Write-Error "Test binary not found under: $buildDir"
    Get-ChildItem -Path $buildDir -Recurse | Select-Object FullName
    exit 1
}

Write-Host "Found test binary at: $testExe"
& $testExe --gtest_shuffle
