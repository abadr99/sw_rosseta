set(CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/RosettaTranslationEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/Cli11OptionParser.cpp"
    
    # Binary Loader
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/BinaryLoaderInterface.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/LiefBinaryLoader.cpp"
    
    # Decoder
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/Instruction.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/ZydisDecoder.cpp"

    # CFG
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/BasicBlock.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/ControlFlowGraph.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/CfgBuilder.cpp"

    # IR
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/SsaStatment.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/SsaBlock.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/SsaVariables.cpp"
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
