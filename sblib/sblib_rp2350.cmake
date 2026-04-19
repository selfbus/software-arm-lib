# sblib_rp2350.cmake - Source file list for the RP2350 port of sblib.
#
# This file is the equivalent of sblib.cmake but for the RP2350 target.
# It includes only the platform-independent EIB/KNX stack and the RP2350 HAL.
#
# Usage in your project's CMakeLists.txt:
#   include(sblib_rp2350.cmake)
#   target_link_libraries(your_app sblib_rp2350 pico_stdlib hardware_pio hardware_flash)

# ---- HAL interfaces (platform-independent) ----
set(SBLIB_HAL_HEADERS
        inc/sblib/hal/platform_hal.h
        inc/sblib/hal/gpio_hal.h
        inc/sblib/hal/flash_hal.h
)

# ---- RP2350 HAL implementations ----
set(SBLIB_HAL_RP2350_SRC
        src/hal/rp2350/platform_hal_rp2350.cpp
        src/hal/rp2350/gpio_hal_rp2350.cpp
        src/hal/rp2350/flash_hal_rp2350.cpp
        src/hal/rp2350/timer_rp2350.cpp
        src/hal/rp2350/utils_rp2350.cpp
        src/hal/rp2350/digital_pin_rp2350.cpp
        src/hal/rp2350/iap_rp2350.cpp
        src/hal/rp2350/serial_rp2350.cpp
        src/hal/rp2350/i2c_rp2350.cpp
        src/hal/rp2350/spi_rp2350.cpp
        src/hal/rp2350/analog_pin_rp2350.cpp
        src/hal/rp2350/knx_tp1.pio
)

# ---- Platform-independent KNX bus interfaces ----
set(SBLIB_BUS_INTERFACE_SRC
        inc/sblib/eib/knx_bus_interface.h
        inc/sblib/eib/knx_bus_pio.h
        inc/sblib/eib/knx_bus_tpuart.h
        src/eib/knx_bus_pio.cpp
        src/eib/knx_bus_tpuart.cpp
)

# ---- KNX/EIB protocol stack (platform-independent) ----
set(SBLIB_KNX_PORTABLE_SRC
        # Headers
        inc/sblib/eib/addr_tables.h
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
        inc/sblib/eib/bus_const.h
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
        # Sources
        src/eib/addr_tables.cpp
        src/eib/addr_tablesBCU1.cpp
        src/eib/addr_tablesBCU2.cpp
        src/eib/addr_tablesSYSTEMB.cpp
        src/eib/apci.cpp
        src/eib/bcu_base.cpp
        src/eib/bcu_default.cpp
        src/eib/bcu1.cpp
        src/eib/bcu2.cpp
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
        src/eib/property_types.cpp
        src/eib/systemb.cpp
        src/eib/userEeprom.cpp
        src/eib/userRam.cpp
)

# ---- Portable utility sources ----
set(SBLIB_UTIL_PORTABLE_SRC
        inc/sblib/arrays.h
        inc/sblib/bits.h
        inc/sblib/debounce.h
        inc/sblib/math.h
        inc/sblib/timeout.h
        inc/sblib/types.h
        inc/sblib/utils.h
        inc/sblib/version.h
        inc/sblib/digital_pin.h
        inc/sblib/io_pin_names.h
        inc/sblib/serial.h
        inc/sblib/spi.h
        inc/sblib/i2c.h
        inc/sblib/analog_pin.h
        inc/sblib/timer.h
        src/arrays.cpp
        src/debounce.cpp
        src/iap.cpp
        src/main.cpp
        src/new.cpp
        src/timer.cpp
        src/serial.cpp
        src/serial0.cpp
        src/i2c.cpp
        src/spi.cpp
        src/analog_pin.cpp
        src/print.cpp
        src/stream.cpp
        src/buffered_stream.cpp
        src/mem_mapper.cpp
        src/utils.cpp
        src/version.cpp
)

# ---- I2C sensor drivers (platform-independent, uses Chip_I2C_* API) ----
set(SBLIB_SENSOR_SRC
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
        src/sensors/dht.cpp
        src/sensors/ds18x20.cpp
        src/onewire.cpp
)

# ---- Combined source list for RP2350 ----
set(SBLIB_RP2350_SRC
        ${SBLIB_HAL_HEADERS}
        ${SBLIB_HAL_RP2350_SRC}
        ${SBLIB_BUS_INTERFACE_SRC}
        ${SBLIB_KNX_PORTABLE_SRC}
        ${SBLIB_UTIL_PORTABLE_SRC}
        ${SBLIB_SENSOR_SRC}
)
