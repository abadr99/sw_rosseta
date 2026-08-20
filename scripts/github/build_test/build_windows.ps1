$ErrorActionPreference = "Stop"
Write-Host "`n--- Compiling Project ---"

$scriptPath = Split-Path -Parent $PSCommandPath
$projectRoot = (Resolve-Path (Join-Path $scriptPath "../../..")).Path
$buildDir = Join-Path $projectRoot "build"

$env:Path = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;" + $env:Path

make -C $buildDir