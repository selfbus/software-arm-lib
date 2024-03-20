/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @addtogroup SBLIB_BOOTLOADER_MAIN Bootloader
 * @ingroup SBLIB_BOOTLOADER
 * @brief   Bootloader main program
 * @details
 *
 * @{
 *
 * @file   bootloader.cpp
 * @author Martin Glueck <martin@mangari.org> Copyright (c) 2015
 * @author Stefan Haller Copyright (c) 2021
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 -----------------------------------------------------------------------------*/

#include <sblib/main.h>
#include <sblib/interrupt.h>
#include <sblib/digital_pin.h>
#include <sblib/eib/apci.h>
#include <sblib/hardware_descriptor.h>
#include "boot_descriptor_block.h"
#include "bcu_updater.h"
#include "dump.h"

#ifdef DEBUG
#   include <sblib/serial.h>
#   include <sblib/version.h>
#   include <sblib/bits.h>
#   include "flash.h"
#endif

// bootloader specific settings
#define RUN_MODE_BLINK_CONNECTED (250) //!< while connected, programming and run led blinking time in milliseconds
#define RUN_MODE_BLINK_IDLE (1000)     //!< while idle/disconnected, programming and run led blinking time in milliseconds
#define BL_RESERVED_RAM_START (0x10000000) //!< RAM start address for bootloader
#define BL_DEFAULT_VECTOR_TABLE_SIZE (192 / sizeof(uint32_t)) //!< vectortable size to copy prior application start

#define APP_START_DELAY_MS (250)          //!< Time in milliseconds the programming led will light before the app is started

// KNX/EIB specific settings
#define DEFAULT_BL_KNX_ADDRESS (((15 << 12) | (15 << 8) | 192)) //!< 15.15.192 default updater KNX-address

BcuUpdate bcu = BcuUpdate(); //!< @ref BcuUpdate instance used for bus communication of the bootloader

Timeout runModeTimeout; //!< running mode LED blinking timeout
bool blinky = false;

void startup();

uint32_t getProgrammingButton()
{
#ifdef GNAX_IO16FM_PROGRAMMING_BUTTON
    return (PIO2_11);
#elif defined ALTERNATIVE_PROGRAMMING_BUTTON
    return (PIO2_8);
#else
    return (hwPinProgButton());
#endif
}

/**
 * @brief Starts BCU with @ref DEFAULT_BL_KNX_ADDRESS (15.15.192)
 *        Sets @ref PIN_RUN as output and in debug build also @ref PIN_INFO as output
 */
BcuBase* setup()
{
// setup LED's for debug
#if defined(DEBUG) && (!(defined(TS_ARM)))
    pinMode(PIN_INFO, OUTPUT);
    digitalWrite(PIN_INFO, false);  // Turn off info LED
    pinMode(PIN_RUN, OUTPUT);
#endif

// if sblib didn't configure the serial port -> set it up for debugging
#if !defined(INCLUDE_SERIAL) && defined(DEBUG)
#   ifdef TS_ARM
        serial.setRxPin(PIO3_1);
        serial.setTxPin(PIO3_0);
#   else
        serial.setRxPin(PIO2_7);
        serial.setTxPin(PIO2_8);
#   endif
    if (!serial.enabled())
    {
        serial.begin(115200);
    }
#endif

    startup();
    bcu.setOwnAddress(DEFAULT_BL_KNX_ADDRESS);
    bcu.setProgPin(getProgrammingButton());
    runModeTimeout.start(1);

    // finally start the bcu
    bcu.begin();

#ifdef DEBUG
    int physicalAddress = bcu.ownAddress();
    serial.println("=========================================================");
    serial.print("Selfbus KNX Bootloader v", BOOTLOADER_MAJOR_VERSION);
    serial.print(".", BOOTLOADER_MINOR_VERSION, DEC, 2);
    serial.println(" DEBUG MODE :-)");
    serial.print("Build                       : ");
    serial.print(__DATE__);
    serial.print(" ");
    serial.println(__TIME__);
    serial.print("Library                     : v", highByte((uint16_t)SBLIB_VERSION), HEX, 2);  // lib version is in hexadecimal
    serial.println(".", lowByte(SBLIB_VERSION), HEX, 2);
    serial.println("Features                    : 0x", BL_FEATURES, HEX, 6);
    serial.print("Flash      (start,end,size) : 0x", flashFirstAddress());
    serial.print(" 0x", flashLastAddress());
    serial.println(" 0x", flashSize(), HEX, 6);
    serial.print("Bootloader (start,end,size) : 0x", bootLoaderFirstAddress());
    serial.print(" 0x", bootLoaderLastAddress());
    serial.println(" 0x", bootLoaderSize(), HEX, 6);
    serial.println("Firmware (start)            : 0x", applicationFirstAddress());
    serial.println("Boot descriptor (start)     : 0x", bootDescriptorBlockAddress());
    serial.println("Boot descriptor page        : 0x", bootDescriptorBlockPage(), HEX, 6);
    serial.println("Boot descriptor size        : 0x", BOOT_BLOCK_DESC_SIZE, HEX, 6);
    serial.print("physical address            : ");
    serial.print(knx_area(physicalAddress));
    serial.print(".", knx_line(physicalAddress));
    serial.print(".", knx_device(physicalAddress));
    serial.println();
    serial.println("=================================================== by sh");
#endif

    ///\todo remove on release
    /*
    testUpdProgram(0xA000, 8192);
    testUpdProgram(0xA000, 4097);
    testUpdProgram(0xA000, 4096);
    testUpdProgram(0xA000, 4095);
    testUpdProgram(0xA000, 1025);
    testUpdProgram(0xA000, 1024);
    testUpdProgram(0xA000, 1023);
    testUpdProgram(0xA000, 513);
    testUpdProgram(0xA000, 512);
    testUpdProgram(0xA000, 511);
    testUpdProgram(0xA000, 257);
    testUpdProgram(0xA000, 256);
    testUpdProgram(0xA000, 255);
    testUpdProgram(0xA000, 1);
    testUpdProgram(0xA000, 0);
    */
    return &bcu;
}

/**
 * @brief Handles LED status
 *
 */
void loop()
{
    if (runModeTimeout.expired())
    {
        if (bcu.directConnection())
        {
            runModeTimeout.start(RUN_MODE_BLINK_CONNECTED);
        }
        else
        {
#if defined(DEBUG) && (!(defined(TS_ARM)))
            digitalWrite(PIN_INFO, false);  // Turn Off info LED
#endif
            runModeTimeout.start(RUN_MODE_BLINK_IDLE);
        }
        blinky = !blinky;
    }

    digitalWrite(getProgrammingButton(), blinky);

#if defined(DEBUG) && (!(defined(TS_ARM)))
    digitalWrite(PIN_RUN, blinky);
#endif
}

/**
 * The processing loop while no KNX-application is loaded
 */
void loop_noapp()
{
}

/**
 * Restores MCU and register changes made by the bootloader (e.g. sysTick).
 */
static void finalize()
{
    Timeout ledTimeout; // don't use delay(), it needs nearly 100% more flash
    pinMode(getProgrammingButton(), OUTPUT);
    digitalWrite(getProgrammingButton(), false);
    ledTimeout.start(APP_START_DELAY_MS);
    while (!ledTimeout.expired())
    {
        waitForInterrupt();
    }
    SysTick->CTRL = 0; // disable sysTick, otherwise other apps may fail to start (e.g. bootloaderupdater)
}

/**
 * @brief Starts the application by copying application's stack top and vectortable to ram, remaps it and calls Reset vector of it
 *
 * @param start     Start address of application
 */
static void jumpToApplication(uint8_t * start)
{
    finalize(); // restore changes made and turn the programming led on
#ifndef IAP_EMULATION
    unsigned int StackTop = *(unsigned int *) (start);
    unsigned int ResetVector = *(unsigned int *) (start + 4);
    unsigned int * rom = (unsigned int *) start;
    unsigned int * ram = (unsigned int *) BL_RESERVED_RAM_START;
    unsigned int i;
    // copy the first 192 bytes (vector table) of the "application"
    // into the RAM and than remap the vector table inside the RAM

    d3(serial.println("Vectortable Size: ", (unsigned int) (BL_DEFAULT_VECTOR_TABLE_SIZE * sizeof(uint32_t)), HEX, 4););

    for (i = 0; i < BL_DEFAULT_VECTOR_TABLE_SIZE; i++, rom++, ram++)
    {
        *ram = *rom;
    }
    LPC_SYSCON->SYSMEMREMAP = 0x01;
    // DO NOT use a __DSB here, even if the user manual UM10398 28.4.2.4 states it, otherwise application won't start.

    /* Normally during RESET the stack pointer will be loaded
     * with the value stored location 0x0. Since the vector
     * table of the application is not located at 0x0 we have to do this
     * manually to ensure a correct stack.
     */
    asm volatile ("mov SP, %0" : : "r" (StackTop));
    /* Once the stack is setup we jump to the application reset vector */
    asm volatile ("bx      %0" : : "r" (ResetVector));
#endif
}

/**
 * @brief  Checks if "magic word" @ref BOOTLOADER_MAGIC_ADDRESS for bootloader mode is present and starts in bootloader mode.
 *         If no "magic word" is present it checks for a valid application to start,
 *         otherwise starts in bootloader mode
 */
void startup()
{
    // Updater request from application by setting magicWord
    unsigned int * magicWord = BOOTLOADER_MAGIC_ADDRESS;
    if (*magicWord == BOOTLOADER_MAGIC_WORD)
    {
        *magicWord = 0; // avoid restarting BL after flashing
        return;
    }
    *magicWord = 0; // wrong magicWord, delete it

    // Enter Updater when programming button was pressed at power up
    pinMode(getProgrammingButton(), INPUT | PULL_UP);
    if (!digitalRead(getProgrammingButton()))
    {
        return;
    }

    // Start main application at address
    AppDescriptionBlock * block = (AppDescriptionBlock *) bootDescriptorBlockAddress();
    if (checkApplication(block))
    {
        jumpToApplication(block->startAddress);
    }
    // Start updater in case of error
}

/** @}*/
