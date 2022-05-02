/*
 * userRam.h
 *
 *  Created on: 18.11.2021
 *      Author: dridders
 */

#ifndef SBLIB_EIB_USERRAM_SYSTEMB_H_
#define SBLIB_EIB_USERRAM_SYSTEMB_H_

#include <stdint.h>
#include <sblib/eib/userRamMASK0701.h>

/**
 * The SYSTEM B user RAM
 */
class UserRamSYSTEMB : public UserRamMASK0701
{
public:
	UserRamSYSTEMB() : UserRamMASK0701(0x5FC, 0x304, 3) {}

protected:
	UserRamSYSTEMB(uint32_t start, uint32_t size, uint32_t shadowSize) : UserRamMASK0701(start, size, shadowSize) {}

private:

};

#endif /* SBLIB_EIB_USERRAM_SYSTEMB_H_ */
