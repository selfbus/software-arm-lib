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
 * @file   bus_debug.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2023
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <sblib/eib/bus_debug.h>

#if defined(DEBUG_BUS) || defined(DEBUG_BUS_BITLEVEL) || defined (DUMP_TELEGRAMS)
    Timer& ttimer = timer32_0;
#endif

#ifdef DUMP_TELEGRAMS
    volatile unsigned char telBuffer[32];
    volatile unsigned int telLength = 0;
    volatile unsigned char txtelBuffer[32];
    volatile unsigned int txtelLength = 0;
    volatile unsigned int tx_rep_count = 0;
    volatile unsigned int tx_busy_rep_count = 0;
    volatile unsigned int tx_telrxerror = 0;

    volatile unsigned int telRXtime = 0;
    volatile bool telcollision;
    volatile unsigned int telrxerror = 0;
    volatile unsigned int telRXStartTime = 0;
    volatile unsigned int telTXStartTime = 0;
    volatile unsigned int telRXEndTime = 0;
    volatile unsigned int telTXEndTime = 0;
    volatile unsigned int telTXAck = 0;
    volatile unsigned int telRXWaitInitTime = 0;
    volatile unsigned int telRXWaitIdleTime = 0;
    volatile unsigned int telRXTelStartTime = 0;
    volatile unsigned int telRXTelByteStartTime = 0;
    volatile unsigned int telRXTelBitTimingErrorLate = 0;
    volatile unsigned int telRXTelBitTimingErrorEarly = 0;
    //volatile unsigned int db_state= 2000;
#   define DB_TELEGRAM(x) x
#else
#   define DB_TELEGRAM(x)
#endif

#if defined(DEBUG_BUS) || defined(DEBUG_BUS_BITLEVEL)
    volatile struct s_td td_b[tb_lngth]; // buffer to dump interrupt event data on the serial line
    volatile unsigned int tb_in = 0;
    volatile unsigned int tb_out = 0;
    volatile unsigned int tbd = 0;
    volatile bool tb_in_ov = false;
    volatile bool rec_end = false;
    volatile unsigned int b1=0;
    volatile unsigned int b2=0;
    volatile unsigned int b3=0;
    volatile unsigned int b4=0;
    volatile unsigned short b5=0;
#endif


#ifdef DUMP_TELEGRAMS
void dumpTelegrams()
{
    static unsigned int telLastRXEndTime = 0;
    static unsigned int telLastTXEndTime = 0;

    if (telTXAck)
    {
        serial.println("TXAck:", telTXAck, HEX, 2);
        telTXAck = 0;
    }

    //dump transmitting part
    if (telTXEndTime) // we transmitted a telegram
    {
        serial.print("TX : (S", telTXStartTime, DEC, 6);
        serial.print(" E", telTXEndTime, DEC, 6);

        if (telLastRXEndTime)
        {
            // print time in between last rx-tel and current tx-tel
            serial.print(" dt RX-TX:", (telTXStartTime - telLastRXEndTime), DEC, 8);
            telLastRXEndTime = 0;
        }
        else if(telLastTXEndTime)
        {
            // print time in between last tx-tel and current tx-tel
            serial.print(" dt TX-TX:", (telTXStartTime - telLastTXEndTime), DEC, 8);
            telLastTXEndTime = 0;
        }

        if (tx_telrxerror != 0)
        {
            serial.print(" err: 0x", tx_telrxerror, HEX, 4);
        }
        else
        {
            serial.print("  ok: 0x", tx_telrxerror, HEX, 4);
        }
        serial.print(" rep:", tx_rep_count, DEC, 1);
        serial.print(" brep:", tx_busy_rep_count, DEC, 1);
        serial.print(") ");

        //dump tx telegram data
        for (unsigned int i = 0; i < txtelLength; ++i)
        {
            if (i) serial.print(" ");
            serial.print(txtelBuffer[i], HEX, 2);
        }
        serial.println();

        telLastTXEndTime = telTXEndTime;
        telTXEndTime = 0;
        telTXStartTime = 0;
    }

/*
    //dump wait for idle stuff
    if (telRXWaitInitTime)
    {
        if (telLastTXEndTime)
        {
            serial.print("WI-TX:", (telRXWaitInitTime -telLastTXEndTime), DEC, 6);
            //serial.println("");
            telRXWaitInitTime = 0;
        }
        else if (telLastRXEndTime)
        {
            serial.print("WI-RX:", (telRXWaitInitTime - telLastRXEndTime), DEC, 6);
            //serial.println("");
            telRXWaitInitTime = 0;
        }
        else
        {
            serial.print("WI:", (telRXWaitInitTime ), DEC, 6);
            //serial.print(" ");
        }
        telRXWaitInitTime = 0;
    }
*/

/*
    if (telRXWaitIdleTime)
    {
        if (telLastTXEndTime)
        {
            serial.print("WID-TX:", (telRXWaitIdleTime - telLastTXEndTime), DEC, 6);
            serial.print(" ");
        }
        else if (telLastRXEndTime)
        {
            serial.print("WID-RX:", (telRXWaitIdleTime - telLastRXEndTime), DEC, 6);
            serial.print(" ");
        }
        else
        {
            serial.print("WID:", telRXWaitIdleTime, DEC, 6);
            serial.print(" ");
        }
        telRXWaitIdleTime = 0;
    }
*/

    //dump receiving bit error stuff
    //rx bit timing errors
    if (telRXTelBitTimingErrorLate)
    {
        serial.println(" ERL:", telRXTelBitTimingErrorLate, DEC, 6);
        telRXTelBitTimingErrorLate = 0;
    }
    if (telRXTelBitTimingErrorEarly)
    {
        serial.println(" ERE:", telRXTelBitTimingErrorEarly, DEC, 6);
        telRXTelBitTimingErrorEarly = 0;
    }

    //dump tel receiving part
    if (telLength > 0)
    {
        serial.print("RX : (S", telRXStartTime, DEC, 6 );
        serial.print(" E", telRXEndTime, DEC, 6);
        /*
        serial.print(") ");
        serial.print(", LRXE:", telLastRXEndTime, DEC, 9);
        serial.print(", LTXE:", telLastTXEndTime, DEC,9);
        serial.print(") ");
        */
        if (telLastTXEndTime)
        {
            // print time in between last tx-tel and current rx-tel
            serial.print(" dt TX-RX:", (telRXStartTime - telLastTXEndTime), DEC, 8);
            telLastTXEndTime = 0;
        }
        else if(telLastRXEndTime)
        {
            // print time in between last rx-tel and current rx-tel
            serial.print(" dt RX-RX:", (telRXStartTime - telLastRXEndTime), DEC, 8);
            //serial.println(") ");
            //telLastRXEndTime = 0;
        }

        if (telrxerror != 0)
        {
            serial.print(" err: 0x", telrxerror, HEX, 4);
        }
        else
        {
            serial.print("  ok: 0x", telrxerror, HEX, 4);
        }
        serial.print(") ");

        if (telcollision)
        {
            serial.print("collision ");
        }

        //dump tel data
        if (telLength > 1)
        {
            for (unsigned int i = 0; i < telLength; ++i)
            {
                if (i) serial.print(" ");
                serial.print(telBuffer[i], HEX, 2);
            }
        }
        else if (telLength == 1)
        {
            // maybe a LL_ACK, LL_NACK or LL_BUSY, try to decode it
            switch (telBuffer[0])
            {
            case 0xcc :
                serial.print("LL_ACK");
                break;
            case 0x0c :
                serial.print("LL_NACK");
                break;
            case 0xc0 :
                serial.print("LL_BUSY");
                break;
            default:
                serial.print(telBuffer[0], HEX, 2);
            }
        }
        serial.println();

        //reset all debug data
        telLength = 0;
        telLastRXEndTime = telRXEndTime;
        telRXEndTime = 0;
        telrxerror = 0;
        telRXTelByteStartTime = 0;
    }
}
#endif


#ifdef DEBUG_BUS
void debugBus()
{
    // trace buffer content:
    // trace point id (start with s) followed by trace data, coding: sittee
    // i: state machine trace point code
    //  0000-3999  one timer value
    //  4000-5999 one hex value
    //  6000-7999 one dec value
    //  8000-8999 all timer values at interrupt
    //  9000 - rx tel data values
    // tt: trace point number within certain state
    // ee: state of state machine at trace point

    static unsigned int t,l, l1, lt,lt1, s, cv,tv, tmv;
    bool cf;
    l=0; l1=0;
    while (tb_in != tb_out && l1 < 5)
    {
    //while (tb_in != tb_out) {
        l1++;
        s= td_b[tb_out].ts;
        t= td_b[tb_out].tt;
        tv= td_b[tb_out].ttv;
        cv= td_b[tb_out].tcv;
        tmv= td_b[tb_out].ttmv;
        cf= td_b[tb_out].tc;
        if ((s>=8000 && s<=8999) )
        {
            serial.println();
            serial.print("s", (unsigned int) s, DEC, 3);
            serial.print(" t", (unsigned int) t, DEC, 6);
            serial.print(" dt", (unsigned int) t-lt, DEC,4);
            serial.print(" f", (unsigned int)cf, DEC, 1);
            serial.print(" c", (unsigned int)cv, DEC, 4);
            serial.print(" t", (unsigned int)tv, DEC, 4);
            serial.print(" m", (unsigned int)tmv, DEC,4);
            /*
            serial.print(" i", (unsigned int)tb_in, DEC,3);
            serial.print(" o", (unsigned int)tb_out, DEC,3);
            serial.print("*");
            */

            l=1;
            lt = t;
            lt1= t;
        }
        else if (s>=9000)
        {
            serial.println();
            serial.print("s", (unsigned int) s, DEC, 3);
            serial.print(" c/v", (unsigned int)cf, HEX, 2);
            serial.print(" L", (unsigned int)tmv, DEC, 2);
            serial.print(" t", (unsigned int) t, HEX, 8);
            serial.print(" ", (unsigned int)cv, HEX, 4);
            serial.print(" ", (unsigned int)tv, HEX, 4);
            //serial.print("*");
        }
        else if (s>=9005)
        {
            serial.println();
            serial.print("s");
            serial.print( (unsigned int) s, DEC, 3);
            serial.print(" ");
            serial.print((unsigned int)tmv, HEX,4);
            serial.print(" ");
            serial.print( (unsigned int) t, HEX, 8);
            serial.print(" ");
            serial.print((unsigned int)cv, HEX, 4);
            serial.print(" ");
            serial.print((unsigned int)tv, HEX, 4);
            serial.print(" d");
            serial.print((unsigned int)cf, HEX, 4);
            //serial.print("*");
        }
        else if (s < 4000)
        { // one  delta timer
            serial.print("s", (unsigned int) s -2000, DEC, 3);
            serial.print(" dt", (unsigned int) t-lt1, DEC, 6);
            lt1 = t;
            l++;
        }
        else if (s < 5000)
        { // one hex
            serial.print("s", (unsigned int) s- 4000, DEC, 3);
            serial.print(" h", (unsigned int) t,HEX,4);
            l++;
        }
        else if (s < 6000)
        { // one dec
            serial.print("s", (unsigned int) s- 5000, DEC, 3);
            serial.print(" d", (unsigned int) t,DEC,4);
            l++;
        }

        if (l >5)
        {
            l=0;
            serial.println();
            //serial.print("* ");
        }
        else
        {
            serial.print(" ");
        }

        if (++tb_out >= tb_lngth)
        {
            tb_out =0;
            tb_in_ov = false;
        }

        // buffer overflow
        if (tb_in_ov && tb_out <= tb_in)
        {
            serial.print(" !!OV**");
        }
    }
}
#endif

/** @}*/