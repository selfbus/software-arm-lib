cmake_minimum_required(VERSION 3.28)

# Generic toolchain file for an arm-none-eabi GCC (e.g. Arm GNU Toolchain or the
# toolchain shipped with MCUXpresso) targeting the LPC1115 (Cortex-M0).
#
# Unlike arm-none-eabi_mcuxpresso.cmake this file does not require an MCUXpresso
# installation (no LinkServer binaries, no version check). It uses the same
# compiler/linker/assembler flags (arm-none-eabi_mcuxpresso/flags_*.cmake), so
# the generated code is identical when the same compiler is used.
#
# The toolchain directory (the one containing bin/arm-none-eabi-gcc) is taken from
# the first of:
#   1. -DTOOLCHAIN_PREFIX=<dir>
#   2. environment variable ARM_TOOLCHAIN_PATH
#   3. environment variable MCUXPRESSO_TOOLCHAIN_PATH (e.g. C:/nxp/MCUXpressoIDE_24.9.25/ide/tools)
#   4. arm-none-eabi-gcc found in PATH
#
# The compiler detection is skipped (*_COMPILER_FORCED). With a forced compiler CMake
# derives the default standard from the compiler version (Compiler/GNU-<LANG>.cmake);
# arm-none-eabi_mcuxpresso/compiler_defaults.cmake sets that version, so
# C_STANDARD/CXX_STANDARD and C_EXTENSIONS/CXX_EXTENSIONS result in -std=... as with a
# detected compiler.
#
# The compiler version is reported at configure time. Reference compiler is Arm GNU Toolchain
# 14.2.rel1; an older compiler (e.g. 13.2.1 of MCUXpresso 24.9) only triggers a
# warning, it stays usable. The message may appear several times per configure
# (the file is read again for each try_compile).
#
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=path/to/arm-none-eabi.cmake ...
#
# Note: projects linking against NXP's libcr_* libraries (e.g. via GROUP() in
# MCUXpresso generated linker scripts) still need the MCUXpresso toolchain.

if(CMAKE_HOST_WIN32)
    set(EXE_SUFFIX ".exe")
else()
    set(EXE_SUFFIX "")
endif()

set(TARGET_TRIPLET "arm-none-eabi")

set(_sb_toolchain_source "TOOLCHAIN_PREFIX")
if(NOT TOOLCHAIN_PREFIX)
    if(DEFINED ENV{ARM_TOOLCHAIN_PATH} AND NOT "$ENV{ARM_TOOLCHAIN_PATH}" STREQUAL "")
        set(TOOLCHAIN_PREFIX "$ENV{ARM_TOOLCHAIN_PATH}")
        set(_sb_toolchain_source "ARM_TOOLCHAIN_PATH")
    elseif(DEFINED ENV{MCUXPRESSO_TOOLCHAIN_PATH} AND NOT "$ENV{MCUXPRESSO_TOOLCHAIN_PATH}" STREQUAL "")
        set(TOOLCHAIN_PREFIX "$ENV{MCUXPRESSO_TOOLCHAIN_PATH}")
        set(_sb_toolchain_source "MCUXPRESSO_TOOLCHAIN_PATH")
    else()
        find_program(_sb_arm_gcc ${TARGET_TRIPLET}-gcc${EXE_SUFFIX})
        if(NOT _sb_arm_gcc)
            message(FATAL_ERROR "No arm-none-eabi toolchain found. Set -DTOOLCHAIN_PREFIX=<dir>, "
                                "the environment variable ARM_TOOLCHAIN_PATH or MCUXPRESSO_TOOLCHAIN_PATH, "
                                "or add arm-none-eabi-gcc to PATH.")
        endif()
        get_filename_component(_sb_arm_bin "${_sb_arm_gcc}" DIRECTORY)
        get_filename_component(TOOLCHAIN_PREFIX "${_sb_arm_bin}/.." REALPATH)
        set(_sb_toolchain_source "PATH")
    endif()
endif()
set(TOOLCHAIN_PREFIX "${TOOLCHAIN_PREFIX}" CACHE PATH "Directory of the arm-none-eabi toolchain (contains bin/)")

if(NOT EXISTS "${TOOLCHAIN_PREFIX}/bin/${TARGET_TRIPLET}-gcc${EXE_SUFFIX}")
    message(FATAL_ERROR "No ${TARGET_TRIPLET}-gcc in ${TOOLCHAIN_PREFIX}/bin")
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Creates compile_commands.json file for easier debugging and IDE support
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY) # Prevent CMake from trying to link executables during compiler tests

get_filename_component(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_PREFIX}/bin REALPATH CACHE)
set(TOOLS_PREFIX ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET})
set(CMAKE_C_COMPILER ${TOOLS_PREFIX}-gcc${EXE_SUFFIX} CACHE FILEPATH "c compiler")
set(CMAKE_CXX_COMPILER ${TOOLS_PREFIX}-g++${EXE_SUFFIX} CACHE FILEPATH "cxx compiler")
set(CMAKE_ASM_COMPILER ${TOOLS_PREFIX}-gcc${EXE_SUFFIX} CACHE FILEPATH "asm compiler")
set(CMAKE_OBJCOPY ${TOOLS_PREFIX}-objcopy${EXE_SUFFIX} CACHE FILEPATH "objcopy")
set(CMAKE_OBJDUMP ${TOOLS_PREFIX}-objdump${EXE_SUFFIX} CACHE FILEPATH "objdump")
set(CMAKE_AR ${TOOLS_PREFIX}-ar${EXE_SUFFIX} CACHE FILEPATH "archiver")
set(CMAKE_STRIP ${TOOLS_PREFIX}-strip${EXE_SUFFIX} CACHE FILEPATH "strip")
set(CMAKE_SIZE ${TOOLS_PREFIX}-size${EXE_SUFFIX} CACHE FILEPATH "size")

foreach(tool CMAKE_C_COMPILER CMAKE_CXX_COMPILER CMAKE_OBJCOPY CMAKE_OBJDUMP CMAKE_AR CMAKE_SIZE)
    if(NOT EXISTS "${${tool}}")
        message(FATAL_ERROR "${tool} does not exist: ${${tool}}")
    endif()
endforeach()

# The compiler detection is skipped (FORCED), so ask the compiler for its version
set(_sb_gcc_version "")
execute_process(COMMAND "${CMAKE_C_COMPILER}" -dumpfullversion
                OUTPUT_VARIABLE _sb_gcc_version
                RESULT_VARIABLE _sb_gcc_version_result
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET)
if(_sb_gcc_version_result EQUAL 0 AND _sb_gcc_version MATCHES "^[0-9]+[.][0-9]+")
    message(STATUS "Toolchain from ${_sb_toolchain_source}: ${TOOLCHAIN_PREFIX} (GCC ${_sb_gcc_version})")
    if(_sb_gcc_version VERSION_LESS 14.2)
        message(WARNING "arm-none-eabi-gcc ${_sb_gcc_version} is older than the reference compiler "
                        "Arm GNU Toolchain 14.2.rel1. Set ARM_TOOLCHAIN_PATH to its directory "
                        "to get the reference build.")
    endif()
else()
    message(STATUS "Toolchain from ${_sb_toolchain_source}: ${TOOLCHAIN_PREFIX}")
    message(WARNING "Could not determine the version of ${CMAKE_C_COMPILER}. "
                    "Reference compiler is Arm GNU Toolchain 14.2.rel1 (ARM_TOOLCHAIN_PATH).")
endif()

# Search headers and libraries in the target environment, programs on the host
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

# LPC1115 (Cortex-M0) flags, shared with arm-none-eabi_mcuxpresso.cmake
set(TOOLCHAIN_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(FLAGS_DIR "${TOOLCHAIN_DIR}/arm-none-eabi_mcuxpresso")
include("${FLAGS_DIR}/compiler_defaults.cmake")
include("${FLAGS_DIR}/flags_compiler.cmake")
include("${FLAGS_DIR}/flags_linker.cmake")
include("${FLAGS_DIR}/flags_assembler.cmake")
include("${FLAGS_DIR}/flags_set_init.cmake")
include("${FLAGS_DIR}/flags_override_with_init.cmake")
