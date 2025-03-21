cmake_minimum_required(VERSION 3.30)

project(lib-test-cases LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Enable verbose output from the build system" FORCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "How to build the ${CMAKE_PROJECT_NAME}")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)

set(CATCH_PATH "${CMAKE_SOURCE_DIR}/../../Catch" CACHE PATH "Path to the Catch project")
set(SBLIB_PATH "${CMAKE_SOURCE_DIR}/../../sblib" CACHE PATH "Path to the sblib project")
set(SBLIB_TEST_PATH "${CMAKE_SOURCE_DIR}/../sblib" CACHE PATH "Path to the sblib-test project")

include(${CATCH_PATH}/catch.cmake)                # add catch.hpp
include(${CMAKE_SOURCE_DIR}/lib-test-cases.cmake) # add all test cases

set(SBLIB_TEST_CASE_SRC
        ${SBLIB_CATCH_SRC}
        ${SBLIB_LIB_TEST_CASES_SRC}
)

set(TARGET_X86 ${CMAKE_PROJECT_NAME}_x86)
set(TARGET_X64 ${CMAKE_PROJECT_NAME}_x64)

set(INCLUDE_DIRECTORIES
        ${CATCH_PATH}/inc          # catch
        ${SBLIB_PATH}/inc          # sblib
        ${SBLIB_TEST_PATH}/inc     # sblib-test
        ${SBLIB_TEST_PATH}/cpu-emu # sblib-test cpu emulation
)

#todo create options for definitions/symbols
##add_definitions(-DSERIAL_RX_PIN=PIO2_7)
##add_definitions(-DSERIAL_TX_PIN=PIO2_8)
##add_definitions(-DINCLUDE_SERIAL)
##add_definitions(-DROUTER)
##add_definitions(-DBUSMONITOR)
set(DEBUG_DEFINES
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
        __LPC11XX__
        IAP_EMULATION
)

set(RELEASE_DEFINES
        NDEBUG
        __LPC11XX__
        IAP_EMULATION
)

#  todo Warnings to enable in future
# -Wextra
# -pedantic
# -Wconversion
set(CORE_FLAGS
        -Wall -Wlogical-op -Woverloaded-virtual
        -std=c++17
        -O0
        -g3
        -c
        -fmessage-length=0
)

# From MCUXpresso lib-test-cases Debug32 build
# -std=c++0x -DDEBUG -D__LPC11XX__ -DIAP_EMULATION
# -O0 -g3 -Wall -m32 -c -fmessage-length=0
set(DEBUG_X86_FLAGS
        ${CORE_FLAGS}
        -m32
)

set(RELEASE_X86_FLAGS
        ${DEBUG_X86_FLAGS}
)

# From lib-test-cases Debug64 build
# -std=c++0x -DDEBUG -D__LPC11XX__ -DIAP_EMULATION
# -O0 -g3 -Wall -m64 -c -fmessage-length=0
set(DEBUG_X64_FLAGS
        ${CORE_FLAGS}
        -m64
)

set(RELEASE_X64_FLAGS
        ${DEBUG_X64_FLAGS}
)

# todo find safer way to detect 32/64bit support
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-m32" COMPILER_SUPPORTS_32BIT)
if(COMPILER_SUPPORTS_32BIT)
    add_executable(${TARGET_X86} ${SBLIB_TEST_CASE_SRC})
    target_include_directories(${TARGET_X86} PRIVATE ${INCLUDE_DIRECTORIES})
    add_subdirectory(${SBLIB_TEST_PATH} "sblib-test_x86")
    target_compile_definitions(${TARGET_X86} PRIVATE
            $<$<CONFIG:Debug>:${DEBUG_DEFINES}>
            $<$<CONFIG:Release>:${RELEASE_DEFINES}>
    )
    target_compile_options(${TARGET_X86} PRIVATE
            $<$<CONFIG:Debug>:${DEBUG_X86_FLAGS}>
            $<$<CONFIG:Release>:${RELEASE_X86_FLAGS}>
    )
    target_link_options(${TARGET_X86} PRIVATE "-m32")
    target_link_libraries(${TARGET_X86} "sblib-test_x86")
else()
    message(NOTICE "Looks like the compiler has no 32bit support. (in ${PROJECT_NAME})")
endif()

# todo find safer way to detect 32/64bit support
check_cxx_compiler_flag("-m64" COMPILER_SUPPORTS_64BIT)
if(COMPILER_SUPPORTS_64BIT)
    #todo test x64 target
    message(AUTHOR_WARNING "64bit target ${TARGET_X64} is not tested yet")
    add_executable(${TARGET_X64} ${SBLIB_TEST_CASE_SRC})
    target_include_directories(${TARGET_X64} PRIVATE ${INCLUDE_DIRECTORIES})
    add_subdirectory(${SBLIB_TEST_PATH} "sblib-test_x64")
    target_compile_definitions(${TARGET_X64} PRIVATE
            $<$<CONFIG:Debug>:${DEBUG_DEFINES}>
            $<$<CONFIG:Release>:${RELEASE_DEFINES}>
    )
    target_compile_options(${TARGET_X64} PRIVATE
            $<$<CONFIG:Debug>:${DEBUG_X64_FLAGS}>
            $<$<CONFIG:Release>:${RELEASE_X64_FLAGS}>
    )
    target_link_options(${TARGET_X64} PRIVATE "-m64")
    target_link_libraries(${TARGET_X86} "sblib-test_x64")
else()
    message(NOTICE "Looks like the compiler has no 64bit support. (in ${PROJECT_NAME})")
endif()
