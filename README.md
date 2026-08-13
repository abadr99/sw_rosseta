# SW Rosetta

## Project Structure

```text
.
├── build/                 # Compiled binaries and build output
├── dev/                   # Main application code
│   ├── inc/               # C++ Header files (.hpp, .h)
│   ├── src/               # C++ Source files (.cpp)
│   ├── CMakeLists.txt     # CMake configuration for the dev module
│   └── main.cpp           # Main entry point for the application
├── scripts/               # Utility scripts
├── tests/                 # Unit & Regrestion tests
├── CMakeLists.txt         # Root CMake configuration
├── CPPLINT.cfg            # Google C++ linter configuration rules
└── README.md              # Project documentation
```

## Commands
### Building
```bash
cmake -B build
cmake --build build

# Running app
./build/dev/rosetta 
```
### Testing
```bash
# Lint testing
python scripts/lint.py -d dev

# Unit testing
./build/tests/rosetta_unit_test
# or
ctest --test-dir build --output-on-failure
```
