cmake_minimum_required(VERSION 3.25.1)

project(IONENGINE VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

if(NOT DEFINED DEFAULT_TARGET_ARCH)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
        set(DEFAULT_TARGET_ARCH "x64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i386|i686")
        set(DEFAULT_TARGET_ARCH "x86")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|ARM64")
        set(DEFAULT_TARGET_ARCH "arm64")
    else()
        message(FATAL_ERROR "Unsupported host processor: ${CMAKE_SYSTEM_PROCESSOR}. "
                "Please define DEFAULT_TARGET_ARCH explicitly (x86, x64, arm64)")
    endif()
endif()

set(SUPPORTED_ARCHITECTURES "x86;x64;arm64")

set(TARGET_ARCH ${DEFAULT_TARGET_ARCH} CACHE STRING "Target architecture")

if(NOT TARGET_ARCH IN_LIST SUPPORTED_ARCHITECTURES)
    message(FATAL_ERROR "Unsupported architecture: ${TARGET_ARCH}. Supported: ${SUPPORTED_ARCHITECTURES}")
endif()

option(BUILD_TESTING "Build unit-tests" TRUE)

add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(rhi)
add_subdirectory(mdl)
add_subdirectory(txe)
add_subdirectory(shadersys)
add_subdirectory(engine)
add_subdirectory(tools)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()