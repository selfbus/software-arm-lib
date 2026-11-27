cmake_minimum_required(VERSION 3.28)

project(bootloaderupdater LANGUAGES CXX ASM VERSION 1.20)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Enable verbose output from the build system")
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SB_BOOTLOADER_PATH "${CMAKE_SOURCE_DIR}/../bootloader" CACHE PATH "Path to the Bootloader project")

# Toolchain file is set in CMakePresets.json or on command line
if(CMAKE_CROSSCOMPILING AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "Cross-compiling but CMAKE_TOOLCHAIN_FILE not set. Set in CMakePresets.json or configure with -DCMAKE_TOOLCHAIN_FILE=<path to>/arm-none-eabi_mcuxpresso.cmake")
endif()
if(CMAKE_CROSSCOMPILING AND DEFINED CMAKE_TOOLCHAIN_FILE AND NOT EXISTS "${CMAKE_TOOLCHAIN_FILE}")
    message(FATAL_ERROR "Toolchain file does not exist: ${CMAKE_TOOLCHAIN_FILE}")
endif()

set(TARGET_ARM ${PROJECT_NAME}_arm)

if (NOT EXISTS "${SB_BOOTLOADER_PATH}")
    message(FATAL_ERROR "Bootloader project not found. Check setting of SB_BOOTLOADER_PATH (${SB_BOOTLOADER_PATH})")
endif()

# Add the bootloader as a subdirectory to ensure it is built before the updater
add_subdirectory(${SB_BOOTLOADER_PATH} bootloader_arm)

set(BLU_PROGRAMMING_BUTTON "PIO2_0" CACHE STRING "Programming button to use. default/S9 = PIO2_0, alternative/S10 = PIO2_8, GNAX IO16FM = PIO2_11")
set_property(CACHE BLU_PROGRAMMING_BUTTON PROPERTY STRINGS PIO2_0 PIO2_8 PIO2_11)

# Conditionally add core options based on CMake options
set(CORE_DEFINES
)

if(NOT BL_PROGRAMMING_BUTTON STREQUAL "")
    list(APPEND CORE_DEFINES PROGRAMMING_BUTTON=${BL_PROGRAMMING_BUTTON})
endif()

# Conditionally add debug options based on CMake options
set(DEBUG_DEFINES
)

# common compile options for all targets and builds
add_compile_options(
    -Wall -Wlogical-op -Woverloaded-virtual -Wextra -pedantic -Wconversion
    -Wshadow -Wpointer-arith -Wwrite-strings -Wunreachable-code
    -Wno-unused-parameter -Wctor-dtor-privacy -Wold-style-cast
    -Wmissing-include-dirs -Wshadow=local -Wdouble-promotion
    -Wsign-conversion -Wformat=2 -Wno-missing-field-initializers
    -Wswitch-default -Wfloat-equal -Weffc++ -Wundef
    -Wstrict-null-sentinel -Wsign-promo
    -Wredundant-decls
    -Wcast-qual
    -Wcast-align
    -g3
)

include(bootloaderupdater.cmake)

# definitions for all targets and builds
add_compile_definitions(
    __USE_CMSIS=CMSIS_CORE_LPC11xx
    __LPC11XX__
    __NEWLIB__
    CORE_M0
    $<$<CONFIG:Debug>:DEBUG>
)

# Check if cross-compiling for ARM target
if(CMAKE_CROSSCOMPILING)
    message(NOTICE "Compiling ${PROJECT_NAME} for ARM architecture.")
    add_executable(${TARGET_ARM} ${BLU_SRC})
    target_link_libraries(${TARGET_ARM} PUBLIC CMSIS_CORE_LPC11xx_arm) # add CMSIS_CORE_LPC11xx_arm library
    target_link_libraries(${TARGET_ARM} PUBLIC sblib_arm)              # add sblib library
    target_include_directories(${TARGET_ARM} PUBLIC ${SBLPC_DIR}/inc)  # add CMSIS_CORE_LPC11xx include directory
    target_include_directories(${TARGET_ARM} PUBLIC ${SBLIB_PATH}/inc) # add sblib include directory

    # Add path to bootloader binary and ensure it is built first
    add_dependencies(${TARGET_ARM} bootloader_arm)
    target_include_directories(${TARGET_ARM} PRIVATE $<TARGET_FILE_DIR:bootloader_arm>)

    target_compile_definitions(${TARGET_ARM} PRIVATE
        ${CORE_DEFINES}
        $<$<CONFIG:Debug>:${DEBUG_DEFINES}>
    )
    target_compile_options(${TARGET_ARM} PRIVATE
        $<$<CONFIG:Debug>: -O0>
        $<$<CONFIG:Release>: -Os>
    )

    target_link_options(${TARGET_ARM} PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/linkscripts/bootloaderupdater_Flashstart_0x3000_combined.ld)
    target_link_options(${TARGET_ARM} PRIVATE -Xlinker -Map=$<TARGET_FILE_DIR:${TARGET_ARM}>/${TARGET_ARM}.map)
    add_custom_command(TARGET ${TARGET_ARM} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${TARGET_ARM}> $<TARGET_FILE_DIR:${TARGET_ARM}>/${TARGET_ARM}.hex
    )
    add_custom_command(TARGET ${TARGET_ARM} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET_ARM}> $<TARGET_FILE_DIR:${TARGET_ARM}>/${TARGET_ARM}.bin
    )
else()
    message(FATAL_ERROR "Unknown architecture. Only cross-compiling for ARM is supported.")
endif()
