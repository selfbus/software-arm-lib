/*
 *  knx_bus_tpuart.cpp - KNX TP1 physical layer using TPUART/NCN5120 transceiver.
 *
 *  The TPUART chip handles all bit-level timing and collision detection.
 *  This driver converts between the TPUART serial protocol (19200 baud, 8E1)
 *  and the KnxBusInterface API.
 *
 *  TPUART Protocol (simplified):
 *   TX to TPUART:  [U_L_DataStart + index] [byte0] [byte1] ... [U_L_DataEnd + index] [lastbyte]
 *   RX from TPUART: [L_DataStandard_ind]  [byte0] [byte1] ... [checksum]
 *   Confirmation:   [L_Data.con] (positive or negative)
 *
 *  Reference: Siemens TP-UART IC datasheet, ON Semi NCN5120/NCN5130 datasheet.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#include <sblib/eib/knx_bus_tpuart.h>
#include <sblib/eib/knx_lpdu.h>
#include <sblib/hal/platform_hal.h>

#include <cstring>

/* ---- TPUART service codes ---- */
static constexpr uint8_t U_RESET_REQ        = 0x01;
static constexpr uint8_t U_STATE_REQ        = 0x02;
static constexpr uint8_t U_ACK_REQ          = 0x11; // bit 4..0 = nack/busy flags
static constexpr uint8_t U_ACK_REQ_ACK      = 0x11;
static constexpr uint8_t U_ACK_REQ_NACK     = 0x14;
static constexpr uint8_t U_ACK_REQ_BUSY     = 0x12;
static constexpr uint8_t U_PRODUCTID_REQ    = 0x14;
static constexpr uint8_t U_ACTIVATEBUSMON   = 0x05;

static constexpr uint8_t U_L_DATA_START     = 0x80; // | byteIndex (0x00..0x3F)
static constexpr uint8_t U_L_DATA_CONT      = 0x80;
static constexpr uint8_t U_L_DATA_END       = 0x40; // | byteIndex (0x00..0x3F)

/* TPUART response indicators */
static constexpr uint8_t U_RESET_IND        = 0x03;
static constexpr uint8_t L_DATA_CONF_POS    = 0x8B;
static constexpr uint8_t L_DATA_CONF_NEG    = 0x0B;
static constexpr uint8_t L_DATA_STANDARD_IND = 0x90; // Bit pattern: 1001_DCBA

/* Byte reception timeout (ms) — if no next byte within this, frame is complete */
static constexpr uint32_t RX_TIMEOUT_MS = 3;

/* ===========================================================================
 * Constructor / Destructor
 * =========================================================================*/

KnxBusTpuart::KnxBusTpuart(TpuartUart& uart)
    : uart_(uart)
{
}

KnxBusTpuart::~KnxBusTpuart()
{
    end();
    delete[] rxBuffer_;
}


/* ===========================================================================
 * KnxBusInterface implementation
 * =========================================================================*/

void KnxBusTpuart::begin(KnxBusCallback* callback)
{
    callback_ = callback;

    delete[] rxBuffer_;
    rxBuffer_ = new uint8_t[callback_->maxTelegramSize()]();

    rxIndex_ = 0;
    txTelegram_ = nullptr;
    txByteIndex_ = 0;
    sendRetries_ = 0;
    sendBusyRetries_ = 0;

    uart_.begin();
    resetTpuart();
    state_ = State::IDLE;
    setAckMode();
}

void KnxBusTpuart::end()
{
    uart_.end();
    state_ = State::RESET;
}

void KnxBusTpuart::pause(bool waitForTelegramSent)
{
    if (waitForTelegramSent)
    {
        while (txTelegram_ != nullptr)
            loop();
    }
    state_ = State::IDLE;
}

void KnxBusTpuart::resume()
{
    state_ = State::IDLE;
}

void KnxBusTpuart::loop()
{
    // Process incoming bytes from TPUART
    while (uart_.available())
    {
        uint8_t byte = (uint8_t)uart_.read();
        rxLastByteTime_ = PlatformHAL::millis();
        processReceivedByte(byte);
    }

    // Check for RX timeout (end of telegram)
    if (state_ == State::RX_RECEIVING && rxIndex_ > 0)
    {
        uint32_t elapsed = PlatformHAL::millis() - rxLastByteTime_;
        if (elapsed >= RX_TIMEOUT_MS)
        {
            handleRxTelegram();
        }
    }
}

void KnxBusTpuart::sendTelegram(uint8_t* telegram, uint16_t length)
{
    // Set source address and checksum
    setSenderAddress(telegram, callback_->ownAddress());
    uint8_t checksum = 0xFF;
    for (uint16_t i = 0; i < length; ++i)
        checksum ^= telegram[i];
    telegram[length] = checksum;

    // Wait for previous TX to complete
    while (txTelegram_ != nullptr)
        loop();

    txTelegram_ = telegram;
    txTelegramLen_ = length + 1; // +1 for checksum
    txByteIndex_ = 0;
    sendRetries_ = 0;
    sendBusyRetries_ = 0;
    state_ = State::TX_SENDING;

    // Start sending first byte
    sendNextTxByte();
}

bool KnxBusTpuart::sendingFrame() const
{
    return txTelegram_ != nullptr;
}

void KnxBusTpuart::maxSendRetries(int retries)
{
    sendRetriesMax_ = retries;
}

void KnxBusTpuart::maxSendBusyRetries(int retries)
{
    sendBusyRetriesMax_ = retries;
}


/* ===========================================================================
 * Internal protocol logic
 * =========================================================================*/

void KnxBusTpuart::processReceivedByte(uint8_t byte)
{
    switch (state_)
    {
    case State::IDLE:
        // Check if it's a data indication (receive telegram from bus)
        if ((byte & 0xD3) == 0x90) // L_DATA_STANDARD_IND pattern
        {
            state_ = State::RX_RECEIVING;
            rxIndex_ = 0;
            // The first telegram byte follows immediately
        }
        else if (byte == U_RESET_IND)
        {
            // TPUART was reset, re-initialize
            setAckMode();
        }
        break;

    case State::RX_RECEIVING:
        if (rxIndex_ < callback_->maxTelegramSize())
        {
            rxBuffer_[rxIndex_++] = byte;
        }
        break;

    case State::TX_SENDING:
        // Characters we receive during TX are typically our own bytes echoed back
        // or indications from the TPUART. Ignore during TX.
        break;

    case State::TX_WAIT_CONFIRM:
        if (byte == L_DATA_CONF_POS)
        {
            handleTxConfirmation(true);
        }
        else if (byte == L_DATA_CONF_NEG)
        {
            handleTxConfirmation(false);
        }
        break;

    case State::RESET:
        if (byte == U_RESET_IND)
        {
            state_ = State::IDLE;
            setAckMode();
        }
        break;
    }
}

void KnxBusTpuart::handleRxTelegram()
{
    if (rxIndex_ < 7)
    {
        // Too short for a valid telegram
        rxIndex_ = 0;
        state_ = State::IDLE;
        return;
    }

    // Verify checksum
    uint8_t checksum = 0xFF;
    for (int i = 0; i < rxIndex_; ++i)
        checksum ^= rxBuffer_[i];

    if (checksum != 0)
    {
        // Checksum error — discard
        rxIndex_ = 0;
        state_ = State::IDLE;
        return;
    }

    // Deliver to upper layer
    if (callback_->canAcceptTelegram())
    {
        callback_->onTelegramReceived(rxBuffer_, rxIndex_);
        // TPUART sends ACK automatically if configured via U_ACK_REQ
    }

    rxIndex_ = 0;
    state_ = State::IDLE;
}

void KnxBusTpuart::sendNextTxByte()
{
    if (txByteIndex_ >= txTelegramLen_)
    {
        // All bytes sent, wait for confirmation
        state_ = State::TX_WAIT_CONFIRM;
        return;
    }

    // TPUART protocol: each byte is prefixed with U_L_DATA_START/CONT/END + index
    uint8_t prefix;
    if (txByteIndex_ == txTelegramLen_ - 1)
        prefix = U_L_DATA_END | (txByteIndex_ & 0x3F);
    else
        prefix = U_L_DATA_START | (txByteIndex_ & 0x3F);

    uart_.write(prefix);
    uart_.write(txTelegram_[txByteIndex_]);
    txByteIndex_++;

    // Continue sending remaining bytes
    if (txByteIndex_ < txTelegramLen_)
    {
        // Small delay between bytes (TPUART expects < 110µs between consecutive pairs)
        sendNextTxByte();
    }
    else
    {
        state_ = State::TX_WAIT_CONFIRM;
    }
}

void KnxBusTpuart::handleTxConfirmation(bool positive)
{
    if (positive)
    {
        // Telegram sent and acknowledged
        auto* telegram = txTelegram_;
        txTelegram_ = nullptr;
        state_ = State::IDLE;
        if (callback_)
            callback_->onTelegramSent(true);
    }
    else
    {
        // Negative confirmation — retry if possible
        sendRetries_++;
        if (sendRetries_ >= sendRetriesMax_)
        {
            auto* telegram = txTelegram_;
            txTelegram_ = nullptr;
            state_ = State::IDLE;
            if (callback_)
                callback_->onTelegramSent(false);
        }
        else
        {
            // Retry: resend the telegram
            txByteIndex_ = 0;
            state_ = State::TX_SENDING;
            sendNextTxByte();
        }
    }
}

void KnxBusTpuart::resetTpuart()
{
    uart_.write(U_RESET_REQ);
    uart_.flush();
    PlatformHAL::delayMs(50);
    // Drain any bytes
    while (uart_.available())
        uart_.read();
}

void KnxBusTpuart::setAckMode()
{
    // Configure TPUART to auto-ACK addressed telegrams
    // For individual addressing, the host application must handle ACK
    uart_.write(U_ACK_REQ_ACK);
    uart_.flush();
}

void KnxBusTpuart::requestState()
{
    uart_.write(U_STATE_REQ);
    uart_.flush();
}
