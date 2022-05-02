#ifndef sblib_usereeprom_mask0701_h
#define sblib_usereeprom_mask0701_h

#include <sblib/eib/userEepromBCU2.h>

/**
 * The BCU BIM112 MaskVersion 0x0701 user EEPROM
 */
class UserEepromMASK0701 : public UserEepromBCU2
{
public:
    UserEepromMASK0701() = delete;
    ///\todo make start at 0x4000, right now 0x4000-0x100= 0x3f00 is chosen to avoid address-offset calculations to a BCU1
    UserEepromMASK0701(BcuBase* bcu) : UserEepromBCU2(bcu, 0x3f00, 3072, 4096) {};
	~UserEepromMASK0701() = default;

protected:
	UserEepromMASK0701(BcuBase* bcu, unsigned int start, unsigned int size, unsigned int flashSize) : UserEepromBCU2(bcu, start, size, flashSize) {};
};

#endif /*sblib_usereeprom_mask0701_h*/
