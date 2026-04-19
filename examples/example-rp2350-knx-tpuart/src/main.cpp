/*
 *  main.cpp - KNX device example for RP2350 using TPUART/NCN5120 transceiver.
 *
 *  This demonstrates how to use sblib on an RP2350 with a TPUART-compatible
 *  transceiver (Siemens TPUART/TPUART2, onsemi NCN5120/NCN5130).
 *
 *  The TPUART is connected via UART1:
 *    - GP8: TX  (to TPUART RX)
 *    - GP9: RX  (from TPUART TX)
 *
 *  TPUART protocol: 19200 baud, 8E1
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

#include <sblib/eib/knx_bus_tpuart.h>
#include <sblib/digital_pin.h>
#include <sblib/timer.h>

/* ---- UART pin configuration ---- */
#define TPUART_UART      uart1
#define TPUART_TX_PIN    8
#define TPUART_RX_PIN    9

#define PROG_LED_PIN     25

/* ---- Platform UART implementation for TPUART ---- */
class PicoTpuartUart : public TpuartUart
{
public:
    void begin() override
    {
        uart_init(TPUART_UART, 19200);
        uart_set_format(TPUART_UART, 8, 1, UART_PARITY_EVEN);
        gpio_set_function(TPUART_TX_PIN, GPIO_FUNC_UART);
        gpio_set_function(TPUART_RX_PIN, GPIO_FUNC_UART);
    }

    void end() override
    {
        uart_deinit(TPUART_UART);
    }

    int available() const override
    {
        return uart_is_readable(TPUART_UART) ? 1 : 0;
    }

    int read() override
    {
        if (uart_is_readable(TPUART_UART))
            return uart_getc(TPUART_UART);
        return -1;
    }

    void write(uint8_t byte) override
    {
        uart_putc_raw(TPUART_UART, byte);
    }

    void write(const uint8_t* data, uint16_t length) override
    {
        uart_write_blocking(TPUART_UART, data, length);
    }

    void flush() override
    {
        uart_tx_wait_blocking(TPUART_UART);
    }
};

/* ---- KNX bus callback ---- */
class SimpleKnxCallback : public KnxBusCallback
{
public:
    bool onTelegramReceived(const uint8_t* telegram, uint16_t length) override
    {
        gpio_put(PROG_LED_PIN, !gpio_get(PROG_LED_PIN));
        return true;
    }

    void onTelegramSent(bool success) override
    {
        (void)success;
    }

    uint16_t ownAddress() const override
    {
        return 0x1101; // 1.1.1
    }

    bool isAddressRelevant(uint16_t destAddr, bool isGroupAddr) const override
    {
        (void)destAddr;
        (void)isGroupAddr;
        return true; // Accept all for testing
    }

    bool canAcceptTelegram() const override
    {
        return true;
    }

    int maxTelegramSize() const override
    {
        return 23;
    }
};

static PicoTpuartUart tpuartUart;
static KnxBusTpuart knxBus(tpuartUart);
static SimpleKnxCallback knxCallback;

int main()
{
    stdio_init_all();

    pinMode(PROG_LED_PIN, OUTPUT);
    digitalWrite(PROG_LED_PIN, true);

    // Initialize KNX bus via TPUART
    knxBus.begin(&knxCallback);

    while (true)
    {
        knxBus.loop();
        __wfi();
    }
}
