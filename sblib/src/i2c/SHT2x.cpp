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
 *  Adapted to Selfbus Library by Oliver Stefan (2019)
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


/******************************************************************************
 * Global Functions
 ******************************************************************************/

/*****************************************************************************
** Function name:  Init
**
** Descriptions:   Initialize the SHT21
**
** parameters:     none
**
** Returned value: true on success, false on failure
**
*****************************************************************************/
void SHT2xClass::Init(void)
{
  i2c_lpcopen_init();
  /*
  i2c_sht2x= I2C::Instance();
  if(!i2c_sht2x->bI2CIsInitialized) {
      i2c_sht2x->I2CInit();
  }
  return (i2c_sht2x ? true:false);*/
}

/**********************************************************
 * GetHumidity
 *  Gets the current humidity from the sensor.
 *
 * @return float - The relative humidity in %RH
 **********************************************************/
//float SHT2xClass::GetHumidity(void)
//{
//    float value = readSensor(eRHumidityHoldCmd);
//    if (value == 0) {
//        return 0;                       // Some unrealistic value
//    }
//    return -6.0 + 125.0 / 65536.0 * value;
//}
int SHT2xClass::GetHumidity(void)
{
  unsigned int value = readSensor(eRHumidityHoldCmd);
  if (value == 0) {
    return 0;                       // Some unrealistic value
  }
  value = 12500 * value;
  value = value / 65536;
  value = value -600;
  return value; //-600 + 12500 / 65536 * value;  //changed to int and factor 100
}

/**********************************************************
 * GetTemperature
 *  Gets the current temperature from the sensor.
 *
 * @return float - The temperature in Deg C
 **********************************************************/
//float SHT2xClass::GetTemperature(void)
//{
//    float value = readSensor(eTempHoldCmd);
//    if (value == 0) {
//        return -273;                    // Roughly Zero Kelvin indicates an error
//    }
//    return -46.85 + 175.72 / 65536.0 * value;
//}
int SHT2xClass::GetTemperature(void)
{
  unsigned int value = readSensor(eTempHoldCmd);
  if (value == 0) {
    return -273;                    // Roughly Zero Kelvin indicates an error
  }
  value = 17600 * value;
  value = value / 65536;
  value = value - 4700;
  return value; //-4700 + 17600 / 65536 * value;	//changed to int and factor 100
}

/**********************************************************
 * GetDewPoint
 *  Gets the current dew point based on the current humidity and temperature
 *
 * @return float - The dew point in Deg C
 **********************************************************/
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

/******************************************************************************
 * Private Functions
 ******************************************************************************/

uint16_t SHT2xClass::readSensor(uint8_t command)
{
  uint8_t result[3];
  //result[0] = command;
  //if( i2c_sht2x->Write(eSHT2xAddress, (const char*) &command, sizeof(command)) == false){

  // wenn kein Byte versendet werden konnte
  if(Chip_I2C_MasterSend(I2C0, eSHT2xAddress, &command, sizeof(command)) == 0){
    i2c_lpcopen_init();
    //i2c_sht2x->I2CInit();
    return 0;
  }

  uint32_t timeout = millis() + 300;       // Don't hang here for more than 300ms


  //while (i2c_sht2x->Read(eSHT2xAddress, (char*) result, 3, 0) == false) {
  // so lange, wei kein Byte empfangen wurde
  while (Chip_I2C_MasterCmdRead(I2C0, eSHT2xAddress, command, result, 3) == 0){
    if ((millis() - timeout) > 0) {
      //i2c_sht2x->I2CInit();
      i2c_lpcopen_init();
      return 0;
    }
  }

  return ((result[0] << 8) | (result[1] << 0));
}

//SHT2xClass SHT2x;
