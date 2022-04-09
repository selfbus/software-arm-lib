/*
 * userRam.h
 *
 *  Created on: 18.11.2021
 *      Author: dridders
 */

#ifndef SBLIB_EIB_USERRAM_MASK0705_H_
#define SBLIB_EIB_USERRAM_MASK0705_H_

#include <sblib/eib/userRamMASK0701.h>

/**
 * The user RAM.
 *
 * The user RAM can be accessed by name, like userRam.status and as an array, like
 * userRam[addr]. Please note that the start address of the RAM is subtracted.
 */
class UserRamMASK0705 : public UserRamMASK0701
{
public:
	UserRamMASK0705() : UserRamMASK0701(0x5FC, 0x304, 3) {}

protected:
	UserRamMASK0705(unsigned int start, unsigned int size, unsigned int shadowSize) : UserRamMASK0701(start, size, shadowSize) {}
};

#endif /* SBLIB_EIB_USERRAM_MASK0705_H_ */
