cmake_minimum_required(VERSION 3.15)

# Configure ARM Cortex toolchain
include(cmake/ToolchainARMCortex.cmake)

project(Aron
        DESCRIPTION "Firmware for Modular Current Source (Aron)"
        LANGUAGES CXX C)

# Project configuration (rest remains the same)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
set(CMAKE_BUILD_TYPE Release)
set(MCU_TYPE "CM4F")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(LINKER_SCRIPT_PATH lib/Device_SAME51/linker)
set(LINKER_SCRIPT_NAME ${LINKER_SCRIPT_PATH}/same51j20a_flash.ld)
set(MCU_NAME SAME51J20A)

include(cmake/PreventInSourceBuilds.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/ARMMCU.cmake)
include(cmake/PrintSize.cmake)

# fmt (remains the same)
set(FMT_OS OFF CACHE BOOL "Disable OS")
set(CMAKE_CXX_STANDARD_TEMP ${CMAKE_CXX_STANDARD})
set(CMAKE_CXX_STANDARD 20)
add_subdirectory(lib/fmt_minimal EXCLUDE_FROM_ALL)
set(CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD_TEMP})
target_compile_definitions(fmt PUBLIC
        FMT_EXCEPTIONS=0
        FMT_USE_NOEXCEPT=1
        FMT_USE_FLOAT=1
        FMT_USE_DOUBLE=0
        FMT_USE_LONG_DOUBLE=0
        FMT_USE_FLOAT128=0
        FMT_STATIC_THOUSANDS_SEPARATOR=','
        FMT_UNICODE=0
        FMT_USE_USER_DEFINED_LITERALS=1
        FMT_REDUCE_INT_INSTANTIATIONS=1
        FMT_ENFORCE_COMPILE_STRING
        NDEBUG)
target_link_libraries(fmt PUBLIC mcu_cm4f)

# Include directories
include_directories(
    lib/CMSIS/include
    lib/Device_SAME51/include
    src # Add the main source directory
)

# Startup code
add_library(startup
        lib/Device_SAME51/src/startup_same51.c
        lib/Device_SAME51/src/system_same51.c
)
target_link_libraries(startup PUBLIC mcu_cm4f)

# Main code
file(GLOB_RECURSE ARON_SOURCES
    "src/*.cpp"
    "src/*.c"
    "src/*.hpp"
)

add_executable(aron ${ARON_SOURCES})

target_link_libraries(aron PUBLIC
        startup
        fmt::fmt
        warnings
)

print_size(aron)