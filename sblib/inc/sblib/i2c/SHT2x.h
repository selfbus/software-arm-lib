/*
  SHT2x - A Humidity Library.

  Supported Sensor modules:
    SHT21-Breakout Module - http://www.moderndevice.com/products/sht21-humidity-sensor
    SHT2x-Breakout Module - http://www.misenso.com/products/001


 * This file is part of SHT2x.
 *
 * SHT2x is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or(at your option) any later version.
 *
 * SHT2x is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SHT2x.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Adapted to Selfbus Library by Oliver Stefan (2019)
 * Adapted to lpcopen I2C functions by Oliver Stefan (2021)
 */


#ifndef SHT2X_H
#define SHT2X_H

#include <stdint.h>

class SHT2xClass
{
private:
  uint16_t readSensor(uint8_t command);

public:
  void Init(void);
  /*
   * get Humidity from SHT2x sensor
   * returns the humidity with factor 100 (2045 = 20,45%RH)
   */
  int GetHumidity(void);
  /*
   * get Temperature from SHT2x sensor
   * returns the temperature with factor 100 (2045 = 20,45°C)
   */
  int GetTemperature(void);
  float GetDewPoint(void);
};

#endif
