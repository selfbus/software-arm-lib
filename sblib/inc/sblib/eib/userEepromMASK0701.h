#ifndef sblib_usereeprom_mask0701_h
#define sblib_usereeprom_mask0701_h

#include <sblib/eib/userEepromBCU2.h>

/**
 * The user EEPROM.
 *
 * The user EEPROM can be accessed by name, like userEeprom.status and as an array, like
 * userEeprom[addr]. Please note that the start address of the EEPROM is subtracted. That means
 * userEeprom[0x107] is the correct address for userEeprom.version; not userEeprom[0x07].
 */
class UserEepromMASK0701 : public UserEepromBCU2
{
public:
	UserEepromMASK0701(BcuBase* bcu) : UserEepromBCU2(bcu, 0x3f00, 3072, 4096) {};
	~UserEepromMASK0701() = default;

protected:
	UserEepromMASK0701(BcuBase* bcu, unsigned int start, unsigned int size, unsigned int flashSize) : UserEepromBCU2(bcu, start, size, flashSize) {};
};

#endif
