cmake_minimum_required(VERSION 3.14)
project(sw_rosseta
        VERSION 0.1.0
        LANGUAGES CXX C ASM
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

set(LIEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/LIEF")
find_package(LIEF REQUIRED CONFIG PATHS "${LIEF_ROOT}/lib/cmake/LIEF" NO_DEFAULT_PATH)

add_subdirectory(dev)
add_subdirectory(tests)
