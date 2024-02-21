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

add_definitions(-DSERIAL_WRITE_DIRECT -D${BusType} )

add_subdirectory(${SBLPC_DIR} CMSIS_CORE_LPC11xx)

include_directories(inc)
include_directories(${SBLPC_DIR}/inc)

set(SRC inc/sblib/eib/addr_tables.h
        inc/sblib/eib/addr_tablesBCU1.h
        inc/sblib/eib/addr_tablesBCU2.h
        inc/sblib/eib/addr_tablesMASK0701.h
        inc/sblib/eib/addr_tablesMASK0705.h
        inc/sblib/eib/addr_tablesSYSTEMB.h
        inc/sblib/eib/apci.h
        inc/sblib/eib/bcu_base.h
        inc/sblib/eib/bcu_default.h
        inc/sblib/eib/bcu1.h
        inc/sblib/eib/bcu2.h
        inc/sblib/eib/bus.h
        inc/sblib/eib/bus_const.h
        inc/sblib/eib/bus_debug.h
        inc/sblib/eib/com_objects.h
        inc/sblib/eib/com_objectsBCU1.h
        inc/sblib/eib/com_objectsBCU2.h
        inc/sblib/eib/com_objectsMASK0701.h
        inc/sblib/eib/com_objectsMASK0705.h
        inc/sblib/eib/com_objectsSYSTEMB.h
        inc/sblib/eib/datapoint_types.h
        inc/sblib/eib/knx_lpdu.h
        inc/sblib/eib/knx_npdu.h
        inc/sblib/eib/knx_tlayer4.h
        inc/sblib/eib/knx_tpdu.h
        inc/sblib/eib/mask0701.h
        inc/sblib/eib/mask0705.h
        inc/sblib/eib/memory.h
        inc/sblib/eib/propertiesBCU2.h
        inc/sblib/eib/propertiesMASK0701.h
        inc/sblib/eib/propertiesMASK0705.h
        inc/sblib/eib/propertiesSYSTEMB.h
        inc/sblib/eib/property_types.h
        inc/sblib/eib/systemb.h
        inc/sblib/eib/types.h
        inc/sblib/eib/typesBCU1.h
        inc/sblib/eib/typesBCU2.h
        inc/sblib/eib/typesMASK0701.h
        inc/sblib/eib/typesMASK0705.h
        inc/sblib/eib/typesSYSTEMB.h
        inc/sblib/eib/userEeprom.h
        inc/sblib/eib/userEepromBCU1.h
        inc/sblib/eib/userEepromBCU2.h
        inc/sblib/eib/userEepromMASK0701.h
        inc/sblib/eib/userEepromMASK0705.h
        inc/sblib/eib/userEepromSYSTEMB.h
        inc/sblib/eib/userRam.h
        inc/sblib/eib/userRamBCU1.h
        inc/sblib/eib/userRamBCU2.h
        inc/sblib/eib/userRamMASK0701.h
        inc/sblib/eib/userRamMASK0705.h
        inc/sblib/eib/userRamSYSTEMB.h
        inc/sblib/i2c/bh1750.h
        inc/sblib/i2c/CCS811.h
        inc/sblib/i2c/ds2482.h
        inc/sblib/i2c/ds3231.h
        inc/sblib/i2c/iaq-core.h
        inc/sblib/i2c/sensirion_gas_index_algorithm.h
        inc/sblib/i2c/SGP4x.h
        inc/sblib/i2c/SHT1x.h
        inc/sblib/i2c/SHT2x.h
        inc/sblib/i2c/SHT4x.h
        inc/sblib/internal/iap.h
        inc/sblib/lcd/font.h
        inc/sblib/lcd/font_5x7.h
        inc/sblib/lcd/graphical.h
        inc/sblib/lcd/graphical_eadogs.h
        inc/sblib/lcd/graphical_sed1520.h
        inc/sblib/sensors/dht.h
        inc/sblib/sensors/ds18x20.h
        inc/sblib/sensors/units.h
        inc/sblib/analog_pin.h
        inc/sblib/arrays.h
        inc/sblib/bits.h
        inc/sblib/buffered_stream.h
        inc/sblib/core.h
        inc/sblib/debounce.h
        inc/sblib/digital_pin.h
        inc/sblib/eibBCU1.h
        inc/sblib/eibBCU2.h
        inc/sblib/eibMASK0701.h
        inc/sblib/eibMASK0705.h
        inc/sblib/eibSYSTEMB.h
        inc/sblib/hardware_descriptor.h
        inc/sblib/i2c.h
        inc/sblib/interrupt.h
        inc/sblib/io_pin_names.h
        inc/sblib/ioports.h
        inc/sblib/libconfig.h
        inc/sblib/main.h
        inc/sblib/math.h
        inc/sblib/mem_mapper.h
        inc/sblib/onewire.h
        inc/sblib/platform.h
        inc/sblib/print.h
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
        src/eib/addr_tablesBCU1.cpp
        src/eib/addr_tablesBCU2.cpp
        src/eib/addr_tablesSYSTEMB.cpp
        src/eib/apci.cpp
        src/eib/bcu_base.cpp
        src/eib/bcu_default.cpp
        src/eib/bcu1.cpp
        src/eib/bcu2.cpp
        src/eib/bus.cpp
        src/eib/bus_debug.cpp
        src/eib/com_objects.cpp
        src/eib/com_objectsBCU1.cpp
        src/eib/com_objectsBCU2.cpp
        src/eib/com_objectsSYSTEMB.cpp
        src/eib/datapoint_types.cpp
        src/eib/hardware_descriptor.cpp
        src/eib/knx_tlayer4.cpp
        src/eib/mask0701.cpp
        src/eib/mask0705.cpp
        src/eib/memory.cpp
        src/eib/propertiesBCU2.cpp
        src/eib/propertiesMASK0701.cpp
        src/eib/propertiesSYSTEMB.cpp
        src/eib/property_dump.cpp
        src/eib/property_types.cpp
        src/eib/systemb.cpp
        src/eib/userEeprom.cpp
        src/eib/userRam.cpp
        src/i2c/bh1750.cpp
        src/i2c/CCS811.cpp
        src/i2c/ds2482.cpp
        src/i2c/ds3231.cpp
        src/i2c/iaq-core.cpp
        src/i2c/sensirion_gas_index_algorithm.cpp
        src/i2c/SGP4x.cpp
        src/i2c/SHT1x.cpp
        src/i2c/SHT2x.cpp
        src/i2c/SHT4x.cpp
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
        src/utils.cpp
        src/version.cpp)

if (LOGGING)
    add_subdirectory(${SBLOG_DIR} sblog)
    include_directories(${SBLOG_DIR})
    file(GLOB LOGSRC ${SBLOG_DIR}/log/*.cpp ${SBLOG_DIR}/log/*.h)
    list(APPEND SRC ${LOGSRC})
    add_definitions(-DLOGGING)
endif()

add_library(sblib ${SRC})

