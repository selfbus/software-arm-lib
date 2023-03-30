/*
  SHT4x - A Humidity Library.

  Supported Sensor modules:
    SHT41-Breakout Module - http://www.moderndevice.com/products/SHT41-humidity-sensor
    SHT4x-Breakout Module - http://www.misenso.com/products/001


 * This file is part of SHT4x.
 *
 * SHT4x is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or(at your option) any later version.
 *
 * SHT4x is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SHT4x.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Adapted to Selfbus Library by Oliver Stefan (2019)
 * Adapted to lpcopen I2C functions by Oliver Stefan (2021)
 */

/*
 * This file is part of SHT4x.
 *
 *  Sensirion code adapted to Selfbus Library by Doumanix (2023)
 */


#ifndef SHT4X_H
#define SHT4X_H

#include <stdint.h>

enum class Sht4xCommand : uint8_t {
	measHi 				= 0xFD,
	measMed 			= 0xF6,
	measLo				= 0xE0,
	getSerial 			= 0x89,
	softReset			= 0x94,
	measHiHeatHiLong	= 0x39,
	measHiHeatHiShort	= 0x32,
	measHiHeatMidLong	= 0x2F,
	measHiHeatMidShort 	= 0x24,
	measHiHeatLoLong	= 0x1E,
	measHiHeatLoShort 	= 0x15,
	testTrash			= 0x0D
};

class SHT4xClass
{
private:
	float temperature;
	float humidity;
//  uint16_t readSensor(Sht4xCommand command);

public:
	uint16_t readSensor(Sht4xCommand command, uint8_t* buffer, uint8_t resultlength);
//	uint16_t readSensor(Sht4xCommand command);
  /**
   * Initialize the SHTxx sensor
   */
  void init(void);

  /**
   * get Humidity from SHT4x sensor
   * returns the humidity as float
   */
  float getHumidity(void);

  /**
   * get Temperature from SHT4x sensor
   * returns the current temperature as float
   */
  float getTemperature(void);

  /**
   * measureHighPrecisionTicks() - SHT4x command for a single shot measurement
   * with high repeatability.
   *
   * @param temperatureTicks Temperature ticks. Convert to degrees celsius by
   * (175 * value / 65535) - 45
   *
   * @param humidityTicks Humidity ticks. Convert to degrees celsius by (125 *
   * value / 65535) - 6
   *
   * @return 0 on success, an error code otherwise
   */
  uint16_t measureHighPrecisionTicks(uint16_t& temperatureTicks, uint16_t& humidityTicks);


  /**
   * measureHighPrecision() - SHT4x command for a single shot
   * measurement with high repeatability.
   *
   * @param temperature Temperature in degrees celsius.
   *
   * @param humidity Humidity in percent relative humidity.
   *
   * @return 0 on success, an error code otherwise
   */
  uint16_t measureHighPrecision(float &temperature, float &humidity);

  float getDewPoint(void);
  uint32_t getSerialnumber(void);
  float _convertTicksToCelsius(uint16_t);
  float _convertTicksToPercentRH(uint16_t);
  uint8_t crc8(const uint8_t* , int);

};

#endif  /*sblib_SHT4x_h*/


