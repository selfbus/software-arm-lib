/*
 *  gpio_rp2350.h - GPIO pin definitions for RP2350 target boards.
 *
 *  These are default pin assignments. Override in your application's
 *  board header or CMakeLists.txt as needed.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#ifndef SBLIB_GPIO_RP2350_H_
#define SBLIB_GPIO_RP2350_H_

// On RP2350, pins are simple integers (GPIO 0..47).
// No port encoding needed.

// KNX bus interface pins — connect to KNX transceiver (e.g. TJA1021, discrete circuit)
#ifndef PIN_EIB_TX
#define PIN_EIB_TX  2   // GPIO for TX (active-low pulse to transceiver)
#endif

#ifndef PIN_EIB_RX
#define PIN_EIB_RX  3   // GPIO for RX (falling edge from transceiver)
#endif

// Programming button and LED
#ifndef PIN_PROG
#define PIN_PROG    0   // GPIO for programming mode button/LED
#endif

// VBUS detect (if applicable)
#ifndef PIN_VBUS
#define PIN_VBUS    0   // Not used by default on RP2350
#endif

// Info LED
#ifndef PIN_INFO
#define PIN_INFO    25  // On-board LED on Pico 2 (GPIO 25)
#endif

// Run pin (active-low, directly on module; stub for API compat)
#ifndef PIN_RUN
#define PIN_RUN     PIN_INFO  // No separate RUN LED on standard Pico 2
#endif

// Application I/O pins — remapped for RP2350
#ifndef PIN_IO1
#define PIN_IO1     4
#endif
#ifndef PIN_IO2
#define PIN_IO2     5
#endif
#ifndef PIN_IO3
#define PIN_IO3     6
#endif
#ifndef PIN_IO4
#define PIN_IO4     7
#endif

// I2C pins
#ifndef PIO_SCL
#define PIO_SCL     9
#endif
#ifndef PIO_SDA
#define PIO_SDA     8
#endif

// UART pins
#ifndef PIN_TX
#define PIN_TX      0   // UART0 TX
#endif
#ifndef PIN_RX
#define PIN_RX      1   // UART0 RX
#endif

// SPI pins (SPI0)
#ifndef PIN_SCK0
#define PIN_SCK0    18
#endif
#ifndef PIN_MISO0
#define PIN_MISO0   16
#endif
#ifndef PIN_MOSI0
#define PIN_MOSI0   19
#endif
#ifndef PIN_SSEL0
#define PIN_SSEL0   17
#endif

#endif /* SBLIB_GPIO_RP2350_H_ */
