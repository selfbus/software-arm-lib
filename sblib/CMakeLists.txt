cmake_minimum_required(VERSION 3.5)

# Set toolchain file if not specified
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    get_filename_component(CMAKE_TOOLCHAIN_FILE
            ${CMAKE_CURRENT_SOURCE_DIR}/../../CMSIS_CORE_LPC11xx/LPC11xx.cmake
            REALPATH CACHE)
endif()
if(NOT EXISTS ${CMAKE_TOOLCHAIN_FILE})
    message(FATAL_ERROR "Toolchain file does not exist: " ${CMAKE_TOOLCHAIN_FILE})
endif()

project(sblib)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "How to build")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
set(BusType "BCU_TYPE=10" CACHE STRING "Bus type")
set_property(CACHE BusType PROPERTY STRINGS BCU_TYPE=10 BCU_TYPE=20 BIM112)

set(LOGGING OFF CACHE BOOL "Whether to through the serial port")

add_definitions(-DSPI_BLOCK_TRANSFER -DSERIAL_WRITE_DIRECT -D${BusType} )

add_subdirectory(${SBLPC_DIR} CMSIS_CORE_LPC11xx)

include_directories(inc)
include_directories(${SBLPC_DIR}/inc)

set(SRC inc/sblib/eib/addr_tables.h
        inc/sblib/eib/apci.h
        inc/sblib/eib/bcu.h
        inc/sblib/eib/bcu_base.h
        inc/sblib/eib/bcu_type.h
        inc/sblib/eib/bus.h
        inc/sblib/eib/com_objects.h
        inc/sblib/eib/datapoint_types.h
        inc/sblib/eib/properties.h
        inc/sblib/eib/property_types.h
        inc/sblib/eib/sblib_default_objects.h
        inc/sblib/eib/telegram.h
        inc/sblib/eib/types.h
        inc/sblib/eib/user_memory.h
        inc/sblib/i2c/bh1750.h
        inc/sblib/i2c/ds3231.h
        inc/sblib/internal/functions.h
        inc/sblib/internal/iap.h
        inc/sblib/internal/variables.h
        inc/sblib/lcd/font.h
        inc/sblib/lcd/font_5x7.h
        inc/sblib/lcd/graphical.h
        inc/sblib/lcd/graphical_eadogs.h
        inc/sblib/lcd/graphical_sed1520.h
        inc/sblib/sensors/dht.h
        inc/sblib/sensors/ds18x20.h
        inc/sblib/analog_pin.h
        inc/sblib/arrays.h
        inc/sblib/binary.h
        inc/sblib/bits.h
        inc/sblib/buffered_stream.h
        inc/sblib/core.h
        inc/sblib/debounce.h
        inc/sblib/debug.h
        inc/sblib/digital_pin.h
        inc/sblib/eib.h
        inc/sblib/i2c.h
        inc/sblib/interrupt.h
        inc/sblib/io_pin_names.h
        inc/sblib/ioports.h
        inc/sblib/main.h
        inc/sblib/math.h
        inc/sblib/mem_mapper.h
        inc/sblib/onewire.h
        inc/sblib/platform.h
        inc/sblib/print.h
        inc/sblib/printable.h
        inc/sblib/serial.h
        inc/sblib/spi.h
        inc/sblib/stream.h
        inc/sblib/timeout.h
        inc/sblib/timer.h
        inc/sblib/types.h
        inc/sblib/usr_callback.h
        inc/sblib/utils.h
        inc/sblib/version.h
        src/eib/addr_tables.cpp
        src/eib/bcu.cpp
        src/eib/bcu_base.cpp
        src/eib/bus.cpp
        src/eib/com_objects.cpp
        src/eib/datapoint_types.cpp
        src/eib/properties.cpp
        src/eib/property_defs.cpp
        src/eib/property_types.cpp
        src/eib/telegram.cpp
        src/eib/user_memory.cpp
        src/i2c/bh1750.cpp
        src/i2c/ds3231.cpp
        src/lcd/font.cpp
        src/lcd/font_5x7.cpp
        src/lcd/graphical.cpp
        src/lcd/graphical_eadogs.cpp
        src/lcd/graphical_sed1520.cpp
#        src/lpc11uxx/digital_pin.cpp
#        src/lpc11uxx/digital_pin_port.cpp
#        src/lpc11uxx/digital_pin_pulse.cpp
#        src/lpc11uxx/platform.cpp
        src/lpc11xx/digital_pin.cpp
        src/lpc11xx/digital_pin_port.cpp
        src/lpc11xx/digital_pin_pulse.cpp
        src/lpc11xx/platform.cpp
        src/sensors/dht.cpp
        src/sensors/ds18x20.cpp
        src/analog_pin.cpp
        src/arrays.cpp
        src/buffered_stream.cpp
        src/debounce.cpp
        src/debug.cpp
        src/digital_pin_shift.cpp
        src/i2c.cpp
        src/iap.cpp
        src/ioports.cpp
        src/main.cpp
        src/mem_mapper.cpp
        src/new.cpp
        src/onewire.cpp
        src/print.cpp
        src/serial.cpp
        src/serial0.cpp
        src/spi.cpp
        src/stream.cpp
        src/timer.cpp
        src/utils.cpp)

if (LOGGING)
    add_subdirectory(${SBLOG_DIR} sblog)
    include_directories(${SBLOG_DIR})
    file(GLOB LOGSRC ${SBLOG_DIR}/log/*.cpp ${SBLOG_DIR}/log/*.h)
    list(APPEND SRC ${LOGSRC})
    add_definitions(-DLOGGING)
endif()

add_library(sblib ${SRC})

