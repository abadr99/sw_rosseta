cmake_minimum_required(VERSION 3.14)
project(sw_rosseta VERSION 0.1.0 LANGUAGES CXX C ASM)

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

include(FetchContent)

set(LIEF_PYTHON_API OFF CACHE BOOL "" FORCE)
set(LIEF_C_API OFF CACHE BOOL "" FORCE)
set(LIEF_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LIEF_TESTS OFF CACHE BOOL "" FORCE)
set(LIEF_DOC OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        LIEF
        GIT_REPOSITORY https://github.com/lief-project/LIEF.git
        GIT_TAG        1.0.0
)
FetchContent_MakeAvailable(LIEF)

add_subdirectory(dev)
add_subdirectory(tests)
