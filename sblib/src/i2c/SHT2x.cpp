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

#include <stdint.h>
#include <math.h>
#include <sblib/i2c.h>
#include <sblib/timer.h>
#include <sblib/i2c/SHT2x.h>

// Specify the constants for water vapor and barometric pressure.
#define WATER_VAPOR 17.62f
#define BAROMETRIC_PRESSURE 243.5f

typedef enum {
  eSHT2xAddress = 0x40,
} HUM_SENSOR_T;

typedef enum {
  eTempHoldCmd        = 0xE3,
  eRHumidityHoldCmd   = 0xE5,
  eTempNoHoldCmd      = 0xF3,
  eRHumidityNoHoldCmd = 0xF5,
} HUM_MEASUREMENT_CMD_T;

void SHT2xClass::Init(void)
{
  i2c_lpcopen_init();
}

int SHT2xClass::GetHumidity(void)
{
  unsigned int value = readSensor(eRHumidityHoldCmd);
  if (value == 0 || !(value & 0x2)) {
    return 0; // Some unrealistic value
  }

  value = value & 0xFFFC; //remove last two status Bits
  value = 12500 * value;
  value = value / 65536;
  value = value -600;
  return value; //-600 + 12500 / 65536 * value;  //changed to int and factor 100
}

int SHT2xClass::GetTemperature(void)
{
  int value = readSensor(eTempHoldCmd);
  if (value == 0 || (value & 0x2)) {
    return -273;                    // Roughly Zero Kelvin indicates an error
  }

  value = value & 0xFFFC; //remove last two status Bits
  value = 17572 * value;
  value = value / 65536;
  value = value - 4685;
  return value; //-4685 + 17572 / 65536 * value;	//changed to int and factor 100
}

float SHT2xClass::GetDewPoint(void)
{
  float humidity = GetHumidity();
  float temperature = GetTemperature();

  // Calculate the intermediate value 'gamma'
  float gamma = log(humidity / 100) + WATER_VAPOR * temperature / (BAROMETRIC_PRESSURE + temperature);
  // Calculate dew point in Celsius
  float dewPoint = BAROMETRIC_PRESSURE * gamma / (WATER_VAPOR - gamma);

  return dewPoint;
}

uint16_t SHT2xClass::readSensor(uint8_t command)
{
  uint8_t result[3];

  uint32_t timeout = millis() + 300; // 300ms timeout for I2C communication

  // loop to receive measurement result within the timeout period
  while (Chip_I2C_MasterCmdRead(I2C0, eSHT2xAddress, command, result, 3) == 0){
    if ((millis() - timeout) > 0) {
      i2c_lpcopen_init();
      return 0;
    }
  }

  //TODO: CRC8 and status Bit verification
  if (crc8(result, 2) != result[2])
  {
    return 0;
  }

  // Concatenate result Bytes
  return ((result[0] << 8) | (result[1] << 0));
}

uint8_t SHT2xClass::crc8(const uint8_t *data, uint8_t len)
{
  // CRC-8 formula from page 14 of SHT spec pdf
  // Sensirion_Humidity_Sensors_SHT2x_CRC_Calculation.pdf
  const uint8_t POLY = 0x31;
  uint8_t crc = 0x00;

  for (uint8_t j = len; j; --j)
  {
    crc ^= *data++;

    for (uint8_t i = 8; i; --i)
    {
      crc = (crc & 0x80) ? (crc << 1) ^ POLY : (crc << 1);
    }
  }
  return crc;
}
