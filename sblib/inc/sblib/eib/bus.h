/*
 *  bus.h - Low level EIB bus access.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *
 * last update: March 2021, Hora,  added some states and debug data definition
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_bus_h
#define sblib_bus_h

#include <sblib/core.h>
#include <sblib/eib/bcu_type.h>

// dump all received and sent telegrams out on the serial interface
#if defined (DUMP_TELEGRAMS) || defined(DEBUG_BUS) || defined(DEBUG_BUS_BITLEVEL)

#include <sblib/serial.h>

#endif

#if defined (DUMP_TELEGRAMS) || defined(DEBUG_BUS) || defined(DEBUG_BUS_BITLEVEL)

struct s_td {
	unsigned short ts; // state
	unsigned int tt;	// system time
	unsigned short tcv; // capture value
	unsigned short ttmv; // timer match value
	unsigned short ttv; // timer value
	unsigned short tc; // capture flag
};

#define tb_lngth 300

extern volatile struct s_td td_b[tb_lngth];
extern volatile unsigned int tb_in ;
extern volatile unsigned int tb_out;
extern volatile unsigned int tbd;
extern volatile bool  tb_in_ov;

#endif


class Bus;

/**
 * The EIB bus access object.
 */
extern Bus bus;


/**
 * Bus short acknowledgment frame: acknowledged
 */
#define SB_BUS_ACK 0xcc

/**
 * Bus short acknowledgment frame: not acknowledged
 */
#define SB_BUS_NACK 0x0c

/**
 * Bus short acknowledgment frame: busy
 */
#define SB_BUS_BUSY 0xc0

/**
 * Bus short acknowledgment frame: not acknowledged & busy
 * Shall be handled as SB_BUS_BUSY
 */
#define SB_BUS_NACK_BUSY 0x00


// The size of the telegram buffer in bytes
#define SB_TELEGRAM_SIZE 24

/**
 * Test if we are in programming mode (the button on the controller is pressed and
 * the red programming LED is on).
 *
 * @return 1 if in programming mode, 0 if not.
 */
#define sb_prog_mode_active() (sbUserRam->status & 1)


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
    Bus(Timer& timer, int rxPin, int txPin, TimerCapture captureChannel, TimerMatch matchChannel);

    /**
     * Begin using the bus.
     *
     * This powers on all used components.
     * This method must be called before the bus can be used.
     */
    void begin();

    /**
     * End using the bus.
     *
     * This powers the bus off.
     */
    void end();

    /**
     * Test if the bus is idle, no telegram is about to being sent or received.
     *
     * @return true when idle, false when not.
     */
    bool idle() const;

    /**
     *    Interface to upper layer for sending a telegram
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
        * Get the state of the last send Telegram
        *
        * @return  0 if no error, or tx error state
        */
    int sendTelegramState() const;

    /**
          * Get the state of the last received Telegram
          *
          * @return  0 if no error, or rx error state
          */
    int receivedTelegramState() const;


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
     * Get our own physical address.
     */
    int ownAddress() const;

    /**
     * Set weather the an acknowledgment from the last received byte should be sent.
     * !!!!!!! critical as this could change the sendAck value during usage in the  SM - should be not used outside the bus SM!!
     *  not needed as the SM should check the bit1 in the telegram header to check if the sender is requesting an ACK
     */
    void setSendAck(int sendAck);

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

    	INIT,	//! The Lib is initializing, waiting for 50bit time inactivity on the bus
		IDLE,	//!< The lib is idle. there was no receiving or sending for at least 50 bit times. only cap intr enabled, no timeout intr
		RECV_WAIT_FOR_STARTBIT_OR_TELEND,	//!< The lib is receiving a byte. cap intr  (low bit received) and timeout (stop bit: char end)) intr enabeld
		RECV_BITS_OF_BYTE,	//!< The lib is waiting for the start bit window of the next byte. only timeout enabled
		RECV_WAIT_FOR_NEXT_CHAR_WINDOW,	//!< After Tel is received the lib is waiting for  begin of window for sending an ACK to remote side.
		RECV_WAIT_FOR_TX_ACK_WINDOW,	//!< After Tel is received the lib is waiting for start sending an ACK to remote side.
		RECV_WAIT_FOR_ACK_TX_START,	//!< Timeout event: Start sending the telegram (also triggered in sbSendTelegram[]), cap event: start RX of a new tel
		WAIT_FOR_NEXT_RX_OR_PENDING_TX, 	//!< Send a start bit
		SEND_START_BIT,	//!< Send the first bit of the current byte
		SEND_BIT_0,	//!< Send the bits of the current byte
		SEND_BITS_OF_BYTE,	//!< Send high bit(s) and wait for receiving a the falling edge of our next 0-bit.
		SEND_WAIT_FOR_HIGH_BIT_END,	//!< Wait between two transmissions, cap disabled, timeout: start sending next char
		SEND_WAIT_FOR_NEXT_TX_CHAR,	//!< Finish sending current byte
		SEND_END_OF_TX,	//!< after sending we wait for the ack receive window to start, only timeout event enabled
		SEND_WAIT_FOR_RX_ACK_WINDOW,	//!< after sending we wait for the ack in the ack receive window, cap event: rx start, timeout: repeat tel
		SEND_WAIT_FOR_RX_ACK,
		STATE_END
    };

    enum
    {
        TELEGRAM_SIZE = 24   //!< The maximum size of a telegram including the checksum byte
    };

    /**
     * The received telegram.
     * The higher layer process should not change the telegram data in the buffer!
     */
    byte telegram[TELEGRAM_SIZE];

    /**
      * The total length of the received telegram in telegram[].
      */
     volatile int telegramLen;

     /**
       * The result of the rx process on the bus.
       */
      volatile int bus_rx_state;

      /**
        * The result of the tx process on the bus.
        */
       volatile int bus_tx_state;


#if defined(DEBUG_BUS) || defined(DEBUG_BUS_BITLEVEL) || defined (DUMP_TELEGRAMS)
    Timer& ttimer = timer32_0;                //!< The debug timer for SM timing
#endif


private:
    /**
     * Switch to idle state
     */
    void idleState();

    /**
     * Switch to the next telegram for sending.
     *
     * mark the current send buffer as free -> set first byte of buffer to 0
     * load a possible waiting/next buffer to current buffer
     * initialize some low level parameters for the interrupt driven send process
     *
     */
    void sendNextTelegram();

    /**
     * Prepare the telegram for sending. Set the sender address to our own
     * address, and calculate the checksum of the telegram.
     * Stores the checksum at telegram[length].
     *
     * @param telegram - the telegram to process
     * @param length - the length of the telegram
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


    /** !!!!!!!!!!! not available in code !!!!!!!!!!!!
       * Sending Process is waiting for an acknowledgment.
       * Handle the received byte(s) on a low level. Repeat the last send Telegram in case of invalid ack
       *  This function is called by the function TIMER16_1_IRQHandler() to decide about repetition
       *  of last sent Telegram.
       *
       * @param valid - 1 if (all) bytes had correct parity, 0 if not
       */

    void handleAckTelegram(bool valid);



protected:
    friend class BcuBase;
    Timer& timer;                //!< The timer
    int rxPin, txPin;            //!< The pins for bus receiving and sending
    TimerCapture captureChannel; //!< The timer channel that captures the timer value on the bus-in pin
    TimerMatch pwmChannel;       //!< The timer channel for PWM for sending
    TimerMatch timeChannel;      //!< The timer channel for timeouts
    volatile int ownAddr;                 //!< Our own physical address on the bus
    volatile int sendAck;                 //!< Send an acknowledge or not-acknowledge byte if != 0

private:
    volatile State state;                 //!< The state of the lib's telegram sending/receiving
    volatile int sendTries;               //!< The number of repeats when sending a telegram
    volatile int sendTriesMax;            //!< The maximum number of repeats when sending a telegram. Default: 3 todo hora: load from EPROM
    volatile int sendBusyTries;           //!< The number of busy repeats when sending a telegram
    volatile int sendBusyTriesMax;        //!< The maximum number of busy repeats when sending a telegram. Default: 3 todo hora: load from EPROM
    int nextByteIndex;           //!< The number of the next byte in the telegram

    int currentByte;             //!< The current byte that is received/sent, including the parity bit
    int sendTelegramLen;         //!< The size of the to be sent telegram in bytes (including the checksum).
    volatile byte *sendCurTelegram;       //!< The telegram that is currently being sent.
    volatile byte *sendNextTel;           //!< The telegram to be sent after sbSendTelegram is done.
    volatile byte rx_telegram[TELEGRAM_SIZE]; // !< Telegram buffer for the L1/L2 receiving process

    int bitMask;
    int bitTime;                 // The bit-time within a byte when receiving
    int parity;                  // Parity bit of the current byte
    int valid;                   // 1 if parity is valid for all bits of the telegram
    int checksum;                // Checksum of the telegram: 0 if valid at end of telegram
    unsigned short rx_error;	// hold the rx error flags of the rx process of the state machine
    unsigned short tx_error;	// hold the tx error flags of the tx process of the state machine
    bool wait_for_ack_from_remote; // sending process is requesting an ack from remote side
   // bool need_to_send_ack_to_remote; // receiving process need to send ack to remote sending side
    bool busy_wait_from_remote; // remote side is busy, re-send telegram after 150bit time wait
   // bool busy_wait_to_remote; // receiving process/ upper layer busy, send busy to remote sender
    bool repeated;              // A send telegram is repeated
    bool repeatTelegram;        // need to repeat last  telegram sent
    bool collision;              // A collision occurred
    unsigned int lastRXTimeVal; // time measurement between telegrams - last SysTime value


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

/**
 * Get the size of a telegram, including the protocol header but excluding
 * the checksum byte. The size is calculated by getting the length from byte 5 of the
 * telegram and adding 7 for the protocol overhead.
 *
 * @param tel - the telegram to get the size
 *
 * @return The size of the telegram, excluding the checksum byte.
 */
#define telegramSize(tel) (7 + (tel[5] & 15))


//define some error states
#define RX_OK 0
#define RX_STARTBIT_ERROR 1  			// we detected high level few us after cap. event of start bit
#define RX_STOPBIT_ERROR 2 			// we received a cap event during stop bit
#define RX_TIMING_ERROR 4			// received edge of bits is not in the timing window n*104-7 - n*104+33
#define RX_INVALID_TELEGRAM_ERROR 8	// we received something but not a valid tel frame: to short,  to long, spike
#define RX_PARITY_ERROR 16			// parity not valid
#define RX_CHECKSUM_ERROR 32		// checksum not valid
#define RX_LENGHT_ERROR 64			// checksum not valid
#define RX_BUFFER_BUSY 128			// rx buffer still busy by higher layer process while a new telegram was received

#define TX_OK 0
#define TX_STARTBIT_BUSY_ERROR 1	// bus is busy few us before we intended to send a start bit
#define TX_TIMING_ERROR 2			// error during the sending of bits (low bit after high bit was not detected
#define TX_NACK_ERROR 4				// we received a NACK
#define TX_ACK_TIMEOUT_ERROR 8		// we did not received an ACK after sending in the respective time window
#define TX_REMOTE_BUSY_ERROR 16		// AFTER SENDING, REMOTE SIDE SENDS busy BACK
#define TX_PWM_STARTBIT_ERROR 32	// we could not receive our send start bit as cap event, possible HW fault
#define TX_COLLISION_ERROR 64		// collision, we tried to send high bit but low bit was detected- send from other device
#define TX_RETRY_ERROR 128			// max number of retries reached, tx process terminated


//
//  Inline functions
//

inline bool Bus::idle() const
{
    return state == IDLE && sendCurTelegram == 0;
}

inline int Bus::ownAddress() const
{
    return bus.ownAddr;
}

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
    return sendCurTelegram != 0;
}

inline bool Bus::telegramReceived() const
{
    return telegramLen != 0;
}

inline int Bus::receivedTelegramState() const
{
    return bus_rx_state;
}

inline int Bus::sendTelegramState() const
{
    return bus_tx_state;
}

inline void Bus::discardReceivedTelegram()
{
    telegramLen = 0;
}

inline void Bus::end()
{
}

inline void  Bus::setSendAck(int sendAck)
{
	this->sendAck = sendAck;
}
#endif /*sblib_bus_h*/
