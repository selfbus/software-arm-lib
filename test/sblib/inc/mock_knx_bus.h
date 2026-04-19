/*
 *  mock_knx_bus.h - Mock implementation of KnxBusInterface for unit testing.
 *
 *  This mock simulates the KNX TP1 physical layer without any hardware.
 *  It allows test cases to inject received telegrams and inspect sent telegrams.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */
#ifndef TEST_MOCK_KNX_BUS_H
#define TEST_MOCK_KNX_BUS_H

#include <sblib/eib/knx_bus_interface.h>
#include <string.h>
#include <vector>
#include <cstdint>

/**
 * Mock KNX bus interface for protocol-level tests.
 *
 * Usage:
 *   MockKnxBus mockBus;
 *   BcuDefault bcu(&mockBus);
 *   bcu.begin(...);
 *
 *   // Simulate receiving a telegram from the bus
 *   mockBus.injectReceivedTelegram(bytes, len);
 *   bcu.loop();
 *
 *   // Check what the BCU sent
 *   REQUIRE(mockBus.sentTelegramCount() == 1);
 *   REQUIRE(mockBus.lastSentTelegram()[0] == 0xB0);
 */
class MockKnxBus : public KnxBusInterface
{
public:
    static constexpr int MAX_TELEGRAM_SIZE = 64;

    MockKnxBus()
        : callback_(nullptr)
        , sending_(false)
        , paused_(false)
        , sendRetriesMax_(3)
        , sendBusyRetriesMax_(3)
        , ackBehavior_(ACK_ALWAYS)
    {
    }

    // ---- KnxBusInterface overrides ----

    void begin(KnxBusCallback* callback) override
    {
        callback_ = callback;
        sending_ = false;
        paused_ = false;
        sentTelegrams_.clear();
        pendingRx_.clear();
    }

    void end() override
    {
        callback_ = nullptr;
    }

    void pause(bool waitForTelegramSent) override
    {
        (void)waitForTelegramSent;
        paused_ = true;
    }

    void resume() override
    {
        paused_ = false;
    }

    void loop() override
    {
        // Deliver any pending received telegrams to the callback
        while (!pendingRx_.empty() && callback_)
        {
            RxTelegram& rx = pendingRx_.front();
            callback_->onTelegramReceived(rx.data, rx.length);
            pendingRx_.erase(pendingRx_.begin());
        }
    }

    void sendTelegram(uint8_t* telegram, uint16_t length) override
    {
        sending_ = true;

        // Store a copy of the sent telegram
        SentTelegram tx;
        tx.length = (length < MAX_TELEGRAM_SIZE) ? length : MAX_TELEGRAM_SIZE;
        memcpy(tx.data, telegram, tx.length);

        // Calculate and append checksum
        uint8_t checksum = 0xFF;
        for (uint16_t i = 0; i < length; i++)
            checksum ^= telegram[i];
        if (tx.length < MAX_TELEGRAM_SIZE)
            tx.data[tx.length] = checksum;

        sentTelegrams_.push_back(tx);
        sending_ = false;

        // Simulate ACK from remote
        if (callback_)
        {
            bool success = (ackBehavior_ == ACK_ALWAYS);
            callback_->onTelegramSent(success);
        }
    }

    bool sendingFrame() const override
    {
        return sending_;
    }

    void maxSendRetries(int retries) override
    {
        sendRetriesMax_ = retries;
    }

    void maxSendBusyRetries(int retries) override
    {
        sendBusyRetriesMax_ = retries;
    }

    // ---- Test helper methods ----

    /**
     * Inject a telegram as if received from the KNX bus.
     * The telegram will be delivered via callback_->onTelegramReceived()
     * on the next loop() call.
     *
     * @param telegram  Raw telegram bytes including checksum.
     * @param length    Total length including checksum.
     */
    void injectReceivedTelegram(const uint8_t* telegram, uint16_t length)
    {
        RxTelegram rx;
        rx.length = (length < MAX_TELEGRAM_SIZE) ? length : MAX_TELEGRAM_SIZE;
        memcpy(rx.data, telegram, rx.length);
        pendingRx_.push_back(rx);
    }

    /**
     * Inject a telegram and immediately deliver it (bypass loop()).
     */
    void injectAndDeliver(const uint8_t* telegram, uint16_t length)
    {
        if (callback_)
            callback_->onTelegramReceived(telegram, length);
    }

    /** Number of telegrams sent by the BCU so far. */
    size_t sentTelegramCount() const { return sentTelegrams_.size(); }

    /** Get the data of the n-th sent telegram (0-based). */
    const uint8_t* sentTelegram(size_t index) const
    {
        return (index < sentTelegrams_.size()) ? sentTelegrams_[index].data : nullptr;
    }

    /** Get the length of the n-th sent telegram. */
    uint16_t sentTelegramLength(size_t index) const
    {
        return (index < sentTelegrams_.size()) ? sentTelegrams_[index].length : 0;
    }

    /** Get the last sent telegram data. */
    const uint8_t* lastSentTelegram() const
    {
        return sentTelegrams_.empty() ? nullptr : sentTelegrams_.back().data;
    }

    /** Get the last sent telegram length. */
    uint16_t lastSentTelegramLength() const
    {
        return sentTelegrams_.empty() ? 0 : sentTelegrams_.back().length;
    }

    /** Clear all recorded sent telegrams. */
    void clearSentTelegrams() { sentTelegrams_.clear(); }

    /** Clear all pending received telegrams. */
    void clearPendingRx() { pendingRx_.clear(); }

    /** Reset everything to initial state. */
    void reset()
    {
        sentTelegrams_.clear();
        pendingRx_.clear();
        sending_ = false;
        paused_ = false;
    }

    /** ACK behavior control */
    enum AckBehavior { ACK_ALWAYS, NACK_ALWAYS, BUSY_ALWAYS };

    void setAckBehavior(AckBehavior behavior) { ackBehavior_ = behavior; }

    /** Access the callback for advanced test scenarios. */
    KnxBusCallback* getCallback() const { return callback_; }

private:
    struct RxTelegram
    {
        uint8_t data[MAX_TELEGRAM_SIZE];
        uint16_t length;
    };

    struct SentTelegram
    {
        uint8_t data[MAX_TELEGRAM_SIZE];
        uint16_t length;
    };

    KnxBusCallback* callback_;
    bool sending_;
    bool paused_;
    int sendRetriesMax_;
    int sendBusyRetriesMax_;
    AckBehavior ackBehavior_;

    std::vector<SentTelegram> sentTelegrams_;
    std::vector<RxTelegram> pendingRx_;
};

#endif /* TEST_MOCK_KNX_BUS_H */
