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

add_subdirectory(dev)
add_subdirectory(tests)
