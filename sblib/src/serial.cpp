/**
 * @file serial.cpp
 * @brief LPC11xx Serial port driver
 *
 * @author Stefan Taferner <stefan.taferner@gmx.at> Copyright (c) 2014
 * @author HoRa  Copyright (c) March 2021
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 *
 * @bug No known bugs.
 *
 * @note Interrupt priority set to lowest level in order to avoid conflicts
 *       with the time critical knx bus interrupt source value for high
 *       speed baud rates for debugging of bus timing
 *
 * @par
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include <sblib/serial.h>
#include <sblib/digital_pin.h>
#include <sblib/interrupt.h>
#include <sblib/platform.h>

#define LSR_RDR  0x01   //!> UART line status: receive data ready bit: RBR holds an unread character
#define LSR_OE   0x02   //!> UART line status: overrun error bit
#define LSR_PE   0x04   //!> UART line status: parity error bit
#define LSR_FE   0x08   //!> UART line status: framing error bit
#define LSR_BI   0x10   //!> UART line status: break interrupt bit
#define LSR_THRE 0x20   //!> UART line status: the transmitter hold register (THR) is empty
#define LSR_TEMT 0x40   //!> UART line status: transmitter empty (THR and TSR are empty)
#define LSR_RXFE 0x80   //!> UART line status: error in RX FIFO
#define UART_IE_RBR 0x01    //!> UART read-buffer-ready interrupt
#define UART_IE_THRE 0x02   //!> UART transmit-hold-register-empty interrupt

Serial::Serial(int rxPin, int txPin) :
 enabled_(false)
{
	setRxPin(rxPin);
	setTxPin(txPin);
}

void Serial::setRxPin(int rxPin)
{
    //FIXME check is it allowed to change the rxPin while port is active?
    pinMode(rxPin, SERIAL_RXD);
}

void Serial::setTxPin(int txPin)
{
    //FIXME check is it allowed to change the rxPin while port is active?
    pinMode(txPin, SERIAL_TXD);
}

void Serial::begin(int baudRate, SerialConfig config)
{
    disableInterrupt(UART_IRQn);

    LPC_SYSCON->SYSAHBCLKCTRL |= 1 << 12; // Enable UART clock
    LPC_SYSCON->UARTCLKDIV = 1;           // divided by 1

    LPC_UART->LCR = 0x80 | config;

    unsigned int val = SystemCoreClock * LPC_SYSCON->SYSAHBCLKDIV /
        LPC_SYSCON->UARTCLKDIV / 16 / baudRate;

    if (baudRate == 460800) //FIXME works only with SystemCoreClock=48000000 ?
    {
        val = 5;
        LPC_UART->FDR = ( 0x00a3);  //DIVADDVAL = 3, MULVAL = 10
    }
    else if( baudRate == 576000) //FIXME works only with SystemCoreClock=48000000 ?
    {
        val = 3;
        LPC_UART->FDR = (0x00fb);  //DIVADDVAL = 11, MULVAL = 15
    }

	LPC_UART->DLM  = val / 256;
    LPC_UART->DLL  = val % 256;

    LPC_UART->LCR = (int) config;  // Configure data bits, parity, stop bits
    LPC_UART->FCR = 0x07;          // Enable and reset TX and RX FIFO.
    LPC_UART->MCR = 0;             // Disable modem controls (DTR, DSR, RTS, CTS)
    LPC_UART->IER |= UART_IE_RBR;  // Enable RX/TX interrupts

    // Ensure a clean start, no data in either TX or RX FIFO
    clearBuffers();
    flush();

    // Drop data from the RX FIFO
    while (LPC_UART->LSR & LSR_RDR)
        val = LPC_UART->RBR;

    //added by Hora in order to provide the highest interrupt level to the bus timer of the lib
    NVIC_SetPriority (UART_IRQn, 3);

    enableInterrupt(UART_IRQn);
    enabled_ = true;
}

void Serial::end()
{
    flush();
    disableInterrupt(UART_IRQn);
    LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << 12); // Disable UART clock
    enabled_ = false;
}

int Serial::write(byte ch)
{
    if (!enabled_)
    {
        return 0;
    }

#ifdef SERIAL_WRITE_DIRECT

    // wait until the transmitter hold register is free
   while (!(LPC_UART->LSR & LSR_THRE))
       ;
   LPC_UART->THR = ch;
   return 1;
#else

    if (writeHead == writeTail && (LPC_UART->LSR & LSR_THRE))
    {
        // Transmitter hold register and write buffer are empty -> directly send
        LPC_UART->THR = ch;
        LPC_UART->IER |= UART_IE_THRE;
        return 1;
    }

    int writeTailNext = (writeTail + 1) & BufferedStream::BUFFER_SIZE_MASK;

    // Wait until the output buffer has space
    while (writeHead == writeTailNext)
        ;

    writeBuffer[writeTail] = ch;
    writeTail = writeTailNext;
    LPC_UART->IER |= UART_IE_THRE;

    return 1;

#endif
}

void Serial::flush(void)
{
    if (!enabled_)
    {
        return;
    }
#ifdef SERIAL_WRITE_DIRECT
    while ((LPC_UART->LSR & (LSR_THRE|LSR_TEMT)) != (LSR_THRE|LSR_TEMT))
        ;
#else
    while (writeHead != writeTail)
    		;
#endif
}

int Serial::read()
{
    if (!enabled_)
    {
        return -1;
    }

    bool readFull = readBufferFull();
    int ch = BufferedStream::read();

    if (readFull && (LPC_UART->LSR & LSR_RDR))
    {
        disableInterrupt(UART_IRQn);
        interruptHandler();
        enableInterrupt(UART_IRQn);
    }

    return ch;
}

void Serial::interruptHandler()
{
    //FIXME check if this is save to activate
    /*
    if (!enabled_)
    {
        return;
    }
    */

    if (LPC_UART->LSR & LSR_THRE)
    {
        if (writeHead == writeTail)
        {
            LPC_UART->IER &= ~UART_IE_THRE;
        }
        else
        {
            LPC_UART->THR = writeBuffer[writeHead];

            ++writeHead;
            writeHead &= BufferedStream::BUFFER_SIZE_MASK;
        }
    }

    while (LPC_UART->LSR & LSR_RDR)
    {
        if  (!readBufferFull())
        {
            readBuffer[readTail] = LPC_UART->RBR;

            ++readTail;
            readTail &= BufferedStream::BUFFER_SIZE_MASK;
        } else {
            LPC_UART->RBR; // if the readBuffer ist full, empty UART
        }
    }
}
