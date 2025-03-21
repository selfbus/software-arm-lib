cmake_minimum_required(VERSION 3.30)

project(sblib LANGUAGES CXX VERSION 2.10)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Enable verbose output from the build system" FORCE)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SBLPC_DIR ${CMAKE_SOURCE_DIR}/../../CMSIS_CORE_LPC11xx CACHE PATH "Where the CMSIS_CORE_LPC11xx lib is")
# todo write docu how to get/set our CMAKE_TOOLCHAIN_FILE (CMSIS_CORE_LPC11xx/LPC11xx.cmake).
# Set it here only, if cross-compiling, otherwise sblib-test and lib-test-cases builds will fail
# Set toolchain file, if cross-compiling and it´s not specified
if(CMAKE_CROSSCOMPILING AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    get_filename_component(CMAKE_TOOLCHAIN_FILE ${SBLPC_DIR}/LPC11xx.cmake REALPATH CACHE)
endif()
if(CMAKE_CROSSCOMPILING AND NOT EXISTS ${CMAKE_TOOLCHAIN_FILE})
    message(FATAL_ERROR "Toolchain file does not exist: " ${CMAKE_TOOLCHAIN_FILE})
endif()

set(TARGET_ARM ${PROJECT_NAME})
set(TARGET_X86 ${PROJECT_NAME}_x86)
set(TARGET_X64 ${PROJECT_NAME}_x64)


set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "How to build")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
set(LOGGING OFF CACHE BOOL "Whether to through the serial port")



# todo create options for all defines/symbols
option(DEBUG_BUS "Enable dumping of state machine interrupt data e.g timer values" OFF)
option(DEBUG_BUS_BITLEVEL "Extension used with DEBUG_BUS to dump interrupt of each bit -
        use with care due to easy overflow of the trace buffer" OFF)

set(DEBUG_DEFINES_ARM
#        DEBUG
        SERIAL_SPEED=1500000
        DEBUG_BUS
        DEBUG_BUS_BITLEVEL
        DUMP_TELEGRAMS
        PIO_FOR_TEL_END_IND=PIO1_4
        DUMP_COM_OBJ
        DUMP_MEM_OPS
        DUMP_PROPERTIES
        DUMP_TL4
#        SERIAL_RX_PIN=PIO2_7
#        SERIAL_TX_PIN=PIO2_8
#        INCLUDE_SERIAL
#        ROUTER
#        BUSMONITOR
)
set(DEBUG_DEFINES_X86_X64
        DEBUG
        INCLUDE_SERIAL
        SERIAL_SPEED=1500000
        DEBUG_BUS
        DEBUG_BUS_BITLEVEL
        DUMP_TELEGRAMS
        PIO_FOR_TEL_END_IND=PIO1_4
        DUMP_COM_OBJ
        DUMP_MEM_OPS
        DUMP_PROPERTIES
        DUMP_TL4
        __LPC11XX__
        IAP_EMULATION
)

set(RELEASE_DEFINES
        NDEBUG
        __LPC11XX__
        IAP_EMULATION
)

# todo -fmacro-prefix-map="${CMAKE_SOURCE_DIR}/"= does not work as intended
# Most of the debug compile flags are already set by the LPC11xx toolchain file (LPC11xx.cmake)
set(DEBUG_FLAGS_ARM
        #${CMAKE_CXX_FLAGS_DEBUG}
        # todo warnings to enable in future
        # -Wextra
        # -pedantic
        # -Wconversion
)

# todo -fmacro-prefix-map="${CMAKE_SOURCE_DIR}/"= does not work as intended
# Most of the release compile flags are already set by the LPC11xx toolchain file (LPC11xx.cmake)
set(RELEASE_FLAGS_ARM
        #${CMAKE_CXX_FLAGS_RELEASE}
        # todo warnings to enable in future
        # -Wextra
        # -pedantic
        # -Wconversion
)

set (COMMON_X86_X64_FLAGS
        -Wall -Wlogical-op -Woverloaded-virtual
        -O0
        -g3
        -c
        -fmessage-length=0
)

set(DEBUG_X86_FLAGS
        ${COMMON_X86_X64_FLAGS}
        -m32
)

set(RELEASE_X86_FLAGS
        ${COMMON_X86_X64_FLAGS}
        -m32
)

set(DEBUG_X64_FLAGS
        ${COMMON_X86_X64_FLAGS}
        -m64
)

set(RELEASE_X64_FLAGS
        ${COMMON_X86_X64_FLAGS}
        -m64
)

# include sblib source code
include_directories(inc) # sblib/inc directory
include(sblib.cmake)     # list of all sblib source code files (*.h, *.cpp, ...)

# include potential logging source code
# todo LOGGING is untested
if (LOGGING AND CMAKE_CROSSCOMPILING)
    add_subdirectory(${SBLOG_DIR} sblog)
    target_include_directories(${TARGET_ARM} PUBLIC ${SBLOG_DIR})
    file(GLOB LOGSRC ${SBLOG_DIR}/log/*.cpp ${SBLOG_DIR}/log/*.h)
    list(APPEND SBLIB_SRC ${LOGSRC})
    add_definitions(-DLOGGING)
endif()

if(CMAKE_CROSSCOMPILING)
    add_library(${TARGET_ARM} STATIC ${SBLIB_SRC}) # sblib for LPC11xx
    # add CMSIS_CORE_LPC11xx library only when cross-compiling for it
    add_subdirectory(${SBLPC_DIR} CMSIS_CORE_LPC11xx)
    target_include_directories(${TARGET_ARM} PUBLIC ${SBLPC_DIR}/inc)
    target_compile_definitions(${TARGET_ARM} PRIVATE
            $<$<CONFIG:Debug>:${DEBUG_DEFINES_ARM}>
            $<$<CONFIG:Release>:${RELEASE_DEFINES}>
    )
    target_compile_options(${TARGET_ARM} PRIVATE
            $<$<CONFIG:Debug>:${DEBUG_FLAGS_ARM}>
            $<$<CONFIG:Release>:${RELEASE_FLAGS_ARM}>
    )
else()
    # todo find safer way to detect 32/64bit support
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-m32" COMPILER_SUPPORTS_32BIT)
    if(COMPILER_SUPPORTS_32BIT)
        add_library(${TARGET_X86} STATIC ${SBLIB_SRC}) # 32bit sblib
        target_include_directories(${TARGET_X86} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../test/sblib/cpu-emu)
        target_compile_definitions(${TARGET_X86} PRIVATE
                $<$<CONFIG:Debug>:${DEBUG_DEFINES_X86_X64}>
                $<$<CONFIG:Release>:${RELEASE_DEFINES}>
        )
        target_compile_options(${TARGET_X86} PRIVATE
                $<$<CONFIG:Debug>:${DEBUG_X86_FLAGS}>
                $<$<CONFIG:Release>:${RELEASE_X86_FLAGS}>
        )
    else()
        message(NOTICE "Looks like the compiler has no 32bit support. (in ${PROJECT_NAME})")
    endif()

    # todo find safer way to detect 32/64bit support
    check_cxx_compiler_flag("-m64" COMPILER_SUPPORTS_64BIT)
    if(COMPILER_SUPPORTS_64BIT)
        #todo test x64 target
        message(AUTHOR_WARNING "64bit target ${TARGET_X64} is not tested yet")
        add_library(${TARGET_X64} STATIC ${SBLIB_SRC}) # 64bit sblib
        target_include_directories(${TARGET_X64} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../test/sblib/cpu-emu)
        target_compile_definitions(${TARGET_X64} PRIVATE
                $<$<CONFIG:Debug>:${DEBUG_DEFINES_X86_X64}>
                $<$<CONFIG:Release>:${RELEASE_DEFINES}>
        )
        target_compile_options(${TARGET_X64} PRIVATE
                $<$<CONFIG:Debug>:${DEBUG_X64_FLAGS}>
                $<$<CONFIG:Release>:${RELEASE_X64_FLAGS}>
        )
    else()
        message(NOTICE "Looks like the compiler has no 64bit support. (in ${PROJECT_NAME})")
    endif()
endif()
