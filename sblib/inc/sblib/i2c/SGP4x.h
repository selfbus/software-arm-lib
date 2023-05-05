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


#ifndef SGP4X_H
#define SGP4X_H

//#include <stdint.h>
//#include <sblib/i2c/sensirion_voc_algorithm.h>
//#include <sblib/i2c/sensirion_arch_config.h>
#include <sblib/i2c/sensirion_gas_index_algorithm.h>

//VocAlgorithmParams voc_algorithm_params;


enum class Sgp4xCommand : uint16_t ;
//{
//    getSerial 	= 0x3682,  // 0x3682
//    measureRaw 	= 0x260F,
//    heaterOff 	= 0x3615,
//	selfTest	= 0x280E
//};

class SGP4xClass
{
private:
  bool readSensor(Sgp4xCommand command, uint8_t* buffer, uint8_t bufferLength);

public:
  void init(void);

  /**
   * executeSelfTest() - This command triggers the built-in self-test checking
   * for integrity of both hotplate and MOX material and returns the result of
   * this test as 2 bytes
   *
   * @param testResult 0xXX 0xYY: ignore most significant byte 0xXX. The four
   * least significant bits of the least significant byte 0xYY provide
   * information if the self-test has or has not passed for each individual
   * pixel. All zero mean all tests passed successfully. Check the datasheet
   * for more detailed information.
   *
   *
   */
  uint8_t executeSelfTest();

  bool measureRawSignal(uint16_t, uint16_t, uint16_t&);

  uint16_t executeConditioning();

  void readVocValue();

  uint64_t getSerialnumber(void);

  /**
   * Do a CRC validation of result
   *
   * @param data  Pointer to the data to use when calculating the CRC8.
   * @param len   The number of bytes in 'data'.
   *
   * @return CRC result value
   */
  uint8_t crc8(const uint8_t* data, int len);
};

#endif
