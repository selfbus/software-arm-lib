/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @defgroup SBLIB_BOOTLOADER_MAIN Bootloader
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
#include <sblib/serial.h>
#include <sblib/internal/iap.h> // for IAP_SUCCESS
#include "boot_descriptor_block.h"
#include "bcu_updater.h"

static BcuUpdate _bcu = BcuUpdate();
BcuBase& bcu = _bcu;

Timeout blinky;

/**
 * Setup the library.
 */
static inline void lib_setup()
{
    // Configure the system timer to call SysTick_Handler once every 1 msec
    SysTick_Config(SystemCoreClock / 1000);
    systemTime = 0;
}

void setup()
{
    bcu.begin(4, 0x2060, 1); // We are a "Jung 2138.10" device, version 0.1
    pinMode(PIN_RUN, OUTPUT);

#ifdef DEBUG
    pinMode(PIN_INFO, OUTPUT);
    digitalWrite(PIN_INFO, false);  // Turn Off info LED
#endif


    blinky.start(1);
    bcu.setOwnAddress(0xFFC0);		// 15.15.192 default updater PA
    extern byte userEepromModified;
    userEepromModified = 0;
}

void loop()
{
	//bool blink_state = false;
    if (blinky.expired())
    {
        if (bcu.directConnection())
            blinky.start(250);
        else
            blinky.start(1000);
        //blink_state = !blink_state;
        digitalWrite(PIN_RUN, !digitalRead(PIN_RUN));
    }
    digitalWrite(PIN_PROG, digitalRead(PIN_RUN));

    // Check if there is data to flash when bus is idle
    if(bus.idle())
    {
        // reverted to old method, because this leads to timeouts
        // also on success we would need to send a response
        // writing to flash may time out the PC_Updater_tool for around 5s
        // because a bus timer interrupt may be canceled due to
        // iapProgram->IAP_Call_InterruptSafe->noInterrupts();
        // if (request_flashWrite(NULL, 0) == IAP_SUCCESS)
    	// {
    	// }

        // Check if restart request is pending
    	if (restartRequestExpired())
        {
#ifdef DUMP_TELEGRAMS_LVL1
    	    serial.print("Systime: ", systemTime, DEC);
    	    serial.println(" reset");
    	    serial.flush();  // give time to send serial data
#endif
    	    NVIC_SystemReset();
        }
    }
}

static inline void jumpToApplication(unsigned int start)
{
    unsigned int StackTop = *(unsigned int *) (start);
    unsigned int ResetVector = *(unsigned int *) (start + 4);
    unsigned int * rom = (unsigned int *) start;
    unsigned int * ram = (unsigned int *) 0x10000000;
    unsigned int i;
    // copy the first 200 bytes of the "application" (the vector table)
    // into the RAM and than remap the vector table inside the RAM
    for (i = 0; i < 50; i++, rom++, ram++)
        *ram = *rom;
    LPC_SYSCON->SYSMEMREMAP = 0x01;

    /* Normally during RESET the stack pointer will be loaded
     * with the value stored location 0x0. Since the vector
     * table of the application is not located at 0x0 we have to do this
     * manually to ensure a correct stack.
     */
    asm volatile ("mov SP, %0" : : "r" (StackTop));
    /* Once the stack is setup we jump to the application reset vector */
    asm volatile ("bx      %0" : : "r" (ResetVector));
}

static inline void run_updater(bool programmingMode)
{
    lib_setup();
    setup();

    if (programmingMode)
    {
        ((BcuUpdate &) bcu).setProgrammingMode(programmingMode);
    }

#ifdef DUMP_TELEGRAMS_LVL1
    //serial.setRxPin(PIO3_1);
    //serial.setTxPin(PIO3_0);
    if (!serial.enabled())
    {
        serial.begin(115200);
    }
    serial.println("=======================================================");
    serial.print("Selfbus KNX Bootloader V", BL_IDENTITY, HEX, 4);
    serial.println(", DEBUG MODE :-)");
    serial.print("Build: ");
    serial.print(__DATE__);
    serial.print(" ");
    serial.print(__TIME__);
    serial.println(" Features: 0x", BL_FEATURES, HEX, 4);
    serial.print("Flash (start,end,size)    : 0x", flashFirstAddress(), HEX, 6);
    serial.print(" 0x", flashLastAddress(), HEX, 6);
    serial.println(" 0x", flashSize(), HEX, 6);
    serial.print("Firmware (start,end,size) : 0x", bootLoaderFirstAddress(), HEX, 6);
    serial.print(" 0x", bootLoaderLastAddress(), HEX, 6);
    serial.println(" 0x", bootLoaderSize(), HEX, 6);
    serial.println("------------------------------------------------- by sh");
#endif

    while (1)
    {
        bcu.loop();
        loop();
    }
}

int main()
{
    // Updater request from application by setting magicWord
  	unsigned int * magicWord = BOOTLOADER_MAGIC_ADDRESS;
 	if (*magicWord == BOOTLOADER_MAGIC_WORD)
    {
        *magicWord = 0;	// avoid restarting BL after flashing
        run_updater(true);
    }
    *magicWord = 0;		// wrong magicWord, delete it

    // Enter Updater when prog button pressed at power up
    pinMode(PIN_PROG, INPUT | PULL_UP);
    if (!digitalRead(PIN_PROG))
    {
        run_updater(true);
    }

    // Start main application at address
    AppDescriptionBlock * block = (AppDescriptionBlock *) APPLICATION_FIRST_SECTOR;
    block--; // one block backwards
    for (int i = 0; i < BOOT_BLOCK_COUNT; i++, block--)
    {
        if (checkApplication(block))
        {
            jumpToApplication(block->startAddress);
        }
    }
    // Start updater in case of error
    run_updater(false);
    return 0;
}

/** @}*/
