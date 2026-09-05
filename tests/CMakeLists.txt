include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

set(TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/test_Decoder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_tests/BinaryLoader/LiefBinaryLoaderTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/unit_tests/option_parser/OptionParserTests.cpp"
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
