Write-Host "`n--- Running Raw Google Tests (Windows) ---"

$env:Path = "C:\msys64\ucrt64\bin;" + $env:Path

.\build\tests\unit_tests.exe
