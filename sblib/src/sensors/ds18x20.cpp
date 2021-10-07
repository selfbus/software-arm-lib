/*
 *  ds18x20.h - 1-Wire Temperature Sensors
 *
 *  Copyright (c) 2015 Erkan Colak <erkanc@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as published
 *  by the Free Software Foundation.
 */

#include <math.h>
#include <sblib/core.h>

#include <sblib/sensors/ds18x20.h>


/*****************************************************************************
** Function name:  ds18x20Init
**
** Descriptions:   Initialize the ds18x20
**                 Initialize DS18x20 only once.
**                 bParasiteMode is default off!
**
** parameters:     pin (PORT) and bParasiteMode (set to true if power via the dataline)
**
** Returned value: none
**
*****************************************************************************/
void DS18x20::DS18x20Init(int pin, bool bParasiteMode)
{
  this->_OW_DS18x.OneWireInit(pin, bParasiteMode);
}

/*****************************************************************************
** Function name:  ds18x20DeInit
**
** Descriptions:   No op
**
** parameters:     none
**
** Returned value: none
**
*****************************************************************************/
void DS18x20::DS18x20DeInit()
{
}
#if DS18X20_SEARCH
/*****************************************************************************
** Function name:  Search
**
** Descriptions:   Search for 1-Wire DS18x20 Family devices.
**
** parameters:     uMaxDeviceSearch Maximal count of devices to find.
**
** Returned value: 0, one or more devices were found.
**                 1, unknown device type on at least one device
**                 2, CRC8 failed on at least one device
**                 4, the search failed no devices
**
**                 On Success, the following global Parameters will be filled:
**                 m_foundDevices - Includes the number of found devices
**                 m_dsDev        - Includes the Information about the devices
**
*****************************************************************************/
uint8_t DS18x20::Search(uint8_t uMaxDeviceSearch)
{
  uint8_t bRet= 4;
  this->m_foundDevices= 0;
  this->_OW_DS18x.OneWireResetSearch(); // do a reset to start the search
  sDS18x20 sDevTmp;
  for(uint8_t j=0; this->_OW_DS18x.OneWireSearch(sDevTmp.addr) > 0 && j < uMaxDeviceSearch; j++)
  {
    sDevTmp.crcOK= (OneWire::OneWireCRC8(sDevTmp.addr, 7) == sDevTmp.addr[7]);
    if( sDevTmp.crcOK )
    {
      switch(sDevTmp.addr[0])
      {
        case DS18S20: sDevTmp.type= DS18S20; break;
        case DS18B20: sDevTmp.type= DS18B20; break;
        case DS1822 : sDevTmp.type= DS1822 ; break;
        default: sDevTmp.type= DS_UNKNOWN; break;
      }
      if( sDevTmp.type != DS_UNKNOWN)
      {
        sDevTmp.res_type= (sDevTmp.type == DS18S20)? 1: 0;
        sDevTmp.conversionStarted = false;
        this->m_dsDev[j]= sDevTmp;
        this->m_foundDevices++;
        bRet= 0; // Found one or more devices!
      } else bRet = 1;
    } else bRet =2;
  }
  return bRet;
}
#endif

/*****************************************************************************
** Function name:  startConversion
**
** Descriptions:   Starts the Temperature conversion of the given sDS18x20
**                 device.
**
** parameters:     sDS18x20 *sDev: which must include the '.addr' of the
**                 DS18x20 device.(Is filled automatically by Search() function.)
**
** Returned value: true, if the device was accesed successfully.
**                 following global Parameter of sDS18x20 will be filled:
**
*****************************************************************************/
bool DS18x20::startConversion(int deviceIdx)
{
  if (deviceIdx >= this->m_foundDevices) return false;
  sDS18x20 *sDev = &m_dsDev[deviceIdx];
  if( ( sDev->type != DS_UNKNOWN || sDev->addr[0] ) && this->_OW_DS18x.OneWireReset() )
  {
    this->_OW_DS18x.OneWireSelect( sDev->addr );
    // start conversion, with parasite power on at the end
    this->_OW_DS18x.OneWireWrite(0x44);
    sDev->conversionStarted = true;
    // maybe 750ms is enough, maybe not. We might do a
    // _OW_DS18x.OneWireDePower() here, but the reset will take care of it.
    sDev->readyAt = millis() + leadTime;
    return true;
  }
  return false;
}

bool DS18x20::readResult(int deviceIdx)
{
  if (deviceIdx >= this->m_foundDevices) return false;
  sDS18x20* sDev = &this->m_dsDev[deviceIdx];
  if (!sDev->conversionStarted || millis() < sDev->readyAt) return false;

  if(!this->_OW_DS18x.OneWireReset()) return false;

  this->_OW_DS18x.OneWireSelect(sDev->addr);
  this->_OW_DS18x.OneWireWrite(0xBE);     // Read Scratchpad
  if( this->_OW_DS18x.IsParasiteMode() ) this->_OW_DS18x.OneWireDePower();
  for ( uint8_t i = 0; i < 9; i++) // we need 9 bytes
  {
    sDev->data[i] = this->_OW_DS18x.OneWireRead();
  }

  // Convert the data to actual temperature because the result is a 16 bit
  // signed integer, it should be stored to an "int16_t" type, which is always
  // 16 bits even when compiled on a 32 bit processor.
  int16_t raw = (sDev->data[1] << 8) | sDev->data[0];
  if(sDev->res_type)
  {
    raw = raw << 3;                // 9 bit resolution default
    if (sDev->data[7] == 0x10)  // "count remain" gives full 12 bit resolution
    {
      raw = (raw & 0xFFF0) + 12 - sDev->data[6];
    }
  } else {
    byte cfg = (sDev->data[4] & 0x60);

    // at lower res, the low bits are undefined, so let's zero them
    if(cfg == 0x00) raw = raw & ~7;         // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3;   // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1;   // 11 bit res, 375 ms
    // default is 12 bit resolution, 750 ms conversion time
  }
  sDev->current_temperature= (float)raw / 16.0;
  sDev->lastReadOK= (sDev->current_temperature >=-55 && (sDev->current_temperature) <= 125);
  if( sDev->lastReadOK ) {
      sDev->last_temperature= sDev->current_temperature;
  }
  sDev->conversionStarted = false;
  return true;
}

bool DS18x20::lastReadOk(int deviceIdx)
{
  if (deviceIdx >= this->m_foundDevices) return false;
  return this->m_dsDev[deviceIdx].lastReadOK;
}

float DS18x20::temperature(int deviceIdx)
{
  if (deviceIdx >= this->m_foundDevices) return -999.9;
  return this->m_dsDev[deviceIdx].last_temperature;
}

/*****************************************************************************
** Function name:  startConversionAll
**
** Descriptions:   Iterates the the global m_dsDev object and calls
**                 startConversion() for each device.
**
** Returned value: true, if one or more devices started successfully.
**
*****************************************************************************/
bool DS18x20::startConversionAll()
{
  bool bRet=false;

  for(uint8_t j=0; j < this->m_foundDevices; j++)
  {
    if( this->startConversion(j) )
    {
      bRet= true; // Read one or more was successful.
    }
  }
  return bRet;
}

/*****************************************************************************
** Function name:  readResultAll
**
** Descriptions:   Iterates the the global m_dsDev object and calls
**                 readResult() for each device.
**
** Returned value: true, if one or more devices reads are successful.
**                 following global object parameters will be filled:
**                 last_temperature - the current read temperature
**                 lastReadOK       - will be set to true if read was successful
**
*****************************************************************************/
bool DS18x20::readResultAll()
{
  bool bRet=false;

  for(uint8_t j=0; j < this->m_foundDevices; j++)
  {
    if( this->readResult(j) )
    {
      bRet= true; // Read one or more was successful.
    }
  }
  return bRet;
}

/*
 * Returns a string representation of the requested sensor
 */
const char* DS18x20::TypeStr(int deviceNum)
{
  if (deviceNum < this->m_foundDevices)
  {
    switch (this->m_dsDev[deviceNum].addr[0])
    {
      case DS18S20:
        return "DS18S20";
      case DS18B20:
        return "DS18B20";
      case DS1822 :
        return "DS1822";
    }
  }
  return "UNKNOWN";
}

/*****************************************************************************
** Function name: ConvertTemperature
**
** Descriptions:  Will convert given fTemperature to given scale
**
** parameters:    fTemperature: the Temperature to convert
**                eScale: FARENHEIT | KELVIN | CELSIUS
**
** Returned value: Converted value
**
*****************************************************************************/
float DS18x20::ConvertTemperature(float fTemperature, eScale Scale)
{
  if (Scale == FARENHEIT) return (fTemperature * 9 / 5 + 32);
  else if (Scale == KELVIN) return (float)(fTemperature + 273.15);
  else return fTemperature;
}
