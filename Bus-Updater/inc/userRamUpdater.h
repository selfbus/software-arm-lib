/*
 * userRamUpdater.h
 *
 */

#ifndef SBLIB_EIB_USERRAM_UPDATER_H_
#define SBLIB_EIB_USERRAM_UPDATER_H_

#include <stdint.h>
#include <sblib/eib/userRam.h>

/**
 * The bootloader specific user RAM
 */
class UserRamUpdater final : public UserRam // don't derive from UserRamUpdater, it will be deleted after class Bus is fixed
{
public:
    UserRamUpdater() : UserRam(0, 0x2, 0) {}

    uint8_t& deviceControl() const override { return userRamData[_fakeByteOffset]; }
    uint8_t& peiType() const override { return userRamData[_fakeByteOffset]; }

protected:
	UserRamUpdater(unsigned int start, unsigned int size, unsigned int shadowSize) : UserRam(start, size, shadowSize) {}
    uint32_t statusOffset() const override {return _statusOffset;}
    uint32_t runStateOffset() const override {return _fakeByteOffset;}

    ///\todo until class Bus checks the userRam->status() we need at least this info in byte zero of @ref userRamData
    static const uint32_t _statusOffset = 0x0;

    // all other we just fake to the second byte of userRamData
    static const uint32_t _fakeByteOffset = 0x1;
};

#endif /* SBLIB_EIB_USERRAM_UPDATER_H_ */
