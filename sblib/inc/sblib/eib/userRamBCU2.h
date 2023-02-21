/*
 * userRam.h
 *
 *  Created on: 18.11.2021
 *      Author: dridders
 */

#ifndef SBLIB_EIB_USERRAM_BCU2_H_
#define SBLIB_EIB_USERRAM_BCU2_H_

#include <stdint.h>
#include <sblib/eib/userRamBCU1.h>

/**
 * The BCU 2 user RAM
 */
class UserRamBCU2 : public UserRamBCU1
{
public:
    ///\todo BUG? Check this, originally (pre OOP) shadowSize was 0 but i think total size of class UserRam is 258 bytes and not 256 like before with 0
    UserRamBCU2() : UserRamBCU1(0, 0x100, 3) {};

protected:
	UserRamBCU2(uint32_t start, uint32_t size, uint32_t shadowSize) : UserRamBCU1(start, size, shadowSize) {}
};

#endif /* SBLIB_EIB_USERRAM_BCU2_H_ */
