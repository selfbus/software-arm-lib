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
#include <sblib/internal/iap.h> // for IAP_SUCCESS
#include <sblib/eib/apci.h>
#include <sblib/hardware_descriptor.h>
#include "boot_descriptor_block.h"
#include "bcu_updater.h"
#include "dump.h"

#ifdef DEBUG
#   include <sblib/serial.h>
#   include "flash.h"
#endif

// bootloader specific settings
#define RUN_MODE_BLINK_CONNECTED (250) //!< while connected, programming and run led blinking time in milliseconds
#define RUN_MODE_BLINK_IDLE (1000)     //!< while idle/disconnected, programming and run led blinking time in milliseconds
#define BL_RESERVED_RAM_START (0x10000000) //!< RAM start address for bootloader
#define BL_DEFAULT_VECTOR_TABLE_SIZE (192 / sizeof(unsigned int)) //!< vectortable size to copy prior application start

// KNX/EIB specific settings
#define DEFAULT_BL_KNX_ADDRESS (((15 << 12) | (15 << 8) | 192)) //!< 15.15.192 default updater KNX-address
#define MANUFACTURER 0x04 //!< Manufacturer ID -> Jung
#define DEVICETYPE 0x2060 //!< Device Type -> 2138.10
#define APPVERSION 0x01   //!< Application Version -> 0.1

BcuUpdate bcu = BcuUpdate(); //!< @ref BcuUpdate instance used for bus communication of the bootloader

Timeout runModeTimeout;              //!< running mode LED blinking timeout
bool blinky = false;

uint32_t getProgrammingButton()
{
#ifdef ALTERNATIVE_PROGRAMMING_BUTTON
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

// setup serial port for debug
#ifdef DEBUG
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

#ifdef DEBUG
    int physicalAddress = bcu.ownAddress();
    serial.println("=========================================================");
    serial.print("Selfbus KNX Bootloader V", BL_IDENTITY, HEX, 4);
    serial.println(", DEBUG MODE :-)");
    serial.print("Build: ");
    serial.print(__DATE__);
    serial.print(" ");
    serial.println(__TIME__);
    serial.println("Features                    : 0x", BL_FEATURES, HEX, 6);
    serial.print("Flash      (start,end,size) : 0x", flashFirstAddress(), HEX, 6);
    serial.print(" 0x", flashLastAddress(), HEX, 6);
    serial.println(" 0x", flashSize(), HEX, 6);
    serial.print("Bootloader (start,end,size) : 0x", bootLoaderFirstAddress(), HEX, 6);
    serial.print(" 0x", bootLoaderLastAddress(), HEX, 6);
    serial.println(" 0x", bootLoaderSize(), HEX, 6);
    serial.println("Firmware (start)            : 0x", applicationFirstAddress(), HEX, 6);
    serial.println("Boot descriptor (start)     : 0x", bootDescriptorBlockAddress(), HEX, 6);
    serial.println("Boot descriptor page        : 0x", bootDescriptorBlockPage(), HEX, 6);
    serial.println("Boot descriptor size        : 0x", BOOT_BLOCK_DESC_SIZE * BOOT_BLOCK_COUNT, HEX, 6);
    serial.println("Boot descriptor count       : ", BOOT_BLOCK_COUNT, DEC);
    serial.print("physical address            : ");
    dumpKNXAddress(physicalAddress);
    serial.println();
    serial.println("=================================================== by sh");
#endif

    // finally start the bcu
    bcu.begin();
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
static void jumpToApplication(unsigned int start)
{
#ifndef IAP_EMULATION
    unsigned int StackTop = *(unsigned int *) (start);
    unsigned int ResetVector = *(unsigned int *) (start + 4);
    unsigned int * rom = (unsigned int *) start;
    unsigned int * ram = (unsigned int *) BL_RESERVED_RAM_START;
    unsigned int i;
    // copy the first 192 bytes (vector table) of the "application"
    // into the RAM and than remap the vector table inside the RAM

    d3(serial.println("Vectortable Size: ", (unsigned int) (BL_DEFAULT_VECTOR_TABLE_SIZE * sizeof(start)), HEX, 4););

    for (i = 0; i < BL_DEFAULT_VECTOR_TABLE_SIZE; i++, rom++, ram++)
    {
        *ram = *rom;
    }
    LPC_SYSCON->SYSMEMREMAP = 0x01;

    /* Normally during RESET the stack pointer will be loaded
     * with the value stored location 0x0. Since the vector
     * table of the application is not located at 0x0 we have to do this
     * manually to ensure a correct stack.
     */
    asm volatile ("mov SP, %0" : : "r" (StackTop));

    __DSB(); // DSB after a memory map switching according to UM10398 28.4.2.4
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
        //((BcuUpdate &) bcu).setProgrammingMode(programmingMode);
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
    for (int i = 0; i < BOOT_BLOCK_COUNT; i++, block--)
    {
        if (checkApplication(block))
        {
            jumpToApplication(block->startAddress);
        }
    }
    // Start updater in case of error
    run_updater(false);
    return (0);
}

/** @}*/
