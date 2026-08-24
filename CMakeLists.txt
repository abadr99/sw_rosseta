cmake_minimum_required(VERSION 3.14)
project(sw_rosseta
    VERSION 0.1.0
    LANGUAGES CXX C ASM
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    zydis
    GIT_REPOSITORY https://github.com/zyantific/zydis.git
    GIT_TAG        v4.1.0
)

FetchContent_MakeAvailable(zydis)

enable_testing()

add_subdirectory(dev)
add_subdirectory(tests)

# ==========================================
# Integration Testing (LLVM lit)
# ==========================================
find_program(LIT_TOOL NAMES lit)

if(LIT_TOOL)
    add_custom_target(check-lit
        # Run lit against the tests/lit directory in verbose mode (-v)
        COMMAND ${LIT_TOOL} -v ${CMAKE_SOURCE_DIR}/scripts/Lit_Script
        # Force it to run from the build directory so relative paths work
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running LLVM lit regression tests..."
    )
else()
    message(WARNING "lit tool not found. 'make check-lit' will not be available.")
endif()