/*
 *  main.cpp - Minimal KNX device example for RP2350 using PIO bus interface.
 *
 *  This demonstrates how to use the ported sblib on an RP2350 board.
 *  The KNX transceiver (e.g. discrete circuit) is connected to:
 *    - GPIO 2: KNX TX (active-low pulse output)
 *    - GPIO 3: KNX RX (falling-edge input)
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#include "pico/stdlib.h"

#include <sblib/hal/platform_hal.h>
#include <sblib/eib/knx_bus_pio.h>

// TODO: Include BCU headers once the full stack is ported.
// For now, this demonstrates the physical layer only.

/* ---- Pin assignments ---- */
static constexpr int KNX_TX_PIN = 2;
static constexpr int KNX_RX_PIN = 3;
static constexpr int PROG_LED_PIN = 25; // On-board LED on Pico

/* ---- Simple callback for testing ---- */
class SimpleKnxCallback : public KnxBusCallback
{
public:
    bool onTelegramReceived(const uint8_t* telegram, uint16_t length) override
    {
        // For now, just toggle the LED on telegram reception
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
        return true; // Accept everything for testing
    }

    bool canAcceptTelegram() const override
    {
        return true;
    }

    int maxTelegramSize() const override
    {
        return 23; // Standard telegram max
    }
};

static SimpleKnxCallback knxCallback;
static KnxBusPio knxBus(0, KNX_RX_PIN, KNX_TX_PIN); // PIO 0

int main()
{
    PlatformHAL::init();

    // LED
    gpio_init(PROG_LED_PIN);
    gpio_set_dir(PROG_LED_PIN, GPIO_OUT);
    gpio_put(PROG_LED_PIN, 1);

    // Initialize KNX bus
    knxBus.begin(&knxCallback);

    while (true)
    {
        knxBus.loop();

        // Application logic here...

        PlatformHAL::waitForInterrupt();
    }
}
