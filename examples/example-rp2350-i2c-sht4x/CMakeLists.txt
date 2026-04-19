cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(rp2350_i2c_sht4x C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

set(RP2350_EXAMPLE_NAME rp2350_i2c_sht4x)
set(RP2350_EXAMPLE_SRC src/main.cpp)
include(${CMAKE_CURRENT_SOURCE_DIR}/../rp2350_example_common.cmake)
