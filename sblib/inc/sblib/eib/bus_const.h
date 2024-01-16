/*
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *  Copyright (c) 2021 Horst Rauch
 *
 *  Refactoring and bug fixes:
 *  Copyright (c) 2024 Darthyson <darth@maptrack.de>
 *  Copyright (c) 2024 Thomas Dallmair <dev@thomas-dallmair.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef SBLIB_KNX_BUS_CONST_H_
#define SBLIB_KNX_BUS_CONST_H_

/**
 * Data link layer short acknowledgment frames
 */
enum ShortAcknowledgeFrame
{
    SB_BUS_ACK       = 0xcc, //!< Acknowledged
    SB_BUS_NACK      = 0x0c, //!< Not acknowledged
    SB_BUS_BUSY      = 0xc0, //!< Busy
    SB_BUS_NACK_BUSY = 0x00, //!< Not acknowledged & busy. Shall be handled as @ref SB_BUS_BUSY
};

/**
 * Rx error code values
 */
enum RxErrorCode
{
    RX_OK                     = 0,   //!< No Rx error
    RX_STOPBIT_ERROR          = 1,   //!< we received a cap event during stop bit
    RX_TIMING_ERROR           = 2,   //!< received edge of bits is not in the timing window n*104-7 - n*104+33
    RX_TIMING_ERROR_SPIKE     = 4,   //!< received edge of bit with incorrect timing
    RX_PARITY_ERROR           = 8,   //!< parity not valid
    RX_CHECKSUM_ERROR         = 16,  //!< checksum not valid
    RX_LENGTH_ERROR           = 32,  //!< received number of byte does not match length value of telegram
    RX_BUFFER_BUSY            = 64,  //!< rx buffer still busy by higher layer process while a new telegram was received
    RX_INVALID_TELEGRAM_ERROR = 128, //!< we received something but not a valid tel frame: to short,  to long, spike
    RX_PREAMBLE_ERROR         = 256, //!< first char we received has invalid value in bit 0 and bit 1
};

/**
 * Tx error code values
 */
enum TxErrorCode
{
    TX_OK                  = 0,   //!< No Tx error
    TX_STARTBIT_BUSY_ERROR = 1,   //!< bus is busy few us before we intended to send a start bit
    TX_NACK_ERROR          = 4,   //!< we received a NACK
    TX_ACK_TIMEOUT_ERROR   = 8,   //!< we did not received an ACK after sending in the respective time window
    TX_REMOTE_BUSY_ERROR   = 16,  //!< AFTER SENDING, REMOTE SIDE SENDS busy BACK
    TX_PWM_STARTBIT_ERROR  = 32,  //!< we could not receive our send start bit as cap event, possible HW fault
    TX_COLLISION_ERROR     = 64,  //!< collision, we tried to send high bit but low bit was detected- send from other device
    TX_RETRY_ERROR         = 128, //!< max number of retries reached, tx process terminated
};

/* L1/L2 msg header control field data bits meaning */
#define ALWAYS0            0 //!< bit 0 is always 0
#define ACK_REQ_FLAG       1 //!< 0 LL_ACK is requested - for TP-Bus ACK is mandatory for normal telegrams - always 0
#define PRIO0_FLAG         2 //!< 00 system priority, 10 alarm priority, 01 high priority, 11 low priority
#define PRIO1_FLAG         3
#define ACK_FRAME_FLAG     4 //!< 1 frame is data or poll, 0 ack frame
#define REPEAT_FLAG        5 //!< 1 not repeated, 0 repeated frame
#define DATA_FRAME_FLAG    6 //!< 1 poll data frame, 0 data frame
#define LONG_FRAME_FLAG    7 //!< 1,  short frame <=15 char, 0 long frame > 15 char

// telegram control byte bit definitions
#define SB_TEL_NULL_FLAG        (1 << ALWAYS0)
#define SB_TEL_ACK_REQ_FLAG     (1 << ACK_REQ_FLAG)
#define SB_TEL_PRIO0_FLAG       (1 << PRIO0_FLAG)
#define SB_TEL_PRIO1_FLAG       (1 << PRIO1_FLAG)
#define SB_TEL_ACK_FRAME_FLAG   (1 << ACK_FRAME_FLAG)
#define SB_TEL_REPEAT_FLAG      (1 << REPEAT_FLAG)
#define SB_TEL_DATA_FRAME_FLAG  (1 << DATA_FRAME_FLAG)
#define SB_TEL_LONG_FRAME_FLAG  (1 << LONG_FRAME_FLAG)
#define SB_TEL_PRIO_FLAG        (SB_TEL_PRIO0_FLAG | SB_TEL_PRIO1_FLAG)

// we accept only normal data frames for telegram processing
#define VALID_DATA_FRAME_TYPE_MASK (SB_TEL_LONG_FRAME_FLAG | SB_TEL_DATA_FRAME_FLAG | SB_TEL_ACK_FRAME_FLAG | SB_TEL_ACK_REQ_FLAG | SB_TEL_NULL_FLAG)
#define VALID_DATA_FRAME_TYPE_VALUE  (SB_TEL_LONG_FRAME_FLAG | SB_TEL_ACK_FRAME_FLAG )

#define PREAMBLE_MASK  (( 1<< ALWAYS0) | ( 1<< ACK_REQ_FLAG))

#define PRIO_FLAG_HIGH     (SB_TEL_PRIO0_FLAG)


#define NACK_RETRY_DEFAULT  3   //!< default NACK retry
#define BUSY_RETRY_DEFAULT  3   //!< default BUSY retry
//#define ROUTE_CNT_DEFAULT 6     //!< default Route Count


/* Timing related macros */

#define TICKS_PER_SECOND    1000000                                  //!< Timer is in microseconds (usec)
#define TIMER_PRESCALER     (SystemCoreClock / TICKS_PER_SECOND - 1) //!< The value for the prescaler
#define BIT_TIMES(x)        (((x) * TICKS_PER_SECOND + 4800) / 9600) //!< Microseconds of x bits on the bus, with integer rounding

#define BUS_STARTBIT_OFFSET_MIN 7  //!< >=7 us before expected start bit check for bus busy
#define BUS_STARTBIT_OFFSET_MAX 33 //!< <=33 us after expected start bit is still ok

#define BIT_TIME            BIT_TIMES(1)               //!< Default time between two bits (104 usec)
#define BIT_WAIT_TIME       ((BIT_TIME * 4 + 3) / 6)   //!< Time between two bits (69 usec) - high level part of the pulse on the bus
#define BIT_PULSE_TIME      (BIT_TIME - BIT_WAIT_TIME) //!< Pulse duration of a bit (35 usec)

#define BYTE_TIME_INCL_STOP BIT_TIMES(11) //!< Maximum time from start bit to end of stop bit: BIT_TIME*11
#define BYTE_TIME_EXCL_STOP BIT_TIMES(10) //!< Maximum time from start bit to start of stop bit: BIT_TIME*10

/**
 * Maximum time from end of stop bit to start bit of next byte = timeout for end of telegram,
 * KNX Spec. 2.1 3/2/2 p. 35 figure 40, inner_frame_char
 */
#define MAX_INTER_CHAR_TIME (BIT_TIMES(2) + 30)

#define SEND_ACK_WAIT_TIME  BIT_TIMES(15) //!< Time to wait before sending an ACK after valid rx telegram: 15* BIT Time

/**
 * For rx process: Time from end of stop bit to start of ACK = 15* bit time -5us/+30us according to spec.
 * We add an marging of 100us as we have seen some early acks on the bus
 */
#define ACK_WAIT_TIME_MIN   (BIT_TIMES(15) - 5 - 100)
#define ACK_WAIT_TIME_MAX   (BIT_TIMES(15) +30 + 100)

#define WAIT_50BIT_FOR_IDLE BIT_TIMES(50) //!< Time to wait before bus is in idle state: BIT_TIME * 50
#define SEND_WAIT_TIME      BIT_TIMES(50) //!< Time to wait before starting to send: BIT_TIME * 50
#define RANDOMIZE_FACTOR    131           //!< Factor for calculation of randomized additional time before starting to send
#define RANDOMIZE_MODULUS   BIT_TIMES(3)  //!< Modulus for calculation of randomized additional time before starting to send

//!< Time to wait before repetition of sending a telegram due to busy from remote side  BIT_TIME * 150
#define BUSY_WAIT_150BIT    BIT_TIMES(150)

#define PRE_SEND_TIME       BIT_TIMES(1) //!< Time to listen for bus activity before sending starts: BIT_TIME * 1

#endif /* SBLIB_KNX_BUS_CONST_H_ */

