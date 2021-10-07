/*
 *  dht.h - Digital-output Humidity and Temperature Sensors
 *
 *  Copyright (c) 2015 Erkan Colak <erkanc@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef dht_h
#define dht_h
#include <sblib/sensors/units.h>

/****************************************************************************
 *  Works with DHT11, DHT22
 *      SEN11301P,  Grove - Temperature & Humidity Sensor    (Seeed Studio)
 *      SEN51035P,  Grove - Temperature & Humidity Sensor Pro (Seeed Studio)
 *      AM2302   ,  Temperature-Humidity Sensor
 *      HM2303   ,  Digital-output humidity and temperature sensor
 ****************************************************************************/
enum eType {
  DHT11     = 11,
  SEN11301P = 11,
  RHT01     = 11,
  DHT22     = 22,
  AM2302    = 22,
  SEN51035P = 22,
  RHT02     = 22,
  RHT03     = 22
};

enum eLastError {
  ERROR_NONE              = 0,
  BUS_BUSY                = 1,
  ERROR_NOT_PRESENT       = 2,
  ERROR_ACK_TOO_LONG      = 3,
  ERROR_SYNC_TIMEOUT      = 4,
  ERROR_DATA_TIMEOUT      = 5,
  ERROR_CHECKSUM          = 6,
  ERROR_NO_PATIENCE       = 7,
  ERROR_TIMER_NOT_REACHED = 8
};

/****************************************************************************
* DHT Class
*****************************************************************************/
class DHT
{
public:
  static const uint32_t leadTime = 2000; // once every 2 seconds
  float _lastTemperature, _lastHumidity;
  eLastError _lastError;
  uint8_t _DHTtype;
  uint8_t DHT_data[6];

  void DHTInit(int pin,int DHTtype);
  bool readData(bool bForceRead =false );
  float ReadHumidity(void);
  float ConvertTemperature(eScale Scale);
  float CalcdewPointFast(float celsius, float humidity);
  //float CalcdewPoint(float celsius, float humidity);
private:
  int  _pin, _lastReadTime;
  uint32_t _maxcycles;
  uint32_t expectPulse(bool level);
};

#endif /* dht_h */
