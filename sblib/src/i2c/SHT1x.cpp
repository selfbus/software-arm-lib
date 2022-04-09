/**************************************************************************//**
 * @addtogroup SBLIB_SENSOR Selfbus library sensors
 * @defgroup SBLIB_SENSOR_SHT1x SHT1x Temperature/humidity sensor
 * @ingroup SBLIB_SENSOR
 * @brief   Implementation for the usage of the Sensirion SHT1x temperature and humidity sensor
 *
 * @{
 *
 * @file   SHT1x.cpp
 * @bug No known bugs.
 ******************************************************************************/

/*
 * SHT1x Library
 *
 * https://github.com/practicalarduino/SHT1x (04/01/2022) commit sha: be7042c3e3cb32c778b9c9ca270b3df57f769ea5
 * Copyright 2009 Jonathan Oxer <jon@oxer.com.au> / <www.practicalarduino.com>
 * Based on previous work by:
 *    Maurice Ribble: <www.glacialwanderer.com/hobbyrobotics/?p=5>
 *    Wayne ?: <ragingreality.blogspot.com/2008/01/ardunio-and-sht15.html>
 *
 * Manages communication with SHT1x series (SHT10, SHT11, SHT15)
 * temperature / humidity sensors from Sensirion (www.sensirion.com).
 */

#include <cmath>
#include <sblib/i2c/SHT1x.h>
#include <sblib/digital_pin.h>
#include <sblib/timer.h>
//#include <sblib/serial.h>

SHT1x::SHT1x(int dataP, int clockP) :
    dataPin(dataP),
    clockPin(clockP),
    lastTemperatureC(INVALID_TEMPERATURE),
    lastHumidity(INVALID_HUMIDITY)
{
    pinMode(clockPin, OUTPUT);
    digitalWrite(clockPin, false);
    releaseDataPin();
}

void SHT1x::activateDataPin()
{
    pinMode(dataPin, OUTPUT | OPEN_DRAIN);
    digitalWrite(dataPin, true);
}

void SHT1x::releaseDataPin()
{
    pinMode(dataPin, INPUT);
}

void SHT1x::clockCycle()
{
    digitalWrite(clockPin, true);
    __NOP();
    digitalWrite(clockPin, false);
}

float SHT1x::convertRawTemperature(const float& temperature, eScale type)
{
    // Conversion coefficients from SHT1x datasheet
    const float D1 = -39.7f;  // @ 3.5V VDD (-39.6 @ 3.0V)
    float D2 = 0.01f; // default
    switch (type)
    {
        case CELCIUS:
            D2 = 0.01f; // for 14 Bit DEGC
            break;

        case FARENHEIT:
            D2 = 0.018f; // for 14 Bit DEGF
            break;

        default:
            D2 = 0.0f;
    }
    return ((temperature * D2) + D1);
}

bool SHT1x::readTemperatureC(float* newTemperature)
{
    int16_t val; // raw value returned from sensor
    if (!readTemperatureRaw(&val))
    {
        return (false);
    }
    *newTemperature = convertRawTemperature(val, CELCIUS);
    lastTemperatureC = (int16_t)trunc(*newTemperature * 100);
    return (true);
}

bool SHT1x::readTemperatureF(float* newTemperature)
{
    int16_t val; // raw value returned from sensor
    if (!readTemperatureRaw(&val))
    {
        return (false);
    }
    *newTemperature = convertRawTemperature(val, CELCIUS); // stupid hack to store the last temp in C
    lastTemperatureC = (int16_t)trunc(*newTemperature * 100);

    *newTemperature = convertRawTemperature(val, FARENHEIT);
    return (true);
}

bool SHT1x::readHumidity(float* humidity)
{
    lastHumidity = INVALID_HUMIDITY;
    int _val;                   // Raw humidity value returned from sensor
    float linearHumidity;       // Humidity with linear correction applied
    float temperatureC;

    // Conversion coefficients from SHT15 datasheet (I don't see this Cx values in the datasheet
    // const float C1 = -4.0f;       // for 12 Bit
    // const float C2 =  0.0405f;    // for 12 Bit
    // const float C3 = -0.0000028f; // for 12 Bit

    const float T1 =  0.01f;      // for 14 Bit @ 5V
    const float T2 =  0.00008f;   // for 14 Bit @ 5V

    const float C1 = -2.0468f;       // for 12 Bit
    const float C2 =  0.0367f;    // for 12 Bit
    const float C3 = -1.5955E-6f; // for 12 Bit

    // Get current temperature for humidity correction
    if (!readTemperatureC(&temperatureC))
    {
        return (false);
    }

    // Fetch the value from the sensor
    if (!sendByteToSHT(sht1xMeasureRelativeHumidity, true))
    {
        return (false);
    }

    if (!waitForResultSHT())
    {
        return (false);
    }
    _val = getData16SHT();
    skipCrcSHT();

    // Apply linear conversion to raw value
    linearHumidity = C1 + C2 * _val + C3 * _val * _val;

    // Correct humidity value for current temperature
    *humidity = (temperatureC - 25.0f ) * (T1 + T2 * _val) + linearHumidity;
    lastHumidity = (uint16_t)trunc(*humidity * 100);
    return (true);
}

bool SHT1x::readTemperatureRaw(int16_t* temperatureRaw)
{
    lastTemperatureC = INVALID_TEMPERATURE;
    if (!sendByteToSHT(sht1xMeasureTemperature, true))
    {
        return (false);
    }

    if (!waitForResultSHT())
    {
        return (false);
    }
    *temperatureRaw = getData16SHT();
    skipCrcSHT();

    return (true);
}

void SHT1x::sendTransmissionStart()
{
    digitalWrite(dataPin, true);
    digitalWrite(clockPin, true);
    __NOP();
    digitalWrite(dataPin, false);
    __NOP();
    digitalWrite(clockPin, false);
    __NOP();
    digitalWrite(clockPin, true);
    __NOP();
    digitalWrite(dataPin, true);
    __NOP();
    digitalWrite(clockPin, false);
    __NOP();
}

bool SHT1x::sendByteToSHT(uint8_t toSend, bool isCommand)
{
    activateDataPin();
    if (isCommand)
    {
        // Transmission Start
        sendTransmissionStart();
    }

    shiftOut(dataPin, clockPin, MSBFIRST, toSend);
    releaseDataPin();

    // Verify we get a ack (9th clock cycle)
    digitalWrite(clockPin, true);
    __NOP();
    if (digitalRead(dataPin) != false)
    {
        //serial.println("Ack Error 1, DATA not low");
        return (false);
    }
    digitalWrite(clockPin, false); // (end of 9th clock cycle)
    __NOP();
    uint8_t i = 0;
    do {
        if (digitalRead(dataPin) == true)
        {
            return (true);
        }
        delayMicroseconds(MAX_DELAY_MICROSECONDS); // need to wait, seems like DATA-pin needs ~8-100ms to raise to high
        i++;
    } while ((i < 25));
    //serial.println("Ack Error 2, DATA not high");
    return (false);
}

bool SHT1x::waitForResultSHT()
{
    releaseDataPin();
    unsigned int i = 0;
    do
    {
        if (digitalRead(dataPin) == false) // DATA line low -> measurement complete
        {
            return (true);
        }
        else
        {
            i++;
            delay(WAIT_STEP_MS);
        }
    } while ((i * WAIT_STEP_MS) <= MAX_WAIT_MS);
    //serial.println("Error, wait for DATA low failed");
    return (false);
}

uint16_t SHT1x::getData16SHT()
{
    uint16_t val;

    // Get the most significant bits
    releaseDataPin();
    val = shiftIn(dataPin, clockPin, MSBFIRST) << 8;

    // Send the required ack
    activateDataPin();
    digitalWrite(dataPin, false);
    clockCycle();
    releaseDataPin();

    // Get the least significant bits
    val |= shiftIn(dataPin, clockPin, MSBFIRST);
    return val;
}

void SHT1x::skipCrcSHT()
{
    // Skip acknowledge to end trans (no CRC)
    activateDataPin();
    clockCycle();
    releaseDataPin();
}

void SHT1x::Init()
{
    // nothing to do here, just for compatibility with SHT2x
}

uint16_t SHT1x::GetHumidity()
{
    float humidity;
    if (readHumidity(&humidity))
    {
        return ((uint16_t)(humidity * 100));
    }
    else
    {
        return (INVALID_HUMIDITY); // error
    }
}

int16_t SHT1x::GetTemperature()
{
    float temperature;
    if (readTemperatureC(&temperature))
    {
        return ((uint16_t)(temperature * 100));
    }
    else
    {
        return (INVALID_TEMPERATURE); // error
    }
}

float SHT1x::GetDewPoint()
{
    float humidity;
    if (!readHumidity(&humidity) || (getLastTemperature() == INVALID_TEMPERATURE)) // this also triggers temperature measurement
    {
        return (INVALID_DEW_POINT);
    }

    float temperature = getLastTemperature()/100;

// Specify the constants for water vapor and barometric pressure.
#define WATER_VAPOR 17.62f
#define BAROMETRIC_PRESSURE 243.5f

    // Calculate the intermediate value 'gamma'
    float gamma = log(humidity / 100) + (WATER_VAPOR * temperature) / (BAROMETRIC_PRESSURE + temperature);
    // Calculate dew point in Celsius
    float dewPoint = BAROMETRIC_PRESSURE * gamma / (WATER_VAPOR - gamma);

    return dewPoint;
}

bool SHT1x::getStatusRegister(uint16_t* status)
{
    activateDataPin();
    for (uint8_t i = 0; i < 10; i++)
    {
        clockCycle();
    }

    if (!sendByteToSHT(sht1xReadStatusRegister, true))
    {
        return (false);
    }

    if (!waitForResultSHT())
    {
        return (false);
    }
    *status = getData16SHT();
    skipCrcSHT();

    return (true);
}

bool SHT1x::setStatusRegister(const uint16_t& status)
{
    if (!sendByteToSHT(sht1xWriteStatusRegister, true))
    {
        return (false);
    }

    if (!sendByteToSHT(status, false))
    {
        return (false);
    }
    return (true);
}

bool SHT1x::softReset()
{
    if (!sendByteToSHT(sht1xSoftReset, true))
    {
        return (false);
    }
    return (true);
}


