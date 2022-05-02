/*
 * userRam.h
 *
 *  Created on: 18.11.2021
 *      Author: dridders
 */

#ifndef SBLIB_EIB_USERRAM_MASK0705_H_
#define SBLIB_EIB_USERRAM_MASK0705_H_

#include <stdint.h>
#include <sblib/eib/userRamMASK0701.h>

/**
 * The mask version 0x0705 user RAM
 */
class UserRamMASK0705 : public UserRamMASK0701
{
public:
	UserRamMASK0705() : UserRamMASK0701(0x5FC, 0x304, 3) {}

protected:
	UserRamMASK0705(uint32_t start, uint32_t size, uint32_t shadowSize) : UserRamMASK0701(start, size, shadowSize) {}

private:
};

#endif /* SBLIB_EIB_USERRAM_MASK0705_H_ */
