/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Selfbus KNX-Library
 * @defgroup SBLIB_SUB_GROUP_KNX KNX TP1 debugging
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   
 * @details 
 *
 *
 * @{
 *
 * @file   bus_debug.h
 * @author HoRa Copyright (c) 2021
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2023
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#ifndef SBLIB_KNX_BUS_DEBUG_H_
#define SBLIB_KNX_BUS_DEBUG_H_

#include <sblib/libconfig.h>

#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
#endif

#if defined(DEBUG_BUS) || defined(DEBUG_BUS_BITLEVEL) || defined (DUMP_TELEGRAMS)
    #include <sblib/timer.h>
#endif

#if defined(DEBUG_BUS) || defined(DEBUG_BUS_BITLEVEL) || defined (DUMP_TELEGRAMS)
    extern Timer& ttimer; //!< The debug timer for state machine timing
#endif

/** @def tb_lngth trace buffer length @warning each trace buffer needs 16 bytes. So change with RAM space in mind */
#define tb_lngth 300

#ifdef DUMP_TELEGRAMS
    extern volatile unsigned char telBuffer[32]; //!< buffer to dump a rx telegram info to the serial line including collisions and rx-timing (in ms)
    extern volatile unsigned int telLength;
    extern volatile unsigned char txtelBuffer[32];
    extern volatile unsigned int txtelLength;
    extern volatile unsigned int tx_rep_count;
    extern volatile unsigned int tx_busy_rep_count;
    extern volatile unsigned int tx_telrxerror;

    extern volatile uint8_t telcollisions;
    extern volatile unsigned int telrxerror;
    extern volatile unsigned int telRXStartTime; //!< time when the reception  (start bit of first byte) of a telegram from bus started
    extern volatile unsigned int telTXStartTime; //!< time when the transmission (start bit of first byte)of a telegram to the bus started
    extern volatile unsigned int telRXEndTime; //!< time when the reception  (last stop bit) of a telegram from bus ended
    extern volatile unsigned int telTXEndTime; //!< time when the transmission (last stop bit)of a telegram to the bus ended
    extern volatile bool telRXNotProcessed; //!< received telegram was not processed as it does not affect us
    extern volatile unsigned int telTXAck; //!< ack send by L2
    extern volatile unsigned int telRXWaitInitTime; //!< Wait for 50 bit time after last RX/TX telegram, could be less- rx will be ignored
    extern volatile unsigned int telRXWaitIdleTime; //!< bus is in idle after 50 bit times, now wait for next start of RX/TX
    extern volatile unsigned int telRXTelStartTime; //!< Start Time of a telegram
    extern volatile unsigned int telRXTelByteStartTime; //!< Start Time of a byte within a telegram
    extern volatile unsigned int telRXTelByteEndTime; //!< End Time of a byte within a telegram
    extern volatile unsigned int telRXTelBitTimingErrorLate; //!< late bit error - falling rx edge was not in the expected window >33us
    extern volatile unsigned int telRXTelBitTimingErrorEarly; //!< early bit error - falling rx edge was not in the expected window -7us till +33us
    //volatile unsigned int db_state= 2000;
#endif

#ifdef DUMP_TELEGRAMS

/**
 * Send bus telegrams to serial port inclusive some debugging informations
 */
void dumpTelegrams();

#endif

#if defined(DEBUG_BUS) || defined(DEBUG_BUS_BITLEVEL)
    struct s_td {
        unsigned short ts;   //!< state
        unsigned int tt;     //!< system time
        unsigned short tcv;  //!< capture value
        unsigned short ttmv; //!< timer match value
        unsigned short ttv;  //!< timer value
        unsigned short tc;   //!< capture flag
    };

    extern volatile unsigned int b1;
    extern volatile unsigned int b2;
    extern volatile unsigned int b3;
    extern volatile unsigned int b4;
    extern volatile unsigned short b5;
    extern volatile struct s_td td_b[]; //!< buffer to dump interrupt event data on the serial line
    extern volatile unsigned int tb_in;
    extern volatile unsigned int tb_out;
    extern volatile unsigned int tbd;
    extern volatile bool tb_in_ov;
    extern volatile bool rec_end;

    // dump telegram data in buffer
#   define tb2(s, ptc, t,  ptcv, pttv, pttmv,  i) \
            td_b[i].ts=s; \
            td_b[i].tt=t; \
            td_b[i].tc=ptc; \
            td_b[i].tcv=ptcv; \
            td_b[i].ttv=pttv; \
            td_b[i].ttmv=pttmv;  \
            if ( ++i>=tb_lngth) \
            { \
                i=0; tb_in_ov = true; \
            }

    //dump a timer value
    #define tb_t(s, t,  i ) \
            td_b[i].ts=s+2000; \
            td_b[i].tt=t; \
            if (++i>=tb_lngth) \
            { \
                i=0; \
                tb_in_ov = true; \
            }

    // dump a hex value
#   define tb_h(s, t,  i ) \
        td_b[i].ts=s+4000; \
        td_b[i].tt=t; \
        if (++i>=tb_lngth) \
        { \
            i=0; \
            tb_in_ov = true; \
        }

    // dump a decimal value
#   define tb_d(s, t,  i ) \
        td_b[i].ts=s+5000; \
        td_b[i].tt=t; \
        if (++i>=tb_lngth) \
        { \
            i=0; \
            tb_in_ov = true; \
        }

#   ifdef DEBUG_BUS_BITLEVEL
        //dump each interrupt with timer data
#       define tbint(d1, d2, d3, d4, d5, d6, i)  \
            td_b[i].ts=d1; \
            td_b[i].tt=d2; \
            td_b[i].tc=d3; \
            td_b[i].tcv=d4; \
            td_b[i].ttv=d5; \
            td_b[i].ttmv=d6; \
            if (++i>=tb_lngth) \
            { \
                i=0; \
                tb_in_ov = true; \
            }

#   else
        //dump on bit level only first bit interrupt to reduce possible buffer overflows
#       define tbint(s, t, cf, cv, tv, tmv,  i) \
            if (!( (s==8000 + RECV_BITS_OF_BYTE || s==8000 + SEND_BITS_OF_BYTE ) && !rec_end) ) \
            { \
                if (s==800 + RECV_WAIT_FOR_STARTBIT_OR_TELEND || s==8000 + SEND_BIT_0) \
                    rec_end = true; \
                else \
                    rec_end = false; \
                td_b[i].tt=t; \
                td_b[i].tc=cf; \
                td_b[i].tcv=cv; \
                td_b[i].ttv=tv; \
                td_b[i].ttmv=tmv; \
                td_b[i].ts=s; \
                if (++i>=tb_lngth) \
                { \
                    i=0; \
                    tb_in_ov = true; \
                } \
            }
#   endif
#else
    // no dump at all
#    define tb2(s, ptc, t,  ptcv, pttv, pttmv,  i) ;
#    define tb_t(s, t,  i ) ;
#    define tb_h(s, t,  i ) ;
#    define tb_d(s, t,  i ) ;
#    define tbint(s, t, cf, cv, tv, tmv,  i) ;
#endif

#if defined(DEBUG_BUS) || defined(DEBUG_BUS_BITLEVEL)
    /**
     * Send bus state machine informations to serial port
     */
    void debugBus();
#endif

#endif /* SBLIB_KNX_BUS_DEBUG_H_ */
/** @}*/
