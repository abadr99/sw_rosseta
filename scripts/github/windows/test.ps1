$ErrorActionPreference = "Stop"
Write-Host "`n--- Running Raw Google Tests (Windows) ---"

$scriptPath = Split-Path -Parent $PSCommandPath
$projectRoot = (Resolve-Path (Join-Path $scriptPath "../../..")).Path
$buildDir = Join-Path $projectRoot "build"

# NMake is single-config: the binary sits next to the target, not under Release/.
& (Join-Path $buildDir "tests\unit_tests.exe") --gtest_shuffle
