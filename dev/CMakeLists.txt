set(CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/Decoder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/Instruction.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/BinaryLoaderInterface.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/LiefBinaryLoader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/Cli11OptionParser.cpp"
)

add_library(sw_rosseta_lib STATIC ${CORE_SOURCES})

target_include_directories(sw_rosseta_lib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        ${CMAKE_SOURCE_DIR}/third_party
)

target_link_libraries(sw_rosseta_lib
      PUBLIC
        Zydis
        LIEF::LIEF
)

# LIEF's cryptography parser requires the Windows Socket API
if(WIN32)
    target_link_libraries(sw_rosseta_lib PUBLIC ws2_32)
endif()

add_executable(rosetta src/main.cpp)

target_link_libraries(rosetta
        PRIVATE
        sw_rosseta_lib
)
