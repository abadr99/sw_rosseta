# SW Rosetta
- This tool is used to convert from x86 binary code to RV binary code

## Prerequisites
Before you begin, ensure you have the following installed:
* **CMake** (v3.14 or higher)
* **C++17** compatible compiler (GCC, Clang, or MSVC)
* **Python 3** (required for the linting script)

### Build the Project
```bash
# Step 1: Generate the build system files
cmake -B build

# Step 2: Compile the code
cmake --build build
```
### Run the Application
```
./build/dev/rosetta
```
### Run the Tests
```bash
# Unit testing
./build/tests/rosetta_unit_test
# or using ctest
ctest --test-dir build --output-on-failure
```
