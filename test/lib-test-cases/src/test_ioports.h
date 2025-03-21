/*
 * Helpers for ioports.h test cases
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef TEST_SBLIB_IOPORTS_H_
#define TEST_SBLIB_IOPORTS_H_

#include <vector>
#include <sblib/ioports.h>


/**
 * Pairs a @ref PortPin with its printable name.
 */
struct PortPinInfo {
    PortPin pin;
    uint8_t portNumber;
    uint8_t pinNumber;
    char name[20];
};

/**
 * Pairs a @ref PinFunc with its printable name.
 */
struct PinFunctionInfo {
    PinFunc pinFunction;
    char name[30];
};

/**
 * Contains all available @ref PortPin with their port number, pin number and printable names.
 */
static const std::vector<PortPinInfo> allPins = {
    // Port 0
    {PIO0_0, 0, 0, "PIO0_0"}, {PIO0_1, 0, 1, "PIO0_1"}, {PIO0_2, 0, 2, "PIO0_2"}, {PIO0_3, 0, 3, "PIO0_3"}, {PIO0_4, 0, 4, "PIO0_4"},
    {PIO0_5, 0, 5, "PIO0_5"}, {PIO0_6, 0, 6, "PIO0_6"}, {PIO0_7, 0, 7, "PIO0_7"}, {PIO0_8, 0, 8, "PIO0_8"}, {PIO0_9, 0, 9, "PIO0_9"},
    {PIO0_10, 0, 10, "PIO0_10"}, {PIO0_11, 0, 11, "PIO0_11"},

    // Port 1
    {PIO1_0, 1, 0, "PIO1_0"}, {PIO1_1, 1, 1, "PIO1_1"}, {PIO1_2, 1, 2, "PIO1_2"}, {PIO1_3, 1, 3, "PIO1_3"}, {PIO1_4, 1, 4, "PIO1_4"},
    {PIO1_5, 1, 5, "PIO1_5"}, {PIO1_6, 1, 6, "PIO1_6"}, {PIO1_7, 1, 7, "PIO1_7"}, {PIO1_8, 1, 8, "PIO1_8"}, {PIO1_9, 1, 9, "PIO1_9"},
    {PIO1_10, 1, 10, "PIO1_10"}, {PIO1_11, 1, 11, "PIO1_11"},

    // Port 2
    {PIO2_0, 2, 0, "PIO2_0"}, {PIO2_1, 2, 1, "PIO2_1"}, {PIO2_2, 2, 2, "PIO2_2"}, {PIO2_3, 2, 3, "PIO2_3"}, {PIO2_4, 2, 4, "PIO2_4"},
    {PIO2_5, 2, 5, "PIO2_5"}, {PIO2_6, 2, 6, "PIO2_6"}, {PIO2_7, 2, 7, "PIO2_7"}, {PIO2_8, 2, 8, "PIO2_8"}, {PIO2_9, 2, 9, "PIO2_9"},
    {PIO2_10, 2, 10, "PIO2_10"}, {PIO2_11, 2, 11, "PIO2_11"},

    // Port 3
    {PIO3_0, 3, 0, "PIO3_0"}, {PIO3_1, 3, 1, "PIO3_1"}, {PIO3_2, 3, 2, "PIO3_2"}, {PIO3_3, 3, 3, "PIO3_3"}, {PIO3_4, 3, 4, "PIO3_4"},
    {PIO3_5, 3, 5, "PIO3_5"}
};

/**
 * Contains all available @ref PinFunc and their printable names.
 */
static const std::vector<PinFunctionInfo> allPinFunctions = {
        {PF_NONE, "PF_NONE"},
        {PF_PIO, "PF_PIO"},
        {PF_AD, "PF_AD"},
        {PF_RESET, "PF_RESET"},
        {PF_SWDIO, "PF_SWDIO"},
        {PF_SWCLK, "PF_SWCLK"},
        {PF_MAT, "PF_MAT"},
        {PF_CAP, "PF_CAP"},
        {PF_CLKOUT, "PF_CLKOUT"},
        {PF_SDA, "PF_SDA"},
        {PF_SCL, "PF_SCL"},
        {PF_SSEL, "PF_SSEL"},
        {PF_MISO, "PF_MISO"},
        {PF_MOSI, "PF_MOSI"},
        {PF_SCK, "PF_SCK"},
        {PF_RXD,"PF_RXD"},
        {PF_TXD, "PF_TXD"},
        {PF_RTS, "PF_RTS"},
        {PF_DTR, "PF_DTR"},
        {PF_DSR, "PF_DSR"},
        {PF_CTS, "PF_CTS"},
        {PF_DCD, "PF_DCD"},
        {PF_RI, "PF_RI"},
        {PF_USBP, "PF_USBP"},
        {PF_USB_M, "PF_USB_M"},
        {PF_USB_VBUS, "PF_USB_VBUS"},
        {PF_USB_CONNECT, "PF_USB_CONNECT"},
        {PF_USB_FTOGGLE, "PF_USB_FTOGGLE"},
        {PF_ARM_TRACE_CLK, "PF_ARM_TRACE_CLK"},
        {PF_ARM_TRACE_SWV, "PF_ARM_TRACE_SWV"},
        {PF_SCLK, "PF_SCLK"}
};

#endif /* TEST_SBLIB_IOPORTS_H_ */
