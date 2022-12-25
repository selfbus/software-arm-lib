#ifndef sblib_usereeprom_mask0701_h
#define sblib_usereeprom_mask0701_h

#include <sblib/eib/userEepromBCU2.h>

/**
 * The BCU BIM112 MaskVersion 0x0701 user EEPROM
 * @details Can be accessed by name, like userEeprom.manuDataH() and as an array, like
 *          userEeprom[addr]. Please note that the @ref startAddress is subtracted.
 *          E.g. userEeprom[0x107] is the correct address for userEeprom.version() not userEeprom[0x07].
 *
 * @note see KNX Spec. 2.1
 *       - BIM112            : not in Spec. 2.1 some information in 06 Profiles 4.2.10 p.36
 */
class UserEepromMASK0701 : public UserEepromBCU2
{
public:
    ///\todo make start at 0x4000, right now 0x4000-0x100= 0x3f00 is chosen to avoid address-offset calculations to a BCU1
    UserEepromMASK0701() : UserEepromBCU2(0x3f00, 3072, 4096) {};
	~UserEepromMASK0701() = default;

protected:
	UserEepromMASK0701(unsigned int start, unsigned int size, unsigned int flashSize) : UserEepromBCU2(start, size, flashSize) {};
};

#endif /*sblib_usereeprom_mask0701_h*/
