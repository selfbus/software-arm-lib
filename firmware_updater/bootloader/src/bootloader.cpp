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

// KNX/EIB specific settings
#define DEFAULT_BL_KNX_ADDRESS (((15 << 12) | (15 << 8) | 192)) //!< 15.15.192 default updater KNX-address
#define MANUFACTURER 0x04 //!< Manufacturer ID -> Jung
#define DEVICETYPE 0x2060 //!< Device Type -> 2138.10
#define APPVERSION 0x01   //!< Application Version -> 0.1

BcuUpdate bcu = BcuUpdate(); //!< @ref BcuUpdate instance used for bus communication of the bootloader

Timeout runModeTimeout; //!< running mode LED blinking timeout
bool blinky = false;

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
 * @brief Configures the system timer to call SysTick_Handler once every 1 msec.
 *
 */
static void lib_setup()
{
    SysTick_Config(SystemCoreClock / 1000);
    systemTime = 0;
}

/**
 * @brief Starts BCU with @ref DEFAULT_BL_KNX_ADDRESS (15.15.192) as a Jung 2138.10 device, version 0.1<br>
 *        Sets @ref PIN_RUN as output and in debug build also @ref PIN_INFO as output
 *
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

    return &bcu;
}

/**
 * @brief Handles bus.idle(), LED status and MCU's reset (if requested by KNX-telegram), called from run_updater(...)
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
 * @brief Starts the application by coping application's stack top and vectortable to ram, remaps it and calls Reset vector of it
 *
 * @param start     Start address of application
 */
static void jumpToApplication(uint8_t * start)
{
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
 * @brief The real "main()" of the bootloader. Calls libsetup() to initialize the Selfbus library
 *        and setup() to initialize itself. Handles loop() of the BCU and itself.
 *
 * @param programmingMode if true bootloader enables BCU programming mode active, otherwise not.
 */
static void run_updater(bool programmingMode)
{
    lib_setup();
    setup();

    if (programmingMode)
    {
        bcu.setProgrammingMode(programmingMode);
    }

    while (1)
    {
        bcu.loop();
        loop();
    }
}

/**
 * @brief Checks if "magic word" @ref BOOTLOADER_MAGIC_ADDRESS for bootloader mode is present and starts in bootloader mode.<br/>
 *        If no "magic word" is present it checks for a valid application to start,<br>
 *        otherwise starts in bootloader mode
 *
 * @return never returns
 */
#ifndef IAP_EMULATION
    int main()
#else
    int alt_main()
#endif
{
    // Updater request from application by setting magicWord
  	unsigned int * magicWord = BOOTLOADER_MAGIC_ADDRESS;
 	if (*magicWord == BOOTLOADER_MAGIC_WORD)
    {
        *magicWord = 0;	// avoid restarting BL after flashing
        run_updater(true);
    }
    *magicWord = 0;		// wrong magicWord, delete it

    // Enter Updater when programming button was pressed at power up
    pinMode(getProgrammingButton(), INPUT | PULL_UP);
    if (!digitalRead(getProgrammingButton()))
    {
        run_updater(true);
    }

    // Start main application at address
    AppDescriptionBlock * block = (AppDescriptionBlock *) bootDescriptorBlockAddress();
    if (checkApplication(block))
    {
        jumpToApplication(block->startAddress);
    }
    // Start updater in case of error
    run_updater(false);
    return (0);
}

/** @}*/
