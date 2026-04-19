/*
 *  knx_bus_legacy.h - Wrapper for the existing LPC11xx Timer-based Bus class.
 *
 *  This adapter wraps the original Bus class to conform to the new
 *  KnxBusInterface, enabling incremental migration.
 *
 *  For new ports (RP2350, TPUART), use the dedicated implementations instead.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */
#ifndef SBLIB_EIB_KNX_BUS_LEGACY_H
#define SBLIB_EIB_KNX_BUS_LEGACY_H

#include <sblib/eib/knx_bus_interface.h>

class Bus;
class Timer;
enum TimerCapture : int;
enum TimerMatch : int;

/**
 * Adapter that wraps the existing LPC11xx Bus class behind the KnxBusInterface.
 *
 * This allows the refactored BcuBase to work with both the legacy Bus implementation
 * and the new PIO/TPUART implementations.
 */
class KnxBusLegacy : public KnxBusInterface
{
public:
    /**
     * @param timer           Reference to the LPC timer to use.
     * @param rxPin           Bus receive pin (e.g., PIO1_8).
     * @param txPin           Bus transmit pin (e.g., PIO1_9).
     * @param captureChannel  Timer capture channel for rxPin.
     * @param matchChannel    Timer match channel for txPin.
     */
    KnxBusLegacy(Timer& timer, int rxPin, int txPin,
                 TimerCapture captureChannel, TimerMatch matchChannel);
    ~KnxBusLegacy() override;

    void begin(KnxBusCallback* callback) override;
    void end() override;
    void pause(bool waitForTelegramSent = false) override;
    void resume() override;
    void loop() override;
    void sendTelegram(uint8_t* telegram, uint16_t length) override;
    bool sendingFrame() const override;
    void maxSendRetries(int retries) override;
    void maxSendBusyRetries(int retries) override;

    /** Access the underlying Bus object (needed for interrupt handler macro). */
    Bus& underlyingBus();

private:
    Bus* bus_;
    KnxBusCallback* callback_ = nullptr;
};

#endif /* SBLIB_EIB_KNX_BUS_LEGACY_H */
