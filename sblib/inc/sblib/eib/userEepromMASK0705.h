#ifndef sblib_usereeprom_mask0705_h
#define sblib_usereeprom_mask0705_h

#include <sblib/eib/userEepromMASK0701.h>

/**
 * The BCU BIM112 MaskVersion 0x0705 user EEPROM
 */
class UserEepromMASK0705 : public UserEepromMASK0701
{
public:
    ///\todo make start at 0x4000, right now 0x4000-0x100= 0x3f00 is chosen to avoid address-offset calculations to a BCU1
	UserEepromMASK0705() : UserEepromMASK0701(0x3f00, 3072, 4096) {};
	~UserEepromMASK0705() = default;

protected:
	UserEepromMASK0705(unsigned int start, unsigned int size, unsigned int flashSize) : UserEepromMASK0701(start, size, flashSize) {};
};

#endif /*sblib_usereeprom_mask0705_h*/
