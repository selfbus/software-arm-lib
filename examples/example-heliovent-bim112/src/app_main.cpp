/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_BIM112_HELIOVENT_1 BCU BIM112 Maskversion 0x0705 Example
 * @ingroup SBLIB_EXAMPLES
 * @brief   A simple application which shows the usage of the Selfbus library sblib
 *          as a bus coupling unit (BCU).
 * @details The mask version of this example is 0x0705.
 *
 *          links against BIM112_75 version of the sblib library
 * @{
 *
 * @file   app_main.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib.h>
#include <sblib/eib/user_memory.h>
#include <sblib/eib/sblib_default_objects.h>
#include <sblib/io_pin_names.h>
#include <string.h>
#include "config.h"
#include "app_heliovent.h"


#if defined(HELIOS)
#   include "heliosvent.h"
#elif defined(VALLOX)
#   include "vallox.h"
#else
#   include "mdtled.h"
#endif


#if defined(HELIOS)
    extern "C" const char APP_VERSION[13] = "BS4.70  0.9";
#elif defined(VALLOX)
    extern "C" const char APP_VERSION[13] = "VALLOX  0.1";
#else
    extern "C" const char APP_VERSION[13] = "AKD-0424R.02";
#endif


MemMapper memMapper(0xe900, 0x500);

const char * getAppVersion()
{
    return APP_VERSION;
}

/**
 * @brief This function is called by the Selfbus's library main
 *        when the processor is started or reset.
 *
 * @note  You must implement this function in your code.
 */
void setup(void)
{
    // currentVersion = &hardwareVersion[HARDWARE_ID];
    volatile char v = getAppVersion()[0];
    v++;
#if defined(DUMP_PROPERTIES) || defined(DUMP_MEM_OPS)
    serial.setRxPin(PIO2_7);
    serial.setTxPin(PIO2_8);
#endif

#if defined(HELIOS)
    bcu.begin(MANUFACTURER, DEVICETYPE, APPVERSION, 0x43FE);
#elif defined(VALLOX)
    bcu.begin(MANUFACTURER, DEVICETYPE, APPVERSION);
#else
    bcu.begin(MANUFACTURER, DEVICETYPE, APPVERSION);
#endif


    memcpy(userEeprom.order, &hardwareVersion, sizeof(hardwareVersion));

#if !defined(HELIOS) && !defined(VALLOX)
    _bcu.setMemMapper(&memMapper);
    // if (memMapper.addRange(0x4A00, 0x400) != MEM_MAPPER_SUCCESS)
    // if (memMapper.addRange(0x4B00, 0x100) != MEM_MAPPER_SUCCESS) // just for testing
    if (memMapper.addRange(0x4B00, 0x300) != MEM_MAPPER_SUCCESS)
    {
        fatalError();
    }
#endif

    // test UserRAM
    /*
    const int testSize = 12;
    byte payLoad[testSize] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B};
    for (int i = 0; i < testSize; i++)
    {
        {
            _bcu.processApciMemoryWritePDU(getUserRamEnd() - i, &payLoad[0], testSize);
        }
        serial.println("---------------------");
    }
    */
/*
    // test USER_EEPROM
    const int testSize2 = 12;
    byte payLoad2[testSize2];
    byte readLoad2[testSize2];
    for (int f = 0; f < testSize2; f++)
    {
        payLoad2[f] = f;
        readLoad2[f] = 0;
    }

    for (int k = 0; k < 0x120; k = k +testSize2)
    {
        for (int i = 0; i < testSize2; i++)
        {
            int eepromEnd = USER_EEPROM_START + USER_EEPROM_SIZE - 1;
            _bcu.processApciMemoryWritePDU(eepromEnd - testSize2-1 + i + k, &payLoad2[0], testSize2);
            _bcu.processApciMemoryReadPDU(eepromEnd - testSize2-1 + i + k, &readLoad2[0], testSize2);

            for (int testing = 0; testing < testSize2; testing++)
            {
                if (payLoad2[testing] != readLoad2[testing])
                {
                    serial.println("---  ERROR  ------");
                    // fatalError();
                }
            }

            //serial.println("---------------------");
            serial.flush();
        }
        // serial.println("---------------------");
        // serial.flush();
    }
    serial.println("---  DONE  ----------");


    fatalError();
    */
    initApplication();
}

/**
 * @brief The main processing loop while a KNX-application is loaded.
 *
 */
void loop(void)
{
    int objno;
    // Handle updated communication objects
    while ((objno = nextUpdatedObject()) >= 0)
    {
        objectUpdated(objno);
    }

    if ((millis() % 1000) == 0)
    {
        //FIXME why is this running into an fatalError()?
        serial.println("userEeprom.optionReg: 0x", userEeprom.optionReg, HEX, 2);
    }
    checkPeriodic();

    // Sleep up to 1 millisecond if there is nothing to do
    if (bus.idle())
        waitForInterrupt();
}

/**
 * @brief The processing loop while no KNX-application is loaded.
 */
void loop_noapp(void)
{
    // Sleep up to 1 millisecond if there is nothing to do
    if (bus.idle())
        waitForInterrupt();
}

/** @}*/
