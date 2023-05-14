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

SGP4xResult SGP4xClass::readSensor(uint8_t * commandBuffer, uint8_t commandBufferSize,
        uint8_t * readBuffer, uint8_t readBufferSize, uint16_t processDelayMs)
{
    if ((commandBuffer == nullptr) || (commandBufferSize == 0))
    {
        return SGP4xResult::invalidCommandBuffer;
    }

    int32_t bytesProcessed = Chip_I2C_MasterSend(I2C0, eSGP4xAddress, commandBuffer, commandBufferSize);
    if (bytesProcessed != commandBufferSize) {
        i2c_lpcopen_init();
        return SGP4xResult::sendError;
    }

    if (processDelayMs > 0) {
        delay(processDelayMs);
    }

    if ((readBuffer == nullptr) || (readBufferSize == 0))
    {
        // command expects no response
        return SGP4xResult::success;
    }

    bytesProcessed = Chip_I2C_MasterRead(I2C0, eSGP4xAddress, readBuffer, readBufferSize);
    if (bytesProcessed != readBufferSize) {
        i2c_lpcopen_init();
        return SGP4xResult::readError;
    }

    // crc8 checksum is transmitted after every word (2 bytes)
    // including the checksum we have to receive a multiple of 3 bytes
    if ((bytesProcessed == 0) || ((bytesProcessed % 3) != 0))
    {
        return SGP4xResult::invalidByteCount;
    }

    for (uint8_t i = 0; i < bytesProcessed; i = i + 3)
    {
        if (crc8(&readBuffer[i], 2) != readBuffer[i+2])
        {
            return SGP4xResult::crc8Mismatch;
        }
    }

    return SGP4xResult::success;
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

    // max. duration for processing sgp41_execute_conditioning is 50ms
    SGP4xResult result = readSensor(cmd, commandBufferSize, readBuffer, readBufferSize, 50);
    return result;
}


SGP4xResult SGP4xClass::executeSelfTest() {
    uint8_t cmd[2] = {
            highByte((uint16_t)Sgp4xCommand::selfTest),
            lowByte((uint16_t)Sgp4xCommand::selfTest),
    };
    uint8_t commandBufferSize = sizeof(cmd)/sizeof(*cmd);

    uint8_t readBuffer[3] = {};
    uint8_t readBufferSize = sizeof(readBuffer)/sizeof(*readBuffer);

    // max. duration for processing sgp41_execute_self_test is 320ms (we add a margin of 30ms to be on the safe side)
    SGP4xResult result = readSensor(cmd, commandBufferSize, readBuffer, readBufferSize, 350);
    if (result != SGP4xResult::success)
    {
        return result;
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

SGP4xResult SGP4xClass::measureRawSignal(uint16_t relativeHumidityTicks, uint16_t temperatureTicks, bool useCompensation)
{
    uint8_t readBuffer[6] = {};
    uint8_t readBufferSize = sizeof(readBuffer)/sizeof(*readBuffer);
    // default static command without temperature/humidity correction
    // 0x26, 0x19, 0x80, 0x00, 0xA2, 0x66, 0x66, 0x93
    uint8_t cmd[8] = {
            highByte((uint16_t)Sgp4xCommand::measureRaw),
            lowByte((uint16_t)Sgp4xCommand::measureRaw),
            0x80, 0x00, 0xA2, 0x66, 0x66, 0x93};
    uint8_t commandBufferSize = sizeof(cmd)/sizeof(*cmd);

    if (useCompensation)
    {
        // set provided relative humidity and temperature in command parameters
        cmd[2] = lowByte(relativeHumidityTicks);
        cmd[3] = highByte(relativeHumidityTicks);
        cmd[4] = crc8((uint8_t*)&relativeHumidityTicks, sizeof(relativeHumidityTicks));
        cmd[5] = lowByte(temperatureTicks);
        cmd[6] = highByte(temperatureTicks);
        cmd[7] = crc8((uint8_t*)&temperatureTicks, sizeof(temperatureTicks));
    }

    // max. duration for processing sgp41_measure_raw_signals is 50ms
    SGP4xResult result = readSensor(cmd, commandBufferSize, readBuffer, readBufferSize, 50);
    if (result != SGP4xResult::success)
    {
        return result;
    }

	rawVocTics = makeWord(readBuffer[1], readBuffer[0]);
	GasIndexAlgorithm_process(&voc_algorithm_params, rawVocTics , &vocIndexValue);

	rawNoxTics = makeWord(readBuffer[4], readBuffer[3]);
	GasIndexAlgorithm_process(&nox_algorithm_params, rawNoxTics, &noxIndexValue);

	return SGP4xResult::success;
}

SGP4xResult SGP4xClass::measureRawSignal()
{
    return measureRawSignal(0, 0, false);
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

    // max. duration for processing sgp4x_get_serial_number is 1 second
    SGP4xResult result = readSensor(cmd, commandBufferSize, readBuffer, readBufferSize, 1000);
    if (result != SGP4xResult::success)
    {
        return result;
    }

    *serialNumber = static_cast<uint64_t>(readBuffer[0]) << 40;
    *serialNumber |= static_cast<uint64_t>(readBuffer[1]) << 32;
    *serialNumber |= static_cast<uint64_t>(readBuffer[3]) << 24;
    *serialNumber |= static_cast<uint64_t>(readBuffer[4]) << 16;
    *serialNumber |= static_cast<uint64_t>(readBuffer[6]) << 8;
    *serialNumber |= static_cast<uint64_t>(readBuffer[7]);
	return SGP4xResult::success;
}

SGP4xResult SGP4xClass::turnHeaterOffAndReturnToIdle()
{
    uint8_t cmd[2] = {
            highByte((uint16_t)Sgp4xCommand::heaterOff),
            lowByte((uint16_t)Sgp4xCommand::heaterOff),
    };
    uint8_t commandBufferSize = sizeof(cmd)/sizeof(*cmd);

    // max. duration for processing sgp4x_turn_heater_off is 1 second
    SGP4xResult result = readSensor(cmd, commandBufferSize, nullptr, 0, 1000);
    return result;
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
