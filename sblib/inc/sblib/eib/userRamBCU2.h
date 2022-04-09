/*
 * userRam.h
 *
 *  Created on: 18.11.2021
 *      Author: dridders
 */

#ifndef SBLIB_EIB_USERRAM_BCU2_H_
#define SBLIB_EIB_USERRAM_BCU2_H_

#include <sblib/eib/userRamBCU1.h>

/**
 * The user RAM.
 *
 * The user RAM can be accessed by name, like userRam.status and as an array, like
 * userRam[addr]. Please note that the start address of the RAM is subtracted.
 */
class UserRamBCU2 : public UserRamBCU1
{
public:
	UserRamBCU2() : UserRamBCU1(0, 0x100, 3) {}; ///\todo BUG? Check this, originally shadowSize was 0 but i think total size of class UserRam is 258 bytes and not 256 like before with 0

protected:
	UserRamBCU2(unsigned int start, unsigned int size, unsigned int shadowSize) : UserRamBCU1(start, size, shadowSize) {}
};

#endif /* SBLIB_EIB_USERRAM_BCU2_H_ */
