/*
 * userRam.h
 *
 *  Created on: 18.11.2021
 *      Author: dridders
 */

#ifndef SBLIB_EIB_USERRAM_MASK0705_H_
#define SBLIB_EIB_USERRAM_MASK0705_H_

#include <sblib/eib/userRam.h>

/**
 * The user RAM.
 *
 * The user RAM can be accessed by name, like userRam.status and as an array, like
 * userRam[addr]. Please note that the start address of the RAM is subtracted.
 */
class UserRamMASK0705 : public UserRamMASK0701
{
public:
	UserRamMASK0705() : UserRamMASK0701(0, 0x304) {}

protected:
	UserRamMASK0705(int start, int size) : UserRamMASK0701(start, size) {}
};

#endif /* SBLIB_EIB_USERRAM_H_ */
