#ifndef sblib_usereeprom_systemb_h
#define sblib_usereeprom_systemb_h

#include <sblib/eib/userEeprom.h>

/**
 * The user EEPROM.
 *
 * The user EEPROM can be accessed by name, like userEeprom.status and as an array, like
 * userEeprom[addr]. Please note that the start address of the EEPROM is subtracted. That means
 * userEeprom[0x107] is the correct address for userEeprom.version; not userEeprom[0x07].
 */
class UserEepromSYSTEMB : public UserEepromBCU2
{
public:
	UserEepromSYSTEMB(BcuBase* bcu) : UserEepromBCU2(bcu, 0x3300, 3072, 4096) {};
	virtual ~UserEepromSYSTEMB() = default;

    virtual int userEepromFlashSize() const;

	static const int addrTabAddrOffset = 33;
	static const int assocTabAddrOffset = 35;
	static const int addrTabMcbOffset = 77;
	static const int assocTabMcbOffset = 85;
	static const int commsTabMcbOffset = 93;
	static const int commsSeg0McbOffset = 109;
	static const int eibObjMcbOffset = 101;

	virtual word& addrTabAddr() const { return *(word*)&userEepromData[addrTabAddrOffset]; }
	virtual word& assocTabAddr() const { return *(word*)&userEepromData[assocTabAddrOffset]; }
	virtual byte* addrTabMcb() const { return &userEepromData[addrTabMcbOffset]; }
	virtual byte* assocTabMcb() const { return &userEepromData[assocTabMcbOffset]; }
	virtual byte* commsTabMcb() const { return &userEepromData[commsTabMcbOffset]; }
	virtual byte* commsSeg0Mcb() const { return &userEepromData[commsSeg0McbOffset]; }
	virtual byte* eibObjMcb() const { return &userEepromData[eibObjMcbOffset]; }

protected:
	UserEepromSYSTEMB(BcuBase* bcu, int start, int size, int flashSize) : UserEepromBCU2(bcu, start, size, flashSize) {};
};

#endif
