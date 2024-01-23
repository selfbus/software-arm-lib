/*
 *  bus.h - Low level EIB bus access.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_bus_h
#define sblib_bus_h

#include <sblib/core.h>
#include <sblib/eib/bcu_base.h>

#include <sblib/timer.h>
#include <sblib/eib/types.h>

/**
 * Low level class for EIB bus access.
 *
 * When creating a bus object, the handler for the timer must also be
 * created. Example:
 *
 * Bus mybus(timer32_0);
 * BUS_TIMER_INTERRUPT_HANDLER(TIMER32_0_IRQHandler, mybus);
 */
class Bus
{
private:
    BcuBase* bcu;

public:

    /**
     * Create a bus access object.
     *
     * @param timer - The timer to use.
     * @param rxPin - The pin for receiving from the bus, e.g. PIO1_8
     * @param txPin - The pin for sending to the bus, e.g. PIO1_10
     * @param captureChannel - the timer capture channel of rxPin, e.g. CAP0
     * @param matchChannel - the timer match channel of txPin, e.g. MAT0
     */
    Bus(BcuBase* bcu, Timer& timer, int rxPin, int txPin, TimerCapture captureChannel, TimerMatch matchChannel);

    /**
     * Begin using the bus.
     *
     * This powers on all used components.
     * This method must be called before the bus can be used.
     */
    void begin();

    /**
     * Waits for a safe time to pause bus access, then pauses bus access.
     *
     * @param waitForTelegramSent - Whether to wait for an enqueued telegram to be sent or not.
     *                              When passing false, the enqueued telegram is sent out after resume().
     */
    void pause(bool waitForTelegramSent = false);

    /**
     * Resume bus access after it had been paused.
     */
    void resume();

    /**
     * End using the bus.
     *
     * This powers the bus off.
     */
    void end();

    /**
     * The Bus processing loop. This is like the application's loop() function,
     * and is called automatically by main() when the BCU is activated with bcu.begin().
     */
    void loop();

    /**
     * Interface to upper layer for sending a telegram
     *
     * Is called from within the BCU-loop method. Is blocking if there is no free buffer pointer.
     *
     * The procedure is handling two buffer pointers: sendCurTel and SendNextTel, the received pointer is loaded to an available
     * free pointer or is waiting for a free pointer
     *
     * !!! As we have only one buffer at BCU level, the check for buffer free is needed on application before any buffer data is
     * written !!!
     *
     * A free buffer is indicated with "0" as the first byte in the buffer: buffer[0]=0
     *
     * Send a telegram. The checksum byte will be added at the end of telegram[].
     * Ensure that there is at least one byte space at the end of telegram[].
     *
     * @param telegram - the telegram to be sent.
     * @param length - the length of the telegram in sbSendTelegram[], without the checksum
     */
    void sendTelegram(unsigned char* telegram, unsigned short length);

    /**
     * This method is called by the timer interrupt handler.
     * Consider it to be a private method and do not call it.
     */
    void timerInterruptHandler();

    /**
     * Test if there is a telegram being sent.
     *
     * @return True if there is a telegram to be sent, false if not.
     */
    bool sendingTelegram() const;


    /**
     * Test if there is a received telegram in bus.telegram[].
     *
     * @return True if there is a telegram in bus.telegram[], false if not.
     */
    bool telegramReceived() const;

    /**
     * Discard the received telegram. Call this method when you successfully
     * processed the telegram.
     */
    void discardReceivedTelegram();

    /**
     * Set the number of tries that we do sent a telegram when it is not ACKed.
     *
     * @param tries - the number of tries. Default: 3.
     */
    void maxSendTries(int tries);

    /**
     * Set the number of busy tries that we do sent a telegram when we received a BUSY from remote.
     *
     * @param tries - the number of tries. Default: 3.
     */
    void maxSendBusyTries(int tries);



    /** The states of the telegram sending/receiving state machine */
    enum State
    {
        INIT,   //!< The Lib is initializing, waiting for 50bit time inactivity on the bus
        IDLE,   //!< The lib is idle. there was no receiving or sending for at least 50bit times, only cap intr enabled, no timeout intr
        INIT_RX_FOR_RECEIVING_NEW_TEL,      //!< The Lib received a cap event and we need to start/init receiving of a new Telegram
        RECV_WAIT_FOR_STARTBIT_OR_TELEND,   //!< The lib is waiting for Startbit (cap intr (low bit received) or timeout (end of stop bit: char end)) intr enabeld
        RECV_BITS_OF_BYTE,                  //!< The lib is collecting all bit of a character
        RECV_WAIT_FOR_ACK_TX_START,         //!< After Tel is received the lib is waiting for start sending an ACK to remote side.
        WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE,    //!< Timeout event: Start sending the telegram (also triggered in sbSendTelegram[]), cap event: start RX of a new tel
        SEND_START_BIT,                 //!< Send a start bit
        SEND_BIT_0,                     //!< Send the first bit of the current byte
        SEND_BITS_OF_BYTE,              //!< Send the bits of the current byte
        SEND_WAIT_FOR_HIGH_BIT_END,     //!< Send high bit(s) and wait for receiving a the falling edge of our next 0-bit.
        SEND_END_OF_BYTE,               //!< Middle of stop bit reached, decide what to do next
        SEND_END_OF_TX,                 //!< Finish sending current byte
        SEND_WAIT_FOR_RX_ACK_WINDOW,    //!< after sending we wait for the ack receive window to start, only timeout event enabled
        SEND_WAIT_FOR_RX_ACK            //!< after sending we wait for the ack in the ack receive window, cap event: rx start, timeout: repeat tel
    };

    /**
     * The received telegram.
     * The higher layer process should not change the telegram data in the buffer!
     */
    byte* telegram = new byte[bcu->maxTelegramSize()]();

    /**
      * The total length of the received telegram in telegram[].
      */
    volatile int telegramLen;

private:
    /**
     * Determines whether it is currently safe to pause bus access.
     *
     * @param waitForTelegramSent - Whether to wait for an enqueued telegram to be sent or not.
     */
    bool canPause(bool waitForTelegramSent);

    /**
     * Switch to @ref Bus::IDLE state
     * @details Set the Bus state machine to @ref Bus::IDLE state.
     *          We waited at least 50 Bit times (without cap event enabled),
     *          now we wait for next Telegram to receive.
     *          Configure the capture to falling edge and interrupt
     *          match register for low PWM output
     */
    void idleState();

    /**
     * Switch to @ref Bus::WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE state to trigger sending
     */
    void startSendingImmediately();

    /**
     * Initializes all class variables to prepare for the next transmission.
     */
    void prepareForSending();

    /**
     * Finish the telegram sending process.
     *
     * Notify upper layer of completion and prepare for next telegram transmission.
     */
    void finishSendingTelegram();

    /**
     * @fn void prepareTelegram(unsigned char*, unsigned short)const
     * @brief Prepare the telegram for sending. Set the sender address to our own
     *        address, and calculate the checksum of the telegram.
     *        Stores the checksum at telegram[length].
     * @param telegram   the telegram to process
     * @param length[in] the length of the telegram
     */
    void prepareTelegram(unsigned char* telegram, unsigned short length) const;

    /**
     * Handle the received bytes on a low level. This function is called by
     * the function TIMER16_1_IRQHandler() to decide about further processing of the
     * received bytes.
     *
     * @param valid - 1 if all bytes had correct parity and the checksum is correct, 0 if not
     */
    void handleTelegram(bool valid);

protected:
    friend class TLayer4;
    friend class BcuDefault;
    friend class BcuBase;

    Timer& timer;                //!< The timer
    int rxPin, txPin;            //!< The pins for bus receiving and sending
    TimerCapture captureChannel; //!< The timer channel that captures the timer value on the bus-in pin
    TimerMatch pwmChannel;       //!< The timer channel for PWM for sending
    TimerMatch timeChannel;      //!< The timer channel for timeouts
    volatile int sendAck;        //!< Send an acknowledge or not-acknowledge byte if != 0

private:
    volatile State state;          //!< The state of the lib's telegram sending/receiving
    volatile int sendTries;        //!< The number of repeats when sending a telegram
    volatile int sendTriesMax;     //!< The maximum number of repeats when sending a telegram. Default: 3 todo hora: load from EPROM
    volatile int sendBusyTries;    //!< The number of busy repeats when sending a telegram
    volatile int sendBusyTriesMax; //!< The maximum number of busy repeats when sending a telegram. Default: 3 todo hora: load from EPROM
    int nextByteIndex;             //!< The number of the next byte in the telegram

    int currentByte;                //!< The current byte that is received/sent, including the parity bit
    int sendTelegramLen;            //!< The size of the to be sent telegram in bytes (including the checksum).
    byte *sendCurTelegram;          //!< The telegram that is currently being sent.
    byte *rx_telegram = new byte[bcu->maxTelegramSize()](); //!< Telegram buffer for the L1/L2 receiving process

    int bitMask;
    int bitTime;                 //!< The bit-time within a byte when receiving
    int parity;                  //!< Parity bit of the current byte
    int valid;                   //!< 1 if parity is valid for all bits of the telegram
    int checksum;                //!< Checksum of the telegram: 0 if valid at end of telegram
    volatile unsigned short rx_error;    //!< hold the rx error flags of the rx process of the state machine
    volatile unsigned short tx_error;    //!< hold the tx error flags of the tx process of the state machine
    bool wait_for_ack_from_remote; //!< sending process is requesting an ack from remote side
    bool busy_wait_from_remote; //!< remote side is busy, re-send telegram after 150bit time wait
    bool repeatTelegram;        //!< need to repeat last  telegram sent
    bool collision;             //!< A collision occurred
};


/**
 * Create an interrupt handler for the EIB bus access. This macro must be used
 * once for every Bus object that is created. For the default bus object, this is
 * done.
 *
 * @param handler - the name of the interrupt handler, e.g. TIMER16_0_IRQHandler
 * @param busObj - the bus object that shall receive the interrupt.
 */
#define BUS_TIMER_INTERRUPT_HANDLER(handler, busObj) \
    extern "C" void handler() { busObj.timerInterruptHandler(); }

//
//  Inline functions
//
inline void Bus::maxSendTries(int tries)
{
    sendTriesMax = tries;
}

inline void Bus::maxSendBusyTries(int tries)
{
    sendBusyTriesMax = tries;
}

inline bool Bus::sendingTelegram() const
{
    return sendCurTelegram != nullptr;
}

inline bool Bus::telegramReceived() const
{
    return telegramLen != 0;
}

inline void Bus::discardReceivedTelegram()
{
    telegramLen = 0;
}

inline void Bus::end()
{
    pause(true);
}

#endif /*sblib_bus_h*/
