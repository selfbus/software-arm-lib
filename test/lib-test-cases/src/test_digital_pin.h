/*
 * Helper for digital_pin.h test cases
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#ifndef TEST_SBLIB_DIGITAL_PIN_H_
#define TEST_SBLIB_DIGITAL_PIN_H_

#include <vector>
#include <sblib/digital_pin.h>

/**
 * Pairs a @ref PinMode with its printable name.
 */
struct PinModeInfo {
    PinMode mode;
    char name[20];
};

/**
 * Contains all available @ref PinMode and their printable names.
 */
static const std::vector<PinModeInfo> allPinModes = {
    {INPUT, "INPUT"},
    {INPUT_CAPTURE, "INPUT_CAPTURE"},
    {INPUT_ANALOG, "INPUT_ANALOG"},
    {PULL_UP, "PULL_UP"},
    {PULL_DOWN, "PULL_DOWN"},
    {HYSTERESIS, "HYSTERESIS"},
    {OUTPUT, "OUTPUT"},
    {OUTPUT_MATCH, "OUTPUT_MATCH"},
    {OPEN_DRAIN, "OPEN_DRAIN"},
    {REPEATER_MODE, "REPEATER_MODE"},
    {SERIAL_RXD, "SERIAL_RXD"},
    {SERIAL_TXD, "SERIAL_TXD"},
    {SPI_CLOCK, "SPI_CLOCK"},
    {SPI_MISO, "SPI_MISO"},
    {SPI_MOSI, "SPI_MOSI"},
    {SPI_SSEL, "SPI_SSEL"}
};

#endif /* TEST_SBLIB_DIGITAL_PIN_H_ */
