file(GLOB_RECURSE TEST_SOURCES 
    "src/*.cpp"
    "src/*.c"
    "src/*.S"
    "src/*.s"
)

add_executable(unit_tests ${TEST_SOURCES})

target_link_libraries(unit_tests
    PRIVATE
        sw_rosseta_lib
)
