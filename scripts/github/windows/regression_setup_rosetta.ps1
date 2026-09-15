$ErrorActionPreference = "Stop"

Write-Host "=== Locating Rosetta Binary (Windows) ==="

$scriptPath = Split-Path -Parent $PSCommandPath
$projectRoot = (Resolve-Path (Join-Path $scriptPath "../../..")).Path
$buildDir = Join-Path $projectRoot "build"
$targetDir = Join-Path $buildDir "dev"
$targetExe = Join-Path $targetDir "rosetta.exe"

$rosettaBin = Get-ChildItem -Path $buildDir -Recurse -Filter "rosetta.exe" | Select-Object -ExpandProperty FullName -First 1

if (-not $rosettaBin -or -not (Test-Path $rosettaBin)) {
    Write-Error "ERROR: rosetta.exe was not found under $buildDir"
    Get-ChildItem -Path $buildDir -Recurse | Select-Object FullName
    exit 1
}

Write-Host "Found rosetta at: $rosettaBin"

if (-not (Test-Path $targetDir)) {
    New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
}

if ($rosettaBin -ne $targetExe) {
    Copy-Item -Path $rosettaBin -Destination $targetExe -Force
}

Write-Host "Rosetta setup complete at $targetExe"
