cmake_minimum_required(VERSION 3.30)

project(sblib-test CXX)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Enable verbose output from the build system" FORCE)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)


set(CATCH_PATH "${CMAKE_SOURCE_DIR}/../../Catch" CACHE PATH "Path the Catch project")
set(SBLIB_PATH "${CMAKE_SOURCE_DIR}/../../sblib" CACHE PATH "Path to the sblib")

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "How to build")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
set(TARGET_X86 ${PROJECT_NAME}_x86)
set(TARGET_X64 ${PROJECT_NAME}_x64)

if (NOT EXISTS "${CATCH_PATH}")
    message(FATAL_ERROR "Catch project not found. Check setting of CATCH_PATH (${CATCH_PATH})")
endif()

if (NOT EXISTS "${SBLIB_PATH}")
    message(FATAL_ERROR "sblib project not found. Check setting of SBLIB_PATH (${SBLIB_PATH})")
endif()

include(sblib-test.cmake)
include(${CATCH_PATH}/catch.cmake)

set(SBLIB_TEST_FULL_SRC
        ${SBLIB_CATCH_SRC}
#        ${SBLIB_SRC}
        ${SBLIB_TEST_SRC}
)

set(CORE_DEFINES
        IAP_EMULATION
        __LPC11XX__
)

set(DEBUG_DEFINES
        ${CORE_DEFINES}
        DEBUG
        SERIAL_SPEED=1500000
        DEBUG_BUS
        DEBUG_BUS_BITLEVEL
        DUMP_TELEGRAMS
        PIO_FOR_TEL_END_IND=PIO1_4
        DUMP_COM_OBJ
        DUMP_MEM_OPS
        DUMP_PROPERTIES
        DUMP_TL4
)

set(RELEASE_DEFINES
        ${CORE_DEFINES}
        NDEBUG
)

set(CORE_FLAGS
        -Wall -Wlogical-op -Woverloaded-virtual
        -O0
        -g3
)

set(DEBUG32_FLAGS
        ${CORE_FLAGS}
        -m32
)

set(RELEASE32_FLAGS
        ${DEBUG32_FLAGS}
)

set(DEBUG64_FLAGS
        ${CORE_FLAGS}
        -m64
)

set(RELEASE64_FLAGS
        ${DEBUG64_FLAGS}
)


include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-m32" COMPILER_SUPPORTS_32BIT)
if(COMPILER_SUPPORTS_32BIT)
    add_library(${TARGET_X86} STATIC ${SBLIB_TEST_FULL_SRC})
    target_link_libraries(${TARGET_X86} PUBLIC sblib_x86)
    #add_subdirectory(${CATCH_PATH} catch)
    add_subdirectory(${SBLIB_PATH} sblib_x86)
    target_include_directories(${TARGET_X86} PUBLIC ${CATCH_PATH}/inc)
    target_include_directories(${TARGET_X86} PUBLIC ${SBLIB_PATH}/inc)
    target_include_directories(${TARGET_X86} PUBLIC cpu-emu)
    target_include_directories(${TARGET_X86} PUBLIC inc)
    target_compile_definitions(${TARGET_X86} PRIVATE
            $<$<CONFIG:Debug>:${DEBUG_DEFINES}>
            $<$<CONFIG:Release>:${RELEASE_DEFINES}>
    )
    target_compile_options(${TARGET_X86} PRIVATE
            $<$<CONFIG:Debug>:${DEBUG32_FLAGS}>
            $<$<CONFIG:Release>:${RELEASE32_FLAGS}>
    )
else()
    message(NOTICE "Looks like the compiler has no 32bit support. (in ${PROJECT_NAME})")
endif()



# todo find safer way to detect 32/64bit support
check_cxx_compiler_flag("-m64" COMPILER_SUPPORTS_64BIT)
if(COMPILER_SUPPORTS_64BIT)
    #todo test x64 target
    message(AUTHOR_WARNING "64bit target ${TARGET_X64} is not tested yet")
    add_library(${TARGET_X64} STATIC ${SBLIB_TEST_FULL_SRC})
    target_link_libraries(${TARGET_X64} PUBLIC sblib_x64)
    #add_subdirectory(${CATCH_PATH} catch)
    add_subdirectory(${SBLIB_PATH} sblib_x64)
    target_include_directories(${TARGET_X64} PUBLIC ${CATCH_PATH}/inc)
    target_include_directories(${TARGET_X64} PUBLIC ${SBLIB_PATH}/inc)
    target_include_directories(${TARGET_X64} PUBLIC cpu-emu)
    target_include_directories(${TARGET_X64} PUBLIC inc)
    target_compile_definitions(${TARGET_X64} PRIVATE
            $<$<CONFIG:Debug>:${DEBUG_DEFINES}>
            $<$<CONFIG:Release>:${RELEASE_DEFINES}>
    )
    target_compile_options(${TARGET_X64} PRIVATE
            $<$<CONFIG:Debug>:${DEBUG64_FLAGS}>
            $<$<CONFIG:Release>:${RELEASE64_FLAGS}>
    )
else()
    message(NOTICE "Looks like the compiler has no 64bit support. (in ${PROJECT_NAME})")
endif()