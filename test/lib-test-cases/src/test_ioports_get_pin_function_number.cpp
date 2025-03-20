/*
 * Test for function getPinFunctionNumber(..) in ioports.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include <catch.hpp>
#include <cstdio>
#include <sblib/ioports.h>

#include "test_ioports.h"

/**
 * Prints all REQUIRE(..) for @ref getPinFunctionNumber
 */
void printPinFunctionNumber()
{
    for (const PortPinInfo& pin : allPins)
    {
        for (const PinFunctionInfo& functionInfo : allPinFunctions)
        {
            int32_t result = getPinFunctionNumber(pin.pin, functionInfo.pinFunction);
            printf("REQUIRE(getPinFunctionNumber(%s, %s) == %d);", pin.name, functionInfo.name, result);
            if (result != -1)
            {
                printf(" // FUNC%d", result);
            }
            printf("\n");
        }
    }
}

TEST_CASE("getPinFunctionNumber", "[ioports]")
{
    //printPinFunctionNumber();
    ///\todo Bug in getPinFunctionNumber(PIOx_y, PF_NONE) should return -1 for all gpios
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_NONE) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_PIO) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_RESET) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_0, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_MAT) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_CLKOUT) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_1, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_CAP) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_SSEL) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_2, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_NONE) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_3, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_NONE) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_SCL) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_4, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_NONE) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_SDA) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_5, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_NONE) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_SCK) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_6, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_NONE) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_CTS) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_7, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_MAT) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_MISO) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_8, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_MAT) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_MOSI) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_9, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_NONE) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_PIO) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_SWCLK) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_MAT) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_SCK) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_10, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_NONE) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_PIO) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_AD) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_MAT) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO0_11, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_NONE) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_PIO) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_AD) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_CAP) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_0, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_NONE) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_PIO) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_AD) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_MAT) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_1, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_NONE) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_PIO) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_AD) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_MAT) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_2, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_NONE) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_PIO) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_AD) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_SWDIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_MAT) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_3, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_AD) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_MAT) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_4, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_CAP) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_RTS) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_5, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_MAT) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_RXD) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_6, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_MAT) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_TXD) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_7, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_NONE) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_CAP) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_8, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_MAT) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_MOSI) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_9, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_NONE) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_AD) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_MAT) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_MISO) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_10, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_AD) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_CAP) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO1_11, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_SSEL) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_DTR) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_0, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_SCK) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_DSR) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_1, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_MISO) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_DCD) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_2, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_MOSI) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_RI) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_3, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_MAT) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_SSEL) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_4, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_NONE) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_MAT) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_5, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_NONE) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_MAT) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_6, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_MAT) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_RXD) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_7, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_MAT) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_TXD) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_8, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_NONE) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_CAP) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_9, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_NONE) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_10, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_CAP) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_SCK) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO2_11, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_NONE) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_MAT) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_TXD) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_DTR) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_0, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_NONE) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_MAT) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_RXD) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_DSR) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_1, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_NONE) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_MAT) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_CAP) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_SCK) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_DCD) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_2, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_CAP) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_RI) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_3, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_CAP) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_RXD) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_TXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_4, PF_SCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_NONE) == 3); // FUNC3
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_PIO) == 0); // FUNC0
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_AD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_RESET) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_SWDIO) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_SWCLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_MAT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_CAP) == 1); // FUNC1
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_CLKOUT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_SDA) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_SCL) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_SSEL) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_MISO) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_MOSI) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_SCK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_RXD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_TXD) == 2); // FUNC2
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_RTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_DTR) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_DSR) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_CTS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_DCD) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_RI) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_USBP) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_USB_M) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_USB_VBUS) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_USB_CONNECT) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_USB_FTOGGLE) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_ARM_TRACE_CLK) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_ARM_TRACE_SWV) == -1);
    REQUIRE(getPinFunctionNumber(PIO3_5, PF_SCLK) == -1);
}
