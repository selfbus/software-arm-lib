# CMakeLists.txt - Example project using sblib on RP2350 (Pico SDK)
#
# This is a template for building a KNX device firmware for the RP2350.
#
# Prerequisites:
#   - Pico SDK installed (set PICO_SDK_PATH environment variable)
#   - ARM or RISC-V toolchain installed
#
# Build:
#   mkdir build && cd build
#   cmake -DPICO_SDK_PATH=/path/to/pico-sdk ..
#   make
#

cmake_minimum_required(VERSION 3.13)

# Pull in Pico SDK (must be before project())
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(knx_rp2350_example C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

# ---- sblib configuration ----
set(SBLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../sblib)
include(${SBLIB_DIR}/sblib_rp2350.cmake)

# ---- PIO assembly ----
# Generate the .pio.h header from the PIO assembly file
pico_generate_pio_header(knx_rp2350_example
        ${SBLIB_DIR}/src/hal/rp2350/knx_tp1.pio
)

# ---- Build sblib as static library ----
add_library(sblib_rp2350 STATIC ${SBLIB_RP2350_SRC})

target_include_directories(sblib_rp2350 PUBLIC
        ${SBLIB_DIR}/inc
)

target_compile_definitions(sblib_rp2350 PRIVATE
        __SBLIB_TARGET_RP2350__
)

target_link_libraries(sblib_rp2350
        pico_stdlib
        hardware_pio
        hardware_flash
        hardware_sync
        hardware_timer
        hardware_gpio
        hardware_clocks
        hardware_watchdog
        pico_unique_id
)

# ---- Example application ----
add_executable(knx_rp2350_example
        src/main.cpp
)

target_link_libraries(knx_rp2350_example
        sblib_rp2350
        pico_stdlib
)

target_compile_definitions(knx_rp2350_example PRIVATE
        __SBLIB_TARGET_RP2350__
)

# Enable USB or UART output for printf debugging
pico_enable_stdio_usb(knx_rp2350_example 1)
pico_enable_stdio_uart(knx_rp2350_example 0)

# Create UF2 output for drag-and-drop flashing
pico_add_extra_outputs(knx_rp2350_example)
