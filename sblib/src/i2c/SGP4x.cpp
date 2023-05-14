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
#include <sblib/bits.h>

typedef enum {
    eSGP4xAddress = 0x59,  // SGP40 (VOC) and SGP41 (VOC & NOx)
} HUM_SENSOR_T;

SGP4xClass::SGP4xClass():
        rawVocTics(0),
        rawNoxTics(0),
        vocIndexValue(-1),
        noxIndexValue(-1)
{
    // init VOC index algorithm with default sampling interval
    GasIndexAlgorithm_init_with_sampling_interval(&voc_algorithm_params, GasIndexAlgorithm_ALGORITHM_TYPE_VOC, GasIndexAlgorithm_DEFAULT_SAMPLING_INTERVAL);
    // init NOx index algorithm with default sampling interval
    GasIndexAlgorithm_init_with_sampling_interval(&nox_algorithm_params, GasIndexAlgorithm_ALGORITHM_TYPE_NOX, GasIndexAlgorithm_DEFAULT_SAMPLING_INTERVAL);
}

SGP4xResult SGP4xClass::init(uint32_t samplingIntervalMs)
{
	i2c_lpcopen_init();
	rawVocTics = 0;
	rawNoxTics = 0;
	vocIndexValue = -1;
	noxIndexValue = -1;
	// init VOC index algorithm with provided sampling interval
	GasIndexAlgorithm_init_with_sampling_interval(&voc_algorithm_params, GasIndexAlgorithm_ALGORITHM_TYPE_VOC, (float)(samplingIntervalMs/1000.f));
	// init NOx index algorithm with provided sampling interval
	GasIndexAlgorithm_init_with_sampling_interval(&nox_algorithm_params, GasIndexAlgorithm_ALGORITHM_TYPE_NOX, (float)(samplingIntervalMs/1000.f));
    return executeConditioning();
}

SGP4xResult SGP4xClass::executeConditioning()
{
    uint8_t readBuffer[3] = {};
    uint8_t readBufferSize = sizeof(readBuffer)/sizeof(*readBuffer);
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
    uint8_t commandBufferSize = sizeof(cmd)/sizeof(*cmd);

    int32_t bytesProcessed = Chip_I2C_MasterSend(I2C0, eSGP4xAddress, cmd, commandBufferSize);
    if (bytesProcessed != commandBufferSize) {
		i2c_lpcopen_init();
		return SGP4xResult::sendError;
    }
    delay(50);

    bytesProcessed = Chip_I2C_MasterRead(I2C0, eSGP4xAddress, readBuffer, readBufferSize);
	if (bytesProcessed != readBufferSize) {
		i2c_lpcopen_init();
		return SGP4xResult::readError;
	}

	if (crc8(readBuffer, 2) != readBuffer[readBufferSize - 1]) {
	    i2c_lpcopen_init();
	    return SGP4xResult::crc8Mismatch;
	}
	return SGP4xResult::success;
}


SGP4xResult SGP4xClass::executeSelfTest() {
    uint8_t cmd[2] = {
            highByte((uint16_t)Sgp4xCommand::selfTest),
            lowByte((uint16_t)Sgp4xCommand::selfTest),
    };
    uint8_t commandBufferSize = sizeof(cmd)/sizeof(*cmd);

    int32_t bytesProcessed = Chip_I2C_MasterSend(I2C0, eSGP4xAddress, cmd, commandBufferSize);
    if (bytesProcessed != commandBufferSize)
    {
        i2c_lpcopen_init();
        return SGP4xResult::sendError;
    }

    delay(350);

    uint8_t readBuffer[3] = {};
    uint8_t readBufferSize = sizeof(readBuffer)/sizeof(*readBuffer);

    bytesProcessed = Chip_I2C_MasterRead(I2C0, eSGP4xAddress, readBuffer, readBufferSize);
    if (bytesProcessed != readBufferSize) {
        i2c_lpcopen_init();
        return SGP4xResult::readError;
    }

    if (crc8(readBuffer, 2) != readBuffer[readBufferSize - 1]) {
        i2c_lpcopen_init();
        return SGP4xResult::crc8Mismatch;
    }

    // Datasheet:
    // The most significant byte shall be ignored and check only nibble of least significant byte
    // ignore bits 2,3

    // bit 0 set -> VOC pixel error
    if ((readBuffer[1] & 0x1) != 0) {
        return SGP4xResult::vocPixelError;
    }

    // bit 1 set -> NOx pixel error
    if ((readBuffer[1] & 0x2) != 0) {
        return SGP4xResult::noxPixelError;
    }

    return SGP4xResult::success;
}

SGP4xResult SGP4xClass::measureRawSignal(uint16_t relativeHumidity, uint16_t temperature)
{
    uint8_t readBuffer[6] = {};
    uint8_t readBufferSize = sizeof(readBuffer)/sizeof(*readBuffer);
    // static command without temperature/humidity correction
    // 0x26, 0x19, 0x80, 0x00, 0xA2, 0x66, 0x66, 0x93
    uint8_t cmd[8] = {
            highByte((uint16_t)Sgp4xCommand::measureRaw),
            lowByte((uint16_t)Sgp4xCommand::measureRaw),
            0x80, 0x00, 0xA2, 0x66, 0x66, 0x93};
    uint8_t commandBufferSize = sizeof(cmd)/sizeof(*cmd);

    cmd[2] = lowByte(relativeHumidity);
    cmd[3] = highByte(relativeHumidity);
    cmd[4] = crc8((uint8_t*)&relativeHumidity, sizeof(relativeHumidity));
    cmd[5] = lowByte(temperature);
    cmd[6] = highByte(temperature);
    cmd[7] = crc8((uint8_t*)&temperature, sizeof(temperature));

    int32_t bytesProcessed = Chip_I2C_MasterSend(I2C0, eSGP4xAddress, cmd, commandBufferSize);
    if (bytesProcessed != commandBufferSize) {
		i2c_lpcopen_init();
		return SGP4xResult::sendError;
    }

    delay(50);

    bytesProcessed = Chip_I2C_MasterRead(I2C0, eSGP4xAddress, readBuffer, readBufferSize);
	if (bytesProcessed != readBufferSize) {
		i2c_lpcopen_init();
		return SGP4xResult::readError;
	}

	uint16_t vocTics = makeWord(readBuffer[0], readBuffer[1]);
	if (crc8((uint8_t*)&vocTics, sizeof(vocTics)) != readBuffer[2]) {
	    return SGP4xResult::crc8Mismatch;
	}
	rawVocTics = vocTics;
	GasIndexAlgorithm_process(&voc_algorithm_params, rawVocTics , &vocIndexValue);

	uint16_t noxTics = makeWord(readBuffer[3], readBuffer[4]);
	if (crc8((uint8_t*)&noxTics, sizeof(noxTics)) != readBuffer[5]) {
	    return SGP4xResult::crc8Mismatch;
	}
	rawNoxTics = noxTics;
	GasIndexAlgorithm_process(&nox_algorithm_params, rawNoxTics, &noxIndexValue);
	return SGP4xResult::success;
}

SGP4xResult SGP4xClass::getSerialnumber(uint64_t * serialNumber)
{
	uint8_t readBuffer[9] = {};
	uint8_t readBufferSize = sizeof(readBuffer)/sizeof(*readBuffer);
    uint8_t cmd[2] = {
            highByte((uint16_t)Sgp4xCommand::getSerial),
            lowByte((uint16_t)Sgp4xCommand::getSerial),
    };
    uint8_t commandBufferSize = sizeof(cmd)/sizeof(*cmd);

    int32_t bytesProcessed = Chip_I2C_MasterSend(I2C0, eSGP4xAddress, cmd, commandBufferSize);
    if (bytesProcessed != commandBufferSize)
    {
        i2c_lpcopen_init();
        return SGP4xResult::sendError;
    }

    delay(1000); // max. duration for processing sgp4x_get_serial_number is 1 second

    bytesProcessed = Chip_I2C_MasterRead(I2C0, eSGP4xAddress, readBuffer, readBufferSize);
    if (bytesProcessed != readBufferSize) {
        i2c_lpcopen_init();
        return SGP4xResult::readError;
    }

    *serialNumber = static_cast<uint64_t>(readBuffer[0]) << 40;
    *serialNumber |= static_cast<uint64_t>(readBuffer[1]) << 32;
    *serialNumber |= static_cast<uint64_t>(readBuffer[3]) << 24;
    *serialNumber |= static_cast<uint64_t>(readBuffer[4]) << 16;
    *serialNumber |= static_cast<uint64_t>(readBuffer[6]) << 8;
    *serialNumber |= static_cast<uint64_t>(readBuffer[7]);
	return SGP4xResult::success;
}

int32_t SGP4xClass::getVocIndexValue()
{
    return vocIndexValue;
}

int32_t SGP4xClass::getNoxIndexValue()
{
    return noxIndexValue;
}

int32_t SGP4xClass::getRawVocValue()
{
    return rawVocTics;
}

int32_t SGP4xClass::getRawNoxValue()
{
    return rawNoxTics;
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
