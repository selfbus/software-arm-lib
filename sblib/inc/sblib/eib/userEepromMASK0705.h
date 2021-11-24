#ifndef sblib_usereeprom_mask0705_h
#define sblib_usereeprom_mask0705_h

#include <sblib/eib/userEepromMASK0701.h>

/**
 * The user EEPROM.
 *
 * The user EEPROM can be accessed by name, like userEeprom.status and as an array, like
 * userEeprom[addr]. Please note that the start address of the EEPROM is subtracted. That means
 * userEeprom[0x107] is the correct address for userEeprom.version; not userEeprom[0x07].
 */
class UserEepromMASK0705 : public UserEepromMASK0701
{
public:
	UserEepromMASK0705(Bus* bus) : UserEepromMASK0701(bus, 0x3f00, 3072) {};
	virtual ~UserEepromMASK0705() = default;

protected:
	UserEepromMASK0705(Bus* bus, int start, int size) : UserEepromMASK0701(bus, start, size) {};
};

#endif
