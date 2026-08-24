$ErrorActionPreference = "Stop"
$scriptPath = Split-Path -Parent $PSCommandPath
$projectRoot = (Resolve-Path (Join-Path $scriptPath "../../..")).Path
$buildDir = Join-Path $projectRoot "build"

Write-Host "--- Configuring UCRT64 (MinGW) Environment ---"

# 1. Update the path for the Current script
$env:Path = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;" + $env:Path

# 2. Update the path for Future steps (Safe in modern pwsh)
if ($env:GITHUB_PATH) {
    echo "C:\msys64\ucrt64\bin" >> $env:GITHUB_PATH
    echo "C:\msys64\usr\bin" >> $env:GITHUB_PATH
}

Write-Host "`n--- Cleaning previous build cache ---"
Remove-Item -Recurse -Force $buildDir -ErrorAction SilentlyContinue

Write-Host "`n--- Generating Build System ---"
cmake $projectRoot -B $buildDir -G "Unix Makefiles"