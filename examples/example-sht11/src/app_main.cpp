/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_SHT1x temperature/humidity sensor SHT1x example
 * @ingroup SBLIB_EXAMPLES
 * @brief   A simple application which shows the usage of the SHT1x class.
 * @details The mask version of this example is 0x0701.
 *
 * @{
 *
 * @file   app_main.cpp
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eibMASK0701.h>
#include <sblib/i2c/SHT1x.h>
#include <sblib/io_pin_names.h>
#include <sblib/serial.h>
#include <sblib/timeout.h>

MASK0701 bcu;
SHT1x sensor(PIO0_9, PIO2_2); // don't use i2c (SDA, SCL) pins, they have worse edge steepness.

Timeout readTimeout;
#define READ_TIMER_MS (500)

#define GET_SHT1x_MEASUREMENT
// #define GET_SHT1x_STATUS
// #define SET_SHT1x_STATUS
// #define SOFTRESET_SHT1x

void sendToSerial(int32_t n)
{
    serial.print((int)n / 100, DEC);
    serial.print(".");
    serial.print((int)abs(n % 100), DEC, 2);
}

/**
 * This function is called by the Selfbus's library main
 * when the processor is started or reset.
 *
 * @note  You must implement this function in your code.
 */
BcuBase* setup()
{
    serial.setTxPin(PIO3_0);
    serial.setRxPin(PIO3_1);
    serial.begin(115200);
    serial.println("example-sht11 started.");
    bcu.begin(0,0,0);
    readTimeout.start(READ_TIMER_MS);
    return &bcu;
}

/**
 * The main processing loop while a KNX-application is loaded.
 */
void loop(void)
{
    if (!readTimeout.expired())
    {
        return;
    }
#ifdef GET_SHT1x_MEASUREMENT
    float dewPoint = sensor.GetDewPoint(); // this also triggers temperature and humidity measurement

    int16_t temperature = sensor.getLastTemperature();
    if (temperature != INVALID_TEMPERATURE)
    {
        sendToSerial(temperature);
        serial.print("C ");
    }
    else
    {
        serial.println("Reading temperature failed.");
    }

    uint16_t humidity = sensor.getLastHumidity();
    if (humidity != INVALID_HUMIDITY)
    {
        sendToSerial(humidity);
        serial.print("%rH ");
    }
    else
    {
        serial.println("Reading relative humidity failed.");
    }

    if (dewPoint > INVALID_DEW_POINT)
    {
        sendToSerial(dewPoint*100);
        serial.print("TD ");
    }
    else
    {
        serial.println("Reading dew point failed.");
    }
#endif

#ifdef GET_SHT1x_STATUS
    uint16_t statusReceived;

    if (sensor.getStatusRegister(&statusReceived))
    {
        serial.print("current status 0x", statusReceived, HEX, 4);
    }
    else
    {
        serial.println("Reading status register failed.");
    }
#endif

#ifdef SET_SHT1x_STATUS
    uint16_t newStatus;
    //newStatus = 0b00000000; // heater off
    newStatus = 0b00000100; // heater on
    // newStatus = 0b00000111; // heater on, reload from OTP, 8bit RH / 12bit Temp
    if (sensor.setStatusRegister(newStatus))
    {
        serial.print("status set to 0x", newStatus, HEX, 4);
    }
    else
    {
        serial.println("Writing status register failed.");
    }
#endif

#ifdef SOFTRESET_SHT1x
    if (sensor.softReset())
    {
        serial.print("sensor reset successful");
    }
    else
    {
        serial.println("Softreset failed.");
    }
#endif

    serial.println();
    readTimeout.start(READ_TIMER_MS);
}

/**
 * The processing loop while no KNX-application is loaded.
 */
void loop_noapp(void)
{
    loop();
}

/** @}*/
