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
 *  Sensirion code adapted to Selfbus Library by Doumanix (2023)
 */

#include <stdint.h>
#include <math.h>
#include <sblib/i2c.h>
#include <sblib/timer.h>
#include <sblib/i2c/SHT4x.h>

#include <sblib/serial.h>
#include <sblib/internal/iap.h>

// Specify the constants for water vapor and barometric pressure.
#define WATER_VAPOR 17.62f
#define BAROMETRIC_PRESSURE 243.5f

typedef enum {
  eSHT4xAddress = 0x44,
} HUM_SENSOR_T;




/******************************************************************************
 * Global Functions
 ******************************************************************************/

/*****************************************************************************
** Function name:  init
**
** Descriptions:   Initialize the SHT41
**
** parameters:     none
**
** Returned value: true on success, false on failure
**
*****************************************************************************/
void SHT4xClass::init(void)
{
	i2c_lpcopen_init();

	#if DEBUG
		serial.setRxPin(PIO1_6); // @ swd/jtag connector
		serial.setTxPin(PIO1_7); // @ swd/jtag connector
		serial.begin(115200);
		serial.println("Selfbus lib I2C SHT4x.cpp");
		serial.print("Target MCU has ", iapFlashSize() / 1024);
		serial.println("k flash");
	#endif


}

uint16_t SHT4xClass::measureHighPrecision(float &temperature, float &humidity) {
    uint16_t error = 0;
    uint16_t tTicks = 0;
    uint16_t hTicks = 0;

    error = measureHighPrecisionTicks(tTicks, hTicks);
    if (error != 6) {
        return error;
    }

    this->temperature = _convertTicksToCelsius(tTicks);
    this->humidity = _convertTicksToPercentRH(hTicks);

	#if DEBUG
		serial.println("temperature === ", (int)(this->temperature*100));
		serial.println("humidity ===", (int)(this->humidity*100));
	#endif

	return error;
}


uint16_t SHT4xClass::measureHighPrecisionTicks(uint16_t &temperatureTicks, uint16_t &humidityTicks)
{
	uint8_t buffer[6] = {};
    uint8_t resultLength = 111;

    resultLength = readSensor(Sht4xCommand::measHi, buffer, sizeof(buffer));

	temperatureTicks = buffer[0];
	temperatureTicks <<= 8;
	temperatureTicks |= buffer[1];
	humidityTicks = buffer[3];
	humidityTicks <<= 8;
	humidityTicks |= buffer[4];

    return resultLength;
}


float SHT4xClass::_convertTicksToCelsius(uint16_t ticks) {
    return static_cast<float>(ticks * 175.0 / 65535.0 - 45.0);
}

float SHT4xClass::_convertTicksToPercentRH(uint16_t ticks) {
    return static_cast<float>(ticks * 125.0 / 65535.0 - 6);
}

/**********************************************************
 * GetHumidity
 *  Gets the current humidity from the sensor.
 *
 * @return float - The relative humidity in %RH
 **********************************************************/
//float SHT4xClass::GetHumidity(void)
//{
//    float value = readSensor(eRHumidityHoldCmd);
//    if (value == 0) {
//        return 0;                       // Some unrealistic value
//    }
//    return -6.0 + 125.0 / 65536.0 * value;
//}
float SHT4xClass::getHumidity(void)
{
	return this->humidity;
}

/**********************************************************
 * GetTemperature
 *  Gets the current temperature from the sensor.
 *
 * @return float - The temperature in Deg C
 **********************************************************/
float SHT4xClass::getTemperature(void)
{
	return this->temperature;
}




///**********************************************************
// * GetDewPoint
// *  Gets the current dew point based on the current humidity and temperature
// *
// * @return float - The dew point in Deg C
// **********************************************************/
//float SHT4xClass::getDewPoint(void)
//{
//  float humidity = getHumidity();
//  float temperature = getTemperature();
//
//  // Calculate the intermediate value 'gamma'
//  float gamma = log(humidity / 100) + WATER_VAPOR * temperature / (BAROMETRIC_PRESSURE + temperature);
//  // Calculate dew point in Celsius
//  float dewPoint = BAROMETRIC_PRESSURE * gamma / (WATER_VAPOR - gamma);
//
//  return dewPoint;
//}




uint32_t SHT4xClass::getSerialnumber(void)
{
	uint8_t result[6] = {};
	uint32_t serialnumber = 0;

//	int readErgebnis = Chip_I2C_MasterWriteRead(I2C0, eSHT4xAddress, &command, result, 1, 6);
	readSensor(Sht4xCommand::getSerial, result, sizeof(result));

	serialnumber = static_cast<uint32_t>(result[0]) << 24;
	serialnumber |= static_cast<uint32_t>(result[1]) << 16;
	serialnumber |= static_cast<uint32_t>(result[3]) << 8;
	serialnumber |= static_cast<uint32_t>(result[4]);

	return serialnumber;
}




/******************************************************************************
 * Private Functions
 ******************************************************************************/

uint16_t SHT4xClass::readSensor(Sht4xCommand command, uint8_t* buffer, uint8_t expResultLength)
{
	uint8_t cmd = (uint8_t)command;
	uint32_t timeout = millis() + 300; // 300ms timeout for I2C communication
	uint8_t resultLength = 0;
//
//  if (!initialized)
//  {
//    Init();
//  }
//
//  if (initialized)
//  {

	if(Chip_I2C_MasterSend(I2C0, eSHT4xAddress, &cmd, sizeof(cmd)) == 0){
		#if DEBUG
			serial.println("i2c send failed!");
		#endif
		i2c_lpcopen_init();
		return 0;
	}

    // loop to receive measurement result within the timeout period
    while (resultLength < expResultLength){
		#if not DEBUG
			if ((millis() - timeout) > 0) {
				i2c_lpcopen_init();
				return 0;
			}
		#endif
		delay(1);
		resultLength = Chip_I2C_MasterRead(I2C0,eSHT4xAddress,buffer,6);
    }

	// check if CRC of measure result is valid
	if (buffer[2] != crc8(buffer, 2) || buffer[5] != crc8(buffer + 3, 2))
		return false;

    return resultLength;
}


/**
 * Do a CRC validation of result
 *
 * @param data  Pointer to the data to use when calculating the CRC8.
 * @param len   The number of bytes in 'data'.
 *
 * @return CRC result value
 */
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

  for (int j = len; j; --j) {
    crc ^= *data++;

    for (int i = 8; i; --i) {
      crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
    }
  }
  return crc;
}
//SHT4xClass SHT4x;


