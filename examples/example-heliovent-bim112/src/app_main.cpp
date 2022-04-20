/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_BIM112_HELIOVENT_1 BCU BIM112 Maskversion 0x0705 Example
 * @ingroup SBLIB_EXAMPLES
 * @brief   A simple application which shows the usage of the Selfbus library sblib
 *          as a bus coupling unit (BCU).
 *
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

#include <sblib/io_pin_names.h>
#include <sblib/serial.h>
#include "config.h"
#include "app_heliovent.h"


#if defined(HELIOS_0705)
#   include "heliosvent.h"
    APP_VERSION("BS4.70  ", "0", "10");
#elif defined(VALLOX_07B0)
#   include "vallox.h"
    APP_VERSION("VALLOX  ", "0", "11");
#elif defined(MDT_TSS_0705)
#   include "mdttss.h"
    APP_VERSION("BE06001 ", "0", "12");
#elif defined(MDT_LED_0705)
#   include "mdtled.h"
    APP_VERSION("AKD0424R", "0", "13");
#elif defined(MDT_GLASS_0705)
#   include "mdtpushbutton.h"
    APP_VERSION("BEGT2Tx ", "0", "14");
#else
#   error "No device macro defined"
#endif

MemMapper memMapper(0xe900, 0x500);

/**
 * @brief This function is called by the Selfbus's library main
 *        when the processor is started or reset.
 *
 * @note  You must implement this function in your code.
 */
BcuBase* setup()
{
    if (!serial.enabled()) // in case sblib didn't open the port, we open it
    {
        serial.setRxPin(PIO2_7); // ID_SEL/SV3 on a 4TE-Controller
        serial.setTxPin(PIO2_8);
        serial.begin(115200);
    }
    serial.print("Example heliovent-bim112 started: ");
    serial.println((const char*)getAppVersion());


#if defined(HELIOS_0705)
    bcu.begin(MANUFACTURER, DEVICETYPE, APPVERSION, 0x43FE);
#elif defined(VALLOX_07B0)
    bcu.begin(MANUFACTURER, DEVICETYPE, APPVERSION);
#else
    bcu.begin(MANUFACTURER, DEVICETYPE, APPVERSION);
#endif

    bcu.setHardwareType(hardwareVersion, sizeof(hardwareVersion));

#if !defined(HELIOS_0705) && !defined(VALLOX_07B0)
    bcu.setMemMapper(&memMapper);
    // if (memMapper.addRange(0x4A00, 0x400) != MEM_MAPPER_SUCCESS)
    // if (memMapper.addRange(0x4B00, 0x100) != MEM_MAPPER_SUCCESS) // just for testing
    if (memMapper.addRange(0x4B00, 0x300) != MEM_MAPPER_SUCCESS)
    {
        fatalError();
    }
#endif

    // test UserRAM
    const int testSize = 12;
    byte payLoad[testSize] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B};
    for (int i = 0; i < testSize; i++)
    {
        {
            bcu.processApciMemoryWritePDU(bcu.userRam->userRamEnd - i, &payLoad[0], testSize);
        }
        serial.println("---------------------");
    }

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
        int eepromEnd = bcu.userEeprom->userEepromEnd;
        for (int i = 0; i < testSize2; i++)
        {
            bcu.processApciMemoryWritePDU(eepromEnd - testSize2-1 + i + k, &payLoad2[0], testSize2);
            bcu.processApciMemoryReadPDU(eepromEnd - testSize2-1 + i + k, &readLoad2[0], testSize2);

            for (int testing = 0; testing < testSize2; testing++)
            {
                if (payLoad2[testing] != readLoad2[testing])
                {
                    serial.println("---  ERROR  ------");
                    // fatalError();
                }
            }
            serial.flush();
        }
    }
    serial.println("---  DONE  ----------");
    serial.flush();

    // fatalError();

    initApplication();
    return (&bcu);
}

/**
 * @brief The main processing loop while a KNX-application is loaded.
 *
 */
void loop(void)
{
    int objno;
    // Handle updated communication objects
    while ((objno = bcu.comObjects->nextUpdatedObject()) >= 0)
    {
        objectUpdated(objno);
    }

    if ((millis() % 1000) == 0)
    {
        //FIXME why this can run into an fatalError()?
        serial.println("bcu.userEeprom->optionReg(): 0x", bcu.userEeprom->optionReg(), HEX, 2);
    }
    checkPeriodic();

    // Sleep up to 1 millisecond if there is nothing to do
    if (bcu.bus->idle())
        waitForInterrupt();
}

/**
 * @brief The processing loop while no KNX-application is loaded.
 */
void loop_noapp(void)
{
    // Sleep up to 1 millisecond if there is nothing to do
    if (bcu.bus->idle())
        waitForInterrupt();
}

/** @}*/
