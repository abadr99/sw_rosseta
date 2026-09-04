set(CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Decoder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Instructions.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/inc/binary_loader.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/binary_loader.cpp"
)

add_library(sw_rosseta_lib STATIC ${CORE_SOURCES})

target_include_directories(sw_rosseta_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(sw_rosseta_lib
    PUBLIC
        Zydis
        LIEF::LIEF
)

# --- Windows CI Fix ---
# LIEF's cryptography parser requires the Windows Socket API
if(WIN32)
    target_link_libraries(sw_rosseta_lib PUBLIC ws2_32)
endif()
# ----------------------

add_executable(rosetta src/main.cpp)

target_link_libraries(rosetta
    PRIVATE
        sw_rosseta_lib
)
