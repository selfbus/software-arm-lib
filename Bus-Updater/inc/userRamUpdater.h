/*
 * userRamUpdater.h
 *
 */

#ifndef SBLIB_EIB_USERRAM_UPDATER_H_
#define SBLIB_EIB_USERRAM_UPDATER_H_

#include <sblib/eib/userRam.h>

/**
 * The bootloader specific user RAM
 */
class UserRamUpdater : public UserRam
{
public:
    UserRamUpdater() : UserRam(0, 0x3, 0) {}

	static const int deviceControlOffset = 0x1;
	static const int peiTypeOffset = 0x1;
	// static const int runStateOffset = 0x61;
	// static const int user2Offset = 0xC8;

    virtual byte& deviceControl() const { return userRamData[deviceControlOffset]; }
    virtual byte& peiType() const { return userRamData[peiTypeOffset]; }
protected:
	UserRamUpdater(unsigned int start, unsigned int size, unsigned int shadowSize) : UserRam(start, size, shadowSize) {}
};

#endif /* SBLIB_EIB_USERRAM_UPDATER_H_ */
