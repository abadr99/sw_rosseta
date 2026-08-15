# build_windows.ps1

Write-Host "--- Configuring UCRT64 (MinGW) Environment ---"
# This perfectly points to the MSYS2 directory both locally and on the GitHub runner
$env:Path = "C:\msys64\ucrt64\bin;" + $env:Path

Write-Host "`n--- Cleaning previous build cache ---"
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue

Write-Host "`n--- Generating Ninja Build System ---"
cmake -S . -B build -G Ninja

Write-Host "`n--- Compiling Project ---"
cmake --build build

Write-Host "`n--- Running CTest ---"
ctest --test-dir build --output-on-failure