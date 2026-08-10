cmake_minimum_required(VERSION 3.20)

project(sw_rosseta)

file(GLOB_RECURSE CORE_SOURCES
    "src/*.cpp"
    "src/*.c"
    "src/*.S"
    "src/*.s"
)

add_library(sw_rosseta_lib STATIC
    ${CORE_SOURCES}
)

target_include_directories(sw_rosseta_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_executable(sw_rosseta_app
    main.cpp
)

target_link_libraries(sw_rosseta_app
    PRIVATE
    sw_rosseta_lib
)