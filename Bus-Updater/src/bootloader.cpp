/*
 *  BootLoader.c - The bootloader.
 *
 *  Copyright (c) 2015 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/main.h>

#include <sblib/eib.h>
#include <sblib/timeout.h>
#include <sblib/internal/variables.h>
#include <sblib/io_pin_names.h>
#include "BCUUpdate.h"

static BCU_Update _bcu = BCU_Update();
BCU_Base * bcu = &_bcu;

// The EIB bus access object
#if defined (__LPC11XX__)
Bus bus(timer16_1, PIO1_8, PIO1_9, CAP0, MAT0);
#elif defined (__LPC11UXX__)
Bus bus(timer16_1, PIO0_20, PIO0_21, CAP0, MAT0);
#endif

#include <cr_section_macros.h>
#include <boot_descriptor_block.h>
#include "sblib/digital_pin.h"
#include "sblib/io_pin_names.h"

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
	bcu->begin(4, 0x2060, 1); // We are a "Jung 2138.10" device, version 0.1
	pinMode(PIN_INFO, OUTPUT);
	pinMode(PIN_RUN,  OUTPUT);
	blinky.start(1);
	bcu->setOwnAddress(0x1103);
	extern byte userEepromModified;
	userEepromModified = 0;
}

void loop()
{
    if (blinky.expired())
    {
        if (bcu->directConnection())
            blinky.start(250);
        else
            blinky.start(1000);
        digitalWrite(PIN_RUN, !digitalRead(PIN_RUN));
    }
    digitalWrite(PIN_PROG, digitalRead(PIN_RUN));
    // Sleep up to 1 millisecond if there is nothing to do
}

void jumpToApplication(unsigned int start)
{
    unsigned int ResetVector = * (unsigned int *) (start + 4);
    unsigned int * rom = (unsigned int *) start;
    unsigned int * ram = (unsigned int *) 0x10000000;
    unsigned int i;
    // copy the first 200 bytes of the "application" (the vector table)
    // into the RAM and than remap the vector table inside the RAM
    for (i = 0; i < 50; i++, rom++, ram++)
        * ram = * rom;
    LPC_SYSCON->SYSMEMREMAP = 0x01;

    void (*fptr)(void);
    fptr = (void (*)(void))ResetVector;
    fptr();
}

void run_updater()
{
    lib_setup();
    setup();

    while (1)
    {
        bcu->loop();
        loop();
    }
}

int main (void)
{
    pinMode(PIN_PROG, INPUT);
    if (! digitalRead(PIN_PROG))
    {
    	run_updater();
    }
	AppDescriptionBlock * block = (AppDescriptionBlock *) (FIRST_SECTOR - BOOT_BLOCK_SIZE);
	if (checkApplication(block))
		jumpToApplication(block->startAddress);
    run_updater();
    return 0 ;
}
