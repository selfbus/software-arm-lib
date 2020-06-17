/*
 *  ds18x20.h - Digital-output 1-Wire Temperature Sensors
 *
 *  Copyright (c) 2015 Erkan Colak <erkanc@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef ds18x20_h
#define ds18x20_h
#include <sblib/sensors/units.h>

#ifndef onewire_h
//#error "Please Include first <OneWire.h>"
#include <sblib/onewire.h>
#endif

#ifndef MAX_DS_DEVICES
#define MAX_DS_DEVICES 10
#endif

#define DS18X20_SEARCH 1

#if DS18X20_SEARCH
// 1-Wire family code
enum eDsType {
  DS18S20 = 0x10,  // DS18S20 & DS1820
  DS18B20 = 0x28,  // MAX31820 & DS18B20
  DS1822  = 0x22,
  DS_UNKNOWN = 0x00 // Unknown or No Sensor found
};
#endif

// Ds18x20 device struct
struct sDS18x20
{
#if DS18X20_SEARCH
  eDsType type;             // Device type
#endif
  uint8_t res_type;         // Resolution type 0 or 1
  byte data[12];            // Read data
  // Device ROM. 64bit Lasered ROM-Code to detect the Family Code
  byte addr[8];
  bool crcOK;               // Last read crc state
  bool lastReadOK;          // Read state of last received value
  float current_temperature;// Current temperature
  float last_temperature;   // Last temperature. Use always This!
  // Indicating that we're waiting for readyAt
  bool conversionStarted;
  // System time at which started conversion is readable
  unsigned int readyAt;
};

/*
 * DHT Class
 */
class DS18x20
{
private:
  OneWire _OW_DS18x;

public:
  // time it takes for a reading
  // TODO: These vary:
  //  DS18S20 9 bit 750ms (0.5deg) / 2
  //  DS18B20 12 bit = 750ms (0.0625deg) / 16
  //  DS18B20 11 bit = 375ms (0.125deg) / 8
  //  DS18B20 10 bit = 187.5ms (0.25deg) / 4
  //  DS18B20 9 bit = 93.75ms (0.5deg) / 2
  static const uint32_t leadTime = 750 + 50; // just in case
  uint8_t m_foundDevices;           // Number of found devices
  sDS18x20 m_dsDev[MAX_DS_DEVICES]; // Object list and informations of the found devices

 /*
  * Function name:  ds18x20Init
  * Descriptions:   Initialize the ds18x20
  *                 Initialize DS18x20 only once.
  *                 bParasiteMode is default off!
  * parameters:     pin (PORT) and bParasiteMode (set to true if power via the dataline)
  * Returned value: none
  */
  void DS18x20Init(int pin, bool bParasiteMode);

 /*
  * Function name:  ds18x20DeInit
  * Descriptions:   DeInitialize the ds18x20
  * parameters:     none
  * Returned value: none
  */
  void DS18x20DeInit();

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
#if DS18X20_SEARCH
  uint8_t Search(uint8_t uMaxDeviceSearch= MAX_DS_DEVICES);
#endif

/*
 * Function name:  startConversion
 *
 * Descriptions:   Starts the Temperature conversion of the given sDS18x20
 *                 device.
 *
 * parameters:     device index of sensor DS18x20 device.(Is filled automatically by Search() function.)
 *
 * Returned value: true, if the device was accesed successfully.
 *                 following global Parameter of sDS18x20 will be filled:
 *
 */
  bool startConversion(int deviceIdx);

/*
 * Function name:  readResult
 *
 * Descriptions:   Reads the Temperature of the given sDS18x20 device.
 *
 * parameters:     device index of sensor DS18x20 device.(Is filled automatically by Search() function.)
 *
 * Returned value: true, if the device was read successfully.
 *                 following global Parameter of sDS18x20 will be filled:
 *                 last_temperature - the current read temperature
 *                 lastReadOK       - will be set to true if read was successful
 *
 */
  bool readResult(int devieIdx);

/*
 * Function name:  startConversionAll
 *
 * Descriptions:   Iterates the the global m_dsDev object and calls
 *                 startConversion() for each device.
 *
 * Returned value: true, if one or more devices started successfully.
 *
 */
  bool startConversionAll();

  // convenience wrappers
  bool lastReadOk(int deviceIdx);
  float temperature(int deviceIdx);

/*
 * Function name:  readResultAll
 *
 * Descriptions:   Iterates the the global m_dsDev object and calls
 *                 readResult() for each device.
 *
 * Returned value: true, if one or more devices reads are successful.
 *                 following global object parameters will be filled:
 *                 last_temperature - the current read temperature
 *                 lastReadOK       - will be set to true if read was successful
 *
 */
  bool readResultAll();

 /*
  * Returns a string representation of the requested sensor
  */
  const char* TypeStr(int deviceNum);

 /*
  * Function name:  ConvertTemperature
  * Descriptions:  Will convert given fTemperature to given scale
  * parameters:    fTemperature: the Temperature to convert
  *                eScale: FARENHEIT | KELVIN | CELSIUS
  * Returned value: Converted value
  */
  float ConvertTemperature(float fTemperature, eScale Scale);
};

#endif /* ds18x20_h */
