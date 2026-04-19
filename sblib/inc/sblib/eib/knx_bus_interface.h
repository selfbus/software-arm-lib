/*
 *  knx_bus_interface.h - Abstract physical layer interface for KNX TP1 bus access.
 *
 *  This interface decouples the KNX protocol stack (L2+) from the physical
 *  layer implementation (L1). Implementations can use:
 *   - Timer-based bit-banging (original LPC11xx approach)
 *   - PIO state machines (RP2350)
 *   - Hardware TPUART/NCN5120 transceivers (any MCU)
 *   - USB/IP tunneling for testing
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */
#ifndef SBLIB_EIB_KNX_BUS_INTERFACE_H
#define SBLIB_EIB_KNX_BUS_INTERFACE_H

#include <stdint.h>
#include <sblib/types.h>
#include <sblib/eib/bus_const.h>

class BcuBase;

/**
 * Callback interface from the physical bus layer up to the protocol stack (L2).
 *
 * The KnxBusInterface implementation calls these methods when frames are
 * received, when transmission completes, or when errors occur.
 */
class KnxBusCallback
{
public:
    virtual ~KnxBusCallback() = default;

    /**
     * Called by the physical layer when a complete, valid telegram has been received.
     *
     * @param telegram  Pointer to the received telegram bytes (including checksum).
     * @param length    Total length of the telegram including checksum byte.
     * @return true if the telegram buffer was accepted (caller may reuse its internal buffer),
     *         false if the upper layer is busy (physical layer should send BUSY or drop).
     */
    virtual bool onTelegramReceived(const uint8_t* telegram, uint16_t length) = 0;

    /**
     * Called when transmission of a telegram has completed.
     *
     * @param success  true if the remote side acknowledged, false on NACK/timeout/collision.
     */
    virtual void onTelegramSent(bool success) = 0;

    /**
     * Get the own physical KNX address of this device (for source address insertion).
     */
    virtual uint16_t ownAddress() const = 0;

    /**
     * Check whether a given destination address is relevant for this device.
     * Used by the physical layer to decide whether to ACK a received telegram.
     *
     * @param destAddr      Destination address from the telegram.
     * @param isGroupAddr   true if the address is a group address, false if physical.
     * @return true if this device should accept the telegram.
     */
    virtual bool isAddressRelevant(uint16_t destAddr, bool isGroupAddr) const = 0;

    /**
     * Query whether the upper layer is ready to accept a new telegram.
     * If false, the physical layer responds with BUSY (if the protocol requires it).
     */
    virtual bool canAcceptTelegram() const = 0;

    /**
     * Get the maximum telegram size supported by this BCU variant.
     */
    virtual int maxTelegramSize() const = 0;
};


/**
 * Abstract interface for the KNX TP1 physical bus layer.
 *
 * Each implementation handles the complete L1 protocol:
 *  - Bit-level encoding/decoding (UART-like 9600 baud, even parity)
 *  - Collision detection and avoidance
 *  - ACK/NACK/BUSY handling
 *  - Telegram checksums
 *  - Retry logic
 *
 * The implementation is expected to work interrupt-driven or DMA-driven.
 * The loop() method is called from the main loop for non-time-critical work.
 */
class KnxBusInterface
{
public:
    virtual ~KnxBusInterface() = default;

    /**
     * Initialize the physical bus layer.
     * Must be called once before any bus activity.
     *
     * @param callback  Pointer to the L2 callback handler (lifetime must exceed this object).
     */
    virtual void begin(KnxBusCallback* callback) = 0;

    /**
     * Shut down the physical bus layer.
     * Releases hardware resources and stops all bus activity.
     */
    virtual void end() = 0;

    /**
     * Pause bus activity (e.g., for flash programming).
     * Waits until is safe to pause, then stops TX/RX.
     *
     * @param waitForTelegramSent  If true, waits until any pending TX completes.
     */
    virtual void pause(bool waitForTelegramSent = false) = 0;

    /** Resume bus activity after pause(). */
    virtual void resume() = 0;

    /**
     * Non-blocking main-loop processing.
     * Called repeatedly from the application main loop.
     * Used for debug output, deferred processing, etc.
     */
    virtual void loop() = 0;

    /**
     * Queue a telegram for transmission.
     *
     * The physical layer will:
     *  1. Set the source address to ownAddress()
     *  2. Calculate and append the checksum
     *  3. Handle collision avoidance, retries, and ACK reception
     *  4. Call onTelegramSent() when done
     *
     * The caller must not modify the telegram buffer until onTelegramSent() fires.
     *
     * @param telegram  Telegram bytes (header + payload). Must have space for 1 extra byte (checksum).
     * @param length    Length of the telegram WITHOUT checksum.
     */
    virtual void sendTelegram(uint8_t* telegram, uint16_t length) = 0;

    /**
     * Test if a frame is currently being sent (data or acknowledge).
     */
    virtual bool sendingFrame() const = 0;

    /**
     * Set the maximum number of NACK retries.
     * @param retries  Default: 3
     */
    virtual void maxSendRetries(int retries) = 0;

    /**
     * Set the maximum number of BUSY retries.
     * @param retries  Default: 3
     */
    virtual void maxSendBusyRetries(int retries) = 0;
};

#endif /* SBLIB_EIB_KNX_BUS_INTERFACE_H */
