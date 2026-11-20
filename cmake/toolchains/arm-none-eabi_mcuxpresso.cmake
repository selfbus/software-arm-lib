cmake_minimum_required(VERSION 3.28)

# Toolchain file for MCUXpresso IDE arm-none-eabi toolchain
#
# Minimum supported MCUXpresso version is 11.9.0.
#
# The compiler detection is skipped (*_COMPILER_FORCED). With a forced compiler CMake derives
# the default standard from the compiler version (Compiler/GNU-<LANG>.cmake);
# arm-none-eabi_mcuxpresso/compiler_defaults.cmake sets that version, so C_STANDARD/CXX_STANDARD
# and C_EXTENSIONS/CXX_EXTENSIONS result in -std=... as with a detected compiler.
#
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=path/to/arm-none-eabi_mcuxpresso.cmake ...

if(NOT TOOLCHAIN_PREFIX)
    set(TOOLCHAIN_PREFIX $ENV{MCUXPRESSO_TOOLCHAIN_PATH})
    message(STATUS "No TOOLCHAIN_PREFIX specified. Using environment variable MCUXPRESSO_TOOLCHAIN_PATH=\"${TOOLCHAIN_PREFIX}\"")
endif()

if(NOT EXISTS "${TOOLCHAIN_PREFIX}")
    message(FATAL_ERROR "TOOLCHAIN_PREFIX directory \"${TOOLCHAIN_PREFIX}\" does not exist.\
            Specify path to arm-none-eabi toolchain with -DTOOLCHAIN_PREFIX=\"C:/nxp/MCUXpressoIDE_25.6.136/ide/tools\" (Windows), \
            -DTOOLCHAIN_PREFIX=\"/usr/local/mcuxpressoide-25.6.136/ide/tools\" (Linux) or set environment variable MCUXPRESSO_TOOLCHAIN_PATH.")
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Creates compile_commands.json file for easier debugging and IDE support
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY) # Prevent CMake from trying to link executables during compiler tests

set(MIN_MCUXPRESSO_VERSION_SUPPORTED "11.9.0") # 11.9.0.2144 changed path from ../ide/binaries to ../ide/LinkServer/binaries

# MCUXpresso version check
string(TOLOWER "${TOOLCHAIN_PREFIX}" LOWER_TOOLCHAIN_PREFIX)
string(REGEX MATCH "mcuxpressoide" MATCH_STR ${LOWER_TOOLCHAIN_PREFIX})
if(NOT ${MATCH_STR} STREQUAL "")
    # Get version from specified path prefix
    string(REGEX MATCH "([0-9]+.[0-9]+.[0-9]+)" MCUXPRESSO_VERSION ${TOOLCHAIN_PREFIX})
    if(NOT ${MCUXPRESSO_VERSION} STREQUAL "")
        message(STATUS "MCUXpresso detected: v${MCUXPRESSO_VERSION}")
        # Check if version is supported
        if(${MCUXPRESSO_VERSION} VERSION_LESS ${MIN_MCUXPRESSO_VERSION_SUPPORTED})
            message(FATAL_ERROR "MCUXpresso version not supported: ${MCUXPRESSO_VERSION}. Use ${MIN_MCUXPRESSO_VERSION_SUPPORTED} or newer.")
        endif()
    else()
        message(NOTICE "Could not check MCUXpresso version. Build may not work correctly.")
    endif()
else()
    message(NOTICE "Could not find MCUXpresso in ${TOOLCHAIN_PREFIX}. Build may not work correctly.")
endif()

if(CMAKE_HOST_WIN32) # TODO use if(CMAKE_HOST_EXECUTABLE_SUFFIX STREQUAL ".exe") when switching to CMake >=3.31
    set(EXE_SUFFIX ".exe") # TODO replace with CMAKE_HOST_EXECUTABLE_SUFFIX when switching to CMake >=3.31
    set(BATCH_SUFFIX ".cmd")
else()
    set(EXE_SUFFIX "")
    set(BATCH_SUFFIX "")
endif()

set(TARGET_TRIPLET "arm-none-eabi")

# Where we find NXP tools for flashing and debugging
get_filename_component(LINK_SERVER_BIN ${TOOLCHAIN_PREFIX}/../LinkServer/binaries REALPATH CACHE "Path to LinkServer binaries")
set(BOOT_LINK1 ${LINK_SERVER_BIN}/boot_link1${BATCH_SUFFIX} CACHE FILEPATH "boot_link1 Filename")
set(BOOT_LINK2 ${LINK_SERVER_BIN}/boot_link2${BATCH_SUFFIX} CACHE FILEPATH "boot_link2 Filename")
set(REDLINK ${LINK_SERVER_BIN}/crt_emu_cm_redlink${EXE_SUFFIX} CACHE FILEPATH "redlink")
if(NOT EXISTS "${LINK_SERVER_BIN}")
    message(FATAL_ERROR "LinkServer binaries directory does not exist: ${LINK_SERVER_BIN}")
endif()
if(NOT EXISTS "${BOOT_LINK1}")
    message(FATAL_ERROR "boot_link1 file does not exist: ${BOOT_LINK1}")
endif()
if(NOT EXISTS "${BOOT_LINK2}")
    message(FATAL_ERROR "boot_link2 file does not exist: ${BOOT_LINK2}")
endif()
if(NOT EXISTS "${REDLINK}")
    message(FATAL_ERROR "crt_emu_cm_redlink file does not exist: ${REDLINK}")
endif()

get_filename_component(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_PREFIX}/bin REALPATH CACHE)
get_filename_component(TOOLCHAIN_INC_DIR ${TOOLCHAIN_PREFIX}/${TARGET_TRIPLET}/include REALPATH CACHE)
get_filename_component(TOOLCHAIN_LIB_DIR ${TOOLCHAIN_PREFIX}/${TARGET_TRIPLET}/lib REALPATH CACHE)

set(TOOLS_PREFIX ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET})
set(CMAKE_C_COMPILER ${TOOLS_PREFIX}-gcc${EXE_SUFFIX} CACHE FILEPATH "c compiler")
set(CMAKE_CXX_COMPILER ${TOOLS_PREFIX}-g++${EXE_SUFFIX} CACHE FILEPATH "cxx compiler")
set(CMAKE_ASM_COMPILER ${TOOLS_PREFIX}-gcc${EXE_SUFFIX} CACHE FILEPATH "asm compiler")
set(CMAKE_OBJCOPY ${TOOLS_PREFIX}-objcopy${EXE_SUFFIX} CACHE FILEPATH "objcopy")
set(CMAKE_OBJDUMP ${TOOLS_PREFIX}-objdump${EXE_SUFFIX} CACHE FILEPATH "objdump")
set(CMAKE_AR ${TOOLS_PREFIX}-ar${EXE_SUFFIX} CACHE FILEPATH "archiver")
set(CMAKE_STRIP ${TOOLS_PREFIX}-strip${EXE_SUFFIX} CACHE FILEPATH "strip")
set(CMAKE_SIZE ${TOOLS_PREFIX}-size${EXE_SUFFIX} CACHE FILEPATH "size")

if(NOT EXISTS "${CMAKE_C_COMPILER}")
    message(FATAL_ERROR "CMAKE_C_COMPILER file does not exist: ${CMAKE_C_COMPILER}")
endif()
if(NOT EXISTS "${CMAKE_CXX_COMPILER}")
    message(FATAL_ERROR "CMAKE_CXX_COMPILER file does not exist: ${CMAKE_CXX_COMPILER}")
endif()
if(NOT EXISTS "${CMAKE_ASM_COMPILER}")
    message(FATAL_ERROR "CMAKE_ASM_COMPILER file does not exist: ${CMAKE_ASM_COMPILER}")
endif()
if(NOT EXISTS "${CMAKE_OBJCOPY}")
    message(FATAL_ERROR "CMAKE_OBJCOPY file does not exist: ${CMAKE_OBJCOPY}")
endif()
if(NOT EXISTS "${CMAKE_OBJDUMP}")
    message(FATAL_ERROR "CMAKE_OBJDUMP file does not exist: ${CMAKE_OBJDUMP}")
endif()
if(NOT EXISTS "${CMAKE_AR}")
    message(FATAL_ERROR "CMAKE_AR file does not exist: ${CMAKE_AR}")
endif()
if(NOT EXISTS "${CMAKE_STRIP}")
    message(FATAL_ERROR "CMAKE_STRIP file does not exist: ${CMAKE_STRIP}")
endif()
if(NOT EXISTS "${CMAKE_SIZE}")
    message(FATAL_ERROR "CMAKE_SIZE file does not exist: ${CMAKE_SIZE}")
endif()

# Adjust the default behaviour of the FIND_XXX() commands:
# i)    Search headers and libraries in the target environment
# ii)   Search programs in the host environment
set(CMAKE_FIND_ROOT_PATH "${TOOLCHAIN_PREFIX}/${TARGET_TRIPLET}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_COMPILER_IS_GNUCC     1)
set(CMAKE_C_COMPILER_ID         GNU)
set(CMAKE_C_COMPILER_ID_RUN     TRUE)
set(CMAKE_C_COMPILER_FORCED     TRUE)
set(CMAKE_CXX_COMPILER_ID       GNU)
set(CMAKE_CXX_COMPILER_ID_RUN   TRUE)
set(CMAKE_CXX_COMPILER_FORCED   TRUE)


set(TOOLCHAIN_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(FLAGS_DIR "${TOOLCHAIN_DIR}/arm-none-eabi_mcuxpresso")

# Compiler version, CMake derives the default language standard from it (the compiler detection is skipped)
include("${FLAGS_DIR}/compiler_defaults.cmake")

# Include compiler flags
include("${FLAGS_DIR}/flags_compiler.cmake")

# Include linker flags
include("${FLAGS_DIR}/flags_linker.cmake")

# Include assembler flags
include("${FLAGS_DIR}/flags_assembler.cmake")

# Set all CMAKE_<LANG>_FLAGS_INIT flags
include("${FLAGS_DIR}/flags_set_init.cmake")

# Intentional override all CMAKE_<LANG>_FLAGS with init flags to stay consistent with MCUxpresso
# Check out https://stackoverflow.com/questions/72549634/cmake-toolchain-file-setting-cmake-cxx-flags
include("${FLAGS_DIR}/flags_override_with_init.cmake")
