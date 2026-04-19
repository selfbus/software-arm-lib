/*
 *  knx_bus_tpuart.h - KNX TP1 physical layer using an external TPUART/NCN5120 chip.
 *
 *  The TPUART handles the complete KNX TP1 bit-level protocol. This driver
 *  communicates with the TPUART via a standard UART (serial) interface.
 *  It translates between the TPUART's serial protocol and the KnxBusInterface API.
 *
 *  Supported chips:
 *   - Siemens TP-UART / TP-UART 2
 *   - ON Semiconductor NCN5120 / NCN5121 / NCN5130
 *
 *  This implementation is MCU-independent — it only requires a UART byte
 *  stream, making it suitable for any platform (RP2350, STM32, ESP32, Linux, ...).
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */
#ifndef SBLIB_EIB_KNX_BUS_TPUART_H
#define SBLIB_EIB_KNX_BUS_TPUART_H

#include <sblib/eib/knx_bus_interface.h>
#include <stdint.h>

/**
 * Abstract UART byte interface required by the TPUART driver.
 *
 * Implement this for your platform's UART peripheral.
 */
class TpuartUart
{
public:
    virtual ~TpuartUart() = default;

    /** Initialize the UART at 19200 baud, 8E1 (TPUART standard). */
    virtual void begin() = 0;

    /** Shut down the UART. */
    virtual void end() = 0;

    /** Return the number of bytes available for reading. */
    virtual int available() const = 0;

    /** Read one byte. Returns -1 if no data available. */
    virtual int read() = 0;

    /** Write one byte. Blocks if buffer is full. */
    virtual void write(uint8_t byte) = 0;

    /** Write multiple bytes. */
    virtual void write(const uint8_t* data, uint16_t length) = 0;

    /** Flush the TX buffer (wait until sent). */
    virtual void flush() = 0;
};


/**
 * TPUART/NCN5120-based KNX TP1 physical bus layer.
 *
 * This implementation works with any MCU by using the TpuartUart abstraction.
 * The TPUART chip handles bit-level timing, collision detection, and ACK generation.
 */
class KnxBusTpuart : public KnxBusInterface
{
public:
    /**
     * @param uart The platform-specific UART to communicate with the TPUART chip.
     */
    explicit KnxBusTpuart(TpuartUart& uart);
    ~KnxBusTpuart() override;

    void begin(KnxBusCallback* callback) override;
    void end() override;
    void pause(bool waitForTelegramSent = false) override;
    void resume() override;
    void loop() override;
    void sendTelegram(uint8_t* telegram, uint16_t length) override;
    bool sendingFrame() const override;
    void maxSendRetries(int retries) override;
    void maxSendBusyRetries(int retries) override;

private:
    /* ---- TPUART protocol commands ---- */
    static constexpr uint8_t TPUART_RESET_REQ       = 0x01;
    static constexpr uint8_t TPUART_STATE_REQ        = 0x02;
    static constexpr uint8_t TPUART_ACTIVATEBUSMON   = 0x05;
    static constexpr uint8_t TPUART_ACK_INFO         = 0x10;
    static constexpr uint8_t TPUART_DATA_START       = 0x80;
    static constexpr uint8_t TPUART_DATA_CONTINUE    = 0x80;
    static constexpr uint8_t TPUART_DATA_END         = 0x40;

    /* ---- TPUART response indicators ---- */
    static constexpr uint8_t TPUART_RESET_IND        = 0x03;
    static constexpr uint8_t TPUART_STATE_IND_MASK   = 0x07;
    static constexpr uint8_t TPUART_DATA_CONF_POS    = 0x8B;
    static constexpr uint8_t TPUART_DATA_CONF_NEG    = 0x0B;

    /* ---- State machine ---- */
    enum class State : uint8_t
    {
        RESET,
        IDLE,
        RX_RECEIVING,
        TX_SENDING,
        TX_WAIT_CONFIRM,
    };

    TpuartUart& uart_;
    KnxBusCallback* callback_ = nullptr;
    volatile State state_ = State::RESET;

    /* ---- RX ---- */
    uint8_t* rxBuffer_ = nullptr;
    int rxIndex_ = 0;
    uint32_t rxLastByteTime_ = 0;

    /* ---- TX ---- */
    uint8_t* txTelegram_ = nullptr;
    uint16_t txTelegramLen_ = 0;
    int txByteIndex_ = 0;
    int sendRetries_ = 0;
    int sendRetriesMax_ = 3;
    int sendBusyRetries_ = 0;
    int sendBusyRetriesMax_ = 3;

    /* ---- Internal helpers ---- */
    void processReceivedByte(uint8_t byte);
    void handleRxTelegram();
    void sendNextTxByte();
    void handleTxConfirmation(bool positive);
    void requestState();
    void resetTpuart();
    void setAckMode();
};

#endif /* SBLIB_EIB_KNX_BUS_TPUART_H */
