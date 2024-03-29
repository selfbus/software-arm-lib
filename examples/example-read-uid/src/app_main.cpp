/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_PROC_UID_1 Processor UID (GUID) and serial example
 * @ingroup SBLIB_EXAMPLES
 * @brief   Sends UID, shorted UID (for bus-updater use) and KNX-serial number to the serial port
 * @details A simple application which:
 *              - reads the UID (GUID) of the LPCxx processor
 *              - calculates a hashed serial used for the KNX bus
 *              - blinks the program led at ~2Hz
 *              - sends UID, shorted UID (for bus-updater use) and KNX-serial number to the serial port<br/>
 *
 *          Connect a terminal program to the ARM's serial port.<br/>
 *          Default Tx-pin is PIO1.7 and Rx-pin is PIO1.6.<br/>
 *          You can change the serial port by commenting/uncommenting PIN_SERIAL_RX and PIN_SERIAL_TX.<br/>
 *          The connection settings are 115200 8N1 (115200 baud, 8 data bits, no parity, 1 stop bit).<br/>
 *
 * @{
 *
 * @file   app_main.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <sblib/eibBCU1.h>
#include <sblib/internal/iap.h>
#include <sblib/io_pin_names.h>
#include <sblib/serial.h>
#include <sblib/timer.h>

#define PAUSE_MS 1000                   //!< pause in milliseconds between each serial transmission

// use serial port on port 1
//#define PIN_SERIAL_RX PIO1_6            //!< serial Rx-Pin on PIO1.6
//#define PIN_SERIAL_TX PIO1_7            //!< serial Tx-Pin on PIO1.7

// use serial port on port 2
#define PIN_SERIAL_RX PIO2_7            //!< serial Rx-Pin on PIO2.7
#define PIN_SERIAL_TX PIO2_8            //!< serial Tx-Pin on PIO2.8

// use serial port on port 3
//#define PIN_SERIAL_RX PIO3_1            //!< serial Rx-Pin on PIO3.1
//#define PIN_SERIAL_TX PIO3_0            //!< serial Tx-Pin on PIO3.0

#define DEFAULT_SERIAL_SPEED 115200      //!< serial speed in baud

#define UID_BYTES_FOR_BUSUPDATER 12     //!< number of byte the bus-updater needs for option -uid

APP_VERSION("SBuid   ", "1", "01"); //!< Create APP_VERSION, its used in the bus updater magic string


/// @cond DEVELOPER
#define KNX_SERIAL_NUMBER_LENGTH 6      //!< length of a KNX serial number
/// @endcond

BCU1 bcu = BCU1();

void sendBytesInHexToSerialPort(Serial &serialPort, byte* buffer, unsigned int length, char separator='\0');

/**
 * @brief This function is called by the Selfbus's library main
 *        when the processor is started or reset.
 *
 * @note  You must implement this function in your code.
 */
BcuBase* setup()
{
    pinMode(PIN_PROG, OUTPUT);
    digitalWrite(PIN_PROG, true);
    serial.setRxPin(PIN_SERIAL_RX);
    serial.setTxPin(PIN_SERIAL_TX);
    serial.begin(DEFAULT_SERIAL_SPEED);
    serial.println("Selfbus read UID example");
    return (&bcu);
}


unsigned char __attribute__((section (".selfbusSection"))) selfBusBuffer[12];
unsigned char __attribute__((section (".selfbusSection"))) selfBusChar = 0xAA;

extern int __selfbus_first_sector;
extern int __selfbus_sector_end;
extern unsigned int __selfbus_image_first_sector;
extern unsigned int __selfbus_image_size;

extern unsigned int __base_Flash;
extern unsigned int __top_Flash;
extern unsigned int _image_start;
extern unsigned int _image_end;
extern unsigned int _image_size;

/*
__base_${memory.name} = ${memory.location}  ; //${memory.name}
__base_${memory.alias} = ${memory.location} ; // ${memory.alias}
__top_${memory.name} = ${memory.location} + ${memory.size} ; // ${memory.sizek}
__top_${memory.alias} = ${memory.location} + ${memory.size} ; // ${memory.sizek}/
_image_start = LOADADDR(.text);
_image_end = LOADADDR(.data) + SIZEOF(.data);
_image_size = _image_end - _image_start;


*/
/**
 * @brief The processing loop while no KNX-application is loaded.
 *
 * @note Because we don't start the bus with bcu.begin(...),
 *       in this example only this function will be called by the Selfbus library.
 */
void loop_noapp()
{


/*
    static int i = 0;
    uint8_t *p, *end;

    p = (uint8_t*)&__selfbus_first_sector;
    end = (uint8_t*)&__selfbus_sector_end;
    while(p<end) {
      *p++ = i++; //initialize with pattern
    }
    for (;;) {
    }
    // Never leave main
    return;
*/
    // unsigned int *p;
    // p = (unsigned int*)&__selfbus_image_first_sector;

    unsigned int flashStartAddress = (unsigned int) (unsigned int*)&__base_Flash;
    unsigned int flashEndAddress = (unsigned int) (unsigned int*)&__top_Flash;

    flashEndAddress--; // bug in NXP linkerscript adds 1 to endAddress

    unsigned int imageStartAddress = (unsigned int) (unsigned int*)&_image_start;
    unsigned int imageEndAddress = (unsigned int) (unsigned int*)&_image_end;
    unsigned int imageSize = (unsigned int) (unsigned int*)&_image_size;

    imageSize++; // bug in NXP linkerscript misses 1 of imageSize

    serial.print("Flash    (start,end,size) : 0x", flashStartAddress, HEX, 6);
    serial.print(" 0x", flashEndAddress, HEX, 6);
    serial.println(" 0x", flashEndAddress - flashStartAddress + 1, HEX, 6);

    serial.print("Firmware (start,end,size) : 0x", imageStartAddress, HEX, 6);
    serial.print(" 0x", imageEndAddress, HEX, 6);
    serial.println(" 0x", imageSize, HEX, 6);

    byte uniqueID[IAP_UID_LENGTH]; // buffer for the UID/GUID of the processsor
    byte knxSerial[KNX_SERIAL_NUMBER_LENGTH]; // buffer for the KNX serial number

    if (iapReadUID(&uniqueID[0]) == IAP_SUCCESS)
    {
        serial.print("Target UID is             : ");
        // send the uid/guid we received from iapReadUID
        sendBytesInHexToSerialPort(serial, &uniqueID[0], IAP_UID_LENGTH, ':');
        serial.print("Busupdater needs  (--uid) : ");
        sendBytesInHexToSerialPort(serial, &uniqueID[0], UID_BYTES_FOR_BUSUPDATER, ':');

        // create a 48bit serial/hash from the 128bit GUID
        if (hashUID(&uniqueID[0], sizeof(uniqueID), &knxSerial[0], sizeof(knxSerial)))
        {
            serial.print("KNX-Serial                : ");
            sendBytesInHexToSerialPort(serial, &knxSerial[0], sizeof(knxSerial), ':');
        }
        else
        {
            serial.println("Error hashing the UID.");
        }
        serial.println();
    }
    else
    {
        serial.println("Error reading Target UID.");
    }
    digitalWrite(PIN_PROG, !digitalRead(PIN_PROG));
    delay(PAUSE_MS);
}

/**
 * @brief The main processing loop while a KNX-application is loaded.
 *
 * @note Will never be called in this example
 */
void loop()
{
}

/**
 * @brief Sends the hexadecimal value of bytes from buffer as text to the serial port
 *
 * @param serialPort    serial port to use for transmission
 * @param buffer        buffer containing the bytes
 * @param length        length of buffer
 * @param separator     optional separator which will be placed between the bytes
 */
void sendBytesInHexToSerialPort(Serial &serialPort, byte* buffer, unsigned int length, char separator)
{
    for (unsigned int i = 0; i < length; ++i)
    {
        serialPort.print(buffer[i], HEX, 2);
        if ((i < (length - 1)) && (separator != '\0'))
        {
            serialPort.print(":");
        }
    }
    serialPort.println();
}

/** @}*/
