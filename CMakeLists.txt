cmake_minimum_required(VERSION 3.15)
project(sw_rosseta VERSION 0.1.0 LANGUAGES CXX C ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

include(FetchContent)

FetchContent_Declare(
    zydis
    GIT_REPOSITORY https://github.com/zyantific/zydis.git
    GIT_TAG        v4.1.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(zydis)

# Unpack the vendored LIEF SDK into the build tree, then consume it via find_package.
set(LIEF_SDK_VERSION "1.0.0")
if(WIN32)
    set(LIEF_SDK_ARCHIVE "${CMAKE_SOURCE_DIR}/third_party/lief/LIEF-${LIEF_SDK_VERSION}-win64.zip")
    set(LIEF_SDK_ROOT "${CMAKE_BINARY_DIR}/lief-sdk/LIEF-${LIEF_SDK_VERSION}-win64")
else()
    set(LIEF_SDK_ARCHIVE "${CMAKE_SOURCE_DIR}/third_party/lief/LIEF-${LIEF_SDK_VERSION}-Linux-x86_64.tar.gz")
    set(LIEF_SDK_ROOT "${CMAKE_BINARY_DIR}/lief-sdk/LIEF-${LIEF_SDK_VERSION}-Linux-x86_64")
endif()

if(NOT EXISTS "${LIEF_SDK_ARCHIVE}")
    message(FATAL_ERROR "Vendored LIEF SDK archive not found: ${LIEF_SDK_ARCHIVE}")
endif()

if(NOT EXISTS "${LIEF_SDK_ROOT}/lib/cmake/LIEF/LIEFConfig.cmake")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/lief-sdk")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf "${LIEF_SDK_ARCHIVE}"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/lief-sdk"
        RESULT_VARIABLE LIEF_EXTRACT_RC
    )
    if(NOT LIEF_EXTRACT_RC EQUAL 0)
        message(FATAL_ERROR "Failed to extract LIEF SDK from ${LIEF_SDK_ARCHIVE}")
    endif()
endif()

find_package(LIEF ${LIEF_SDK_VERSION} REQUIRED COMPONENTS STATIC PATHS "${LIEF_SDK_ROOT}" NO_DEFAULT_PATH)

enable_testing()

add_subdirectory(dev)
add_subdirectory(tests)
