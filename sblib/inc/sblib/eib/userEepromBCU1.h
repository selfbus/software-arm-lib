#ifndef sblib_usereeprom_bcu1_h
#define sblib_usereeprom_bcu1_h

#include <sblib/eib/userEeprom.h>

/**
 * The user EEPROM.
 *
 * The user EEPROM can be accessed by name, like userEeprom.status and as an array, like
 * userEeprom[addr]. Please note that the start address of the EEPROM is subtracted. That means
 * userEeprom[0x107] is the correct address for userEeprom.version; not userEeprom[0x07].
 */
class UserEepromBCU1 : public UserEeprom
{
public:
	UserEepromBCU1(BcuBase *bcu) : UserEeprom(bcu, 0x100, 156, 156) {};
	virtual ~UserEepromBCU1() = default;

	static const int manufacturerHOffset = 3;
	static const int manufacturerLOffset = 4;
	static const int deviceTypeHOffset = 5;
	static const int deviceTypeLOffset = 6;
	static const int versionOffset = 7;
	static const int appPeiTypeOffset = 9;
	static const int portADDROffset = 12;
	static const int runErrorOffset = 13;
	static const int assocTabPtrOffset = 17;
	static const int commsTabPtrOffset = 18;
	static const int addrTabSizeOffset = 22;
	static const int addrTabOffset = 23;

	virtual byte& manufacturerH() const { return userEepromData[manufacturerHOffset]; }
	virtual byte& manufacturerL() const { return userEepromData[manufacturerLOffset]; }
	virtual byte& deviceTypeH() const { return userEepromData[deviceTypeHOffset]; }
	virtual byte& deviceTypeL() const { return userEepromData[deviceTypeLOffset]; }
	virtual byte& version() const { return userEepromData[versionOffset]; }
	virtual byte& appPeiType() const { return userEepromData[appPeiTypeOffset]; }
	virtual byte& portADDR() const { return userEepromData[portADDROffset]; }
	virtual byte& runError() const { return userEepromData[runErrorOffset]; }
    virtual byte& addrTabSize() const { return userEepromData[addrTabSizeOffset]; }
    virtual byte& assocTabPtr() const { return userEepromData[assocTabPtrOffset]; }
    virtual byte* addrTab() const { return &userEepromData[addrTabOffset]; }
    virtual byte& commsTabPtr() const { return userEepromData[commsTabPtrOffset]; };

protected:
	UserEepromBCU1(BcuBase *bcu, int start, int size, int flashSize) : UserEeprom(bcu, start, size, flashSize) {};
};

#endif
