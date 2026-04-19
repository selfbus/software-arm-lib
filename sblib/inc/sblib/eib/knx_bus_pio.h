/*
 *  knx_bus_pio.h - KNX TP1 physical layer using RP2350 PIO state machines.
 *
 *  Uses two PIO state machines:
 *   - SM0 (TX): Generates precise 35µs low pulses for each 0-bit at 104µs intervals.
 *   - SM1 (RX): Captures falling edges with cycle-accurate timestamps.
 *
 *  The byte/frame-level protocol (collision detection, ACK, retries) runs
 *  in the PIO IRQ handler + alarm callback on the ARM/RISC-V core.
 *
 *  The state machine closely mirrors the original LPC11xx timer-based
 *  bus.cpp implementation, but uses PIO events and hardware alarm timers
 *  instead of timer capture/match.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */
#ifndef SBLIB_EIB_KNX_BUS_PIO_H
#define SBLIB_EIB_KNX_BUS_PIO_H

#include <sblib/eib/knx_bus_interface.h>
#include <stdint.h>

/**
 * PIO-based KNX TP1 physical bus layer for RP2350.
 *
 * Pin assignment (connects to KNX transceiver, e.g. TJA1021 or discrete circuit):
 *  - txPin: GPIO driving the bus transmitter (active-low pulse = 0-bit)
 *  - rxPin: GPIO connected to bus receiver (falling edge = start of 0-bit)
 */
class KnxBusPio : public KnxBusInterface
{
public:
    KnxBusPio(int pioIndex, int rxPin, int txPin);
    ~KnxBusPio() override;

    void begin(KnxBusCallback* callback) override;
    void end() override;
    void pause(bool waitForTelegramSent = false) override;
    void resume() override;
    void loop() override;
    void sendTelegram(uint8_t* telegram, uint16_t length) override;
    bool sendingFrame() const override;
    void maxSendRetries(int retries) override;
    void maxSendBusyRetries(int retries) override;

    /** Called from PIO IRQ handler trampolines. Do not call directly. */
    void pioIrqHandler();

    /** Called from hardware alarm callback. Do not call directly. */
    void alarmCallback();

    /** Returns the RX pin number (for ISR routing). */
    int rxPinPublic() const { return rxPin_; }

private:
    /* ---- Configuration ---- */
    int pioIndex_;
    int rxPin_;
    int txPin_;

    /* ---- Callback to L2 ---- */
    KnxBusCallback* callback_ = nullptr;

    /* ---- State machine (mirrors bus.cpp states) ---- */
    enum class State : uint8_t
    {
        INIT,                               // Waiting for 50BT idle
        IDLE,                               // Bus idle, waiting for RX start or TX trigger
        RX_WAIT_FOR_STARTBIT_OR_TELEND,     // Waiting for next start bit or end-of-byte timeout
        RX_BITS_OF_BYTE,                    // Collecting bits of current byte
        RX_WAIT_FOR_ACK_TX_START,           // Waiting 15BT before sending ACK
        WAIT_50BT,                          // Waiting 50BT for next RX/TX/idle
        TX_START_BIT,                       // Sending start bit, watching for collision
        TX_BIT_0,                           // Preparing first data bit
        TX_BITS_OF_BYTE,                    // Sending bits of current byte
        TX_END_OF_BYTE,                     // Stop bit reached, decide next byte or end
        TX_END_OF_TX,                       // Last byte sent, sync with bus timing
        TX_WAIT_FOR_RX_ACK_WINDOW,          // Waiting 15BT for ACK window to open
        TX_WAIT_FOR_RX_ACK,                 // Waiting for ACK in the ACK window
    };
    volatile State state_ = State::INIT;

    /* ---- TX state ---- */
    uint8_t* txTelegram_ = nullptr;         // Pointer to telegram being sent (nullptr = nothing pending)
    int txTelegramLen_ = 0;                 // Length including checksum
    int txByteIndex_ = 0;                   // Next byte index in txTelegram_
    int txCurrentByte_ = 0;                 // Current byte incl. parity in bit8
    int txBitMask_ = 0;                     // Current bit position being sent
    int sendRetries_ = 0;
    int sendRetriesMax_ = 3;
    int sendBusyRetries_ = 0;
    int sendBusyRetriesMax_ = 3;
    uint8_t collisions_ = 0;
    bool repeatTelegram_ = false;
    bool busyWaitFromRemote_ = false;
    bool waitForAckFromRemote_ = false;
    uint16_t txError_ = 0;

    /* ---- RX state ---- */
    uint8_t* rxBuffer_ = nullptr;           // L1 receive buffer (allocated in begin())
    int rxByteIndex_ = 0;                   // Number of bytes received so far
    int rxBitMask_ = 0;                     // Current bit position within byte
    int rxBitTime_ = 0;                     // Expected time of current bit position
    int rxCurrentByte_ = 0;                 // Byte being assembled
    int rxParity_ = 0;                      // Running parity for current byte
    int rxValid_ = 0;                       // 1 if all parities are valid
    int rxChecksum_ = 0;                    // Running XOR checksum
    uint16_t rxError_ = 0;

    /* ---- ACK ---- */
    int sendAck_ = 0;                       // ACK byte to send (0 = none)

    /* ---- Timing ---- */
    int hardwareAlarm_ = -1;                // RP2350 hardware alarm number (0-3)
    uint64_t lastEdgeUs_ = 0;               // Timestamp of last captured falling edge
    uint64_t alarmTargetUs_ = 0;            // When the current alarm fires

    /* ---- Internal helpers ---- */
    void initState();
    void idleState();
    void prepareForSending();
    void finishSendingTelegram(bool success);
    void encounteredCollision();
    void handleReceivedTelegram();
    void prepareTelegram(uint8_t* telegram, uint16_t length) const;

    void startAlarm(uint32_t delayUs);
    void cancelAlarm();

    /* ---- PIO helpers ---- */
    void pioInit();
    void pioDeInit();
    void pioSendPulse(uint32_t delayUs);
    void pioStopPulse();
    void pioStartCapture();
    void pioStopCapture();

    /* ---- Error flags (same as bus.h) ---- */
    static constexpr uint16_t RX_OK                 = 0x0000;
    static constexpr uint16_t RX_CHECKSUM_ERROR      = 0x0001;
    static constexpr uint16_t RX_PARITY_ERROR        = 0x0002;
    static constexpr uint16_t RX_LENGTH_ERROR        = 0x0004;
    static constexpr uint16_t RX_TIMING_ERROR        = 0x0008;
    static constexpr uint16_t RX_PREAMBLE_ERROR      = 0x0010;
    static constexpr uint16_t RX_STOPBIT_ERROR       = 0x0020;
    static constexpr uint16_t RX_BUFFER_BUSY         = 0x0040;
    static constexpr uint16_t RX_INVALID_TELEGRAM    = 0x0080;

    static constexpr uint16_t TX_OK                  = 0x0000;
    static constexpr uint16_t TX_COLLISION_ERROR     = 0x0001;
    static constexpr uint16_t TX_NACK_ERROR          = 0x0002;
    static constexpr uint16_t TX_REMOTE_BUSY_ERROR   = 0x0004;
    static constexpr uint16_t TX_RETRY_ERROR         = 0x0008;
    static constexpr uint16_t TX_ACK_TIMEOUT_ERROR   = 0x0010;
    static constexpr uint16_t TX_PWM_STARTBIT_ERROR  = 0x0020;

    static constexpr int COLLISION_RETRY_MAX = 3;
};

#endif /* SBLIB_EIB_KNX_BUS_PIO_H */
