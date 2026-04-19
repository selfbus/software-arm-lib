# rp2350_example_common.cmake - Shared CMake configuration for RP2350 examples
#
# Include this from each RP2350 example's CMakeLists.txt after defining:
#   RP2350_EXAMPLE_NAME   - executable/project name
#   RP2350_EXAMPLE_SRC    - list of source files
#
# Optional:
#   RP2350_EXTRA_LIBS     - additional Pico SDK libraries to link

# ---- sblib configuration ----
set(SBLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../sblib)
include(${SBLIB_DIR}/sblib_rp2350.cmake)

# ---- PIO assembly (needed by sblib even if example doesn't use KNX) ----
pico_generate_pio_header(${RP2350_EXAMPLE_NAME}
        ${SBLIB_DIR}/src/hal/rp2350/knx_tp1.pio
)

# ---- Build sblib as static library (shared by all targets in this build) ----
if(NOT TARGET sblib_rp2350)
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
            hardware_i2c
            hardware_spi
            hardware_adc
            hardware_pwm
            pico_unique_id
    )
endif()

# ---- Example application ----
add_executable(${RP2350_EXAMPLE_NAME} ${RP2350_EXAMPLE_SRC})

target_link_libraries(${RP2350_EXAMPLE_NAME}
        sblib_rp2350
        pico_stdlib
        ${RP2350_EXTRA_LIBS}
)

target_compile_definitions(${RP2350_EXAMPLE_NAME} PRIVATE
        __SBLIB_TARGET_RP2350__
)

# Enable USB output for printf debugging (no UART conflict with sblib Serial)
pico_enable_stdio_usb(${RP2350_EXAMPLE_NAME} 1)
pico_enable_stdio_uart(${RP2350_EXAMPLE_NAME} 0)

# Create UF2 output for drag-and-drop flashing
pico_add_extra_outputs(${RP2350_EXAMPLE_NAME})
