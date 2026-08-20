set(CORE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/types.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/inc/elf_loader.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/elf_loader.cpp"
)

add_library(sw_rosseta_lib STATIC ${CORE_SOURCES})

target_include_directories(sw_rosseta_lib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(sw_rosseta_lib
        PUBLIC
        LIEF::LIEF
)

add_executable(rosetta src/main.cpp)

target_link_libraries(rosetta
        PRIVATE
        sw_rosseta_lib
)
