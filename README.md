# SW Rosetta
- This tool is used to convert from x86 binary code to RV binary code

## Prerequisites
Before you begin, ensure you have the following installed:
* **CMake** (v3.15 or higher)
* **C++17** compatible compiler (GCC/Clang on Linux, MSVC on Windows)
* **Python 3** (required for the linting script)

LIEF is vendored as a prebuilt SDK under `third_party/lief/`. CMake unpacks it
into the build directory; you do not need to compile LIEF yourself.

### Build the Project
```bash
# Linux
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

# Windows (MSVC, NMake — same single-config flow as Linux)
# cmake -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
# cmake --build build --parallel
```
### Run the Application
```
./build/dev/rosetta
```
### Run the Tests
```bash
./build/tests/unit_tests
# or using ctest
ctest --test-dir build --output-on-failure
```
