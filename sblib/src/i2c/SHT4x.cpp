/*
  SHT4x - a Temperature and Humidity Library.
*/
/*
 * Copyright (c) 2021, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file is part of SHT4x.
 *
 * Sensirion code adapted to Selfbus Library by Doumanix (2023)
 */

#include <stdint.h>
#include <sblib/math.h>
#include <sblib/i2c.h>
#include <sblib/timer.h>
#include <sblib/i2c/SHT4x.h>

#define WATER_VAPOR         (17.62f) //!< constant for water vapor
#define BAROMETRIC_PRESSURE (243.5f) //!< constant for barometric pressure

bool SHT4xClass::init(void)
{
	i2c_lpcopen_init();

	bool initialized = writeCommand(Sht4xCommand::softReset);
	delay(1);
	return initialized;
}

bool SHT4xClass::measureHighPrecision()
{
    uint16_t tTicks = 0;
    uint16_t hTicks = 0;

    if (!measureHighPrecisionTicks(tTicks, hTicks))
    {
        return false;
    }

    this->temperature = convertTicksToCelsius(tTicks);
    this->humidity = convertTicksToPercentRH(hTicks);
    return true;
}

bool SHT4xClass::measureHighPrecisionTicks(uint16_t &temperatureTicks, uint16_t &humidityTicks)
{
	uint8_t buffer[6] = {};

    if (!readSensor(Sht4xCommand::measHi, buffer, sizeof(buffer) / sizeof(*buffer)))
    {
        return false;
    }

    temperatureTicks = buffer[0];
    temperatureTicks <<= 8;
    temperatureTicks |= buffer[1];
    this->temperatureTicks = temperatureTicks;
    humidityTicks = buffer[3];
    humidityTicks <<= 8;
    humidityTicks |= buffer[4];
    this->humidityTicks = humidityTicks;
    return true;
}


float SHT4xClass::convertTicksToCelsius(uint16_t ticks)
{
    return static_cast<float>(ticks * 175.0f / 65535.0f - 45.0f);
}

float SHT4xClass::convertTicksToPercentRH(uint16_t ticks)
{
    return static_cast<float>(ticks * 125.0f / 65535.0f - 6.0f);
}

float SHT4xClass::getHumidity(void)
{
	return this->humidity;
}

float SHT4xClass::getTemperature(void)
{
	return this->temperature;
}

uint16_t SHT4xClass::getHumTicks(void)
{
	return this->humidityTicks;
}

uint16_t SHT4xClass::getTempTicks(void)
{
	return this->temperatureTicks;
}

float SHT4xClass::getDewPoint(void)
{
/*
  float humidity = getHumidity();
  float temperature = getTemperature();

  // Calculate the intermediate value 'gamma'
  float gamma = log(humidity / 100.0f) + WATER_VAPOR * temperature / (BAROMETRIC_PRESSURE + temperature);
  // Calculate dew point in Celsius
  float dewPoint = BAROMETRIC_PRESSURE * gamma / (WATER_VAPOR - gamma);

  return (dewPoint);
*/
    return (-1.0f); // function not implemented
}

uint32_t SHT4xClass::getSerialnumber(void)
{
	uint8_t result[6] = {};
	uint32_t serialnumber;

	if (!readSensor(Sht4xCommand::getSerial, result, sizeof(result) / sizeof(*result)))
	{
	    return 0;
	}

    serialnumber = static_cast<uint32_t>(result[0]) << 24;
    serialnumber |= static_cast<uint32_t>(result[1]) << 16;
    serialnumber |= static_cast<uint32_t>(result[3]) << 8;
    serialnumber |= static_cast<uint32_t>(result[4]);
	return serialnumber;
}

/******************************************************************************
 * Private Functions
 ******************************************************************************/
bool SHT4xClass::writeCommand(Sht4xCommand command)
{
	uint8_t cmd = (uint8_t)command;

	return Chip_I2C_MasterSend(I2C0, eSHT4xAddress, &cmd, sizeof(cmd)) == sizeof(cmd);
}

bool SHT4xClass::readSensor(Sht4xCommand command, uint8_t* buffer, uint8_t bufferLength)
{
	uint8_t resultLength;

	// each response is 6 bytes in size, so if buffer is smaller we can't process the response
	// as the crc would be incomplete
	if (bufferLength < 6)
	{
	    return false;
	}

    if (!writeCommand(command))
    {
        return false;
    }

	uint32_t timeout = millis() + 300; // 300ms timeout for I2C communication
    // loop to receive measurement result within the timeout period
    do
    {
        if (millis() > timeout)
        {
            i2c_lpcopen_init();
            return false;
        }
        delay(1);
        resultLength = Chip_I2C_MasterRead(I2C0,eSHT4xAddress,buffer, 6);
    }
    while (resultLength == 0);

    // check if CRC of measure result is valid
    return resultLength == 6 &&
             buffer[2] == crc8(buffer, 2) &&
             buffer[5] == crc8(buffer + 3, 2);
}

uint8_t SHT4xClass::crc8(const uint8_t *data, int len) {
  /*
   *
   * CRC-8 formula from page 14 of SHT spec pdf
   *
   * Test data 0xBE, 0xEF should yield 0x92
   *
   * Initialization data 0xFF
   * Polynomial 0x31 (x8 + x5 +x4 +1)
   * Final XOR 0x00
   */

  const uint8_t POLYNOMIAL(0x31);
  uint8_t crc(0xFF);

  for (int j = len; j; --j)
  {
    crc ^= *data++;

    for (int i = 8; i; --i)
    {
      crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
    }
  }
  return crc;
}


