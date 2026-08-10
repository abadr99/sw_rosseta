cmake_minimum_required(VERSION 3.7)
project(sw_rosseta
    VERSION 0.1.0
    LANGUAGES CXX C ASM
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(dev)
add_subdirectory(tests)
