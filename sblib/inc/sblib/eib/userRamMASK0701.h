/*
 * userRam.h
 *
 *  Created on: 18.11.2021
 *      Author: dridders
 */

#ifndef SBLIB_EIB_USERRAM_MASK0701_H_
#define SBLIB_EIB_USERRAM_MASK0701_H_

#include <stdint.h>
#include <sblib/eib/userRamBCU2.h>

/**
 * The mask version 0x0701 user RAM
 */
class UserRamMASK0701 : public UserRamBCU2
{
public:
	UserRamMASK0701() : UserRamBCU2(0x5FC, 0x304, 3) {}

protected:
	UserRamMASK0701(uint32_t start, uint32_t size, uint32_t shadowSize) : UserRamBCU2(start, size, shadowSize) {}
private:

};

#endif /* SBLIB_EIB_USERRAM_MASK0701_H_ */
