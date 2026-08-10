include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

if(TEST_SOURCES)
    add_executable(unit_tests ${TEST_SOURCES})

    target_link_libraries(unit_tests
        PRIVATE
            gtest_main
            sw_rosseta_lib
    )

    include(GoogleTest)
    gtest_discover_tests(unit_tests)
endif()
