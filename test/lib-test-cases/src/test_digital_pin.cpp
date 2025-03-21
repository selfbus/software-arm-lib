/*
 * Tests for the digital_pin.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include <catch.hpp>
#include <sblib/digital_pin.h>
#include "test_ioports.h"
#include "test_digital_pin.h"


void printPinMask()
{
    for (const PortPinInfo& pin : allPins)
    {
        printf("REQUIRE(digitalPinToBitMask(%s) == 0x%.8x);\n", pin.name, digitalPinToBitMask(pin.pin));
    }
    printf("\n");
}

TEST_CASE("digitalPinToPort", "[digital_pin]")
{
    for (const PortPinInfo& pin :  allPins)
    {
        uint8_t portNumber = digitalPinToPort(pin.pin);
        REQUIRE(portNumber == pin.portNumber);
    }
}

TEST_CASE("digitalPinToPinNum", "[digital_pin]")
{
    for (const PortPinInfo& pin :  allPins)
    {
        uint8_t pinNumber = digitalPinToPinNum(pin.pin);
        REQUIRE(pinNumber == pin.pinNumber);
    }
}

TEST_CASE("digitalPinToBitMask", "[digital_pin]")
{
    //printPinMask();
    // Port 0
    REQUIRE(digitalPinToBitMask(PIO0_0) == 0x00000001);
    REQUIRE(digitalPinToBitMask(PIO0_1) == 0x00000002);
    REQUIRE(digitalPinToBitMask(PIO0_2) == 0x00000004);
    REQUIRE(digitalPinToBitMask(PIO0_3) == 0x00000008);
    REQUIRE(digitalPinToBitMask(PIO0_4) == 0x00000010);
    REQUIRE(digitalPinToBitMask(PIO0_5) == 0x00000020);
    REQUIRE(digitalPinToBitMask(PIO0_6) == 0x00000040);
    REQUIRE(digitalPinToBitMask(PIO0_7) == 0x00000080);
    REQUIRE(digitalPinToBitMask(PIO0_8) == 0x00000100);
    REQUIRE(digitalPinToBitMask(PIO0_9) == 0x00000200);
    REQUIRE(digitalPinToBitMask(PIO0_10) == 0x00000400);
    REQUIRE(digitalPinToBitMask(PIO0_11) == 0x00000800);

    // Port 1
    REQUIRE(digitalPinToBitMask(PIO1_0) == 0x00000001);
    REQUIRE(digitalPinToBitMask(PIO1_1) == 0x00000002);
    REQUIRE(digitalPinToBitMask(PIO1_2) == 0x00000004);
    REQUIRE(digitalPinToBitMask(PIO1_3) == 0x00000008);
    REQUIRE(digitalPinToBitMask(PIO1_4) == 0x00000010);
    REQUIRE(digitalPinToBitMask(PIO1_5) == 0x00000020);
    REQUIRE(digitalPinToBitMask(PIO1_6) == 0x00000040);
    REQUIRE(digitalPinToBitMask(PIO1_7) == 0x00000080);
    REQUIRE(digitalPinToBitMask(PIO1_8) == 0x00000100);
    REQUIRE(digitalPinToBitMask(PIO1_9) == 0x00000200);
    REQUIRE(digitalPinToBitMask(PIO1_10) == 0x00000400);
    REQUIRE(digitalPinToBitMask(PIO1_11) == 0x00000800);

    // Port 2
    REQUIRE(digitalPinToBitMask(PIO2_0) == 0x00000001);
    REQUIRE(digitalPinToBitMask(PIO2_1) == 0x00000002);
    REQUIRE(digitalPinToBitMask(PIO2_2) == 0x00000004);
    REQUIRE(digitalPinToBitMask(PIO2_3) == 0x00000008);
    REQUIRE(digitalPinToBitMask(PIO2_4) == 0x00000010);
    REQUIRE(digitalPinToBitMask(PIO2_5) == 0x00000020);
    REQUIRE(digitalPinToBitMask(PIO2_6) == 0x00000040);
    REQUIRE(digitalPinToBitMask(PIO2_7) == 0x00000080);
    REQUIRE(digitalPinToBitMask(PIO2_8) == 0x00000100);
    REQUIRE(digitalPinToBitMask(PIO2_9) == 0x00000200);
    REQUIRE(digitalPinToBitMask(PIO2_10) == 0x00000400);
    REQUIRE(digitalPinToBitMask(PIO2_11) == 0x00000800);

    // Port 3
    REQUIRE(digitalPinToBitMask(PIO3_0) == 0x00000001);
    REQUIRE(digitalPinToBitMask(PIO3_1) == 0x00000002);
    REQUIRE(digitalPinToBitMask(PIO3_2) == 0x00000004);
    REQUIRE(digitalPinToBitMask(PIO3_3) == 0x00000008);
    REQUIRE(digitalPinToBitMask(PIO3_4) == 0x00000010);
    REQUIRE(digitalPinToBitMask(PIO3_5) == 0x00000020);
}

void resetRegisters(PortPin pin)
{
    LPC_GPIO_TypeDef* port = gpioPorts[digitalPinToPort(pin)];
    port->DIR = 0;
}

uint32_t getDigitalPinFunctionNumber(PortPin pin, PinFunc pinFunction)
{
    if ((pin & PFL_ADMODE) == PFL_ADMODE) // If pin has ADC support we need to set ADMODE = Digital functional mode bit 7
    {
        return getPinFunctionNumber(pin, pinFunction) | 0x80;
    }

    return getPinFunctionNumber(pin, pinFunction);
}

struct IOConRegister
{
    uint32_t RESERVED0[1];
    uint32_t SSEL1_LOC;
    uint32_t SCK_LOC;
    uint32_t DSR_LOC;
    uint32_t DCD_LOC;
    uint32_t RI_LOC;
    uint32_t CT16B0_CAP0_LOC;
    uint32_t SCK1_LOC;
    uint32_t MISO1_LOC;
    uint32_t MOSI1_LOC;
    uint32_t CT32B0_CAP0_LOC;
    uint32_t RXD_LOC;
};

void readIOConRegisters(IOConRegister* reg)
{
    reg->RESERVED0[0] = LPC_IOCON->RESERVED0[0];
    reg->SSEL1_LOC = LPC_IOCON->SSEL1_LOC;
    reg->SCK_LOC = LPC_IOCON->SCK_LOC;
    reg->DSR_LOC = LPC_IOCON->DSR_LOC;
    reg->DCD_LOC = LPC_IOCON->DCD_LOC;
    reg->RI_LOC = LPC_IOCON->RI_LOC;
    reg->CT16B0_CAP0_LOC = LPC_IOCON->CT16B0_CAP0_LOC;
    reg->SCK1_LOC = LPC_IOCON->SCK1_LOC;
    reg->MISO1_LOC = LPC_IOCON->MISO1_LOC;
    reg->MOSI1_LOC = LPC_IOCON->MOSI1_LOC;
    reg->CT32B0_CAP0_LOC = LPC_IOCON->CT32B0_CAP0_LOC;
    reg->RXD_LOC = LPC_IOCON->RXD_LOC;
}

void requireReservedRegistersUnchanged(IOConRegister* reg)
{
    REQUIRE(reg->RESERVED0[0] == LPC_IOCON->RESERVED0[0]);
}

void requireSerialRegistersUnchanged(IOConRegister* reg)
{
    REQUIRE(reg->DSR_LOC == LPC_IOCON->DSR_LOC);
    REQUIRE(reg->DCD_LOC == LPC_IOCON->DCD_LOC);
    REQUIRE(reg->RI_LOC == LPC_IOCON->RI_LOC);
    REQUIRE(reg->RXD_LOC == LPC_IOCON->RXD_LOC);
}

void requireAllIOConRegistersUnchanged(IOConRegister* reg)
{
    requireReservedRegistersUnchanged(reg);
    requireSerialRegistersUnchanged(reg);
    REQUIRE(reg->SSEL1_LOC == LPC_IOCON->SSEL1_LOC);
    REQUIRE(reg->SCK_LOC == LPC_IOCON->SCK_LOC);
    REQUIRE(reg->CT16B0_CAP0_LOC == LPC_IOCON->CT16B0_CAP0_LOC);
    REQUIRE(reg->SCK1_LOC == LPC_IOCON->SCK1_LOC);
    REQUIRE(reg->MISO1_LOC == LPC_IOCON->MISO1_LOC);
    REQUIRE(reg->MOSI1_LOC == LPC_IOCON->MOSI1_LOC);
    REQUIRE(reg->CT32B0_CAP0_LOC == LPC_IOCON->CT32B0_CAP0_LOC);
}

TEST_CASE("pinMode(pin, OUTPUT) / pinMode(pin, OUTPUT_MATCH)", "[digital_pin]")
{
    for (const PortPinInfo& pin : allPins)
    {
        IOConRegister savedRegister;
        readIOConRegisters(&savedRegister);

        const LPC_GPIO_TypeDef* port = gpioPorts[digitalPinToPort(pin.pin)];
        const uint32_t* iocon = ioconPointer(pin.pin);
        const uint32_t bitOutMask = 1 << digitalPinToPinNum(pin.pin);

        uint32_t pinPIOfunctionNumber = getDigitalPinFunctionNumber(pin.pin, PF_PIO);

        // Set pin mode to output
        pinMode(pin.pin, OUTPUT);
        REQUIRE((port->DIR & bitOutMask) == bitOutMask); // port direction bit for pin number set

        REQUIRE(*iocon == pinPIOfunctionNumber); // IOCON_PIO_x_y set to correct pin function
        requireAllIOConRegistersUnchanged(&savedRegister);

        pinMode(pin.pin, INPUT);  // set to input
        pinMode(pin.pin, OUTPUT); // and again to output
        REQUIRE((port->DIR & bitOutMask) == bitOutMask); // port direction bit still set?

        if (getPinFunctionNumber(pin.pin, PF_MAT) < 0)
        {
            ///\todo it would be better, if pinMode had a return error value with [[nounused]]
            continue; // pin doesn´t support output match, so continue
        }

        // Set pin mode to output match
        pinMode(pin.pin, OUTPUT_MATCH);
        REQUIRE((port->DIR & bitOutMask) == bitOutMask); // port direction bit for pin number set

        pinPIOfunctionNumber = getDigitalPinFunctionNumber(pin.pin, PF_MAT);

        REQUIRE(*iocon == pinPIOfunctionNumber); // IOCON_PIO_x_y set to correct pin function
        requireAllIOConRegistersUnchanged(&savedRegister);
    }
}

TEST_CASE("pinMode(pin, INPUT)", "[digital_pin]")
{
    for (const PortPinInfo& pin : allPins)
    {
        IOConRegister savedRegister;
        readIOConRegisters(&savedRegister);

        const LPC_GPIO_TypeDef* port = gpioPorts[digitalPinToPort(pin.pin)];
        const uint32_t* iocon = ioconPointer(pin.pin);
        const uint32_t bitOutMask = 1 << digitalPinToPinNum(pin.pin);

        uint32_t pinPIOfunctionNumber = getDigitalPinFunctionNumber(pin.pin, PF_PIO);

        pinMode(pin.pin, INPUT); // Set pin mode to input
        REQUIRE((port->DIR & bitOutMask) == 0); // port direction bit for pin number NOT set

        REQUIRE(*iocon == pinPIOfunctionNumber); // IOCON_PIO_x_y set to correct pin function
        requireAllIOConRegistersUnchanged(&savedRegister);

        pinMode(pin.pin, OUTPUT); // set to output
        pinMode(pin.pin, INPUT);  // and again to input
        REQUIRE((port->DIR & bitOutMask) == 0); // port direction bit still NOT set?
        requireAllIOConRegistersUnchanged(&savedRegister);
    }
}

TEST_CASE("pinMode(pin, INPUT_CAPTURE)", "[digital_pin]")
{
    // we run this test twice
    // to check correct changes in CT16B0_CAP0_LOC and CT32B0_CAP0_LOC
    for (auto i = 0; i < 2; i++)
    {
        for (const PortPinInfo& pin : allPins)
        {
            if (getPinFunctionNumber(pin.pin, PF_CAP) < 0)
            {
                ///\todo it would be better, if pinMode had a return error value with [[nounused]]
                continue; // pin doesn´t support INPUT_CAPTURE, so continue
            }

            IOConRegister savedRegister;
            readIOConRegisters(&savedRegister);

            const LPC_GPIO_TypeDef* port = gpioPorts[digitalPinToPort(pin.pin)];
            const uint32_t* iocon = ioconPointer(pin.pin);
            const uint32_t bitOutMask = 1 << digitalPinToPinNum(pin.pin);

            uint32_t pinPIOfunctionNumber = getDigitalPinFunctionNumber(pin.pin, PF_CAP);

            pinMode(pin.pin, INPUT_CAPTURE); // Set pin mode to input capture
            REQUIRE((port->DIR & bitOutMask) == 0); // port direction bit for pin number NOT set

            REQUIRE(*iocon == pinPIOfunctionNumber); // IOCON_PIO_x_y set to correct pin function

            switch (pin.pin)
            {
                case PIO0_2:
                    REQUIRE(LPC_IOCON->CT16B0_CAP0_LOC == 0);
                    savedRegister.CT16B0_CAP0_LOC = 0;
                    break;
                case PIO1_0: // CT32B1_CAP0, has no location register
                    break;
                case PIO1_5:
                    REQUIRE(LPC_IOCON->CT32B0_CAP0_LOC == 0);
                    savedRegister.CT32B0_CAP0_LOC = 0;
                    break;
                case PIO1_8: // CT16B1_CAP0, has no location register
                    break;
                case PIO1_11: // CT32B1_CAP1, has no location register
                    break;
                case PIO2_9:
                    REQUIRE(LPC_IOCON->CT32B0_CAP0_LOC == 1);
                    savedRegister.CT32B0_CAP0_LOC = 1;
                    break;
                case PIO2_11: // CT32B0_CAP1, has no location register
                    break;
                case PIO3_3:
                    REQUIRE(LPC_IOCON->CT16B0_CAP0_LOC == 1);
                    savedRegister.CT16B0_CAP0_LOC = 1;
                    break;
                case PIO3_4: // CT16B0_CAP1, has no location register
                    break;
                case PIO3_5: // CT16B1_CAP1, has no location register
                    break;
                default:
                    FAIL("Unknown pin for INPUT_CAPTURE.");
                    break;
            }

            requireAllIOConRegistersUnchanged(&savedRegister);
        }
    }
}

TEST_CASE("pinMode(pin, INPUT_ANALOG)", "[digital_pin]")
{
    for (const PortPinInfo& pin : allPins)
    {
        if (getPinFunctionNumber(pin.pin, PF_AD) < 0)
        {
            ///\todo it would be better, if pinMode had a return error value with [[nounused]]
            continue; // pin doesn´t support INPUT_ANALOG, so continue
        }
        IOConRegister savedRegister;
        readIOConRegisters(&savedRegister);

        const LPC_GPIO_TypeDef* port = gpioPorts[digitalPinToPort(pin.pin)];
        const uint32_t* iocon = ioconPointer(pin.pin);
        const uint32_t bitOutMask = 1 << digitalPinToPinNum(pin.pin);

        uint32_t pinPIOfunctionNumber = getPinFunctionNumber(pin.pin, PF_AD);

        pinMode(pin.pin, INPUT_ANALOG);         // Set pin mode to input
        REQUIRE((port->DIR & bitOutMask) == 0); // port direction bit for pin number NOT set

        REQUIRE(*iocon == pinPIOfunctionNumber); // IOCON_PIO_x_y set to correct pin function
        requireAllIOConRegistersUnchanged(&savedRegister);
    }
}

TEST_CASE("pinMode(pin, SERIAL_RXD)", "[digital_pin]")
{
    for (const PortPinInfo& pin : allPins)
    {
        if (getPinFunctionNumber(pin.pin, PF_RXD) < 0)
        {
            ///\todo it would be better, if pinMode had a return error value with [[nounused]]
            continue; // pin doesn´t support PF_RXD, so continue
        }

        IOConRegister savedRegister;
        readIOConRegisters(&savedRegister);

        const LPC_GPIO_TypeDef* port = gpioPorts[digitalPinToPort(pin.pin)];
        const uint32_t* iocon = ioconPointer(pin.pin);
        const uint32_t bitOutMask = 1 << digitalPinToPinNum(pin.pin);

        uint32_t pinPIOfunctionNumber = getDigitalPinFunctionNumber(pin.pin, PF_RXD);

        pinMode(pin.pin, SERIAL_RXD); // Set pin mode to input capture
        REQUIRE((port->DIR & bitOutMask) == 0); // port direction bit for pin number NOT set

        REQUIRE(*iocon == pinPIOfunctionNumber); // IOCON_PIO_x_y set to correct pin function

        switch (pin.pin)
        {
            case PIO1_6:
                REQUIRE(LPC_IOCON->RXD_LOC == 0);
                savedRegister.RXD_LOC = 0;
                break;
            case PIO2_7:
                REQUIRE(LPC_IOCON->RXD_LOC == 1);
                savedRegister.RXD_LOC = 1;
                break;
            case PIO3_1:
                REQUIRE(LPC_IOCON->RXD_LOC == 2);
                savedRegister.RXD_LOC = 2;
                break;
            case PIO3_4:
                REQUIRE(LPC_IOCON->RXD_LOC == 3);
                savedRegister.RXD_LOC = 3;
                break;
            default:
                FAIL("Unknown pin for SERIAL_RXD.");
                break;
        }

        requireAllIOConRegistersUnchanged(&savedRegister);
    }
}

TEST_CASE("pinMode(pin, SPI_MISO)", "[digital_pin]")
{
    // we run this test twice
    // to check correct changes in MISO1_LOC
    for (auto i = 0; i < 2; i++)
    {
        for (const PortPinInfo& pin : allPins)
        {
            if (getPinFunctionNumber(pin.pin, PF_MISO) < 0)
            {
                ///\todo it would be better, if pinMode had a return error value with [[nounused]]
                continue; // pin doesn´t support PF_MISO, so continue
            }

            IOConRegister savedRegister;
            readIOConRegisters(&savedRegister);

            const LPC_GPIO_TypeDef* port = gpioPorts[digitalPinToPort(pin.pin)];
            const uint32_t* iocon = ioconPointer(pin.pin);
            const uint32_t bitOutMask = 1 << digitalPinToPinNum(pin.pin);

            uint32_t pinPIOfunctionNumber = getDigitalPinFunctionNumber(pin.pin, PF_MISO);

            pinMode(pin.pin, SPI_MISO); // Set pin mode to input capture
            REQUIRE((port->DIR & bitOutMask) == 0); // port direction bit for pin number NOT set

            REQUIRE(*iocon == pinPIOfunctionNumber); // IOCON_PIO_x_y set to correct pin function

            switch (pin.pin)
            {
                case PIO0_8: // has no location register
                    break;
                case PIO1_10:
                    REQUIRE(LPC_IOCON->MISO1_LOC == 1);
                    savedRegister.MISO1_LOC = 1;
                    break;
                case PIO2_2:
                    REQUIRE(LPC_IOCON->MISO1_LOC == 0);
                    savedRegister.MISO1_LOC = 0;
                    break;

                default:
                    FAIL("Unknown pin for SPI_MISO.");
                    break;
            }

            requireAllIOConRegistersUnchanged(&savedRegister);
        }
    }
}

TEST_CASE("pinMode(pin, SPI_MOSI)", "[digital_pin]")
{
    // we run this test twice
    // to check correct changes in MOSI1_LOC
    for (auto i = 0; i < 2; i++)
    {
        for (const PortPinInfo& pin : allPins)
        {
            if (getPinFunctionNumber(pin.pin, PF_MOSI) < 0)
            {
                ///\todo it would be better, if pinMode had a return error value with [[nounused]]
                continue; // pin doesn´t support PF_MISO, so continue
            }

            IOConRegister savedRegister;
            readIOConRegisters(&savedRegister);

            const LPC_GPIO_TypeDef* port = gpioPorts[digitalPinToPort(pin.pin)];
            const uint32_t* iocon = ioconPointer(pin.pin);
            const uint32_t bitOutMask = 1 << digitalPinToPinNum(pin.pin);

            uint32_t pinPIOfunctionNumber = getDigitalPinFunctionNumber(pin.pin, PF_MOSI);

            pinMode(pin.pin, SPI_MOSI); // Set pin mode to input capture
            REQUIRE((port->DIR & bitOutMask) == 0); // port direction bit for pin number NOT set

            REQUIRE(*iocon == pinPIOfunctionNumber); // IOCON_PIO_x_y set to correct pin function

            switch (pin.pin)
            {
                case PIO0_9: // has no location register
                    break;
                case PIO1_9:
                    REQUIRE(LPC_IOCON->MOSI1_LOC == 1);
                    savedRegister.MOSI1_LOC = 1;
                    break;
                case PIO2_3:
                    REQUIRE(LPC_IOCON->MOSI1_LOC == 0);
                    savedRegister.MOSI1_LOC = 0;
                    break;

                default:
                    FAIL("Unknown pin for SPI_MOSI.");
                    break;
            }

            requireAllIOConRegistersUnchanged(&savedRegister);
        }
    }
}

TEST_CASE("pinMode(pin, SPI_CLOCK)", "[digital_pin]")
{
    for (const PortPinInfo& pin : allPins)
    {
        if (getPinFunctionNumber(pin.pin, PF_SCK) < 0)
        {
            ///\todo it would be better, if pinMode had a return error value with [[nounused]]
            continue; // pin doesn´t support PF_SCK, so continue
        }

        IOConRegister savedRegister;
        readIOConRegisters(&savedRegister);

        const LPC_GPIO_TypeDef* port = gpioPorts[digitalPinToPort(pin.pin)];
        const uint32_t* iocon = ioconPointer(pin.pin);
        const uint32_t bitOutMask = 1 << digitalPinToPinNum(pin.pin);

        uint32_t pinPIOfunctionNumber = getDigitalPinFunctionNumber(pin.pin, PF_SCK);

        pinMode(pin.pin, SPI_CLOCK); // Set pin mode to input capture
        REQUIRE((port->DIR & bitOutMask) == 0); // port direction bit for pin number NOT set

        REQUIRE(*iocon == pinPIOfunctionNumber); // IOCON_PIO_x_y set to correct pin function

        switch (pin.pin)
        {
            case PIO0_6:
                REQUIRE(LPC_IOCON->SCK_LOC == 2);
                savedRegister.SCK_LOC = 2;
                break;
            case PIO0_10:
                REQUIRE(LPC_IOCON->SCK_LOC == 0);
                savedRegister.SCK_LOC = 0;
                break;
            case PIO2_1:
                REQUIRE(LPC_IOCON->SCK1_LOC == 0);
                savedRegister.SCK1_LOC = 0;
                break;
            case PIO2_11:
                REQUIRE(LPC_IOCON->SCK_LOC == 1);
                savedRegister.SCK_LOC = 1;
                break;
            case PIO3_2:
                REQUIRE(LPC_IOCON->SCK1_LOC == 1);
                savedRegister.SCK1_LOC = 1;
                break;
            default:
                FAIL("Unknown pin for SPI_CLOCK.");
                break;
        }

        requireAllIOConRegistersUnchanged(&savedRegister);
    }
}

TEST_CASE("pinMode(pin, SPI_SSEL)", "[digital_pin]")
{
    for (const PortPinInfo& pin : allPins)
    {
        if (getPinFunctionNumber(pin.pin, PF_SSEL) < 0)
        {
            ///\todo it would be better, if pinMode had a return error value with [[nounused]]
            continue; // pin doesn´t support PF_SSEL, so continue
        }

        IOConRegister savedRegister;
        readIOConRegisters(&savedRegister);

        const LPC_GPIO_TypeDef* port = gpioPorts[digitalPinToPort(pin.pin)];
        const uint32_t* iocon = ioconPointer(pin.pin);
        const uint32_t bitOutMask = 1 << digitalPinToPinNum(pin.pin);

        uint32_t pinPIOfunctionNumber = getDigitalPinFunctionNumber(pin.pin, PF_SSEL);

        pinMode(pin.pin, SPI_SSEL); // Set pin mode to input capture
        REQUIRE((port->DIR & bitOutMask) == 0); // port direction bit for pin number NOT set

        REQUIRE(*iocon == pinPIOfunctionNumber); // IOCON_PIO_x_y set to correct pin function

        switch (pin.pin)
        {
            case PIO0_2: // has no location register
                break;
            case PIO2_0:
                REQUIRE(LPC_IOCON->SSEL1_LOC == 0);
                savedRegister.SSEL1_LOC = 0;
                break;
            case PIO2_4:
                REQUIRE(LPC_IOCON->SSEL1_LOC == 1);
                savedRegister.SSEL1_LOC = 1;
                break;
            default:
                FAIL("Unknown pin for SPI_SSEL.");
                break;
        }

        requireAllIOConRegistersUnchanged(&savedRegister);
    }
}

TEST_CASE("pinMode(pin, INPUT | ...)", "[digital_pin]")
{
    ///\todo OPEN_DRAIN, HYSTERESIS, PULL_DOWN, PULL_UP
}

