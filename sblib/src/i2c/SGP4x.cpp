/**************************************************************************//**
 *
 * @author Doumanix <doumanix@gmx.de> Copyright (c) 2023
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <stdint.h>
#include <sblib/i2c/SGP4x.h>
#include <sblib/i2c.h>
#include <sblib/timer.h>
#include <sblib/serial.h>
#include <sblib/internal/iap.h>
#include <sblib/utils.h>
#include <sblib/math.h>
#include <sblib/bits.h>

// Specify the constants for water vapor and barometric pressure.
#define WATER_VAPOR 17.62f
#define BAROMETRIC_PRESSURE 243.5f

typedef enum {
	eSGP4xAddress = 0x59,  //SGP40 (VOC)
} HUM_SENSOR_T;

//VocAlgorithmParams voc_algorithm_params;
GasIndexAlgorithmParams voc_algorithm_params;

int16_t SGP4xClass::init(void)
{
	i2c_lpcopen_init();
	GasIndexAlgorithm_init(&voc_algorithm_params, GasIndexAlgorithm_ALGORITHM_TYPE_VOC);
    return executeConditioning();
}

int16_t SGP4xClass::executeConditioning()
{
    int16_t bytesRead;
    uint8_t readBuffer[3] = {};
    uint8_t cmd[8] =
    {
        highByte((uint16_t)Sgp4xCommand::selfConditioning),
        lowByte((uint16_t)Sgp4xCommand::selfConditioning),
        0x80,
        0x00,
        0xA2,
        0x66,
        0x66,
        0x93
    };

    if (Chip_I2C_MasterSend(I2C0, eSGP4xAddress, cmd, sizeof(cmd)/sizeof(*cmd)) == 0) {
		i2c_lpcopen_init();
		return -1;
    }
    delay(50);

    bytesRead = Chip_I2C_MasterRead(I2C0, eSGP4xAddress, readBuffer, sizeof(readBuffer)/sizeof(*readBuffer));
	if (bytesRead == 0) {
		i2c_lpcopen_init();
	}
	return bytesRead;
}


uint8_t SGP4xClass::executeSelfTest() {
//    uint8_t cmd[2] = {0x28, 0x0E};
    uint16_t cmd = reverseByteOrder((uint16_t)Sgp4xCommand::selfTest);
    int tstresult = 0 ;
    tstresult = Chip_I2C_MasterSend(I2C0, eSGP4xAddress, (uint8_t*)&cmd, 3);
        if( tstresult == 0){
			serial.println("SELF! Konnte nichts versenden!");
			i2c_lpcopen_init();
			return 0;
        }

    delay(350);

    uint8_t buffer[3] = {};
    uint8_t testResult = 0;
    tstresult = Chip_I2C_MasterRead(I2C0,eSGP4xAddress,buffer,3);
    	if( tstresult == 0){
			serial.println("SELF! Konnte nichts LESEN!");
			i2c_lpcopen_init();
			return 123;
    	}

    	testResult = 0;
//    	Datasheet: The most significant byte shall be ignored
//    	testResult = static_cast<uint16_t>(buffer[0]) << 8;
    	testResult |= static_cast<uint16_t>(buffer[1]);

    	return testResult;
}

/*
 * ToDo: NOx Rückgabe hinzufügen
 */
bool SGP4xClass::measureRawSignal(uint16_t relativeHumidity, uint16_t temperature, uint16_t& srawVoc)
{
    uint8_t buffer[6] = {};
    int result = 0 ;


    //DMX default temp / hum
    // nicht löschen!
//    uint8_t cmd[8] = {0x26, 0x19, 0x80, 0x00, 0xA2, 0x66, 0x66, 0x93};


    serial.println("CRC: " , (crc8((uint8_t*)&relativeHumidity, 2)));
    uint8_t cmd[8] =
    {
		((uint16_t)Sgp4xCommand::measureRaw >>  8) & 0xFF,
		((uint16_t)Sgp4xCommand::measureRaw >>  0) & 0xFF,

		(relativeHumidity >> 0) & 0xFF,
		(relativeHumidity >> 8) & 0xFF,
		((uint8_t)crc8((uint8_t*)&relativeHumidity, 2) >> 0) & 0xFF,
		(temperature >> 0) & 0xFF,
		(temperature >> 8) & 0xFF,
		((uint8_t)crc8((uint8_t*)&temperature, 2) >> 0) & 0xFF
    };

    // static command without temp / hum correction
    // 0x26, 0x19, 0x80, 0x00, 0xA2, 0x66, 0x66, 0x93

    result = Chip_I2C_MasterSend(I2C0, eSGP4xAddress, cmd, 8);
    if( result == 0){
		serial.println("ARGH! Konnte nichts versenden!");
		i2c_lpcopen_init();
		return 0;
    } else
    	serial.println("result ==== ",result);

    delay(50);

    result = Chip_I2C_MasterRead(I2C0,eSGP4xAddress,buffer,6);
	if( result == 0){
		serial.println("BRGH! Konnte nichts LESEN!");
		i2c_lpcopen_init();
		return 0;
	} else
		serial.println("LESE result ==== ",result);

	uint16_t vocTics;
	vocTics = static_cast<uint16_t>(buffer[0]) << 8;
	vocTics |= static_cast<uint16_t>(buffer[1]);
	uint16_t nox_tics;
	nox_tics = static_cast<uint16_t>(buffer[3]) << 8;
	nox_tics |= static_cast<uint16_t>(buffer[4]);

	srawVoc = vocTics;

	int32_t vocRawValue = static_cast<uint32_t>(vocTics); // read from sensor in step 2-3
	int32_t vocIndexValue;
	GasIndexAlgorithm_process(&voc_algorithm_params,vocRawValue , &vocIndexValue);
	serial.println("VOC: ", (float)vocIndexValue);

	return true;
}



/**********************************************************
 * 	getSerialnumber
 *	Read the sensor's unique serial number
 *
 * 	@return uint16_t
 **********************************************************/
uint64_t SGP4xClass::getSerialnumber(void)
{
	uint8_t buffer[9] = {};

    if (!readSensor(Sgp4xCommand::getSerial, buffer, sizeof(buffer) / sizeof(*buffer)))
    {
        return false;
    }

    uint64_t sgp4Serial;
    sgp4Serial = static_cast<uint64_t>(buffer[0]) << 40;
    sgp4Serial |= static_cast<uint64_t>(buffer[1]) << 32;
    sgp4Serial |= static_cast<uint64_t>(buffer[3]) << 24;
    sgp4Serial |= static_cast<uint64_t>(buffer[4]) << 16;
    sgp4Serial |= static_cast<uint64_t>(buffer[6]) << 8;
    sgp4Serial |= static_cast<uint64_t>(buffer[7]);

    //sblib cannot print 64 bit value to serial console
    // lossy cast to int -  hoping its big enough
    // just for test serial output
    serial.println("SerialNumber:", (int)sgp4Serial);

	uint16_t serialnr[3] = {};
	serialnr[0] = buffer[0] << 8;
	serialnr[0] |= buffer[1];

	serialnr[1] = buffer[3] << 8;
	serialnr[1] |= buffer[4];

	serialnr[2] = buffer[6] << 8;
	serialnr[2] |= buffer[7];

	serial.print("HEX SerialNr:");
	serial.print("0x", serialnr[0], HEX, 2);
	serial.print(" 0x", serialnr[1], HEX, 2);
	serial.println(" 0x", serialnr[2], HEX, 2);

	return sgp4Serial;
}


/******************************************************************************
 * Private Functions
 ******************************************************************************/


bool SGP4xClass::readSensor(Sgp4xCommand command, uint8_t* buffer, uint8_t bufferLength)
{
// switch byte order since the commands given are big endian and LPC1115 is little endian
  uint16_t revCmd = reverseByteOrder((uint16_t)command);
  serial.println("revCmd = 0x", revCmd, HEX, 2);
  int resultLength = Chip_I2C_MasterWriteRead(I2C0, eSGP4xAddress, (uint8_t*)&revCmd, buffer, 2, 9);

  serial.println("readSensor Ergebnislänge: ", resultLength);
  return true;
}



uint8_t SGP4xClass::crc8(const uint8_t *data, int len) {
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

//SGP4xClass SGP4x;
