$ErrorActionPreference = "Stop"
Write-Host "`n--- Compiling Project ---"

$scriptPath = Split-Path -Parent $PSCommandPath
$projectRoot = (Resolve-Path (Join-Path $scriptPath "../../..")).Path
$buildDir = Join-Path $projectRoot "build"

cmake --build $buildDir --parallel
