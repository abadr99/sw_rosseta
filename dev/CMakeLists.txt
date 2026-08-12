set(CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/calc.cpp"
)

add_library(sw_rosseta_lib STATIC ${CORE_SOURCES})

target_include_directories(sw_rosseta_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_executable(rosetta main.cpp)

target_link_libraries(rosetta
    PRIVATE
        sw_rosseta_lib
)
