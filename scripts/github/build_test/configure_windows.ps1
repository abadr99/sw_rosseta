Write-Host "--- Configuring UCRT64 (MinGW) Environment ---"

# 1. Update the path for the Current script

$env:Path = "C:\msys64\ucrt64\bin;" + $env:Path

# 2. Update the path for Future steps

echo "C:\msys64\ucrt64\bin" >> $env:GITHUB_PATH

Write-Host "`n--- Cleaning previous build cache ---"

Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue

Write-Host "`n--- Generating Ninja Build System ---"

cmake . -B build -G Ninja