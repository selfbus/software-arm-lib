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
public:
    /**
     * Create the SHT2xClass
     */
    SHT2xClass() = default;
    ~SHT2xClass() = default;

    /**
     * Initialize the SHTxx sensor
     */
    void Init(void);

    /**
     * Get humidity from SHT2x sensor
     * @return The humidity with factor 100 (2045 = 20,45%rH)
     */
    int GetHumidity(void);

    /**
    * Get temperature from SHT2x sensor
    * @return Temperature with factor 100 (2045 = 20,45°C)
    */
    int GetTemperature(void);

    /**
     * Get the current dew point based on the current humidity and temperature
     * @return The dew point in °C 
     */
    float GetDewPoint(void);

private:
    uint16_t readSensor(uint8_t command);
    bool initialized = false;

protected:
    uint8_t crc8(const uint8_t *data, uint8_t len);

};

#endif
