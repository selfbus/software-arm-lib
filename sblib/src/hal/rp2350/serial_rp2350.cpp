/*
 *  serial_rp2350.cpp - Serial port driver for RP2350 using Pico SDK UART.
 *
 *  Implements the sblib Serial class API using UART0 on the RP2350.
 *  Default pins: GP0 (TX), GP1 (RX) — configurable via setTxPin/setRxPin.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#if defined(__SBLIB_TARGET_RP2350__)

#include <sblib/serial.h>
#include <sblib/digital_pin.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

// Map sblib serial port to UART0
static uart_inst_t* const UART_INST = uart0;
static const uint UART_IRQ = UART0_IRQ;

// Default pins (overridable via setTxPin/setRxPin)
static uint uart_tx_gpio = 0;
static uint uart_rx_gpio = 1;

// Forward declaration of ISR
static void uart_isr();

Serial::Serial(int rxPin, int txPin) :
    enabled_(false)
{
    uart_rx_gpio = (uint)rxPin;
    uart_tx_gpio = (uint)txPin;
}

void Serial::setRxPin(int rxPin)
{
    if (enabled()) end();
    uart_rx_gpio = (uint)rxPin;
}

void Serial::setTxPin(int txPin)
{
    if (enabled()) end();
    uart_tx_gpio = (uint)txPin;
}

void Serial::begin(int baudRate, SerialConfig config)
{
    if (enabled()) end();

    uart_init(UART_INST, (uint)baudRate);
    gpio_set_function(uart_tx_gpio, GPIO_FUNC_UART);
    gpio_set_function(uart_rx_gpio, GPIO_FUNC_UART);

    // Parse SerialConfig: bits 0-1 = data bits (0=5..3=8), bit 2 = stop bits, bits 3-4 = parity
    uint dataBits = (config & 0x03) + 5;
    uint stopBits = (config & 0x04) ? 2 : 1;
    uart_parity_t parity = UART_PARITY_NONE;
    if (config & 0x08)
        parity = UART_PARITY_ODD;
    else if (config & 0x10)
        parity = UART_PARITY_EVEN;

    uart_set_format(UART_INST, dataBits, stopBits, parity);
    uart_set_fifo_enabled(UART_INST, true);

    // Enable RX interrupt
    irq_set_exclusive_handler(UART_IRQ, uart_isr);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_INST, true, false);

    clearBuffers();
    enabled_ = true;
}

void Serial::end()
{
    if (!enabled_) return;
    flush();
    uart_set_irq_enables(UART_INST, false, false);
    irq_set_enabled(UART_IRQ, false);
    uart_deinit(UART_INST);
    enabled_ = false;
}

int Serial::write(byte ch)
{
    if (!enabled_) return 0;
    uart_putc_raw(UART_INST, ch);
    return 1;
}

void Serial::flush(void)
{
    if (!enabled_) return;
    uart_tx_wait_blocking(UART_INST);
}

int Serial::read()
{
    if (!enabled_) return -1;
    return BufferedStream::read();
}

void Serial::interruptHandler()
{
    while (uart_is_readable(UART_INST))
    {
        int ch = uart_getc(UART_INST);
        int nextHead = (readHead + 1) & BUFFER_SIZE_MASK;
        if (nextHead != readTail)
        {
            readBuffer[readHead] = (byte)ch;
            readHead = nextHead;
        }
    }
}

static void uart_isr()
{
    serial.interruptHandler();
}

// Default serial instance — GP0=TX, GP1=RX
#if !defined(SERIAL_TX_PIN)
#   define SERIAL_TX_PIN 0
#endif
#if !defined(SERIAL_RX_PIN)
#   define SERIAL_RX_PIN 1
#endif

Serial serial(SERIAL_RX_PIN, SERIAL_TX_PIN);

#endif // __SBLIB_TARGET_RP2350__
