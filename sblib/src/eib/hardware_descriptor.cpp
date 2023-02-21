/**************************************************************************//**
 * @addtogroup
 * @defgroup
 * @ingroup
 * @brief
 * @details
 *
 * @{
 *
 * @file   hardware_descriptor.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 -----------------------------------------------------------------------------*/

#include <sblib/hardware_descriptor.h>
#include <sblib/io_pin_names.h>

struct EIB
{
    uint32_t pinTx;             //!<
    uint32_t pinRx;             //!<
    uint32_t pinProgButton;     //!<
    uint32_t pinBusVoltage;     //!<
    uint8_t invertProgButton;   //!< 1 if programming button is inverted, otherwise 0
};

struct EepromI2C
{
    uint32_t pinData;
    uint32_t pinClock;
    uint8_t address;   //!< i2c address of a possible installed eeprom-chip, otherwise 0
};

struct EepromSPI
{
    uint32_t pinClock;
    uint32_t pinMISO;
    uint32_t pinMOSI;
    uint32_t pinChipSelect;
};

struct HardwareDescriptor
{
    uint32_t guid;              //!< unique identifier of the hardware descriptor
    uint8_t size;               //!< size of the hardware descriptor
    uint16_t version;           //!< version of the hardware descriptor
    uint16_t hardwareID;        //!< unique identifier of the hardware pcb
    uint16_t hardwareVersion;   //!<
    uint32_t options;           //!<
    EIB eib;                    //!<
    uint32_t pinInfo;           //!<
    uint32_t pinRun;            //!<
    EepromI2C eEpromI2C;        //!<
    EepromSPI eEpromSPI;        //!<
    uint8_t reserved[2];        //!< reserved,
    uint32_t crc32;
};

__attribute__((used, section (".selfbus"), )) const HardwareDescriptor
        selfbusHardwareDescriptor =
        {
            .guid = 0xFEAFDEBA,
            .size = sizeof(selfbusHardwareDescriptor),
            .version = 0x0001,
            .hardwareID = 0xBA55,
            .hardwareVersion = 0x0000,
            .options = 0x12345678,
            .eib = {.pinTx = PIN_EIB_TX, .pinRx = PIN_EIB_RX, .pinProgButton = PIN_PROG, .pinBusVoltage = PIN_VBUS, .invertProgButton = 0},
            .pinInfo = PIN_INFO,
            .pinRun  = PIN_RUN,
            .eEpromI2C = {.pinData = PIO_SDA, .pinClock = PIO_SCL, .address = 0},
            .eEpromSPI = {.pinClock = PIN_SCK0, .pinMISO = PIN_MISO0, .pinMOSI = PIN_MOSI0, .pinChipSelect = PIN_SSEL0},
            .reserved  = {0x30, 0x31},
        };
/*
        selfbusHardwareDescriptor =
        {
            .guid = 0xBADEAFFE,
            .size = sizeof(selfbusHardwareDescriptor),
            .version = 0xD00F,
            .hardwareID = 0xBA55,
            .options = 0x12345678,
            .pinEibTx = 0xCAFEBABE,
            .pinEibRx = 0xDEADC0DE,
            .pinProgButton = 0xABAD1DEA,
            .pinBusVoltage = 0xBADA55,
            .pinInfo = 0xDEADBEAF,
            .pinRun  = 0xBADCAB1E,
            .invertProgButton = 0x29,
            .reserved  = {0x30, 0x31},
        };
*/
uint32_t hwGuid()
{
    return (selfbusHardwareDescriptor.guid);
}

uint8_t hwSize()
{
    return (selfbusHardwareDescriptor.size);
}

uint16_t hwDescriptorVersion()
{
    return (selfbusHardwareDescriptor.version);
}

uint16_t hwHardwareID()
{
    return (selfbusHardwareDescriptor.hardwareID);
}

uint32_t hwOptions()
{
    return (selfbusHardwareDescriptor.options);
}

uint32_t hwPinEibTx()
{
    return (selfbusHardwareDescriptor.eib.pinTx);
}

uint32_t hwPinEibRx()
{
    return (selfbusHardwareDescriptor.eib.pinRx);
}

uint32_t hwPinProgButton()
{
    return (selfbusHardwareDescriptor.eib.pinProgButton);
}

uint32_t hwPinBusVoltage()
{
    return (selfbusHardwareDescriptor.eib.pinBusVoltage);
}

uint32_t hwPinInfo()
{
    return (selfbusHardwareDescriptor.pinInfo);
}

uint32_t hwPinRun()
{
    return (selfbusHardwareDescriptor.pinRun);
}

uint8_t hwInvertProgButton()
{
    return (selfbusHardwareDescriptor.eib.invertProgButton);
}


/** @}*/
