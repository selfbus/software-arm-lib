/*
 *  dht.h - Digital-output Humidity and Temperature Sensors
 *
 *  Copyright (c) 2015 Erkan Colak <erkanc@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as published
 *  by the Free Software Foundation.
 */

#include <math.h>
#include <sblib/core.h>
#include <sblib/digital_pin.h>

#include <sblib/sensors/dht.h>

/*****************************************************************************
** Function name:  DHTInit
**
** Descriptions:   Initialize the DHT
**
** parameters:     PORT and DHT Type
**
** Returned value: none
**
*****************************************************************************/
void DHT::DHTInit(int pin,int DHTtype)
{
  this->_pin = pin;
  this->_DHTtype = DHTtype;
  this->_lastReadTime = -2000;
  this->_lastError= ERROR_NONE;
  this->_lastHumidity=0;
  this->_lastTemperature=0;
  this->_maxcycles = microsecondsToClockCycles(1000);

  pinMode(this->_pin, INPUT |PULL_UP); // set up the pins!
}

/*****************************************************************************
** Function name:  readData
**
** Descriptions:   Read the Temperature and Humidity
**
** parameters:     bForceRead, to ignore the internal DHT TIMER
**
** Returned value: true on success. Check: _lastHumidity and _lastTemperature
**                 false on error. Check _lastError for reason.
**
*****************************************************************************/
bool DHT::readData(bool bForceRead)
{
  bool bRet= false;
  this->_lastError= ERROR_NONE;
  int currenttime = millis();
  if (!bForceRead && ((currenttime - this->_lastReadTime) < 2000)) {
    this->_lastError= ERROR_TIMER_NOT_REACHED;
    return bRet;
  }

  uint8_t cycles[80];
  this->_lastReadTime = currenttime;
  // Reset 40 bits of received data to zero.
  this->DHT_data[0]=this->DHT_data[1]=this->DHT_data[2]=this->DHT_data[3]=this->DHT_data[4]=0;
  // High impedance state to let pull-up raise data line level and start the reading process.
  digitalWrite(_pin, 1);
  delay(1);
  // First set data line low for 20 milliseconds.
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, 0);
  if (this->_DHTtype == DHT22) {
    delayMicroseconds(1100);
  } else {
    delay(20);
  }

  do {
    // Turn off interrupts temporarily. This section timings are critical.
    pinDisableInterrupt(_pin);
    // End the start signal by setting data line high for 40 microseconds.
    digitalWrite(_pin, 1);
    delayMicroseconds(55);
    // Now start reading the data line to get the value from the DHT sensor.
    pinMode(_pin, INPUT| PULL_UP);
    // Delay a bit to let sensor pull data line low.
    delayMicroseconds(10);

    if (this->expectPulse(0) == 0) {
      // First expect a low signal for ~80 microseconds followed by a high signal for ~80 microseconds again.
      this->_lastError = ERROR_DATA_TIMEOUT;
      break;
    }

    if (this->expectPulse(1) == 0) {
      this->_lastError = ERROR_NOT_PRESENT;
      break;
    }

    for (uint8_t i = 0; i < 80; i += 2) {
      cycles[i] = this->expectPulse(0);
      cycles[i + 1] = this->expectPulse(1);
    }
    // Inspect pulses and determine which ones are 0 (high state cycle count < low
    for (uint8_t i = 0; i < 40 && this->_lastError == ERROR_NONE; ++i) {
      // state cycle count), or 1 (high state cycle count > low state cycle count).
      uint8_t lowCycles = cycles[2 * i];
      uint8_t highCycles = cycles[2 * i + 1];
      if ((lowCycles == 0) || (highCycles == 0)) {
        this->_lastError = ERROR_DATA_TIMEOUT;
        break;
      }
      // Now compare the low and high cycle times to see if the bit is a 0 or 1.
      this->DHT_data[i / 8] <<= 1;
      if (highCycles > lowCycles) {
        // High cycles are greater than 50us low cycle count, must be a 1.
        this->DHT_data[i / 8] |= 1;
      }
    }
  } while(0);
  // Turn on pin interrupts. Timing critical code is now complete.
  pinEnableInterrupt(_pin);

  // Check we read 40 bits and that the checksum matches.
  if(this->_lastError != ERROR_NONE) return bRet;

  if(this->DHT_data[4] != ((unsigned)(this->DHT_data[0] + this->DHT_data[1] + this->DHT_data[2] + this->DHT_data[3]) & 0xFF))
  {
    this->_lastError = ERROR_CHECKSUM;
    return bRet;
  }
  if (this->_DHTtype == DHT22) {
    if (this->DHT_data[2] & 0x80) {
      this->_lastTemperature = (((0x7F & this->DHT_data[2])<<8) + this->DHT_data[3])*-10;
    } else {
      this->_lastTemperature = ((this->DHT_data[2]<<8) + this->DHT_data[3])*10;
    }
    this->_lastHumidity= ((this->DHT_data[0]<<8) + this->DHT_data[1])*10;
  } else {
    if (this->DHT_data[2] & 0x80) {
      this->_lastTemperature= ((0x7F & this->DHT_data[2]) *-100);
    } else {
      this->_lastTemperature= this->DHT_data[2]*100;
    }
    this->_lastHumidity= this->DHT_data[0]*100;
  }
  this->_lastTemperature = this->_lastTemperature/100;
  this->_lastHumidity = this->_lastHumidity/100;

  bRet= true;
  return bRet;
}

/*****************************************************************************
** Function name:  ConvertTemperature
**
** Descriptions:  Will Converter _lastTemperatur to given scale
**
** parameters:    eScale: FARENHEIT | KELVIN | CELSIUS
**
** Returned value: Converter value
**
*****************************************************************************/
float DHT::ConvertTemperature(eScale Scale)
{
  if (Scale == FARENHEIT) return (this->_lastTemperature * 9 / 5 + 32);
  else if (Scale == KELVIN) return (this->_lastTemperature + 273.15);
  else return this->_lastTemperature;
}

/*****************************************************************************
** Function name:  expectPulse
**
** Descriptions:
**
** parameters:
**
** Returned value:
**
*****************************************************************************/
uint32_t DHT::expectPulse(bool level) {
  uint32_t count = 0;
  while (digitalRead(_pin) == level)
  {
    if (count++ >= this->_maxcycles) {
      return 0; // Exceeded timeout, fail.
    }
  }
  return count;
}

/*****************************************************************************
** Function name:  CalcdewPointFast
**
** Descriptions:  Calculate the Dew Point. %x faster than dewPoint().
**                Delta max = 0.6544 wrt dewPoint()
**                reference: http://en.wikipedia.org/wiki/Dew_point
**
** parameters:    celsius and humidity
**
** Returned value: Dew Point
**
*****************************************************************************/
float DHT::CalcdewPointFast(float celsius, float humidity)
{
  float temp = (17.271 * celsius) / (237.7 + celsius) + log(humidity/100);
  return ( (237.7 * temp) / (17.271 - temp) );
}

/*****************************************************************************
** Function name:  CalcdewPointFast
**
** Descriptions:  Calculate the Dew Point. dewPoint function NOAA
**                reference: http://wahiduddin.net/calc/density_algorithms.htm
**
** parameters:    celsius and humidity
**
** Returned value: Dew Point
**
*****************************************************************************/
/*
float DHT::CalcdewPoint(float celsius, float humidity)
{
  float T= ( log( pow( 10, (( (-7.90298 * (373.15/(273.15 + celsius)-1)) ) +
           ( 5.02808 * log10( 373.15/(273.15 + celsius)) ) +
           ( -1.3816e-7 * (pow( 10, (11.344*(1-1/373.15/(273.15 + celsius))))-1) ) +
           ( 8.1328e-3 * (pow( 10,(-3.49149*(373.15/(273.15 + celsius)-1)))-1) ) +
           ( log10( 1013.246)) ))-3) * humidity/0.61078); // temp var
  return (241.88 * T) / (17.558-T);
}
*/
