#ifndef sblib_usereeprom_bcu2_h
#define sblib_usereeprom_bcu2_h

#include <sblib/eib/userEeprom.h>

/**
 * The user EEPROM.
 *
 * The user EEPROM can be accessed by name, like userEeprom.status and as an array, like
 * userEeprom[addr]. Please note that the start address of the EEPROM is subtracted. That means
 * userEeprom[0x107] is the correct address for userEeprom.version; not userEeprom[0x07].
 */
class UserEepromBCU2 : public UserEepromBCU1
{
public:
	UserEepromBCU2(BcuBase* bcu) : UserEepromBCU1(bcu, 0x100, 1024, 1024) {};
	~UserEepromBCU2() = default;

	static const int appTypeOffset = 21;
	static const int loadStateOffset = 880;
	static const int addrTabAddrOffset = 888;
	static const int assocTabAddrOffset = 890;
	static const int commsTabAddrOffset = 892;
    static const int commsSeg0AddrOffset = 894;
    static const int commsSeg1AddrOffset = 894;
	static const int eibObjAddrOffset = 898;
	static const int eibObjCountOffset = 900;
	static const int serviceControlOffset = 902;
	static const int serialOffset = 906;
	static const int orderOffset = 912;
	static const int addrTabMcbOffset = 932;
	static const int assocTabMcbOffset = 940;
	static const int commsTabMcbOffset = 948;
    static const int eibObjMcbOffset = 956;
    static const int eibObjVerOffset = 970;
	static const int commsSeg0VerOffset = 975;

	virtual const int serialSize() const { return 6; };

	virtual byte* loadState() const { return &userEepromData[loadStateOffset]; }
	virtual byte& appType() const { return userEepromData[appTypeOffset]; }
	virtual word& commsTabAddr() const { return *(word*)&userEepromData[commsTabAddrOffset]; }
	virtual word& commsSeg0Addr() const { return *(word*)&userEepromData[commsSeg0AddrOffset]; }
	virtual word& commsSeg1Addr() const { return *(word*)&userEepromData[commsSeg1AddrOffset]; }
	virtual word& eibObjAddr() const { return *(word*)&userEepromData[eibObjAddrOffset]; }
	virtual byte& eibObjCount() const { return userEepromData[eibObjCountOffset]; }
	virtual word& addrTabAddr() const { return *(word*)&userEepromData[addrTabAddrOffset]; }
	virtual word& assocTabAddr() const { return *(word*)&userEepromData[assocTabAddrOffset]; }
	virtual byte* serial() const { return &userEepromData[serialOffset]; }
	virtual byte* order() const { return &userEepromData[orderOffset]; }

	//virtual byte& peiType() const { return userEepromData[peiTypeOffset]; }

protected:
	UserEepromBCU2(BcuBase* bcu, unsigned int start, unsigned int size, unsigned int flashSize) : UserEepromBCU1(bcu, start, size, flashSize) {};
};

#endif
